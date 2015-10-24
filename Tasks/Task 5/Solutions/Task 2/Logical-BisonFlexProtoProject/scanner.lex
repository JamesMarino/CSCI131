%option noyywrap

%{
#include "parser.h"
#include <stdio.h>
#include <string.h>
%}


WHITE [ \t]+
NAME [a-z]+
QUOTESTRING \"(\\.|[^"])*\"
OCTDIGITS [0-7]+

%%


PROGRAM         return PROGRAM;
END             return END;
VAR             return DECLARE;
READOCT return READ;
PRINTOCT return PRINT;
"&" return AND;
"|" return OR;
"^" return XOR;
"(" return LPAR;
")" return RPAR;
"=" return ASSIGN;
,               return COMMA;
;               return SEMICOLON;
"~" return NOT;

{OCTDIGITS} {

    char *ptr;
    long val;
    val = strtol(yytext, &ptr, 8);
    yylval.ival = val & 0177777;
    return NUMBER;
}


{QUOTESTRING}   {
                /* Remove quote marks */
                char* ptr = yytext; 
                int len = strlen(yytext) - 1;
                char* qstring = malloc(len);
                
                ptr++;
                strncpy(qstring, ptr, len-1);
                yylval.str_val = qstring;
                return QSTRING;
                }
MSG             return MSG;
NEWL            return NEWL; 
{NAME}          { yylval.str_val = strdup(yytext);  return IDENTIFIER; }

{WHITE}         ;
\n              { yylineno++; }
.               { printf("Unrecognized token%s!\n", yytext); exit(1);  }
%%