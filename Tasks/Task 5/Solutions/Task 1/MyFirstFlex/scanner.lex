%option noyywrap

%{
#include <stdlib.h>
long sum = 0;
int val = 0;
int count = 0;
int line_num = 1;
%}

DIGIT [0-9]
NUMBER {DIGIT}+
WHITE [ \t]+
NAME [a-zA-Z][a-zA-Z_0-9]*

%%

{NUMBER} {
    count++;
   val = atoi(yytext);
   printf("found a number, value %d, on line %d\n",val, line_num);
   sum += val;
   }
\n { line_num++ ; }
{WHITE} ;
{NAME} ;
%%

int main(int argc, char** argv)
{
    yylex();
    printf("I thought there were %d number on the %d lines of input\n", count, line_num);
    printf("I make the sum %ld\n",sum);
    sleep(1);
    return 0;
}


