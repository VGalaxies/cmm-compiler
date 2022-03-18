/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYNTAX_TAB_H_INCLUDED
# define YY_YY_SYNTAX_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMI = 258,                    /* SEMI  */
    COMMA = 259,                   /* COMMA  */
    LC = 260,                      /* LC  */
    RC = 261,                      /* RC  */
    TYPE = 262,                    /* TYPE  */
    ID = 263,                      /* ID  */
    INT = 264,                     /* INT  */
    FLOAT = 265,                   /* FLOAT  */
    IF = 266,                      /* IF  */
    WHILE = 267,                   /* WHILE  */
    STRUCT = 268,                  /* STRUCT  */
    RETURN = 269,                  /* RETURN  */
    ASSIGNOP = 270,                /* ASSIGNOP  */
    OR = 271,                      /* OR  */
    AND = 272,                     /* AND  */
    RELOP = 273,                   /* RELOP  */
    PLUS = 274,                    /* PLUS  */
    MINUS = 275,                   /* MINUS  */
    STAR = 276,                    /* STAR  */
    DIV = 277,                     /* DIV  */
    NOT = 278,                     /* NOT  */
    UNARY_MINUS = 279,             /* UNARY_MINUS  */
    DOT = 280,                     /* DOT  */
    LP = 281,                      /* LP  */
    RP = 282,                      /* RP  */
    LB = 283,                      /* LB  */
    RB = 284,                      /* RB  */
    LOWER_THAN_ELSE = 285,         /* LOWER_THAN_ELSE  */
    ELSE = 286                     /* ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "./syntax.y"

  struct Ast *type_ast;

#line 99 "./syntax.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SYNTAX_TAB_H_INCLUDED  */
