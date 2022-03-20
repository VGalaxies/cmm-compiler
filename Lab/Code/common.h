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
#define info(format, ...) printf("\33[1;32m" format "\33[0m\n", ##__VA_ARGS__)

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

#define MAX_CHILDREN 8
#define MAX_ATTR 1024
#define MAX_NODE 10240

struct Ast {
  int lineno;
  int attr_index;
  int type;
  size_t children_count;
  struct Ast *children[MAX_CHILDREN];
};

typedef union {
  int _attr;        // for TYPE or RELOP
  unsigned _int;    // for INT
  float _float;     // for FLOAT
  char _string[64]; // for ID
} Attribute;

extern Attribute attr_table[MAX_ATTR];
extern size_t attr_table_index;

extern void *node_table[MAX_NODE];
extern size_t node_table_index;

extern void *log_node_malloc(size_t size);
extern void make_root(struct Ast **root);
extern void make_node(struct Ast **node, int type);
extern void make_children(struct Ast **root, int count, ...);

/* semantic part */

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
      unsigned size;
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

#define MAX_ARGUMENT 16
#define MAX_BRACKET 16
typedef struct SymbolItem *Symbol;
typedef struct SymbolInfoItem *SymbolInfo;

struct SymbolInfoItem {
  enum { TypeDef, VariableInfo, FunctionInfo } kind;
  char name[64];
  union {
    Type type;
    struct {
      Symbol arguments[MAX_ARGUMENT];
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
