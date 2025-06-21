/* A Bison parser, made by GNU Bison 3.8.2.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYSY_TAB_H_INCLUDED
# define YY_YY_SYSY_TAB_H_INCLUDED
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
    IDENT = 258,                   /* IDENT  */
    INCONST = 259,                 /* INCONST  */
    FLOACONST = 260,               /* FLOACONST  */
    CONST = 261,                   /* CONST  */
    INT = 262,                     /* INT  */
    FLOAT = 263,                   /* FLOAT  */
    VOID = 264,                    /* VOID  */
    IF = 265,                      /* IF  */
    ELSE = 266,                    /* ELSE  */
    WHILE = 267,                   /* WHILE  */
    BREAK = 268,                   /* BREAK  */
    CONTINUE = 269,                /* CONTINUE  */
    RETURN = 270,                  /* RETURN  */
    PLUS = 271,                    /* PLUS  */
    MINUS = 272,                   /* MINUS  */
    MUL = 273,                     /* MUL  */
    DIV = 274,                     /* DIV  */
    MOD = 275,                     /* MOD  */
    ASSIGN = 276,                  /* ASSIGN  */
    EQ = 277,                      /* EQ  */
    NEQ = 278,                     /* NEQ  */
    LT = 279,                      /* LT  */
    GT = 280,                      /* GT  */
    LTE = 281,                     /* LTE  */
    GTE = 282,                     /* GTE  */
    AND = 283,                     /* AND  */
    OR = 284,                      /* OR  */
    NOT = 285,                     /* NOT  */
    LPAREN = 286,                  /* LPAREN  */
    RPAREN = 287,                  /* RPAREN  */
    LBRACE = 288,                  /* LBRACE  */
    RBRACE = 289,                  /* RBRACE  */
    LBRACK = 290,                  /* LBRACK  */
    RBRACK = 291,                  /* RBRACK  */
    COMMA = 292,                   /* COMMA  */
    SEMICOLON = 293,               /* SEMICOLON  */
    IFX = 294                      /* IFX  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "sysy.y"

    char* str_val;
    struct Node* node_ptr;

#line 108 "sysy.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYSY_TAB_H_INCLUDED  */
