/* Read key characters from file into character array. Skip whitespace characters */
#define _CRT_SECURE_NO_WARNINGS
#include "debruijn.h"
#include <stdio.h>
#include <stdlib.h>

/* static arrray to hold the data */

char key[MAX_STRING];


int readfile( char *name ){

	int c,cnt = 0;
	FILE *src;

	if(name) {

		src = fopen(name,"r");
		if(!src){
			fprintf(stderr,"readfile: cannot open data file named %s\n",name);
			return cnt;
		}
	}
	else return 0;
	while((c = fgetc(src))!= EOF){
		if((c==' ')||(c=='\n')||(c=='\t'))continue;
		key[cnt++]=c;
		if(cnt >= MAX_STRING){
				fprintf(stderr,"readfile: trying to store past end of array\n");
				break;
			}
	}
	fclose(src);
	key[cnt]='\0';
	return cnt;
}