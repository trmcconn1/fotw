/* secs.c exchange time string with second.s
       input: line
       prints: same line with 
		times or seconds converted.
       here: time string = hh:mm:sec.s, mm:sec.s, or secs.s

field separators: \n, \t, space     */
 
/* usage: secs [-1]

	options:  -1: convert seconds to time string
		  default: convert time string to secs
		  -d: output double precision answer
		  -n: Do not echo newlines
*/
/*    bugs:
		hundredths should be rounded to tenths in secs -1
		minutes <10 should be dispayed 09 etc in secs -1

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXFIELDS 3
#define ERRMSG "secs: field out of range"
#define BUFSIZE 10
#define TRUE 1
#define FALSE 0
#define FLDSEP ':' /* separator within time string */
#define FMT "%8.2f"

int unsecs( float secs );
float secs( float times[], int nfields);
int
main(int argc, char *argv[]) 
{
	char buffer[BUFSIZE];
	float times[MAXFIELDS]; /* store (hh,mm,sec.s) */
	static char filler[BUFSIZE];
	static float zeros[MAXFIELDS]; 
	char temp[BUFSIZE];
	int timeok = TRUE;        /* flag for invalid time string */
	int flds  = 0;
	int nxtchr;
        int ctr = 0;
	int count = 0;
	int infield = FALSE;  /* flag for in field or not */
	int decptflg = FALSE; /* flag to find multiple decimal pts. */
	char option = '\0', c;
	char *fmtstr=FMT;

	while (--argc > 0 && (*++argv)[0] == '-')
		while ( c = *++argv[0])
		switch (c) {
		case '1':
			option = '1';       /* handle option(s) */
			break;
		case 'd':
			fmtstr="%f";
			break;
		case 'n':
			option = 'n';
			break;
		default:
			printf("secs: illegal option %c\n", c);
			return 1;
		}
	if(argc != 0)
		printf("Usage: secs [-1dn]\n");
	else

	while ((nxtchr = getchar()) != EOF)  /* main loop */
	{
	if ( infield == FALSE && (nxtchr == ' ' || nxtchr == '\t' ||
		nxtchr == '\n') )
		{ if( option != 'n' || nxtchr != '\n') 
			putchar((char) nxtchr); /* echo whitespace if not */
	               		                   /* in field */
		}
	else {                /* we are in a field */
	infield = TRUE;
	*strcpy(times,zeros); 

/*   parsing loop: acquire field and process it. */

	while (!((nxtchr == ' ')||(nxtchr =='\t')||(nxtchr == '\n'))) {
		buffer[ctr] = nxtchr; /* store all input */
		if (nxtchr == FLDSEP) {  /* dump next field from temp */
			if (timeok){
				temp[count] = '\0';
				times[flds] = atof(temp);
				++flds;
				count = 0;
				*strcpy(temp,filler); /* zero out temp */

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
		nxtchr = getchar(); /* continue to acquire field */
		++ctr;
	   } /* end of parsing while, field now complete */

	infield = FALSE;   /* signal that we are now outside a field */
	buffer[ctr] = '\0'; /* properly end strings */
	temp[count] = '\0';
	if (timeok) {
		if(option == '1') {
		unsecs(atof(temp)); /* convert to time string */
		}
		else {
		times[flds] = atof(temp);           /* compute decimal time */
		printf(fmtstr,secs(times,flds+1)); /* and print it out */

	        } 
	}
	else printf("%s",buffer); /* or pass through non-time string */
	if( option != 'n' || nxtchr != '\n')
	putchar(nxtchr);		/* pass whitespace char, prepare */
	*strcpy(temp,filler);          /* for new imput */
	*strcpy(buffer,filler);
	flds = count = ctr = 0;
	timeok = TRUE;
	decptflg = FALSE;
	}           /* end field-processing else clause */
	}               /* end main loop */ 
	return 0;
			
}
				


/* secs:  convert (hrs,mins,sec.s) to sec.s
    secs(float time[], int nfields) */

float secs(float times[],int nfields)
{
	float s = 0.0;
	int count = 0;

	if (nfields > MAXFIELDS || nfields <0)
		printf(ERRMSG);
        else {
		while (count + 1 <= nfields) {
			s = 60.0*s + times[count];
			++count;
 		      }
	}
	return s;
}
			
/* unsecs.c     convert number of seconds to hh:mm:sec.s or
   mm:sec.s if hh = 0

usage: unsecs(float second.s)

*/


int unsecs (float secs )
{
     int hh, mm;
     float ss, temp;

     hh = secs / 3600;
     temp = secs - (float) hh*3600;
     mm = temp / 60;
     ss = secs - (float) hh*3600 - (float) mm*60;
     if (hh == 0) {
          if( mm == 0){
			  printf("%.2f",ss);
		       }
          else  
	  if ( ss < 10.0 )
		printf("%d:0%.2f",mm,ss);
	  else printf("%d:%.2f",mm,ss);
          }
	else {
          if( mm == 0){
			if (ss < 10.0) 
				printf("%d:00:0%.2f",hh,ss); 
			 else printf("%d:00:%.2f",hh,ss);
		       }
          else  
	  if ( mm < 10 )
		if ( ss < 10.0 )
			printf("%d:0%d:0%.2f",hh,mm,ss);
		else    printf("%d:0%d:%.2f",hh,mm,ss);
	else 
		if ( ss < 10.0) 
			printf("%d:%d:0%.2f",hh,mm,ss);
		else
			printf("%d:%d:%.2f",hh,mm,ss);
	 }
     return 0;
}
