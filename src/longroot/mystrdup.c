/* mystrdup.c: implement a strdup for use on platforms that lack it. */

#include<stdlib.h>
#include<string.h>

char *mystrdup(const char *s)
{
	char *r;
	int n;

	if(!s) return NULL;
	n = strlen(s);
	r = (char *)malloc((n+1)*sizeof(char));
	if(!r)return NULL;
	strcpy(r,s);
	return r;
}

