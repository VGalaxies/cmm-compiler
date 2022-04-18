#include "common.h"
#include "module.h"

/* place, var, label */

#define MAX_PLACE 1024
static size_t next_placeno = 0;
static const char *placemap[MAX_PLACE];

static size_t next_varno = 0;
static size_t next_labelno = 0;

static char *gen_var() {
  char *var = (char *)mm->log_malloc((64 * sizeof(char)));
  memset(var, 0, 64 * sizeof(char));
  sprintf(var, "var%zu", next_varno++);
  return var;
}

static char *gen_label() {
  char *label = (char *)mm->log_malloc((64 * sizeof(char)));
  memset(label, 0, 64 * sizeof(char));
  sprintf(label, "lable%zu", next_labelno++);
  return label;
}

static void store_place(const char *name, size_t placeno) {
  if (placeno >= MAX_PLACE) {
    assert(0); // TODO
  }
  placemap[placeno] = name;
}

/* ir */

static InterCodes ir_st;
static InterCodes ir_ed;

static void init_ir() {
  ir_st = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev->lineno = 0;

  ir_st->next = NULL;
  ir_ed = ir_st;
}

static void construct_single_ir(int kind, Operand op) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.single.op = op,
  };

  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));

  InterCodes tmp = ir_ed;
  ir_ed = ir_ed->next;
  ir_ed->prev = tmp;
  ir_ed->next = NULL;
}

static void construct_assign_ir(int kind, Operand left, Operand right) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.assign.right = right,
      .u.assign.left = left,
  };

  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));

  InterCodes tmp = ir_ed;
  ir_ed = ir_ed->next;
  ir_ed->prev = tmp;
  ir_ed->next = NULL;
}

static void construct_binop_ir(int kind, Operand result, Operand op1,
                               Operand op2) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.binop.op1 = op1,
      .u.binop.op2 = op2,
      .u.binop.result = result,
  };

  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));

  InterCodes tmp = ir_ed;
  ir_ed = ir_ed->next;
  ir_ed->prev = tmp;
  ir_ed->next = NULL;
}

static void construct_relop_ir(int kind, Operand x, Operand y, Operand z,
                               int relop_type) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.relop.x = x,
      .u.relop.y = y,
      .u.relop.z = z,
      .u.relop.type = relop_type,
  };

  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));

  InterCodes tmp = ir_ed;
  ir_ed = ir_ed->next;
  ir_ed->prev = tmp;
  ir_ed->next = NULL;
}

// static void construct_dec_ir(int kind, Operand var, Operand size) {}

static const char *interp_op(Operand op) {
  switch (op->kind) {
  case OP_VARIABLE:
  case OP_STRING:
    return placemap[op->u.placeno];
  case OP_CONSTANT: {
    char *res = (char *)mm->log_malloc(32 * sizeof(char));
    memset(res, 0, 32 * sizeof(char));
    sprintf(res, "#%d", op->u.value);
    return res;
  }
  case OP_REF:
    break;
  case OP_DEREF:
    break;
  default:
    break;
  }

  return NULL;
}

static const char *interp_relop(int type) {
  char *res = (char *)mm->log_malloc(4 * sizeof(char));
  memset(res, 0, 4 * sizeof(char));

  switch (type) {
  case _LT:
    sprintf(res, "<");
    break;
  case _LE:
    sprintf(res, "<=");
    break;
  case _EQ:
    sprintf(res, "==");
    break;
  case _NE:
    sprintf(res, "!=");
    break;
  case _GT:
    sprintf(res, ">");
    break;
  case _GE:
    sprintf(res, ">=");
    break;
  default:
    break;
  }

  return res;
}

static void ir_generate_step(FILE *f, struct InterCode ir) {
  switch (ir.kind) {
  case IR_ASSIGN:
    fprintf(f, "%s := %s\n", interp_op(ir.u.assign.left),
            interp_op(ir.u.assign.right));
    break;
  case IR_ASSIGN_CALL:
    fprintf(f, "%s := CALL %s\n", interp_op(ir.u.assign.left),
            interp_op(ir.u.assign.right));
    break;
  case IR_LABEL:
    fprintf(f, "LABEL %s :\n", interp_op(ir.u.single.op));
    break;
  case IR_FUNCTION:
    fprintf(f, "FUNCTION %s :\n", interp_op(ir.u.single.op));
    break;
  case IR_GOTO:
    fprintf(f, "GOTO %s\n", interp_op(ir.u.single.op));
    break;
  case IR_RETURN:
    fprintf(f, "RETURN %s\n", interp_op(ir.u.single.op));
    break;
  case IR_ARG:
    fprintf(f, "ARG %s\n", interp_op(ir.u.single.op));
    break;
  case IR_PARAM:
    fprintf(f, "PARAM %s\n", interp_op(ir.u.single.op));
    break;
  case IR_READ:
    fprintf(f, "READ %s\n", interp_op(ir.u.single.op));
    break;
  case IR_WRITE:
    fprintf(f, "WRITE %s\n", interp_op(ir.u.single.op));
    break;
  case IR_ADD:
    fprintf(f, "%s := %s + %s\n", interp_op(ir.u.binop.result),
            interp_op(ir.u.binop.op1), interp_op(ir.u.binop.op2));
    break;
  case IR_SUB:
    fprintf(f, "%s := %s - %s\n", interp_op(ir.u.binop.result),
            interp_op(ir.u.binop.op1), interp_op(ir.u.binop.op2));
    break;
  case IR_MUL:
    fprintf(f, "%s := %s * %s\n", interp_op(ir.u.binop.result),
            interp_op(ir.u.binop.op1), interp_op(ir.u.binop.op2));
    break;
  case IR_DIV:
    fprintf(f, "%s := %s / %s\n", interp_op(ir.u.binop.result),
            interp_op(ir.u.binop.op1), interp_op(ir.u.binop.op2));
    break;
  case IR_RELOP:
    fprintf(f, "IF %s %s %s GOTO %s\n", interp_op(ir.u.relop.x),
            interp_relop(ir.u.relop.type), interp_op(ir.u.relop.y),
            interp_op(ir.u.relop.z));
    break;
  case IR_DEC:
    break;
  default:
    break;
  }

  fflush(f);
}

static void ir_generate(FILE *f) {
  InterCodes curr = ir_st;
  while (curr) {
    if (curr->lineno == 0) {
      break;
    }
    ir_generate_step(f, curr->code);
    curr = curr->next;
  }
}

/* helper function */

static int check_node(struct Ast *node, size_t count, ...) {
  if (node->children_count != count) {
    return 0;
  }

  va_list valist;
  va_start(valist, count);
  for (size_t i = 0; i < count; ++i) {
    int type = va_arg(valist, int);
    if (type != node->children[i]->type) {
      return 0;
    }
  }
  va_end(valist);

  return 1;
}

/* traverse and translate functions */
static Operand translate_exp(struct Ast *node, size_t placeno);
static void translate_args(struct Ast *node, SymbolInfo func) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Args);

  Operand args_op[MAX_ARGUMENT];

  struct Ast *curr_node = node;
  for (size_t i = 0; i < func->info.function.argument_count; ++i) {
    struct Ast *exp_node = curr_node->children[0];
    store_place(gen_var(), next_placeno);
    args_op[i] = translate_exp(exp_node, next_placeno++);
    int kind = args_op[i]->kind;
    assert(kind == OP_VARIABLE || kind == OP_CONSTANT || kind == OP_REF);
    curr_node = curr_node->children[2];
  }

  if (!strcmp(func->name, "write")) {
    construct_single_ir(IR_WRITE, args_op[0]);
  } else {
    for (size_t i = func->info.function.argument_count - 1; i >= 0; --i) {
      construct_single_ir(IR_PARAM, args_op[i]);
    }
  }
}

static void translate_cond(struct Ast *node, Operand label_true,
                           Operand label_false) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Exp);

  if (check_node(node, 2, _NOT, _Exp)) {
    translate_cond(node->children[1], label_false, label_true);
    return;
  }

  if (check_node(node, 3, _Exp, _RELOP, _Exp)) {
    size_t x_no = next_placeno;
    store_place(gen_var(), next_placeno++);
    size_t y_no = next_placeno;
    store_place(gen_var(), next_placeno++);

    Operand x = translate_exp(node->children[0], x_no);
    Operand y = translate_exp(node->children[2], y_no);
    Operand z = label_true;

    int type = parser->get_attribute(node->children[1]->attr_index)._attr;

    construct_relop_ir(IR_RELOP, x, y, z, type);
    construct_single_ir(IR_GOTO, label_false);
    return;
  }

  if (check_node(node, 3, _Exp, _AND, _Exp)) {
    size_t label_in_no = next_placeno;
    store_place(gen_label(), label_in_no);

    Operand label_in = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_in->kind = OP_STRING;
    label_in->u.placeno = label_in_no;

    translate_cond(node->children[0], label_in, label_false);
    construct_single_ir(IR_LABEL, label_in);
    translate_cond(node->children[2], label_true, label_false);
    return;
  }

  if (check_node(node, 3, _Exp, _OR, _Exp)) {
    size_t label_in_no = next_placeno;
    store_place(gen_label(), label_in_no);

    Operand label_in = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_in->kind = OP_STRING;
    label_in->u.placeno = label_in_no;

    translate_cond(node->children[0], label_true, label_in);
    construct_single_ir(IR_LABEL, label_in);
    translate_cond(node->children[2], label_true, label_false);
    return;
  }

  store_place(gen_var(), next_placeno);
  Operand x = translate_exp(node, next_placeno++);
  Operand y = (Operand)mm->log_malloc(sizeof(struct OperandItem));
  y->kind = OP_CONSTANT;
  y->u.value = 0;
  Operand z = label_true;

  construct_relop_ir(IR_RELOP, x, y, z, _NE);
  construct_single_ir(IR_GOTO, label_false);
  return;
}

static Operand translate_exp(struct Ast *node,
                             size_t placeno) { // return op for args...
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Exp);

  // assignment
  if (check_node(node, 3, _Exp, _ASSIGNOP, _Exp)) {
    size_t left_no = next_placeno;
    store_place(gen_var(), next_placeno++);
    size_t right_no = next_placeno;
    store_place(gen_var(), next_placeno++);

    Operand left = translate_exp(node->children[0], left_no);
    Operand right = translate_exp(node->children[2], right_no);

    construct_assign_ir(IR_ASSIGN, left, right);

    // placeno unused
    return left;
  }

  // identifier
  if (check_node(node, 1, _ID)) {
    struct Ast *id_node = node->children[0];
    SymbolInfo var = analyzer->lookup(
        VariableInfo, parser->get_attribute(id_node->attr_index)._string);
    assert(var);

    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op->kind = OP_VARIABLE;
    op->u.placeno = next_placeno;
    store_place(var->name, next_placeno++);

    // place unused
    return op;
  }

  // literal
  if (check_node(node, 1, _INT)) {
    int literal = parser->get_attribute(node->children[0]->attr_index)._int;

    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op->kind = OP_CONSTANT;
    op->u.value = literal;

    // place unused
    return op;
  }

  // function call without arguments
  if (check_node(node, 3, _ID, _LP, _RP)) {
    struct Ast *func_id_node = node->children[0];
    SymbolInfo func = analyzer->lookup(
        FunctionInfo, parser->get_attribute(func_id_node->attr_index)._string);
    assert(func);

    if (!strcmp(func->name, "read")) {
      // built-in function read
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_VARIABLE;
      op->u.placeno = placeno;

      construct_single_ir(IR_READ, op);
      return op;
    } else {
      // user-defined function
      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_STRING;
      right->u.placeno = next_placeno;
      store_place(func->name, next_placeno++);

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;

      construct_assign_ir(IR_ASSIGN_CALL, left, right);
      return left;
    }
  }

  // function call with arguments
  if (check_node(node, 4, _ID, _LP, _Args, _RP)) {
    struct Ast *func_id_node = node->children[0];
    SymbolInfo func = analyzer->lookup(
        FunctionInfo, parser->get_attribute(func_id_node->attr_index)._string);
    assert(func);

    translate_args(node->children[2], func);

    if (strcmp(func->name, "write")) {
      // user-defined function
      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_STRING;
      right->u.placeno = next_placeno;
      store_place(func->name, next_placeno++);

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;

      construct_assign_ir(IR_ASSIGN_CALL, left, right);
      return left;
    } else {
      // do nothing
      // construct IR_WRITE in `translate_args` already
      return NULL;
    }
  }

  // literals
  if (check_node(node, 1, _FLOAT)) {
    trace("ignore float literals\n");
    return NULL;
  }

  // ()
  if (check_node(node, 3, _LP, _Exp, _RP)) {
    return translate_exp(node->children[1], placeno);
  }

  // unary minus
  if (check_node(node, 2, _MINUS, _Exp)) {
    store_place(gen_var(), next_placeno);
    Operand op2 = translate_exp(node->children[1], next_placeno++);

    Operand op1 = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op1->kind = OP_CONSTANT;
    op1->u.value = 0;

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_VARIABLE;
    result->u.placeno = placeno;

    construct_binop_ir(IR_SUB, result, op1, op2);
    return result;
  }

  // array
  if (check_node(node, 4, _Exp, _LB, _Exp, _RB)) {
    return NULL;
  }

  // structure
  if (check_node(node, 3, _Exp, _DOT, _ID)) {
    return NULL;
  }

  // conditional exp
  if (check_node(node, 3, _Exp, _AND, _Exp) ||
      check_node(node, 3, _Exp, _OR, _Exp) ||
      check_node(node, 3, _Exp, _RELOP, _Exp) ||
      check_node(node, 2, _NOT, _Exp)) {
    size_t label_true_no = next_placeno;
    store_place(gen_label(), next_placeno++);
    size_t label_false_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_STRING;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_STRING;
    label_true->u.placeno = label_false_no;

    // place := #0
    {
      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_CONSTANT;
      right->u.value = 0;

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;

      construct_assign_ir(IR_ASSIGN, left, right);
    }

    // cond
    translate_cond(node, label_true, label_false);

    // LABEL label_true
    { construct_single_ir(IR_LABEL, label_true); }

    // place := #1
    {
      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_CONSTANT;
      right->u.value = 1;

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;

      construct_assign_ir(IR_ASSIGN, left, right);
    }

    // LABEL label_false
    { construct_single_ir(IR_LABEL, label_false); }

    return NULL;
  }

  // arithmetic exp
  if (check_node(node, 3, _Exp, _PLUS, _Exp) ||
      check_node(node, 3, _Exp, _MINUS, _Exp) ||
      check_node(node, 3, _Exp, _STAR, _Exp) ||
      check_node(node, 3, _Exp, _DIV, _Exp)) {
    size_t op1_no = next_placeno;
    store_place(gen_var(), next_placeno++);
    size_t op2_no = next_placeno;
    store_place(gen_var(), next_placeno++);

    Operand op1 = translate_exp(node->children[0], op1_no);
    Operand op2 = translate_exp(node->children[2], op2_no);

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_VARIABLE;
    result->u.placeno = placeno;

    int kind = 0;
    switch (node->children[1]->type) {
    case _PLUS:
      kind = IR_ADD;
      break;
    case _MINUS:
      kind = IR_SUB;
      break;
    case _STAR:
      kind = IR_MUL;
      break;
    case _DIV:
      kind = IR_DIV;
      break;
    default:
      trace("broken invariant\n");
      break;
    }

    construct_binop_ir(kind, result, op1, op2);
    return result;
  }

  assert(0);
  return NULL;
}

static void translate_dec(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Dec);

  /* Dec -> VarDec */
  if (check_node(node, 1, _VarDec)) {
    trace("ignore local variable definition\n");
    return;
  }

  /* Dec -> VarDec ASSIGNOP Exp */
  if (check_node(node, 3, _VarDec, _ASSIGNOP, _Exp)) {
    struct Ast *var_dec_node = node->children[0];
    if (check_node(var_dec_node, 1, _ID)) {
      struct Ast *id_node = var_dec_node->children[0];
      SymbolInfo var = analyzer->lookup(
          VariableInfo, parser->get_attribute(id_node->attr_index)._string);
      assert(var);

      store_place(var->name, next_placeno);
      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = next_placeno;

      Operand right = translate_exp(node->children[2], next_placeno++);
      construct_assign_ir(IR_ASSIGN, left, right);
    }
    // ignore the direct assignment of array or structure
    return;
  }

  assert(0);
  return;
}

static void translate_declist(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _DecList);

  /* DecList -> Dec */
  if (check_node(node, 1, _Dec)) {
    translate_dec(node->children[0]);
    return;
  }

  /* DecList -> Dec COMMA DecList */
  if (check_node(node, 3, _Dec, _COMMA, _DecList)) {
    translate_dec(node->children[0]);
    translate_declist(node->children[2]);
    return;
  }

  assert(0);
  return;
}

static void translate_def(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Def);

  /* Def -> Specifier DecList SEMI */
  if (check_node(node, 3, _Specifier, _DecList, _SEMI)) {
    translate_declist(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void translate_deflist(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _DefList);

  /* DefList -> Def EMPTY */
  if (check_node(node, 2, _Def, _EMPTY)) {
    translate_def(node->children[0]);
    note("hit EMPTY\n");
    return;
  }

  /* DefList -> Def DefList */
  if (check_node(node, 2, _Def, _DefList)) {
    translate_def(node->children[0]);
    translate_deflist(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void translate_compst(struct Ast *node);
static void translate_stmt(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Stmt);

  if (check_node(node, 2, _Exp, _SEMI)) {
    // expect `Exp -> Exp ASSIGNOP Exp`
    translate_exp(node->children[0], -1);
    return;
  }

  if (check_node(node, 1, _CompSt)) {
    translate_compst(node->children[0]);
    return;
  }

  if (check_node(node, 3, _RETURN, _Exp, _SEMI)) {
    store_place(gen_var(), next_placeno);
    Operand op = translate_exp(node->children[1], next_placeno++);
    construct_single_ir(IR_RETURN, op);
    return;
  }

  if (check_node(node, 5, _IF, _LP, _Exp, _RP, _Stmt)) {
    size_t label_true_no = next_placeno;
    store_place(gen_label(), next_placeno++);
    size_t label_false_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_STRING;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_STRING;
    label_false->u.placeno = label_false_no;

    translate_cond(node->children[2], label_true, label_false);

    // LABEL label_true
    { construct_single_ir(IR_LABEL, label_true); }

    translate_stmt(node->children[4]);

    // LABEL label_false
    { construct_single_ir(IR_LABEL, label_false); }

    return;
  }

  if (check_node(node, 7, _IF, _LP, _Exp, _RP, _Stmt, _ELSE, _Stmt)) {
    size_t label_end_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    size_t label_true_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    size_t label_false_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_STRING;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_STRING;
    label_false->u.placeno = label_false_no;

    translate_cond(node->children[2], label_true, label_false);

    // LABEL label_true
    { construct_single_ir(IR_LABEL, label_true); }

    translate_stmt(node->children[4]);

    // GOTO label_end
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_STRING;
      op->u.placeno = label_end_no;

      construct_single_ir(IR_GOTO, op);
    }

    // LABEL label_false
    { construct_single_ir(IR_LABEL, label_false); }

    translate_stmt(node->children[6]);

    // LABEL label_end
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_STRING;
      op->u.placeno = label_end_no;

      construct_single_ir(IR_LABEL, op);
    }

    return;
  }

  if (check_node(node, 5, _WHILE, _LP, _Exp, _RP, _Stmt)) {
    size_t label_begin_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    size_t label_true_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    size_t label_false_no = next_placeno;
    store_place(gen_label(), next_placeno++);

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_STRING;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_STRING;
    label_false->u.placeno = label_false_no;

    // LABEL label_begin
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_STRING;
      op->u.placeno = label_begin_no;

      construct_single_ir(IR_LABEL, op);
    }

    translate_cond(node->children[2], label_true, label_false);

    // LABEL label_true
    { construct_single_ir(IR_LABEL, label_true); }

    translate_stmt(node->children[4]);

    // GOTO label_begin
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_STRING;
      op->u.placeno = label_begin_no;

      construct_single_ir(IR_GOTO, op);
    }

    // LABEL label_false
    { construct_single_ir(IR_LABEL, label_false); }

    return;
  }

  return;
}

static void translate_stmtlist(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _StmtList);

  /* StmtList -> Stmt EMPTY */
  if (check_node(node, 2, _Stmt, _EMPTY)) {
    translate_stmt(node->children[0]);
    note("hit EMPTY\n");
    return;
  }

  /* StmtList -> Stmt StmtList */
  if (check_node(node, 2, _Stmt, _StmtList)) {
    translate_stmt(node->children[0]);
    translate_stmtlist(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void translate_compst(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _CompSt);

  /* CompSt -> LC DefList StmtList RC */
  assert(node->children_count == 4);
  if (node->children[1]->type != _EMPTY) {
    translate_deflist(node->children[1]);
  }

  if (node->children[2]->type != _EMPTY) {
    translate_stmtlist(node->children[2]);
  }

  return;
}

static void translate_func(struct Ast *node, SymbolInfo func) {
  trace("translate %s\n", func->name);
  assert(node->type == _CompSt);

  // FUNCTION f :
  Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
  op->kind = OP_STRING;
  op->u.placeno = next_placeno;
  store_place(func->name, next_placeno++);

  construct_single_ir(IR_FUNCTION, op);

  // PARAM x
  for (size_t i = 0; i < func->info.function.argument_count; ++i) {
    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op->kind = OP_STRING;
    op->u.placeno = next_placeno;
    store_place(func->info.function.arguments[i]->name, next_placeno++);

    construct_single_ir(IR_PARAM, op);
  }

  translate_compst(node);
}

static void translate_ext_def(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _ExtDef);

  /* ExtDef -> Specifier SEMI */
  if (check_node(node, 2, _Specifier, _SEMI)) {
    trace("ignore structure definition\n");
    return;
  }

  /* ExtDef -> Specifier ExtDecList SEMI */
  if (check_node(node, 3, _Specifier, _ExtDecList, _SEMI)) {
    trace("ignore global variable definition\n");
    return;
  }

  /* ExtDef -> Specifier FunDec CompSt */
  if (check_node(node, 3, _Specifier, _FunDec, _CompSt)) {
    struct Ast *func_node = node->children[1];
    struct Ast *func_id_node = func_node->children[0];
    SymbolInfo func = analyzer->lookup(
        FunctionInfo, parser->get_attribute(func_id_node->attr_index)._string);
    assert(func);
    translate_func(node->children[2], func);
    return;
  }

  assert(0);
  return;
}

static void translate_ext_deflist(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _ExtDefList);

  if (check_node(node, 2, _ExtDef, _EMPTY)) {
    translate_ext_def(node->children[0]);
    note("hit EMPTY\n");
    return;
  }

  if (check_node(node, 2, _ExtDef, _ExtDefList)) {
    translate_ext_def(node->children[0]);
    translate_ext_deflist(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void translate_program(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Program);

  if (check_node(node, 1, _ExtDefList)) {
    translate_ext_deflist(node->children[0]);
    return;
  }

  assert(0);
  return;
}

static void translate(struct Ast *root) {
  init_ir();
  translate_program(root);
}

/* interface */

MODULE_DEF(ir) = {
    .ir_translate = translate,
    .ir_generate = ir_generate,
};
