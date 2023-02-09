#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "groups.h"
#include "events.h"
#include "table.c"

int ngroups = sizeof(AgeGroups)/(sizeof(char *));
int nevents = 16;
int AgeGroupNo(char *);

int main()
{
	int i=0,j;
	double std,  points;
	char title[] = "Noontime Running League Standards";


	InitTable();
	printf("<HTML>\n");
	printf("<HEAD>\n");
	printf("<TITLE> %s </TITLE>\n",title);
	printf("</HEAD>\n");
	printf("<BODY>\n");
	printf("<H2> %s </H2>\n<P>\n", title);
	for(i=0;i<ngroups;i++){
	printf("<P>");
	printf("<H3> %s Age Group </H3>\n<P>\n",AgeGroups[i]);
	printf("<TABLE CAPTION=%s>\n",AgeGroups[i]);
	printf("<TR> <TD> Event </TD> ");
	for(points=10;points > 1;points -= 0.5)
		printf("<TD> %4.1f </TD>",points);
	printf("</TR>\n");
	for(j=0;j<nevents;j++){
		printf("<TR> <TD> <STRONG> %s </STRONG> </TD>", EventNames[j]);
		points=10;
		std=table[i][j];
		while(points>1){
			if(strcmp(EventNames[j],"55m")==0)
			printf("<TD> %8.2f </TD>", floor(std*2000.0/(10.0 + points))/100.0);
			else
			printf("<TD> %8.1f </TD>", floor(std*200.0/(10.0 + points))/10.0);
			points-=0.5;
		}
		printf("</TR>\n");	
	}
	printf("</TABLE>\n");
	printf("<P>");
	}
	printf("</BODY>\n");
	printf("</HTML>\n");
	
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
                else return 24;
		}
        if(strcmp(Sex,"m")==0 && age > 29)
                return (age-30)/5 + 6;
        if(strcmp(Sex,"f")==0 && age > 29)
                 return  (age-30)/5 + 25;
        if(age <= 19 && age > 11)
                if(strcmp(Sex,"m")==0)
                 return (age-12)/2 + 1;
                else return (age-12)/2 +20;
       if(age <= 11 && age > 9)
               if(strcmp(Sex,"m")==0)
                 return 0;
               else return 19;
	if(age <= 9)
		if(strcmp(Sex,"m")==0)
		 return 0; /* wrong */
		else return 18;
}


