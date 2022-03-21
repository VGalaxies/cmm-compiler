#include "common.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* error handler */

int semantic_errors = 0;
static void semantic_error(int type, int lineno) {
  ++semantic_errors;
  panic("Error type %d at Line %d.", type, lineno);
}

/* memory management */

#define MAX_MALLOC 10240
static void *semantic_malloc_table[MAX_MALLOC];
static size_t semantic_malloc_table_index = 0;

void *log_semantic_malloc(size_t size) {
  void *res = malloc(size);
  semantic_malloc_table[semantic_malloc_table_index] = res;
  ++semantic_malloc_table_index;
  return res;
}

void clear_semantic_malloc_table() {
  action("total %zu allocation(s)", semantic_malloc_table_index);
  for (size_t i = 0; i < semantic_malloc_table_index; ++i) {
    free(semantic_malloc_table[i]);
  }
}

/* symbol table */

Symbol symbol_table = NULL;

void insert_symbol_table(SymbolInfo symbol_info) {
  action("insert symbol %s", symbol_info->name);

  Symbol symbol = (Symbol)log_semantic_malloc(sizeof(struct SymbolItem));
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

void print_type(Type type);
void print_structure_type(FieldList field) {
  if (field == NULL) {
    return;
  }
  printf(" .%s\n", field->name);
  print_type(field->type);
  print_structure_type(field->tail);
}

void print_type(Type type) {
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

void print_symbol_info(SymbolInfo symbol_info) {
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

void print_symbol_table() {
  print("------------------------");
  Symbol curr_symbol = symbol_table;
  while (curr_symbol) {
    print_symbol_info(curr_symbol->symbol_info);
    print("------------------------");
    curr_symbol = curr_symbol->tail;
  }
}

/* semantic analysis */

void variable_declaration(struct Ast *node, Type type,
                          SymbolInfo filled_symbol_info);
SymbolInfo specifier(struct Ast *node);

void declaration(struct Ast *node, Type type, SymbolInfo struct_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Dec);

  if (node->children_count == 1) {
    /* Dec -> VarDec */
    variable_declaration(node->children[0], type, struct_symbol_info);
    return;
  } else if (node->children_count == 3) {
    /* Dec -> VarDec ASSIGNOP Exp */
    if (struct_symbol_info != NULL) {
      // define structure with initialized field
      semantic_error(15, node->children[1]->lineno);
      return;
    } else {
      // TODO: check type
      variable_declaration(node->children[0], type, struct_symbol_info);
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

  if (node->children_count == 1) {
    /* DecList -> Dec */
    declaration(node->children[0], type, struct_symbol_info);
    return;
  } else if (node->children_count == 3) {
    /* DecList -> Dec COMMA DecList */
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
  assert(node->children_count == 3);

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
          (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
      memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
      insert_symbol_table(copied_symbol_info);
      declaration_list(node->children[1], symbol_info->info.type,
                       struct_symbol_info);
      return;
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
  /* DefList -> Def DefList | Def EMPTY */
  assert(node->children_count == 2);

  if (node->children[1]->type == _EMPTY) {
    definition(node->children[0], struct_symbol_info);
    info("hit EMPTY");
    return;
  } else if (node->children[1]->type == _DefList) {
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

  if (node->children_count == 2) {
    /* StructSpecifier -> STRUCT Tag */
    SymbolInfo symbol_info =
        (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
    symbol_info->kind = VariableInfo; // assume
    strcpy(symbol_info->name,
           /* Tag -> ID */
           attr_table[node->children[1]->children[0]->attr_index]._string);

    symbol_info->info.type = NULL; // scan the table to find info
    return symbol_info;
  } else if (node->children_count == 5) {
    /* StructSpecifier -> STRUCT OptTag LC DefList RC */
    SymbolInfo struct_symbol_info =
        (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
    if (node->children[1]->type == _EMPTY) { // anonymous structure
      struct_symbol_info->kind = VariableInfo;
      strcpy(struct_symbol_info->name, ""); // useless
    } else {
      struct_symbol_info->kind = TypeDef;
      strcpy(struct_symbol_info->name,
             /* OptTag -> ID */
             attr_table[node->children[1]->children[0]->attr_index]._string);
    }

    struct_symbol_info->info.type =
        (Type)log_semantic_malloc(sizeof(struct TypeItem)); // alloc space;
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
  assert(node->children_count == 1);

  struct Ast *child = node->children[0];
  if (child->type == _StructSpecifier) {
    /* Specifier -> StructSpecifier */
    return struct_specifier(child);
  } else if (child->type == _TYPE) {
    /* Specifier -> TYPE */
    SymbolInfo symbol_info =
        (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
    symbol_info->kind = VariableInfo; // assume
    strcpy(symbol_info->name, "");    // useless

    Type type = (Type)log_semantic_malloc(sizeof(struct TypeItem));
    type->kind = BASIC;
    type->u.basic = attr_table[child->attr_index]._attr;
    symbol_info->info.type = type;
    return symbol_info;
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
      Type tail_type = (Type)log_semantic_malloc(sizeof(struct TypeItem));
      memcpy(tail_type, type, sizeof(struct TypeItem));
      for (size_t i = 0; i < curr_dimension; ++i) {
        Type type = (Type)log_semantic_malloc(sizeof(struct TypeItem));
        type->kind = ARRAY;
        type->u.array.elem = tail_type;
        type->u.array.size = dimensions[i];
        tail_type = type;
      }

      SymbolInfo symbol_info =
          (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
      symbol_info->kind = VariableInfo;
      symbol_info->info.type = tail_type;
      strcpy(symbol_info->name, attr_table[curr_node->attr_index]._string);
      insert_symbol_table(symbol_info);

      // fill into structure or function
      if (filled_symbol_info) {
        if (filled_symbol_info->kind != FunctionInfo) {
          assert(filled_symbol_info->info.type);
          FieldList field =
              (FieldList)log_semantic_malloc(sizeof(struct FieldListItem));
          strcpy(field->name, attr_table[curr_node->attr_index]._string);
          Type struct_type = (Type)log_semantic_malloc(sizeof(struct TypeItem));
          memcpy(struct_type, tail_type, sizeof(struct TypeItem));
          field->type = struct_type;
          field->tail = filled_symbol_info->info.type->u.structure;
          filled_symbol_info->info.type->u.structure = field;
        } else {
          size_t index = filled_symbol_info->info.function.argument_count;
          filled_symbol_info->info.function.arguments[index] =
              (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
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
}

void external_declaration_list(struct Ast *node, Type type) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDecList);
  assert(type);

  if (node->children_count == 1) {
    /* ExtDecList -> VarDec */
    variable_declaration(node->children[0], type, NULL);
    return;
  } else if (node->children_count == 3) {
    /* ExtDecList -> VarDec COMMA ExtDecList */
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
  assert(node->children_count == 2);

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
        variable_declaration(node->children[1], found_symbol_info->info.type,
                             function_symbol_info);
      }
    } else {
      // like `int a;` or `struct {...} a;`
      variable_declaration(node->children[1], symbol_info->info.type,
                           function_symbol_info);
    }
    return;
  } else if (symbol_info->kind == TypeDef) {
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
          (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
      memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
      insert_symbol_table(copied_symbol_info);
      variable_declaration(node->children[1], symbol_info->info.type,
                           function_symbol_info);
      return;
    }
  }
}

void variable_list(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _VarList);

  if (node->children_count == 1) {
    /* VarList -> VarList */
    parameter_declaration(node->children[0], function_symbol_info);
    return;
  } else if (node->children_count == 3) {
    /* VarList -> ParamDec COMMA VarList */
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
  if (node->children_count == 3) {
    /* FunDec -> ID LP RP */
    // do nothing
    return;
  } else if (node->children_count == 4) {
    /* FunDec -> ID LP VarList RP */
    variable_list(node->children[2], function_symbol_info);
    return;
  }

  assert(0);
  return;
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
  if (node->children[0]->type == _Exp) {
    expression(node->children[0], 0);
    return;
  }

  if (node->children[0]->type == _CompSt) {
    compound_statement(node->children[0], function_symbol_info);
    return;
  }

  if (node->children[0]->type == _RETURN) {
  }

  assert(0);
  return;
}

void statement_list(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _CompSt);
  /* StmtList -> Stmt StmtList | Stmt EMPTY */
  assert(node->children_count == 2);

  if (node->children[1]->type == _EMPTY) {
    statement(node->children[0], function_symbol_info);
    return;
  } else if (node->children[1]->type == _StmtList) {
    statement(node->children[0], function_symbol_info);
    statement_list(node->children[1], function_symbol_info);
  }

  assert(0);
  return;
}

void compound_statement(struct Ast *node, SymbolInfo function_symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _CompSt);
  /* CompSt -> LC DefList StmtList RC */
  assert(node->children_count == 4);

  definition_list(node->children[1], NULL);
  statement_list(node->children[2], function_symbol_info);
  return;
}

void external_definition(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDef);

  if (node->children_count == 2) {
    /* ExtDef -> Specifier SEMI */
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
            (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
        memcpy(copied_symbol_info, symbol_info, sizeof(struct SymbolInfoItem));
        insert_symbol_table(copied_symbol_info);
        return;
      }
    }
  } else if (node->children_count == 3) {
    if (node->children[2]->type == _SEMI) {
      /* ExtDef -> Specifier ExtDecList SEMI */
      SymbolInfo symbol_info = specifier(node->children[0]);
      assert(symbol_info);
      if (symbol_info->kind == VariableInfo) {
        if (symbol_info->info.type == NULL) { // like `struct A a;`
          SymbolInfo found_symbol_info =
              find_symbol_table(TypeDef, symbol_info->name);
          if (found_symbol_info == NULL) {
            // undefined structure
            semantic_error(17, node->children[0]->lineno);
          } else {
            external_declaration_list(node->children[1],
                                      found_symbol_info->info.type);
          }
        } else { // like `int a;` or `struct {...} a;`
          external_declaration_list(node->children[1], symbol_info->info.type);
        }
        return;
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
              (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
          memcpy(copied_symbol_info, symbol_info,
                 sizeof(struct SymbolInfoItem));
          insert_symbol_table(copied_symbol_info);
          external_declaration_list(node->children[1], symbol_info->info.type);
          return;
        }
      }
    } else if (node->children[2]->type == _CompSt) {
      /* ExtDef -> Specifier FunDec CompSt */
      SymbolInfo symbol_info = specifier(node->children[0]);
      assert(symbol_info);
      if (symbol_info->kind == TypeDef) { // like `struct A {...} a;`
        assert(symbol_info->info.type);
        assert(symbol_info->info.type->kind == STRUCTURE);
        if (find_symbol_table(-1, symbol_info->name) != NULL) {
          // -1 -> all name
          // redefine structure
          semantic_error(16, node->lineno);
        } else {
          // insert into symbol table
          SymbolInfo copied_symbol_info =
              (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
          memcpy(copied_symbol_info, symbol_info,
                 sizeof(struct SymbolInfoItem));
          insert_symbol_table(copied_symbol_info);

          // serve as return type of function
          SymbolInfo function_symbol_info =
              (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
          function_symbol_info->kind = FunctionInfo;
          strcpy(function_symbol_info->name,
                 ""); // filled when in function declaration
          function_symbol_info->info.function.return_type =
              (Type)log_semantic_malloc(sizeof(struct TypeItem));
          memcpy(function_symbol_info->info.function.return_type,
                 symbol_info->info.type, sizeof(struct TypeItem));
          function_symbol_info->info.function.argument_count = 0;
          function_declaration(node->children[1], function_symbol_info);

          // insert into symbol table
          if (strcmp("", function_symbol_info->name)) {
            insert_symbol_table(function_symbol_info);
          }
        }
        return;
      } else if (symbol_info->kind == VariableInfo) {
        if (symbol_info->info.type == NULL) { // like `struct A a;`
          SymbolInfo found_symbol_info =
              find_symbol_table(TypeDef, symbol_info->name);
          if (found_symbol_info == NULL) {
            // undefined structure
            semantic_error(17, node->children[0]->lineno);
          } else {
            // serve as return type of function
            SymbolInfo function_symbol_info =
                (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
            function_symbol_info->kind = FunctionInfo;
            strcpy(function_symbol_info->name,
                   ""); // filled when in function declaration
            function_symbol_info->info.function.return_type =
                (Type)log_semantic_malloc(sizeof(struct TypeItem));
            memcpy(function_symbol_info->info.function.return_type,
                   found_symbol_info->info.type, sizeof(struct TypeItem));
            function_symbol_info->info.function.argument_count = 0;
            function_declaration(node->children[1], function_symbol_info);

            // insert into symbol table
            if (strcmp("", function_symbol_info->name)) {
              insert_symbol_table(function_symbol_info);
            }
          }
        } else { // like `int a;` or `struct {...} a;`
          // serve as return type of function
          SymbolInfo function_symbol_info =
              (SymbolInfo)log_semantic_malloc(sizeof(struct SymbolInfoItem));
          function_symbol_info->kind = FunctionInfo;
          strcpy(function_symbol_info->name,
                 ""); // filled when in function declaration
          function_symbol_info->info.function.return_type =
              (Type)log_semantic_malloc(sizeof(struct TypeItem));
          memcpy(function_symbol_info->info.function.return_type,
                 symbol_info->info.type, sizeof(struct TypeItem));
          function_symbol_info->info.function.argument_count = 0;
          function_declaration(node->children[1], function_symbol_info);

          // insert into symbol table
          if (strcmp("", function_symbol_info->name)) {
            insert_symbol_table(function_symbol_info);
          }
        }
        return;
      }
    }
  }

  assert(0);
}

void external_definition_list(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDefList);

  for (size_t i = 0; i < node->children_count; ++i) {
    struct Ast *child = node->children[i];
    switch (child->type) {
    case _ExtDef:
      external_definition(child);
      break;
    case _ExtDefList:
      external_definition_list(child);
      break;
    case _EMPTY:
      info("hit EMPTY");
      break;
    default:
      break;
    }
  }
}

void entry(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _Program);
  assert(node->children_count == 1);

  external_definition_list(node->children[0]);
}

void analysis(struct Ast *root) {
  entry(root);
  print_symbol_table();
  clear_semantic_malloc_table();
}
