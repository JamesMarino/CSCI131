#ifndef DISK_H
#define DISK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Constants
 */
#define MAXFILES 64
#define DISKBLOCKS 512
#define NAMESIZE 32


/*
 * Structures
 */
// File System Errors
enum FileSystemErrors {
	NO_ERR,
	DIRECTORY_FULL,
	CREATE_FAIL,
	NON_EXISTENT_FILE,
	DUPLICATE_NAME,
	INVALID_BLOCK,
	ZERO_SIZE
};

// Directory Instance
struct DirEntry {
	// Filename of current file
	char FileName[NAMESIZE];
	// The start block number
	int Start;
	// Number of blocks allocated to file
	int Size;
	// Symbol representation
	char Symbol;
};

/*
 * Naming Conventions
 */
// File System Errors
typedef enum FileSystemErrors FileSysErrors;
// Directory Instance
typedef struct DirEntry DirectoryEntry;
// Bitmap
typedef int BitMap;
// Bool - 'cause we all wanna use C++
typedef int bool;


/*
 * Data
 */
// Directory List
static DirectoryEntry Directory[MAXFILES];
// Disk Bitmap - The actual HardDisk representation
static BitMap Disk[DISKBLOCKS];

/*
 * Main Functions
 */
void initialiseFileSystem();

FileSysErrors createFile(const char* filename, int size);

FileSysErrors deleteFile(const char* filename);

FileSysErrors writeBlock(const char* filename, int block, int value);

FileSysErrors readBlock(const char* filename, int block, int * vp);

int compactFiles();

void displayDisk();

void showHistory();

/*
 * Helper Functions
 */
int findMemory(int fileSize);

FileSysErrors createFileErrorChecking(const char *filename, int size);

FileSysErrors deleteFileErrorChecking(const char *fileName);

FileSysErrors readWriteBlockErrorChecking(const char *filename, int block);

#endif
