#include "common.h"
#include "module.h"

/* error handler */

static jmp_buf buf;
int ir_errors = 0;

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

static Type int_type;
static void build_int_type() {
  int_type = (Type)mm->log_malloc(sizeof(struct TypeItem));
  int_type->kind = BASIC;
  int_type->u.basic = _INT;
  int_type->width = 4;
}

/* place system */

#define MAX_PLACE 1024
static size_t next_placeno = 0;
static const char *placemap[MAX_PLACE];

static size_t next_anonymous_no = 0;

static struct {
  const char *hint;
  const char *prefix;
} hint_to_prefix[] = {[0] =
                          {
                              .hint = "tmp",
                              .prefix = "ir_tmp_",
                          },
                      [1] =
                          {
                              .hint = "label",
                              .prefix = "ir_label_",
                          },
                      [2] =
                          {
                              .hint = "func",
                              .prefix = "",
                          },
                      [3] =
                          {
                              .hint = "var",
                              .prefix = "ir_var_",
                          },
                      [4] = {
                          .hint = "param",
                          .prefix = "ir_addr_",
                      }};

#define LENGTH_ARR(arr) (sizeof(arr) / sizeof((arr)[0]))
static size_t get_hint_index(const char *hint) {
  for (size_t i = 0; i < LENGTH_ARR(hint_to_prefix); ++i) {
    if (!strcmp(hint, hint_to_prefix[i].hint)) {
      return i;
    }
  }
  return -1;
}

static int is_param_exist(const char *name) {
  char *name_fix = (char *)mm->log_malloc((LENGTH + 16) * sizeof(char));
  memset(name_fix, 0, (LENGTH + 16) * sizeof(char));

  sprintf(name_fix, "%s%s", hint_to_prefix[4].prefix, name);

  for (size_t i = 0; i < next_placeno; ++i) {
    if (!strcmp(name_fix, placemap[i])) {
      return 1;
    }
  }

  return 0;
}

static size_t store_place(const char *name, const char *hint) {
  if (next_placeno >= MAX_PLACE) {
    printf("IR Error: Placemap overflow.\n");
    longjmp(buf, 1);
  }

  char *name_fix = (char *)mm->log_malloc((LENGTH + 16) * sizeof(char));
  memset(name_fix, 0, (LENGTH + 16) * sizeof(char));

  size_t index = get_hint_index(hint);
  assert(index != -1);

  if (!name) {
    assert(index == 0 || index == 1);
    sprintf(name_fix, "%s%zu", hint_to_prefix[index].prefix,
            next_anonymous_no++);
  } else {
    sprintf(name_fix, "%s%s", hint_to_prefix[index].prefix, name);
  }

  placemap[next_placeno] = name_fix;

  return next_placeno++;
}

static size_t load_placeno(const char *name, const char *hint) {
  char *name_fix = (char *)mm->log_malloc((LENGTH + 16) * sizeof(char));
  memset(name_fix, 0, (LENGTH + 16) * sizeof(char));

  size_t index = get_hint_index(hint);
  assert(index != -1);
  assert(index >= 2);  // avoid tmp or label
  assert(name);

  sprintf(name_fix, "%s%s", hint_to_prefix[index].prefix, name);

  // avoid redundancy
  size_t res = -1;
  for (size_t i = 0; i < next_placeno; ++i) {
    if (!strcmp(placemap[i], name_fix)) {
      res = i;
      break;
    }
  }

  if (res == -1) {
    return store_place(name, hint);
  }

  return res;
}

/* ir */

static InterCodes ir_st;
static InterCodes ir_ed;

static void init_ir() {
  ir_st = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));
  ir_st->prev->lineno = 0;  // for increasing lineno
  ir_st->next = NULL;

  ir_ed = ir_st;
  ir_ed->lineno = -1;  // mark ir ed
}

static void extend_ir() {
  ir_ed->lineno = ir_ed->prev->lineno + 1;
  ir_ed->next = (InterCodes)mm->log_malloc(sizeof(struct InterCodesItem));

  InterCodes tmp = ir_ed;
  ir_ed = ir_ed->next;
  ir_ed->prev = tmp;
  ir_ed->next = NULL;
  ir_ed->lineno = -1;
}

static void construct_single_ir(int kind, Operand op) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.single.op = op,
  };

  extend_ir();
}

static void construct_assign_ir(int kind, Operand left, Operand right) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.assign.right = right,
      .u.assign.left = left,
  };

  extend_ir();
}

static void construct_binop_ir(int kind, Operand result, Operand op1,
                               Operand op2) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.binop.op1 = op1,
      .u.binop.op2 = op2,
      .u.binop.result = result,
  };

  extend_ir();
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

  extend_ir();
}

static void construct_dec_ir(int kind, Operand var, Operand size) {
  ir_ed->code = (struct InterCode){
      .kind = kind,
      .u.dec.var = var,
      .u.dec.size = size,
  };

  extend_ir();
}

static const char *interp_op(Operand op) {
  switch (op->kind) {
    case OP_CONSTANT: {
      char *res = (char *)mm->log_malloc(16 * sizeof(char));
      memset(res, 0, 16 * sizeof(char));
      sprintf(res, "#%d", op->u.value);
      return res;
    }
    case OP_FUNC:
    case OP_LABEL:
    case OP_ADDRESS:
    case OP_VARIABLE: {
      return placemap[op->u.placeno];
    }
    case OP_ADDRESS_ORI: {
      char *res = (char *)mm->log_malloc((LENGTH + 1) * sizeof(char));
      memset(res, 0, (LENGTH + 1) * sizeof(char));
      sprintf(res, "&%s", placemap[op->u.placeno]);
      return res;
    }
    case OP_ADDRESS_DEREF: {
      char *res = (char *)mm->log_malloc((LENGTH + 1) * sizeof(char));
      memset(res, 0, (LENGTH + 1) * sizeof(char));
      sprintf(res, "*%s", placemap[op->u.placeno]);
      return res;
    }
    default:
      printf("IR Error: Broken invariant.\n");
      longjmp(buf, 1);
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
      printf("IR Error: Broken invariant.\n");
      longjmp(buf, 1);
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
      assert(ir.u.dec.var->kind == OP_ADDRESS_ORI);
      assert(ir.u.dec.size->kind == OP_CONSTANT);
      fprintf(f, "DEC %s %u\n", placemap[ir.u.dec.var->u.placeno],
              ir.u.dec.size->u.value);
      break;
    default:
      printf("IR Error: Broken invariant.\n");
      longjmp(buf, 1);
      break;
  }

  fflush(f);
}

static void ir_generate(FILE *f) {
  InterCodes curr = ir_st;
  while (curr) {
    if (curr->lineno == -1) {  // ir end
      break;
    }
    ir_generate_step(f, curr->code);
    curr = curr->next;
  }
}

/* translate functions */

static Operand guard = (Operand)1; // TODO -> bad practice
static Operand translate_exp(struct Ast *node, size_t placeno);
static Operand translate_exp_chk(struct Ast *node, size_t placeno) {
  Operand res = translate_exp(node, placeno);
  if (res == guard) {
      printf("IR Error: Code serves bool as int.\n");
      longjmp(buf, 1);
      return NULL;
  }
  return res;
}

static void translate_args(struct Ast *node, SymbolInfo func) {
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Args);

  Operand args_op[MAX_ARGUMENT];
  memset(args_op, 0, sizeof(args_op));

  struct Ast *curr_node = node;
  for (size_t i = 0; i < func->info.function.argument_count; ++i) {
    struct Ast *exp_node = curr_node->children[0];
    args_op[i] = translate_exp_chk(exp_node, store_place(NULL, "tmp"));

    int kind = args_op[i]->kind;
    assert(kind == OP_ADDRESS || kind == OP_ADDRESS_DEREF ||
           kind == OP_ADDRESS_ORI || kind == OP_CONSTANT ||
           kind == OP_VARIABLE);

    if (args_op[i]->type->kind == ARRAY) {
      printf("IR Error: Code contains array type arguments.\n");
      longjmp(buf, 1);
    }

    curr_node = curr_node->children[2];
  }

  if (!strcmp(func->name, "write")) {
    construct_single_ir(IR_WRITE, args_op[0]);
  } else {
    // dead loop when using `size_t`
    for (int i = func->info.function.argument_count - 1; i >= 0; --i) {
      construct_single_ir(IR_ARG, args_op[i]);
    }
  }
}

static void translate_cond(struct Ast *node, Operand label_true,
                           Operand label_false) {
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Exp);

  if (check_node(node, 2, _NOT, _Exp)) {
    translate_cond(node->children[1], label_false, label_true);
    return;
  }

  if (check_node(node, 3, _Exp, _RELOP, _Exp)) {
    size_t x_no = store_place(NULL, "tmp");
    size_t y_no = store_place(NULL, "tmp");

    Operand x = translate_exp_chk(node->children[0], x_no);
    Operand y = translate_exp_chk(node->children[2], y_no);
    Operand z = label_true;

    int type = parser->get_attribute(node->children[1]->attr_index)._attr;

    construct_relop_ir(IR_RELOP, x, y, z, type);
    construct_single_ir(IR_GOTO, label_false);
    return;
  }

  if (check_node(node, 3, _Exp, _AND, _Exp)) {
    size_t label_in_no = store_place(NULL, "label");

    Operand label_in = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_in->kind = OP_LABEL;
    label_in->u.placeno = label_in_no;

    translate_cond(node->children[0], label_in, label_false);
    construct_single_ir(IR_LABEL, label_in);
    translate_cond(node->children[2], label_true, label_false);
    return;
  }

  if (check_node(node, 3, _Exp, _OR, _Exp)) {
    size_t label_in_no = store_place(NULL, "label");

    Operand label_in = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_in->kind = OP_LABEL;
    label_in->u.placeno = label_in_no;

    translate_cond(node->children[0], label_true, label_in);
    construct_single_ir(IR_LABEL, label_in);
    translate_cond(node->children[2], label_true, label_false);
    return;
  }

  // serve int as bool
  Operand x = translate_exp_chk(node, store_place(NULL, "tmp"));
  Operand y = (Operand)mm->log_malloc(sizeof(struct OperandItem));
  y->kind = OP_CONSTANT;
  y->u.value = 0;
  Operand z = label_true;

  construct_relop_ir(IR_RELOP, x, y, z, _NE);
  construct_single_ir(IR_GOTO, label_false);

  return;
}

static Operand translate_exp(
    struct Ast *node,
    size_t placeno) {  // return op for args, cond and exp
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Exp);

  // assignment
  if (check_node(node, 3, _Exp, _ASSIGNOP, _Exp)) {
    size_t left_no = store_place(NULL, "tmp");
    size_t right_no = store_place(NULL, "tmp");

    Operand left = translate_exp_chk(node->children[0], left_no);
    Operand right = translate_exp_chk(node->children[2], right_no);

    assert(left->type && right->type);
    if (left->type->kind != BASIC && right->type->kind != BASIC) {
      printf("IR Error: Code contains direct assignment between compound variables.\n");
      longjmp(buf, 1);
    }

    construct_assign_ir(IR_ASSIGN, left, right);

    // placeno unused
    return left;
  }

  // identifier
  if (check_node(node, 1, _ID)) {
    struct Ast *id_node = node->children[0];
    SymbolInfo var_sym = analyzer->lookup(
        VariableInfo, parser->get_attribute(id_node->attr_index)._string);
    assert(var_sym);

    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    int is_basic = (var_sym->info.type->kind == BASIC);

    if (is_basic) {
      op->kind = OP_VARIABLE;
      op->u.placeno = load_placeno(var_sym->name, "var");
      op->type = var_sym->info.type;
    } else {
      if (is_param_exist(var_sym->name)) {
        assert(var_sym->info.type->kind == STRUCTURE);
        op->kind = OP_ADDRESS;
        op->u.placeno = load_placeno(var_sym->name, "param");
        op->type = var_sym->info.type;
      } else {
        if (var_sym->info.type->kind == ARRAY) {
          if (var_sym->info.type->u.array.elem->kind == ARRAY) {
            printf("IR Error: Code contains multi-dimensional array type.\n");
            longjmp(buf, 1);
          }
        }

        size_t before_no = next_placeno;
        size_t var_no = load_placeno(var_sym->name, "var");
        size_t after_no = next_placeno;

        if (before_no != after_no) {
          // alloc space for array or structure for the first time
          Operand var = (Operand)mm->log_malloc(sizeof(struct OperandItem));
          var->kind = OP_ADDRESS_ORI;
          var->u.placeno = var_no;

          Operand size = (Operand)mm->log_malloc(sizeof(struct OperandItem));
          size->kind = OP_CONSTANT;
          size->u.value = var_sym->info.type->width;

          construct_dec_ir(IR_DEC, var, size);
        }

        op->kind = OP_ADDRESS_ORI;
        op->u.placeno = var_no;
        op->type = var_sym->info.type;
      }
    }

    // place unused
    return op;
  }

  // literal
  if (check_node(node, 1, _INT)) {
    unsigned literal =
        parser->get_attribute(node->children[0]->attr_index)._int;

    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op->kind = OP_CONSTANT;
    op->u.value = literal;
    op->type = int_type;

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
      op->type = int_type;

      construct_single_ir(IR_READ, op);
      return op;
    } else {
      // user-defined function
      size_t func_no = load_placeno(func->name, "func");

      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_FUNC;
      right->u.placeno = func_no;

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;
      left->type = int_type;

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
      size_t func_no = load_placeno(func->name, "func");

      Operand right = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      right->kind = OP_FUNC;
      right->u.placeno = func_no;

      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = placeno;
      left->type = int_type;

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
    printf("IR Error: Code contains float literals.\n");
    longjmp(buf, 1);
    return NULL;
  }

  // ()
  if (check_node(node, 3, _LP, _Exp, _RP)) {
    return translate_exp_chk(node->children[1], placeno);
  }

  // unary minus
  if (check_node(node, 2, _MINUS, _Exp)) {
    Operand op2 = translate_exp_chk(node->children[1], store_place(NULL, "tmp"));

    Operand op1 = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op1->kind = OP_CONSTANT;
    op1->u.value = 0;

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_VARIABLE;
    result->u.placeno = placeno;
    result->type = int_type;

    construct_binop_ir(IR_SUB, result, op1, op2);
    return result;
  }

  // array
  if (check_node(node, 4, _Exp, _LB, _Exp, _RB)) {
    size_t left_no = store_place(NULL, "tmp");
    size_t right_no = store_place(NULL, "tmp");

    Operand left_op = translate_exp_chk(node->children[0], left_no);
    Operand right_op = translate_exp_chk(node->children[2], right_no);

    assert(left_op->kind == OP_ADDRESS || left_op->kind == OP_ADDRESS_ORI);
    assert(right_op->kind == OP_VARIABLE || right_op->kind == OP_CONSTANT);

    Type arr_type = left_op->type;
    assert(arr_type && arr_type->kind == ARRAY);

    Operand op1 = left_op;
    Operand op2 = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op2->kind = OP_VARIABLE;
    op2->u.placeno = store_place(NULL, "tmp");

    Operand op2_1 = right_op;
    Operand op2_2 = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op2_2->kind = OP_CONSTANT;
    op2_2->u.value = arr_type->u.array.elem->width;

    construct_binop_ir(IR_MUL, op2, op2_1, op2_2);

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_ADDRESS;
    result->u.placeno = store_place(NULL, "tmp");
    result->type = arr_type->u.array.elem;

    construct_binop_ir(IR_ADD, result, op1, op2);

    if (result->type->kind == BASIC) {
      Operand result_fix = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      result_fix->kind = OP_ADDRESS_DEREF;
      result_fix->u.placeno = result->u.placeno;
      result_fix->type = int_type;

      return result_fix;
    }

    return result;
  }

  // structure
  if (check_node(node, 3, _Exp, _DOT, _ID)) {
    size_t struct_no = store_place(NULL, "tmp");

    Operand struct_op = translate_exp_chk(node->children[0], struct_no);
    assert(struct_op->kind == OP_ADDRESS || struct_op->kind == OP_ADDRESS_ORI);

    Type struct_type = struct_op->type;
    assert(struct_type && struct_type->kind == STRUCTURE);

    const char *field_name =
        parser->get_attribute(node->children[2]->attr_index)._string;
    unsigned offset = -1;
    FieldList curr = struct_type->u.structure;
    Type field_type = NULL;
    while (curr) {
      if (!strcmp(field_name, curr->name)) {
        offset = curr->offset;
        field_type = curr->type;
      }
      curr = curr->tail;
    }
    assert(offset != -1 && field_type);

    Operand op1 = struct_op;
    Operand op2 = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    op2->kind = OP_CONSTANT;
    op2->u.value = offset;

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_ADDRESS;
    result->u.placeno = store_place(NULL, "tmp");
    result->type = field_type;

    construct_binop_ir(IR_ADD, result, op1, op2);

    if (result->type->kind == BASIC) {
      Operand result_fix = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      result_fix->kind = OP_ADDRESS_DEREF;
      result_fix->u.placeno = result->u.placeno;
      result_fix->type = int_type;

      return result_fix;
    }

    return result;
  }

  // conditional exp
  if (check_node(node, 3, _Exp, _AND, _Exp) ||
      check_node(node, 3, _Exp, _OR, _Exp) ||
      check_node(node, 3, _Exp, _RELOP, _Exp) ||
      check_node(node, 2, _NOT, _Exp)) {
    size_t label_true_no = store_place(NULL, "label");
    size_t label_false_no = store_place(NULL, "label");

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_LABEL;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_LABEL;
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

    // avoid serve bool as int
    return guard;
  }

  // arithmetic exp
  if (check_node(node, 3, _Exp, _PLUS, _Exp) ||
      check_node(node, 3, _Exp, _MINUS, _Exp) ||
      check_node(node, 3, _Exp, _STAR, _Exp) ||
      check_node(node, 3, _Exp, _DIV, _Exp)) {
    size_t op1_no = store_place(NULL, "tmp");
    size_t op2_no = store_place(NULL, "tmp");

    Operand op1 = translate_exp_chk(node->children[0], op1_no);
    Operand op2 = translate_exp_chk(node->children[2], op2_no);

    Operand result = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    result->kind = OP_VARIABLE;
    result->u.placeno = placeno;
    result->type = int_type;

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
        printf("IR Error: Broken invariant.\n");
        longjmp(buf, 1);
        break;
    }

    construct_binop_ir(kind, result, op1, op2);
    return result;
  }

  assert(0);
  return NULL;
}

static void translate_dec(struct Ast *node) {
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Dec);

  /* Dec -> VarDec */
  if (check_node(node, 1, _VarDec)) {
    trace("ignore local variable declaration\n");
    return;
  }

  /* Dec -> VarDec ASSIGNOP Exp */
  if (check_node(node, 3, _VarDec, _ASSIGNOP, _Exp)) {
    struct Ast *var_dec_node = node->children[0];
    // ignore the direct assignment of array
    if (check_node(var_dec_node, 1, _ID)) {
      struct Ast *id_node = var_dec_node->children[0];
      SymbolInfo var = analyzer->lookup(
          VariableInfo, parser->get_attribute(id_node->attr_index)._string);
      assert(var);

      size_t var_no = load_placeno(var->name, "var");
      Operand left = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      left->kind = OP_VARIABLE;
      left->u.placeno = var_no;

      Operand right =
          translate_exp_chk(node->children[2], store_place(NULL, "tmp"));
      construct_assign_ir(IR_ASSIGN, left, right);
    }
    return;
  }

  assert(0);
  return;
}

static void translate_declist(struct Ast *node) {
  action("hit %s\n", unit_names[node->type]);
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
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Def);

  /* Def -> Specifier DecList SEMI */
  if (check_node(node, 3, _Specifier, _DecList, _SEMI)) {
    // ignore the direct assignment of structure
    if (check_node(node->children[0], 1, _TYPE)) {
      translate_declist(node->children[1]);
    }
    return;
  }

  assert(0);
  return;
}

static void translate_deflist(struct Ast *node) {
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _DefList);

  /* DefList -> Def EMPTY */
  if (check_node(node, 2, _Def, _EMPTY)) {
    translate_def(node->children[0]);
    action("hit EMPTY\n");
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
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Stmt);

  if (check_node(node, 2, _Exp, _SEMI)) {
    // expect `Exp -> Exp ASSIGNOP Exp`
    translate_exp_chk(node->children[0], -1);
    return;
  }

  if (check_node(node, 1, _CompSt)) {
    translate_compst(node->children[0]);
    return;
  }

  if (check_node(node, 3, _RETURN, _Exp, _SEMI)) {
    Operand op = translate_exp_chk(node->children[1], store_place(NULL, "tmp"));
    construct_single_ir(IR_RETURN, op);
    return;
  }

  if (check_node(node, 5, _IF, _LP, _Exp, _RP, _Stmt)) {
    size_t label_true_no = store_place(NULL, "label");
    size_t label_false_no = store_place(NULL, "label");

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_LABEL;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_LABEL;
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
    size_t label_end_no = store_place(NULL, "label");
    size_t label_true_no = store_place(NULL, "label");
    size_t label_false_no = store_place(NULL, "label");

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_LABEL;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_LABEL;
    label_false->u.placeno = label_false_no;

    translate_cond(node->children[2], label_true, label_false);

    // LABEL label_true
    { construct_single_ir(IR_LABEL, label_true); }

    translate_stmt(node->children[4]);

    // GOTO label_end
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_LABEL;
      op->u.placeno = label_end_no;

      construct_single_ir(IR_GOTO, op);
    }

    // LABEL label_false
    { construct_single_ir(IR_LABEL, label_false); }

    translate_stmt(node->children[6]);

    // LABEL label_end
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_LABEL;
      op->u.placeno = label_end_no;

      construct_single_ir(IR_LABEL, op);
    }

    return;
  }

  if (check_node(node, 5, _WHILE, _LP, _Exp, _RP, _Stmt)) {
    size_t label_begin_no = store_place(NULL, "label");
    size_t label_true_no = store_place(NULL, "label");
    size_t label_false_no = store_place(NULL, "label");

    Operand label_true = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_true->kind = OP_LABEL;
    label_true->u.placeno = label_true_no;

    Operand label_false = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    label_false->kind = OP_LABEL;
    label_false->u.placeno = label_false_no;

    // LABEL label_begin
    {
      Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
      op->kind = OP_LABEL;
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
      op->kind = OP_LABEL;
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
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _StmtList);

  /* StmtList -> Stmt EMPTY */
  if (check_node(node, 2, _Stmt, _EMPTY)) {
    translate_stmt(node->children[0]);
    action("hit EMPTY\n");
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
  action("hit %s\n", unit_names[node->type]);
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
  size_t func_no = load_placeno(func->name, "func");

  Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
  op->kind = OP_FUNC;
  op->u.placeno = func_no;

  construct_single_ir(IR_FUNCTION, op);

  // PARAM x
  for (size_t i = 0; i < func->info.function.argument_count; ++i) {
    Type param_type = func->info.function.arguments[i]->info.type;
    if (param_type->kind == ARRAY) {
      printf("IR Error: Code contains array type parameters.\n");
      longjmp(buf, 1);
    }

    Operand op = (Operand)mm->log_malloc(sizeof(struct OperandItem));
    size_t var_no;

    if (param_type->kind == BASIC) {
      var_no = load_placeno(func->info.function.arguments[i]->name, "var");
      op->kind = OP_VARIABLE;
    } else if (param_type->kind == STRUCTURE) {
      var_no = load_placeno(func->info.function.arguments[i]->name, "param");
      op->kind = OP_ADDRESS;
    } else {
      printf("IR Error: Broken invariant.\n");
      longjmp(buf, 1);
    }

    op->u.placeno = var_no;

    construct_single_ir(IR_PARAM, op);
  }

  translate_compst(node);
}

static void translate_ext_def(struct Ast *node) {
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _ExtDef);

  /* ExtDef -> Specifier SEMI */
  if (check_node(node, 2, _Specifier, _SEMI)) {
    trace("ignore structure definition\n");
    return;
  }

  /* ExtDef -> Specifier ExtDecList SEMI */
  if (check_node(node, 3, _Specifier, _ExtDecList, _SEMI)) {
    trace("ignore global variable declaration\n");
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
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _ExtDefList);

  if (check_node(node, 2, _ExtDef, _EMPTY)) {
    translate_ext_def(node->children[0]);
    action("hit EMPTY\n");
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
  action("hit %s\n", unit_names[node->type]);
  assert(node->type == _Program);

  if (check_node(node, 1, _ExtDefList)) {
    translate_ext_deflist(node->children[0]);
    return;
  }

  assert(0);
  return;
}

static void translate(struct Ast *root) {
  if (!setjmp(buf)) {
    init_ir();
    build_int_type();
    translate_program(root);
  } else {
    printf("IR Error: Translation fails.\n");
    ir_errors++;
  }
}

static InterCodes get_ir_st() {
  return ir_st;
}

static const char *get_place(size_t index) {
  return placemap[index];
}

/* interface */

MODULE_DEF(ir) = {
    .ir_translate = translate,
    .ir_generate = ir_generate,
    .get_ir_st = get_ir_st,
    .get_place = get_place,
};
