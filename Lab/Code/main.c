#include "common.h"
#include "module.h"

/* #ifdef SYNTAX_DEBUG
 * extern int yydebug;
 * #endif */

/* external errors */

extern int syntax_errors;
extern int lexical_errors;
extern int semantic_errors;
extern int ir_errors;

/* program entry */

static void print_usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s [INPUT FILE] [OUTPUT FILE]\n", argv[0]);
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
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

      if (!ir_errors) {
        FILE *f = fopen(argv[2], "w");
        if (!f) {
          perror(argv[2]);
          exit(EXIT_FAILURE);
        }
        ir->ir_generate(f);
        ir->ir_generate(stdout);
        assert(!fclose(f));
      }
    }
  }

  mm->clear_malloc();
  parser->lex_destroy();
  assert(!fclose(f));

  exit(EXIT_SUCCESS);
}
