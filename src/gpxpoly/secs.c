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
#include <time.h>
#define MAXFIELDS 3
#define ERRMSG "secs: field out of range"
#define BUFSIZE 32
#define TRUE 1
#define FALSE 0
#define FLDSEP ':' /* separator within time string */

int unsecs( double secs );
double lsecs( double times[], int nfields);
double jd(int,int,int);
static char filler[BUFSIZE];

int secs(char *buf, double *timeptr) 
{
	double times[MAXFIELDS]; /* store (hh,mm,sec.s) */
	char buffer[BUFSIZ];
	char temp[BUFSIZE];
	int timeok = TRUE;        /* flag for invalid time string */
	int flds  = 0;
	int nxtchr;
        int ctr = 0;
	int count = 0;
	int infield = FALSE;  /* flag for in field or not */
	int decptflg = FALSE; /* flag to find multiple decimal pts. */

	for(flds=0;flds<MAXFIELDS;flds++)
		times[flds]=0.0;
	flds=0;

	while ((nxtchr = *buf++) != '\0')  /* main loop */
	{
	if ( infield == FALSE && (nxtchr == ' ' || nxtchr == '\t' ||
		nxtchr == '\n') )
		continue;
	else {                /* we are in a field */
	infield = TRUE;

/*   parsing loop: acquire field and process it. */

	while (!((nxtchr == ' ')||(nxtchr =='\t')||(nxtchr == '\n')||(nxtchr=='\0'))) {
		buffer[ctr] = nxtchr; /* store all input */
		if (nxtchr == FLDSEP) {  /* dump next field from temp */
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
		nxtchr = *buf++; /* continue to acquire field */
		++ctr;
	   } /* end of parsing while, field now complete */

	infield = FALSE;   /* signal that we are now outside a field */
	buffer[ctr] = '\0'; /* properly end strings */
	temp[count] = '\0';
	if (timeok) {
		times[flds] = atof(temp);           /* compute decimal time */
		*timeptr = lsecs(times,flds+1); /* and store it */
	}
	else return 1;
	}           /* end field-processing else clause */
	if(nxtchr == '\0')return 0;
	}               /* end main loop */ 
	return 0;
}
				


/* lsecs:  convert (hrs,mins,sec.s) to sec.s
    lsecs(double time[], int nfields) */

double lsecs(double times[],int nfields)
{
	double s = 0.0;
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

usage: unsecs(double second.s)

*/


int unsecs (double secs )
{
     int hh, mm;
     float ss, temp;

     hh = secs / 3600;
     temp = secs - (float) hh*3600;
     mm = temp / 60;
     ss = secs - (float) hh*3600 - (float) mm*60;
     if (hh == 0) {
          if( mm == 0){
			  printf("%.1f",ss);
		       }
          else  
	  if ( ss < 10.0 )
		printf("%d:0%.1f",mm,ss);
	  else printf("%d:%.1f",mm,ss);
          }
	else {
          if( mm == 0){
			if (ss < 10.0) 
				printf("%d:00:0%.1f",hh,ss); 
			 else printf("%d:00:%.1f",hh,ss);
		       }
          else  
	  if ( mm < 10 )
		if ( ss < 10.0 )
			printf("%d:0%d:0%.1f",hh,mm,ss);
		else    printf("%d:0%d:%.1f",hh,mm,ss);
	else 
		if ( ss < 10.0) 
			printf("%d:%d:0%.1f",hh,mm,ss);
		else
			printf("%d:%d:%.1f",hh,mm,ss);
	 }
     return 0;
}


/* Parse a date string, eg 4/30/54 -> struct tm  */
/* e.g., 4-30-54, 4/30/1954, and "4 30 54" are all ok Return 0 if parse
   succeeded, 1 else 

   Return the julian day at 00:00 UTC of this date through the second arg. 
*/

#define FIELDSEPS "-/."

int ParseDate( char *DateString , double *julian_day)
{
        char buf[32];
        char numbuf[5];
        char *ptr;
	int mm,dd,yyyy;
        int temp;


        strcpy(buf,DateString);
        
        /* Get Month */
        if((ptr = strtok(buf,FIELDSEPS))==NULL){
                return 1;
        } 
        strcpy(numbuf,ptr);
        if((temp = atoi(numbuf)) > 12 || temp < 0 )
		return 2;
        mm = temp;

        /* Get Day */   
        if((ptr=strtok(NULL,FIELDSEPS))==NULL){
                return 3;
        } 
        strcpy(numbuf,ptr);
        if((temp = atoi(numbuf)) > 31 || temp < 1 )
		return 4;
        dd = temp;

        /* Get Year */
        if((ptr=strtok(NULL,FIELDSEPS))==NULL){
                return 5;
        } 
        strcpy(numbuf,ptr);
        yyyy = atoi(numbuf);
	*julian_day = jd(mm,dd,yyyy);
	return 0;
}

/* Compute the julian date at midnight UTC. Uses the algorithm from
   page 61 of Jean Meeus, Astronomical Algorithms. */

double jd( int mm, int dd, int yyyy )
{
	double my_mm, my_dd, my_yyyy, my_jd, A,B;
	int temp;

	my_mm = (double)mm;
	my_dd = (double)dd;
	my_yyyy = (double)yyyy;

	if((mm==1) || (mm == 2)){
		my_yyyy=my_yyyy-1.0;
		my_mm = my_mm + 12.0;
	}
	
	temp = (int)(my_yyyy/100.0);
	A = (double)temp;

	temp = (int)(A/4.0);
	B = 2.0 - A + (double)temp;

	temp = (int)(365.25*(my_yyyy+4716.0));
	my_jd = (double)temp + my_dd + B - 1524.5;
	temp = (int)(30.6001*(my_mm + 1));
	my_jd += (double)temp;
	return my_jd;
}

