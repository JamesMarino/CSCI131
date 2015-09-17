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
	createFile("doge.png", 26);
	createFile("test.exe", 11);
	createFile("snjs.sh", 1);
	createFile("snjs.sh1", 100);
	
	deleteFile("doge.png");
	
	createFile("danielWest.exe", 10);
	createFile("asjad.psd", 150);
	
	deleteFile("snjs.sh");
	
	createFile("wordflow.app", 1);
	
	createFile("doges.png2", 70);
	createFile("blah.osd", 430);
	
	writeBlock("doges.png2", 1, 420);
	
	int pointerToData;
	
	readBlock("doges.png2", 1, &pointerToData);
	
	displayDisk();
	
    return 0;
}
