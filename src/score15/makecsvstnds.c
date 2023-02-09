#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "groups.h"
#include "events.h"
#include "table.c"

int ngroups = sizeof(AgeGroups)/(sizeof(char *));
int nevents = 15;
int AgeGroupNo(char *);

int main()
{
	int i=0,j;
	double std,  points;
	char title[] = "Noontime Running League Standards";


	InitTable();
	for(i=0;i<ngroups;i++){
	printf("%s Age Group\n",AgeGroups[i]);
	printf("Event,");
	for(points=10;points > 1;points -= 0.5)
		printf("%4.1f,",points);
	printf("\n");
	for(j=0;j<nevents;j++){
		printf("%s,",EventNames[j]);
		points=10;
		std=table[i][j];
		while(points>1){
			if(strcmp(EventNames[j],"55m")==0)
			printf("%8.2f,", floor(std*2000.0/(10.0 + points))/100.0);
			else
			printf("%8.1f,", floor(std*200.0/(10.0 + points))/10.0);
			points-=0.5;
		}
		printf("\n");	
	}
	}
}

int AgeGroupNo(char *g)
{
	int age;  // on the day of run CompetitionYear
	char Sex[2];

	if(g[0]=='m')strcpy(Sex,"m");
		else strcpy(Sex,"f");
	age = atoi(g+1);

        if(age <= 29 && age >= 20){
                        if (strcmp(Sex,"m")==0)return 5;
                else return 23;
		}
        if(strcmp(Sex,"m")==0 && age > 29)
                return (age-30)/5 + 6;
        if(strcmp(Sex,"f")==0 && age > 29)
                 return  (age-30)/5 + 24;
        if(age <= 19 && age > 11)
                if(strcmp(Sex,"m")==0)
                 return (age-12)/2 + 1;
                else return (age-12)/2 +19;
       if(age <= 11 && age > 9)
               if(strcmp(Sex,"m")==0)
                 return 0;
               else return 18;
	if(age <= 9)
		if(strcmp(Sex,"m")==0)
		 return 0; /* wrong */
		else return 17;
}


