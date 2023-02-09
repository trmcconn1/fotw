/* perms2str.c: convert a 16-bit int to a perms string as used in
   ls -l 
*/

#define _POSIX_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>

#define STICKY 0001000

int 
perms2str(char *buf, int perms)
{
	int type;

	type = _S_IFMT & perms;
	*buf = '\0';

	switch(type){
		case _S_IFDIR:
			sprintf(buf,"%s","d");
			break;
		case _S_IFCHR:
			sprintf(buf,"%s","c");
			break;
		case _S_IFBLK:
			sprintf(buf,"%s","b");
		default:
			;
	}

	if(S_IRUSR & perms)strcat(buf,"r");
		else strcat(buf,"-");
	if(S_IWUSR & perms)strcat(buf,"w");
		else strcat(buf,"-");
	if(S_ISUID & perms)strcat(buf,"s");
		else
			if(S_IXUSR & perms)strcat(buf,"x");
				else strcat(buf,"-");
	if(S_IRGRP & perms)strcat(buf,"r");
		else strcat(buf,"-");
	if(S_IWGRP & perms)strcat(buf,"w");
		else strcat(buf,"-");
	if(S_ISGID & perms)strcat(buf,"s");
		else
			if(S_IXGRP & perms)strcat(buf,"x");
				else strcat(buf,"-");
	if(S_IROTH & perms)strcat(buf,"r");
		else strcat(buf,"-");
	if(S_IWOTH & perms)strcat(buf,"w");
		else strcat(buf,"-");
	if(STICKY & perms)strcat(buf,"t");
		else
			if(S_IXOTH & perms)strcat(buf,"x");
				else strcat(buf,"-");

	return 0;
}


int
main(int argc, char **argv)
{

	int perms;
	char buffer[15];

	perms = atoi(argv[1]);

	perms2str(buffer,perms);

	printf("%s\n",buffer);
	return 0;
}
	
