#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "wordcollection.h"

#define MAXWORDS 4096

static int wordcount = 0;

static Word* thewords[MAXWORDS];

int findNdx(char* astr)
{
    int min = 0;
    int max = wordcount - 1;
    
    if (wordcount == 0) return 0;
    
    while (min <= max) {
        int pos = (min + max) / 2;
        Word* aword = thewords[pos];
        int cmp = strcmp(aword->_str, astr);
        
        if (cmp < 0) {
            min = pos + 1;
        } else if (cmp == 0) {
            return pos;
        } else {
            max = pos - 1;
        }
    }
    
    return min;
}

void addnewWordAt(char* astr, int ndx)
{
    Word* newword = malloc(sizeof(Word));
    newword->_count = 1;
    newword->_str = strdup(astr);
    thewords[ndx] = newword;
    wordcount++;
}

void addWordAt(char* astr, int ndx)
{
    if (ndx == wordcount) {
        addnewWordAt(astr, ndx);
    } else if (0 == strcmp(astr, thewords[ndx]->_str)) {
        thewords[ndx]->_count++;
    } else {
        int i;
        for (i = wordcount; i > ndx; i--)
            thewords[i] = thewords[i - 1];
        addnewWordAt(astr, ndx);
    }
}

int getNumWords()
{
    return wordcount;
}

Word* getWord(int ndx)
{
    assert(ndx >= 0);
    assert(ndx < wordcount);
    return thewords[ndx];
}

void addWord(char* astr)
{
    int ndx = 0;
    
    char* ptr = astr;
    
    for (; *ptr != '\0'; ptr++) *ptr = tolower(*ptr);
    
    int where = findNdx(astr);
    addWordAt(astr, where);
}
