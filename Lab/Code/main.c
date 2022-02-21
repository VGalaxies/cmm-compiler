#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex();
extern void debug();

int main(int argc, char *argv[]) {
  if (argc > 1) {
    if (!(yyin = fopen(argv[1], "r"))) {
      perror(argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  while (yylex() != 0)
    ;
  debug();
  exit(EXIT_SUCCESS);
}
