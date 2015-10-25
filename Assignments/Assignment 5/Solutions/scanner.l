%option noyywrap

%{
#include "parser.h"
#include <stdio.h>
#include <string.h>
%}

WHITE [ \t]+
NAME [a-z]+
INTNUM [0-9]+
QUOTESTRING \"(\\.|[^"])*\"

%%

PROGRAM         return PROGRAM;
END             return END;
VAR             return DECLARE;
,               return COMMA;
;               return SEMICOLON;
"+"             return PLUS;
"-"             return MINUS;
"*"             return MULTIPLY;
"/"             return DIVIDE;
"("             return LPAR;
")"             return RPAR;
"="             return ASSIGN;
"<"             return LESS;
">="            return GE;
WHILE           return WHILE;
DO              return DO;
ENDWHILE        return ENDWHILE;

{QUOTESTRING}   {
                yylval.str_val = strdup(yytext);
                return QSTRING;
                }

READ            return READ;
READOCT         return READOCT;
PRINT           return PRINT;
PRINTOCT        return PRINTOCT;
MSG             return MSG;
NEWL            return NEWL;

{NAME}          {
				yylval.str_val = strdup(yytext);
				return IDENTIFIER;
				}

{INTNUM}        {
				yylval.ival = atoi(yytext);
				return NUMBER;
				}

{WHITE}         ;

\n              {
				yylineno++;
				}

.               {
				printf("Unrecognized token%s!\n", yytext);
				exit(1);
				}

%%
