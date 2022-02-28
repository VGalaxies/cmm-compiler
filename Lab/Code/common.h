#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define print(format, ...) printf(format "\n", ##__VA_ARGS__)
#define panic(format, ...) printf("\33[1;31m" format "\33[0m\n", ##__VA_ARGS__)
#define log(format, ...) printf("\33[1;35m" format "\33[0m\n", ##__VA_ARGS__)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

enum {
  // terminal
  _LT,
  _LE,
  _EQ,
  _NE,
  _GT,
  _GE,
  _RELOP,
  _ASSIGNOP,
  _SEMI,
  _COMMA,
  _DOT,
  _LP,
  _RP,
  _LB,
  _RB,
  _LC,
  _RC,
  _PLUS,
  _MINUS,
  _STAR,
  _DIV,
  _AND,
  _OR,
  _NOT,
  _TYPE,
  _INT,
  _FLOAT,
  _ID,
  _IF,
  _ELSE,
  _WHILE,
  _STRUCT,
  _RETURN,

  // non-terminal
  _Program,
  _ExtDefList,
  _ExtDef,
  _ExtDecList,
  _Specifier,
  _StructSpecifier,
  _OptTag,
  _Tag,
  _VarDec,
  _FunDec,
  _VarList,
  _ParamDec,
  _CompSt,
  _StmtList,
  _Stmt,
  _DefList,
  _Def,
  _DecList,
  _Dec,
  _Exp,
  _Args,

  // ε
  _EMPTY,
};

#define SYMBOL_KEYS(_)                                                         \
  _(LT)                                                                        \
  _(LE)                                                                        \
  _(EQ)                                                                        \
  _(NE)                                                                        \
  _(GT)                                                                        \
  _(GE)                                                                        \
  _(RELOP)                                                                     \
  _(ASSIGNOP)                                                                  \
  _(SEMI)                                                                      \
  _(COMMA)                                                                     \
  _(DOT)                                                                       \
  _(LP)                                                                        \
  _(RP)                                                                        \
  _(LB)                                                                        \
  _(RB)                                                                        \
  _(LC)                                                                        \
  _(RC)                                                                        \
  _(PLUS)                                                                      \
  _(MINUS)                                                                     \
  _(STAR)                                                                      \
  _(DIV)                                                                       \
  _(AND)                                                                       \
  _(OR)                                                                        \
  _(NOT)                                                                       \
  _(TYPE)                                                                      \
  _(INT)                                                                       \
  _(FLOAT)                                                                     \
  _(ID)                                                                        \
  _(IF)                                                                        \
  _(ELSE)                                                                      \
  _(WHILE)                                                                     \
  _(STRUCT)                                                                    \
  _(RETURN)                                                                    \
  _(Program)                                                                   \
  _(ExtDefList)                                                                \
  _(ExtDef)                                                                    \
  _(ExtDecList)                                                                \
  _(Specifier)                                                                 \
  _(StructSpecifier)                                                           \
  _(OptTag)                                                                    \
  _(Tag)                                                                       \
  _(VarDec)                                                                    \
  _(FunDec)                                                                    \
  _(VarList)                                                                   \
  _(ParamDec)                                                                  \
  _(CompSt)                                                                    \
  _(StmtList)                                                                  \
  _(Stmt)                                                                      \
  _(DefList)                                                                   \
  _(Def)                                                                       \
  _(DecList)                                                                   \
  _(Dec)                                                                       \
  _(Exp)                                                                       \
  _(Args)

#define SYMBOL_NAME(name) [_##name] = #name,
static const char *symbol_names[] = {SYMBOL_KEYS(SYMBOL_NAME)};

#define MAX_CHILDREN 8
#define MAX_SYMBOL 1024
#define MAX_NODE 10240

struct Ast {
  int lineno;
  int symbol_index;
  int type;
  size_t children_count;
  struct Ast *children[MAX_CHILDREN];
};

typedef union {
  int _attr;        // for TYPE or RELOP
  unsigned _int;    // for INT
  float _float;     // for FLOAT
  char _string[64]; // for ID
} Symbol;

extern Symbol symbol_table[MAX_SYMBOL];
extern size_t symbol_table_index;

extern void *node_table[MAX_NODE];
extern size_t node_table_index;

extern void *log_malloc(size_t size);
extern void make_root(struct Ast **root);
extern void make_node(struct Ast **node, int type);
extern void make_children(struct Ast **root, int count, ...);

#endif
