/* 
 * File:   data.h
 * Author: neil
 *
 * Created on 17 September 2014, 10:42 AM
 */

#ifndef DATA_H
#define	DATA_H
#include <stdio.h>
struct data {
    char* name;
    int value;
};

typedef struct data Data;

Data *find(char* varname);

void insert(char* varname);

int value(char* varname);

void printsyms(FILE* output);

#endif	/* DATA_H */

