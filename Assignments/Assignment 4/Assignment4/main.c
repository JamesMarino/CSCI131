#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>
#include <gd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include "base64.h"

/*
 * Data
 */
static const char *HostName = "127.0.0.1";
static redisContext *Context;
static const int Port = 6379;
static const char *BackgroundDIR = "background";
static const char *TargetDIR = "target";

// Images
static const int MIN_IMAGE_TYPES = 5;
static const int MIN_EACH_TYPE = 3;

static const double BACKGROUND_WIDTH = 700.0;
static const double TARGET_WIDTH = 100.0;

// Redis groups
static const char *GRP_BACKGROUND_NAME = "bkgrdimgs";
static const char *GRP_TARGET_NAME = "targetname";
static const char *GRP_TARGET_COUNTER = "targetcnt";

/*
 * Definitions
 */
void menuSelect();
void setupConnection();
void exit();
void closeConnection();
void addBackground();
void addTarget();
void generatePuzzle();
gdImagePtr loadImage(char* fileName, char* fileType);
unsigned char* convertToBase64(gdImagePtr im);
void storeImageBackground(gdImagePtr image, const char *fileExtension,
				const char *dir, const char *fileName, const char *groupName);
void setImage(unsigned char *image);
char* getImage(char *group);
void createDirectory(const char *directory);




int main(int argc, const char * argv[])
{
	/*
	 * Actual app
	 */
	// Setup Connection to DB
	setupConnection();
	
	// Create dirs
	createDirectory(BackgroundDIR);
	createDirectory(TargetDIR);
	
	// Run Menu
	menuSelect();
	
	// Close Connection
	closeConnection();
	/*
	 * END Actual app
	 */
	
    return 0;
}

/*
 * Utilities
 */
int generateRandomNumber(int minNum, int maxNum)
{
	int result = 0, low = 0, high = 0;
	
	if (minNum < maxNum) {
		low = minNum;
		high = maxNum + 1;
	} else {
		low = maxNum + 1;
		high = minNum;
	}
	
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand((int)t1.tv_usec * (int)t1.tv_sec);
	
	// srand((unsigned int)time(NULL));
	result = (rand()%(high-low)) + low;
	
	return result;
}

void generateRandomTargetGroups(int minNum, int maxNum, int *randomArray)
{
	int result = 0, low = 0, high = 0, foundSpots = 1;
	int isDuplicate = 0, i = 0, j = 0;
	
	while (foundSpots == 1) {
	
		// Reset
		foundSpots = 0;
		isDuplicate = 0;
		
		if (minNum < maxNum) {
			low = minNum;
			high = maxNum + 1;
		} else {
			low = maxNum + 1;
			high = minNum;
		}
		
		srand((unsigned int)time(NULL));
		result = (rand()%(high-low)) + low;
		
		for (i = 0; i < maxNum; i++) {
			if (randomArray[i] == 0) {
				foundSpots = 1;
				break;
			}
		}
		
		for (j = 0; j < maxNum; j++) {
			// Check if random value already in list
			if (randomArray[j] == result) {
				isDuplicate = 1;
				break;
			}
		}
		
		if (foundSpots && !isDuplicate) {
			randomArray[i] = result;
		}
	}
}

char* getFileName(char *absoluteAddress)
{
	return strrchr(absoluteAddress, '/');
}

void createDirectory(const char *directory)
{
	struct stat current = {0};
	
	if (stat(directory, &current) == -1) {
		mkdir(directory, 0700);
	}
}

gdImagePtr blendTransparency(gdImagePtr image, int height)
{
	gdImageAlphaBlending(image, 0);
	
	int w = TARGET_WIDTH;
	int h = height;
	int h1 = h / 6;
	int h2 = 5 * h1;
	int l1 = w / 6;
	int l2 = 5 * 11;
	int row, col;
	
	for (row = 0; row < h; row++) {
		for (col = 0; col < w; col++) {
			int aval = 40;
			
			if ((row < h1) || (row > h2))
				aval = 80;
			else
				if ((col < l1) || (col > l2))
					aval = 80;
			
			int c = gdImageGetPixel(image, col, row);
			int r = gdImageRed(image, c);
			int g = gdImageGreen(image, c);
			int b = gdImageBlue(image, c);
			int c1 = gdImageColorAllocateAlpha(image, r, g, b, aval);
			
			gdImageSetPixel(image, col, row, c1);
		}
	}
	
	gdImageSaveAlpha(image, 1);
	
	return image;
}

void storeImageBackground(gdImagePtr image, const char *fileExtension,
				const char *dir, const char *fileName, const char *groupName)
{
	char directoryFinal[64];
	char redisIndex[4];
	long long index;
	
	strcpy(directoryFinal, "");
	strcat(directoryFinal, dir);
	strcat(directoryFinal, "/");
	strcat(directoryFinal, dir);
	
	// Get current image count in database
	redisReply *getReply = redisCommand(Context, "SCARD %s", groupName);
	
	index = getReply->integer;
	
	// Get the next
	index++;
	
	snprintf(redisIndex, 4, "%lld", index);
	
	freeReplyObject(getReply);
	
	// Regardless store the filename
	redisReply *setReply = redisCommand(Context, "SADD %s %s", groupName, fileName);
	freeReplyObject(setReply);
	
	strcat(directoryFinal, redisIndex);
	strcat(directoryFinal, fileExtension);
	
	// Scale the image
	double imageHeight = BACKGROUND_WIDTH*((double)image->sx/(double)image->sy);
	int imageHeightRounded = (int)imageHeight;
	image = gdImageScale(image, BACKGROUND_WIDTH, imageHeightRounded);
	
	FILE *output = fopen(directoryFinal, "w");
	gdImageJpeg(image, output, 100);
	fclose(output);
}

void storeImageTarget(gdImagePtr image, const char *fileExtension,
				const char *dir, const char *fileName, const char *groupName)
{
	char directoryFinal[64];
	char fileNameFinal[64];
	char redisIndex[4];
	long long index;
	
	strcpy(directoryFinal, "");
	strcat(directoryFinal, dir);
	strcat(directoryFinal, "/");
	strcat(directoryFinal, dir);
	
	// Get current image count in database
	redisReply *setIncrement = redisCommand(Context, "INCR %s", GRP_TARGET_COUNTER);
	
	index = setIncrement->integer;
	
	snprintf(redisIndex, 4, "%lld", index);
	
	freeReplyObject(setIncrement);
	
	strcpy(fileNameFinal, "");
	strcat(fileNameFinal, dir);
	strcat(fileNameFinal, redisIndex);
	
	// Regardless store the filename
	redisReply *setReply = redisCommand(Context, "SADD %s %s", groupName, fileNameFinal);
	freeReplyObject(setReply);
	
	strcat(directoryFinal, redisIndex);
	strcat(directoryFinal, fileExtension);
	
	// Scale the image
	double imageHeight = TARGET_WIDTH*((double)image->sx/(double)image->sy);
	int imageHeightRounded = (int)imageHeight;
	image = gdImageScale(image, TARGET_WIDTH, imageHeightRounded);
	
	// Add transparency
	image = blendTransparency(image, image->sy);
	
	FILE *output = fopen(directoryFinal, "w");
	gdImagePng(image, output);
	fclose(output);
}

unsigned char* convertToBase64(gdImagePtr im) {
	// Convert the image to 1peg, default quality, getting
	// back pointer to allocated bytes and length
	int size;
	void* jpegbytes = gdImageJpegPtr(im, &size, 75);
	size_t encodedsize = 0;
	
	unsigned char* source = (unsigned char*) jpegbytes;
	unsigned char* b64codes = NULL;
	
	// Invoke base64 encoder first time just to get length of
	// base 64 string
	base64_encode(b64codes, &encodedsize, source, size);
	
	// Allocate space
	b64codes = malloc(encodedsize + 1);
	
	// Convert
	int res = base64_encode(b64codes, &encodedsize, source, size);
	gdFree(jpegbytes);
	
	if (res != 0) {
		printf("Failed to base 64 encode data\n");
		if (b64codes != NULL)
			free(b64codes);
		return NULL;
	}
	
	return b64codes;
}

gdImagePtr loadImage(char* fileName, char* fileType)
{
	// Read in image
	FILE *file = NULL;
	file = fopen(fileName, "r");
	
	// Check for error
	if (file == NULL) {
		printf("\n");
		perror("fopen");
		return NULL;
	}
	
	gdImagePtr image = NULL;
	
	// Process appropriate image
	if (strcmp(fileType, ".gif") == 0) {
		image = gdImageCreateFromGif(file);
	} else if (0 == strcmp(fileType, ".png")) {
		image = gdImageCreateFromPng(file);
	} else if ((strcmp(fileType, ".jpg") == 0) ||
			   (strcmp(fileType, ".jpeg") == 0)) {
		image = gdImageCreateFromJpeg(file);
	} else {
		printf("\nCannot handle image type %s\n", fileType);
	}
	
	fclose(file);
	
	return image;
}

void addBackground()
{
	// Setup
	static char absoluteAddress[64];
	static char *extension;
	static char *fileName;
	
	// Clear array, avaoid issues
	int i;
	for (i = 0; i < 64; i++)
		absoluteAddress[i] = '\0';
	
	printf("Enter full pathname of background image:\n");
	printf("Path: ");
	
	// Get Filename
	fflush(stdin);
	fgets(absoluteAddress, sizeof(absoluteAddress), stdin);
	absoluteAddress[strcspn(absoluteAddress, "\n")] = '\0';
	
	fflush(stdin);
	fflush(stdout);
	printf("Reading File: '%s' ", absoluteAddress);
	
	// Copy the string
	char absoluteAddressFileName[64];
	char absoluteAddressExtension[64];
	strcpy(absoluteAddressFileName, absoluteAddress);
	strcpy(absoluteAddressExtension, absoluteAddress);
	
	// Get the extension
	extension = strrchr(absoluteAddressExtension, '.');
	
	// Get the filename
	fileName = strrchr(absoluteAddressFileName, '/');
	fileName[strlen(fileName)-strlen(extension)] = '\0';
	for (i = 0; i < strlen(fileName); i++)
		fileName[i] = fileName[i+1];
	
	gdImagePtr background = loadImage(absoluteAddress, extension);
	
	if (background != NULL) {
		
		// Store data in directory
		storeImageBackground(background, ".jpg", BackgroundDIR, fileName, GRP_BACKGROUND_NAME);
		
		// Successful
		printf("\nAdded to collection of background images\n");
		
	} else {
		printf("Cannot Find Image.\n");
	}
	
	free(background);
}

void addTarget()
{
	// Setup
	static char absoluteAddress[64];
	static char targetTag[64];
	static char *extension;
	static char *fileName;
	
	// Clear array, avaoid issues
	int i;
	for (i = 0; i < 64; i++)
		absoluteAddress[i] = '\0';
	
	printf("Enter full pathname of background image:\n");
	printf("Path: ");
	
	// Get Filename
	fflush(stdin);
	fgets(absoluteAddress, sizeof(absoluteAddress), stdin);
	absoluteAddress[strcspn(absoluteAddress, "\n")] = '\0';
	
	fflush(stdin);
	fflush(stdout);
	printf("Reading File: '%s' ", absoluteAddress);
	
	// Copy the string
	char absoluteAddressFileName[64];
	char absoluteAddressExtension[64];
	strcpy(absoluteAddressFileName, absoluteAddress);
	strcpy(absoluteAddressExtension, absoluteAddress);
	
	// Get the extension
	extension = strrchr(absoluteAddressExtension, '.');
	
	// Get the filename
	fileName = strrchr(absoluteAddressFileName, '/');
	fileName[strlen(fileName)-strlen(extension)] = '\0';
	for (i = 0; i < strlen(fileName); i++)
		fileName[i] = fileName[i+1];
	
	gdImagePtr target = loadImage(absoluteAddress, extension);
	
	if (target != NULL) {
		
		// Get Tag
		printf("\nEnter a tag for this target: ");
		fflush(stdin);
		fgets(targetTag, sizeof(targetTag), stdin);
		targetTag[strcspn(targetTag, "\n")] = '\0';
		
		// Store data in directory
		storeImageTarget(target, ".png", TargetDIR, fileName, targetTag);
		
		// Store Tag
		redisReply *setReply = redisCommand(Context, "SADD %s %s", GRP_TARGET_NAME, targetTag);
		freeReplyObject(setReply);
		
		// Successful
		printf("Added to collection of background images\n");
		
	} else {
		printf("Cannot Find Image.\n");
	}
	
	free(target);
	
}

void generatePuzzle()
{
	size_t keyCount = 0;
	size_t targetCount = 0;
	int i = 0;
	
	// Get current image count in database
	redisReply *getReply = redisCommand(Context, "SCARD %s", GRP_TARGET_NAME);
	keyCount = getReply->integer;
	freeReplyObject(getReply);
	
	// Check if there is right amount of images
	// DELETE
	//keyCount = 5;
	// DELETE
	if (keyCount >= MIN_IMAGE_TYPES) {
		
		/*
		 * Get background
		 */
		// Get background size
		redisReply *backgroundSize = redisCommand(Context, "SCARD %s", GRP_BACKGROUND_NAME);
		
		char backgroundNumber[4];
		char backgroundFileName[64];
		int randomBackgroundNumber = 1;
		
		targetCount = backgroundSize->integer;
		
		if (randomBackgroundNumber != (int)targetCount) {
			randomBackgroundNumber = generateRandomNumber(1, (int)backgroundSize->integer);
		}
		
		snprintf(backgroundNumber, 4, "%d", randomBackgroundNumber);
		strcpy(backgroundFileName, "");
		strcat(backgroundFileName, BackgroundDIR);
		strcat(backgroundFileName, "/");
		strcat(backgroundFileName, BackgroundDIR);
		strcat(backgroundFileName, backgroundNumber);
		strcat(backgroundFileName, ".jpg");
		
		freeReplyObject(backgroundSize);
		
		// Get a background image
		gdImagePtr background = loadImage(backgroundFileName, ".jpg");
		
		if (background == NULL) {
			// Exit out
			printf("Error opening background image\n");
			return;
		}
		
		int randomArray[MIN_IMAGE_TYPES] = {0, 0, 0, 0, 0};
		generateRandomTargetGroups(1, MIN_IMAGE_TYPES, randomArray);
		
		redisReply *targetList = redisCommand(Context, "SMEMBERS %s", GRP_TARGET_NAME);
		
		for (i = 0; i < MIN_IMAGE_TYPES; i++) {
			
			// Check if there is min amount of images
			redisReply *minImages = redisCommand(Context, "SCARD %s", targetList->element[i]->str);
			
			if (minImages->integer >= MIN_EACH_TYPE) {
				
				redisReply *currentTarget = redisCommand(Context, "SMEMBERS %s", targetList->element[i]->str);
				
				int randomTargetArray[MIN_EACH_TYPE] = {0, 0, 0};
				generateRandomTargetGroups(1, MIN_EACH_TYPE, randomTargetArray);
				
				int j = 0;
				
				for (j = 0; j < MIN_EACH_TYPE; j++) {
					
					/*
					 * Get Each Target
					 */
					char targetFileName[64];
					
					strcpy(targetFileName, "");
					strcat(targetFileName, TargetDIR);
					strcat(targetFileName, "/");
					strcat(targetFileName, currentTarget->element[j]->str);
					strcat(targetFileName, ".png");
					
					// Get a background image
					gdImagePtr targetImage = loadImage(targetFileName, ".png");
					
					gdImageCopy(background, targetImage,
								generateRandomNumber(100, (background->sx - 100)),
								generateRandomNumber(100, (background->sy - 100)),
								0, 0, 100, targetImage->sy);
					
					gdImageDestroy(targetImage);
				}
				
				// Free
				freeReplyObject(currentTarget);
				
				
			} else {
				printf("Add %lld more target groups to %s\n", (MIN_EACH_TYPE - minImages->integer),
					   targetList->element[0]->str);
				break;
			}
			
			FILE *output = fopen("/Users/james/Desktop/tomOut.png", "w");
			gdImagePng(background, output);
			fclose(output);
			
			freeReplyObject(minImages);
			
		}
		
		gdImageDestroy(background);
		freeReplyObject(targetList);
		
	} else {
		printf("Add %lu more target groups\n", (MIN_IMAGE_TYPES - keyCount));
	}
	
}

void closeConnection()
{
	redisFree(Context);
	printf("Disconnected from Redis Server\n");
}

void setupConnection() {
	struct timeval timeout = {1, 500000};
	
	Context = redisConnectWithTimeout(HostName, Port, timeout);
	
	if ((Context == NULL) || (Context->err)) {
		if (Context) {
			printf("Connection error: %s\n",Context->errstr);
			redisFree(Context);
		} else {
			printf("Connection error: can't allocate redis context\n");
			fflush(stdout);
		}
		
		exit(1);
	}
	
	// All is fine
	printf("Connected to Redis Server\n");
}

void menuSelect()
{
	setvbuf(stdout, NULL, _IONBF, 0);
	
	while (1) {
		printf("> ");
		
		char command[32];
		fgets(command, 32*sizeof(char), stdin);
		command[strcspn(command, "\n")] = 0;
		
		if (command[0] == 'q') {
			printf("Goodbye.\n");
			break;
		} else if (command[0] == '?') {
			printf("Commands:\n");
			printf("\t?: Print this command list\n");
			printf("\tq: Quit\n");
			printf("\tBackground: Add another background picture\n");
			printf("\tTarget: Add another target image\n");
			printf("\tGenerate: Create a puzzle\n");
		} else if (strcmp(command, "Background") == 0) {
			addBackground();
		} else if (strcmp(command, "Target") == 0) {
			addTarget();
		} else if (strcmp(command, "Generate") == 0) {
			generatePuzzle();
		}
		
	}
}
