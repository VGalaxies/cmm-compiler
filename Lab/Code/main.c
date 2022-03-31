#include "common.h"
#include <stdlib.h>

#ifdef SYNTAX_DEBUG
extern int yydebug;
#endif

extern void yyrestart(FILE *);
extern int yyparse();
extern int yylex_destroy();
extern void analysis(); // for semantic analysis

extern int syntax_errors;
extern int lexical_errors;

/* memory management */

void **malloc_table = NULL;
size_t malloc_table_index = 0;
size_t malloc_table_size = 0;

void *log_malloc(size_t size) {
  if (malloc_table_size == 0) {
    malloc_table = (void **)malloc(INIT_MALLOC_SIZE * sizeof(void *));
    malloc_table_size = INIT_MALLOC_SIZE;
  }

  if (malloc_table_index == malloc_table_size) {
    malloc_table =
        (void **)realloc(malloc_table, INIT_MALLOC_SIZE * 2 * sizeof(void *));
    malloc_table_size *= 2;
  }

  void *res = malloc(size);
  malloc_table[malloc_table_index] = res;
  ++malloc_table_index;
  return res;
}

static void clear_malloc() {
  for (size_t i = 0; i < malloc_table_index; ++i) {
    free(malloc_table[i]);
    malloc_table[i] = NULL;
  }
  free(malloc_table);
  malloc_table = NULL;
}

/* program entry */

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
#ifdef SYNTAX_DEBUG
    print_ast_tree_interface();
#endif

    /* print_ast_tree_interface(); */
    analysis(get_ast_root());

#ifdef SEMANTIC_DEBUG
    print_symbol_table_interface();
#endif
  }

  clear_malloc();
  yylex_destroy();
  fclose(f);

  exit(EXIT_SUCCESS);
}
