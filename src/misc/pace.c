/* pace.c   unfriendly compute pace for distance at target time

	usage: pace [-e -t -i ] distance target

	returns (double ) time in seconds

	options: -e: english units, -t: track event, -i: indoor

	target is in seconds

	distance:  km
 		   miles if -e
		   m     if -t
		   yd    if -e -t

	lap:       km
		   mile  if -e
		   400M  if -t
		   200M  if -t -e
		   440y  if -e -t
		   220y  if -e -t -i

	Bugs:      This program cannot handle odd length tracks

*/

#include "conversions.h"
#include <stdio.h>
#include <stdlib.h>

main ( int argc, char *argv[])
{
	double dunit = 1.0;
	double dist,lap = 1.0,time;
	char system, type, venue;
	int c;

	/* defaults */

	system = 'm';   /* metric */
	type = 'r';     /* road */
	venue = 'o';    /* outdoor */

	/* process options and set units */

	while (--argc > 0 && (*++argv)[0] == '-')
	  while ( c = *++argv[0] )
	  switch (c) {
	 	case 'e': 
			system = 'e';
			dunit = K_PER_MILE;  /* default dist unit is K */
			lap = dunit;
			break;
	  
		case 't':
			type = 't';
			break;
		case 'i':
			venue = 'i';
			break;
		case 'm':
			break;
		case 'o':
			break; /* patch: for script pacer */
		case 'r':
			break;
		default:
			printf("pace: illegal option\n");
			break;
	}
	  if (type =='t'){
		if(venue =='i'){
			if (system =='e')
			lap = dunit/8.0;
			else lap = .2;	
		}
		else    {   
			if (system =='e') {
			lap = dunit/4.0;
			}
			else lap = .4;
		}
	}
	if (argc != 2)
		printf("usage: pace [-e -t -i] dist target\n");
	else   {
		dist = atof(*argv)*dunit;
		time = atof(*++argv);
		printf("%g\n",lap*time/dist);
		}
}

			
				
