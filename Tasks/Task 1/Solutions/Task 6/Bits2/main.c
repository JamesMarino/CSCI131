/* 
 * Name: James Marino
 * Task: Task 1 - Task 6
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

enum INPUTSTYLE {
    OCTAL, HEX
};

typedef enum INPUTSTYLE Style;

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

static unsigned short readunsignedshort(const char* prompt, Style s)
{
    for (;;) {
        printf("%s", prompt);
        
        unsigned short enteredval;
        int transferred;
        if (s == OCTAL) transferred = scanf("%ho", &enteredval);
        else transferred = scanf("%hx", &enteredval);
        
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
        
        unsigned short sentry = readunsignedshort("Enter Octal: ", 0);
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
        
        unsigned short guess = readunsignedshort(" = ", 0);
        unsigned short value = one & two;
        
        if (guess == value)
            printf("Correct\n");
        else
            printf("Incorrect\n");
    }
}

static unsigned short readasbits() {
    char buf[256];
    printf("bits: ");
    gets(buf);
    
    unsigned short val = 0;
    int len = strlen(buf);
    int i = 0;
    
    while ((i < len) && (isspace(buf[i]))) i++;
    
    while ((i < len) && ((buf[i] == '0') || (buf[i] == '1'))) {
        val = val << 1;
        
        if (buf[i] == '1') {
            val = val | 01;
        }
            
        i++;
    }
    
    return val;
}


int main(int argc, char** argv) {

    int correct = 0;
    
    do {
        printf("Enter bit string for short integer :\n");
        unsigned short valb = readasbits();
        
        printf("Value as 16-bit bit string : %s\n", binarystr(valb));
        printf("Enter hex and octal representations :\n");
        unsigned short valh = readunsignedshort("hex :", HEX);
        unsigned short valo = readunsignedshort("octal:", OCTAL);
        
        if ((valb == valh) && (valb == valo)) {
            correct++;
            printf("Correct\n");
        } else {
            printf("Incorrect! Start again\n");
            printf("Value as octal: %6ho\n", valb);
            printf("Value as hex: %6hx\n", valb);
        }
        
        char discard[100];
        gets(discard);
    } while (correct < 5);
    
    return (EXIT_SUCCESS); 
}
