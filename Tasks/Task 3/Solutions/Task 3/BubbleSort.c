#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/time.h>
#include "data.h"

const char *fakefile = "/home/undergrad/j/jm617/File2.txt";
const int versionid = 1;

static key_t shmkey;
static int shmid;

static Sortdata *thedata;

static double datacopy[DATASIZE];

int childpids[4];

static void makekey()
{
	shmkey = ftok(fakefile, versionid);
	
	if (shmkey == (key_t) - 1) {
		printf("ftok() for shm failed\n");
		exit(1);
	}
}

static void getdata()
{
	makekey();

	int howbig = sizeof(Sortdata);
	
	void *where;
	
	shmid = shmget(shmkey, howbig, 0666);
	
	if (shmid == -1) {
		perror("shmget() failed\n");
		exit(1);
	}
	
	where = shmat(shmid, NULL, 0);
	if (where == NULL) {
		perror("shmat() failed\n");
		exit(1);
	}
	
	thedata = (Sortdata*) where;
}

static void copydata()
{
	int i;
	for (i = 0; i < DATASIZE; i++)
		datacopy[i] = thedata->data[i];
fprintf(stderr,"Some of the data\n");
for(i=0;i<5;i++) fprintf(stderr,"%10.4lf\n",datacopy[i]);

}

static void releasesharedmemory()
{
	void *where = (void*) thedata;
	int res = shmdt(where);

	if (res != 0) {
		perror("Error detaching shared memeory\n");
		exit(1);
	};
}

static void bubblesort()
{
	int swaps;
	fprintf(stderr,"I shall sort");
	do {
		swaps = 0;
		
		int i;
		for (i = 1; i < DATASIZE; i++) {

			if (datacopy[i-1] > datacopy[i]) {
				double temp = datacopy[i];
				datacopy[i] = datacopy[i-1];
				datacopy[i-1] = temp;
				swaps++;
			}
		}
	} while (swaps > 0);
	sleep(2);
}

static void timedsort()
{
	struct timeval starttime;
	gettimeofday(&starttime, NULL);
	bubblesort();
	
	struct timeval endtime;
	gettimeofday(&endtime, NULL);
	long musec_diff = (endtime.tv_sec - starttime.tv_sec)*1000000 + (endtime.tv_usec - starttime.tv_usec);
	fprintf(stderr,"Sorted %ld\n",musec_diff);
	thedata->timers[BUBBLE] = musec_diff;
fprintf(stderr,"Put my result in element %d of the result structure\n",BUBBLE);
}

int main(int argc, char ** argv)
{
	getdata();
	copydata();
	fprintf(stderr,"Have copied data\n");
	timedsort();
	fprintf(stderr,"have finished sort\n");
	releasesharedmemory();
	
	sleep(1);
	
	return (EXIT_SUCCESS);
}
