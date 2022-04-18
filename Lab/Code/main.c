#include "common.h"
#include "module.h"

/* #ifdef SYNTAX_DEBUG
 * extern int yydebug;
 * #endif */

/* external errors */

extern int syntax_errors;
extern int lexical_errors;
extern int semantic_errors;

/* program entry */

static void print_usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
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

  parser->restart(f);

  /* #ifdef SYNTAX_DEBUG
   *   yydebug = 1;
   * #endif */

  parser->parse();

  if (!syntax_errors && !lexical_errors) {
#ifdef SYNTAX_DEBUG
    parser->print_ast_tree();
#endif

    struct Ast *root = parser->get_ast_root();
    analyzer->semantic_analysis(root);

    if (!semantic_errors) {
#ifdef SEMANTIC_DEBUG
      analyzer->print_symbol_table();
#endif
      ir->ir_translate(root);

      FILE *f = fopen("/home/vgalaxy/Desktop/shared/demo.ir", "w");
      assert(f);
      ir->ir_generate(f);
      ir->ir_generate(stdout);
    }
  }

  mm->clear_malloc();
  parser->lex_destroy();
  fclose(f);

  exit(EXIT_SUCCESS);
}
