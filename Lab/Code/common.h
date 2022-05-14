#ifndef __COMMON_H__
#define __COMMON_H__

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* debug control */

// #define LEXICAL_DEBUG
// #define SYNTAX_DEBUG
// #define SEMANTIC_DEBUG
#define IR_DEBUG

/* debug info */

#define print(format, ...) printf(format "\n", ##__VA_ARGS__)
#define panic(format, ...)                                \
  do {                                                    \
    printf("\33[1;31m" format "\33[0m\n", ##__VA_ARGS__); \
    exit(EXIT_FAILURE);                                   \
  } while (0)

#ifdef LEXICAL_DEBUG
#define log(format, ...) printf("\33[1;35m" format "\33[0m\n", ##__VA_ARGS__)
#else
#define log(format, ...)
#endif

#ifdef SEMANTIC_DEBUG
#define info(format, ...) printf("\33[1;32m" format "\33[0m", ##__VA_ARGS__)
#else
#define info(format, ...)
#endif

#ifdef IR_DEBUG
#define trace(format, ...) printf("\33[1;34m" format "\33[0m", ##__VA_ARGS__)
#else
#define trace(format, ...)
#endif

#if defined(SEMANTIC_DEBUG) || defined(IR_DEBUG)
#define action(format, ...) printf("\33[1;36m" format "\33[0m", ##__VA_ARGS__)
#else
#define action(format, ...)
#endif

/* macros */

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* units */

enum {
  // terminal
  _LT,  // 0
  _LE,
  _EQ,
  _NE,
  _GT,
  _GE,
  _RELOP,
  _ASSIGNOP,
  _SEMI,
  _COMMA,
  _DOT,  // 10
  _LP,
  _RP,
  _LB,
  _RB,
  _LC,
  _RC,
  _PLUS,
  _MINUS,
  _STAR,
  _DIV,  // 20
  _AND,
  _OR,
  _NOT,
  _TYPE,
  _INT,
  _FLOAT,
  _ID,
  _IF,
  _ELSE,
  _WHILE,  // 30
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
  _Tag,  // 40
  _VarDec,
  _FunDec,
  _VarList,
  _ParamDec,
  _CompSt,
  _StmtList,
  _Stmt,
  _DefList,
  _Def,
  _DecList,  // 50
  _Dec,
  _Exp,
  _Args,

  // ε
  _EMPTY,
};

#define UNIT_KEYS(_) \
  _(LT)              \
  _(LE)              \
  _(EQ)              \
  _(NE)              \
  _(GT)              \
  _(GE)              \
  _(RELOP)           \
  _(ASSIGNOP)        \
  _(SEMI)            \
  _(COMMA)           \
  _(DOT)             \
  _(LP)              \
  _(RP)              \
  _(LB)              \
  _(RB)              \
  _(LC)              \
  _(RC)              \
  _(PLUS)            \
  _(MINUS)           \
  _(STAR)            \
  _(DIV)             \
  _(AND)             \
  _(OR)              \
  _(NOT)             \
  _(TYPE)            \
  _(INT)             \
  _(FLOAT)           \
  _(ID)              \
  _(IF)              \
  _(ELSE)            \
  _(WHILE)           \
  _(STRUCT)          \
  _(RETURN)          \
  _(Program)         \
  _(ExtDefList)      \
  _(ExtDef)          \
  _(ExtDecList)      \
  _(Specifier)       \
  _(StructSpecifier) \
  _(OptTag)          \
  _(Tag)             \
  _(VarDec)          \
  _(FunDec)          \
  _(VarList)         \
  _(ParamDec)        \
  _(CompSt)          \
  _(StmtList)        \
  _(Stmt)            \
  _(DefList)         \
  _(Def)             \
  _(DecList)         \
  _(Dec)             \
  _(Exp)             \
  _(Args)

#define UNIT_NAME(name) [_##name] = #name,
static const char *unit_names[] = {UNIT_KEYS(UNIT_NAME)};

/* syntax AST definition */

#define MAX_CHILDREN 8
struct Ast {
  int lineno;
  int attr_index;
  int type;
  size_t children_count;
  struct Ast *children[MAX_CHILDREN];
};

/* lexical attribute definition */

#define MAX_ATTR 1024
#define LENGTH 64
union Attribute {
  int _attr;             // for TYPE or RELOP
  unsigned _int;         // for INT
  float _float;          // for FLOAT
  char _string[LENGTH];  // for ID
};

/* semantic type system definition */

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
  size_t width;
};

struct FieldListItem {
  char name[LENGTH];  // 域的名字
  Type type;          // 域的类型
  FieldList tail;     // 下一个域
  size_t offset;
};

/* semantic symbol definition */

#define MAX_BRACKET 16
#define MAX_ARGUMENT 16

typedef struct SymbolItem *Symbol;
typedef struct SymbolInfoItem *SymbolInfo;

struct SymbolInfoItem {
  enum { TypeDef, VariableInfo, FunctionInfo } kind;
  char name[LENGTH];
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

/* intermediate representation */

typedef struct OperandItem *Operand;
struct OperandItem {
  enum {
    OP_FUNC,
    OP_ADDRESS,
    OP_ADDRESS_ORI,
    OP_ADDRESS_DEREF,
    OP_VARIABLE,
    OP_CONSTANT,
    OP_LABEL,  // for label
  } kind;
  union {
    size_t placeno;
    unsigned value;  // for constant, note unsigned
  } u;
  Type type;
};

struct InterCode {
  enum {
    // assign
    IR_ASSIGN,
    IR_ASSIGN_CALL,
    // single
    IR_LABEL,
    IR_FUNCTION,
    IR_GOTO,
    IR_RETURN,
    IR_ARG,
    IR_PARAM,
    IR_READ,
    IR_WRITE,
    // binop
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    // relop
    IR_RELOP,
    // dec
    IR_DEC,
  } kind;
  union {
    struct {
      Operand right, left;
    } assign;
    struct {
      Operand result, op1, op2;
    } binop;
    struct {
      Operand x, y, z;
      int type;
    } relop;
    struct {
      Operand op;
    } single;
    struct {
      Operand var, size;
    } dec;
  } u;
};

typedef struct InterCodesItem *InterCodes;
struct InterCodesItem {
  size_t lineno;
  struct InterCode code;
  InterCodes prev, next;
};

/* code generation */

static const char *reg_name[32] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2",
    "$t3",   "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5",
    "$s6",   "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
};

enum { REG_NONE = 0, REG_IN_USE, REG_FREE };

struct RegisterDescriptor {
  int state;
  int var_no;
};

struct FunctionDescriptor {
  const char *func_name;  // for debug
  size_t func_no;
  int curr_offset_fp;
};

struct VariableDescriptor {
  const char *var_name;  // for debug
  size_t var_no;
  size_t func_no;  // for offset
  struct {         // not union, keep offset info
    int offset_fp;
    int reg_id;
  } pos;
  enum { STACK, REG } type;
};

#endif
