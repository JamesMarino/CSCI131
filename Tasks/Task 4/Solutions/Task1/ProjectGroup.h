#define NAMELENGTH 32
#define INITIALLENGTH 4
#define PHONENUMLEN 16
#define USERIDLEN 12

struct projectgroupmember {
	char familyname[NAMELENGTH];
	char givenname[NAMELENGTH];
	char initials[INITIALLENGTH];
	unsigned int roles;
	unsigned int social;
	char mobile[PHONENUMLEN];
	char other[PHONENUMLEN];
	char uname[USERIDLEN];
};

typedef struct projectgroupmember Member;
