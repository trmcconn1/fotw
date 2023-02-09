/*
* curse1.c
*
* Get user's name and print a personalized curse. */
#include <stdio.h>
#define NBYTES   15

main()
{
     char name[NBYTES];  /* input buffer */
     printf("what is your name? ");

     /* read the user's input */
     gets(name);

     /* print a personal "greeting" */

     printf("\nFuck you, %s, you fucking asshole. You shit-for-brains, cum
guzzling,vaginal blood-fart sniffing, quiff-burrowing, micro-cephalic
pee-hole!\n",name);
}
