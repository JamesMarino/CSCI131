#include "Disk.h"

/*
 * Tester functions
 */
extern void runSimpleSimulation();
extern void runSomeErrorTests();
extern void runSimulation(int);

int main(int argc, const char **argv)
{
    
    printf("Error Tests:\n");
    runSomeErrorTests();
    printf("\n");
    
    printf("Simple Simulation:\n");
    runSimpleSimulation();
    printf("\n");
    
    printf("Full Simulation:\n");
    runSimulation(1);
    printf("\n");
    
    sleep(1);
	
    return 0;
}
