#include "common.h"
#include "module.h"

static InterCodes curr;
static FILE *f;

static const char *reg_name[32] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0",   "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0",   "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8",   "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
};

// -1 -> free
// -2 -> imm
// other -> var no
static int reg_state[32] = {[0 ... 31] = -1};  // gcc only

struct FunctionStack {
  const char *func_name;
  int curr_offset_fp;
};

static struct FunctionStack func_stack[16];
static int curr_func_depth = -1;

static void push_func(const char *func_name) {
  ++curr_func_depth;
  func_stack[curr_func_depth].func_name = func_name;
  func_stack[curr_func_depth].curr_offset_fp = 40;  // $ra, old $fp, $s0 ~ $s7
}

static void pop_func() { --curr_func_depth; }

static struct FunctionStack *get_curr_func() {
  assert(curr_func_depth >= 0);
  return &func_stack[curr_func_depth];
}

struct Position {  // not union, keep offset info
  int offset_fp;
  int reg_id;
};

struct VariableDescriptor {
  int var_no;
  int func_depth;  // for offset
  struct Position pos;
  enum { STACK, REG } type;
};

static struct VariableDescriptor var_infos[128];
static size_t curr_var_info_index;

static size_t get_var_info_index(int var_no) {
  for (size_t i = 0; i < curr_var_info_index; ++i) {
    if (var_infos[i].var_no == var_no) {
      return i;
    }
  }
  return -1;
}

static int is_var_in_reg(int var_no) {
  for (int i = 16; i < 24; ++i) {  // $s0 ~ $s7
    if (reg_state[i] == var_no) {
      return i;
    }
  }
  return -1;  // not in reg
}

static int alloc_reg() {
  for (int i = 16; i < 24; ++i) {
    if (reg_state[i] == -1) {
      return i;
    }
  }

  int l = 16, r = 24;
  int victim = rand() % (r - l) + l;
  assert(l <= victim && victim < r);

  int var_no = reg_state[victim];
  size_t var_info_index = get_var_info_index(var_no);
  assert(var_info_index != -1);
  struct VariableDescriptor var_info = var_infos[var_info_index];
  assert(var_info.type == REG && var_info.pos.reg_id == victim);

  var_infos[var_info_index].type = STACK;
  int depth = var_infos[var_info_index].func_depth;
  assert(depth == curr_func_depth);  // note

  if (var_info.pos.offset_fp == 0) {       // first spill
    get_curr_func()->curr_offset_fp += 4;  // assume 4 bytes
    var_infos[var_info_index].pos.offset_fp = get_curr_func()->curr_offset_fp;

    fprintf(f, "subu $sp, $sp, 4\n");
  }

  fprintf(f, "sw %s, %d($fp)\n", reg_name[var_infos[var_info_index].pos.reg_id],
          var_infos[var_info_index].pos.offset_fp);
  var_infos[var_info_index].pos.reg_id = 0;  // reset

  reg_state[victim] = -1;  // now free
  return victim;
}

static int ensure_var_in_reg(int var_no) {
  int reg_id = is_var_in_reg(var_no);
  if (reg_id != -1) {
    return reg_id;
  }

  reg_id = alloc_reg();
  reg_state[reg_id] = var_no;

  size_t var_info_index = get_var_info_index(var_no);
  if (var_info_index != -1) {
    var_infos[var_info_index].type = REG;
    var_infos[var_info_index].pos.reg_id = reg_id;
  } else {
    var_infos[curr_var_info_index].var_no = var_no;
    var_infos[curr_var_info_index].type = REG;
    var_infos[curr_var_info_index].pos.reg_id = reg_id;

    var_infos[curr_var_info_index].func_depth = curr_func_depth;
    curr_var_info_index++;
  }

  return reg_id;
}

static void generate_assign(struct InterCode code) {
  assert(code.kind == IR_ASSIGN);
  Operand right = code.u.assign.right;
  Operand left = code.u.assign.left;

  if (left->kind == OP_VARIABLE && right->kind == OP_CONSTANT) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    fprintf(f, "li %s, %u\n", reg_name[left_reg_id], right->u.value);
    return;
  }

  if (left->kind == OP_VARIABLE && right->kind == OP_VARIABLE) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    fprintf(f, "move %s, %s\n", reg_name[left_reg_id], reg_name[right_reg_id]);
    return;
  }

  if (left->kind == OP_VARIABLE && right->kind == OP_ADDRESS_DEREF) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    fprintf(f, "lw %s, 0(%s)\n", reg_name[left_reg_id], reg_name[right_reg_id]);
    return;
  }

  if (left->kind == OP_ADDRESS_DEREF && right->kind == OP_VARIABLE) {
    int left_reg_id = ensure_var_in_reg(left->u.placeno);
    int right_reg_id = ensure_var_in_reg(right->u.placeno);
    fprintf(f, "sw %s, 0(%s)\n", reg_name[right_reg_id], reg_name[left_reg_id]);
    return;
  }

  assert(0);
  return;
}

static void generate_binop(struct InterCode code) {
  Operand result = code.u.binop.result;
  Operand op1 = code.u.binop.op1;
  Operand op2 = code.u.binop.op2;

  if (result->kind == OP_ADDRESS && (op1->kind == OP_ADDRESS_DEREF || op1->kind == OP_ADDRESS)) {
    if (op2->kind == OP_CONSTANT) {

    }

    if (op2->kind == OP_VARIABLE) {
      
    }
  }

  assert(0);
  return;
}

static void generate_relop(struct InterCode code) {

}

static void code_generate_step() {
  struct InterCode code = curr->code;
  switch (code.kind) {
    case IR_LABEL:
      fprintf(f, "%s:\n", ir->get_place(code.u.single.op->u.placeno));
      curr = curr->next;
      break;
    case IR_FUNCTION:
      const char *func_name = ir->get_place(code.u.single.op->u.placeno);
      fprintf(f, "%s:\n", func_name);
      push_func(func_name);
      curr = curr->next;
      break;

    case IR_GOTO:
      fprintf(f, "j %s\n", ir->get_place(code.u.single.op->u.placeno));
      curr = curr->next;
      break;

    case IR_ASSIGN_CALL:
      assert(0);
      break;
    case IR_RETURN:
      pop_func();
      assert(0);
      break;
    case IR_ARG:
      assert(0);
      break;
    case IR_PARAM:
      assert(0);
      break;

    case IR_READ:
      break;
    case IR_WRITE:
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

      break;
    default:
      assert(0);
      break;
  }

  fflush(f);
}

static void code_generate(FILE *file) {
  srand(time(NULL));  // for rand victim

  f = file;
  curr = ir->get_ir_st();

  while (curr) {
    if (curr->lineno == -1) {  // ir end
      break;
    }
    code_generate_step(f);
  }
}

MODULE_DEF(code) = {
    .generate = code_generate,
};
