#ifndef DATA_H
#define DATA_H

#define BUBBLE 0
#define INSERT 1
#define SELECT 2
#define QSORT  3

#define DATASIZE 131072

struct sortdata
{
	long timers[4];
	double data[DATASIZE];
};

typedef struct sortdata Sortdata;

#endif /* DATA_H */
