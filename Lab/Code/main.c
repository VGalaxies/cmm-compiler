#include "common.h"

/* #define SYNTAX_DEBUG */
#ifdef SYNTAX_DEBUG
extern int yydebug;
#endif

extern void yyrestart(FILE *);
extern int yyparse();
extern int yylex_destroy();
extern void analysis(); // for semantic analysis

extern int syntax_errors;
extern int lexical_errors;
int prev_lineno = 0; // for syntax error

static struct Ast *ast_root;

Attribute attr_table[MAX_ATTR];
size_t attr_table_index = 0;

void *node_table[MAX_NODE];
size_t node_table_index = 0;

void *log_node_malloc(size_t size) {
  void *res = malloc(size);
  node_table[node_table_index] = res;
  ++node_table_index;
  return res;
}

void make_root(struct Ast **root) {
  (*root) = (struct Ast *)log_node_malloc(sizeof(struct Ast));
  (*root)->type = _Program;
  (*root)->children_count = 0;
  (*root)->lineno = INT_MAX;

  ast_root = (*root);
}

void make_node(struct Ast **node, int type) {
  (*node) = (struct Ast *)log_node_malloc(sizeof(struct Ast));
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
    lineno = MIN(lineno, node->lineno); // assume
  }

  va_end(valist);

  assert(lineno != INT_MAX);
  (*root)->lineno = lineno;
  (*root)->children_count = count;

  prev_lineno = lineno;
}

static void print_lower(const char *s) {
  size_t length = strlen(s);
  for (size_t i = 0; i < length; ++i) {
    putc(tolower(s[i]), stdout);
  }
}

static void indented(int indent) {
  for (int i = 0; i < indent; ++i) {
    printf("  ");
  }
}

static void print_attr(int type, int index) {
  assert(index >= 0);

  switch (type) {
  case _TYPE:
    /* case _RELOP: */
    print_lower(unit_names[attr_table[index]._attr]);
    /* printf("%s", unit_names[attr_table[index]._attr]); */
    break;
  case _INT:
    printf("%d", attr_table[index]._int);
    break;
  case _FLOAT:
    printf("%f", attr_table[index]._float);
    break;
  case _ID:
    printf("%s", attr_table[index]._string);
    break;
  }
}

static void print_tree(struct Ast *root, int indent) {
  int type = root->type;
  if (type == _EMPTY) {
    return;
  }

  indented(indent);
  printf("%s", unit_names[root->type]);

  switch (type) {
  case _TYPE:
  /* case _RELOP: */
  case _INT:
  case _FLOAT:
  case _ID:
    printf(": ");
    print_attr(type, root->attr_index);
    break;
  default:
    if (type > _RETURN) { // non-terminal
      printf(" (%d)", root->lineno);
    }
    break;
  }
  printf("\n");

  for (int i = 0; i < root->children_count; ++i) {
    print_tree(root->children[i], indent + 1);
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

#ifdef SYNTAX_DEBUG
  yydebug = 1;
#endif

  yyparse();

  if (!syntax_errors && !lexical_errors) {
    print_tree(ast_root, 0);
    analysis(ast_root);
  }

  clear_tree();
  yylex_destroy();
  fclose(f);

  exit(EXIT_SUCCESS);
}
