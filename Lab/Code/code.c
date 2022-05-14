#include "common.h"
#include "module.h"

// global variables

static InterCodes curr;
static FILE *f;

// output

#define gen_code(format, ...) fprintf(f, format "\n", ##__VA_ARGS__)
#define gen_code_indent(format, ...) fprintf(f, "  " format "\n", ##__VA_ARGS__)

// reg

static struct RegisterDescriptor reg_infos[32];

// func descriptor

static struct FunctionDescriptor func_infos[32];
static size_t curr_func_info_index;

static size_t get_func_info_index(size_t func_no) {
  for (size_t i = 0; i < curr_func_info_index; ++i) {
    if (func_infos[i].func_no == func_no) {
      return i;
    }
  }
  return -1;
}

static struct FunctionDescriptor *get_curr_func() {
  if (curr_func_info_index == 0) {
    assert(0);
  }
  return &func_infos[curr_func_info_index - 1];
}

// var descriptor

static struct VariableDescriptor var_infos[1024];
static size_t curr_var_info_index;

static size_t get_var_info_index(size_t var_no) {
  for (size_t i = 0; i < curr_var_info_index; ++i) {
    if (var_infos[i].var_no == var_no) {
      return i;
    }
  }
  return -1;
}

// helper functions

static void reset() {
  curr_func_info_index = 0;
  curr_var_info_index = 0;

  memset(func_infos, 0, sizeof(func_infos));
  memset(var_infos, 0, sizeof(var_infos));
  memset(reg_infos, 0, sizeof(reg_infos));
}

static int is_var_in_reg(size_t var_no) {
  for (int i = 16; i < 24; ++i) {  // $s0 ~ $s7
    if (reg_infos[i].state != REG_NONE && reg_infos[i].var_no == var_no) {
      return i;
    }
  }
  return -1;  // not in reg
}

static int alloc_reg() {
  for (int i = 16; i < 24; ++i) {
    if (reg_infos[i].state == REG_NONE) {
      return i;
    }
  }

  int l = 16, r = 24, victim;
  while (1) {
    victim = rand() % (r - l) + l;
    assert(l <= victim && victim < r);
    if (reg_infos[victim].state == REG_FREE) {
      break;
    }
  }

  size_t var_info_index = get_var_info_index(reg_infos[victim].var_no);
  assert(var_info_index != -1);

  struct VariableDescriptor var_info = var_infos[var_info_index];
  assert(var_info.type == REG && var_info.pos.reg_id == victim);

  size_t func_info_index = get_func_info_index(var_info.func_no);
  assert(func_info_index != -1);

  if (var_info.pos.offset_fp == 0) {                  // first spill
    func_infos[func_info_index].curr_offset_fp += 4;  // assume 4 bytes
    var_infos[var_info_index].pos.offset_fp =
        func_infos[func_info_index].curr_offset_fp;

    gen_code_indent("subu $sp, $sp, 4");
  }

  gen_code_indent("sw %s, -%d($fp)", reg_name[var_info.pos.reg_id],
                  var_infos[var_info_index].pos.offset_fp);
  var_infos[var_info_index].type = STACK;
  var_infos[var_info_index].pos.reg_id = 0;  // reset

  reg_infos[victim].var_no = 0;  // reset
  return victim;
}

static int get_tmp_reg() {
  int reg_id = alloc_reg();
  reg_infos[reg_id].state = REG_IN_USE;  // now in use
  return reg_id;
}

static int ensure_var_in_reg(size_t var_no) {
  int reg_id = is_var_in_reg(var_no);
  if (reg_id != -1) {
    return reg_id;
  }

  reg_id = alloc_reg();
  reg_infos[reg_id].state = REG_IN_USE;  // now in use
  reg_infos[reg_id].var_no = var_no;

  size_t var_info_index = get_var_info_index(var_no);
  if (var_info_index != -1) {
    var_infos[var_info_index].type = REG;
    var_infos[var_info_index].pos.reg_id = reg_id;

    // load from stack
    gen_code_indent("lw %s, -%d($fp)", reg_name[reg_id],
                    var_infos[var_info_index].pos.offset_fp);
  } else {  // first occur
    var_infos[curr_var_info_index].type = REG;
    var_infos[curr_var_info_index].pos.reg_id = reg_id;

    var_infos[curr_var_info_index].var_no = var_no;
    var_infos[curr_var_info_index].var_name = ir->get_place(var_no);
    var_infos[curr_var_info_index].func_no = get_curr_func()->func_no;

    curr_var_info_index++;
  }

  return reg_id;
}

// generate functions

static void generate_assign(struct InterCode code) {
  Operand right = code.u.assign.right;
  Operand left = code.u.assign.left;

  // a = 1
  if (left->kind == OP_VARIABLE && right->kind == OP_CONSTANT) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    gen_code_indent("li %s, %u", reg_name[left_reg_id], right->u.value);
    reg_infos[left_reg_id].state = REG_FREE;
    return;
  }

  // a = b
  if (left->kind == OP_VARIABLE && right->kind == OP_VARIABLE) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    gen_code_indent("move %s, %s", reg_name[left_reg_id],
                    reg_name[right_reg_id]);
    reg_infos[left_reg_id].state = REG_FREE;
    reg_infos[right_reg_id].state = REG_FREE;
    return;
  }

  // x = arr[1]
  if (left->kind == OP_VARIABLE && right->kind == OP_ADDRESS_DEREF) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    gen_code_indent("lw %s, 0(%s)", reg_name[left_reg_id],
                    reg_name[right_reg_id]);
    reg_infos[left_reg_id].state = REG_FREE;
    reg_infos[right_reg_id].state = REG_FREE;
    return;
  }

  // arr[1] = x
  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_VARIABLE) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    gen_code_indent("sw %s, 0(%s)", reg_name[right_reg_id],
                    reg_name[left_reg_id]);
    reg_infos[left_reg_id].state = REG_FREE;
    reg_infos[right_reg_id].state = REG_FREE;
    return;
  }

  // arr[1] = 1
  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_CONSTANT) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int tmp_reg_id = get_tmp_reg();
    gen_code_indent("li %s, %u", reg_name[tmp_reg_id], right->u.value);
    gen_code_indent("sw %s, 0(%s)", reg_name[tmp_reg_id],
                    reg_name[left_reg_id]);
    reg_infos[left_reg_id].state = REG_FREE;
    reg_infos[tmp_reg_id].state = REG_NONE;  // note
    return;
  }

  assert(0);
  return;
}

static void generate_binop(struct InterCode code) {
  Operand result = code.u.binop.result;
  Operand op1 = code.u.binop.op1;
  Operand op2 = code.u.binop.op2;

  // a = 1 + 1
  if (op1->kind == OP_CONSTANT && op2->kind == OP_CONSTANT) {
    if (result->kind == OP_VARIABLE) {
      int result_reg_id = ensure_var_in_reg(result->u.placeno);
      unsigned res;
      switch (code.kind) {
        case IR_ADD:
          res = op1->u.value + op2->u.value;
          break;
        case IR_SUB:
          res = op1->u.value - op2->u.value;
          break;
        case IR_MUL:
          res = op1->u.value * op2->u.value;
          break;
        case IR_DIV:
          res = op1->u.value / op2->u.value;
          break;
        default:
          assert(0);
      }
      gen_code_indent("li %s, %u", reg_name[result_reg_id], res);
      reg_infos[result_reg_id].state = REG_FREE;
      return;
    }
  }

  // array
  if (result->kind == OP_ADDRESS &&
      (op1->kind == OP_ADDRESS_ORI || op1->kind == OP_ADDRESS)) {
    assert(code.kind == IR_ADD && op2->kind == OP_VARIABLE);  // assumed
    if (op2->kind == OP_VARIABLE) {
      int result_reg_id = ensure_var_in_reg(result->u.placeno);
      int op2_reg_id = ensure_var_in_reg(op2->u.placeno);

      size_t addr_no = get_var_info_index(op1->u.placeno);
      assert(addr_no != -1 && var_infos[addr_no].type == STACK);

      gen_code_indent("addi %s, $fp, -%u", reg_name[result_reg_id],
                      var_infos[addr_no].pos.offset_fp);
      gen_code_indent("add %s, %s, %s", reg_name[result_reg_id],
                      reg_name[result_reg_id], reg_name[op2_reg_id]);

      reg_infos[result_reg_id].state = REG_FREE;
      reg_infos[op2_reg_id].state = REG_FREE;
      return;
    }
  }

  // a = b + 1 or a = b + c
  if (result->kind == OP_VARIABLE && op1->kind == OP_VARIABLE) {
    if (op2->kind == OP_CONSTANT) {
      if (code.kind == IR_ADD || code.kind == IR_SUB) {
        int result_reg_id = ensure_var_in_reg(result->u.placeno);
        int op1_reg_id = ensure_var_in_reg(op1->u.placeno);
        if (code.kind == IR_ADD) {
          gen_code_indent("addi %s, %s, %u", reg_name[result_reg_id],
                          reg_name[op1_reg_id], op2->u.value);
        } else {
          gen_code_indent("addi %s, %s, -%u", reg_name[result_reg_id],
                          reg_name[op1_reg_id], op2->u.value);
        }
        reg_infos[result_reg_id].state = REG_FREE;
        reg_infos[op1_reg_id].state = REG_FREE;
      } else if (code.kind == IR_MUL || code.kind == IR_DIV) {
        int result_reg_id = ensure_var_in_reg(result->u.placeno);
        int op1_reg_id = ensure_var_in_reg(op1->u.placeno);
        int op2_reg_id = get_tmp_reg();
        gen_code_indent("li %s, %u", reg_name[op2_reg_id], op2->u.value);
        if (code.kind == IR_MUL) {
          gen_code_indent("mul %s, %s, %s", reg_name[result_reg_id],
                          reg_name[op1_reg_id], reg_name[op2_reg_id]);
        } else {
          gen_code_indent("div %s, %s", reg_name[op1_reg_id],
                          reg_name[op2_reg_id]);
          gen_code_indent("mflo %s", reg_name[result_reg_id]);
        }
        reg_infos[result_reg_id].state = REG_FREE;
        reg_infos[op1_reg_id].state = REG_FREE;
        reg_infos[op2_reg_id].state = REG_NONE;  // note
      }
      return;
    }

    if (op2->kind == OP_VARIABLE) {
      int result_reg_id = ensure_var_in_reg(result->u.placeno);
      int op1_reg_id = ensure_var_in_reg(op1->u.placeno);
      int op2_reg_id = ensure_var_in_reg(op2->u.placeno);

      switch (code.kind) {
        case IR_ADD:
          gen_code_indent("add %s, %s, %s", reg_name[result_reg_id],
                          reg_name[op1_reg_id], reg_name[op2_reg_id]);
          break;
        case IR_SUB:
          gen_code_indent("sub %s, %s, %s", reg_name[result_reg_id],
                          reg_name[op1_reg_id], reg_name[op2_reg_id]);
          break;
        case IR_MUL:
          gen_code_indent("mul %s, %s, %s", reg_name[result_reg_id],
                          reg_name[op1_reg_id], reg_name[op2_reg_id]);
          break;
        case IR_DIV:
          gen_code_indent("div %s, %s", reg_name[op1_reg_id],
                          reg_name[op2_reg_id]);
          gen_code_indent("mflo %s", reg_name[result_reg_id]);
          break;
        default:
          assert(0);
      }

      reg_infos[result_reg_id].state = REG_FREE;
      reg_infos[op1_reg_id].state = REG_FREE;
      reg_infos[op2_reg_id].state = REG_FREE;
      return;
    }
  }

  assert(0);
  return;
}

static void generate_relop(struct InterCode code) {}

static void generate_callee_prologue() {
  gen_code_indent("subu $sp, $sp, 40");
  gen_code_indent("sw $ra, 36($sp)");
  gen_code_indent("sw $fp, 32($sp)");
  gen_code_indent("addi $fp, $sp, 40");
  gen_code_indent("sw $s7, 28($sp)");
  gen_code_indent("sw $s6, 24($sp)");
  gen_code_indent("sw $s5, 20($sp)");
  gen_code_indent("sw $s4, 16($sp)");
  gen_code_indent("sw $s3, 12($sp)");
  gen_code_indent("sw $s2, 8($sp)");
  gen_code_indent("sw $s1, 4($sp)");
  gen_code_indent("sw $s0, 0($sp)");
  fflush(f);
}

static void generate_callee_epilogue() {
  gen_code_indent("lw $s0, -40($fp)");
  gen_code_indent("lw $s1, -36($fp)");
  gen_code_indent("lw $s2, -32($fp)");
  gen_code_indent("lw $s3, -28($fp)");
  gen_code_indent("lw $s4, -24($fp)");
  gen_code_indent("lw $s5, -20($fp)");
  gen_code_indent("lw $s6, -16($fp)");
  gen_code_indent("lw $s7, -12($fp)");
  gen_code_indent("move $sp, $fp");
  gen_code_indent("lw $fp, -8($sp)");
  gen_code_indent("lw $ra, -4($sp)");
  gen_code_indent("jr $ra");
  fflush(f);
}

static void generate_func(struct InterCode code) {
  size_t func_no = code.u.single.op->u.placeno;
  const char *func_name = ir->get_place(func_no);
  gen_code("%s:", func_name);

  func_infos[curr_func_info_index].func_no = func_no;
  func_infos[curr_func_info_index].func_name = func_name;
  func_infos[curr_func_info_index].curr_offset_fp =
      40;  // $ra, old $fp, $s0 ~ $s7
  curr_func_info_index++;

  generate_callee_prologue();
}

static void generate_return(struct InterCode code) {
  Operand op = code.u.single.op;
  if (op->kind == OP_VARIABLE) {
    int reg_id = ensure_var_in_reg(op->u.placeno);
    gen_code_indent("move $v0, %s", reg_name[reg_id]);
    reg_infos[reg_id].state = REG_FREE;
    goto END;
  }

  if (op->kind == OP_CONSTANT) {
    gen_code_indent("li $v0, %u", op->u.value);
    goto END;
  }

  assert(0);

END:
  generate_callee_epilogue();
}

static void generate_read(struct InterCode code) {
  Operand op = code.u.single.op;
  if (op->kind == OP_VARIABLE) {
    int reg_id = ensure_var_in_reg(op->u.placeno);
    gen_code_indent("jal read");
    gen_code_indent("move %s, $v0", reg_name[reg_id]);
    reg_infos[reg_id].state = REG_FREE;
    return;
  }

  assert(0);
}

static void generate_write(struct InterCode code) {
  Operand op = code.u.single.op;
  if (op->kind == OP_VARIABLE) {
    int reg_id = ensure_var_in_reg(op->u.placeno);
    gen_code_indent("move $a0, %s", reg_name[reg_id]);
    gen_code_indent("jal write");
    reg_infos[reg_id].state = REG_FREE;
    return;
  }

  if (op->kind == OP_CONSTANT) {
    gen_code_indent("li $a0, %u", op->u.value);
    gen_code_indent("jal write");
    return;
  }

  if (op->kind == OP_ADDRESS_DEREF) {
    int reg_id = ensure_var_in_reg(op->u.placeno);
    gen_code_indent("lw $a0, 0(%s)", reg_name[reg_id]);
    gen_code_indent("jal write");
    reg_infos[reg_id].state = REG_FREE;
    return;
  }

  assert(0);
}

static void generate_dec(struct InterCode code) {
  Operand var = code.u.dec.var;
  Operand size = code.u.dec.size;

  var_infos[curr_var_info_index].type = STACK;
  var_infos[curr_var_info_index].var_no = var->u.placeno;
  var_infos[curr_var_info_index].var_name = ir->get_place(var->u.placeno);
  var_infos[curr_var_info_index].func_no = get_curr_func()->func_no;

  gen_code_indent("subu $sp, $sp, %u", size->u.value);
  get_curr_func()->curr_offset_fp += size->u.value;
  var_infos[curr_var_info_index].pos.offset_fp =
      get_curr_func()->curr_offset_fp;

  curr_var_info_index++;
}

static void code_generate_step() {
  struct InterCode code = curr->code;
  switch (code.kind) {
    case IR_LABEL:
      gen_code("%s:", ir->get_place(code.u.single.op->u.placeno));
      curr = curr->next;
      break;
    case IR_GOTO:
      gen_code_indent("j %s", ir->get_place(code.u.single.op->u.placeno));
      curr = curr->next;
      break;

    case IR_FUNCTION:
      generate_func(code);
      curr = curr->next;
      break;
    case IR_RETURN:
      generate_return(code);
      curr = curr->next;
      break;
    case IR_ASSIGN_CALL:
      assert(0);
      break;
    case IR_ARG:
      assert(0);
      break;
    case IR_PARAM:
      assert(0);
      break;

    case IR_READ:
      generate_read(code);
      curr = curr->next;
      break;
    case IR_WRITE:
      generate_write(code);
      curr = curr->next;
      break;

    case IR_ASSIGN:
      generate_assign(code);
      curr = curr->next;
      break;

    case IR_ADD:
    case IR_SUB:
    case IR_MUL:
    case IR_DIV:
      generate_binop(code);
      curr = curr->next;
      break;

    case IR_RELOP:
      generate_relop(code);
      curr = curr->next;
      break;

    case IR_DEC:
      assert(code.u.dec.var->kind == OP_ADDRESS_ORI);
      assert(code.u.dec.size->kind == OP_CONSTANT);
      generate_dec(code);
      curr = curr->next;
      break;

    default:
      assert(0);
      break;
  }

  fflush(f);
}

static void generate_header() {
  gen_code(".data");
  gen_code("_prompt: .asciiz \"Enter an integer:\"");
  gen_code("_ret: .asciiz \"\\n\"");
  gen_code(".globl main");
  gen_code(".text");

  gen_code("read:");
  gen_code_indent("li $v0, 4");
  gen_code_indent("la $a0, _prompt");
  gen_code_indent("syscall");
  gen_code_indent("li $v0, 5");
  gen_code_indent("syscall");
  gen_code_indent("jr $ra");

  gen_code("write:");
  gen_code_indent("li $v0, 1");
  gen_code_indent("syscall");
  gen_code_indent("li $v0, 4");
  gen_code_indent("la $a0, _ret");
  gen_code_indent("syscall");
  gen_code_indent("move $v0, $0");
  gen_code_indent("jr $ra");
}

// interface

static void code_generate(FILE *file) {
  srand(time(NULL));  // for rand victim

  f = file;
  curr = ir->get_ir_st();
  reset();

  generate_header();
  while (curr) {
    if (curr->lineno == -1) {  // ir end
      break;
    }
    code_generate_step();
  }
}

MODULE_DEF(code) = {
    .generate = code_generate,
};
