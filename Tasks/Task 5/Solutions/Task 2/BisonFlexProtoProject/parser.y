%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "data.h"
extern FILE *yyin;

extern int yylineno;
extern char* yytext;

int yylex(); 
int yyerror(const char *p) { printf("Error : %s\n",p); 
          printf("About line %d\n", yylineno);
          printf("Near %s\n", yytext);
}

void undefined(char *varname) { printf("Reference to undefined variable %s\n", varname); exit(1); }

char* programname;

%}


%union {
  int ival; 
  char* str_val;
};

%token <ival> DECLARE PROGRAM END SEMICOLON COMMA
%token <ival> MSG NEWL 
%token <str_val> IDENTIFIER QSTRING


//Grammar for programming language
%%
program: PROGRAM pname declarations statements END {   printf("Variables declared in program %s\n", programname); 
                                                    printsyms(stdout); };

pname: IDENTIFIER SEMICOLON                             { programname = $1;  }

declarations: DECLARE declarationlist SEMICOLON         {  printf("Parsed declarations\n");  };

declarationlist: declarationlist COMMA IDENTIFIER       {   insert($3); }
   | IDENTIFIER                                         {   insert($1); };

statements: statements  statement 
    | statement;

statement: 
    NEWL SEMICOLON                                      {   printf("\n"); }
   | MSG QSTRING SEMICOLON                               {   printf("%s", $2); };
    

%%

int main(int argc, char** argv)
{
 char filename[128];
  printf("Enter name of file with program to be interpreted : ");
  scanf("%s",filename);
  FILE* input = fopen(filename,"r");
  if ( input == NULL ) { printf( "Could not open %s \n " , filename); exit (0);}
  yyin = input; 

  yyparse();
  sleep(1);
  return 0;
}

