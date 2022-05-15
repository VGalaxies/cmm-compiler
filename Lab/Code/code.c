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

static int alloc_reg() {
  for (int i = 16; i < 24; ++i) {
    if (reg_infos[i].state == REG_FREE) {
      return i;
    }
  }

  assert(0);
}

static void free_reg(int reg_id) {
  if (reg_id == -1) {
    return;
  }

  assert(reg_infos[reg_id].state == REG_IN_USE);
  reg_infos[reg_id].state = REG_FREE;

  if (reg_infos[reg_id].var_no != -1) {
    size_t var_info_index = get_var_info_index(reg_infos[reg_id].var_no);
    assert(var_info_index != -1 && var_infos[var_info_index].type == REG &&
           var_infos[var_info_index].pos.reg_id == reg_id);

    // store into stack
    if (var_infos[var_info_index].pos.offset_fp <= 0) {  // param
      gen_code_indent("sw %s, %d($fp)", reg_name[reg_id],
                      -var_infos[var_info_index].pos.offset_fp);
    } else if (var_infos[var_info_index].pos.offset_fp >= 8 + 4) {
      gen_code_indent("sw %s, -%d($fp)", reg_name[reg_id],
                      var_infos[var_info_index].pos.offset_fp);
    } else {
      assert(0);
    }

    var_infos[var_info_index].type = STACK;
    var_infos[var_info_index].pos.reg_id = 0;  // reset
  }

  reg_infos[reg_id].var_no = -1;  // reset
}

static int get_tmp_reg() {
  int reg_id = alloc_reg();
  reg_infos[reg_id].state = REG_IN_USE;  // now in use
  reg_infos[reg_id].var_no = -1;         // no var
  return reg_id;
}

static int get_var_reg(size_t var_no) {
  int reg_id = alloc_reg();
  reg_infos[reg_id].state = REG_IN_USE;  // now in use
  reg_infos[reg_id].var_no = var_no;

  size_t var_info_index = get_var_info_index(var_no);
  if (var_info_index != -1) {
    assert(var_infos[var_info_index].type == STACK &&
           var_infos[var_info_index].pos.reg_id == 0);

    // load from stack
    if (var_infos[var_info_index].pos.offset_fp <= 0) {
      gen_code_indent("lw %s, %d($fp)", reg_name[reg_id],
                      -var_infos[var_info_index].pos.offset_fp);
    } else if (var_infos[var_info_index].pos.offset_fp >= 8 + 4) {
      gen_code_indent("lw %s, -%d($fp)", reg_name[reg_id],
                      var_infos[var_info_index].pos.offset_fp);
    } else {
      assert(0);
    }

    var_infos[var_info_index].type = REG;
    var_infos[var_info_index].pos.reg_id = reg_id;
  } else {  // first occur
    var_infos[curr_var_info_index].type = REG;
    var_infos[curr_var_info_index].pos.reg_id = reg_id;

    var_infos[curr_var_info_index].var_no = var_no;
    var_infos[curr_var_info_index].var_name = ir->get_place(var_no);
    var_infos[curr_var_info_index].func_no = get_curr_func()->func_no;

    size_t func_info_index = get_func_info_index(get_curr_func()->func_no);
    assert(func_info_index != -1);

    // set offset
    func_infos[func_info_index].curr_offset_fp += 4;  // assume 4 bytes
    var_infos[curr_var_info_index].pos.offset_fp =
        func_infos[func_info_index].curr_offset_fp;
    gen_code_indent("subu $sp, $sp, 4");

    curr_var_info_index++;
  }

  return reg_id;
}

// generate functions

static void generate_assign(struct InterCode code) {
  Operand left = code.u.assign.left;
  Operand right = code.u.assign.right;

  // a = 1
  if (left->kind == OP_VARIABLE && right->kind == OP_CONSTANT) {
    int left_reg_id = get_var_reg(left->u.placeno);
    gen_code_indent("li %s, %u", reg_name[left_reg_id], right->u.value);
    free_reg(left_reg_id);
    return;
  }

  // a = b
  if (left->kind == OP_VARIABLE && right->kind == OP_VARIABLE) {
    int left_reg_id = get_var_reg(left->u.placeno);
    int right_reg_id = get_var_reg(right->u.placeno);
    gen_code_indent("move %s, %s", reg_name[left_reg_id],
                    reg_name[right_reg_id]);
    free_reg(left_reg_id);
    free_reg(right_reg_id);
    return;
  }

  // x = arr[1]
  if (left->kind == OP_VARIABLE && right->kind == OP_ADDRESS_DEREF) {
    int left_reg_id = get_var_reg(left->u.placeno);
    int right_reg_id = get_var_reg(right->u.placeno);
    gen_code_indent("lw %s, 0(%s)", reg_name[left_reg_id],
                    reg_name[right_reg_id]);
    free_reg(left_reg_id);
    free_reg(right_reg_id);
    return;
  }

  // arr[1] = x
  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_VARIABLE) {
    int left_reg_id = get_var_reg(left->u.placeno);
    int right_reg_id = get_var_reg(right->u.placeno);
    gen_code_indent("sw %s, 0(%s)", reg_name[right_reg_id],
                    reg_name[left_reg_id]);
    free_reg(left_reg_id);
    free_reg(right_reg_id);
    return;
  }

  // arr[1] = 1
  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_CONSTANT) {
    int left_reg_id = get_var_reg(left->u.placeno);
    int tmp_reg_id = get_tmp_reg();
    gen_code_indent("li %s, %u", reg_name[tmp_reg_id], right->u.value);
    gen_code_indent("sw %s, 0(%s)", reg_name[tmp_reg_id],
                    reg_name[left_reg_id]);
    free_reg(left_reg_id);
    free_reg(tmp_reg_id);
    return;
  }

  // arr[1] = arr[2]
  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_ADDRESS_DEREF) {
    int left_reg_id = get_var_reg(left->u.placeno);
    int right_reg_id = get_var_reg(right->u.placeno);
    int tmp_reg_id = get_tmp_reg();
    gen_code_indent("lw %s, 0(%s)", reg_name[tmp_reg_id],
                    reg_name[right_reg_id]);
    gen_code_indent("sw %s, 0(%s)", reg_name[tmp_reg_id],
                    reg_name[left_reg_id]);
    free_reg(left_reg_id);
    free_reg(right_reg_id);
    free_reg(tmp_reg_id);
  }

  assert(0);
  return;
}

static void generate_binop(struct InterCode code) {
  Operand result = code.u.binop.result;
  Operand op1 = code.u.binop.op1;
  Operand op2 = code.u.binop.op2;

  // special handling for array addresses
  if (result->kind == OP_ADDRESS) {
    assert(code.kind == IR_ADD && op1->kind == OP_ADDRESS_ORI &&
           op2->kind == OP_VARIABLE);  // assumed

    int result_reg_id = get_var_reg(result->u.placeno);
    int op2_reg_id = get_var_reg(op2->u.placeno);

    size_t addr_no = get_var_info_index(
        op1->u.placeno);  // first address of the array always in stack
    assert(addr_no != -1 && var_infos[addr_no].type == STACK &&
           var_infos[addr_no].pos.reg_id == 0 &&
           var_infos[addr_no].pos.offset_fp >= 8 + 4);  // not param

    gen_code_indent("addi %s, $fp, -%d", reg_name[result_reg_id],
                    var_infos[addr_no].pos.offset_fp);
    gen_code_indent("add %s, %s, %s", reg_name[result_reg_id],
                    reg_name[result_reg_id], reg_name[op2_reg_id]);

    free_reg(result_reg_id);
    free_reg(op2_reg_id);

    return;
  }

  // lval -> var / deref
  // rval -> [var / deref / const] [op] [var / deref / const]

  int op1_reg_id = -1, op2_reg_id = -1;
  int op1_tmp_reg_id = -1, op2_tmp_reg_id = -1;

  switch (op1->kind) {
    case OP_VARIABLE:
      op1_tmp_reg_id = get_var_reg(op1->u.placeno);  // note tmp
      break;
    case OP_CONSTANT:
      op1_tmp_reg_id = get_tmp_reg();
      gen_code_indent("li %s, %u", reg_name[op1_tmp_reg_id], op1->u.value);
      break;
    case OP_ADDRESS_DEREF:
      op1_reg_id = get_var_reg(op1->u.placeno);
      op1_tmp_reg_id = get_tmp_reg();
      gen_code_indent("lw %s, 0(%s)", reg_name[op1_tmp_reg_id],
                      reg_name[op1_reg_id]);
      break;
    default:
      assert(0);
  }

  switch (op2->kind) {
    case OP_VARIABLE:
      op2_tmp_reg_id = get_var_reg(op2->u.placeno);  // note tmp
      break;
    case OP_CONSTANT:
      op2_tmp_reg_id = get_tmp_reg();
      gen_code_indent("li %s, %u", reg_name[op2_tmp_reg_id], op2->u.value);
      break;
    case OP_ADDRESS_DEREF:
      op2_reg_id = get_var_reg(op2->u.placeno);
      op2_tmp_reg_id = get_tmp_reg();
      gen_code_indent("lw %s, 0(%s)", reg_name[op2_tmp_reg_id],
                      reg_name[op2_reg_id]);
      break;
    default:
      assert(0);
  }

  int result_tmp_reg_id = get_tmp_reg();
  switch (code.kind) {
    case IR_ADD:
      gen_code_indent("add %s, %s, %s", reg_name[result_tmp_reg_id],
                      reg_name[op1_tmp_reg_id], reg_name[op2_tmp_reg_id]);
      break;
    case IR_SUB:
      gen_code_indent("sub %s, %s, %s", reg_name[result_tmp_reg_id],
                      reg_name[op1_tmp_reg_id], reg_name[op2_tmp_reg_id]);
      break;
    case IR_MUL:
      gen_code_indent("mul %s, %s, %s", reg_name[result_tmp_reg_id],
                      reg_name[op1_tmp_reg_id], reg_name[op2_tmp_reg_id]);
      break;
    case IR_DIV:
      gen_code_indent("div %s, %s", reg_name[op1_tmp_reg_id],
                      reg_name[op2_tmp_reg_id]);
      gen_code_indent("mflo %s", reg_name[result_tmp_reg_id]);
      break;
    default:
      assert(0);
  }

  int result_reg_id = get_var_reg(result->u.placeno);
  switch (result->kind) {
    case OP_VARIABLE:
      gen_code_indent("move %s, %s", reg_name[result_reg_id],
                      reg_name[result_tmp_reg_id]);
      break;
    case OP_ADDRESS_DEREF:
      gen_code_indent("sw %s, 0(%s)", reg_name[result_tmp_reg_id],
                      reg_name[result_reg_id]);
      break;
    default:
      assert(0);
  }

  // reset reg state
  free_reg(op1_reg_id);
  free_reg(op1_tmp_reg_id);
  free_reg(op2_reg_id);
  free_reg(op2_tmp_reg_id);
  free_reg(result_reg_id);
  free_reg(result_tmp_reg_id);

  // TDDO -> assertion
  return;
}

static void generate_relop(struct InterCode code) {
  Operand x = code.u.relop.x;
  Operand y = code.u.relop.y;
  Operand z = code.u.relop.z;

  int x_reg_id = -1, y_reg_id = -1;
  int x_tmp_reg_id = -1, y_tmp_reg_id = -1;
  const char *label = ir->get_place(z->u.placeno);

  switch (x->kind) {
    case OP_VARIABLE:
      x_tmp_reg_id = get_var_reg(x->u.placeno);  // note tmp
      break;
    case OP_CONSTANT:
      x_tmp_reg_id = get_tmp_reg();
      gen_code_indent("li %s, %u", reg_name[x_tmp_reg_id], x->u.value);
      break;
    case OP_ADDRESS_DEREF:
      x_reg_id = get_var_reg(x->u.placeno);
      x_tmp_reg_id = get_tmp_reg();
      gen_code_indent("lw %s, 0(%s)", reg_name[x_tmp_reg_id],
                      reg_name[x_reg_id]);
      break;
    default:
      assert(0);
  }

  switch (y->kind) {
    case OP_VARIABLE:
      y_tmp_reg_id = get_var_reg(y->u.placeno);  // note tmp
      break;
    case OP_CONSTANT:
      y_tmp_reg_id = get_tmp_reg();
      gen_code_indent("li %s, %u", reg_name[y_tmp_reg_id], y->u.value);
      break;
    case OP_ADDRESS_DEREF:
      y_reg_id = get_var_reg(y->u.placeno);
      y_tmp_reg_id = get_tmp_reg();
      gen_code_indent("lw %s, 0(%s)", reg_name[y_tmp_reg_id],
                      reg_name[y_reg_id]);
      break;
    default:
      assert(0);
  }

  // reset reg state
  // before cond jump
  free_reg(x_reg_id);
  free_reg(x_tmp_reg_id);
  free_reg(y_reg_id);
  free_reg(y_tmp_reg_id);

  switch (code.u.relop.type) {
    case _LT:
      gen_code_indent("blt %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    case _LE:
      gen_code_indent("ble %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    case _EQ:
      gen_code_indent("beq %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    case _NE:
      gen_code_indent("bne %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    case _GT:
      gen_code_indent("bgt %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    case _GE:
      gen_code_indent("bge %s, %s, %s", reg_name[x_tmp_reg_id],
                      reg_name[y_tmp_reg_id], label);
      break;
    default:
      assert(0);
      break;
  }

  // TDDO -> assertion
  return;
}

static void generate_callee_prologue() {
  gen_code_indent("subu $sp, $sp, 8");
  gen_code_indent("sw $ra, 4($sp)");
  gen_code_indent("sw $fp, 0($sp)");
  gen_code_indent("addi $fp, $sp, 8");
  fflush(f);
}

static void generate_callee_epilogue() {
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
  func_infos[curr_func_info_index].curr_offset_fp = 8;  // $ra, old $fp
  curr_func_info_index++;

  generate_callee_prologue();
}

static void generate_return(struct InterCode code) {
  Operand op = code.u.single.op;
  if (op->kind == OP_VARIABLE) {
    int reg_id = get_var_reg(op->u.placeno);
    gen_code_indent("move $v0, %s", reg_name[reg_id]);
    free_reg(reg_id);
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
    int reg_id = get_var_reg(op->u.placeno);
    gen_code_indent("jal read");
    gen_code_indent("move %s, $v0", reg_name[reg_id]);
    free_reg(reg_id);
    return;
  }

  assert(0);
}

static void generate_write(struct InterCode code) {
  Operand op = code.u.single.op;
  if (op->kind == OP_VARIABLE) {
    int reg_id = get_var_reg(op->u.placeno);
    gen_code_indent("move $a0, %s", reg_name[reg_id]);
    gen_code_indent("jal write");
    free_reg(reg_id);
    return;
  }

  if (op->kind == OP_CONSTANT) {
    gen_code_indent("li $a0, %u", op->u.value);
    gen_code_indent("jal write");
    return;
  }

  if (op->kind == OP_ADDRESS_DEREF) {
    int reg_id = get_var_reg(op->u.placeno);
    gen_code_indent("lw $a0, 0(%s)", reg_name[reg_id]);
    gen_code_indent("jal write");
    free_reg(reg_id);
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
  var_infos[curr_var_info_index].pos.reg_id = 0;

  gen_code_indent("subu $sp, $sp, %u", size->u.value);
  get_curr_func()->curr_offset_fp += size->u.value;
  var_infos[curr_var_info_index].pos.offset_fp =
      get_curr_func()->curr_offset_fp;

  curr_var_info_index++;
}

static int generate_arg() {
  int count = 0;
  InterCodes ed = curr;
  while (curr->code.kind == IR_ARG) {
    curr = curr->next;
    count++;
  }
  assert(curr->code.kind == IR_ASSIGN_CALL);
  InterCodes st = curr;

  gen_code_indent("subu $sp, $sp, %d", count * 4);
  int offset = count * 4 - 4;
  while (ed != st) {
    switch (ed->code.u.single.op->kind) {
      case OP_CONSTANT: {
        int reg_id = get_tmp_reg();
        gen_code_indent("li %s, %u", reg_name[reg_id],
                        ed->code.u.single.op->u.value);
        gen_code_indent("sw %s, %d($sp)", reg_name[reg_id], offset);
        free_reg(reg_id);
        break;
      }
      case OP_VARIABLE: {
        int reg_id = get_var_reg(ed->code.u.single.op->u.placeno);
        gen_code_indent("sw %s, %d($sp)", reg_name[reg_id], offset);
        free_reg(reg_id);
        break;
      }
      case OP_ADDRESS_DEREF: {
        int reg_id = get_var_reg(ed->code.u.single.op->u.placeno);
        int reg_tmp_id = get_tmp_reg();
        gen_code_indent("lw %s, 0(%s)", reg_name[reg_tmp_id], reg_name[reg_id]);
        gen_code_indent("sw %s, %d($sp)", reg_name[reg_tmp_id], offset);
        free_reg(reg_id);
        free_reg(reg_tmp_id);
        break;
      }
      default:
        assert(0);
        break;
    }
    ed = ed->next;
    offset -= 4;
  }
  assert(offset == -4);

  return count;
}

static void generate_param() {
  int offset = 0;
  while (curr->code.kind == IR_PARAM) {
    assert(curr->code.u.single.op->kind == OP_VARIABLE);

    var_infos[curr_var_info_index].type = STACK;
    var_infos[curr_var_info_index].var_no = curr->code.u.single.op->u.placeno;
    var_infos[curr_var_info_index].var_name =
        ir->get_place(curr->code.u.single.op->u.placeno);
    var_infos[curr_var_info_index].func_no = get_curr_func()->func_no;
    var_infos[curr_var_info_index].pos.offset_fp = offset;
    var_infos[curr_var_info_index].pos.reg_id = 0;

    curr_var_info_index++;
    offset -= 4;

    curr = curr->next;
  }
}

static void generate_assign_call(struct InterCode code, int arg_count) {
  Operand left = code.u.assign.left;
  Operand right = code.u.assign.right;

  assert(right->kind == OP_FUNC);
  gen_code_indent("jal %s", ir->get_place(right->u.placeno));

  switch (left->kind) {
    case OP_VARIABLE: {
      int reg_id = get_var_reg(left->u.placeno);
      gen_code_indent("move %s, $v0", reg_name[reg_id]);
      free_reg(reg_id);
      break;
    }
    case OP_ADDRESS_DEREF: {
      int reg_id = get_var_reg(left->u.placeno);
      gen_code_indent("sw $v0, 0(%s)", reg_name[reg_id]);
      free_reg(reg_id);
    }
    default:
      assert(0);
  }

  if (arg_count != 0) {
    gen_code_indent("addi $sp, $sp, %d", arg_count * 4);
  }
}

static void code_generate_step() {
  struct InterCode code = curr->code;
  int arg_count = 0;

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

    case IR_ARG:
      arg_count = generate_arg();
      assert(curr->code.kind == IR_ASSIGN_CALL);
    case IR_ASSIGN_CALL:
      generate_assign_call(curr->code, arg_count);
      curr = curr->next;
      break;

    case IR_PARAM:
      generate_param();
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
