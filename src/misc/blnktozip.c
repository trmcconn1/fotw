/* blnktozip.c   on a line of input replace \t[..n tabs ]\t with
		 \t[0.0...n+1times ...0.0]\t 
*/
/* useful in classrecord files to convert no recorded scores to zeros
*/
/* official bug: it doesn't work right if the first character
   of a line is a tab. */

#include <stdio.h>
#include <stdlib.h>
#define OUTSTR "0.0"   /* what to replace internal tabs with */

int passtab(void);

int main(void)
{
	char nxtchr;

	while((nxtchr = getchar()) != EOF){
	if(nxtchr != '\n'){ /* if it's not a blank line */
	putchar(nxtchr);	
	while((nxtchr = getchar()) != '\n') {
		if(nxtchr == '\t') {  /* drop into infield mode */
			putchar('\t');
				if(passtab()){
					 nxtchr = '\n';
					 break; /* \n found */
				}
			}
		else putchar(nxtchr); /* otherwise pass it on */
	}
	putchar(nxtchr); /* pass on closing newline */
	}
		else putchar(nxtchr); /* echo blank line */
		}
}

/* passtab: echo tabs as OUTSTR\t until first non-tab.
		Then echo nontab char and return.  */
/*		unless it's a newline */

/* returns: 1 if newline detected. 0 otherwise. */

int passtab(void)
{
	char nxtchr;
	int exception = 0; /* to handle initial newline */
	
	while((nxtchr = getchar()) == '\t'){ 
		exception = 1;	
		printf("%s\t",OUTSTR);
	}
	if(nxtchr == '\n'){
		if(exception == 0){
			printf("%s",OUTSTR);
		}
		return 1;         /* newline found */
	}
	putchar(nxtchr);
	return 0;
}	
	

	
