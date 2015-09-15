#include "Disk.h"

/*
 * Tester functions
 */
// extern void runSimpleSimulation();
// extern void runSomeErrorTests();
// extern void runSimulation(int);

int main(int argc, const char **argv)
{
	// Format Disk
	initialiseFileSystem();
	
	// Write something
	writeBlock("doge.exe", 0, 511);
	writeBlock("dog.png", 0, 2);
	
	// Print
	displayDisk();
	
	// runSimulation(1);
	
    return 0;
}
