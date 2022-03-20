#include "common.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int semantic_errors = 0;
static void semantic_error(int type, int lineno) {
  ++semantic_errors;
  panic("Error type %d at Line %d.", type, lineno);
}

Symbol symbol_table = NULL;

void insert_symbol_table(SymbolInfo symbol_info) {
  Symbol symbol = (Symbol)malloc(sizeof(struct SymbolItem));
  symbol->tail = symbol_table;
  symbol->symbol_info = symbol_info;
  symbol_table = symbol;
}

SymbolInfo find_symbol_table(int kind, const char *name) {
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

void free_type(Type type);
void free_field_list(FieldList field_list) {
  if (field_list == NULL) {
    return;
  }
  if (field_list->type) {
    free_type(field_list->type);
  }
  free_field_list(field_list->tail);
  field_list = NULL;
}

void free_type(Type type) {
  assert(type);
  switch (type->kind) {
  case BASIC:
    free(type);
    break;
  case ARRAY:
    if (type->u.array.elem) {
      free_type(type->u.array.elem);
    }
    break;
  case STRUCTURE:
    if (type->u.structure) {
      free_field_list(type->u.structure);
    }
    break;
  default:
    assert(0);
    break;
  }
  type = NULL;
}

void free_symbol_type(SymbolInfo symbol_info) {
  assert(symbol_info);
  assert(symbol_info->kind == TypeDef || symbol_info->kind == VariableInfo);
  if (symbol_info->info.type) {
    free_type(symbol_info->info.type);
  }
  free(symbol_info);
  symbol_info = NULL;
}

void definition_list(struct Ast *node) {
  // TODO: use global variables
  assert(0);
  return;
}

SymbolInfo struct_specifier(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _StructSpecifier);

  if (node->children_count == 2) {
    /* StructSpecifier -> STRUCT Tag */
    SymbolInfo symbol_info = (SymbolInfo)malloc(sizeof(struct SymbolInfoItem));
    symbol_info->kind = VariableInfo; // assume
    strcpy(symbol_info->name,
           /* Tag -> ID */
           attr_table[node->children[1]->children[0]->attr_index]._string);

    symbol_info->info.type = NULL; // scan the table to find info
    return symbol_info;
  } else if (node->children_count == 5) {
    /* StructSpecifier -> STRUCT OptTag LC DefList RC */
    SymbolInfo symbol_info = (SymbolInfo)malloc(sizeof(struct SymbolInfoItem));
    if (node->children[1]->type == _EMPTY) { // anonymous structure
      symbol_info->kind = VariableInfo;
      strcpy(symbol_info->name, ""); // fill in declaration
    } else {
      symbol_info->kind = TypeDef;
      strcpy(symbol_info->name,
             /* OptTag -> ID */
             attr_table[node->children[1]->children[0]->attr_index]._string);
    }

    definition_list(node->children[3]);
    return symbol_info;
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
    SymbolInfo symbol_info = (SymbolInfo)malloc(sizeof(struct SymbolInfoItem));
    symbol_info->kind = VariableInfo; // assume
    strcpy(symbol_info->name, "");    // fill in declaration

    Type type = (Type)malloc(sizeof(struct TypeItem));
    type->kind = BASIC;
    type->u.basic = attr_table[child->attr_index]._attr;
    symbol_info->info.type = type;
    return symbol_info;
  }

  assert(0);
  return NULL;
}

void variable_declaration(struct Ast *node, SymbolInfo symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _VarDec);

  struct Ast *nodes[MAX_BRACKET];
  unsigned dimensions[MAX_BRACKET];
  size_t curr_dimension = 0;
  nodes[curr_dimension] = node;

  while (curr_dimension < MAX_BRACKET) {
    if (nodes[curr_dimension]->children_count == 4) {
      /* VarDec -> VarDec LB INT RB */
      if (nodes[curr_dimension]->children[2]->type != _INT) {
        // subscripts are not integers
        semantic_error(12, nodes[curr_dimension]->children[2]->lineno);
        return;
      }
      dimensions[curr_dimension] =
          attr_table[nodes[curr_dimension]->children[2]->attr_index]._int;
      nodes[curr_dimension + 1] = nodes[curr_dimension]->children[0];
      curr_dimension++;
    } else if (nodes[curr_dimension]->children_count == 1) {
      /* VarDec -> ID */
      // backtrace
      Type tail_type = (Type)malloc(sizeof(struct TypeItem));
      memcpy(tail_type, symbol_info->info.type, sizeof(struct TypeItem));
      for (size_t i = 0; i < curr_dimension; ++i) {
        Type type = (Type)malloc(sizeof(struct TypeItem));
        type->kind = ARRAY;
        type->u.array.elem = tail_type;
        type->u.array.size = dimensions[i];
        tail_type = type;
      }
      SymbolInfo symbol_info =
          (SymbolInfo)malloc(sizeof(struct SymbolInfoItem));
      symbol_info->kind = VariableInfo;
      symbol_info->info.type = tail_type;
      strcpy(symbol_info->name,
             attr_table[nodes[curr_dimension]->attr_index]._string);
      insert_symbol_table(symbol_info);
      return;
    }
  }

  if (curr_dimension == MAX_BRACKET) {
    info("ignore transfinite dimensions");
    return;
  }

  assert(0);
}

void external_declaration_list(struct Ast *node, SymbolInfo symbol_info) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDecList);

  if (node->children_count == 1) {
    /* ExtDecList -> VarDec */
    variable_declaration(node->children[0], symbol_info);
    return;
  } else if (node->children_count == 3) {
    /* ExtDecList -> VarDec COMMA ExtDecList */
    variable_declaration(node->children[0], symbol_info);
    external_declaration_list(node->children[2], symbol_info);
    return;
  }

  assert(0);
}

void external_definition(struct Ast *node) {
  info("hit %s", unit_names[node->type]);
  assert(node->type == _ExtDef);

  if (node->children_count == 2) {
    /* ExtDef -> Specifier SEMI */
    SymbolInfo symbol_info = specifier(node->children[0]);
    if (symbol_info->kind == VariableInfo) {
      if (symbol_info->info.type == NULL) { // ignore like `struct A;`
        info("ignore like `struct A;`");
        free_symbol_type(symbol_info);
        return;
      } else {
        if (symbol_info->info.type->kind == BASIC) { // ignore like `int;`
          info("ignore like `int;`");
          free_symbol_type(symbol_info);
          return;
        }
      }
    } else if (symbol_info->kind == TypeDef) {
      assert(symbol_info->info.type);
      int kind = symbol_info->info.type->kind;
      if (kind == STRUCTURE) {
        if (find_symbol_table(-1, symbol_info->name) != NULL) {
          // -1 -> all name
          // redefine structure
          semantic_error(16, node->lineno);
          free_symbol_type(symbol_info);
          return;
        } else {
          // insert into symbol table
        }
      }
    }
  } else if (node->children_count == 3) {
    if (node->children[2]->type == _SEMI) {
      /* ExtDef -> Specifier ExtDecList SEMI */
      SymbolInfo symbol_info = specifier(node->children[0]);
      if (symbol_info->kind == VariableInfo) {
        external_declaration_list(node->children[1], symbol_info);
        free_symbol_type(symbol_info);
        return;
      } else if (symbol_info->kind == TypeDef) { // like `struct A {...} a;`
        assert(symbol_info->info.type);
        int kind = symbol_info->info.type->kind;
        if (kind == STRUCTURE) {
          if (find_symbol_table(-1, symbol_info->name) != NULL) {
            // -1 -> all name
            // redefine structure
            semantic_error(16, node->lineno);
            free_symbol_type(symbol_info);
            return;
          } else {
            // insert into symbol table and declare variables
          }
        }
      }
    } else if (node->children[2]->type == _CompSt) {
      /* ExtDef -> Specifier FunDec CompSt */
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

void analysis(struct Ast *root) {
  info("hit %s", unit_names[root->type]);
  assert(root->type == _Program);
  assert(root->children_count == 1);

  external_definition_list(root->children[0]);
}
