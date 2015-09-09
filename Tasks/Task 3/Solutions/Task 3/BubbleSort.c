#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys//time.h>
#include "data.h"

const char *fakefile = "";
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

static void createsharedmemory()
{
	int howbig = sizeof(Sortdata);
	
	void *where;
	
	shmid = shmget(shmkey, howbig, 0666 | IPC_CREAT | IPC_EXCL);
	
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

static void releasememory()
{
	void *where = (void*) thedata;
	int res = shmdt(where);
	
	if (res != 0) {
		perror("Error detaching shared mmeory\n");
		exit(1);
	}
	
	shmctl(shmid, IPC_RMID, 0);
}

static void writedata()
{
	int i;
	
	for (i = 0; i < DATASIZE; i++)
		thedata->data[i] = (double) rand();
	
	for (i = 0; i < 4; i++)
		thedata->timers[i] = 0;
	
}

static void startbubble()
{
	pid_t childpid = fork();
	
	if(childpid == -1) {
		perror("fork");
		exit(1);
	}
	
	if (childpid == 0) {
		execl("./Bubble", "Bubble", (char*) 0);
	}
	
	childpids[BUBBLE] = childpid;
}

static void startinsert()
{
	
}

static void startselection()
{
	
}

static void startqsort()
{
	
}

static void waitforchildren()
{
	int countfinished = 0;
	
	do {
		int status;
		pid_t childfinished = wait(&status);
		countfinished++;
		if(childfinished == childpids[BUBBLE]) printf("Bubble\n");
		if(childfinished == childpids[INSERT]) printf("Insert\n");
		if(childfinished == childpids[SELECT]) printf("Select\n");
		if(childfinished == childpids[QSORT]) printf("Qsort\n");
	} while (countfinished < 4);
	
}

static void reportruntimes()
{
	printf("All child processes have finished\n");
	printf("Bubble %ld musec\n", thedata->timers[BUBBLE]);
	printf("Insert %ld musec\n", thedata->timers[INSERT]);
	printf("Select %ld musec\n", thedata->timers[SELECT]);
	printf("QSort %ld musec\n", thedata->timers[QSORT]);
}

/*Sorting*/
static void getdata()
{
	int howbig = sizeof(Sortdata);
	
	void *where;
	
	shmid = shmget(shmkey, howbig, 0666 | IPC_CREAT | IPC_EXCL);
	
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
}

static void releasesharedmemory()
{
	
}

static void bubblesort()
{
	int swaps;
	
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
}

static void timedsort()
{
	struct timeval starttime;
	gettimeofday(&starttime, NULL);
	bubblesort();
	
	struct timeval endtime;
	gettimeofday(&endtime, NULL);
	long musec_diff = (endtime.tv_sec - starttime.tv_sec)*1000000 + (endtime.tv_usec - starttime.tv_usec);
	
	thedata->timers[BUBBLE] = musec_diff;
}

int main(int argc, char ** argv)
{
	getdata();
	copydata();
	
	timedsort();
	
	releasesharedmemory();
	
	return (EXIT_SUCCESS);
}
