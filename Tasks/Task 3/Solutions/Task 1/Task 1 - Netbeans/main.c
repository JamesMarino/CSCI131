#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>

static char searchstring[150];

static void checkfile(char *path)
{
	struct stat finfo;
	int statresult;
	
	statresult = lstat(path, &finfo);
	
	if (statresult != 0) {
		perror("stat failed for directory");
		
		exit(1);
	}
	
	int filesize = finfo.st_size;
	void *content = malloc(filesize);
	int fd = open(path, O_RDONLY);
	int numread = read(fd, content, filesize);
	close(fd);
	
	if (numread != filesize) {
		printf("Unable to read %s fully\n", path);
		free(content);
		return;
	}
	
	int i;
	char *p = (char*)content;
	
	for (i = 0; i < numread; i++) {
		if (!isascii(*p)) {
			free(content);
			return;
		}
		
		p++;
	}
	
	int len = strlen(searchstring);
	int count = 0;
	
	p = (char*) content;
	
	for (;;) {
		char *res = strstr(p, searchstring);
		if (res == NULL) {
			break;
		}
		
		count++;
		p = res + len;
	}
	
	free(content);
	
	if (count > 0) {
		printf("%4d         %s", count, path);
	}
}

static void checkDirectory(DIR *dir, char *name)
{
	struct dirent *dp;
	
	rewinddir(dir);
	
	while ((dp = readdir(dir)) != NULL) {
		char *aname = dp->d_name;
		
		if (0 == strcmp("..", aname)) {
			continue;
		}
		
		if (0 == strcmp(".", aname)) {
			continue;
		}
		
		unsigned char entrytype = dp->d_type;
		
		if ((entrytype == DT_BLK) || (entrytype == DT_CHR) || (entrytype == DT_FIFO) || (entrytype == DT_SOCK)) {
			continue;
		}
		
		char path[1025];
		strcpy(path, name);
		strcat(path, "/");
		strcat(path, aname);
		
		struct stat finfo;
		int statresult;
		statresult = lstat(path, &finfo);
		
		if (statresult != 0) {
			perror("Stat failed for the file: ");
			exit(1);
		}
		
		if (entrytype == DT_DIR) {
			DIR *subdir;
			subdir = opendir(path);
			
			if (subdir == NULL) {
				if (errno == EACCES) {
					fprintf(stdout, "Cannot access %s\n", path);
					continue;
				} else {
					perror("Opendir failed");
					exit(1);
				}
			}
			
			checkDirectory(subdir, path);
			closedir(subdir);
		} else {
			if (entrytype == DT_REG) {
				checkfile(path);
			}
		}
	}
}

int main(int argc, const char **argv)
{
	printf("Searching for files with given string that exists in directories and sub directories\n");
	printf("Enter string whose occurances are to be found: ");
	scanf("%s", searchstring);
	printf("Enter directory path name: ");
	
	char rootdirectory[PATH_MAX];
	scanf("%s", rootdirectory);
	
	DIR *dir = opendir(rootdirectory);
	
	if (dir == NULL) {
		perror("Failed to open that directory: ");
		exit(1);
	}
	
	printf("\nStarting search for files with string %s\n", searchstring);
	printf("#occurances File Name\n");
	
	checkDirectory(dir, rootdirectory);
	
	closedir(dir);
	
	return 0;
}
