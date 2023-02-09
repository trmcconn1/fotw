/* atosecs.c (function version)
	returns the number of seconds in a time string.
       here: time string = hh:mm:sec.s, mm:sec.s, or secs.s
*/
 
/* usage: unsigned long int atosecs(char *time_string)
  	ALSO HAS: SecsToStr  (cf below)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXFIELDS 3
#define BUFSIZE 20
#define TRUE 1
#define FALSE 0
#define FLDSEP ':' /* separator within time string */
#define FMT "%7.2f"

double secs( double times[], int nfields);

double atosecs(char *time_string)
{
	char buffer[BUFSIZE];
	char secsbuf[BUFSIZE];
	double times[MAXFIELDS]; /* store (hh,mm,sec.s) */
	static char filler[BUFSIZE];
	char temp[BUFSIZE];
	int timeok = TRUE;        /* flag for invalid time string */
	int flds  = 0;
	int nxtchr;
	int ctr = 0;
	int count = 0;
	int decptflg = FALSE; /* flag to find multiple decimal pts. */
	char *fmtstr=FMT;

	nxtchr = *time_string;
	while (nxtchr  != '\0')  /* main loop */
	{
		buffer[ctr] = nxtchr; /* store all input */
		if (nxtchr == FLDSEP){   /* dump next field from temp */
			if (timeok){
				temp[count] = '\0';
				times[flds] = atof(temp);
				++flds;
				count = 0;
				strcpy(temp,filler); /* zero out temp */
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
		nxtchr = *++time_string;
		++ctr;
	}
	buffer[ctr] = '\0'; /* properly end strings */
	temp[count] = '\0';
	if (timeok) {
		times[flds] = atof(temp);           /* compute decimal time */
		sprintf(secsbuf,fmtstr,secs(times,flds+1)); /* and print it out */
		return atof(secsbuf);
		} 
	else return 0.0;
}
				
/* secs:  convert (hrs,mins,sec.s) to sec.s
    secs(double time[], int nfields) */

double secs(double times[],int nfields)
{
	double s = 0.0;
	int count = 0;

		while (count + 1 <= nfields) {
			s = 60.0*s + times[count];
			++count;
		      }
	return s;
}

/* SecsToStr    convert number of seconds to hh:mm:sec.s or
   mm:sec.s if hh = 0

usage: SecsToStr(float second.s, char *TimeString)

*/
int SecsToStr(double secs,  char *TimeString  )
{
     int hh, mm;
	  double ss, temp;
	  char *ptr;


	  ptr = TimeString;
     hh = secs / 3600;
	  temp = secs - (double) hh*3600;
     mm = temp / 60;
	  ss = secs - (double) hh*3600 - (double) mm*60;
     if (hh == 0) {
          if( mm == 0){
			  sprintf(ptr,"%.2f",ss);
		       }
          else  
	  if ( ss < 10.0 )
		sprintf(ptr,"%d:0%.2f",mm,ss);
	  else sprintf(ptr,"%d:%.2f",mm,ss);
          }
	else {
          if( mm == 0){
			if (ss < 10.0) 
				sprintf(ptr,"%d:00:0%.2f",hh,ss);
			 else sprintf(ptr,"%d:00:%.2f",hh,ss);
		       }
          else  
	  if ( mm < 10 )
		if ( ss < 10.0 )
			sprintf(ptr,"%d:0%d:0%.2f",hh,mm,ss);
		else    sprintf(ptr,"%d:0%d:%.2f",hh,mm,ss);
	else 
		if ( ss < 10.0) 
			sprintf(ptr,"%d:%d:0%.2f",hh,mm,ss);
		else
			sprintf(ptr,"%d:%d:%.2f",hh,mm,ss);
	 }
     return 0;
}
