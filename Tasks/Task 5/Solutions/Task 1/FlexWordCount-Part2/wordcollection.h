#ifndef WORDCOLLECTION_H
#define	WORDCOLLECTION_H

#ifdef	__cplusplus
extern "C" {
#endif

    struct word {
        char* _str;
        int _count;
    };
    
    typedef struct word Word;
    
    int getNumWords();
    
    Word* getWord(int ndx);
    
    void addWord(char* astr);

#ifdef	__cplusplus
}
#endif

#endif	/* WORDCOLLECTION_H */

