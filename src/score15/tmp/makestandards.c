/* Make Standards: adapted from mkhtmlstnds.c that writes a web page of age group standards for the noontime running
league. This program outputs a text file in csv format that is suitable for importing the standards into an access
database table. (Specifically the standards table of one of my noontime league annual results databases. */



#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "groups.h"
#include "events.h"

extern double table[34][16]; // defined in table.c


int ngroups = sizeof(AgeGroups)/(sizeof(char *));
int nevents = 16;
int AgeGroupNo(char *);

int main()
{
	int i=0,j,temp,hrs,mins;
	double std,secs;
	char header[] = "age_low,age_hi,sex,distance,hrs,mins,secs";


	InitTable();
	printf("%s\n", header);
	for(i=0;i<ngroups;i++){
		for(j=0;j<nevents;j++){
			std=table[i][j]; // This is given in seconds
		/* Extract hrs,mins, seconds from std */
			temp = (int)floor(std);
			hrs = temp/3600;
			temp %= 3600;
			mins = temp/60;
			temp = hrs*3600 + mins*60;
			secs = std - (double)temp;
			printf("%d,%d,%s,%s,%d,%d,%.2f\n",AgeGroupsAge_Lo[i],AgeGroupsAge_Hi[i],
				AgeGroupsSex[i],EventNames[j],hrs,mins,secs);
		}
	}
}

// Probably will not be needed, but left in just in case
#if 0
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
#endif
