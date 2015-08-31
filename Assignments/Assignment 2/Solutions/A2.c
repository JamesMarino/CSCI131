#include <stdio.h>
#include <limits.h>

const int DATA_SIZE = 250;

const int DATA[] = {
	1, 4, 8, 20, 4, 39, 21, 23, 16, 5,
	3, 7, 63, 1, 42, 4, 35, 3, 29, 17,
	8, 24, 54, 94, 28, 52, 63, 28, 59, 47,
	43, 78, 34, 35, 44, 39, 32, 63, 67, 63,
	59, 45, 83, 68, 39, 68, 27, 60, 59, 49,
	73, 61, 54, 52, 50, 74, 58, 43, 64, 64,
	69, 54, 42, 52, 50, 50, 61, 81, 76, 41,
	30, 53, 68, 66, 57, 59, 48, 71, 33, 75,
	35, 53, 48, 60, 65, 52, 60, 37, 44, 48,
	54, 76, 71, 27, 50, 64, 98, 65, 55, 60,
	75, 60, 55, 49, 53, 52, 52, 35, 53, 24,
	27, 79, 69, 44, 42, 51, 46, 66, 50, 52,
	71, 59, 57, 49, 59, 56, 51, 71, 84, 43,
	72, 46, 76, 67, 73, 52, 65, 59, 27, 58,
	37, 47, 65, 71, 96, 45, 53, 55, 59, 74,
	34, 72, 46, 41, 59, 80, 36, 61, 44, 42,
	53, 40, 67, 48, 54, 46, 71, 75, 46, 60,
	43, 58, 63, 54, 62, 89, 54, 75, 66, 72,
	66, 66, 74, 34, 50, 61, 60, 56, 75, 88,
	61, 96, 44, 71, 62, 79, 60, 65, 57, 60,
	54, 56, 65, 52, 53, 81, 52, 59, 67, 39,
	53, 70, 67, 30, 81, 85, 66, 41, 56, 68,
	90, 56, 72, 63, 58, 69, 63, 54, 60, 66,
	69, 50, 35, 63, 38, 28, 74, 87, 66, 73,
	51, 62, 51, 64, 61, 63, 86, 62, 71, 77,
};

void mean()
{
	int total = 0, i = 0, mean = 0;
	
	for (i = 0; i < DATA_SIZE; i++) {
		total += DATA[i];
	}
	
	mean = total / DATA_SIZE;
	
	printf("Mean : %d\n", mean);
}

void mode()
{
	int i;
	int dmode[101];
	
	for (i = 0; i <= 100; i++) {
		dmode[i] = 0;
	}
	
	for (i = 0; i < DATA_SIZE; i++) {
		int value = DATA[i];
		dmode[value]++;
	}
	
	int maxndx = 0, maxval= 0;

	for (i = 0; i <= 100; i++) {
		if (dmode[i] > maxval) {
			maxval = dmode[i];
			maxndx = i;
		}
	}
	
	printf("Mode : %d\n", maxndx);
}

void maxAndMin()
{
	int max = INT_MIN;
	int min = INT_MAX;
	int i = 0, j = 0, current = 0;
	
	for (i = 0; i < DATA_SIZE; i++) {
		current = DATA[i];
		
		for (j = 0; j < DATA_SIZE; j++) {
			
			// Test Max
			if (current > max) {
				max = current;
			}
			
			// Test Min
			if (current < min) {
				min = current;
			}
			
		}
	}
	
	printf("Max : %d\n", max);
	printf("Min : %d\n", min);
}

int main()
{
	mean();
	mode();
	maxAndMin();
	
    return 0;
}
