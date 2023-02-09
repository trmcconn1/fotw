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

int sunsecs (float secs , char *buffer)
{
     int hh, mm;
     float ss, temp;

     hh = secs / 3600;
     temp = secs - (float) hh*3600;
     mm = temp / 60;
     ss = secs - (float) hh*3600 - (float) mm*60;
     if (hh == 0) {
          if( mm == 0){
			  sprintf(buffer,"%.2f",ss);
		       }
          else  
	  if ( ss < 10.0 )
		sprintf(buffer,"%d:0%.2f",mm,ss);
	  else sprintf(buffer,"%d:%.2f",mm,ss);
          }
	else {
          if( mm == 0){
			if (ss < 10.0) 
				sprintf(buffer,"%d:00:0%.2f",hh,ss); 
			 else sprintf(buffer,"%d:00:%.2f",hh,ss);
		       }
          else  
	  if ( mm < 10 )
		if ( ss < 10.0 )
			sprintf(buffer,"%d:0%d:0%.2f",hh,mm,ss);
		else    sprintf(buffer,"%d:0%d:%.2f",hh,mm,ss);
	else 
		if ( ss < 10.0) 
			sprintf(buffer,"%d:%d:0%.2f",hh,mm,ss);
		else
			sprintf(buffer,"%d:%d:%.2f",hh,mm,ss);
	 }
     return 0;
}
