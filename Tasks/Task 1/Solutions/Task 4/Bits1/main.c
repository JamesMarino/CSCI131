/* 
 * Name: James Marino
 * Task: Task 1 - Task 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>


static char* binarystr(unsigned short val)
{
    static char buffer[18];
    memset(buffer, 0, 18);
    
    int mask = SHRT_MIN;
    int count;
    
    for (count = 0; count < 16; count++) {
        char ch = (val & mask) ? '1' : '0';
        val = val << 1;
        buffer[count] = ch;
    }
    
    buffer[count] = 'b';
    
    return buffer;
}

static unsigned short randomunsignedshort()
{
    int val = rand() % 65536;
    
    val -= 327668;
    
    short sval = (short)val;
    
    return (unsigned short)sval;
}

static unsigned short readunsignedshort(const char* prompt)
{
    for (;;) {
        printf("%s", prompt);
        
        unsigned short enteredval;
        int transferred = scanf("%ho", &enteredval);
        
        if (transferred != 1) {
            if (feof(stdin)) {
                printf("Input terminated, program will exit\n");
                exit(0);
            }
            
            printf("Invalid Input");
            scanf("%*[^\n]%*c");
            
            continue;
        }
        
        return enteredval;
    }
}

static void testoctal()
{
    printf("Testing Understanding of Binary and Octal Patterns\n");
    
    int i, correct = 0;
    
    for (i = 0; i < 8; i++) {
        unsigned short sval = randomunsignedshort();
        
        printf("Binary Value: %s\n", binarystr(sval));
        
        unsigned short sentry = readunsignedshort("Enter Octal: ");
        if (sentry == sval)
            printf("Correct\n");
        else
            printf("Incorrect\n");
    }
}

static void testands()
{
    printf("Testing 'and' operations on bit patterns represented as octal\n");
    
    int i = 0;
    
    for (i = 0; i < 8; i++) {
        unsigned short one = randomunsignedshort();
        unsigned short two = randomunsignedshort();
        
        printf("%6ho & %6ho ", one, two);
        
        unsigned short guess = readunsignedshort(" = ");
        unsigned short value = one & two;
        
        if (guess == value)
            printf("Correct\n");
        else
            printf("Incorrect\n");
    }
}

int main(int argc, char** argv) {

    srand(2673);
    
    testoctal();
    
    testands();
    
    return (EXIT_SUCCESS);
}

