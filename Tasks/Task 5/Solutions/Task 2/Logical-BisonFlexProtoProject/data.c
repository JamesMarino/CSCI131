#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

#define MAXVARS 100

static int nvars = 0;

static Data *variables[MAXVARS];


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
    ptr->value = 0;
    ptr->name = varname;
    
    variables[nvars++] = ptr;

}

int value(char* varname) {
    Data *ptr = find(varname);
    if(ptr==NULL) {
        printf("Undefined variable %s\n" , varname);
        exit(1);
    }

    return ptr->value;
}

void printsyms(FILE* output) {
    int i;

    for(i=0;i<nvars;i++)
        fprintf(output,"%s\n", variables[i]->name);
}
