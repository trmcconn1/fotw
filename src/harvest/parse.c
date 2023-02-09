/* parse.c return next token in an input stream */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"  /* declares parsing states */


/* Change as needed for a given application */
#define WORD_DELIMS " 	\r"
#define LINE_DELIMS "\n"
#define MY_BUFSIZ 2048

static char my_buffer[MY_BUFSIZ];

/* This is a very basic parser that returns a pointer to the next
field of an input stream. It returns special status EOL, EOF, ERROR
through the integer pointer.
Returned strings and status value are destroyed
on the next call, so use them or lose them */

char *parse(FILE *input, int *status)
{
	char nxtchr;
	int chrcnt = 0;
	
	if(input == NULL){
		*status = MY_ERROR;
		return NULL;
	}
	if(status == NULL){
		return NULL;
	}

	*status = MY_NORMAL;
	chrcnt = 0;
	while((nxtchr = fgetc(input)) != EOF){

		if(!strchr(WORD_DELIMS,nxtchr)){
			/* Is it end of line ?*/
			if(strchr(LINE_DELIMS,nxtchr)){
					*status = MY_EOL;
					 break;
			}
			if(chrcnt >= MY_BUFSIZ){
				*status = MY_ERROR;	
				return NULL;
			}
			my_buffer[chrcnt++] = nxtchr;
			continue;
		}
		else break;
	}
	if(nxtchr == EOF) *status = MY_EOF; 
	my_buffer[chrcnt] = '\0';
	return my_buffer;
}



