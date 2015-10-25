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
     WHILE = 278,
     DO = 279,
     ENDWHILE = 280,
     IDENTIFIER = 281,
     QSTRING = 282,
     NUMBER = 283
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
#define WHILE 278
#define DO 279
#define ENDWHILE 280
#define IDENTIFIER 281
#define QSTRING 282
#define NUMBER 283




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 30 "parser.y"
{
  int ival; 
  char* str_val;
}
/* Line 1529 of yacc.c.  */
#line 110 "parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

