#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>
#include <gd.h>
#include "base64.h"

/*
 * Data
 */
static char* HostName = "127.0.0.1";
static redisContext *Context;
static int Port = 6379;

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
unsigned char* convert(gdImagePtr im);

int main(int argc, const char * argv[])
{
	// Setup Connection to DB
	//setupConnection();
	
	/*
	 * Test
	 */
	gdImagePtr background = loadImage("background.jpg", "jpg");
	gdImagePtr trigger = loadImage("trigger1.png", "png");
	
	printf("%d\n", background->sy);
	
	// gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h)
	gdImageCopy(background, trigger, 40, 500, 0, 0, trigger->sx, trigger->sy);
	
	FILE *output = fopen("final.png", "w");
	gdImagePng(background, output);
	
	//unsigned char *test = convert(image);
	
	/*
	// Menu Loop
	menuSelect();
	
	// Close Connection
	closeConnection();
	 */
	
    return 0;
}

/*
 * Utilities
 */
unsigned char* convert(gdImagePtr im) {
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
	FILE *file;
	file = fopen(fileName, "r");
	
	// Check for error
	if (file == NULL) {
		perror("Error: File failed to open");
		exit(1);
	}
	
	gdImagePtr image = NULL;
	
	// Process appropriate image
	if (strcmp(fileType, "gif") == 0) {
		image = gdImageCreateFromGif(file);
	} else if (0 == strcmp(fileType, "png")) {
		image = gdImageCreateFromPng(file);
	} else if ((strcmp(fileType, "jpg") == 0) ||
			   (strcmp(fileType, "jpeg") == 0)) {
		image = gdImageCreateFromJpeg(file);
	} else {
		printf("Cannot handle image type .%s\n", fileType);
	}
	
	fclose(file);
	
	return image;
}

void addBackground()
{
	
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
		}
		
		exit(1);
	}
	
	// All is fine
	printf("Connected to Redis Server\n");
}

void menuSelect()
{
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
		} else if (strcmp(command, "background") == 0) {
			addBackground();
		} else if (strcmp(command, "target") == 0) {
			addTarget();
		} else if (strcmp(command, "generate") == 0) {
			generatePuzzle();
		}
		
	}
}
