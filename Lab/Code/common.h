#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* debug control */

// #define LEXICAL_DEBUG
// #define SYNTAX_DEBUG
// #define SEMANTIC_DEBUG

/* debug info */

#define print(format, ...) printf(format "\n", ##__VA_ARGS__)
#define panic(format, ...) printf("\33[1;31m" format "\33[0m\n", ##__VA_ARGS__)

#ifdef LEXICAL_DEBUG
#define log(format, ...) printf("\33[1;35m" format "\33[0m\n", ##__VA_ARGS__)
#else
#define log(format, ...)
#endif

#ifdef SEMANTIC_DEBUG
#define info(format, ...) printf("\33[1;32m" format "\33[0m", ##__VA_ARGS__)
#define action(format, ...) printf("\33[1;36m" format "\33[0m", ##__VA_ARGS__)
#else
#define info(format, ...)
#define action(format, ...)
#endif

/* macros */

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* units */

enum {
  // terminal
  _LT, // 0
  _LE,
  _EQ,
  _NE,
  _GT,
  _GE,
  _RELOP,
  _ASSIGNOP,
  _SEMI,
  _COMMA,
  _DOT, // 10
  _LP,
  _RP,
  _LB,
  _RB,
  _LC,
  _RC,
  _PLUS,
  _MINUS,
  _STAR,
  _DIV, // 20
  _AND,
  _OR,
  _NOT,
  _TYPE,
  _INT,
  _FLOAT,
  _ID,
  _IF,
  _ELSE,
  _WHILE, // 30
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
  _Tag, // 40
  _VarDec,
  _FunDec,
  _VarList,
  _ParamDec,
  _CompSt,
  _StmtList,
  _Stmt,
  _DefList,
  _Def,
  _DecList, // 50
  _Dec,
  _Exp,
  _Args,

  // ε
  _EMPTY,
};

#define UNIT_KEYS(_)                                                           \
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

#define UNIT_NAME(name) [_##name] = #name,
static const char *unit_names[] = {UNIT_KEYS(UNIT_NAME)};

/* syntax AST interface */

#define MAX_CHILDREN 8
struct Ast {
  int lineno;
  int attr_index;
  int type;
  size_t children_count;
  struct Ast *children[MAX_CHILDREN];
};

extern void print_ast_tree_interface();
extern struct Ast *get_ast_root();

/* lexical attribute interface */

typedef union {
  int _attr;        // for TYPE or RELOP
  unsigned _int;    // for INT
  float _float;     // for FLOAT
  char _string[64]; // for ID
} Attribute;

#define MAX_ATTR 1024
extern Attribute get_attribute(size_t index);

/* memory management interface */

#define INIT_MALLOC_SIZE 1024
extern void *log_malloc(size_t size);

/* semantic interface */

extern void print_symbol_table_interface();

/* semantic type system */

typedef struct TypeItem *Type;
typedef struct FieldListItem *FieldList;

struct TypeItem {
  enum { BASIC, ARRAY, STRUCTURE } kind;
  union {
    // 基本类型
    int basic;
    // 数组类型信息包括元素类型与数组大小构成
    struct {
      Type elem;
      size_t size;
    } array;
    // 结构体类型信息是一个链表
    FieldList structure;
  } u;
};

struct FieldListItem {
  char name[64];  // 域的名字
  Type type;      // 域的类型
  FieldList tail; // 下一个域
};

/* semantic symbol */

#define MAX_BRACKET 16
#define MAX_ARGUMENT 16

typedef struct SymbolItem *Symbol;
typedef struct SymbolInfoItem *SymbolInfo;

struct SymbolInfoItem {
  enum { TypeDef, VariableInfo, FunctionInfo } kind;
  char name[64];
  union {
    Type type;
    struct {
      SymbolInfo arguments[MAX_ARGUMENT];
      size_t argument_count;
      Type return_type;
    } function;
  } info;
};

struct SymbolItem {
  SymbolInfo symbol_info;
  Symbol tail;
};

#endif
