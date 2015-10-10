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
static const char *TargerDIR = "target";

// Images
static const int IMAGE_TYPES = 5;
static const int MIN_AMOUNT_EACH_TYPE = 3;
static const double BACKGROUND_WIDTH = 700.0;

// Redis groups
static const char *GRP_BACKGROUND_NAME = "bkgrdimgs";

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

void storeImage(gdImagePtr image, const char *fileExtension, const char *group, const char *fileName);
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
	createDirectory(TargerDIR);
	
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

void storeImage(gdImagePtr image, const char *fileExtension, const char *group, const char *fileName)
{
	char directoryFinal[64];
	char redisIndex[4];
	int index;
	
	strcpy(directoryFinal, "");
	strcat(directoryFinal, group);
	strcat(directoryFinal, "/");
	strcat(directoryFinal, BackgroundDIR);
	
	// Get current image count in database
	redisReply *getReply = redisCommand(Context, "SCARD %s", GRP_BACKGROUND_NAME);
	
	if (getReply->str == NULL) {
		// Start at beginging
		strcpy(redisIndex, "0");
	} else {
		// Get Value
		strcpy(redisIndex, getReply->str);
	}
	
	// Get the next
	index = atoi(redisIndex);
	index++;
	
	snprintf(redisIndex, 4, "%d", index);
	
	freeReplyObject(getReply);
	
	// Regardless store the filename
	redisReply *setReply = redisCommand(Context, "SADD %s %s", GRP_BACKGROUND_NAME, fileName);
	freeReplyObject(setReply);
	
	strcat(directoryFinal, redisIndex);
	strcat(directoryFinal, fileExtension);
	
	// Scale the image
	double imageHeight = BACKGROUND_WIDTH*((double)image->sx/(double)image->sy);
	int imageHeightRounded = (int)imageHeight;
	image = gdImageScale(image, imageHeightRounded, BACKGROUND_WIDTH);
	
	FILE *output = fopen(directoryFinal, "w");
	gdImageJpeg(image, output, 10);
	fclose(output);
}

char* getImage(char *group)
{
	// Get size of the set
	redisReply *reply;
	reply = redisCommand(Context, "SCARD %s", group);
	
	int count = (int)reply->integer;
	freeReplyObject(reply);
	
	reply = redisCommand(Context, "SMEMBERS %s", group);
	
	int j;
	//for (j=0; j < reply->elements; j++) {
		//return reply->element[j]->str;
	//}
	
	freeReplyObject(reply);
	
	return reply->element[0]->str;
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
		storeImage(background, ".jpg", BackgroundDIR, fileName);
		
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
	static char *extension;
	
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
	char absoluteAddressCopy[64];
	strcpy(absoluteAddressCopy, absoluteAddress);
	
	// Get the extension
	extension = strrchr(absoluteAddressCopy, '.');
	
	gdImagePtr background = loadImage(absoluteAddress, extension);
	
	if (background != NULL) {
		
		// Store data in directory
		storeImage(background, extension, BackgroundDIR, "traget.png.exe.test");
		
		// Successful
		printf("\nAdded to collection of background images\n");
		
	} else {
		printf("Cannot Find Image.\n");
	}
	
	free(background);
}

void generatePuzzle()
{
	
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
		// scanf("%s *[^\n]", command);
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
