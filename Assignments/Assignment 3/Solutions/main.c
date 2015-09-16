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
	createFile("doge.png", 502);
	createFile("test.exe", 11);
	createFile("snjs.sh", 1);
	createFile("snjs.sh1", 1);
	//createFile("doges.png2", 70);
	//createFile("blah.osd", 430);
	printf("\n");
	displayDisk();
	
	// runSimulation(1);
	
    return 0;
}
