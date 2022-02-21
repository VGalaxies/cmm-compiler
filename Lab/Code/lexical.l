%{

#include <stdio.h>

#define DEBUG
#ifdef DEBUG
#define DBG_PRINTF() printf("line %d | token %s\n", yylineno, yytext);
#else
#define DBG_PRINTF()
#endif

#define panic(format, ...) printf("\33[1;31m" format "\33[0m\n", ##__VA_ARGS__)
#define log(format, ...) printf("\33[1;35m" format "\33[0m\n", ##__VA_ARGS__)

typedef struct {
  int yyname;
  union {
    int yylval;
    unsigned yyint;
    float yyfloat;
    char yystring[64];
  };
} Token;

Token tokens[1024];
size_t curr_index = 0;

enum {
  LT,
  LE,
  EQ,
  NE,
  GT,
  GE,
  RELOP,

  ASSIGNOP,

  SEMI,
  COMMA,
  DOT,

  LP,
  RP,
  LB,
  RB,
  LC,
  RC,

  PLUS,
  MINUS,
  STAR,
  DIV,
  AND,
  OR,
  NOT,

  TYPE,
  INT,
  FLOAT,

  IF,
  ELSE,
  WHILE,
  STRUCT,
  RETURN,

  ID,
};

static void install(int type, int attr) {
  if (attr >= 0) {
    tokens[curr_index].yylval = attr;
  }
  DBG_PRINTF();
  tokens[curr_index].yyname = type;
  ++curr_index;
}

void debug() {
  for (size_t i = 0; i < curr_index; ++i) {
    switch (tokens[i].yyname) {
    case INT:
      log("%u", tokens[i].yyint);
      break;
    case FLOAT:
      log("%f", tokens[i].yyfloat);
      break;
    case ID:
      log("%s", tokens[i].yystring);
      break;
    default:
      break;
    }
  }
}

%}

%option yylineno

/* regular definitions */
delim " "|\t|\n|\r\n
WS {delim}+

letter [_A-Za-z]
digit [0-9]

dec 0|[1-9]{digit}*
hex 0x([0-9a-f]+)|0X([0-9A-F]+)
oct 0[0-7]+
INT {dec}|{hex}|{oct}

/*
 * normal_float ({digit}+)"."({digit}+)
 * exponent_float ((({digit}*)"."({digit}+))|(({digit}+)"."({digit}*)))[eE]["+"|"-"]?{digit}+
 * FLOAT {normal_float}|{exponent_float}
 */

FLOAT ({digit}+)"."({digit}+)

ID {letter}({letter}|{digit})*
SEMI ";"
COMMA ","
ASSIGNOP "="
GT ">"
GE ">="
EQ "=="
NE "!="
LT "<"
LE "<="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE "int" | "float"
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"


%%

{WS} { /* no action and no return */ }

{INT} { sscanf(yytext, "%u", &tokens[curr_index].yyint); install(INT, -1);  }
{FLOAT} { sscanf(yytext, "%f", &tokens[curr_index].yyfloat); install(FLOAT, -1); }

{LT} { install(RELOP, LT); }
{LE} { install(RELOP, LE); }
{GT} { install(RELOP, GT); }
{GE} { install(RELOP, GE); }
{EQ} { install(RELOP, EQ); }
{NE} { install(RELOP, NE); }

{ASSIGNOP} { install(ASSIGNOP, -1); }
{SEMI} { install(SEMI, -1); }
{COMMA} { install(COMMA, -1); }
{DOT} { install(DOT, -1); }
{LP} { install(LP, -1); }
{RP} { install(RP, -1); }
{LB} { install(LB, -1); }
{RB} { install(RB, -1); }
{LC} { install(LC, -1); }
{RC} { install(RC, -1); }
{PLUS} { install(PLUS, -1); }
{MINUS} { install(MINUS, -1); }
{STAR} { install(STAR, -1); }
{DIV} { install(DIV, -1); }
{AND} { install(AND, -1); }
{OR} { install(OR, -1); }
{IF} { install(IF, -1); }
{ELSE} { install(ELSE, -1); }
{WHILE} { install(WHILE, -1); }
{STRUCT} { install(STRUCT, -1); }
{RETURN} { install(RETURN, -1); }

"int" { install(TYPE, INT); }
"float" { install(TYPE, FLOAT); }

{ID} { sscanf(yytext, "%63s", tokens[curr_index].yystring); install(ID, -1); }

. { panic("Illegal token"); DBG_PRINTF(); }

%%
