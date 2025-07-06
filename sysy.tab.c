/* A Bison parser, made by GNU Bison 3.8.2.  */

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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "sysy.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ir.h"
#include "node.h"

// 函数声明
void write_AST(Node *node, FILE* file, int level);
extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char *yytext;
void yyerror(const char *s);
Node* ast_root = NULL;

// 添加这两个函数的声明，以便main函数可以调用
void set_lex_output_file(const char* filename);
void close_lex_output_file();

#line 93 "sysy.tab.c"

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

#include "sysy.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENT = 3,                      /* IDENT  */
  YYSYMBOL_INCONST = 4,                    /* INCONST  */
  YYSYMBOL_FLOACONST = 5,                  /* FLOACONST  */
  YYSYMBOL_CONST = 6,                      /* CONST  */
  YYSYMBOL_INT = 7,                        /* INT  */
  YYSYMBOL_FLOAT = 8,                      /* FLOAT  */
  YYSYMBOL_VOID = 9,                       /* VOID  */
  YYSYMBOL_IF = 10,                        /* IF  */
  YYSYMBOL_ELSE = 11,                      /* ELSE  */
  YYSYMBOL_WHILE = 12,                     /* WHILE  */
  YYSYMBOL_BREAK = 13,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 14,                  /* CONTINUE  */
  YYSYMBOL_RETURN = 15,                    /* RETURN  */
  YYSYMBOL_PLUS = 16,                      /* PLUS  */
  YYSYMBOL_MINUS = 17,                     /* MINUS  */
  YYSYMBOL_MUL = 18,                       /* MUL  */
  YYSYMBOL_DIV = 19,                       /* DIV  */
  YYSYMBOL_MOD = 20,                       /* MOD  */
  YYSYMBOL_ASSIGN = 21,                    /* ASSIGN  */
  YYSYMBOL_EQ = 22,                        /* EQ  */
  YYSYMBOL_NEQ = 23,                       /* NEQ  */
  YYSYMBOL_LT = 24,                        /* LT  */
  YYSYMBOL_GT = 25,                        /* GT  */
  YYSYMBOL_LTE = 26,                       /* LTE  */
  YYSYMBOL_GTE = 27,                       /* GTE  */
  YYSYMBOL_AND = 28,                       /* AND  */
  YYSYMBOL_OR = 29,                        /* OR  */
  YYSYMBOL_NOT = 30,                       /* NOT  */
  YYSYMBOL_LPAREN = 31,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 32,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 33,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 34,                    /* RBRACE  */
  YYSYMBOL_LBRACK = 35,                    /* LBRACK  */
  YYSYMBOL_RBRACK = 36,                    /* RBRACK  */
  YYSYMBOL_COMMA = 37,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 38,                 /* SEMICOLON  */
  YYSYMBOL_IFX = 39,                       /* IFX  */
  YYSYMBOL_YYACCEPT = 40,                  /* $accept  */
  YYSYMBOL_CompUnit = 41,                  /* CompUnit  */
  YYSYMBOL_Decl = 42,                      /* Decl  */
  YYSYMBOL_BType = 43,                     /* BType  */
  YYSYMBOL_ConstDecl = 44,                 /* ConstDecl  */
  YYSYMBOL_ConstDef_List = 45,             /* ConstDef_List  */
  YYSYMBOL_ConstDef = 46,                  /* ConstDef  */
  YYSYMBOL_VarDecl = 47,                   /* VarDecl  */
  YYSYMBOL_VarDef_List = 48,               /* VarDef_List  */
  YYSYMBOL_VarDef = 49,                    /* VarDef  */
  YYSYMBOL_ConstSubscript_List = 50,       /* ConstSubscript_List  */
  YYSYMBOL_ConstInitVal = 51,              /* ConstInitVal  */
  YYSYMBOL_OptConstInitVal_List = 52,      /* OptConstInitVal_List  */
  YYSYMBOL_ConstInitVal_List = 53,         /* ConstInitVal_List  */
  YYSYMBOL_InitVal = 54,                   /* InitVal  */
  YYSYMBOL_OptInitVal_List = 55,           /* OptInitVal_List  */
  YYSYMBOL_InitVal_List = 56,              /* InitVal_List  */
  YYSYMBOL_FuncDef = 57,                   /* FuncDef  */
  YYSYMBOL_FuncFParamsOpt = 58,            /* FuncFParamsOpt  */
  YYSYMBOL_FuncFParams = 59,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 60,                /* FuncFParam  */
  YYSYMBOL_Block = 61,                     /* Block  */
  YYSYMBOL_BlockItem_List = 62,            /* BlockItem_List  */
  YYSYMBOL_BlockItem = 63,                 /* BlockItem  */
  YYSYMBOL_Stmt = 64,                      /* Stmt  */
  YYSYMBOL_ExpOpt = 65,                    /* ExpOpt  */
  YYSYMBOL_Cond = 66,                      /* Cond  */
  YYSYMBOL_ConstExp = 67,                  /* ConstExp  */
  YYSYMBOL_Exp = 68,                       /* Exp  */
  YYSYMBOL_LOrExp = 69,                    /* LOrExp  */
  YYSYMBOL_LAndExp = 70,                   /* LAndExp  */
  YYSYMBOL_EqExp = 71,                     /* EqExp  */
  YYSYMBOL_RelExp = 72,                    /* RelExp  */
  YYSYMBOL_AddExp = 73,                    /* AddExp  */
  YYSYMBOL_MulExp = 74,                    /* MulExp  */
  YYSYMBOL_UnaryExp = 75,                  /* UnaryExp  */
  YYSYMBOL_PrimaryExp = 76,                /* PrimaryExp  */
  YYSYMBOL_Number = 77,                    /* Number  */
  YYSYMBOL_UnaryOp = 78,                   /* UnaryOp  */
  YYSYMBOL_LVal = 79,                      /* LVal  */
  YYSYMBOL_FuncRParamsOpt = 80,            /* FuncRParamsOpt  */
  YYSYMBOL_FuncRParams = 81                /* FuncRParams  */
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
typedef yytype_uint8 yy_state_t;

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

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   191

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  93
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  160

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    63,    63,    64,    65,    68,    68,    70,    71,    74,
      76,    77,    79,    81,    83,    84,    87,    88,    92,    93,
      97,    98,   101,   102,   105,   106,   110,   111,   114,   115,
     118,   119,   122,   125,   126,   129,   130,   133,   134,   137,
     139,   140,   142,   142,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   157,   158,   160,   161,   163,   165,   165,
     167,   167,   169,   169,   169,   171,   171,   171,   171,   171,
     173,   173,   173,   175,   175,   175,   175,   178,   179,   180,
     183,   184,   185,   187,   187,   188,   188,   188,   191,   192,
     196,   197,   200,   201
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
  "\"end of file\"", "error", "\"invalid token\"", "IDENT", "INCONST",
  "FLOACONST", "CONST", "INT", "FLOAT", "VOID", "IF", "ELSE", "WHILE",
  "BREAK", "CONTINUE", "RETURN", "PLUS", "MINUS", "MUL", "DIV", "MOD",
  "ASSIGN", "EQ", "NEQ", "LT", "GT", "LTE", "GTE", "AND", "OR", "NOT",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACK", "RBRACK", "COMMA",
  "SEMICOLON", "IFX", "$accept", "CompUnit", "Decl", "BType", "ConstDecl",
  "ConstDef_List", "ConstDef", "VarDecl", "VarDef_List", "VarDef",
  "ConstSubscript_List", "ConstInitVal", "OptConstInitVal_List",
  "ConstInitVal_List", "InitVal", "OptInitVal_List", "InitVal_List",
  "FuncDef", "FuncFParamsOpt", "FuncFParams", "FuncFParam", "Block",
  "BlockItem_List", "BlockItem", "Stmt", "ExpOpt", "Cond", "ConstExp",
  "Exp", "LOrExp", "LAndExp", "EqExp", "RelExp", "AddExp", "MulExp",
  "UnaryExp", "PrimaryExp", "Number", "UnaryOp", "LVal", "FuncRParamsOpt",
  "FuncRParams", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-54)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      58,    -1,  -121,  -121,    30,    58,    37,  -121,  -121,    58,
      55,  -121,  -121,   -23,    34,    38,  -121,    49,    47,    50,
      -1,    52,    65,  -121,    86,    70,  -121,    55,    90,    62,
    -121,    60,    67,  -121,  -121,  -121,  -121,  -121,    52,    63,
       2,    61,  -121,  -121,  -121,    52,    66,     6,    49,  -121,
      11,  -121,    69,    72,    -1,    52,    68,     2,    49,    52,
      52,    52,    52,    52,  -121,    52,     6,  -121,  -121,    11,
    -121,  -121,    71,   122,  -121,  -121,    73,    74,  -121,  -121,
    -121,    61,    61,  -121,  -121,  -121,    75,    76,    78,  -121,
      77,    81,  -121,    49,    85,   100,    64,    95,    52,  -121,
      86,  -121,   106,   122,  -121,   103,  -121,    24,    52,  -121,
    -121,     6,  -121,    11,  -121,  -121,    52,    52,  -121,  -121,
     104,  -121,  -121,  -121,    52,  -121,  -121,  -121,   111,    79,
     116,    48,    36,     2,   113,  -121,   108,   153,    52,    52,
      52,    52,    52,    52,    52,    52,   153,  -121,    98,   116,
      48,    36,    36,     2,     2,     2,     2,  -121,   153,  -121
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     7,     8,     0,     2,     0,     5,     6,     2,
       0,     1,     3,    18,     0,    14,     4,    18,     0,    10,
      33,     0,    16,    13,     0,     0,     9,     0,     0,     0,
      34,    35,    88,    83,    84,    85,    86,    87,     0,     0,
      56,    70,    73,    77,    82,     0,    81,     0,    18,    15,
       0,    11,    37,     0,     0,    90,     0,    57,    18,     0,
       0,     0,     0,     0,    79,     0,    28,    17,    26,    22,
      12,    20,     0,    40,    32,    36,    92,     0,    91,    80,
      19,    71,    72,    74,    75,    76,     0,    30,     0,    29,
      24,     0,    23,    18,     0,     0,     0,     0,    53,    42,
       0,    46,     0,    40,    43,     0,    54,    81,     0,    78,
      89,     0,    27,     0,    21,    38,     0,     0,    50,    51,
       0,    39,    41,    45,     0,    93,    31,    25,     0,    55,
      58,    60,    62,    65,     0,    52,     0,    53,     0,     0,
       0,     0,     0,     0,     0,     0,    53,    44,    47,    59,
      61,    63,    64,    66,    67,    68,    69,    49,    53,    48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -121,    44,   -68,     0,  -121,   120,  -121,  -121,   124,  -121,
     -15,    99,  -121,    41,   112,  -121,    39,  -121,  -121,    97,
    -121,   109,    82,  -121,  -120,    80,    54,   140,   -34,  -121,
      26,    33,  -116,   -21,   -13,   -11,  -121,  -121,  -121,   -70,
    -121,    83
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     4,     5,     6,     7,    18,    19,     8,    14,    15,
      22,    90,    91,    92,    87,    88,    89,     9,    29,    30,
      31,   101,   102,   103,   104,   105,   128,    71,   106,   129,
     130,   131,   132,    57,    41,    42,    43,    44,    45,    46,
      77,    78
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      40,    10,    25,   107,    56,    99,     2,     3,    20,    32,
      33,    34,    21,    68,    32,    33,    34,   148,    59,    60,
      28,    76,    35,    36,   151,   152,   157,    35,    36,    40,
      11,    86,    68,   107,    64,    99,    37,    38,   159,    66,
      13,    37,    38,    80,    69,   124,    81,    82,    40,    12,
      83,    84,    85,    16,    28,    32,    33,    34,    17,    65,
     142,   143,   144,   145,     1,     2,     3,   107,    35,    36,
     140,   141,    23,   100,    76,    24,   107,    68,   115,    61,
      62,    63,    37,    38,    21,    26,    47,    27,   107,    48,
     136,    50,    40,    52,    53,   133,   133,    54,    55,    58,
      79,    65,   118,   100,    72,    73,   109,    93,   138,   158,
     108,   110,   112,   111,   113,   114,   116,   133,   133,   133,
     133,   153,   154,   155,   156,    32,    33,    34,     1,     2,
       3,   117,    94,   119,    95,    96,    97,    98,    35,    36,
     121,   123,   135,   137,   139,   146,   147,    51,    49,    70,
     126,    75,    37,    38,   127,    73,    32,    33,    34,    67,
     -53,    39,    74,    94,   149,    95,    96,    97,    98,    35,
      36,   134,   150,     0,     0,     0,     0,     0,   120,     0,
       0,     0,     0,    37,    38,   122,    73,     0,     0,     0,
       0,   125
};

static const yytype_int16 yycheck[] =
{
      21,     1,    17,    73,    38,    73,     7,     8,    31,     3,
       4,     5,    35,    47,     3,     4,     5,   137,    16,    17,
      20,    55,    16,    17,   140,   141,   146,    16,    17,    50,
       0,    65,    66,   103,    45,   103,    30,    31,   158,    33,
       3,    30,    31,    58,    33,    21,    59,    60,    69,     5,
      61,    62,    63,     9,    54,     3,     4,     5,     3,    35,
      24,    25,    26,    27,     6,     7,     8,   137,    16,    17,
      22,    23,    38,    73,   108,    37,   146,   111,    93,    18,
      19,    20,    30,    31,    35,    38,    21,    37,   158,     3,
     124,    21,   113,     3,    32,   116,   117,    37,    31,    36,
      32,    35,    38,   103,    35,    33,    32,    36,    29,    11,
      37,    36,    34,    37,    37,    34,    31,   138,   139,   140,
     141,   142,   143,   144,   145,     3,     4,     5,     6,     7,
       8,    31,    10,    38,    12,    13,    14,    15,    16,    17,
      34,    38,    38,    32,    28,    32,    38,    27,    24,    50,
     111,    54,    30,    31,   113,    33,     3,     4,     5,    47,
      38,    21,    53,    10,   138,    12,    13,    14,    15,    16,
      17,   117,   139,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    30,    31,   103,    33,    -1,    -1,    -1,
      -1,   108
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,     8,    41,    42,    43,    44,    47,    57,
      43,     0,    41,     3,    48,    49,    41,     3,    45,    46,
      31,    35,    50,    38,    37,    50,    38,    37,    43,    58,
      59,    60,     3,     4,     5,    16,    17,    30,    31,    67,
      73,    74,    75,    76,    77,    78,    79,    21,     3,    48,
      21,    45,     3,    32,    37,    31,    68,    73,    36,    16,
      17,    18,    19,    20,    75,    35,    33,    54,    68,    33,
      51,    67,    35,    33,    61,    59,    68,    80,    81,    32,
      50,    74,    74,    75,    75,    75,    68,    54,    55,    56,
      51,    52,    53,    36,    10,    12,    13,    14,    15,    42,
      43,    61,    62,    63,    64,    65,    68,    79,    37,    32,
      36,    37,    34,    37,    34,    50,    31,    31,    38,    38,
      65,    34,    62,    38,    21,    81,    56,    53,    66,    69,
      70,    71,    72,    73,    66,    38,    68,    32,    29,    28,
      22,    23,    24,    25,    26,    27,    32,    38,    64,    70,
      71,    72,    72,    73,    73,    73,    73,    64,    11,    64
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    40,    41,    41,    41,    42,    42,    43,    43,    44,
      45,    45,    46,    47,    48,    48,    49,    49,    50,    50,
      51,    51,    52,    52,    53,    53,    54,    54,    55,    55,
      56,    56,    57,    58,    58,    59,    59,    60,    60,    61,
      62,    62,    63,    63,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    65,    65,    66,    67,    68,    69,    69,
      70,    70,    71,    71,    71,    72,    72,    72,    72,    72,
      73,    73,    73,    74,    74,    74,    74,    75,    75,    75,
      76,    76,    76,    77,    77,    78,    78,    78,    79,    79,
      80,    80,    81,    81
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     1,     1,     1,     4,
       1,     3,     4,     3,     1,     3,     2,     4,     0,     4,
       1,     3,     0,     1,     1,     3,     1,     3,     0,     1,
       1,     3,     6,     0,     1,     1,     3,     2,     5,     3,
       0,     2,     1,     1,     4,     2,     1,     5,     7,     5,
       2,     2,     3,     0,     1,     1,     1,     1,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     4,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       0,     1,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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
    YYNOMEM;
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
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
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
  case 2: /* CompUnit: %empty  */
#line 63 "sysy.y"
                     { (yyval.node_ptr) = new_node("CompUnit", 0); ast_root = (yyval.node_ptr); }
#line 1281 "sysy.tab.c"
    break;

  case 3: /* CompUnit: Decl CompUnit  */
#line 64 "sysy.y"
                         { (yyval.node_ptr) = new_node("CompUnit", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); ast_root = (yyval.node_ptr); }
#line 1287 "sysy.tab.c"
    break;

  case 4: /* CompUnit: FuncDef CompUnit  */
#line 65 "sysy.y"
                       { (yyval.node_ptr) = new_node("CompUnit", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); ast_root = (yyval.node_ptr); }
#line 1293 "sysy.tab.c"
    break;

  case 5: /* Decl: ConstDecl  */
#line 68 "sysy.y"
                { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1299 "sysy.tab.c"
    break;

  case 6: /* Decl: VarDecl  */
#line 68 "sysy.y"
                                       { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1305 "sysy.tab.c"
    break;

  case 7: /* BType: INT  */
#line 70 "sysy.y"
          { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1311 "sysy.tab.c"
    break;

  case 8: /* BType: FLOAT  */
#line 71 "sysy.y"
            { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1317 "sysy.tab.c"
    break;

  case 9: /* ConstDecl: CONST BType ConstDef_List SEMICOLON  */
#line 74 "sysy.y"
                                               { (yyval.node_ptr) = new_node("ConstDecl", 2, (yyvsp[-2].node_ptr), (yyvsp[-1].node_ptr)); }
#line 1323 "sysy.tab.c"
    break;

  case 10: /* ConstDef_List: ConstDef  */
#line 76 "sysy.y"
             { (yyval.node_ptr) = new_node("ConstDef_List", 1, (yyvsp[0].node_ptr)); }
#line 1329 "sysy.tab.c"
    break;

  case 11: /* ConstDef_List: ConstDef COMMA ConstDef_List  */
#line 77 "sysy.y"
                                   { (yyval.node_ptr) = new_node("ConstDef_List", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1335 "sysy.tab.c"
    break;

  case 12: /* ConstDef: IDENT ConstSubscript_List ASSIGN ConstInitVal  */
#line 79 "sysy.y"
                                                        { (yyval.node_ptr) = new_node("ConstDef", 3, (yyvsp[-3].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1341 "sysy.tab.c"
    break;

  case 13: /* VarDecl: BType VarDef_List SEMICOLON  */
#line 81 "sysy.y"
                                     { (yyval.node_ptr) = new_node("VarDecl", 2, (yyvsp[-2].node_ptr), (yyvsp[-1].node_ptr)); }
#line 1347 "sysy.tab.c"
    break;

  case 14: /* VarDef_List: VarDef  */
#line 83 "sysy.y"
           { (yyval.node_ptr) = new_node("VarDef_List", 1, (yyvsp[0].node_ptr)); }
#line 1353 "sysy.tab.c"
    break;

  case 15: /* VarDef_List: VarDef COMMA VarDef_List  */
#line 84 "sysy.y"
                               { (yyval.node_ptr) = new_node("VarDef_List", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1359 "sysy.tab.c"
    break;

  case 16: /* VarDef: IDENT ConstSubscript_List  */
#line 87 "sysy.y"
                              { (yyval.node_ptr) = new_node("VarDef", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); }
#line 1365 "sysy.tab.c"
    break;

  case 17: /* VarDef: IDENT ConstSubscript_List ASSIGN InitVal  */
#line 88 "sysy.y"
                                               { (yyval.node_ptr) = new_node("VarDef_Init", 3, (yyvsp[-3].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1371 "sysy.tab.c"
    break;

  case 18: /* ConstSubscript_List: %empty  */
#line 92 "sysy.y"
                { (yyval.node_ptr) = new_node("ArrayDimList", 0); }
#line 1377 "sysy.tab.c"
    break;

  case 19: /* ConstSubscript_List: LBRACK ConstExp RBRACK ConstSubscript_List  */
#line 93 "sysy.y"
                                                 { (yyval.node_ptr) = new_node("ArrayDimList", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1383 "sysy.tab.c"
    break;

  case 20: /* ConstInitVal: ConstExp  */
#line 97 "sysy.y"
             { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1389 "sysy.tab.c"
    break;

  case 21: /* ConstInitVal: LBRACE OptConstInitVal_List RBRACE  */
#line 98 "sysy.y"
                                         { (yyval.node_ptr) = new_node("InitVal_Aggregate", 1, (yyvsp[-1].node_ptr)); }
#line 1395 "sysy.tab.c"
    break;

  case 22: /* OptConstInitVal_List: %empty  */
#line 101 "sysy.y"
                { (yyval.node_ptr) = new_node("ConstInitVal_List", 0); }
#line 1401 "sysy.tab.c"
    break;

  case 23: /* OptConstInitVal_List: ConstInitVal_List  */
#line 102 "sysy.y"
                        { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1407 "sysy.tab.c"
    break;

  case 24: /* ConstInitVal_List: ConstInitVal  */
#line 105 "sysy.y"
                 { (yyval.node_ptr) = new_node("ConstInitVal_List", 1, (yyvsp[0].node_ptr)); }
#line 1413 "sysy.tab.c"
    break;

  case 25: /* ConstInitVal_List: ConstInitVal COMMA ConstInitVal_List  */
#line 106 "sysy.y"
                                           { (yyval.node_ptr) = new_node("ConstInitVal_List", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1419 "sysy.tab.c"
    break;

  case 26: /* InitVal: Exp  */
#line 110 "sysy.y"
        { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1425 "sysy.tab.c"
    break;

  case 27: /* InitVal: LBRACE OptInitVal_List RBRACE  */
#line 111 "sysy.y"
                                    { (yyval.node_ptr) = new_node("InitVal_Aggregate", 1, (yyvsp[-1].node_ptr)); }
#line 1431 "sysy.tab.c"
    break;

  case 28: /* OptInitVal_List: %empty  */
#line 114 "sysy.y"
                { (yyval.node_ptr) = new_node("InitVal_List", 0); }
#line 1437 "sysy.tab.c"
    break;

  case 29: /* OptInitVal_List: InitVal_List  */
#line 115 "sysy.y"
                   { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1443 "sysy.tab.c"
    break;

  case 30: /* InitVal_List: InitVal  */
#line 118 "sysy.y"
            { (yyval.node_ptr) = new_node("InitVal_List", 1, (yyvsp[0].node_ptr)); }
#line 1449 "sysy.tab.c"
    break;

  case 31: /* InitVal_List: InitVal COMMA InitVal_List  */
#line 119 "sysy.y"
                                 { (yyval.node_ptr) = new_node("InitVal_List", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1455 "sysy.tab.c"
    break;

  case 32: /* FuncDef: BType IDENT LPAREN FuncFParamsOpt RPAREN Block  */
#line 122 "sysy.y"
                                                        { (yyval.node_ptr) = new_node("FuncDef", 4, (yyvsp[-5].node_ptr), (yyvsp[-4].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1461 "sysy.tab.c"
    break;

  case 33: /* FuncFParamsOpt: %empty  */
#line 125 "sysy.y"
                  { (yyval.node_ptr) = new_node("FuncFParamsOpt", 0); }
#line 1467 "sysy.tab.c"
    break;

  case 34: /* FuncFParamsOpt: FuncFParams  */
#line 126 "sysy.y"
                  { (yyval.node_ptr) = new_node("FuncFParamsOpt", 1, (yyvsp[0].node_ptr)); }
#line 1473 "sysy.tab.c"
    break;

  case 35: /* FuncFParams: FuncFParam  */
#line 129 "sysy.y"
               { (yyval.node_ptr) = new_node("FuncFParams", 1, (yyvsp[0].node_ptr)); }
#line 1479 "sysy.tab.c"
    break;

  case 36: /* FuncFParams: FuncFParam COMMA FuncFParams  */
#line 130 "sysy.y"
                                   { (yyval.node_ptr) = new_node("FuncFParams", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1485 "sysy.tab.c"
    break;

  case 37: /* FuncFParam: BType IDENT  */
#line 133 "sysy.y"
                { (yyval.node_ptr) = new_node("FuncFParam", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); }
#line 1491 "sysy.tab.c"
    break;

  case 38: /* FuncFParam: BType IDENT LBRACK RBRACK ConstSubscript_List  */
#line 134 "sysy.y"
                                                    { (yyval.node_ptr) = new_node("FuncFParam_Array", 3, (yyvsp[-4].node_ptr), (yyvsp[-3].node_ptr), (yyvsp[0].node_ptr)); }
#line 1497 "sysy.tab.c"
    break;

  case 39: /* Block: LBRACE BlockItem_List RBRACE  */
#line 137 "sysy.y"
                                    { (yyval.node_ptr) = new_node("Block", 1, (yyvsp[-1].node_ptr)); }
#line 1503 "sysy.tab.c"
    break;

  case 40: /* BlockItem_List: %empty  */
#line 139 "sysy.y"
                     { (yyval.node_ptr) = new_node("BlockItemList", 0); }
#line 1509 "sysy.tab.c"
    break;

  case 41: /* BlockItem_List: BlockItem BlockItem_List  */
#line 140 "sysy.y"
                               { (yyval.node_ptr) = new_node("BlockItemList", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); }
#line 1515 "sysy.tab.c"
    break;

  case 42: /* BlockItem: Decl  */
#line 142 "sysy.y"
                { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1521 "sysy.tab.c"
    break;

  case 43: /* BlockItem: Stmt  */
#line 142 "sysy.y"
                                    { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1527 "sysy.tab.c"
    break;

  case 44: /* Stmt: LVal ASSIGN Exp SEMICOLON  */
#line 145 "sysy.y"
                              { (yyval.node_ptr) = new_node("AssignStmt", 2, (yyvsp[-3].node_ptr), (yyvsp[-1].node_ptr)); }
#line 1533 "sysy.tab.c"
    break;

  case 45: /* Stmt: ExpOpt SEMICOLON  */
#line 146 "sysy.y"
                               { (yyval.node_ptr) = new_node("ExpStmt", 1, (yyvsp[-1].node_ptr)); }
#line 1539 "sysy.tab.c"
    break;

  case 46: /* Stmt: Block  */
#line 147 "sysy.y"
                               { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1545 "sysy.tab.c"
    break;

  case 47: /* Stmt: IF LPAREN Cond RPAREN Stmt  */
#line 148 "sysy.y"
                                           { (yyval.node_ptr) = new_node("IfStmt", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1551 "sysy.tab.c"
    break;

  case 48: /* Stmt: IF LPAREN Cond RPAREN Stmt ELSE Stmt  */
#line 149 "sysy.y"
                                           { (yyval.node_ptr) = new_node("IfElseStmt", 3, (yyvsp[-4].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1557 "sysy.tab.c"
    break;

  case 49: /* Stmt: WHILE LPAREN Cond RPAREN Stmt  */
#line 150 "sysy.y"
                                    { (yyval.node_ptr) = new_node("WhileStmt", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1563 "sysy.tab.c"
    break;

  case 50: /* Stmt: BREAK SEMICOLON  */
#line 151 "sysy.y"
                               { (yyval.node_ptr) = new_node("BreakStmt", 0); }
#line 1569 "sysy.tab.c"
    break;

  case 51: /* Stmt: CONTINUE SEMICOLON  */
#line 152 "sysy.y"
                               { (yyval.node_ptr) = new_node("ContinueStmt", 0); }
#line 1575 "sysy.tab.c"
    break;

  case 52: /* Stmt: RETURN ExpOpt SEMICOLON  */
#line 153 "sysy.y"
                               { (yyval.node_ptr) = new_node("ReturnStmt", 1, (yyvsp[-1].node_ptr)); }
#line 1581 "sysy.tab.c"
    break;

  case 53: /* ExpOpt: %empty  */
#line 157 "sysy.y"
                { (yyval.node_ptr) = new_node("ExpOpt", 0); }
#line 1587 "sysy.tab.c"
    break;

  case 54: /* ExpOpt: Exp  */
#line 158 "sysy.y"
                { (yyval.node_ptr) = new_node("ExpOpt", 1, (yyvsp[0].node_ptr)); }
#line 1593 "sysy.tab.c"
    break;

  case 55: /* Cond: LOrExp  */
#line 160 "sysy.y"
             { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1599 "sysy.tab.c"
    break;

  case 56: /* ConstExp: AddExp  */
#line 161 "sysy.y"
                 { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1605 "sysy.tab.c"
    break;

  case 57: /* Exp: AddExp  */
#line 163 "sysy.y"
            { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1611 "sysy.tab.c"
    break;

  case 58: /* LOrExp: LAndExp  */
#line 165 "sysy.y"
                { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1617 "sysy.tab.c"
    break;

  case 59: /* LOrExp: LOrExp OR LAndExp  */
#line 165 "sysy.y"
                                                 { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1623 "sysy.tab.c"
    break;

  case 60: /* LAndExp: EqExp  */
#line 167 "sysy.y"
               { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1629 "sysy.tab.c"
    break;

  case 61: /* LAndExp: LAndExp AND EqExp  */
#line 167 "sysy.y"
                                                { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1635 "sysy.tab.c"
    break;

  case 62: /* EqExp: RelExp  */
#line 169 "sysy.y"
              { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1641 "sysy.tab.c"
    break;

  case 63: /* EqExp: EqExp EQ RelExp  */
#line 169 "sysy.y"
                                             { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1647 "sysy.tab.c"
    break;

  case 64: /* EqExp: EqExp NEQ RelExp  */
#line 169 "sysy.y"
                                                                                                              { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1653 "sysy.tab.c"
    break;

  case 65: /* RelExp: AddExp  */
#line 171 "sysy.y"
               { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1659 "sysy.tab.c"
    break;

  case 66: /* RelExp: RelExp LT AddExp  */
#line 171 "sysy.y"
                                               { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1665 "sysy.tab.c"
    break;

  case 67: /* RelExp: RelExp GT AddExp  */
#line 171 "sysy.y"
                                                                                                                { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1671 "sysy.tab.c"
    break;

  case 68: /* RelExp: RelExp LTE AddExp  */
#line 171 "sysy.y"
                                                                                                                                                                                  { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1677 "sysy.tab.c"
    break;

  case 69: /* RelExp: RelExp GTE AddExp  */
#line 171 "sysy.y"
                                                                                                                                                                                                                                                    { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1683 "sysy.tab.c"
    break;

  case 70: /* AddExp: MulExp  */
#line 173 "sysy.y"
               { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1689 "sysy.tab.c"
    break;

  case 71: /* AddExp: AddExp PLUS MulExp  */
#line 173 "sysy.y"
                                                 { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1695 "sysy.tab.c"
    break;

  case 72: /* AddExp: AddExp MINUS MulExp  */
#line 173 "sysy.y"
                                                                                                                     { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1701 "sysy.tab.c"
    break;

  case 73: /* MulExp: UnaryExp  */
#line 175 "sysy.y"
                 { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1707 "sysy.tab.c"
    break;

  case 74: /* MulExp: MulExp MUL UnaryExp  */
#line 175 "sysy.y"
                                                    { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1713 "sysy.tab.c"
    break;

  case 75: /* MulExp: MulExp DIV UnaryExp  */
#line 175 "sysy.y"
                                                                                                                        { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1719 "sysy.tab.c"
    break;

  case 76: /* MulExp: MulExp MOD UnaryExp  */
#line 175 "sysy.y"
                                                                                                                                                                                            { (yyval.node_ptr) = new_node("BinaryOp", 3, (yyvsp[-1].node_ptr), (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1725 "sysy.tab.c"
    break;

  case 77: /* UnaryExp: PrimaryExp  */
#line 178 "sysy.y"
               { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1731 "sysy.tab.c"
    break;

  case 78: /* UnaryExp: IDENT LPAREN FuncRParamsOpt RPAREN  */
#line 179 "sysy.y"
                                         { (yyval.node_ptr) = new_node("FuncCall", 2, (yyvsp[-3].node_ptr), (yyvsp[-1].node_ptr)); }
#line 1737 "sysy.tab.c"
    break;

  case 79: /* UnaryExp: UnaryOp UnaryExp  */
#line 180 "sysy.y"
                       { (yyval.node_ptr) = new_node("UnaryOp", 2, (yyvsp[-1].node_ptr), (yyvsp[0].node_ptr)); }
#line 1743 "sysy.tab.c"
    break;

  case 80: /* PrimaryExp: LPAREN Exp RPAREN  */
#line 183 "sysy.y"
                      { (yyval.node_ptr) = (yyvsp[-1].node_ptr); }
#line 1749 "sysy.tab.c"
    break;

  case 81: /* PrimaryExp: LVal  */
#line 184 "sysy.y"
           { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1755 "sysy.tab.c"
    break;

  case 82: /* PrimaryExp: Number  */
#line 185 "sysy.y"
             { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1761 "sysy.tab.c"
    break;

  case 83: /* Number: INCONST  */
#line 187 "sysy.y"
                { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1767 "sysy.tab.c"
    break;

  case 84: /* Number: FLOACONST  */
#line 187 "sysy.y"
                                         { (yyval.node_ptr) = (yyvsp[0].node_ptr); }
#line 1773 "sysy.tab.c"
    break;

  case 85: /* UnaryOp: PLUS  */
#line 188 "sysy.y"
              {(yyval.node_ptr) = (yyvsp[0].node_ptr);}
#line 1779 "sysy.tab.c"
    break;

  case 86: /* UnaryOp: MINUS  */
#line 188 "sysy.y"
                                 {(yyval.node_ptr) = (yyvsp[0].node_ptr);}
#line 1785 "sysy.tab.c"
    break;

  case 87: /* UnaryOp: NOT  */
#line 188 "sysy.y"
                                                  {(yyval.node_ptr) = (yyvsp[0].node_ptr);}
#line 1791 "sysy.tab.c"
    break;

  case 88: /* LVal: IDENT  */
#line 191 "sysy.y"
          { (yyval.node_ptr) = new_node("LVal", 1, (yyvsp[0].node_ptr)); }
#line 1797 "sysy.tab.c"
    break;

  case 89: /* LVal: LVal LBRACK Exp RBRACK  */
#line 192 "sysy.y"
                             { (yyval.node_ptr) = new_node("ArrayAccess", 2, (yyvsp[-3].node_ptr), (yyvsp[-1].node_ptr)); }
#line 1803 "sysy.tab.c"
    break;

  case 90: /* FuncRParamsOpt: %empty  */
#line 196 "sysy.y"
                  { (yyval.node_ptr) = new_node("FuncRParamsOpt", 0); }
#line 1809 "sysy.tab.c"
    break;

  case 91: /* FuncRParamsOpt: FuncRParams  */
#line 197 "sysy.y"
                  { (yyval.node_ptr) = new_node("FuncRParamsOpt", 1, (yyvsp[0].node_ptr)); }
#line 1815 "sysy.tab.c"
    break;

  case 92: /* FuncRParams: Exp  */
#line 200 "sysy.y"
        { (yyval.node_ptr) = new_node("FuncRParams", 1, (yyvsp[0].node_ptr)); }
#line 1821 "sysy.tab.c"
    break;

  case 93: /* FuncRParams: Exp COMMA FuncRParams  */
#line 201 "sysy.y"
                            { (yyval.node_ptr) = new_node("FuncRParams", 2, (yyvsp[-2].node_ptr), (yyvsp[0].node_ptr)); }
#line 1827 "sysy.tab.c"
    break;


#line 1831 "sysy.tab.c"

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
  ++yynerrs;

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
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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

#line 204 "sysy.y"

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}

int main(int argc, char **argv) {
    if (argc <= 2) {
        // 更新用法说明，增加第四个可选参数
        printf("Usage: %s <input-file> <ir-output-file> [ast-output-file] [lex-output-file]\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror(argv[1]);
        return 1;
    }
    yyin = file;

    // 如果供了第四个参数，则将其用作词法分析的输出文件
    if (argc > 4) {
        set_lex_output_file(argv[4]);
    }
    const char* ir_output_filename = argv[2];
    FILE *ast_output_file = NULL;

    // AST输出文件现在是第三个参数
    if (argc > 3) {
      ast_output_file = fopen(argv[3], "w");
       if (!ast_output_file) {
             perror(argv[3]);
             fclose(file);
             close_lex_output_file();
             return 1;
        }
    }

    if (yyparse() == 0) {
        if(ast_output_file) {
            write_AST(ast_root, ast_output_file, 0);
            fflush(ast_output_file);
        }
        generate_llvm_ir(ast_root, ir_output_filename);
    } else {
        printf("Parse failed.\n");
    }
    // 关闭所有打开的文件
    if(ast_output_file) {
        fclose(ast_output_file);
    }
    close_lex_output_file(); // 关闭词法分析输出文件
    fclose(file);
    return 0;
}
