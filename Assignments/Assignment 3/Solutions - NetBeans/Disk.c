#include "Disk.h"
#include <limits.h>

/*
 * ===================== Main Functions =====================
 */

/**
 * Format:
 * Initialises the file system
 * • Sets all blocks to 0 (ie empty)
 * • Set all Directory Entries
 * @return void
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
	
	// Stats
	FilesCreated = 0;
	FilesDeleted = 0;
	FilesCompacted = 0;
	
	DirectoryEntries = 1;
	BlocksAllocated = 0;
	BlocksFree = 0;
}

/**
 * Inserting files into the disk
 * @param const char* File Name
 * @param int Size of file to be inserted
 * @return FileSysErrors Errors for the function that have occurred
 */
FileSysErrors createFile(const char* filename, int size)
{
	FileSysErrors errors;
	
	/* Error Checking Needed:
	 * • Returns DIRECTORY_FULL if maxfiles used
	 * • Returns DUPLICATE_NAME if there is already a file with the name
	 * • Returns ZERO_SIZE if size <= 0
	 * • Returns CREATE_FAIL if unable to find space for the file
	 */
	
	// 1. Check for simple errors before inserting
	errors = createFileErrorChecking(filename, size);
	
	if (errors != NO_ERR) {
		// Exit out
		return errors;
	} else {
		
                // 1. Check for full directory
                //if (DirectoryEntries > MAXFILES) {
                //    return DIRECTORY_FULL;
                //}
            
		// 2. Check for space and position to insert into HDD
		int position = findMemory(size);
		
		if (position < 0) {
			errors = CREATE_FAIL;
			return errors;
		} else {
			
			// 3. Insert into directory
			int count = 0;
			for (count = 0; count < MAXFILES; count++) {
				if (Directory[count].Size == 0) {
					
					// Copy in data
					strcpy(Directory[count].FileName, filename);
					Directory[count].Start = position;
					Directory[count].Size = size;
					
					break;
				}
			}
			
			// 4. Insert into Disk
			for (count = 0; count < size; count++) {
				// Mark positions as read
				Disk[position] = 1;
				
				// Increment the position
				position++;
			}
			
			// Update stats
			FilesCreated++;
			
			// All is good, return no error
			errors = NO_ERR;
			return errors;
		}
		
	}
}

/**
 * Deletes file from disk if available
 * @param const char* File Name
 * @return FileSysErrors appropriate errors
 */
FileSysErrors deleteFile(const char* filename)
{
	FileSysErrors errors;
	
	/* Error checking needed
	 * returns NON_EXISTENT_FILE if filename is invalid
	 */
	errors = deleteFileErrorChecking(filename);
	
	if (errors == NO_ERR) {
		
		// Temp vars
		DirectoryEntry tempDirEntry;
		
		// Get File properties from directory
		int counter = 0;
		for (counter = 0; counter < MAXFILES; counter++) {
			
			// Check for correct record
			if (strcmp(filename, Directory[counter].FileName) == 0) {
				tempDirEntry = Directory[counter];
				break;
			}
		}
		
		// Remove from Bit Map
		for (counter = 0; counter < tempDirEntry.Size; counter++) {
			
			// Format Section
			Disk[tempDirEntry.Start + counter] = 0;
			
		}
		
		// Remove from directory structure
		for (counter = 0; counter < MAXFILES; counter++) {
			
			// Check for correct record
			if (strcmp(filename, Directory[counter].FileName) == 0) {
				// Set empty file name
				int filenameCount = 0;
				for (filenameCount = 0; filenameCount < NAMESIZE; filenameCount++) {
					Directory[counter].FileName[filenameCount] = '\0';
				}
				
				// Set empty size
				Directory[counter].Size = 0;
				
				// Set empty record starting position
				Directory[counter].Start = 0;
				
				// Set empty Symbol file representation (period)
				Directory[counter].Symbol = '.';
				
				break;
			}
		}
		
		// Update stats
		FilesDeleted++;
		
		return errors;
		
	} else {
		return errors;
	}
}

/**
 * Writes a single block value to the disk represented as an int
 * @param const char* File name
 * @param int block - position at which the block is inserted
 * @param int value - the actual value of what needs to be inserted into the position
 * @return FileSysErrors Errors in execution
 */
FileSysErrors writeBlock(const char* filename, int block, int value)
{
	FileSysErrors errors;
	
	/* Error checking needed
	 * • return NON_EXISTENT_FILE if filename is invalid / not found
	 * • return INVALID_BLOCK if requested block is invalid (<0, or >= size of file)
	 */
	
	errors = readWriteBlockErrorChecking(filename, block);
	
	if (errors == NO_ERR) {
		
		// Temps
		int counter = 0;
		DirectoryEntry tempDirEntry;
		
		// Update block in file
		for (counter = 0; counter < MAXFILES; counter++) {
			// Compare all filenames
			if (strcmp(Directory[counter].FileName, filename) == 0) {
				
				// Get the size of record
				tempDirEntry = Directory[counter];
				
				break;
			}
		}
		
		// Get the block location and store
		Disk[block + tempDirEntry.Start] = value;
		
		errors = NO_ERR;
		return errors;
		
	} else {
		return errors;
	}
}

/**
 * Read block - stores value read in integer address
 * @param const char * File Name
 * @param int block
 * @param int* address to value
 * @return FileSysErrors any errors
 */
FileSysErrors readBlock(const char* filename, int block, int *vp)
{
	FileSysErrors error;
	
	error = readWriteBlockErrorChecking(filename, block);
	
	if (error == NO_ERR) {
		
		// Temps
		int counter = 0;
		DirectoryEntry tempDirEntry;
		
		// Update block in file
		for (counter = 0; counter < MAXFILES; counter++) {
			// Compare all filenames
			if (strcmp(Directory[counter].FileName, filename) == 0) {
				
				// Get the size of record
				tempDirEntry = Directory[counter];
				
				break;
			}
		}
		
		// Specify the address
		*vp = Disk[block + tempDirEntry.Start];
		
		error = NO_ERR;
		return error;
		
	} else {
		return error;
	}
}

/**
 * Defrag - groups files and defrags the disk
 * @return int the number of free blocks
 */
int compactFiles()
{
	
	// Global Counter
	int counter = 0;
	
	// Get current directory listing (start with lowest)
	DirectoryEntry *current = getLowestDirPositionEntry(0, 0);

	// Get the next lowest directory listing (next postition above current dir listing)
	DirectoryEntry *next = getLowestDirPositionEntry(current->Start, 0);
	
	// Loop until all records are done, there is nothing smaller
	while (next != NULL) {
		
		if ((current->Size + current->Start) == next->Start) {
			// Already compact, nothing we can do
		} else {
			
			// Get gap = (current dir (size + position) - next dir (position))
			int gap = next->Start - (current->Size + current->Start);
			int appart = gap;
			
			// 2. Update bit map
			
			for (counter = 0; counter < next->Size; counter++) {
				Disk[next->Start - appart] = Disk[next->Start + counter];
				Disk[next->Start + counter] = 0;
				appart--;
			}
			
			// Work on next cause thats what we are bringing closer
			// 1. File Listing related fix up
			next->Start = next->Start - gap;
			
			// loop over (next dir listing file size) {
			
			// bitmap[next-file.position - gap] = bitmap[next-file.position]
			// bitmap[next-file.position] = 0
			
		}
		
		current = next;
		next = getLowestDirPositionEntry(current->Start, 1);
		
	}
	
	// Get number of free blocks
	int freeBlocks = 0;
	for (counter = 0; counter < DISKBLOCKS; counter++) {
		
		if (Disk[counter] == 0) {
			freeBlocks++;
		}
	}
	
	// Stats
	FilesCompacted++;
	
	return freeBlocks;
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
	
	printf("\n\n");
	
}

// show history
// number of files created, deleted, number of entries in directory,
// number of blocks still free, number of compactions performed
void showHistory()
{
	
	int counter = 0;
	for (counter = 0; counter < DISKBLOCKS; counter++) {
		
		if (Disk[counter] == 0) {
			BlocksFree++;
		}
	}
	
	BlocksAllocated = DISKBLOCKS - BlocksFree;
	
	for (counter = 0; counter < MAXFILES; counter++) {
		
		if (Directory[counter].Size != 0) {
			DirectoryEntries++;
		}
		
	}
	
	printf("\n");
	printf("Number of file create operations        : %d\n", FilesCreated);
	printf("Number of file delete operations        : %d\n", FilesDeleted);
	printf("Number of file compaction operations    : %d\n", FilesCompacted);
	printf("\n\n");
	printf("Current number of directory entries     : %d\n", DirectoryEntries);
	printf("Current number of disk blocks allocated : %d\n", BlocksAllocated);
	printf("Current number of disk blocks free      : %d\n", BlocksFree);
	printf("\n");
}

/*
 * ===================== Helper Functions =====================
 */

/**
 * Gets the next lowest directory position above the last position
 * @param int Last position to be used
 * @return Directory Entry details
 */
DirectoryEntry* getLowestDirPositionEntry(int last, bool zeroPosition)
{
	int smallest = INT_MAX;
	DirectoryEntry* lowest = NULL;
	int counter = 0;
	
	// Initial
	if ((last == 0) && (zeroPosition == 1)) {
		
		// Check if lowest is 0
		for (counter = 0; counter < MAXFILES; counter++) {
			
			if (Directory[counter].Start == last) {
				
				// It is found, return the directory listing
				return &Directory[counter];
				
			}
		}
	}
	
	bool nothingGreaterThanLast = 0;
	
	for (counter = 0; counter < MAXFILES; counter++) {
		
		
		
		if (Directory[counter].Start > last) {
			
			// Set flag that there is something greater
			nothingGreaterThanLast = 1;
			
			// if the value we are testing is smaller than the smallest
			if (Directory[counter].Start < smallest) {
				
				// Set that value as the smallest
				smallest = Directory[counter].Start;
				// Set the pointer
				lowest = &Directory[counter];
			}
			
		}
		
	}
	
	if (nothingGreaterThanLast == 0) {
		
		// Nothing more to be found
		return NULL;
	} else {
		return lowest;
	}
	
}

/**
 * First Fit - Find Memory:
 * Start at begginig of HDD and search through for
 * an appropriate sequence of fee blocks = to the requested size
 * @param int File size
 * @return int Block starting location (-1 if error)
 */
int findMemory(int fileSize)
{
	
	// Setup counters
	int diskPosition = 0;
	int sequencePosition = 0;
	
	// Cycle through the whole disk
	for (diskPosition = 0; diskPosition < DISKBLOCKS; diskPosition++) {
		
		// Check at postion _diskPosition for a valid sequence
		for (sequencePosition = 0; sequencePosition <= fileSize; sequencePosition++) {
			
			// Get the current position of array while in the loop
			// Check the next element in the array
			int currentPositition = diskPosition + sequencePosition;
			
			// Check if we are outside the range of position
			if (currentPositition > DISKBLOCKS) {
				
				// return error
				return -1;
			}
			
			if (Disk[currentPositition] != 0) {
				// Non empty block found, sequence is interuppted
				// fast forward main position
				diskPosition = diskPosition + sequencePosition;
				
				// break out of loop, nothing else to find
				break;
			}
			
			// Only when gone though loop with no break set the flag
			if (sequencePosition == fileSize) {
				
				return diskPosition;
				
			}
			
		}
		
	}
	
	// Nothing found, error out
	return -1;
	
}

/**
 * Check errors before writing block
 * @param const char * File Name
 * @param int block to be inserted
 * @param int value to be inserted
 * @return FileSysErrors
 * • return NON_EXISTENT_FILE if filename is invalid / not found
 * • return INVALID_BLOCK if requested block is invalid (<0, or >= size of file)
 */
FileSysErrors readWriteBlockErrorChecking(const char *filename, int block)
{
	FileSysErrors error;
	
	int counter = 0;
	int tempSize = 0;
	
	// 1. Check if filename is found
	{
		bool nameFound = 0;
		
		for (counter = 0; counter < MAXFILES; counter++) {
			// Compare all filenames
			if (strcmp(Directory[counter].FileName, filename) == 0) {
				
				// Get the size of record
				tempSize = Directory[counter].Size;
				
				// Strings are equal
				nameFound = 1;
				break;
			}
		}
		
		if (nameFound == 0) {
			error = NON_EXISTENT_FILE;
			return error;
		}
	}
	
	// 2. Invalid Filename
	{
		if (strlen(filename) <= 0) {
			error = NON_EXISTENT_FILE;
			return error;
		}
	}
	
	// 3. Invalid Blocks
	{
		// INVALID_BLOCK if requested block is invalid (<0, or >= size of file)
		if ((block < 0) || (block >= tempSize)) {
			error = INVALID_BLOCK;
			return error;
		}
	}
	
	// Say no errors for now if nothing found
	error = NO_ERR;
	return error;
}

/**
 * Check simple errors before creating a file
 * @param const char* File Name
 * @param int Size of the file
 * @return FileSysErrors Errors found so far
 * 	• Returns DIRECTORY_FULL if maxfiles used
 * 	• Returns DUPLICATE_NAME if there is already a file with the name
 * 	• Returns ZERO_SIZE if size <= 0
 */
FileSysErrors createFileErrorChecking(const char *filename, int size)
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
				break;
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
			if (strcmp(Directory[counter].FileName, filename) == 0) {
				// Strings are equal
				error = DUPLICATE_NAME;
				return error;
			}
		}
	}
	
	// 3. Check for 0 Size
	{
		if (size <= 0) {
			error = ZERO_SIZE;
			return error;
		}
	}
	
	// Say no errors for now if nothing found
	error = NO_ERR;
	return error;
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
FileSysErrors deleteFileErrorChecking(const char *fileName)
{
	// Local Vars
	FileSysErrors error;
	int counter = 0;
	
	// 1. Check if is not existing filename
	{
		bool flag = 0;
		
		for (counter = 0; counter < MAXFILES; counter++) {
			// Compare all filenames
			if (strcmp(Directory[counter].FileName, fileName) == 0) {
				// Strings are equal
				// Set flag a match is found
				flag = 1;
				
				// Exit out
				break;
			}
		}
		
		// Test for non existance
		if (flag != 1) {
			error = NON_EXISTENT_FILE;
			return error;
		}
	}
	
	// 2. Check Invalid filename
	{
		if (strlen(fileName) <= 0) {
			error = NON_EXISTENT_FILE;
			return error;
		}
	}
	
	// Say no errors for now
	error = NO_ERR;
	return error;
}
