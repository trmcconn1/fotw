/* Calculates and prints the last time (GMT) that unix machines will have
   the correct time. (Extra for pedants: Actually, the time is 12 hours
   earlier GMT since the first machines to fail will be in New Zealand with
   local time GMT + 12 */

#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#define BIGLONG 2147483647   /* = 2^31 - 1, the largest number > 0 that can
                               be stored on a 32 bit machine using 2's
                               complement to represent negatives */ 

int
main()
{

	time_t last_time = BIGLONG;

	printf("Prepare ye! The end of unix time is %s\n", ctime(&last_time));
	return 0;
}
