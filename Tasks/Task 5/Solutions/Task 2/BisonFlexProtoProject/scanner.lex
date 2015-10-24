%option noyywrap

%{
#include "parser.h"
#include <stdio.h>
#include <string.h>
%}


WHITE [ \t]+
NAME [a-z]+
QUOTESTRING \"(\\.|[^"])*\"

%%


PROGRAM         return PROGRAM;
END             return END;
VAR             return DECLARE;
,               return COMMA;
;               return SEMICOLON;

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