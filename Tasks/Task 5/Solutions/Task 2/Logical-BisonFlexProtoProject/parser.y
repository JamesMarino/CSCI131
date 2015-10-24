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
};

void undefined(char *varname) { printf("Reference to undefined variable %s\n", varname); exit(1); }

char* programname;

%}


%union {
  int ival; 
  char* str_val;
};

%token <ival> DECLARE PROGRAM END SEMICOLON COMMA
%token <ival> LPAR RPAR
%token <ival> ASSIGN PRINT READ MSG NEWL
%token <ival> NUMBER
%token <ival> AND OR XOR NOT
%token <str_val> IDENTIFIER QSTRING

%type <ival> expression
%type <ival> term
%type <ival> factor

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
    |READ IDENTIFIER SEMICOLON {
            int num;
            scanf("%o", &num);
            
            Data *var = find($2);
            if (var == NULL) undefined($2);
            var->value = num &0177777;
           }

| PRINT expression SEMICOLON { printf("%o", $2); }
| IDENTIFIER ASSIGN expression SEMICOLON {
    Data *var = find($1);
    if (var == NULL) undefined($1);
    var->value = $3;
    free($1);
};

expression: term {$$ = $1;}
    | expression AND term {$$ = $1 & $3;}
    | expression OR term {$$ = $1 | $3;}
    | expression XOR term {$$ = $1 ^ $3;};

term: factor {$$ = $1;}
    | NOT factor {$$ = (~$2) & 0177777; };

factor: LPAR expression RPAR {$$ = $2;}
    | NUMBER {$$ = $1;}
    | IDENTIFIER {
        int d;
        
        d = value($1);

        $$ = d;
        free($1);
        };
    

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

