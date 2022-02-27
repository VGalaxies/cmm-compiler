%{

#include "lex.yy.c"

int has_syntax_error = 0;
void yyerror(const char *);
static void syntax_error() {
  has_syntax_error = 1;
  panic("Error type B at Line %d: Syntax error.", yylineno);
}

void make_root(struct Ast **root);
void make_node(struct Ast **node, int type);
void make_children(struct Ast **root, int count, ...);

%}

%union {
  struct Ast *type_ast;
}

%token <type_ast> SEMI COMMA LC RC
%token <type_ast> TYPE ID INT FLOAT
%token <type_ast> IF WHILE STRUCT RETURN

%right <type_ast> ASSIGNOP
%left <type_ast> OR
%left <type_ast> AND
%left <type_ast> RELOP
%left <type_ast> PLUS MINUS
%left <type_ast> STAR DIV
%right <type_ast> NOT UNARY_MINUS
%left <type_ast> DOT LP RP LB RB

%nonassoc <type_ast> LOWER_THAN_ELSE
%nonassoc <type_ast> ELSE

%type <type_ast> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%start Program

%%

/* High-level Definitions */
Program : ExtDefList { make_root(&$$); make_children(&$$, 1, $1); }
;
ExtDefList : ExtDef ExtDefList { make_node(&$$, _ExtDefList); make_children(&$$, 2, $1, $2); }
           | { make_node(&$$, _EMPTY); }
;
ExtDef : Specifier ExtDecList SEMI { make_node(&$$, _ExtDef); make_children(&$$, 3, $1, $2, $3); }
       | Specifier SEMI { make_node(&$$, _ExtDef); make_children(&$$, 2, $1, $2); }
       | Specifier FunDec CompSt { make_node(&$$, _ExtDef); make_children(&$$, 3, $1, $2, $3); }
       | error { syntax_error(); }
;
ExtDecList : VarDec { make_node(&$$, _ExtDecList); make_children(&$$, 1, $1); }
           | VarDec COMMA ExtDecList { make_node(&$$, _ExtDecList); make_children(&$$, 3, $1, $2, $3); }
;

/* Specifiers */
Specifier : TYPE { make_node(&$$, _Specifier); make_children(&$$, 1, $1); }
          | StructSpecifier { make_node(&$$, _Specifier); make_children(&$$, 1, $1); }
;
StructSpecifier : STRUCT OptTag LC DefList RC { make_node(&$$, _StructSpecifier); make_children(&$$, 5, $1, $2, $3, $4, $5); }
                | STRUCT Tag { make_node(&$$, _StructSpecifier); make_children(&$$, 2, $1, $2); }
;
OptTag : ID { make_node(&$$, _OptTag); make_children(&$$, 1, $1); }
       | { make_node(&$$, _EMPTY); }
; 
Tag : ID { make_node(&$$, _Tag); make_children(&$$, 1, $1); }
;

/* Declarators */
VarDec : ID { make_node(&$$, _VarDec); make_children(&$$, 1, $1); }
       | VarDec LB INT RB { make_node(&$$, _VarDec); make_children(&$$, 4, $1, $2, $3, $4); }
;
FunDec : ID LP VarList RP { make_node(&$$, _FunDec); make_children(&$$, 4, $1, $2, $3, $4); }
       | ID LP RP { make_node(&$$, _FunDec); make_children(&$$, 3, $1, $2, $3); }
;
VarList : ParamDec COMMA VarList { make_node(&$$, _VarList); make_children(&$$, 3, $1, $2, $3); }
        | ParamDec { make_node(&$$, _VarList); make_children(&$$, 1, $1); }
;
ParamDec : Specifier VarDec { make_node(&$$, _ParamDec); make_children(&$$, 2, $1, $2); }
;

/* Statements */
CompSt : LC DefList StmtList RC { make_node(&$$, _CompSt); make_children(&$$, 4, $1, $2, $3, $4); }
       /* | error RC { error(); } */
;
StmtList : Stmt StmtList { make_node(&$$, _StmtList); make_children(&$$, 2, $1, $2); }
         | { make_node(&$$, _EMPTY); }
; 
Stmt : Exp SEMI { make_node(&$$, _Stmt); make_children(&$$, 2, $1, $2); }
     | CompSt { make_node(&$$, _Stmt); make_children(&$$, 1, $1); }
     | RETURN Exp SEMI { make_node(&$$, _Stmt); make_children(&$$, 3, $1, $2, $3); }
     | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { make_node(&$$, _Stmt); make_children(&$$, 5, $1, $2, $3, $4, $5); }
     | IF LP Exp RP Stmt ELSE Stmt { make_node(&$$, _Stmt); make_children(&$$, 7, $1, $2, $3, $4, $5, $6, $7); }
     | WHILE LP Exp RP Stmt { make_node(&$$, _Stmt); make_children(&$$, 5, $1, $2, $3, $4, $5); }
     /* | error SEMI { error(); } */
;

/* Local Definitions */
DefList : Def DefList { make_node(&$$, _DefList); make_children(&$$, 2, $1, $2); }
        | { make_node(&$$, _EMPTY); }
; 
Def : Specifier DecList SEMI { make_node(&$$, _Def); make_children(&$$, 3, $1, $2, $3); }
    | error SEMI { syntax_error(); }
;
DecList : Dec { make_node(&$$, _DecList); make_children(&$$, 1, $1); }
        | Dec COMMA DecList { make_node(&$$, _DecList); make_children(&$$, 3, $1, $2, $3); }
;
Dec : VarDec { make_node(&$$, _Dec); make_children(&$$, 1, $1); }
    | VarDec ASSIGNOP Exp { make_node(&$$, _Dec); make_children(&$$, 3, $1, $2, $3); }
;

/* Expressions */
Exp : Exp ASSIGNOP Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp AND Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp OR Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp RELOP Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp PLUS Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp MINUS Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp STAR Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp DIV Exp { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | LP Exp RP { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | MINUS Exp %prec UNARY_MINUS { make_node(&$$, _Exp); make_children(&$$, 2, $1, $2); }
    | NOT Exp { make_node(&$$, _Exp); make_children(&$$, 2, $1, $2); }
    | ID LP Args RP { make_node(&$$, _Exp); make_children(&$$, 4, $1, $2, $3, $4); }
    | ID LP RP { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | Exp LB Exp RB { make_node(&$$, _Exp); make_children(&$$, 4, $1, $2, $3, $4); }
    | Exp DOT ID { make_node(&$$, _Exp); make_children(&$$, 3, $1, $2, $3); }
    | ID { make_node(&$$, _Exp); make_children(&$$, 1, $1); }
    | INT { make_node(&$$, _Exp); make_children(&$$, 1, $1); }
    | FLOAT { make_node(&$$, _Exp); make_children(&$$, 1, $1); }
    /* | error RP { error(); } */
;
Args : Exp COMMA Args { make_node(&$$, _Args); make_children(&$$, 3, $1, $2, $3); }
     | Exp { make_node(&$$, _Args); make_children(&$$, 1, $1); }
;

%%

void yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
}
