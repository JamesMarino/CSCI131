#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>
#include <gd.h>
#include <math.h>
#include <ctype.h>
#include "base64.h"
#include "ProjectGroup.h"

#define WIDTH 100

static char* database = "Members.dbm";
static GDBM_FILE databaseconnection;

static void listMembers();
static void addData();
static void getRoles(Member* mptr);
static gdImagePtr getScaledImage();
static gdImagePtr scaleImage(gdImagePtr im);
static void saveData(Member *mptr, gdImagePtr im);
static void generatePage();
static gdImagePtr loadImage(char* afilename, char* afiletype);
static void getCompetencies(Member* mptr);
static void printRolesAndCompetencies(FILE* output, Member* mptr);

static void menuselectloop() {
	printf("Processing options:\n");
	printf("0 => Quit; 1 => Add data; 2 => Generate member page; 3 => List members\n");
	for (;;) {
		printf("Opt>");
		int choice;
		int n = scanf("%d", &choice);
		
		if ((n != 1) || (choice == 0)) break;
		if (choice == 1) addData();
		else
			if (choice == 2) generatePage();
		else
			if (choice == 3) listMembers();
	}
}

static void initialiseDB()
{
	int initialblocksize = 0;
	
	int flags = GDBM_WRCREAT;
	
	int mode = 0600;
	
	databaseconnection = gdbm_open(database, initialblocksize, flags, mode, NULL);
}
static void closeDB()
{
	gdbm_close(databaseconnection);
}

static void listMembers()
{
	// Have keys for member records - Unix id5, e.g. tA9032
	// and keys for image records, e.g. eL9032_image
	// Just list the member keys
	datum akey;
	akey = gdbm_firstkey(databaseconnection);
	
	while(akey.dptr != NULL) {
		char id[128];
		// Caution Aty.piat not terminated!
		memset(id,0,128);
		strncpy(id, akey.dptr, akey.dsize);
		char* imagesuffix = strstr(id,"_image");
		
		if(imagesuffix==NULL)
			printf("%s\n", id);
		datum nkey=gdbm_nextkey(databaseconnection,akey);
		free(akey.dptr);
		akey=nkey;
	}
}

static void addData() {
	Member amember;
	gdImagePtr im;
	memset(&amember, 0, sizeof (Member));
	printf("Enter member details:\n");
	// Careless - not checking for name length limits
	printf("Family name : ");
	scanf("%s", amember.familyname);
	printf("Given name : ");
	scanf("%s", amember.givenname);
	printf("Initials : ");
	scanf("%s", amember.initials);
	printf ("Mobile # : ");
	scanf("%s", amember.mobile);
	printf("Other phone # : ");
	scanf("%s", amember.other);
	printf ("Unix id : ");
	scanf("%s", amember.uname);
	
	getRoles(&amember);
	getCompetencies(&amember);
	im = getScaledImage();
	saveData(&amember, im);
	
	if (im!=NULL)
		gdFree(im);
}

static char* rolenames[] = {
	"Manager ", "Sys_admin", "Dba", "Webmaster",
	"Librarian", "Leadprogrammer", "Designer", "UMLArtist",
	"Programmer", "Documenter", "Toolsmith", "Tester",
	"Versionmanager ", "Pizza_boy"
};

static int nroles = sizeof (rolenames) / sizeof (char*);

static char* competencies[] = {
	"Originator", "Coordinator", "Driver", "Monitor", "Implementer",
	"Supporter", "Investigator", "Finisher", "Chief", "Procrastinator"
};

static int ncompetencies = sizeof (competencies) / sizeof (char*);

static int findRole(char* data) {
	int i;
	
	for (i = 0; i < nroles; i++) {
		if (0 == strcmp(data, rolenames[i])) return i;
	} return -1;
}

static void getRoles(Member* mptr) {
	printf("Enter roles");
	
	for (;;) {
		char instr[128];
		printf(">");
		scanf("%s", instr);
		if (0 == strlen(instr)) continue;
		
		if (instr[0] == '-') break;
		if (instr[0] == '?') {
			int j;
			printf("Choices are:");
			for (j = 0; j < nroles; j++)
				printf("\t%s\n", rolenames[j]);
			continue;
		}
		
		int ndx = findRole(instr);
		if (ndx < 0) {
			printf("Unrecognized input %s\n", instr);
			printf("Enter role name, or ? to list names, or - to terminate\n");
			continue;
		}
		
		int bit = 1 << ndx;
		mptr->roles |= bit;
	}
}

static gdImagePtr getScaledImage() {
	char input[128];
	
	printf("Is an image available (yen) : ");
	scanf("%s", input);
	
	if (input[0] != 'y')
		return NULL;
	
	char afiletype[6];
	char afilename[128];
	
	printf("Name of image file : ");
	scanf("%s", afilename);
	char* typesuffix = strrchr(afilename, '.');
	
	if (typesuffix == NULL) {
		printf("No tatty= suffix?\n");
		printf("Ignoring request of image\n");
		return NULL;
	}
	
	strcpy(afiletype, typesuffix + 1);
	char *p;
	
	for (p = afiletype; *p != '\0'; p++)
		*p = (char) tolower(*p);
	
	gdImagePtr im = loadImage(afilename, afiletype);
	gdImagePtr ims = scaleImage(im);
	gdImageDestroy(im);
	return ims;
}

static gdImagePtr scaleImage(gdImagePtr im)
{
	int w = gdImageSX(im);
	int h = gdImageSY(im);
	
	int nh = (int) ceil(WIDTH * h / w);
	
	gdImagePtr ims = gdImageScale(im, WIDTH, nh);
	
	return ims;
}

static char* convert(gdImagePtr im) {
	// Convert the image to 1peg, default quality, getting
	// back pointer to allocated bytes and length
	int size;
	void* jpegbytes = gdImageJpegPtr(im, &size, 75);
	size_t encodedsize = 0;
	
	unsigned char* source = (unsigned char*) jpegbytes;
	char* b64codes = NULL;
	
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

static void saveData(Member *mptr, gdImagePtr im) {
	datum key;
	datum value;
	
	char* keyv = strdup(mptr->uname);
	int keylen = strlen(keyv);
	
	key.dptr = keyv;
	key.dsize = keylen;
	
	value.dptr = (char*) mptr;
	value.dsize = sizeof(Member);
	
	// Allow overwriting of records
	// So if entry exists with key it will be replaced
	int res = gdbm_store(databaseconnection, key, value, GDBM_REPLACE );
	
	if (res!=0) {
		printf("Failed to store record for %s\n", keyv);
	} else {
	   printf("Stored record for %s\n", keyv);
	   
	   if (im != NULL) {
		  char* imageastr = convert(im);
		  char otherkey[128];
		  strcpy(otherkey, keyv);
		  strcat(otherkey,"_image");
		  datum imagekey;
		  datum imagedata;
		  
		  imagekey.dptr = otherkey;
		  imagekey.dsize = strlen(otherkey);
		  
		  imagedata.dptr = imageastr;
		  imagedata.dsize = strlen(imageastr);
		  
		  int ires = gdbm_store(databaseconnection, imagekey, imagedata, GDBM_REPLACE);
		  
		  if (ires!=0)
			 printf("\t(but failed to store image)\n");
		  
		  free(imageastr);
	   }
	}
	
	free(keyv);
}

static void generatePage() {
   printf("Enter id : ");
   
   char id[128];
   scanf("%s", id);
   
   datum query;
   query.dptr = id;
   query.dsize = strlen(id);
   
   datum data = gdbm_fetch(databaseconnection, query);
   if (data.dptr == NULL) {
	  printf("There is no record for key %s\n", id);
	  return;
   }
   
   Member* amember = (Member*) data.dptr;
   
   // Try for image
   strcat(id, "_image");
   query.dptr = id;
   query.dsize = strlen(id);
   datum imagedata = gdbm_fetch(databaseconnection, query);
   
   char outputfilename[128];
   strcpy(outputfilename, amember->uname);
   strcat(outputfilename, ".html");
   
   FILE* output = fopen(outputfilename, "w");
   
   if (output == NULL) {
	  printf("Unable to create an output file\n");
	  // Need to free space anyway
	  free(data.dptr);
	  
	  if (imagedata.dptr != NULL)
		 free(imagedata.dptr);
   }
   
   
   fprintf(output, "<htm1><head><title>Member %s</title></head>\n", amember->uname);
   fprintf(output, "<body><hl>%s</h1>\n", amember->uname);
   fprintf(output, "<h2>%s %s %s</h2>\n", amember->givenname, amember->initials, amember->familyname);
   fprintf(output, "<h2>Contact numbers</h2>\n<h3>Mobile %s</h3>\n<h3>Other %s</h3>\n", amember->mobile, amember->other);
   printRolesAndCompetencies(output, amember);
   
   if (imagedata.dptr != NULL) {
	  fprintf(output, "<img src='");
	  fprintf(output, "data:image/liag;base64,");
	  
	  fprintf(output, "%*.*s", imagedata.dsize, imagedata.dsize, imagedata.dptr);
	  fprintf(output, "' />\n");
   }
   
   fprintf(output, "</body></html>");

   fclose(output);
   free(data.dptr);
   
   if (imagedata.dptr != NULL)
	  free(imagedata.dptr);
}

static void printBits(FILE* output, int value, char** names) {
   int i = 0;
   int bit = 1;
   
   fprintf(output,"<ul>\n");
   
   for(i=0;i<32;i++) {
	  int tst = value & bit;
	  bit = bit << 1;
	  
	  if(tst) {
		 fprintf(output,"<li>%s</li>\n", names[i]);
	  }
   }
   
   fprintf(output,"</u1>\n");
}

static void printRolesAndCompetencies(FILE* output, Member* mptr) {
   int roles = mptr->roles;
   if(roles!=0) {
	  fprintf(output, "<h3>Roles</h3>\n");
	  printBits(output,roles,rolenames);
   }
   
   int social = mptr->social;
   if (social != 0) {
	  fprintf(output,"<h3>Social Combetencies</h3>\n");
	  printBits(output,social,competencies);
   }
}

static gdImagePtr loadImage(char* afilename, char* afiletype) {
   FILE *afile;
   afile = fopen(afilename, "r");
   
   if (afile == NULL) {
	  perror("File failed to open");
	  exit(1);
   }
   
   gdImagePtr im = NULL;
   
   if (0 == strcmp(afiletype, "gif")) {
	  im = gdImageCreateFromGif(afile);
   } else if (0 == strcmp(afiletype, "png")) {
	  im = gdImageCreateFromPng(afile);
   } else if ((0 == strcmp(afiletype, "jpg")) || (0 == strcmp(afiletype, "jpeg"))) {
	  im = gdImageCreateFromJpeg(afile);
   } else {
	  printf("Sorry - cannot handle image type %s\n", afiletype);
   }
   
   fclose(afile);
   
   return im;
}

static void getCompetencies(Member* mptr)
{
   
}

int main(int argc, char** argv) {
	initialiseDB();
	menuselectloop();
	closeDB();
	return (EXIT_SUCCESS);
}

