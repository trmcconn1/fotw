/* Implementations of the menu choices in menu.cpp */
/* menuxy: choice y on menu x */


#include "actions.h"
#include "gui.h"
#include <string.h>
#include <stdlib.h>
#include <constrea.h>
#include <iomanip.h>
#include <fstream.h>
#include <conio.h>

static char buffer[BUFSIZ];

static FILE *devnull;

void UpdateMessage(char *msg)
{
        char buffer[300];
        char loaded[10];
        if(GetShell("LOADED")==NULL)
                sprintf(loaded,"---");
        else strcpy(loaded,GetShell("LOADED"));
        sprintf(buffer, " %s %s [%s ] %s",GetShell("CWD"),GetShell("SEM"),
          loaded, DEFAULTMESSAGE);
        buffer[80] = '\0';
        strcpy(msg,buffer);
        return;
}
        
void initialize(FILE **openfile, FILE **errorstream)
{

// turn off shell prompting: we don't need it in the ui.

       set("ECHO","OFF",3,NULL);
       *openfile = fopen(INFO,"r");    // Start with info displayed
       rewind(*openfile);
       devnull = fopen("NUL","w");
       *errorstream = devnull;
        set("TEMPFILE",TempFilePath,3,NULL);
        set("MENU","ON",3,NULL);
	strcpy(Normalprinter,GetShell("NORMALPRINTER"));
	strcpy(Landscape,GetShell("LANDSCAPE"));
	if(strlen(Normalprinter)==0)
		strcpy(Normalprinter,DEFAULTNORMALPRINTER);
	if(strlen(Landscape)==0)
		strcpy(Landscape,DEFAULTNORMALPRINTER);
       return;
}

/* Action to open the schedule and put in Main Window */

void menu10(FILE *openfile)
{
        fclose(openfile);
        openfile = fopen(TempFilePath,"w");
        strcpy(mystatus.OpenFile,TempFilePath);
        numtokens = 2;
        strcpy(Tokens[1]->text,Semester);
        schedule(NULL,openfile,NULL,NULL);
        fclose(openfile);
        openfile = fopen(TempFilePath,"r");
        FirstScreen(openfile,&Main_Window);
}

/* Action to run the audit program */

void menu11(FILE *openfile)
{


	strcpy(DialogueData.Title,"           Staff Audit");
	strcpy(DialogueData.msg1,"Use AY or Semester Loads[a|s]?");
	strcpy(DialogueData.msg2,"AY uses half the AY load each sem.");
	if(MYDialogue()==0){
		if(strcmp(DialogueData.Input,"a")!= 0 &&
			strcmp(DialogueData.Input,"s")!=0){
			myerror(&mystatus,"Unrecognized response: ",DialogueData.Input);
			Status(&Status_Bar,&mystatus,0);
			DumpCurrentPage();
			return;
		}
		if(strcmp(DialogueData.Input,"a")==0)
			strcpy(buffer,"-at");
		else strcpy(buffer,"-t"); // buffer now has audit flag
		}
		else {
			DumpCurrentPage();/* redraw */
		}
		strcpy(DialogueData.msg1,"Enter last 2 digits of Academic Year.");
		strcpy(DialogueData.msg2,"e.g., enter 97 in 1996-97.");
		if(MYDialogue()==0){

                  /* The work is done by a take file called audit.ui */

			fclose(openfile);
			openfile = fopen(TempFilePath,"w");
			strcpy(mystatus.OpenFile,TempFilePath);
                  /* Arg for take command */

			numtokens = 2;
			strcpy(Tokens[1]->text,"scripts\\audit.ui");

                  /* Pass User responses to take file using shell vars */
			set("AY",DialogueData.Input,3,NULL);
			set("AUDIT_FLAGS",buffer,3,NULL);


			devnull = fopen("NUL","w");
			take(NULL,openfile,devnull,devnull);
			fclose(openfile);
			openfile = fopen(TempFilePath,"r");
			FirstScreen(openfile,&Main_Window);
			fclose(devnull);

		}
		else {
			DumpCurrentPage();
		}
	return;
}

/* Action to run the room chart program */

void menu12(FILE **openfile)
{

	strcpy(DialogueData.Title,"              Room Chart");
	strcpy(DialogueData.msg1,"Enter date of start of time block.");
	strcpy(DialogueData.msg2,"e.g. 9/11/96");
	if(MYDialogue() == 0)
		set("START_DATE",DialogueData.Input,3,NULL);

	else{

		DumpCurrentPage();
		return;
	}
	strcpy(DialogueData.msg1,"Enter time of start of time block.");
	strcpy(DialogueData.msg2,"e.g. 8:30, or <esc> to bail out");
	if(MYDialogue() == 0)
		set("START_TIME",DialogueData.Input,3,NULL);

	else{
		set("START_DATE","",2,NULL);
		DumpCurrentPage();
		return;
	}
	strcpy(DialogueData.msg1,"Enter date of end of time block.");
	strcpy(DialogueData.msg2,"e.g. 9/11/96, or <esc> to bail out");
	if(MYDialogue() == 0)
		set("END_DATE",DialogueData.Input,3,NULL);

	else{
		set("START_DATE","",2,NULL);
		set("START_TIME","",2,NULL);

		DumpCurrentPage();
		return;
	}
	strcpy(DialogueData.msg1,"Enter time of end of time block.");
	strcpy(DialogueData.msg2,"e.g. 6:00 pm, or <esc> to bail out");
	if(MYDialogue() == 0)
		set("END_TIME",DialogueData.Input,3,NULL);

	else{
		set("START_DATE","",2,NULL);
		set("START_TIME","",2,NULL);
		set("END_DATE","",2,NULL);
		DumpCurrentPage();
		return;
	}
    
        fclose(*openfile);
        
	numtokens = 2;
	strcpy(Tokens[1]->text,"scripts\\roomchar");
	take(NULL,devnull,devnull,devnull);
        *openfile = fopen(TempFilePath,"r");
	strcpy(mystatus.OpenFile,TempFilePath);
        FirstScreen(*openfile,&Main_Window);
        
	return;
}

/* menu choice to run the display people info routines */

void menu13(FILE *openfile)
{
        int i,j,k;
	char *ptr;

	fclose(openfile);
	openfile = fopen(TempFilePath,"w");
	numtokens = 1;
	dpeople(NULL,openfile,openfile,NULL);
	fclose(openfile);
	openfile = fopen(TempFilePath,"r");
	FirstScreen(openfile,&Main_Window);
	strcpy(mystatus.OpenFile,TempFilePath);
	strcpy(DialogueData.Title,"          View Personnel Data");
        strcpy(DialogueData.msg1,"Enter a number, all, wrong, or a range");
	strcpy(DialogueData.msg2,"For a range enter, e.g., 5-23");

	if(MYDialogue()==0){
		strcpy(buffer,DialogueData.Input);
		fclose(openfile);
		openfile = fopen(TempFilePath,"w");
		numtokens = 2;
		if(strcmp(DialogueData.Input,"all")==0)
			for(i=0;i<numpeople;i++){
				sprintf(Tokens[1]->text,"%d",i+1);
				fprintf(openfile,"Semester: %s %65s\n",
				GetShell("SEM"),GetShell("DATE"));
				dpeople(NULL,openfile,openfile,NULL);
				fprintf(openfile,"\n-------------------\n\n");
			}
		else {
                        if(strcmp(DialogueData.Input,"wrong")==0){
                          for(i=0;i<numpeople;i++)
                                if(People[i]->load_sem != People[i]->load_now){
                                  sprintf(Tokens[1]->text,"%d",i+1);
                                  fprintf(openfile,"Semester: %s %65s\n",
                                  GetShell("SEM"),GetShell("DATE"));
                                  dpeople(NULL,openfile,openfile,NULL);
                                  fprintf(openfile,"\n-------------------\n\n");
                                }
                        }
                        else {
                         if(strchr(DialogueData.Input,'-')==NULL){
				strcpy(Tokens[1]->text,DialogueData.Input);
				dpeople(NULL,openfile,openfile,NULL);
                         }
                         else{
				j= atoi(strtok(DialogueData.Input,"-"));
				if((ptr = strtok(NULL,"-")) == NULL){
					myerror(&mystatus,"Bad Input:", DialogueData.Input);
					fclose(openfile);
					openfile = fopen(TempFilePath,"r");
					DumpCurrentPage();
					return;
				}
				k = atoi(ptr);
				if(k < j){
					myerror(&mystatus,"Bad Input:", DialogueData.Input);
					fclose(openfile);
					openfile = fopen(TempFilePath,"r");
					DumpCurrentPage();
					return;
				}

				for(i=j;i<k;i++){
					sprintf(Tokens[1]->text,"%d",i);
					fprintf(openfile,"Semester: %s %65s\n",
					GetShell("SEM"),GetShell("DATE"));
					dpeople(NULL,openfile,openfile,NULL);
					fprintf(openfile,"\n-------------------\n\n");
				}
                          }
                        }
		}
		fclose(openfile);
		openfile = fopen(TempFilePath,"r");

	}
	else{
		DumpCurrentPage();
		return;
	}
	FirstScreen(openfile,&Main_Window);
	return;
}

/* finals menu choice */

void menu14(FILE *openfile)
{
	fclose(openfile);
	openfile = fopen(TempFilePath,"w");
	strcpy(mystatus.OpenFile,TempFilePath);
	numtokens = 1;
	dfinals(NULL,openfile,NULL,NULL);
	fclose(openfile);
	openfile = fopen(TempFilePath,"r");
	FirstScreen(openfile,&Main_Window);
	return;
}

/* Action to set the SEM shell variable */

void menu20(void)
{
	strcpy(DialogueData.Title,"           Set Semester");
	strcpy(DialogueData.msg1,"Enter Semester (e.g f97). ");
	strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

	if(MYDialogue()==0)
		strcpy(Semester,DialogueData.Input);
	DumpCurrentPage();/* redraw */
	return;
}

/* Display and set variables interactively */

void menu21(FILE **openfile)
{
	fclose(*openfile);
	*openfile = fopen(TempFilePath,"w");
	set("","",1,*openfile);
	fclose(*openfile);
	*openfile = fopen(TempFilePath,"r");
        Main_Window.clrscr();
	FirstScreen(*openfile,&Main_Window);
	strcpy(mystatus.OpenFile,TempFilePath);
	strcpy(DialogueData.Title,"          Set Shell Variable");
	strcpy(DialogueData.msg1,"Enter Name of Variable to set.");
	strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

	if(MYDialogue()==0)
		strcpy(buffer,DialogueData.Input);
	else {
		DumpCurrentPage();
		return;
	}
	strcpy(DialogueData.msg1,"Enter value of variable to set.");
	strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

	if(MYDialogue()==0){
		set(buffer,DialogueData.Input,3,NULL);
		fclose(*openfile);
		*openfile = fopen(TempFilePath,"w");
		set("","",1,*openfile);
		fclose(*openfile);
		*openfile = fopen(TempFilePath,"r");
                Main_Window.clrscr();
		FirstScreen(*openfile,&Main_Window);
	}
	return;
}

/* Set WD */

void menu22(void)
{
	strcpy(DialogueData.Title,"       Change Directory");
	strcpy(DialogueData.msg1,"Enter path of dir. to change to. ");
	strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");
	if(MYDialogue()==0){
		strcpy(Tokens[1]->text,DialogueData.Input);
		numtokens = 2;
		cd(NULL,NULL,NULL,NULL);
		strcpy(WorkingDirectory,DialogueData.Input);
	}
	DumpCurrentPage();

	return;
}

/* Set the TBA symbol */

void menu23(void)
{
	strcpy(DialogueData.Title," Set Missing Data Symbol");
	strcpy(DialogueData.msg1,"string to stand for missing Data?");
	strcpy(DialogueData.msg2,"For example, TBA or NA");
	if(MYDialogue()==0){
		strcpy(MissingData,DialogueData.Input);
		set("MISSINGDATA",DialogueData.Input,3,NULL);
	}
	DumpCurrentPage();
	return;
}

void menu24()
{
return;
}
/* Run the interactive shell */

void menu30(void)
{
	numtokens = 2;
	strcpy(Tokens[1]->text,"-free");
	load(NULL,NULL,NULL,NULL);
	system("cls");
	Main_Menu.Wdw->rdbuf()->setcursortype(_NORMALCURSOR);

	set("ECHO","ON",3,NULL);
	Dialogue(stdin,stdout,stderr,stdout,"Interactive Command Shell(1.0)",
		NULL,&shell);
	set("ECHO","OFF",3,NULL);
	Title_Line.clrscr();
	Title_Line << TITLE << " -- " << GetShell("VERSION") << "--";
	DisplayMainMenu(&Main_Menu);
        Status_Bar.clrscr();
	Status(&Status_Bar,&mystatus,1);
	DumpCurrentPage();

	return;
}

/* Run the Newsmstr program */

void menu31()
{
	numtokens = 2;
	strcpy(Tokens[1]->text,"-free");
	load(NULL,NULL,NULL,NULL);
	system("cls");
	Main_Menu.Wdw->rdbuf()->setcursortype(_NORMALCURSOR);
	system(" cd src\\newsmstr");
	system("newsmstr");
	strcpy(buffer,"cd ");
	strcat(buffer,WorkingDirectory);
	system(buffer);
	system("cls");
	Title_Line.clrscr();
	Title_Line << TITLE;
	DisplayMainMenu(&Main_Menu);
	Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
        Status_Bar.clrscr();
	Status(&Status_Bar,&mystatus,1);
	DumpCurrentPage();
	return;
}

/*  Load Data */

void menu32(FILE *openfile, FILE *errorstream)
{

        fclose(openfile);

       // Errorstream is a global stream that the
       // data file parsers print to.
        holdstream = errorstream;
        errorstream = (openfile = fopen(TempFilePath,"w"));
        strcpy(mystatus.OpenFile,TempFilePath);
        numtokens = 2;
        strcpy(Tokens[1]->text,"-free");   // free any previous load
        load(NULL,NULL,openfile,NULL);                                  
        numtokens = 1;
        load(NULL,NULL,openfile,NULL);
        numtokens = 2;
        strcpy(Tokens[1]->text,"data\\sched.");
        strcat(Tokens[1]->text,Semester);
        take(NULL,NULL,openfile,NULL);
        strcpy(Tokens[1]->text,"data\\finals.");
        strcat(Tokens[1]->text,Semester);
        numtokens = 2;
        take(NULL,NULL,openfile,NULL);
        errorstream = holdstream;

        fclose(openfile);
        openfile = fopen(TempFilePath,"r");
        FirstScreen(openfile,&Main_Window);
        return;
}

/* Unload menu choice */

void menu33()
{
        numtokens = 2;
        strcpy(Tokens[1]->text,"-free");   // free any previous load
        load(NULL,NULL,devnull,NULL);
        DumpCurrentPage();

        return;
}

void menu34()
{
return;
}
void menu42()
{
return;
}
void menu43()
{
return;
}
void menu44()
{
return;
}
                                                      

