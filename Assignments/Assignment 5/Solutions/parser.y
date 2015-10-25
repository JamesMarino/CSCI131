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
 return 0;
 }

void undefined(char *varname) { printf("Reference to undefined variable %s\n", varname); exit(1); }

FILE *output;

int comparecount = 0;
int lblcount = 0;
int nestingstack[10];
int nestinglevel = 0;

char* programname;
char header[100];
%}


%union {
  int ival; 
  char* str_val;
};

%token <ival> DECLARE PROGRAM END SEMICOLON COMMA
%token <ival> LPAR RPAR
%token <ival> ASSIGN PRINT READ MSG NEWL READOCT PRINTOCT
%token <ival> PLUS MINUS MULTIPLY DIVIDE
%token <ival> LESS GE
%token <ival> WHILE DO ENDWHILE
%token <str_val> IDENTIFIER QSTRING
%token <ival> NUMBER

%type <ival> expression
%type <ival> term
%type <ival> factor

//Grammar for programming language
%%
program: PROGRAM pname declarations statements END {  fprintf(output,"exit\n"); printsyms(output); printstrings(output); };

pname: IDENTIFIER SEMICOLON                             { programname = $1; 
                                                          strcpy(header,"\"Program "); strcat(header,$1); strcat(header,"\"");
                                                          int ival = addstring(header);
                                                          fprintf(output,".origin 2400\napplication: writeline\n msg%d\ncall newline\n", ival);
                                                        }

declarations: 
    | DECLARE declarationlist SEMICOLON         {  /* printf("Parsed declarations\n"); */ };

declarationlist: 
    declarationlist COMMA IDENTIFIER                   {   insert($3); }
   | IDENTIFIER                                         {   insert($1); };

statements: statements  statement 
    | statement;

statement: 
    | NEWL SEMICOLON                                      {   fprintf(output,"call newline\n"); }
    | MSG QSTRING SEMICOLON                             {   int ival = addstring($2); fprintf(output,"writeline\nmsg%d\n", ival); }
    | READ IDENTIFIER SEMICOLON                         {   fprintf(output,"call readint\nmov r0,%s\n",$2);  }
   | PRINT expression SEMICOLON                         {   fprintf(output,"mov (sp)+,r0\ncall printint\n"); }
    | READOCT IDENTIFIER SEMICOLON                         {   fprintf(output,"call readoct\nmov r0,%s\n",$2);  }
   | PRINTOCT expression SEMICOLON                         {   fprintf(output,"mov (sp)+,r0\ncall printoct\n"); }
   | IDENTIFIER ASSIGN expression SEMICOLON             {
                                                            fprintf(output,"mov (sp)+,%s\n", $1);
                                                            free($1);
                                                        }
    | WHILE                                             { fprintf(output,"wh%d:  ", lblcount); nestingstack[nestinglevel++] = lblcount; }
        comparison                                  
        DO                                              { fprintf(output,"tst r0\nbeq ewh%d\n",lblcount);   lblcount++;}
        statements ENDWHILE SEMICOLON                   { int lbl; nestinglevel--; lbl = nestingstack[nestinglevel]; fprintf(output,"br wh%d\newh%d: ",lbl,lbl);};

expression: term                                        {   }
    | expression PLUS term                              { fprintf(output,"add (sp)+,(sp)\n"); }
    | expression MINUS term                             { fprintf(output,"neg (sp)\nadd (sp)+,(sp)\n"); };

term: factor                                            {  }
    | term MULTIPLY factor                              { fprintf(output,"mov (sp)+,r3\nmul (sp)+,r3\nmov r3,-(sp)\n"); }
    | term DIVIDE factor                                { fprintf(output,"mov (sp)+,r0\n clr r2\nmov (sp)+,r3\n div r0,r2\nmov r2,-(sp)\n");};

factor: LPAR expression RPAR                            {  }
    | IDENTIFIER                                        { fprintf(output,"mov %s,-(sp)\n", $1); }
    | NUMBER                                            { fprintf(output,"mov #%o,-(sp)\n", $1); };

comparison: LPAR expression LESS expression RPAR            { 
                                                          fprintf(output,"clr r0\n");  
                                                          fprintf(output,"cmp (sp)+,(sp)+\n");
                                                          fprintf(output,"ble cmp%d\n",comparecount);
                                                          fprintf(output,"inc r0\n");
                                                          fprintf(output,"cmp%d: ",comparecount);
                                                          comparecount++;
                                                        }
    | LPAR expression GE expression RPAR             { 
                                                          fprintf(output,"clr r0\n");  
                                                          fprintf(output,"cmp (sp)+,(sp)+\n");
                                                          fprintf(output,"bgt cmp%d\n",comparecount);
                                                          fprintf(output,"inc r0\n");
                                                          fprintf(output,"cmp%d: ",comparecount);
                                                          comparecount++;
                                                        }
%%

void copyfile1(char* filename) {
  FILE* tmp = fopen(filename,"r");
  char ch;
  while(( ch = fgetc(tmp) ) != EOF ) fputc(ch,output);
  fclose(tmp);
}

int main(int argc, char** argv)
{
  char filename[128];
  printf("Enter name of file with program to be interpreted : ");
  scanf("%s",filename);
  FILE* input = fopen(filename,"r");
  if ( input == NULL ) { printf( "Could not open %s \n " , filename); exit (0);}
  yyin = input; 
  output = fopen("pdp11.txt","w");
  copyfile1("os.txt");
  copyfile1("libc.txt");
  yyparse();
  fprintf(output,".end osstart\n");
  fclose(output);
  return 0;
}

