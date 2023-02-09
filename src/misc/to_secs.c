/* to_secs.c: source file for to_seconds routine  

int to_seconds(char *timestring, char *secstring)

	converts timestring of format hrs:mins:sec.s into sec.s and
	stores resulting string in secstring. If timestring format
	is invalid, copies timestring up to first trailing whitespace
	to secstring. Returns 1 if valid timestring, 0 if invalid. Initial
	whitespace is copied. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXFIELDS 3
#define BUFSIZE 10
#define TRUE 1
#define FALSE 0
#define FLDSEP ':' /* separator within time string */
#define FMT "%15.10g"

float secs( float times[], int nfields);
int
to_seconds(char *timestring,char *secstring);

/* end of header */

int
to_seconds(char *timestring,char *secstring) 
{
	char buffer[BUFSIZE];
	float times[MAXFIELDS]; /* store (hh,mm,sec.s) */
	static float zeros[MAXFIELDS];
	char temp[BUFSIZE];
	int timeok = TRUE;        /* flag for invalid time string */
	int flds  = 0;
	int nxtchr;
	int i;
        int ctr = 0;
	int count = 0;
	int decptflg = FALSE; /* flag multiple decimal pts. */
	char  c;
	char *fmtstr=FMT;

	while(isspace(nxtchr=*timestring++))
	*secstring++ = nxtchr; /* copy initial whitespace */

	/* initialize hr,min,secs to zero */
	for(i=0;i<MAXFIELDS;i++)times[i]=zeros[i];

/*   parsing loop: acquire field and process it. */

	while (!((nxtchr == ' ')||(nxtchr =='\t')||(nxtchr == '\0'))) {
		buffer[ctr] = nxtchr; /* store all input */
		if (nxtchr == FLDSEP) {  /* dump next field from temp */
			if (timeok){
				temp[count] = '\0';
				times[flds] = atof(temp);
				++flds;
				count = 0;
		  	           }
		   }
		else {      /* gets done for numerical input */
			if (!((nxtchr >='0'&&nxtchr <='9')||
				(nxtchr == '.' && decptflg == FALSE)))
				timeok = FALSE;
			temp[count] = nxtchr;
			++count;
		     }	
		if (nxtchr == '.') decptflg = TRUE;
		nxtchr = *timestring++; /* continue to acquire field */
		++ctr;
	   } /* end of parsing while, field now complete */

	buffer[ctr] = '\0'; /* properly end strings */
	temp[count] = '\0';
	if (timeok) {
		times[flds] = atof(temp);           /* last one */
		sprintf(secstring,fmtstr,secs(times,flds+1));
	        } 
	else sprintf(secstring,"%s",buffer);

	return timeok;
			
}
				


/* secs:  convert (hrs,mins,sec.s) to sec.s
    secs(float time[], int nfields) */

float secs(float times[],int nfields)
{
	float s = 0.0;
	int count = 0;

	if (nfields > MAXFIELDS || nfields <0)
        return 0.0;	
        else {
		while (count + 1 <= nfields) {
			s = 60.0*s + times[count];
			++count;
 		      }
	}
	return s;
}
