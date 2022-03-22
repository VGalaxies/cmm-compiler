#include "common.h"

/* error handler */

int semantic_errors = 0;
static void semantic_error(int type, int lineno) {
  ++semantic_errors;
  panic("Error type %d at Line %d.", type, lineno);
}

/* symbol table */

Symbol symbol_table = NULL;

void insert_symbol_table(SymbolInfo symbol_info) {
  action("insert symbol %s", symbol_info->name);

  Symbol symbol = (Symbol)log_malloc(sizeof(struct SymbolItem));
  symbol->tail = symbol_table;
  symbol->symbol_info = symbol_info;
  symbol_table = symbol;
}

SymbolInfo find_symbol_table(int kind, const char *name) {
  action("find symbol %s", name);

  Symbol curr_symbol = symbol_table;
  while (curr_symbol) {
    if (kind == -1 || curr_symbol->symbol_info->kind == kind) {
      if (!strcmp(name, curr_symbol->symbol_info->name)) {
        return curr_symbol->symbol_info;
      }
    }
    curr_symbol = curr_symbol->tail;
  }
  return NULL;
}

static void print_type(Type type);
static void print_structure_type(FieldList field) {
  if (field == NULL) {
    return;
  }
  printf(" .%s\n", field->name);
  print_type(field->type);
  print_structure_type(field->tail);
}

static void print_type(Type type) {
  assert(type);
  switch (type->kind) {
  case BASIC:
    if (type->u.basic == _INT) {
      log("(int)");
    } else if (type->u.basic == _FLOAT) {
      log("(float)");
    } else {
      assert(0);
    }
    break;
  case ARRAY:
    printf("[%zu]", type->u.array.size);
    print_type(type->u.array.elem);
    break;
  case STRUCTURE:
    log("(structure)");
    print_structure_type(type->u.structure);
    log("(end structure)");
    break;
  default:
    assert(0);
    break;
  }
}

static void print_symbol_info(SymbolInfo symbol_info) {
  printf("==%s== ", symbol_info->name);
  switch (symbol_info->kind) {
  case TypeDef:
    log("[TypeDef]");
    print_type(symbol_info->info.type);
    break;
  case VariableInfo:
    log("[VariableInfo]");
    print_type(symbol_info->info.type);
    break;
  case FunctionInfo:
    log("[FunctionInfo]");
    log("{return type}");
    print_type(symbol_info->info.function.return_type);
    log("{arguments}");
    for (size_t i = 0; i < symbol_info->info.function.argument_count; ++i) {
      print_symbol_info(symbol_info->info.function.arguments[i]);
    }
    break;
  default:
    assert(0);
    break;
  }
}

static void print_symbol_table() {
  print("------------------------");
  Symbol curr_symbol = symbol_table;
  while (curr_symbol) {
    print_symbol_info(curr_symbol->symbol_info);
    print("------------------------");
    curr_symbol = curr_symbol->tail;
  }
}

/* type system */

static SymbolInfo int_type;
static SymbolInfo float_type;

static void build_naked_type() {
  // build int
  {
    int_type = (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
    int_type->kind = VariableInfo; // assume
    strcpy(int_type->name, "");    // useless

    Type type = (Type)log_malloc(sizeof(struct TypeItem));
    type->kind = BASIC;
    type->u.basic = _INT;
    int_type->info.type = type;
  }

  // build float
  {
    float_type = (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
    float_type->kind = VariableInfo; // assume
    strcpy(float_type->name, "");    // useless

    Type type = (Type)log_malloc(sizeof(struct TypeItem));
    type->kind = BASIC;
    type->u.basic = _FLOAT;
    float_type->info.type = type;
  }
}

static int check_type(Type lhs, Type rhs);
static int check_structure_type(FieldList lhs, FieldList rhs) {
  if (lhs == NULL && rhs == NULL) {
    return 1;
  }

  if (lhs == NULL || rhs == NULL) {
    return 0;
  }

  // TODO: structural equivalence
  return check_type(lhs->type, rhs->type) &&
         check_structure_type(lhs->tail, rhs->tail);
}

static int check_type(Type lhs, Type rhs) {
  if (lhs == NULL && rhs == NULL) {
    return 1;
  }

  if (lhs == NULL || rhs == NULL) {
    return 0;
  }

  if (lhs->kind != rhs->kind) {
    return 0;
  }

  if (lhs->kind == BASIC) {
    return lhs->u.basic == rhs->u.basic;
  }

  if (lhs->kind == ARRAY) {
    /* return lhs->u.array.size == rhs->u.array.size &&
     *        check_type(lhs->u.array.elem, rhs->u.array.elem); */
    // no need to check size
    return check_type(lhs->u.array.elem, rhs->u.array.elem);
  }

  if (lhs->kind == STRUCTURE) {
    return check_structure_type(lhs->u.structure, rhs->u.structure);
  }

  return 0;
}

static int is_basic_type(Type type, int attr) {
  if (!type) {
    return 0;
  }
  return type->kind == BASIC && type->u.basic == attr;
}

static int is_array_type(Type type) {
  if (!type) {
    return 0;
  }
  return type->kind == ARRAY;
}

static int is_structure_type(Type type) {
  if (!type) {
    return 0;
  }
  return type->kind == STRUCTURE;
}

static Type scan_structure_field(Type type, const char *name) {
  if (!type) {
    return NULL;
  }
  assert(type->kind == STRUCTURE);

  FieldList curr = type->u.structure;
  while (curr) {
    if (!strcmp(curr->name, name)) {
      return curr->type;
    }
    curr = curr->tail;
  }

  return NULL;
}

/* helper functions */

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

/* semantic analysis */

void variable_declaration(struct Ast *node, Type type,
                          SymbolInfo filled_symbol_info);
SymbolInfo specifier(struct Ast *node);
Type expression(struct Ast *node, int required_lval);

void declaration(struct Ast *node, Type type, SymbolInfo struct_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Dec);

  /* Dec -> VarDec */
  if (check_node(node, 1, _VarDec)) {
    variable_declaration(node->children[0], type, struct_symbol_info);
    return;
  }

  /* Dec -> VarDec ASSIGNOP Exp */
  if (check_node(node, 3, _VarDec, _ASSIGNOP, _Exp)) {
    if (struct_symbol_info != NULL) {
      // define structure with initialized field
      semantic_error(15, node->children[1]->lineno);
      return;
    } else {
      Type rhs = expression(node->children[2], 0);
      if (check_type(type, rhs)) {
        variable_declaration(node->children[0], type, struct_symbol_info);
      } else {
        // mismatch type
        semantic_error(5, node->children[0]->lineno);
      }
      return;
    }
  }

  assert(0);
  return;
}

void declaration_list(struct Ast *node, Type type,
                      SymbolInfo struct_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _DecList);

  /* DecList -> Dec */
  if (check_node(node, 1, _Dec)) {
    declaration(node->children[0], type, struct_symbol_info);
    return;
  }

  /* DecList -> Dec COMMA DecList */
  if (check_node(node, 3, _Dec, _COMMA, _DecList)) {
    declaration(node->children[0], type, struct_symbol_info);
    declaration_list(node->children[2], type, struct_symbol_info);
    return;
  }

  assert(0);
  return;
}

void definition(struct Ast *node, SymbolInfo struct_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Def);

  /* Def -> Specifier DecList SEMI */
  if (check_node(node, 3, _Specifier, _DecList, _SEMI)) {
    // **similar to external definition**
    SymbolInfo symbol_info = specifier(node->children[0]);
    assert(symbol_info);

    if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) {
        // like `struct A a;`
        SymbolInfo found_symbol_info =
            find_symbol_table(TypeDef, symbol_info->name);
        if (found_symbol_info == NULL) {
          // undefined structure
          semantic_error(17, node->children[0]->lineno);
        } else {
          declaration_list(node->children[1], found_symbol_info->info.type,
                           struct_symbol_info);
        }
      } else {
        // like `int a;` or `struct {...} a;`
        declaration_list(node->children[1], symbol_info->info.type,
                         struct_symbol_info);
      }
      return;
    }

    if (symbol_info->kind == TypeDef) { // like `struct A {...} a;`
      assert(symbol_info->info.type);
      assert(symbol_info->info.type->kind == STRUCTURE);
      if (find_symbol_table(-1, symbol_info->name) != NULL) {
        // -1 -> all name
        // redefine structure
        semantic_error(16, node->lineno);
        return;
      } else {
        // insert into symbol table and declare variables
        SymbolInfo copied_symbol_info =
            (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);
        declaration_list(node->children[1], symbol_info->info.type,
                         struct_symbol_info);
        return;
      }
    }
  }

  assert(0);
  return;
}

void definition_list(struct Ast *node, SymbolInfo struct_symbol_info) {
  /* StructSpecifier -> STRUCT OptTag LC DefList RC */
  /* CompSt -> LC DefList StmtList RC */

  info("hit %s", unit_names[node->type]);
  assert(node->type == _DefList);

  /* DefList -> Def EMPTY */
  if (check_node(node, 2, _Def, _EMPTY)) {
    definition(node->children[0], struct_symbol_info);
    info("hit EMPTY");
    return;
  }

  /* DefList -> Def DefList */
  if (check_node(node, 2, _Def, _DefList)) {
    definition(node->children[0], struct_symbol_info);
    definition_list(node->children[1], struct_symbol_info);
    return;
  }

  assert(0);
  return;
}

SymbolInfo struct_specifier(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _StructSpecifier);

  /* StructSpecifier -> STRUCT Tag */
  if (check_node(node, 2, _STRUCT, _Tag)) {
    SymbolInfo symbol_info =
        (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
    symbol_info->kind = VariableInfo; // assume
    strcpy(symbol_info->name,
           /* Tag -> ID */
           attr_table[node->children[1]->children[0]->attr_index]._string);

    symbol_info->info.type = NULL; // scan the table to find info
    return symbol_info;
  }

  /* StructSpecifier -> STRUCT OptTag LC DefList RC */
  if (node->children_count == 5) {
    SymbolInfo struct_symbol_info =
        (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));

    if (node->children[1]->type == _EMPTY) { // anonymous structure
      struct_symbol_info->kind = VariableInfo;
      strcpy(struct_symbol_info->name, ""); // useless
    } else if (node->children[1]->type == _OptTag) {
      struct_symbol_info->kind = TypeDef;
      strcpy(struct_symbol_info->name,
             /* OptTag -> ID */
             attr_table[node->children[1]->children[0]->attr_index]._string);
    } else {
      return NULL;
    }

    struct_symbol_info->info.type =
        (Type)log_malloc(sizeof(struct TypeItem)); // alloc space;
    struct_symbol_info->info.type->u.structure = NULL;
    struct_symbol_info->info.type->kind = STRUCTURE;
    definition_list(node->children[3], struct_symbol_info);

    return struct_symbol_info;
  }

  assert(0);
  return NULL;
}

SymbolInfo specifier(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Specifier);

  /* Specifier -> StructSpecifier */
  if (check_node(node, 1, _StructSpecifier)) {
    return struct_specifier(node->children[0]);
  }

  /* Specifier -> TYPE */
  if (check_node(node, 1, _TYPE)) {
    int attr = attr_table[node->children[0]->attr_index]._attr;
    if (attr == _INT) {
      return int_type;
    } else if (attr == _FLOAT) {
      return float_type;
    }
  }

  assert(0);
  return NULL;
}

void variable_declaration(struct Ast *node, Type type,
                          SymbolInfo filled_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _VarDec);
  assert(type);

  unsigned dimensions[MAX_BRACKET];
  size_t curr_dimension = 0;
  struct Ast *curr_node = node;

  while (curr_dimension < MAX_BRACKET) {
    if (curr_node->children_count == 4) {
      /* VarDec -> VarDec LB INT RB */
      assert(curr_node->children[2]->type == _INT); // subscripts are integers
      dimensions[curr_dimension] =
          attr_table[curr_node->children[2]->attr_index]._int;
      curr_node = curr_node->children[0];
      curr_dimension++;
    } else if (curr_node->children_count == 1) {
      /* VarDec -> ID */
      curr_node = curr_node->children[0];

      // scan the table first
      if (find_symbol_table(-1, attr_table[curr_node->attr_index]._string) !=
          NULL) {
        if (filled_symbol_info) {
          // TODO: multi-definition of field name
          semantic_error(15, curr_node->lineno);
          return;
        } else {
          // multi-definition of variables
          semantic_error(3, curr_node->lineno);
          return;
        }
      }

      // then backtrace
      Type tail_type = (Type)log_malloc(sizeof(struct TypeItem));
      memcpy(tail_type, type, sizeof(struct TypeItem));
      for (size_t i = 0; i < curr_dimension; ++i) {
        Type type = (Type)log_malloc(sizeof(struct TypeItem));
        type->kind = ARRAY;
        type->u.array.elem = tail_type;
        type->u.array.size = dimensions[i];
        tail_type = type;
      }

      SymbolInfo symbol_info =
          (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
      symbol_info->kind = VariableInfo;
      symbol_info->info.type = tail_type;
      strcpy(symbol_info->name, attr_table[curr_node->attr_index]._string);
      insert_symbol_table(symbol_info);

      // fill into structure or function
      if (filled_symbol_info) {
        if (filled_symbol_info->kind != FunctionInfo) {
          assert(filled_symbol_info->info.type);
          FieldList field =
              (FieldList)log_malloc(sizeof(struct FieldListItem));
          strcpy(field->name, attr_table[curr_node->attr_index]._string);
          Type struct_type = (Type)log_malloc(sizeof(struct TypeItem));
          memcpy(struct_type, tail_type, sizeof(struct TypeItem));
          field->type = struct_type;
          field->tail = filled_symbol_info->info.type->u.structure;
          filled_symbol_info->info.type->u.structure = field;
        } else {
          size_t index = filled_symbol_info->info.function.argument_count;
          filled_symbol_info->info.function.arguments[index] =
              (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
          memcpy(filled_symbol_info->info.function.arguments[index],
                 symbol_info, sizeof(struct SymbolInfoItem));
          filled_symbol_info->info.function.argument_count++;
        }
      }

      return;
    }
  }

  if (curr_dimension == MAX_BRACKET) {
    action("ignore transfinite dimensions");
    return;
  }

  assert(0);
  return;
}

void external_declaration_list(struct Ast *node, Type type) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDecList);
  assert(type);

  /* ExtDecList -> VarDec */
  if (check_node(node, 1, _VarDec)) {
    variable_declaration(node->children[0], type, NULL);
    return;
  }

  /* ExtDecList -> VarDec COMMA ExtDecList */
  if (check_node(node, 3, _VarDec, _COMMA, _ExtDecList)) {
    variable_declaration(node->children[0], type, NULL);
    external_declaration_list(node->children[2], type);
    return;
  }

  assert(0);
}

void parameter_declaration(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ParamDec);

  /* ParamDec -> Specifier VarDec */
  if (check_node(node, 2, _Specifier, _VarDec)) {
    // **similar to external definition**
    SymbolInfo symbol_info = specifier(node->children[0]);
    assert(symbol_info);

    if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) {
        // like `struct A a;`
        SymbolInfo found_symbol_info =
            find_symbol_table(TypeDef, symbol_info->name);
        if (found_symbol_info == NULL) {
          // undefined structure
          semantic_error(17, node->children[0]->lineno);
          return;
        } else {
          variable_declaration(node->children[1], found_symbol_info->info.type,
                               function_symbol_info);
          return;
        }
      } else {
        // like `int a;` or `struct {...} a;`
        variable_declaration(node->children[1], symbol_info->info.type,
                             function_symbol_info);
        return;
      }
    }

    if (symbol_info->kind == TypeDef) {
      // like `struct A {...} a;`
      assert(symbol_info->info.type);
      assert(symbol_info->info.type->kind == STRUCTURE);
      if (find_symbol_table(-1, symbol_info->name) != NULL) {
        // -1 -> all name
        // redefine structure
        semantic_error(16, node->lineno);
        return;
      } else {
        // insert into symbol table and declare parameters
        SymbolInfo copied_symbol_info =
            (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);
        variable_declaration(node->children[1], symbol_info->info.type,
                             function_symbol_info);
        return;
      }
    }
  }
}

void variable_list(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _VarList);

  /* VarList -> ParamDec */
  if (check_node(node, 1, _ParamDec)) {
    parameter_declaration(node->children[0], function_symbol_info);
    return;
  }

  /* VarList -> ParamDec COMMA VarList */
  if (check_node(node, 3, _ParamDec, _COMMA, _VarList)) {
    parameter_declaration(node->children[0], function_symbol_info);
    variable_list(node->children[2], function_symbol_info);
    return;
  }

  assert(0);
  return;
}

void function_declaration(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _FunDec);

  if (find_symbol_table(
          -1, attr_table[node->children[0]->attr_index]._string) != NULL) {
    // multi-definition of function name
    semantic_error(4, node->children[0]->lineno);
    return;
  }

  // fill function name
  strcpy(function_symbol_info->name,
         attr_table[node->children[0]->attr_index]._string);

  /* FunDec -> ID LP RP */
  if (check_node(node, 3, _ID, _LP, _RP)) {
    // do nothing
    return;
  }

  /* FunDec -> ID LP VarList RP */
  if (check_node(node, 4, _ID, _LP, _VarList, _RP)) {
    variable_list(node->children[2], function_symbol_info);
    return;
  }

  assert(0);
  return;
}

Type arguments(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Args);

  /* Args : Exp COMMA Args
   *      | Exp */

  // check count
  {
    size_t actual_count = 0;
    struct Ast *curr_node = node;
    while (curr_node->children_count == 3) {
      curr_node = curr_node->children[2];
      actual_count++;
    }
    actual_count++;

    if (actual_count != function_symbol_info->info.function.argument_count) {
      /* mismatch count or type */
      semantic_error(9, node->lineno);
      return NULL;
    }
  }

  // check type
  {
    size_t curr_index = 0;
    struct Ast *curr_node = node;
    while (curr_node->children_count == 3) {
      Type lhs = expression(curr_node->children[0], 0);
      Type rhs =
          function_symbol_info->info.function.arguments[curr_index]->info.type;
      if (!check_type(lhs, rhs)) {
        /* mismatch count or type */
        semantic_error(9, node->lineno);
        return NULL;
      }
      curr_index++;
      curr_node = curr_node->children[2];
    }

    Type lhs = expression(curr_node->children[0], 0);
    Type rhs =
        function_symbol_info->info.function.arguments[curr_index]->info.type;
    if (!check_type(lhs, rhs)) {
      /* mismatch count or type */
      semantic_error(9, node->lineno);
      return NULL;
    }

    return function_symbol_info->info.function.return_type;
  }
}

Type expression(struct Ast *node, int required_lval) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Exp);
  /* Exp : Exp ASSIGNOP Exp
   *     | Exp AND Exp
   *     | Exp OR Exp
   *     | Exp RELOP Exp
   *     | Exp PLUS Exp
   *     | Exp MINUS Exp
   *     | Exp STAR Exp
   *     | Exp DIV Exp
   *     | LP Exp RP
   *     | MINUS Exp
   *     | NOT Exp
   *     | ID LP Args RP
   *     | ID LP RP
   *     | Exp LB Exp RB
   *     | Exp DOT ID
   *     | ID
   *     | INT
   *     | FLOAT */

  // check lval
  if (required_lval) {
    if (!check_node(node, 1, _ID) && !check_node(node, 3, _Exp, _DOT, _ID) &&
        !check_node(node, 4, _Exp, _LB, _Exp, _RB)) {
      semantic_error(6, node->lineno);
      return NULL;
    }
  }

  // handle various situations

  // identifier
  if (check_node(node, 1, _ID)) {
    SymbolInfo symbol_info = find_symbol_table(
        VariableInfo, attr_table[node->children[0]->attr_index]._string);
    if (symbol_info == NULL) {
      semantic_error(1, node->children[0]->lineno);
      return NULL;
    }
    return symbol_info->info.type;
  }

  // function call
  if (check_node(node, 3, _ID, _LP, _RP) ||
      check_node(node, 4, _ID, _LP, _Args, _RP)) {
    SymbolInfo symbol_info = find_symbol_table(
        -1, attr_table[node->children[0]->attr_index]._string);
    if (symbol_info == NULL) {
      semantic_error(2, node->children[0]->lineno);
      return NULL;
    }
    if (symbol_info->kind == VariableInfo) {
      // variable is not function
      semantic_error(11, node->children[0]->lineno);
      return NULL;
    }
    if (symbol_info->kind == TypeDef) {
      action("ignore like `struct A {...}; A();`");
      return NULL;
    }

    if (node->children_count == 4) {
      // function call with argument
      // return NULL, if fail
      return arguments(node->children[2], symbol_info);
    } else if (node->children_count == 3) {
      // function call without argument
      if (symbol_info->info.function.argument_count != 0) {
        // mismatch count or type
        semantic_error(9, node->children[0]->lineno);
        return NULL;
      } else {
        return symbol_info->info.function.return_type;
      }
    }

    return NULL;
  }

  // literal
  if (check_node(node, 1, _INT)) {
    return int_type->info.type;
  }

  // literals
  if (check_node(node, 1, _FLOAT)) {
    return float_type->info.type;
  }

  // ()
  if (check_node(node, 3, _LP, _Exp, _RP)) {
    return expression(node->children[1], 0);
  }

  // unary
  if (check_node(node, 2, _MINUS, _Exp)) {
    Type rhs = expression(node->children[1], 0);
    if (!is_basic_type(rhs, _INT) && !is_basic_type(rhs, _FLOAT)) {
      // mismatch operator and operand
      semantic_error(7, node->children[0]->lineno);
      return NULL;
    }
    return rhs;
  }

  // unary
  if (check_node(node, 2, _NOT, _Exp)) {
    Type rhs = expression(node->children[1], 0);
    if (!is_basic_type(rhs, _INT)) {
      // mismatch operator and operand
      semantic_error(7, node->children[0]->lineno);
      return NULL;
    }
    return rhs;
  }

  // array
  if (check_node(node, 4, _Exp, _LB, _Exp, _RB)) {
    Type arr = expression(node->children[0], 0);
    if (!is_array_type(arr)) {
      // array access operator for non-array variables
      semantic_error(10, node->children[0]->lineno);
      return NULL;
    }

    Type index = expression(node->children[2], 0);
    if (!is_basic_type(index, _INT)) {
      // array index is not integers
      semantic_error(12, node->children[2]->lineno);
      return NULL;
    }

    // modify type
    return arr->u.array.elem;
  }

  // structure
  if (check_node(node, 3, _Exp, _DOT, _ID)) {
    Type structure = expression(node->children[0], 0);
    if (!is_structure_type(structure)) {
      // field access operator for non-structure variables
      semantic_error(13, node->children[0]->lineno);
      return NULL;
    }

    // assume no need to scan symbol table
    Type res = scan_structure_field(
        structure, attr_table[node->children[2]->attr_index]._string);
    if (res == NULL) {
      // access undefined field
      semantic_error(14, node->children[2]->lineno);
    }

    return res;
  }

  // binary
  if (node->children_count == 3 && node->children[0]->type == _Exp &&
      node->children[2]->type == _Exp) {
    Type lhs = expression(node->children[0], 0);
    Type rhs = expression(node->children[2], 0);
    switch (node->children[1]->type) {
    case _AND:
    case _OR:
      if (!is_basic_type(lhs, _INT) || !is_basic_type(rhs, _INT)) {
        // mismatch operator and operand
        semantic_error(7, node->children[1]->lineno);
        return NULL;
      }
      // assume
      return lhs;
      break;
    case _RELOP:
    case _PLUS:
    case _MINUS:
    case _STAR:
    case _DIV:
      if ((is_basic_type(lhs, _INT) && is_basic_type(rhs, _INT)) ||
          (is_basic_type(lhs, _FLOAT) && is_basic_type(rhs, _FLOAT))) {
        // assume
        return lhs;
      } else {
        // mismatch operator and operand
        semantic_error(7, node->children[1]->lineno);
        return NULL;
      }
      break;
    case _ASSIGNOP:
      lhs = expression(node->children[0], 1); // required lval
      if (!check_type(lhs, rhs)) {
        // mismatch type
        semantic_error(5, node->children[1]->lineno);
      }
      return lhs;
      break;
    default:
      break;
    }
  }

  return NULL;
}

void compound_statement(struct Ast *node, SymbolInfo function_symbol_info);
void statement(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Stmt);
  /* Stmt : Exp SEMI
   *      | CompSt
   *      | RETURN Exp SEMI
   *      | IF LP Exp RP Stmt
   *      | IF LP Exp RP Stmt ELSE Stmt
   *      | WHILE LP Exp RP Stmt */
  if (check_node(node, 2, _Exp, _SEMI)) {
    expression(node->children[0], 0);
    return;
  }

  if (check_node(node, 1, _CompSt)) {
    compound_statement(node->children[0], function_symbol_info);
    return;
  }

  if (check_node(node, 3, _RETURN, _Exp, _SEMI)) {
    Type return_type = expression(node->children[1], 0);
    if (!check_type(return_type,
                    function_symbol_info->info.function.return_type)) {
      // mismatch return type
      semantic_error(8, node->children[0]->lineno);
    }
    return;
  }

  if (check_node(node, 5, _IF, _LP, _Exp, _RP, _Stmt)) {
    Type cond = expression(node->children[2], 0);
    if (!is_basic_type(cond, _INT)) {
      // action("ignore like `if (1.0) {...}`");
      semantic_error(7, node->children[2]->lineno);
    }
    statement(node->children[4], function_symbol_info);
    return;
  }

  if (check_node(node, 7, _IF, _LP, _Exp, _RP, _Stmt, _ELSE, _Stmt)) {
    Type cond = expression(node->children[2], 0);
    if (!is_basic_type(cond, _INT)) {
      // action("ignore like `if (1.0) {...} else {...}`");
      semantic_error(7, node->children[2]->lineno);
    }
    statement(node->children[4], function_symbol_info);
    statement(node->children[6], function_symbol_info);
    return;
  }

  if (check_node(node, 5, _WHILE, _LP, _Exp, _RP, _Stmt)) {
    Type cond = expression(node->children[2], 0);
    if (!is_basic_type(cond, _INT)) {
      // action("ignore like `while (1.0) {...}`");
      semantic_error(7, node->children[2]->lineno);
    }
    statement(node->children[4], function_symbol_info);
    return;
  }

  return;
}

void statement_list(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _StmtList);

  /* StmtList -> Stmt EMPTY */
  if (check_node(node, 2, _Stmt, _EMPTY)) {
    statement(node->children[0], function_symbol_info);
    info("hit EMPTY");
    return;
  }

  /* StmtList -> Stmt StmtList */
  if (check_node(node, 2, _Stmt, _StmtList)) {
    statement(node->children[0], function_symbol_info);
    statement_list(node->children[1], function_symbol_info);
    return;
  }

  assert(0);
  return;
}

void compound_statement(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _CompSt);

  /* CompSt -> LC DefList StmtList RC */
  assert(node->children_count == 4);
  if (node->children[1]->type != _EMPTY) {
    definition_list(node->children[1], NULL); // no structure info
  }

  if (node->children[2]->type != _EMPTY) {
    statement_list(node->children[2], function_symbol_info);
  }

  return;
}

static void construct_insert_function_compst(struct Ast *node,
                                             Type return_type) {
  SymbolInfo function_symbol_info =
      (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
  function_symbol_info->kind = FunctionInfo;
  strcpy(function_symbol_info->name,
         ""); // filled when in function declaration

  function_symbol_info->info.function.return_type =
      (Type)log_malloc(sizeof(struct TypeItem));
  // serve as return type of function
  memcpy(function_symbol_info->info.function.return_type, return_type,
         sizeof(struct TypeItem));

  function_symbol_info->info.function.argument_count = 0;
  function_declaration(node->children[1], function_symbol_info);

  // if success:
  // 1. insert into symbol table
  // 2. enter CompSt
  if (strcmp("", function_symbol_info->name)) {
    insert_symbol_table(function_symbol_info);
    compound_statement(node->children[2], function_symbol_info);
  }
}

void external_definition(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDef);

  /* ExtDef -> Specifier SEMI */
  if (check_node(node, 2, _Specifier, _SEMI)) {
    SymbolInfo symbol_info = specifier(node->children[0]);
    assert(symbol_info);
    if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) {
        // ignore like `struct A;`
        action("ignore like `struct A;`");
        return;
      } else {
        if (symbol_info->info.type->kind == BASIC) {
          // ignore like `int;`
          action("ignore like `int;`");
          return;
        } else if (symbol_info->info.type->kind == STRUCTURE) {
          // ignore like `struct {...};`
          action("ignore like `struct {...};`");
          return;
        }
      }
    } else if (symbol_info->kind == TypeDef) {
      // like `struct A {...};`
      assert(symbol_info->info.type);
      assert(symbol_info->info.type->kind == STRUCTURE);
      if (find_symbol_table(-1, symbol_info->name) != NULL) {
        // -1 -> all name
        // redefine structure
        semantic_error(16, node->lineno);
        return;
      } else {
        // insert into symbol table
        SymbolInfo copied_symbol_info =
            (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);
        return;
      }
    }
  }

  /* ExtDef -> Specifier ExtDecList SEMI */
  if (check_node(node, 3, _Specifier, _ExtDecList, _SEMI)) {
    SymbolInfo symbol_info = specifier(node->children[0]);
    assert(symbol_info);
    if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) { // like `struct A a;`
        SymbolInfo found_symbol_info =
            find_symbol_table(TypeDef, symbol_info->name);
        if (found_symbol_info == NULL) {
          // undefined structure
          semantic_error(17, node->children[0]->lineno);
          return;
        } else {
          external_declaration_list(node->children[1],
                                    found_symbol_info->info.type);
          return;
        }
      } else { // like `int a;` or `struct {...} a;`
        external_declaration_list(node->children[1], symbol_info->info.type);
        return;
      }
    } else if (symbol_info->kind == TypeDef) { // like `struct A {...} a;`
      assert(symbol_info->info.type);
      assert(symbol_info->info.type->kind == STRUCTURE);
      if (find_symbol_table(-1, symbol_info->name) != NULL) {
        // -1 -> all name
        // redefine structure
        semantic_error(16, node->lineno);
        return;
      } else {
        // insert into symbol table and declare variables
        SymbolInfo copied_symbol_info =
            (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);
        external_declaration_list(node->children[1], symbol_info->info.type);
        return;
      }
    }
  }

  /* ExtDef -> Specifier FunDec CompSt */
  if (check_node(node, 3, _Specifier, _FunDec, _CompSt)) {
    SymbolInfo symbol_info = specifier(node->children[0]);
    assert(symbol_info);
    if (symbol_info->kind == TypeDef) { // like `struct A {...} a;`
      assert(symbol_info->info.type);
      assert(symbol_info->info.type->kind == STRUCTURE);
      if (find_symbol_table(-1, symbol_info->name) != NULL) {
        // -1 -> all name
        // redefine structure
        semantic_error(16, node->lineno);
        return;
      } else {
        // insert into symbol table
        SymbolInfo copied_symbol_info =
            (SymbolInfo)log_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);

        construct_insert_function_compst(node, symbol_info->info.type);
        return;
      }
    } else if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) { // like `struct A a;`
        SymbolInfo found_symbol_info =
            find_symbol_table(TypeDef, symbol_info->name);
        if (found_symbol_info == NULL) {
          // undefined structure
          semantic_error(17, node->children[0]->lineno);
          return;
        } else {
          construct_insert_function_compst(node, found_symbol_info->info.type);
          return;
        }
      } else { // like `int a;` or `struct {...} a;`
        construct_insert_function_compst(node, symbol_info->info.type);
        return;
      }
    }
  }

  assert(0);
  return;
}

void external_definition_list(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDefList);

  if (check_node(node, 2, _ExtDef, _EMPTY)) {
    external_definition(node->children[0]);
    info("hit EMPTY");
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

void entry(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Program);

  if (check_node(node, 1, _ExtDefList)) {
    external_definition_list(node->children[0]);
    return;
  }

  assert(0);
  return;
}

void analysis(struct Ast *root) {
  build_naked_type();
  entry(root);
  print_symbol_table();
}
