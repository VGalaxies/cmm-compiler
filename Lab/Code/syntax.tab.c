/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "./syntax.y"


#include "lex.yy.c"

extern int prev_lineno;

int syntax_errors = 0;
/* static int last_error_lineno = -1; // avoid repeated errors */
void yyerror(const char *);
static void syntax_error(struct Ast * node) {
  /* syntax_errors++; */
  /* panic("Error type B at Line [prev_lineno %d | node->lineno %d]: Syntax error.", prev_lineno, node->lineno); */
}


#line 87 "./syntax.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "syntax.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMI = 3,                       /* SEMI  */
  YYSYMBOL_COMMA = 4,                      /* COMMA  */
  YYSYMBOL_LC = 5,                         /* LC  */
  YYSYMBOL_RC = 6,                         /* RC  */
  YYSYMBOL_TYPE = 7,                       /* TYPE  */
  YYSYMBOL_ID = 8,                         /* ID  */
  YYSYMBOL_INT = 9,                        /* INT  */
  YYSYMBOL_FLOAT = 10,                     /* FLOAT  */
  YYSYMBOL_IF = 11,                        /* IF  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_STRUCT = 13,                    /* STRUCT  */
  YYSYMBOL_RETURN = 14,                    /* RETURN  */
  YYSYMBOL_ASSIGNOP = 15,                  /* ASSIGNOP  */
  YYSYMBOL_OR = 16,                        /* OR  */
  YYSYMBOL_AND = 17,                       /* AND  */
  YYSYMBOL_RELOP = 18,                     /* RELOP  */
  YYSYMBOL_PLUS = 19,                      /* PLUS  */
  YYSYMBOL_MINUS = 20,                     /* MINUS  */
  YYSYMBOL_STAR = 21,                      /* STAR  */
  YYSYMBOL_DIV = 22,                       /* DIV  */
  YYSYMBOL_NOT = 23,                       /* NOT  */
  YYSYMBOL_UNARY_MINUS = 24,               /* UNARY_MINUS  */
  YYSYMBOL_DOT = 25,                       /* DOT  */
  YYSYMBOL_LP = 26,                        /* LP  */
  YYSYMBOL_RP = 27,                        /* RP  */
  YYSYMBOL_LB = 28,                        /* LB  */
  YYSYMBOL_RB = 29,                        /* RB  */
  YYSYMBOL_LOWER_THAN_ELSE = 30,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_ELSE = 31,                      /* ELSE  */
  YYSYMBOL_YYACCEPT = 32,                  /* $accept  */
  YYSYMBOL_Program = 33,                   /* Program  */
  YYSYMBOL_ExtDefList = 34,                /* ExtDefList  */
  YYSYMBOL_ExtDef = 35,                    /* ExtDef  */
  YYSYMBOL_ExtDecList = 36,                /* ExtDecList  */
  YYSYMBOL_Specifier = 37,                 /* Specifier  */
  YYSYMBOL_StructSpecifier = 38,           /* StructSpecifier  */
  YYSYMBOL_OptTag = 39,                    /* OptTag  */
  YYSYMBOL_Tag = 40,                       /* Tag  */
  YYSYMBOL_VarDec = 41,                    /* VarDec  */
  YYSYMBOL_FunDec = 42,                    /* FunDec  */
  YYSYMBOL_VarList = 43,                   /* VarList  */
  YYSYMBOL_ParamDec = 44,                  /* ParamDec  */
  YYSYMBOL_CompSt = 45,                    /* CompSt  */
  YYSYMBOL_StmtList = 46,                  /* StmtList  */
  YYSYMBOL_Stmt = 47,                      /* Stmt  */
  YYSYMBOL_DefList = 48,                   /* DefList  */
  YYSYMBOL_Def = 49,                       /* Def  */
  YYSYMBOL_DecList = 50,                   /* DecList  */
  YYSYMBOL_Dec = 51,                       /* Dec  */
  YYSYMBOL_Exp = 52,                       /* Exp  */
  YYSYMBOL_Args = 53                       /* Args  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  12
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   256

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  32
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  113

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    44,    44,    46,    47,    50,    51,    52,    53,    55,
      56,    60,    61,    63,    64,    66,    67,    69,    73,    74,
      76,    77,    79,    80,    82,    86,    88,    89,    91,    92,
      93,    94,    95,    96,   100,   101,   104,   105,   107,   108,
     110,   111,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     134,   135
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMI", "COMMA", "LC",
  "RC", "TYPE", "ID", "INT", "FLOAT", "IF", "WHILE", "STRUCT", "RETURN",
  "ASSIGNOP", "OR", "AND", "RELOP", "PLUS", "MINUS", "STAR", "DIV", "NOT",
  "UNARY_MINUS", "DOT", "LP", "RP", "LB", "RB", "LOWER_THAN_ELSE", "ELSE",
  "$accept", "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier",
  "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList",
  "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList",
  "Dec", "Exp", "Args", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286
};
#endif

#define YYPACT_NINF (-53)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-36)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     127,   -53,   -53,    -2,    18,   -53,   127,    70,   -53,     2,
      14,   -53,   -53,   -53,   -53,    -6,    29,     7,    31,   208,
      26,   -53,    30,    25,    36,   -53,   -53,    30,    48,    36,
     -53,    30,    33,    57,   -53,   -53,    47,   109,   -11,    76,
      91,   -53,   -53,    74,   -53,     8,   -53,    72,   -53,   -53,
      75,    85,   116,   116,   116,   116,   -53,   124,   109,    49,
     116,   -53,    30,   -53,    73,   116,   116,    69,    87,    87,
     143,   -53,   -53,   -53,   116,   116,   116,   116,   116,   116,
     116,   116,   114,   116,   185,   -53,   -53,    88,   104,   157,
     171,   -53,   -53,   185,   205,   216,   226,   228,   228,    87,
      87,   -53,   128,   116,   -53,   109,   109,   -53,   -53,   102,
     -53,   109,   -53
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     8,    11,    16,     0,     2,     0,     0,    12,    17,
       0,    14,     1,     3,     6,    18,     0,     9,     0,     0,
       0,     5,     0,     0,     0,     7,    37,     0,     0,     0,
      21,     0,     0,    23,    18,    10,     0,    27,    40,     0,
      38,    13,    34,    24,    20,     0,    19,    57,    58,    59,
       0,     0,     0,     0,     0,     0,    29,     0,    27,     0,
       0,    36,     0,    22,     0,     0,     0,     0,    51,    52,
       0,    25,    26,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    41,    39,    54,    61,     0,     0,
       0,    30,    50,    42,    44,    43,    45,    46,    47,    48,
      49,    56,     0,     0,    53,     0,     0,    55,    60,    31,
      33,     0,    32
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -53,   -53,   131,   -53,   119,    10,   -53,   -53,   -53,   -22,
     -53,    93,   -53,   133,    94,   -48,    51,   -53,    92,   -53,
     -52,    52
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,    16,    27,     8,    10,    11,    17,
      18,    32,    33,    56,    57,    58,    28,    29,    39,    40,
      59,    88
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      67,    68,    69,    70,    60,    38,     9,   -15,    84,    43,
       7,    22,    87,    89,    90,     2,     7,    23,    12,    19,
      20,     3,    93,    94,    95,    96,    97,    98,    99,   100,
      31,   102,    21,     2,    36,    23,    24,    26,    34,     3,
      38,   -35,   -35,     2,   -35,   -35,   -35,   -35,   -35,     3,
     -35,    87,    73,    30,    41,    31,   -35,   109,   110,   -35,
      44,    45,   -35,   112,    74,    75,    76,    77,    78,    79,
      80,    81,    91,    14,    82,    37,    46,    83,    15,    61,
      42,    47,    48,    49,    74,    75,    76,    77,    78,    79,
      80,    81,   103,    53,    82,    62,    54,    83,    64,    55,
      86,    65,    23,    74,    75,    76,    77,    78,    79,    80,
      81,    66,    82,    82,    24,    83,    83,    47,    48,    49,
      50,    51,   101,    52,    47,    48,    49,    -4,     1,    53,
      71,   104,    54,   111,     2,    55,    53,    13,    63,    54,
       3,    35,    55,    74,    75,    76,    77,    78,    79,    80,
      81,    25,    72,    82,    85,   108,    83,   107,    74,    75,
      76,    77,    78,    79,    80,    81,     0,     0,    82,     0,
      92,    83,    74,    75,    76,    77,    78,    79,    80,    81,
       0,     0,    82,     0,   105,    83,    74,    75,    76,    77,
      78,    79,    80,    81,     0,     0,    82,     0,   106,    83,
      74,    75,    76,    77,    78,    79,    80,    81,     0,    26,
      82,     0,     0,    83,   -35,     2,     0,     0,     0,     0,
       0,     3,    76,    77,    78,    79,    80,    81,     0,     0,
      82,     0,     0,    83,    77,    78,    79,    80,    81,     0,
       0,    82,     0,     0,    83,    78,    79,    80,    81,    80,
      81,    82,     0,    82,    83,     0,    83
};

static const yytype_int8 yycheck[] =
{
      52,    53,    54,    55,    15,    27,     8,     5,    60,    31,
       0,     4,    64,    65,    66,     7,     6,    28,     0,     5,
      26,    13,    74,    75,    76,    77,    78,    79,    80,    81,
      20,    83,     3,     7,     9,    28,     5,     1,     8,    13,
      62,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,   103,     3,    27,     6,    45,    20,   105,   106,    23,
      27,     4,    26,   111,    15,    16,    17,    18,    19,    20,
      21,    22,     3,     3,    25,    24,    29,    28,     8,     3,
      29,     8,     9,    10,    15,    16,    17,    18,    19,    20,
      21,    22,     4,    20,    25,     4,    23,    28,    26,    26,
      27,    26,    28,    15,    16,    17,    18,    19,    20,    21,
      22,    26,    25,    25,     5,    28,    28,     8,     9,    10,
      11,    12,     8,    14,     8,     9,    10,     0,     1,    20,
       6,    27,    23,    31,     7,    26,    20,     6,    45,    23,
      13,    22,    26,    15,    16,    17,    18,    19,    20,    21,
      22,    18,    58,    25,    62,   103,    28,    29,    15,    16,
      17,    18,    19,    20,    21,    22,    -1,    -1,    25,    -1,
      27,    28,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    25,    -1,    27,    28,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    -1,    25,    -1,    27,    28,
      15,    16,    17,    18,    19,    20,    21,    22,    -1,     1,
      25,    -1,    -1,    28,     6,     7,    -1,    -1,    -1,    -1,
      -1,    13,    17,    18,    19,    20,    21,    22,    -1,    -1,
      25,    -1,    -1,    28,    18,    19,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    19,    20,    21,    22,    21,
      22,    25,    -1,    25,    28,    -1,    28
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     7,    13,    33,    34,    35,    37,    38,     8,
      39,    40,     0,    34,     3,     8,    36,    41,    42,     5,
      26,     3,     4,    28,     5,    45,     1,    37,    48,    49,
      27,    37,    43,    44,     8,    36,     9,    48,    41,    50,
      51,     6,    48,    41,    27,     4,    29,     8,     9,    10,
      11,    12,    14,    20,    23,    26,    45,    46,    47,    52,
      15,     3,     4,    43,    26,    26,    26,    52,    52,    52,
      52,     6,    46,     3,    15,    16,    17,    18,    19,    20,
      21,    22,    25,    28,    52,    50,    27,    52,    53,    52,
      52,     3,    27,    52,    52,    52,    52,    52,    52,    52,
      52,     8,    52,     4,    27,    27,    27,    29,    53,    47,
      47,    31,    47
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    32,    33,    34,    34,    35,    35,    35,    35,    36,
      36,    37,    37,    38,    38,    39,    39,    40,    41,    41,
      42,    42,    43,    43,    44,    45,    46,    46,    47,    47,
      47,    47,    47,    47,    48,    48,    49,    49,    50,    50,
      51,    51,    52,    52,    52,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
      53,    53
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     3,     2,     3,     1,     1,
       3,     1,     1,     5,     2,     1,     0,     1,     1,     4,
       4,     3,     3,     1,     2,     4,     2,     0,     2,     1,
       3,     5,     7,     5,     2,     0,     3,     1,     1,     3,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     4,     3,     4,     3,     1,     1,     1,
       3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Program: ExtDefList  */
#line 44 "./syntax.y"
                     { make_root(&(yyval.type_ast)); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1237 "./syntax.tab.c"
    break;

  case 3: /* ExtDefList: ExtDef ExtDefList  */
#line 46 "./syntax.y"
                               { make_node(&(yyval.type_ast), _ExtDefList); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1243 "./syntax.tab.c"
    break;

  case 4: /* ExtDefList: %empty  */
#line 47 "./syntax.y"
             { make_node(&(yyval.type_ast), _EMPTY); }
#line 1249 "./syntax.tab.c"
    break;

  case 5: /* ExtDef: Specifier ExtDecList SEMI  */
#line 50 "./syntax.y"
                                   { make_node(&(yyval.type_ast), _ExtDef); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1255 "./syntax.tab.c"
    break;

  case 6: /* ExtDef: Specifier SEMI  */
#line 51 "./syntax.y"
                        { make_node(&(yyval.type_ast), _ExtDef); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1261 "./syntax.tab.c"
    break;

  case 7: /* ExtDef: Specifier FunDec CompSt  */
#line 52 "./syntax.y"
                                 { make_node(&(yyval.type_ast), _ExtDef); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1267 "./syntax.tab.c"
    break;

  case 8: /* ExtDef: error  */
#line 53 "./syntax.y"
               { syntax_error((yyval.type_ast)); }
#line 1273 "./syntax.tab.c"
    break;

  case 9: /* ExtDecList: VarDec  */
#line 55 "./syntax.y"
                    { make_node(&(yyval.type_ast), _ExtDecList); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1279 "./syntax.tab.c"
    break;

  case 10: /* ExtDecList: VarDec COMMA ExtDecList  */
#line 56 "./syntax.y"
                                     { make_node(&(yyval.type_ast), _ExtDecList); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1285 "./syntax.tab.c"
    break;

  case 11: /* Specifier: TYPE  */
#line 60 "./syntax.y"
                 { make_node(&(yyval.type_ast), _Specifier); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1291 "./syntax.tab.c"
    break;

  case 12: /* Specifier: StructSpecifier  */
#line 61 "./syntax.y"
                            { make_node(&(yyval.type_ast), _Specifier); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1297 "./syntax.tab.c"
    break;

  case 13: /* StructSpecifier: STRUCT OptTag LC DefList RC  */
#line 63 "./syntax.y"
                                              { make_node(&(yyval.type_ast), _StructSpecifier); make_children(&(yyval.type_ast), 5, (yyvsp[-4].type_ast), (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1303 "./syntax.tab.c"
    break;

  case 14: /* StructSpecifier: STRUCT Tag  */
#line 64 "./syntax.y"
                             { make_node(&(yyval.type_ast), _StructSpecifier); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1309 "./syntax.tab.c"
    break;

  case 15: /* OptTag: ID  */
#line 66 "./syntax.y"
            { make_node(&(yyval.type_ast), _OptTag); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1315 "./syntax.tab.c"
    break;

  case 16: /* OptTag: %empty  */
#line 67 "./syntax.y"
         { make_node(&(yyval.type_ast), _EMPTY); }
#line 1321 "./syntax.tab.c"
    break;

  case 17: /* Tag: ID  */
#line 69 "./syntax.y"
         { make_node(&(yyval.type_ast), _Tag); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1327 "./syntax.tab.c"
    break;

  case 18: /* VarDec: ID  */
#line 73 "./syntax.y"
            { make_node(&(yyval.type_ast), _VarDec); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1333 "./syntax.tab.c"
    break;

  case 19: /* VarDec: VarDec LB INT RB  */
#line 74 "./syntax.y"
                          { make_node(&(yyval.type_ast), _VarDec); make_children(&(yyval.type_ast), 4, (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1339 "./syntax.tab.c"
    break;

  case 20: /* FunDec: ID LP VarList RP  */
#line 76 "./syntax.y"
                          { make_node(&(yyval.type_ast), _FunDec); make_children(&(yyval.type_ast), 4, (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1345 "./syntax.tab.c"
    break;

  case 21: /* FunDec: ID LP RP  */
#line 77 "./syntax.y"
                  { make_node(&(yyval.type_ast), _FunDec); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1351 "./syntax.tab.c"
    break;

  case 22: /* VarList: ParamDec COMMA VarList  */
#line 79 "./syntax.y"
                                 { make_node(&(yyval.type_ast), _VarList); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1357 "./syntax.tab.c"
    break;

  case 23: /* VarList: ParamDec  */
#line 80 "./syntax.y"
                   { make_node(&(yyval.type_ast), _VarList); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1363 "./syntax.tab.c"
    break;

  case 24: /* ParamDec: Specifier VarDec  */
#line 82 "./syntax.y"
                            { make_node(&(yyval.type_ast), _ParamDec); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1369 "./syntax.tab.c"
    break;

  case 25: /* CompSt: LC DefList StmtList RC  */
#line 86 "./syntax.y"
                                { make_node(&(yyval.type_ast), _CompSt); make_children(&(yyval.type_ast), 4, (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1375 "./syntax.tab.c"
    break;

  case 26: /* StmtList: Stmt StmtList  */
#line 88 "./syntax.y"
                         { make_node(&(yyval.type_ast), _StmtList); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1381 "./syntax.tab.c"
    break;

  case 27: /* StmtList: %empty  */
#line 89 "./syntax.y"
           { make_node(&(yyval.type_ast), _EMPTY); }
#line 1387 "./syntax.tab.c"
    break;

  case 28: /* Stmt: Exp SEMI  */
#line 91 "./syntax.y"
                { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1393 "./syntax.tab.c"
    break;

  case 29: /* Stmt: CompSt  */
#line 92 "./syntax.y"
              { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1399 "./syntax.tab.c"
    break;

  case 30: /* Stmt: RETURN Exp SEMI  */
#line 93 "./syntax.y"
                       { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1405 "./syntax.tab.c"
    break;

  case 31: /* Stmt: IF LP Exp RP Stmt  */
#line 94 "./syntax.y"
                                               { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 5, (yyvsp[-4].type_ast), (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1411 "./syntax.tab.c"
    break;

  case 32: /* Stmt: IF LP Exp RP Stmt ELSE Stmt  */
#line 95 "./syntax.y"
                                   { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 7, (yyvsp[-6].type_ast), (yyvsp[-5].type_ast), (yyvsp[-4].type_ast), (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1417 "./syntax.tab.c"
    break;

  case 33: /* Stmt: WHILE LP Exp RP Stmt  */
#line 96 "./syntax.y"
                            { make_node(&(yyval.type_ast), _Stmt); make_children(&(yyval.type_ast), 5, (yyvsp[-4].type_ast), (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1423 "./syntax.tab.c"
    break;

  case 34: /* DefList: Def DefList  */
#line 100 "./syntax.y"
                      { make_node(&(yyval.type_ast), _DefList); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1429 "./syntax.tab.c"
    break;

  case 35: /* DefList: %empty  */
#line 101 "./syntax.y"
          { make_node(&(yyval.type_ast), _EMPTY); }
#line 1435 "./syntax.tab.c"
    break;

  case 36: /* Def: Specifier DecList SEMI  */
#line 104 "./syntax.y"
                             { make_node(&(yyval.type_ast), _Def); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1441 "./syntax.tab.c"
    break;

  case 37: /* Def: error  */
#line 105 "./syntax.y"
            { syntax_error((yyval.type_ast)); }
#line 1447 "./syntax.tab.c"
    break;

  case 38: /* DecList: Dec  */
#line 107 "./syntax.y"
              { make_node(&(yyval.type_ast), _DecList); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1453 "./syntax.tab.c"
    break;

  case 39: /* DecList: Dec COMMA DecList  */
#line 108 "./syntax.y"
                            { make_node(&(yyval.type_ast), _DecList); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1459 "./syntax.tab.c"
    break;

  case 40: /* Dec: VarDec  */
#line 110 "./syntax.y"
             { make_node(&(yyval.type_ast), _Dec); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1465 "./syntax.tab.c"
    break;

  case 41: /* Dec: VarDec ASSIGNOP Exp  */
#line 111 "./syntax.y"
                          { make_node(&(yyval.type_ast), _Dec); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1471 "./syntax.tab.c"
    break;

  case 42: /* Exp: Exp ASSIGNOP Exp  */
#line 115 "./syntax.y"
                       { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1477 "./syntax.tab.c"
    break;

  case 43: /* Exp: Exp AND Exp  */
#line 116 "./syntax.y"
                  { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1483 "./syntax.tab.c"
    break;

  case 44: /* Exp: Exp OR Exp  */
#line 117 "./syntax.y"
                 { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1489 "./syntax.tab.c"
    break;

  case 45: /* Exp: Exp RELOP Exp  */
#line 118 "./syntax.y"
                    { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1495 "./syntax.tab.c"
    break;

  case 46: /* Exp: Exp PLUS Exp  */
#line 119 "./syntax.y"
                   { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1501 "./syntax.tab.c"
    break;

  case 47: /* Exp: Exp MINUS Exp  */
#line 120 "./syntax.y"
                    { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1507 "./syntax.tab.c"
    break;

  case 48: /* Exp: Exp STAR Exp  */
#line 121 "./syntax.y"
                   { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1513 "./syntax.tab.c"
    break;

  case 49: /* Exp: Exp DIV Exp  */
#line 122 "./syntax.y"
                  { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1519 "./syntax.tab.c"
    break;

  case 50: /* Exp: LP Exp RP  */
#line 123 "./syntax.y"
                { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1525 "./syntax.tab.c"
    break;

  case 51: /* Exp: MINUS Exp  */
#line 124 "./syntax.y"
                                  { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1531 "./syntax.tab.c"
    break;

  case 52: /* Exp: NOT Exp  */
#line 125 "./syntax.y"
              { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 2, (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1537 "./syntax.tab.c"
    break;

  case 53: /* Exp: ID LP Args RP  */
#line 126 "./syntax.y"
                    { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 4, (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1543 "./syntax.tab.c"
    break;

  case 54: /* Exp: ID LP RP  */
#line 127 "./syntax.y"
               { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1549 "./syntax.tab.c"
    break;

  case 55: /* Exp: Exp LB Exp RB  */
#line 128 "./syntax.y"
                    { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 4, (yyvsp[-3].type_ast), (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1555 "./syntax.tab.c"
    break;

  case 56: /* Exp: Exp DOT ID  */
#line 129 "./syntax.y"
                 { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1561 "./syntax.tab.c"
    break;

  case 57: /* Exp: ID  */
#line 130 "./syntax.y"
         { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1567 "./syntax.tab.c"
    break;

  case 58: /* Exp: INT  */
#line 131 "./syntax.y"
          { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1573 "./syntax.tab.c"
    break;

  case 59: /* Exp: FLOAT  */
#line 132 "./syntax.y"
            { make_node(&(yyval.type_ast), _Exp); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1579 "./syntax.tab.c"
    break;

  case 60: /* Args: Exp COMMA Args  */
#line 134 "./syntax.y"
                      { make_node(&(yyval.type_ast), _Args); make_children(&(yyval.type_ast), 3, (yyvsp[-2].type_ast), (yyvsp[-1].type_ast), (yyvsp[0].type_ast)); }
#line 1585 "./syntax.tab.c"
    break;

  case 61: /* Args: Exp  */
#line 135 "./syntax.y"
           { make_node(&(yyval.type_ast), _Args); make_children(&(yyval.type_ast), 1, (yyvsp[0].type_ast)); }
#line 1591 "./syntax.tab.c"
    break;


#line 1595 "./syntax.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 138 "./syntax.y"


void yyerror(const char *s) {
  syntax_errors++;
  /* log("Error type B at Line [prev %d | curr %d]: Syntax error.", prev_lineno, yylineno); */
  print("Error type B at Line %d: Syntax error.", prev_lineno);
}
