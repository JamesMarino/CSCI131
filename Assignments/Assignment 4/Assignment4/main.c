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

void storeImage(gdImagePtr image, const char *fileExtension, const char *group);
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
	
	/*
	 * Test
	 */
	
	// gdImagePtr background = loadImage("background.jpg", "jpg");
	/*
	gdImagePtr trigger = loadImage("trigger1.png", "png");
	
	printf("%d\n", background->sy);
	
	// gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h)
	gdImageCopy(background, trigger, 40, 500, 0, 0, trigger->sx, trigger->sy);
	
	FILE *output = fopen("final.png", "w");
	gdImagePng(background, output);
	 */
	
	// unsigned char *test = convert(background);
	
	/*
	// Menu Loop
	menuSelect();
	
	// Close Connection
	closeConnection();
	 */
	
	/*
	// Create Dir
	createDirectory("test");
	
	setupConnection();
	setImage(test);
	getImage();
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

/*
void storeImage(const char *group, unsigned char *base64Image)
{
	redisReply *reply;
	reply = redisCommand(Context, "SADD %s %s", group, base64Image);
	freeReplyObject(reply);
}
 */
void storeImage(gdImagePtr image, const char *fileExtension, const char *group)
{
	char *directoryFinal;
	char *index;
	
	strcpy(directoryFinal, "");
	strcat(directoryFinal, group);
	strcat(directoryFinal, "/");
	strcat(directoryFinal, BackgroundDIR);
	
	// Regardless store and increment the file count
	redisReply *setReply = redisCommand(Context, "INCR %s", BackgroundDIR);
	freeReplyObject(setReply);
	
	// Get current image count in database
	redisReply *getReply = redisCommand(Context, "GET %s", BackgroundDIR);
	
	if (getReply->str == NULL) {
		// Start at beginging
		strcpy(index, "1");
	} else {
		// Get Value
		strcpy(index, getReply->str);
	}
	
	freeReplyObject(getReply);
	
	strcat(directoryFinal, index);
	strcat(directoryFinal, fileExtension);
	
	FILE *output = fopen(directoryFinal, "w");
	gdImagePng(image, output);
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
		printf("Cannot handle image type %s\n", fileType);
	}
	
	fclose(file);
	
	return image;
}

void addBackground()
{
	/*static*/ char absoluteAddress[128];
	memset(absoluteAddress, 0, sizeof(absoluteAddress));
	
	/*static*/ char *extension;
	
	// Clear array, avaoid issues
	/*
	int i;
	for (i = 0; i < 128; i++)
		absoluteAddress[i] = '\0';
	
	for (i = 0; i < 16; i++)
		extension[i] = '\0';
	*/
	
	printf("Enter full pathname of background image:\n");
	printf("Path: ");
	fflush(stdin);
	scanf("%128s", absoluteAddress);
	fflush(stdout);
	printf("'%s'", absoluteAddress);
	
	//char *absoluteAddressCopy;
	//strcpy(absoluteAddressCopy, absoluteAddress);
	
	// Get the extension
	// extension = strrchr(absoluteAddressCopy, '.');
	strcpy(extension, ".png");
	
	gdImagePtr background = loadImage(absoluteAddress, extension);
	
	if (background != NULL) {
		
		// Store data in directory
		storeImage(background, extension, BackgroundDIR);
		
		// Successful
		printf("Added to collection of background images\n");
		
	} else {
		printf("Cannot Find Image.\n");
	}
	
	free(background);
}

void addTarget()
{
	
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
		scanf("%s", command);
		
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
