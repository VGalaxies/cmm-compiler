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

  FILE *in = fopen(argv[1], "r");
  if (!in) {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }

  parser->restart(in);

  /* #ifdef SYNTAX_DEBUG
   *   yydebug = 1;
   * #endif */

  parser->parse();
  parser->lex_destroy();
  assert(!fclose(in));

  if (lexical_errors || syntax_errors) {
    goto FINAL;
  }

#ifdef SYNTAX_DEBUG
  parser->print_ast_tree();
#endif

  struct Ast *root = parser->get_ast_root();
  analyzer->semantic_analysis(root);

  if (semantic_errors) {
    goto FINAL;
  }

#ifdef SEMANTIC_DEBUG
  analyzer->print_symbol_table();
#endif

  ir->ir_translate(root);

  if (ir_errors) {
    goto FINAL;
  }

  // FILE *ir_out = fopen(argv[2], "w");
  // if (!ir_out) {
  //   perror(argv[2]);
  //   mm->clear_malloc(); // note
  //   exit(EXIT_FAILURE);
  // }
  // ir->ir_generate(ir_out);
  // assert(!fclose(ir_out));
#ifdef IR_DEBUG
  ir->ir_generate(stdout);
#endif

  // FILE *out = fopen(argv[2], "w");
  // if (!out) {
  //   perror(argv[2]);
  //   mm->clear_malloc(); // note
  //   exit(EXIT_FAILURE);
  // }
  // code->generate(out);
  // assert(!fclose(out));
  // code->generate(stdout);

FINAL:
  mm->clear_malloc();
  exit(EXIT_SUCCESS);
}
