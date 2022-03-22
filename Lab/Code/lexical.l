%{

#include "common.h"
#include "syntax.tab.h"

#define DBG_PRINTF() log("line %d | token %s", yylineno, yytext);

int lexical_errors = 0;
static void lexical_error() {
  ++lexical_errors;
  print("Error type A at Line %d: Mysterious character \"%s\".", yylineno, yytext);
}

static Attribute attr_table[MAX_ATTR];
static size_t attr_table_index = 0;

Attribute get_attribute(size_t index) {
  return attr_table[index];
}

enum { _OCT, _DEC, _HEX, };
static const char *int_base_format[] = {
    [_OCT] = "%o",
    [_DEC] = "%d",
    [_HEX] = "%x",
};

static void install(int type, int attr) {
  DBG_PRINTF();

  yylval.type_ast = (struct Ast *)log_malloc(sizeof(struct Ast));
  yylval.type_ast->type = type;
  yylval.type_ast->children_count = 0;
  yylval.type_ast->lineno = yylineno;

  switch (type) {
  case _RELOP:
  case _TYPE:
    yylval.type_ast->attr_index = attr_table_index;
    attr_table[attr_table_index]._attr = attr;
    ++attr_table_index;
    break;
  case _INT:
    yylval.type_ast->attr_index = attr_table_index;
    sscanf(yytext, int_base_format[attr],
           &attr_table[attr_table_index]._int);
    ++attr_table_index;
    break;
  case _FLOAT:
    assert(attr == -1);
    yylval.type_ast->attr_index = attr_table_index;
    sscanf(yytext, "%f", &attr_table[attr_table_index]._float);
    ++attr_table_index;
    break;
  case _ID:
    assert(attr == -1);
    yylval.type_ast->attr_index = attr_table_index;
    sscanf(yytext, "%63s", attr_table[attr_table_index]._string);
    ++attr_table_index;
    break;
  default:
    assert(attr == -1);
    yylval.type_ast->attr_index = -1;
    break;
  }
}

%}

%option yylineno
%option noyywrap

/* regular definitions */

newline ("\n"|"\r\n")+
blank (" "|"\t")+

letter [_A-Za-z]
digit [0-9]

dec_int 0|[1-9]{digit}*
hex_int 0[xX]([0-9a-fA-F]+)
oct_int 0[0-7]+

/*
 * normal_float ({digit}+)"."({digit}+)
 * exponent_float ((({digit}*)"."({digit}+))|(({digit}+)"."({digit}*)))[eE]["+"|"-"]?{digit}+
 */

float ({digit}+)"."({digit}+)

id {letter}({letter}|{digit})*
semi ";"
comma ","
assignop "="
gt ">"
ge ">="
eq "=="
ne "!="
lt "<"
le "<="
plus "+"
minus "-"
star "*"
div "/"
and "&&"
or "||"
dot "."
not "!"
type_int "int"
type_float "float"
lp "("
rp ")"
lb "["
rb "]"
lc "{"
rc "}"
struct "struct"
return "return"
if "if"
else "else"
while "while"


%%

{newline} {}
{blank} {}

{dec_int} { install(_INT, _DEC); return INT; }
{oct_int} { install(_INT, _OCT); return INT; }
{hex_int} { install(_INT, _HEX); return INT; }
{float} { install(_FLOAT, -1); return FLOAT; }

{lt} { install(_RELOP, _LT); return RELOP; }
{le} { install(_RELOP, _LE); return RELOP; }
{gt} { install(_RELOP, _GT); return RELOP; }
{ge} { install(_RELOP, _GE); return RELOP; }
{eq} { install(_RELOP, _EQ); return RELOP; }
{ne} { install(_RELOP, _NE); return RELOP; }

{assignop} { install(_ASSIGNOP, -1); return ASSIGNOP; }
{semi} { install(_SEMI, -1); return SEMI; }
{comma} { install(_COMMA, -1); return COMMA; }
{dot} { install(_DOT, -1); return DOT; }
{lp} { install(_LP, -1); return LP; }
{rp} { install(_RP, -1); return RP; }
{lb} { install(_LB, -1); return LB; }
{rb} { install(_RB, -1); return RB; }
{lc} { install(_LC, -1); return LC; }
{rc} { install(_RC, -1); return RC; }
{plus} { install(_PLUS, -1); return PLUS; }
{minus} { install(_MINUS, -1); return MINUS; }
{star} { install(_STAR, -1); return STAR; }
{div} { install(_DIV, -1); return DIV; }
{and} { install(_AND, -1); return AND; }
{or} { install(_OR, -1); return OR; }
{not} { install(_NOT, -1); return NOT; }
{if} { install(_IF, -1); return IF; }
{else} { install(_ELSE, -1); return ELSE; }
{while} { install(_WHILE, -1); return WHILE; }
{struct} { install(_STRUCT, -1); return STRUCT; }
{return} { install(_RETURN, -1); return RETURN; }

{type_int} { install(_TYPE, _INT); return TYPE; }
{type_float} { install(_TYPE, _FLOAT); return TYPE; }

{id} { install(_ID, -1); return ID; }

. { lexical_error(); }

%%
