#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

struct Data {
	int InputBase;
	int OutputBase;
	unsigned short ValueA;
	unsigned short ValueB;
	unsigned short Result;
};

typedef enum {BITWISE, SHIFT_LOGICAL, SHIFT_ARITHMETIC} QuestionType;
typedef struct Data Data;

int getRandomNumberBetween(int min, int max)
{
	int random = min;
	
	// Gaurd
	while (1) {
		random = (rand() % (max + 1)) + min;
		
		if ((random >= min) && (random <= max)) {
			break;
		}
	}
	
	return random;
}

Data generateData()
{
	Data temp;
	
	temp.InputBase = getRandomNumberBetween(1, 3);
	temp.OutputBase = getRandomNumberBetween(1, 3);
	temp.ValueA = getRandomNumberBetween(0, SHRT_MAX);
	temp.ValueB = getRandomNumberBetween(0, SHRT_MAX);
	temp.Result = 0;
	
	return temp;
}

unsigned short computeLogicOperation(Data data, QuestionType type)
{
	unsigned short result = 0;
	
	if (type == BITWISE) {
		int random = getRandomNumberBetween(0, 5);
		
		switch (random) {
			case 0:
				printf("~a");
				result = ~data.ValueA;
				break;
				
			case 1:
				printf("a & b");
				result = data.ValueA & data.ValueB;
				break;
				
			case 2:
				printf("a & ~b");
				result = data.ValueA & ~data.ValueB;
				break;
				
			case 3:
				printf("a | b");
				result = data.ValueA | data.ValueB;
				break;
				
			case 4:
				printf("a ^ b");
				result = data.ValueA ^ data.ValueB;
				break;
				
			case 5:
				printf("~(~a & ~b)");
				result = ~(~data.ValueA & ~data.ValueB);
				break;
				
			default:
				result = 0;
				break;
		}
		
	} else if ((type == SHIFT_LOGICAL) || (type == SHIFT_ARITHMETIC)) {
		
		int random = getRandomNumberBetween(0, 1);
		int randomValue = getRandomNumberBetween(1, 3);
		
		switch (random) {
			case 0:
				printf("a << %d", randomValue);
				result = data.ValueA << randomValue;
				break;
				
			case 1:
				printf("a >> %d", randomValue);
				result = data.ValueA >> randomValue;
				break;
				
			default:
				result = 0;
				break;
		}
		
	}
	
	printf("\n");
	return result;
}

void printBinary(unsigned short decimal)
{
	int mask = SHRT_MIN, count = 0;
	
	for (count = 0; count < 16; count++) {
		char ch = (decimal & mask) ? '1' : '0';
		decimal = decimal << 1;
		printf("%c", ch);
	}
}

unsigned short scanBinary() {
	char buf[256];
	fgets(buf, sizeof(buf), stdin);
	
	unsigned short val = 0;
	int len = (int)strlen(buf);
	int i = 0;
	
	while ((i < len) && (isspace(buf[i]))) {
		i++;
	}
	
	while ((i < len) && ((buf[i] == '0') || (buf[i] == '1'))) {
		val = val << 1;
		
		if (buf[i] == '1') {
			val = val | 01;
		}
		
		i++;
	}
	
	return val;
}

void printGeneratedValues(Data data, QuestionType type)
{
	switch (data.InputBase) {
		case 1:
			if (type == BITWISE) {
				printf("a = 0x%hx\nb = 0x%hx\n", data.ValueA, data.ValueB);
			} else {
				printf("a = 0x%hx\n", data.ValueA);
			}
			break;
			
		case 2:
			if (type == BITWISE) {
				printf("a = ");
				printBinary(data.ValueA);
				printf("\n");
				
				printf("b = ");
				printBinary(data.ValueB);
				printf("\n");
			} else {
				printf("a = ");
				printBinary(data.ValueA);
				printf("\n");
			}
			break;
			
		case 3:
			if (type == BITWISE) {
				printf("a = 0%ho\nb = 0%ho\n", data.ValueA, data.ValueB);
			} else {
				printf("a = 0%ho\n", data.ValueA);
			}
			break;
			
		default:
			break;
	}
}

unsigned short getUserAnswer(Data data)
{
	unsigned short temp = 0;
	
	printf("Enter Answer (Base 10: %d) in ", data.Result);
	
	switch (data.OutputBase) {
		case 1:
			printf("Hexadecimal: ");
			scanf("%hx", &temp);
			break;
			
		case 2:
			printf("Binary: ");
			temp = scanBinary();
			break;
			
		case 3:
			printf("Octal: ");
			scanf("%ho", &temp);
			break;
			
		default:
			break;
	}
	
	return temp;
}

int correctAnswer(unsigned short user, unsigned short machine)
{
	if (user == machine) {
		// True
		printf("Correct\n\n");
		return 1;
	} else {
		// False
		printf("Incorrect, start all over again\n\n");
		return 0;
	}
}

int correctAnswerArithmetic(signed short user, signed short machine)
{
	if (user == machine) {
		// True
		printf("Correct\n\n");
		return 1;
	} else {
		// False
		printf("Incorrect, start all over again\n\n");
		return 0;
	}
}

void runTestBitwise()
{
	int count = 0;
	
	while (count < 12) {
		unsigned short userResult = 0;
		
		// Set a seed for random
		srand((unsigned int)time(NULL));
		
		// Generate Random Data
		Data data = generateData();
		
		// Print generated values
		printGeneratedValues(data, BITWISE);
		
		// Print and compute logic calculation
		data.Result = computeLogicOperation(data, BITWISE);
		
		// Get User Answer
		userResult = getUserAnswer(data);
		
		// Check Answer
		if (correctAnswer(userResult, data.Result)) {
			count++;
		} else {
			count = 0;
		}
	}
}

void runTestShiftLogical()
{
	int count = 0;
	
	while (count < 6) {
		unsigned short userResult = 0;
		
		// Set a seed for random
		srand((unsigned int)time(NULL));
		
		// Generate Random Data
		Data data = generateData();
		
		// Print generated values
		printGeneratedValues(data, SHIFT_LOGICAL);
		
		// Print and compute logic calculation
		data.Result = computeLogicOperation(data, SHIFT_LOGICAL);
		
		// Get User Answer
		userResult = getUserAnswer(data);
		
		// Check Answer
		if (correctAnswer(userResult, data.Result)) {
			count++;
		} else {
			count = 0;
		}
	}
}

void runTestShiftArithmetic()
{
	int count = 0;
	
	while (count < 6) {
		unsigned short userResult = 0;
		
		// Set a seed for random
		srand((unsigned int)time(NULL));
		
		// Generate Random Data
		Data data = generateData();
		
		// Print generated values
		printGeneratedValues(data, SHIFT_ARITHMETIC);
		
		// Print and compute logic calculation
		data.Result = computeLogicOperation(data, SHIFT_ARITHMETIC);
		
		// Get User Answer
		userResult = getUserAnswer(data);
		
		// Cast to signed, Arithmetic Shifting
		// Check Answer
		if (correctAnswer((signed short)userResult, (signed short)data.Result)) {
			count++;
		} else {
			count = 0;
		}
	}
}

int main(int argc, const char * argv[]) {
	
	printf("Bitwise Operations\n");
	//runTestBitwise();
	
	printf("Logical Shift Operations\n");
	//runTestShiftLogical();
	
	printf("Arithmetic Shift Operations\n");
	runTestShiftArithmetic();
	
	return 0;
}
