#include "common.h"
#include <stdarg.h>
#include <stdio.h>

/* extern int yydebug; */
extern void yyrestart(FILE *);
extern int yyparse();
extern int yylex_destroy();

extern int has_syntax_error;
extern int has_lexical_error;

static struct Ast *ast_root;

Symbol symbol_table[MAX_SYMBOL];
size_t symbol_table_index = 0;

void *node_table[MAX_NODE];
size_t node_table_index = 0;

void *log_malloc(size_t size) {
  void *res = malloc(size);
  node_table[node_table_index] = res;
  ++node_table_index;
  return res;
}

void make_root(struct Ast **root) {
  (*root) = (struct Ast *)log_malloc(sizeof(struct Ast));
  (*root)->type = _Program;
  (*root)->children_count = 0;
  (*root)->lineno = INT_MAX;

  ast_root = (*root);
}

void make_node(struct Ast **node, int type) {
  (*node) = (struct Ast *)log_malloc(sizeof(struct Ast));
  (*node)->type = type;
  (*node)->children_count = 0;
  (*node)->lineno = INT_MAX;
}

void make_children(struct Ast **root, int count, ...) {
  int lineno = INT_MAX;

  va_list valist;
  va_start(valist, count);

  for (int i = 0; i < count; ++i) {
    struct Ast *node = va_arg(valist, struct Ast *);
    (*root)->children[i] = node;
    lineno = MIN(lineno, node->lineno);
  }

  va_end(valist);

  assert(lineno != INT_MAX);
  (*root)->lineno = lineno;
  (*root)->children_count = count;
}

static void indented(int indent) {
  for (int i = 0; i < indent; ++i) {
    printf(" ");
  }
}

static void print_attr(int type, int index) {
  assert(index >= 0);

  switch (type) {
  case _TYPE:
  case _RELOP:
    printf("%s", symbol_names[symbol_table[index]._attr]);
    break;
  case _INT:
    printf("%d", symbol_table[index]._int);
    break;
  case _FLOAT:
    printf("%f", symbol_table[index]._float);
    break;
  case _ID:
    printf("%s", symbol_table[index]._string);
    break;
  }
}

static void print_tree(struct Ast *root, int indent) {
  int type = root->type;
  if (type == _EMPTY) {
    return;
  }

  indented(indent);
  printf("%s", symbol_names[root->type]);

  switch (type) {
  case _TYPE:
  case _RELOP:
  case _INT:
  case _FLOAT:
  case _ID:
    printf(": ");
    print_attr(type, root->symbol_index);
    break;
  default:
    if (type > _RETURN) { // non-terminal
      printf(" (%d)", root->lineno);
    }
    break;
  }
  printf("\n");

  for (int i = 0; i < root->children_count; ++i) {
    print_tree(root->children[i], indent + 2);
  }
}

static void clear_tree() {
  for (size_t i = 0; i < node_table_index; ++i) {
    free(node_table[i]);
    node_table[i] = NULL;
  }
}

static void print_usage(int argc, char *argv[]) {
  printf("Usage: %s [FILE]\n", argv[0]);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    print_usage(argc, argv);
    exit(EXIT_FAILURE);
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  yyrestart(f);

  /* yydebug = 1; */
  yyparse();

  if (!has_syntax_error && !has_lexical_error) {
    print_tree(ast_root, 0);
  }

  clear_tree();
  yylex_destroy();
  fclose(f);

  exit(EXIT_SUCCESS);
}
