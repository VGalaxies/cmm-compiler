#include "common.h"
#include "module.h"

static InterCodes ir_st;
static InterCodes ir_ed;

static void init_ir() {
  ir_st = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev->lineno = 0;

  ir_st->next = NULL;
  ir_ed = ir_st;
}

/* place, var, label */

#define MAX_PLACE 1024
static size_t next_placeno = 0;
static const char *placemap[MAX_PLACE];

static size_t next_varno = 0;
static size_t next_labelno = 0;

static char *gen_var() {
  char *var = (char *)mm->log_malloc((64 * sizeof(char)));
  sprintf(var, "var%zu", next_varno++);
  return var;
}

static char *gen_label() {
  char *lable = (char *)mm->log_malloc((64 * sizeof(char)));
  sprintf(lable, "lable%zu", next_labelno++);
  return lable;
}

static void store_place(const char *name, size_t placeno) {
  if (placeno >= MAX_PLACE) {
    assert(0); // TODO
  }
  placemap[placeno] = name;
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

static void translate_args(struct Ast* node, SymbolInfo func) {

}

static void translate_exp(struct Ast *node, size_t placeno) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Exp);

  // identifier
  if (check_node(node, 1, _ID)) {
    struct Ast *id_node = node->children[0];
    SymbolInfo var = analyzer->lookup(
        VariableInfo, parser->get_attribute(id_node->attr_index)._string);
    assert(var);

    Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    right->kind = OP_VARIABLE;
    right->u.placeno = next_placeno;
    store_place(var->name, next_placeno++);

    Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    left->kind = OP_VARIABLE;
    left->u.placeno = placeno;

    ir_ed->code = (struct InterCode){
        .kind = IR_ASSIGN,
        .u.assign.right = right,
        .u.assign.left = left,
    };

    ir_ed->lineno = ir_ed->prev->lineno + 1;
    ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
    ir_ed = ir_ed->next;

    return;
  }

  // function call without arguments
  if (check_node(node, 3, _ID, _LP, _RP)) {
    struct Ast *func_id_node = node->children[0];
    SymbolInfo func = analyzer->lookup(
        VariableInfo, parser->get_attribute(func_id_node->attr_index)._string);
    assert(func);

    if (!strcmp(func->name, "read")) {
      // built-in function read
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_VARIABLE;
      op->u.placeno = placeno;

      ir_ed->code = (struct InterCode){
          .kind = IR_READ,
          .u.single.op = op,
      };

      ir_ed->lineno = ir_ed->prev->lineno + 1;
      ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
      ir_ed = ir_ed->next;
    } else {
      // user-defined function
      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_STRING;
      right->u.placeno = next_placeno;
      store_place(func->name, next_placeno++);

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;

      ir_ed->code = (struct InterCode){
          .kind = IR_ASSIGN_CALL,
          .u.assign.right = right,
          .u.assign.left = left,
      };

      ir_ed->lineno = ir_ed->prev->lineno + 1;
      ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
      ir_ed = ir_ed->next;
    }

    return;
  }

  // function call with arguments
  if (check_node(node, 4, _ID, _LP, _Args, _RP)) {
    struct Ast *func_id_node = node->children[0];
    SymbolInfo func = analyzer->lookup(
        VariableInfo, parser->get_attribute(func_id_node->attr_index)._string);
    assert(func);

    translate_args(node->children[2], func);
  }

  // literal
  if (check_node(node, 1, _INT)) {
  }

  // literals
  if (check_node(node, 1, _FLOAT)) {
    trace("ignore float literals\n");
    return;
  }

  // ()
  if (check_node(node, 3, _LP, _Exp, _RP)) {
  }

  // unary
  if (check_node(node, 2, _MINUS, _Exp)) {
  }

  // unary
  if (check_node(node, 2, _NOT, _Exp)) {
  }

  // array
  if (check_node(node, 4, _Exp, _LB, _Exp, _RB)) {
  }

  // structure
  if (check_node(node, 3, _Exp, _DOT, _ID)) {
  }

  // binary
  if (node->children_count == 3 && node->children[0]->type == _Exp &&
      node->children[2]->type == _Exp) {
  }

  assert(0);
  return;
}

static void declaration(struct Ast *node) {
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
      store_place(gen_var(), next_placeno);
      translate_exp(node->children[2], next_placeno++);
    }
    // ignore the direct assignment of array or structure
    return;
  }

  assert(0);
  return;
}

static void declaration_list(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _DecList);

  /* DecList -> Dec */
  if (check_node(node, 1, _Dec)) {
    declaration(node->children[0]);
    return;
  }

  /* DecList -> Dec COMMA DecList */
  if (check_node(node, 3, _Dec, _COMMA, _DecList)) {
    declaration(node->children[0]);
    declaration_list(node->children[2]);
    return;
  }

  assert(0);
  return;
}

static void definition(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Def);

  /* Def -> Specifier DecList SEMI */
  if (check_node(node, 3, _Specifier, _DecList, _SEMI)) {
    declaration_list(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void definition_list(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _DefList);

  /* DefList -> Def EMPTY */
  if (check_node(node, 2, _Def, _EMPTY)) {
    note("hit EMPTY\n");
    return;
  }

  /* DefList -> Def DefList */
  if (check_node(node, 2, _Def, _DefList)) {
    definition(node->children[0]);
    definition_list(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void translate_stmt(struct Ast *node) {}

static void statement_list(struct Ast *node) {
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
    statement_list(node->children[1]);
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
    definition_list(node->children[1]);
  }

  if (node->children[2]->type != _EMPTY) {
    statement_list(node->children[2]);
  }

  assert(0);
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

  ir_ed->code = (struct InterCode){
      .kind = IR_FUNCTION,
      .u.single.op = op,
  };

  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_ed = ir_ed->next;

  // PARAM x
  for (size_t i = 0; i < func->info.function.argument_count; ++i) {
    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op->kind = OP_STRING;
    op->u.placeno = next_placeno;
    store_place(func->info.function.arguments[i]->name, next_placeno++);

    ir_ed->code = (struct InterCode){
        .kind = IR_PARAM,
        .u.single.op = op,
    };

    ir_ed->lineno = ir_ed->prev->lineno + 1;
    ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
    ir_ed = ir_ed->next;
  }

  translate_compst(node);
}

static void external_definition(struct Ast *node) {
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

static void external_definition_list(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _ExtDefList);

  if (check_node(node, 2, _ExtDef, _EMPTY)) {
    external_definition(node->children[0]);
    note("hit EMPTY\n");
    return;
  }

  if (check_node(node, 2, _ExtDef, _ExtDefList)) {
    external_definition(node->children[0]);
    external_definition_list(node->children[1]);
    return;
  }

  assert(0);
  return;
}

static void program(struct Ast *node) {
  note("hit %s\n", unit_names[node->type]);
  assert(node->type == _Program);

  if (check_node(node, 1, _ExtDefList)) {
    external_definition_list(node->children[0]);
    return;
  }

  assert(0);
  return;
}

static void translate(struct Ast *root) { program(root); }

/* interface */

MODULE_DEF(ir) = {
    .ir_translate = translate,
};
