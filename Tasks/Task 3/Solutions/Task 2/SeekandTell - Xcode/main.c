#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// Tables
#define MAXRECORDS 0x20000
#define KEYBITS 17
#define SHIFT 3
#define SHIFTMASK 0xe0000
#define KEYMASK 0x1ffff

struct record
{
	char city[40];
	char country[16];
	char continent[20];
	int cap;
	int pop;
};

typedef struct record Record;
static int recordsize = sizeof(Record);

int genkey(Record* ptr) {
	int k = 0;
	// add in letters of city name first
	char* cptr = ptr->city;
	while (*cptr) {
		k = k << SHIFT; // move left
		int save = k & SHIFTMASK;
		
		// save bits
		save = save >> KEYBITS;
		k = k & KEYMASK;
		k = k ^ save;
		k = k ^ (*cptr);
		cptr++;
		
	}
	
	// and now the country name
	cptr = ptr->country;
	
	while (*cptr) {
		k = k << SHIFT;
		
		// move left
		int save = k & SHIFTMASK;
		
		// save bits
		save = save >> KEYBITS;
		k = k & KEYMASK;
		k = k ^ save;
		k = k ^ (*cptr);
		cptr++;
	}
	
	return k;
}

void save(Record* ptr, int k, int fd) {
	int startposn = k;
	int posn = startposn;
	int collisions = 0;
	for (;;) {
		// editor-fold defaultstate="collapsed" desc="Find next position"›
		long byteposn = posn*recordsize;
		// Move to that spot
		off_t spot = lseek(fd, byteposn, SEEK_SET);
		
		if (spot < 0) {
			perror("Disk seek failed");
			exit(1);
		}
		
		//</editor-fold>
		Record existrec;
		memset(&existrec, 0, recordsize);
		
		int numread = read(fd, &existrec, recordsize);
		
		if ((numread < 0) || ((numread > 0) && (numread != recordsize))) {
			perror("Disk read failed");
			exit(1);
		}
		
		if (numread == 0) {
			spot = lseek(fd, byteposn, SEEK_SET);
			
			if (spot < 0) {
				perror("Disk seek failed");
				exit(1);
			}
			
			int numwritten = write(fd, ptr, recordsize);
			if (numwritten != recordsize) {
				perror("Write to disk failed");
				exit(1);
			}
			
			fsync(fd);
			return;
		}
		
		if (0 == strlen(existrec.city)) {
			spot = lseek(fd, byteposn, SEEK_SET);
			if (spot < 0) {
				perror("Disk seek failed");
				exit(1);
			}
			
			int numwritten = write(fd, ptr, recordsize);
			if (numwritten != recordsize) {
				perror("Write to disk failed");
				exit(1);
			}
			
			fsync(fd);
			return;
		}
		
		printf("%d : Collision at %d\n", ++collisions, posn);
		printf("\t%s, %s\n", ptr->city, ptr->country);
		printf("\t%s, %s\n", existrec.city, existrec.country);
		
		posn = (posn + 17) % MAXRECORDS;
		
		if (posn == startposn) {
			printf("No room in file\n");
			exit(1);
		}
	}
}

void load_data(const char * filenmae, int fd) {
	FILE* input = fopen(filenmae, "r");
	for (;;) {
		int n;
		char cityname[256];
		char countryname[256];
		int capital; int population;
		char continent[256];
		
		n = fscanf(input, "%s %s %d %d %s", cityname, countryname, &capital, &population, continent);
		
		if (feof(input))
			break;
		
		if (n != 5) {
			printf("Error at %s cityname", cityname);
			exit(1);
		}
		Record r;
		memset(&r, 0, recordsize);
		strncpy(r.city, cityname, 39);
		strncpy(r.country, countryname, 15);
		strncpy(r.continent, continent, 19);
		r.cap = capital; r.pop = population;
		int key = genkey(&r);
		save(&r, key, fd);
	}
}

int getSearchRecord(Record *ptr) {
	// Return -1 if end of input - program to exit
	// Return 0 if invalid input - program to loop requesting data
	// Return 1 with filled in record if data 93
	char city[256];
	char country[256];
	int nread = scanf("%s", city);
	
	if (nread <= 0) {
		if (feof(stdin)) return -1;
		return 0;
	}
	
	  nread = scanf("%s", country);
	  if (nread <= 0) return 0;
	  int len = strlen(city);
	  if ((len < 2) || (len > 39)) {
		  printf("Inva1id city name\n");
				 return 0;
				 }
				 len = strlen(country);
				 if ((len < 2) || (len > 15)) {
					 printf("Inva1id country name\n");
					 return 0;
				 }
				 strncpy(ptr->city, city, 39);
				 strncpy(ptr->country, country, 15);
				 return 1;
				 }
			 
void checkForRecord(Record *ptr, int fd);

void perform_lookups(int fd) {
	printf("Enter city name and country");
	printf("Names with spaces bailj, be entered with underscore characters\n");
	printf("\te.g. 'Baton BataeL would be entered as 'Baton_BalageL\n");
	for (;;) {
		printf("Search for:\n");
		Record r;
		memset(&r, 0, recordsize);
		int inpt = getSearchRecord(&r);
		
		if (inpt < 0) return;
		if (inpt == 0) continue;
		
		checkForRecord(&r, fd);
	}
}

void checkForRecord(Record *ptr, int fd) {
	int startposn = genkey(ptr);
	int posn = startposn;
	for (;;) {
		long byteposn = posn*recordsize;
		// Move to that spot
		off_t spot = lseek(fd, byteposn, SEEK_SET);
		if (spot < 0) {
			perror("Disk seek failed");
			exit(1);
		}
		// Try reading in a record from that spot
		Record existrec;
		memset(&existrec, 0, recordsize);
		int numread = read(fd, &existrec, recordsize);
		
		if ((numread < 0) || ((numread > 0) && (numread != recordsize))) {
			perror("Disk read failed");
			exit(1);
			
		}
		
		if (numread == 0) {
			// Have read from an unwritten part of file. There is no
			// such city/country combination
			printf("Sorry, have no data for that city\n");
			return;
			
		}
		
		// Check that record does match by city name and country name
		int cmpl;
		strcmp(existrec.city, ptr->city);
		int cmp2;
		strcmp(existrec.country, ptr->country);
		if ((0 == cmpl) && (0 == cmp2)) {
			// Found it
			printf("%s, %s, in %s\n",
				   existrec.city, existrec.country, existrec.continent);
			printf("\tHas a population of %d\n", existrec.pop);
			if (existrec.cap) printf("\tIt is the nation's capital\n");
			return;
			
		}
		
		// It was simply a hash collision so move on
		posn = (posn + 17) % MAXRECORDS;
		
		if (posn == startposn) {
			// This check could fail if mgxpgggpgs were a multiple of 17
			printf("Sorry, have no data for that city\n");
			
			return;
			
		}
	}
}

int main(int argc, char** argv)
{
	printf("Record size is %d, ~700 records is %d\n", recordsize, 700*recordsize);
	printf("Key length is %d bits, number records %d, potential file size %d\n",
		   KEYBITS, MAXRECORDS, MAXRECORDS*recordsize);
	
	int fd = open("/Users/james/UOW/UOW/UOW Repositories/CSCI131/Tasks/Task 3/Support/cities.txt", O_RDWR | O_CREAT | O_TRUNC);
	load_data("/Users/james/UOW/UOW/UOW Repositories/CSCI131/Tasks/Task 3/Support/cities.txt", fd);
	perform_lookups(fd);
	unlink("/Users/james/UOW/UOW/UOW Repositories/CSCI131/Tasks/Task 3/Support/cities.txt");
	
	return (EXIT_SUCCESS);
}

