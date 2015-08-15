int main()
{
	int r0;
	
	char msga[] = "A string whose length is to be determined";
	char msgb[100];
	
	char *r1 = msga;
	char *r2 = msgb;
	
	r0 = 0;
	
	while (*r1) {
		*r2 = *r1;
		r1++;
		r2++;
		r0++;
	}
	
	*r2 = 0;
	
    return 0;
}
