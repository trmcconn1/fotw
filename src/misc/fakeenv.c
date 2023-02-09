/* Testing how minix handles environment */

#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int
main(void)
{
	char **ep;
	char *fake_env[3] = {"FOO=BAR","IGNORANCE=STRENGTH",NULL};

	ep = environ;

	printf("The real environment:\n");
	for(; *ep != NULL;ep++)
		printf("%s\n",*ep);

	environ = fake_env;
	ep = environ;

	printf("\n\nThe fake environment:\n");
	for(; *ep != NULL;ep++)
		printf("%s\n",*ep);

	printf("\n\nUsing getenv to access fake environment:\n");
	printf("IGNORANCE=");
	printf("%s\n",getenv("IGNORANCE"));

	printf("\n\nUsing getenv to access real environment:\n");
	printf("SHELL=");
	printf("%s\n",getenv("SHELL"));

	return 0;
}
