#include "Disk.h"

/*
 * ===================== Main Functions =====================
 */

/**
 * Format:
 * Initialises the file system
 * • Sets all blocks to 0 (ie empty)
 * • Set all Directory Entries
 */
void initialiseFileSystem()
{
	int count = 0;
	
	// Set all the disk blocks to 0
	for (count = 0; count < DISKBLOCKS; count++) {
		Disk[count] = 0;
	}
	
	// Set all Directory entries to Empty values
	for (count = 0; count < MAXFILES; count++) {
		
		// Set empty file name
		int filenameCount = 0;
		for (filenameCount = 0; filenameCount < NAMESIZE; filenameCount++) {
			Directory[count].FileName[filenameCount] = '\0';
		}
		
		// Set empty size
		Directory[count].Size = 0;
		
		// Set empty record starting position
		Directory[count].Start = 0;
		
		// Set empty Symbol file representation (period)
		Directory[count].Symbol = '.';
	}
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

/**
 * Write a file to the disk
 * @param const char* File name
 * @param int block - approx position to be inserted? need to ask neil
 * @param int value - the size of the file being inserted
 * @return FileSysErrors
 */
FileSysErrors writeBlock(const char* filename, int block, int value)
{
	FileSysErrors errors;
	
	/* Error Checking Needed:
	 * readBlock and writeBlock will return NON_EXISTENT_FILE if filename is invalid
	 * INVALID_BLOCK if requested block is invalid (<0, or >= size of file)
	 * blocks would have 512bytes but for simulation content of block represented by
	 *  single integer value
	 */
	
	// 1. Check for simple errors before inserting
	errors = checkErrors(filename, block, value);
	
	if (errors != NO_ERR) {
		// Exit out
		return errors;
	} else {
		
		// 2. Check for space and position to insert into HDD
		int position = findMemory(value);
		
		if (position < 0) {
			errors = CREATE_FAIL;
			return errors;
		} else {
			
			// Insert into directory
			int count = 0;
			for (count = 0; count < MAXFILES; count++) {
				if (Directory[count].Size == 0) {
					
					// Copy in data
					strcpy(Directory[count].FileName, filename);
					Directory[count].Start = position;
					Directory[count].Size = value;
					
					break;
				}
			}
			
			// Insert into Disk
			for (count = 0; count < value; count++) {
				// Mark positions as read
				Disk[position] = 1;
				
				// Increment the position
				position++;
			}
			
			// All is good
			errors = NO_ERR;
			return errors;
		}
		
	}
	
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
// ??????? is this a defrag?
int compactFiles()
{
	return 0;
}

/**
 * Display disk – show directory contents and provide some form of map
 * identifying the mapping of files to disk blocks
 */
void displayDisk()
{
	// Setup final array to be printed
	char ASCIIDisk[DISKBLOCKS];
	
	// Counter and Initialisation
	int counter = 0;
	// Set all blocks to have period
	for (counter = 0; counter < DISKBLOCKS; counter++) {
		ASCIIDisk[counter] = '.';
	}
	
	// Setup Symbol counter
	char symbol = 65;
	
	// Iterate through directory
	for (counter = 0; counter < MAXFILES; counter++) {
		
		// Symbol counter
		int symbolCounter = 0;
		int position = Directory[counter].Start;
		
		// Go through the whole size of the file
		for (symbolCounter = 0; symbolCounter < Directory[counter].Size; symbolCounter++) {
			// Set starting postion
			ASCIIDisk[position] = symbol;
			
			// Update position
			position++;
		}
		
		// Set the symbol used
		Directory[counter].Symbol = symbol;
		
		// Get a new symbol
		// Check bounds - just incase...
		if (symbol >= 91) {
			symbol = 97;
		} else if (symbol >= 123) {
			symbol = 65;
		} else {
			symbol++;
		}
		
	}
	
	// Print out the list of files
	printf("Directory Listing:   Filename:    Start:   Size:  (Symbol): \n");
	// Print out directory ls
	for (counter = 0; counter < MAXFILES; counter++) {
		// No Blank records
		if (Directory[counter].Size != 0) {
			printf("%d                    %s       %d      %d      %c       \n",
				   counter+1,
				   Directory[counter].FileName,
				   Directory[counter].Start,
				   Directory[counter].Size,
				   Directory[counter].Symbol
				   );
		}
	}
	
	printf("\n");
	// Print out the display of files
	for (counter = 0; counter < DISKBLOCKS; counter++) {
		
		// Print in 64 Block lines
		if ((counter % 64) == 0) {
			printf("\n");
		}
		
		// Out the characters
		printf("%c", ASCIIDisk[counter]);
	}
	
}

// show history
// number of files created, deleted, number of entries in directory,
// number of blocks still free, number of compactions performed
void showHistory()
{
	
}

/*
 * ===================== Helper Functions =====================
 */

/**
 * First Fit - Find Memory:
 * Start at begginig of HDD and search through for
 * an appropriate sequence of fee blocks >= the requested size
 * @param int File size
 * @return int Block starting location (-1 if error)
 */
int findMemory(int fileSize)
{
	
	// Setup counters
	int diskPosition = 0;
	int sequencePosition = 0;
	
	// Flag
	// Flag if not one bit of free space for the file is found
	// Assume that there is no free space
	bool freeSpace = 0;
	
	// Cycle through the whole disk
	for (diskPosition = 0; diskPosition < DISKBLOCKS; diskPosition++) {
		
		// Flag if sequence is interupted
		// Assume false
		bool continuosSequence = 0;
		
		// Check at postion _diskPosition for a valid sequence
		for (sequencePosition = 0; sequencePosition < fileSize; sequencePosition++) {
			
			// Check if any interupts along the disk are found
			if (Disk[diskPosition + sequencePosition] == 1) {
				
				// Update the position to start looking at the start of the interupt
				// (some sense of effeciency)
				//diskPosition = diskPosition + sequencePosition;
				
				// Exit out and try
				break;
			} else {
				continuosSequence = 1;
			}
		}
		
		// Exit out if continuous sequence is found
		if (continuosSequence == 1) {
			
			// A valid position has been found, there is free space
			freeSpace = 1;
			break;
		}
		
	}
	
	// Check if not one continuous sequence for the size is avaliable
	// Signal local error if there is error
	if (freeSpace == 1) {
		return diskPosition;
	} else {
		return -1;
	}
	
}

/**
 * Check simple errors before performing directory operations IO
 * Relating To:
 * 	• DIRECTORY_FULL
 * 	• DUPLICATE_NAME
 * 	• ZERO_SIZE
 * 	• NON_EXISTENT_FILE
 *	• INVALID_BLOCK
 * @param const char * File Name
 * @return FileSysErrors File system errors
 */
FileSysErrors checkErrors(const char *fileName, int block, int size)
{
	// Local Vars
	FileSysErrors error;
	int counter = 0;
	
	// 1. Check directory is full
	{
		bool freeSpaceFound = 0;
		
		for (counter = 0; counter < MAXFILES; counter++) {
			if (Directory[counter].Size == 0) {
				// There is free space
				freeSpaceFound = 1;
			}
		}
		
		if (freeSpaceFound == 0) {
			error = DIRECTORY_FULL;
			return error;
		}
	}
	
	// 2. Check duplicate file names
	{
		for (counter = 0; counter < MAXFILES; counter++) {
			// Compare all filenames
			if (strcmp(Directory[counter].FileName, fileName) == 0) {
				// Strings are equal
				error = DUPLICATE_NAME;
				return error;
			}
		}
	}
	
	// 3. Check for 0 Size
	{
		if (size == 0) {
			error = ZERO_SIZE;
			return error;
		}
	}
	
	// 4. Check Invalid filename
	{
		if (strlen(fileName) <= 0) {
			error = NON_EXISTENT_FILE;
			return error;
		}
	}
	
	// 5. Check invalid block
	//    INVALID_BLOCK if requested block is invalid (<0, or >= size of file)
	{
		if ((block < 0) || (block >= size)) {
			error = INVALID_BLOCK;
			return error;
		}
	}
	
	// Say no errors for now
	error = NO_ERR;
	return error;
}
































