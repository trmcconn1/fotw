#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>



int
main()
{
	int my_pid;

        char buffer[80];

	my_pid = getpid();
	sprintf(buffer,"ps -c %d | tail -1 | sed 's/[ \t][ \t]*/ /g' | cut -d' ' -f6\n",my_pid);
	system(buffer);
}




