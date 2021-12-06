/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "decaf.y" /* yacc.c:339  */

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "hashtable.h"
	#include "token.h"
	int yylex();
	void yyerror(char*);

#line 78 "decaf.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "decaf.tab.h".  */
#ifndef YY_YY_DECAF_TAB_H_INCLUDED
# define YY_YY_DECAF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    decimal_literal = 258,
    hex_literal = 259,
    char_literal = 260,
    eq = 261,
    neq = 262,
    and = 263,
    or = 264,
    not = 265,
    leq = 266,
    geq = 267,
    aff_add = 268,
    aff_sub = 269,
    integer = 270,
    boolean = 271,
    voidtype = 272,
    bool_literal = 273,
    id = 274,
    class = 275,
    Program = 276,
    If = 277,
    Else = 278,
    For = 279,
    Return = 280,
    Continue = 281,
    Break = 282,
    NEG = 283
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 13 "decaf.y" /* yacc.c:355  */

	int intval;
	bool boolval;
	char *stringval;
	char **stringlist;
	struct {
		quadop qop;
		int type;
	} literal;
	expr_val exprval;
	ast_node* ast;
	struct decl{
		char *name;
		struct decl *suiv;
	} decl;

#line 164 "decaf.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_DECAF_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 181 "decaf.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   288

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  144

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,     2,     2,     2,    20,     2,     2,
      40,    41,    26,    18,    42,    19,     2,    21,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
      22,    24,    23,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    43,     2,    44,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,    39,     2,     2,     2,     2,
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
      15,    16,    17,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    54,    54,    54,    56,    57,    58,    59,    61,    62,
      64,    65,    67,    68,    70,    71,    74,    75,    76,    78,
      79,    81,    81,    83,    84,    86,   107,   108,   110,   111,
     113,   114,   116,   136,   137,   138,   139,   140,   141,   142,
     144,   145,   147,   148,   151,   153,   154,   155,   157,   158,
     160,   164,   167,   170,   173,   176,   179,   182,   185,   188,
     191,   194,   197,   200,   206,   216,   228,   233,   237,   244,
     251,   259,   260,   262,   263,   264
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "decimal_literal", "hex_literal",
  "char_literal", "eq", "neq", "and", "or", "not", "leq", "geq", "aff_add",
  "aff_sub", "integer", "boolean", "voidtype", "'+'", "'-'", "'%'", "'/'",
  "'<'", "'>'", "'='", "'!'", "'*'", "bool_literal", "id", "class",
  "Program", "If", "Else", "For", "Return", "Continue", "Break", "NEG",
  "'{'", "'}'", "'('", "')'", "','", "'['", "']'", "';'", "$accept",
  "program", "$@1", "P", "FD", "field_decl", "MD", "method_decl", "Param",
  "tab_decl", "block", "$@2", "V", "var_decl", "B", "type", "S",
  "statement", "elseblock", "return_val", "method_call", "E", "location",
  "expr", "literal", "int_literal", "assign_op", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,    43,    45,
      37,    47,    60,    62,    61,    33,    42,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   123,   125,
      40,    41,    44,    91,    93,    59
};
# endif

#define YYPACT_NINF -74

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-74)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -11,    24,    75,    46,   -74,   -74,   112,   -74,   -74,    59,
      53,   112,   -74,   112,   -74,   -74,   -74,    78,    67,   -74,
     -74,   112,   -74,    85,    13,    23,     9,    74,     9,     5,
      88,   -74,     3,    93,    32,   -74,   -74,    80,   -74,    92,
       9,   -74,    92,    25,   -74,   -74,   104,   -74,     9,   -74,
     -74,   -74,    94,   114,     9,   -74,   100,   -74,   116,    84,
     -74,    58,   105,   125,   106,   109,   111,   -74,   -74,   -74,
     113,   -74,   -74,   -74,   106,   106,   106,   133,   -74,   106,
     106,   -74,   121,   106,   122,   -74,   223,   -74,   -74,   -74,
     -74,   -74,    54,   223,    40,   154,   106,   -74,   -74,   106,
     178,   -74,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   -74,   106,   -74,    92,   129,
      82,   -74,    -7,    -7,   262,   244,    60,    60,     1,     1,
     -74,   -74,    60,    60,   -74,   223,   127,   106,   -74,    92,
     -74,   202,   -74,   -74
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,     2,     7,    28,    29,     0,
       0,     5,     9,     4,    13,    11,    10,     0,     0,     3,
       8,     6,    12,     0,    27,     0,    18,     0,    18,     0,
       0,    25,     0,     0,     0,    71,    72,     0,    26,     0,
       0,    17,     0,     0,    21,    15,     0,    14,     0,    20,
      24,    16,     0,     0,    31,    19,     0,    23,     0,     0,
      27,     0,     0,     0,    43,     0,     0,    22,    39,    30,
       0,    74,    75,    73,    47,     0,     0,     0,    69,     0,
       0,    70,    48,     0,     0,    64,    42,    63,    68,    38,
      37,    33,     0,    46,     0,     0,     0,    65,    66,     0,
       0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    44,     0,    32,     0,     0,
       0,    67,    61,    62,    55,    56,    60,    59,    50,    51,
      54,    53,    57,    58,    52,    45,    41,     0,    49,     0,
      34,     0,    40,    35
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -74,   -74,   -74,   -74,   -74,   157,   158,     7,   142,   130,
     -42,   -74,   -74,   128,   -74,    29,   -74,   -74,   -74,   -74,
     -74,   -74,   -74,   -73,   -74,   150,   -74
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     6,    10,    11,    12,    13,    14,    32,    15,
      45,    50,    54,    16,    25,    17,    59,    69,   140,    84,
      70,    92,    85,    86,    87,    88,    75
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,    93,    94,    95,   106,   107,    97,    98,    35,    36,
     100,   108,   109,   110,   111,   112,   113,    68,     1,   114,
      22,   110,   111,   119,     7,     8,   120,   114,    22,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    23,   135,    39,    40,   102,   103,   104,   105,
      23,   106,   107,    28,     3,    33,    29,    33,   108,   109,
     110,   111,   112,   113,   141,    30,   114,    48,    31,    46,
      49,    71,    72,    42,    40,     4,   136,    53,   108,   109,
     110,   111,    73,    58,     5,   117,   114,    18,   102,   103,
     104,   105,    19,   106,   107,   115,   116,   142,    74,   143,
     108,   109,   110,   111,   112,   113,    24,    26,   114,    35,
      36,    78,    61,    27,    28,    62,    38,    63,    64,    65,
      66,    41,    44,    67,    43,    79,   138,     7,     8,     9,
      44,    80,    51,    81,    82,   102,   103,   104,   105,    55,
     106,   107,    56,    29,    60,    76,    83,   108,   109,   110,
     111,   112,   113,    77,    89,   114,    90,    96,    91,   139,
     102,   103,   104,   105,    99,   106,   107,   101,    20,    21,
      34,   137,   108,   109,   110,   111,   112,   113,    52,    37,
     114,     0,    57,     0,   102,   103,   104,   105,     0,   106,
     107,     0,     0,     0,     0,   118,   108,   109,   110,   111,
     112,   113,     0,     0,   114,     0,     0,     0,   102,   103,
     104,   105,     0,   106,   107,     0,     0,     0,     0,   121,
     108,   109,   110,   111,   112,   113,     0,     0,   114,   102,
     103,   104,   105,     0,   106,   107,     0,     0,     0,     0,
      44,   108,   109,   110,   111,   112,   113,     0,     0,   114,
     102,   103,   104,     0,     0,   106,   107,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   102,   103,
     114,     0,     0,   106,   107,     0,     0,     0,     0,     0,
     108,   109,   110,   111,   112,   113,     0,     0,   114
};

static const yytype_int16 yycheck[] =
{
      42,    74,    75,    76,    11,    12,    79,    80,     3,     4,
      83,    18,    19,    20,    21,    22,    23,    59,    29,    26,
      13,    20,    21,    96,    15,    16,    99,    26,    21,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,    13,   116,    41,    42,     6,     7,     8,     9,
      21,    11,    12,    40,    30,    26,    43,    28,    18,    19,
      20,    21,    22,    23,   137,    42,    26,    42,    45,    40,
      45,    13,    14,    41,    42,     0,   118,    48,    18,    19,
      20,    21,    24,    54,    38,    45,    26,    28,     6,     7,
       8,     9,    39,    11,    12,    41,    42,   139,    40,   141,
      18,    19,    20,    21,    22,    23,    28,    40,    26,     3,
       4,     5,    28,    28,    40,    31,    28,    33,    34,    35,
      36,    28,    38,    39,    44,    19,    44,    15,    16,    17,
      38,    25,    28,    27,    28,     6,     7,     8,     9,    45,
      11,    12,    28,    43,    28,    40,    40,    18,    19,    20,
      21,    22,    23,    28,    45,    26,    45,    24,    45,    32,
       6,     7,     8,     9,    43,    11,    12,    45,    11,    11,
      28,    42,    18,    19,    20,    21,    22,    23,    48,    29,
      26,    -1,    54,    -1,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    41,    18,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    -1,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    41,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,
      38,    18,    19,    20,    21,    22,    23,    -1,    -1,    26,
       6,     7,     8,    -1,    -1,    11,    12,    -1,    -1,    -1,
      -1,    -1,    18,    19,    20,    21,    22,    23,     6,     7,
      26,    -1,    -1,    11,    12,    -1,    -1,    -1,    -1,    -1,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    29,    47,    30,     0,    38,    48,    15,    16,    17,
      49,    50,    51,    52,    53,    55,    59,    61,    28,    39,
      51,    52,    53,    61,    28,    60,    40,    28,    40,    43,
      42,    45,    54,    61,    54,     3,     4,    71,    28,    41,
      42,    28,    41,    44,    38,    56,    61,    56,    42,    45,
      57,    28,    55,    61,    58,    45,    28,    59,    61,    62,
      28,    28,    31,    33,    34,    35,    36,    39,    56,    63,
      66,    13,    14,    24,    40,    72,    40,    28,     5,    19,
      25,    27,    28,    40,    65,    68,    69,    70,    71,    45,
      45,    45,    67,    69,    69,    69,    24,    69,    69,    43,
      69,    45,     6,     7,     8,     9,    11,    12,    18,    19,
      20,    21,    22,    23,    26,    41,    42,    45,    41,    69,
      69,    41,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    56,    42,    44,    32,
      64,    69,    56,    56
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    46,    48,    47,    49,    49,    49,    49,    50,    50,
      51,    51,    52,    52,    53,    53,    54,    54,    54,    55,
      55,    57,    56,    58,    58,    59,    60,    60,    61,    61,
      62,    62,    63,    63,    63,    63,    63,    63,    63,    63,
      64,    64,    65,    65,    66,    67,    67,    67,    68,    68,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    70,    70,
      70,    71,    71,    72,    72,    72
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     6,     1,     1,     2,     0,     2,     1,
       1,     1,     2,     1,     6,     6,     4,     2,     0,     8,
       6,     0,     5,     2,     0,     3,     3,     1,     1,     1,
       2,     0,     4,     2,     6,     7,     3,     2,     2,     1,
       2,     0,     1,     0,     4,     3,     1,     0,     1,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- Do a reduction.  |
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
        case 2:
#line 54 "decaf.y" /* yacc.c:1646  */
    {pushctx(); glob_context = curr_context;}
#line 1384 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 54 "decaf.y" /* yacc.c:1646  */
    { /* popctx()*/ ;}
#line 1390 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 56 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1396 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 57 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1402 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 58 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1408 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 59 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1414 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 61 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1420 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 62 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1426 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 64 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1432 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 65 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1438 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 67 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1444 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 68 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1450 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 70 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1456 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 71 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1462 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 74 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1468 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 75 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1474 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 76 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1480 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 78 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1486 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 79 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1492 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 81 "decaf.y" /* yacc.c:1646  */
    { pushctx(); glob_context = curr_context; }
#line 1498 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 81 "decaf.y" /* yacc.c:1646  */
    { /* popctx()*/  ;}
#line 1504 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 83 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1510 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 84 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1516 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 86 "decaf.y" /* yacc.c:1646  */
    {	
								quadop qo,q1;
								if((yyvsp[-2].intval) == INT)
									q1.u.cst = 0;
								else if((yyvsp[-2].intval) == BOOL)
									q1.u.cst = false;
								struct decl *pt = &(yyvsp[-1].decl);
								q1.type = QO_CST;
								qo.type = QO_ID;
								while(pt != NULL){
									Ht_item *item = (Ht_item*) malloc(sizeof(Ht_item));
									qo.u.offset = -4;
									gencode(qo,q1,q1,Q_AFF,-1);
									item->value = (yyvsp[-2].intval);
									item->key = malloc(strlen(pt->name)+1);
									strcpy(item->key, pt->name);
									newname(item);
									pt = pt->suiv;
								}
							 }
#line 1541 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 107 "decaf.y" /* yacc.c:1646  */
    { 	struct decl var; var.name = malloc((strlen((yyvsp[0].stringval))+1)); strcpy(var.name,(yyvsp[0].stringval));var.suiv = &(yyvsp[-2].decl); (yyval.decl) = var;}
#line 1547 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 108 "decaf.y" /* yacc.c:1646  */
    {	(yyval.decl).name = malloc((strlen((yyvsp[0].stringval))+1)); strcpy((yyval.decl).name,(yyvsp[0].stringval)); (yyval.decl).suiv = NULL;}
#line 1553 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 110 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval)=(yyvsp[0].intval);}
#line 1559 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 111 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval)=(yyvsp[0].intval);}
#line 1565 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 113 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1571 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 114 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1577 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 116 "decaf.y" /* yacc.c:1646  */
    {				
													// Ht_item *val = lookup($1);
													// if (!val)
													// 	yyerror("Erreur: Variable non déclarée\n");
													// if(val->value != $3.type)
													// 	yyerror("Erreur: Type de valeur incorrecte\n");
													// if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->value == BOOL || $3.type == BOOL))
													// 	yyerror("Erreur: Type de valeur incorrecte\n");
													// quadop q1, q2;
													// q1.u.offset = offset(val);
													// q1.type = QO_ID;
													// if($2 == Q_AFF)
													// 	gencode(q1,$3.result,$3.result,$2,-1);
													// else
													// 	gencode(q1,q1,$3.result,$2,-1);
													ast_print((yyvsp[-1].ast), 0);
													ast_stack *stack = ast_to_stack((yyvsp[-1].ast), NULL);
													ast_stack_print(stack);
													stack_to_code(stack);
												}
#line 1602 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 136 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1608 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 137 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1614 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 138 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1620 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 139 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1626 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 140 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1632 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 141 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1638 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 142 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1644 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 144 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1650 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 145 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1656 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 147 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1662 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 148 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1668 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 151 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1674 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 153 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1680 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 154 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1686 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 155 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1692 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 157 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1698 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 158 "decaf.y" /* yacc.c:1646  */
    {;}
#line 1704 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 160 "decaf.y" /* yacc.c:1646  */
    {
											if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											(yyvsp[-1].intval) = Q_ADD; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = INT;}
#line 1713 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 164 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											(yyvsp[-1].intval) = Q_SUB; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = INT;}
#line 1721 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 167 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											(yyvsp[-1].intval) = Q_MUL; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = INT;}
#line 1729 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 170 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											(yyvsp[-1].intval) = Q_DIV; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = INT;}
#line 1737 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 173 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											(yyvsp[-1].intval) = Q_MOD; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = INT;}
#line 1745 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 176 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != BOOL || (yyvsp[0].ast)->expr.type != BOOL)
												yyerror("Erreur: AND operator with non boolean value");
											(yyvsp[-1].intval) = Q_AND; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1753 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 179 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != BOOL || (yyvsp[0].ast)->expr.type != BOOL)
												yyerror("Erreur: OR operator with non boolean value");
											(yyvsp[-1].intval) = Q_OR; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast));}
#line 1761 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 182 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											(yyvsp[-1].intval) = Q_LT; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1769 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 185 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											(yyvsp[-1].intval) = Q_GT; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1777 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 188 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											(yyvsp[-1].intval) = Q_GEQ; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1785 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 191 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != INT || (yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											(yyvsp[-1].intval) = Q_LEQ; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1793 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 194 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != (yyvsp[0].ast)->expr.type )
												yyerror("Erreur: Comparaison de types différents");
											(yyvsp[-1].intval) = Q_EQ; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1801 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 197 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[-2].ast)->expr.type != (yyvsp[0].ast)->expr.type )
												yyerror("Erreur: Comparaison de types différents");
											(yyvsp[-1].intval) = Q_NEQ; (yyval.ast) = ast_new_node((yyvsp[-2].ast),(yyvsp[-1].intval),(yyvsp[0].ast)); (yyval.ast)->expr.type = BOOL;}
#line 1809 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 200 "decaf.y" /* yacc.c:1646  */
    {
											expr_val eval;
											eval.result = (yyvsp[0].literal).qop;
											eval.type = (yyvsp[0].literal).type;
											(yyval.ast) = ast_new_leaf(eval);
											}
#line 1820 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 206 "decaf.y" /* yacc.c:1646  */
    {
											Ht_item *val = lookup((yyvsp[0].stringval));
											expr_val eval;
											if(!val)
												yyerror("Erreur: Variable non déclarée\n");
											eval.result.u.offset = offset(val);
											eval.result.type = QO_ID;
											eval.type = val->value;
											(yyval.ast) = ast_new_leaf(eval);			
											}
#line 1835 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 216 "decaf.y" /* yacc.c:1646  */
    {
											if((yyvsp[0].ast)->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											quadop q1;
											q1.type = QO_CST;
											q1.u.cst = 0;
											expr_val eval;
											eval.type = INT;
											eval.result = q1;
											(yyval.ast) = ast_new_node(ast_new_leaf(eval),Q_SUB, (yyvsp[0].ast));
											(yyval.ast)->expr.type = INT;
										}
#line 1852 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 228 "decaf.y" /* yacc.c:1646  */
    {	if((yyvsp[0].ast)->expr.type != BOOL)
												yyerror("Erreur: NOT operator with non boolean value");
											(yyval.ast) = ast_new_node((yyvsp[0].ast), Q_NOT, (yyvsp[0].ast));
											(yyval.ast)->expr.type = BOOL;
										}
#line 1862 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 233 "decaf.y" /* yacc.c:1646  */
    {
											(yyval.ast) = (yyvsp[-1].ast); 	}
#line 1869 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 237 "decaf.y" /* yacc.c:1646  */
    {
									struct quadop q;
									q.u.cst = (yyvsp[0].intval);
									q.type = QO_CST;
									(yyval.literal).qop = q;
									(yyval.literal).type = INT;
								}
#line 1881 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 244 "decaf.y" /* yacc.c:1646  */
    {
									struct quadop q;
									q.u.cst = (yyvsp[0].intval);
									q.type = QO_CST;
									(yyval.literal).qop = q;
									(yyval.literal).type = INT;
								}
#line 1893 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 251 "decaf.y" /* yacc.c:1646  */
    {
									struct quadop q;
									q.u.cst = (yyvsp[0].boolval);
									q.type = QO_CST;
									(yyval.literal).qop = q;
									(yyval.literal).type = BOOL;
								}
#line 1905 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 259 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval) = (yyvsp[0].intval);}
#line 1911 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 260 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval) = (yyvsp[0].intval);}
#line 1917 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 262 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval) = Q_AFF;}
#line 1923 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 263 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval) = Q_AFFADD;}
#line 1929 "decaf.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 264 "decaf.y" /* yacc.c:1646  */
    {(yyval.intval) = Q_AFFSUB;}
#line 1935 "decaf.tab.c" /* yacc.c:1646  */
    break;


#line 1939 "decaf.tab.c" /* yacc.c:1646  */
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

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
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 266 "decaf.y" /* yacc.c:1906  */


void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	exit(0);
}
