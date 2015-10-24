%option noyywrap

%{
#include <stdlib.h>
#include <stdio.h>
#include "wordcollection.h"
%}

NAME [a-zA-Z][a-zA-Z_0-9]*

%%

{NAME} addWord(yytext);
. ;
\n ;
%%

int main(int argc, char** argv)
{
    FILE* input;
    char filename[128];

    int i;
    printf("Enter name of data file : ");
    scanf("%s", filename);

    input = fopen(filename, "r");
    yyin = input;
    yylex();
    printf("Found %d distinct words\n", getNumWords());
    printf("Word            : count \n");

    for (i = 0; i < getNumWords(); i++) {
        Word* aword = getWord(i);
        printf("%20s : %d\n", aword->_str, aword->_count);
    }

    sleep(1);
    return 0;
}
