#ifndef DISK_H
#define DISK_H

#include <stdio.h>
#include <stdlib.h>

#define MAXFILES 64
#define DISKBLOCKS 512
#define NAMESIZE 8


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

// createFile returns DIRECTORY_FULL if maxfiles used
// Returns DUPLICATE_NAME if there is already a file with the name
// Returns ZERO_SIZE if size <= 0
// Returns CREATE_FAIL if unable to find space for
// the file FileSysErrors createFile(const char* filename, int size);
// deleteFile returns NON_EXISTENT_FILE if filename is invalid
FileSysErrors deleteFile(const char* filename);

// readBlock and writeBlock will return NON_EXISTENT_FILE if
// filename is invalid, and INVALID_BLOCK if requested block
// is invalid (<0, or >= size of file)
// blocks would have 512bytes but for simulation content of block represented by
// single integer value
FileSysErrors writeBlock(const char* filename, int block, int value);

// Read block stores value read in integer whose address is passed as
// the third argument
FileSysErrors readBlock(const char* filename, int block, int* vp);

// compactFiles returns number of free blocks - should be all in one large group
// at end of disk after compaction
int compactFiles();

// Display disk – show directory contents and provide some form of map
// identifying the mapping of files to disk blocks
void displayDisk();

// show history
// number of files created, deleted, number of entries in directory,
// number of blocks still free, number of compactions performed
void showHistory();

/*
 * Helper Functions
 */


#endif
