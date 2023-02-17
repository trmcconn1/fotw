/* fancytitle.c   Generate plausible random academic administrative title */

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

	char *qual[] = {" ","Assistant ","Vice ","Acting ",
"Deputy ", "Temporary ", "Consulting ", "Associate ", "Standing " };
	char *title[] = {"Professor ","Dean ","President ","Provost ",
	"Secretary ",
"Treasurer ", "Chancellor ", "Officer ", "Bursar "};
	char *genitive[] = {"of the Student Body ", "of the College ", "of the Division ",
	 "of the University ",
" ", "of Planning ", "of the Division ", "of the Senate ", "without Portfolio "};


	char *dative[] = {"for student ","for community ","for alumni ",
	"for faculty ", "for staff ", "for general ", "for research ",
"for fund-raising ","for minority ", "for student-athlete ", "for parental ",
"continuing education student "};
	char *noun[] = {"engagement ", "assessment ", "development ", "retention", "management ", "reinforcement ", "learning ", "motivation "};

	
/* Seed RNG */
	n = ((int)getpid())*((int)time(NULL));
	srandom(n);

	printf("%s",random_choice_from(qual));
	printf("%s",random_choice_from(title));
	printf("%s",random_choice_from(genitive));
	printf("%s",random_choice_from(dative));
	printf("%s",random_choice_from(noun));
	printf("\n\n");

}

char *random_choice_from(char *str[LEN])
{
	int rounded;
	
	rounded = random()/(RAND_MAX/LEN);

	return str[rounded];
}
   

