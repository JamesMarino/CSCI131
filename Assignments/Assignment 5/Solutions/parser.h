/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DECLARE = 258,
     PROGRAM = 259,
     END = 260,
     SEMICOLON = 261,
     COMMA = 262,
     LPAR = 263,
     RPAR = 264,
     ASSIGN = 265,
     PRINT = 266,
     READ = 267,
     MSG = 268,
     NEWL = 269,
     READOCT = 270,
     PRINTOCT = 271,
     PLUS = 272,
     MINUS = 273,
     MULTIPLY = 274,
     DIVIDE = 275,
     LESS = 276,
     GE = 277,
     LE = 278,
     GT = 279,
     EQ = 280,
     NEQ = 281,
     WHILE = 282,
     DO = 283,
     ENDWHILE = 284,
     IF = 285,
     THEN = 286,
     ENDIF = 287,
     IDENTIFIER = 288,
     QSTRING = 289,
     NUMBER = 290
   };
#endif
/* Tokens.  */
#define DECLARE 258
#define PROGRAM 259
#define END 260
#define SEMICOLON 261
#define COMMA 262
#define LPAR 263
#define RPAR 264
#define ASSIGN 265
#define PRINT 266
#define READ 267
#define MSG 268
#define NEWL 269
#define READOCT 270
#define PRINTOCT 271
#define PLUS 272
#define MINUS 273
#define MULTIPLY 274
#define DIVIDE 275
#define LESS 276
#define GE 277
#define LE 278
#define GT 279
#define EQ 280
#define NEQ 281
#define WHILE 282
#define DO 283
#define ENDWHILE 284
#define IF 285
#define THEN 286
#define ENDIF 287
#define IDENTIFIER 288
#define QSTRING 289
#define NUMBER 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 30 "parser.y"
{
	int ival;
	char* str_val;
}
/* Line 1529 of yacc.c.  */
#line 124 "parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

