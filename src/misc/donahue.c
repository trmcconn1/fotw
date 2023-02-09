/* donahue.c   picks a plausible show title at random */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define LEN 9

char *random_choice_from(char *str[]);

int
main ()
{
	int n;

	char *adj[] = {"Gay ","Frustrated ","Hemophiliac ","Overweight ",
"Lesbian ", "Liberal ", "Elderly ", "Traumatized ", "Reactionary"};
	char *noun[] = {"drug addicts ","priests ","bus drivers ","Trump supporters",
	"strippers ",
"politicians ", "clowns ", "police officers ", "nurses "};
	char *verb[] = {"looking for ", "hiding from ", "in love with ",
	 "sueing ",
"envious of ", "who hate ", "intimidated by ", "fed up with ", "questioning "};
	char *object[] = {"their lovers.","their mothers.","their analysts.",
	"their dogs.",
"their professors.","their plumbers.", "their florists.", "their doctors.",
"elected representatives"};

	
/* Seed RNG */
	n = ((int)getpid())*((int)time(NULL));
	srandom(n);

	printf("\n%s\n","Today on Donahue:");
	printf("%s",random_choice_from(adj));
	printf("%s",random_choice_from(noun));
	printf("%s",random_choice_from(verb));
	printf("%s",random_choice_from(object));
	printf("\n\n");

}
char *random_choice_from(char *str[LEN])
{
	int rounded;
	
	rounded = random()/(RAND_MAX/LEN);

	return str[rounded];
}
   

