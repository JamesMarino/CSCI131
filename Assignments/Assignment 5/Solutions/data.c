
#include <stdlib.h>
#include <string.h>
#include "data.h"

#define MAXVARS 100
#define MAXSTRINGS 20


static Data *variables[MAXVARS];
static char *strings[MAXSTRINGS];

static int nvars = 0;
static int nstrings = 0;

Data *find(char* varname) {

    int i = 0;
    while(i<nvars) {

        if(0==strcmp(varname, variables[i]->name)) return variables[i];
        i++;
    }

    return NULL;
}

void insert(char* varname) {

    Data *ptr = find(varname);
    if(ptr!=NULL) {
        printf("Doubly defined variable name %s\n", varname);
        exit(1);
    }
    ptr = malloc(sizeof(Data));

    ptr->name = varname;
    
    variables[nvars++] = ptr;

}


void printsyms(FILE* output) {
    int i;

    for(i=0;i<nvars;i++)
        fprintf(output,"%s: .word 0\n", variables[i]->name);
}

int addstring(char* str)
{
    int res = nstrings;
    strings[nstrings++] = str;
    return res;
}

void printstrings(FILE* output)
{
    int i;

    for(i=0;i<nstrings;i++)
        fprintf(output,"msg%d: .string %s\n", i,strings[i]);
}