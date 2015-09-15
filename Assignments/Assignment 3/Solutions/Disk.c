#include "Disk.h"

/**
 * Initialises the file system
 * Sets all blocks to 0 (ie empty
 */
void initialiseFileSystem()
{
	
}

// createFile returns DIRECTORY_FULL if maxfiles used
// Returns DUPLICATE_NAME if there is already a file with the name
// Returns ZERO_SIZE if size <= 0
// Returns CREATE_FAIL if unable to find space for
// the file FileSysErrors createFile(const char* filename, int size);
// deleteFile returns NON_EXISTENT_FILE if filename is invalid
FileSysErrors deleteFile(const char* filename)
{
	FileSysErrors errors;
	return errors;
}

// readBlock and writeBlock will return NON_EXISTENT_FILE if
// filename is invalid, and INVALID_BLOCK if requested block
// is invalid (<0, or >= size of file)
// blocks would have 512bytes but for simulation content of block represented by
// single integer value
FileSysErrors writeBlock(const char* filename, int block, int value)
{
	FileSysErrors errors;
	return errors;
}

// Read block stores value read in integer whose address is passed as
// the third argument
FileSysErrors readBlock(const char* filename, int block, int* vp)
{
	FileSysErrors errors;
	return errors;
}

// compactFiles returns number of free blocks - should be all in one large group
// at end of disk after compaction
int compactFiles()
{
	return 0;
}

// Display disk – show directory contents and provide some form of map
// identifying the mapping of files to disk blocks
void displayDisk()
{
	
}

// show history
// number of files created, deleted, number of entries in directory,
// number of blocks still free, number of compactions performed
void showHistory()
{
	
}
