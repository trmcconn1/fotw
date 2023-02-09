/* menu.cpp: C++ implementation of user interface for schedule pgm      */

/* By Terry McConnell, 1996 */


#define TITLE "    Mathematics Department Scheduling Program"
#define __MENU__  

#include <constrea.h>
#include <iomanip.h>
#include <fstream.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <bios.h>
#include <io.h>
#include <errno.h>
extern "C" {
#include "shell.h"
}

extern "C" int load(FILE *, FILE *, FILE *, FILE *);
extern "C" int take(FILE *, FILE *, FILE *, FILE *);
extern "C" int schedule(FILE *, FILE *, FILE *, FILE *);
extern "C" int audit(FILE *, FILE *, FILE *, FILE *);
extern "C" int shell(FILE *, FILE *, FILE *, FILE *,char *errmsg, void *data);
extern "C" int cd(FILE *, FILE *, FILE *, FILE *);
extern "C" int dpeople(FILE *,FILE *,FILE *, FILE *);
extern "C" int dfinals(FILE *,FILE *,FILE *, FILE *);
extern "C" int Dialogue(FILE *, FILE *, FILE *,FILE *,
                char *title, void *data,
                int ( *DialogueHandler ) ( FILE *, FILE *, FILE *,FILE *,
                        char *errmsg, void *data) );
extern "C" char *GetShell(char *);

extern struct TokenType *Tokens[];


#define SCREENLINES 22
#define MAIN_MENU_ITEMS 5
#define MAX_ITEMS 5          /* on a submenu */
#define MAIN_MENU_ITEM_WIDTH 8
#define BACKSPACE 0x08
#define RIGHTARROW 0x4D
#define LEFTARROW 0x4B
#define DOWNARROW 0x50
#define UPARROW 0x48
#define LINEFEED '\x0A'
#define RETURN '\x0D'
#define ESCAPE '\x1B'
#define MAXLINES 5000
#define LINELENGTH 80
#define PAGEUP 0x49
#define PAGEDOWN 0x51
#define END 0x4F
#define HOME 0x47
#define TAB  "     "
#define CONTROL_HOME 0x77
#define CONTROL_END 0x75
#define CONTROL_PAGEUP 0x84
#define CONTROL_PAGEDOWN 0x76
#define CONTROL_RIGHTARROW 0x74
#define CONTROL_LEFTARROW 0x73
#define UPARROWSYMBOL 0x18
#define DOWNARROWSYMBOL 0x19

#define PROMPTSTRING "input>"

enum MenuItemState { NORMAL, SELECTED, DIMMED };
enum Focus { MENUFOCUS, WINDOWFOCUS , SUBMENUFOCUS};

int lineno = 0;
int TotalLines;  // Number of logical lines in file
char LineBuf[LINELENGTH+1];
int X=1,Y=1; /* cursor positions */
fpos_t LinePositions[MAXLINES]; /* offsets of each logical line in file */
			       /* Logical lines count parts of lines which
				  are split off by word wrap   */

constream Main_Window;
constream DialogueBox;

/* My own extractor that does tab to spaces conversion */
ostream& operator <<= (ostream&, char *);

struct DialogueDataStruct {
char Title[40];
char Prompt[10];
char Input[40];
char msg1[40];
char msg2[40];
}DialogueData;

struct StatusInfo {
char OpenFile[80];
char Message[80];
};
struct StatusInfo mystatus; 

int DumpToScreen(FILE *, constream *);
static int MYDialogue();
int ReverseDumpToScreen(FILE *, constream *, int,fpos_t * );  
char *myfgets(char *s, int n, FILE *fp);
void Status(constream *,struct StatusInfo *);
void myerror(struct StatusInfo *,char *, char *);
void FirstScreen(FILE *, constream *);
int PrintOut(FILE *tempfile, int setup);
extern "C" char TempFilePath[];

FILE *tempfile;
FILE *openfile;
FILE *devnull;
static FILE *holdstream;
int handle;

struct MenuItem{
constream *Wdw;
int state;
char text[20];
char ForeGroundColor;
char BackGroundColor;
int BeginRow;
int BeginCol;
int EndRow;
int EndCol;
} ;

struct SUBMENU {
int NoItems;
int CurrentItem;
constream *Wdw;
struct MenuItem Item[MAX_ITEMS];
int BeginRow;
int BeginCol;
int EndRow;
int EndCol;
} ;

struct MENU {
  int CurrentItem;
  constream *Wdw;
  struct MenuItem Item[MAIN_MENU_ITEMS];
  int BeginRow;
  int BeginCol;
  int EndRow;
  int EndCol;
	};

// Externally defined Menu Actions

int DisplayMainMenu(struct MENU *);
int DisplaySubMenu(int,struct SUBMENU *);
int DisplayMainWindow(constream *,FILE *);
void DumpCurrentPage();

static char buffer[BUFSIZ];
constream Status_Bar;
constream Title_Line;

extern "C" int menu();        // so as not to mangle the name.
 struct SUBMENU SubMenu[MAIN_MENU_ITEMS];
 struct MENU  Main_Menu;

int
menu()
{       int longkey;
	char key;
	int c;
        char *ptr;
        int i,j,k;
	int CurrentFocus = WINDOWFOCUS;
       fpos_t start,end;         /* offsets in file */
       constream PaintOver;
       FILE *tempfilehold;

// turn off shell prompting: we don't need it in the ui.
       set("ECHO","OFF",3,NULL);
       devnull = fopen("NUL","w");
       errorstream = devnull;       // don't want error msgs mucking up screen
       tempfile = fopen(TempFilePath,"r");
       openfile = tempfile;
       rewind(openfile);
	// Initialize the status bar at bottom of screen 


        strcpy(mystatus.OpenFile,TempFilePath);
        set("TEMPFILE",TempFilePath,3,NULL);
        set("MENU","ON",3,NULL);
	strcpy(mystatus.Message, "   <esc> = Menu Activate");

	Status_Bar.window(1,25,80,25);
//        Status_Bar.rdbuf()->setcursortype(_NOCURSOR);
	Status_Bar << setattr((BROWN << 4) | WHITE); 
	   Status(&Status_Bar,&mystatus);
     
	// PaintOver:
	// This silly thing is needed because when you print the
	// text of a menu item, the cursor always falls one place beyond
	// the end and changes the screen color there. (even though the
	// cursor itself is invisible.) This stream is then painted over
	// that part in the main screen color to render it invisible.
	PaintOver.rdbuf()->setcursortype(_NOCURSOR);
	PaintOver << setattr((BLUE << 4) | BLUE);

	// Set up Dialogue Box Window

	DialogueBox.window(30,10,73,18);
	DialogueBox << setattr((RED << 4) | YELLOW);
	strcpy(DialogueData.msg1," ");
	strcpy(DialogueData.msg2,"<Enter> to apply, <esc> to dismiss");
	strcpy(DialogueData.Title, "Dialogue Box");
        strcpy(DialogueData.Prompt,PROMPTSTRING);

	// Set up Main Menu and submenus
       
	Main_Menu.CurrentItem=0;  
	Main_Menu.BeginCol = 1;
	Main_Menu.EndCol = 80;
	Main_Menu.BeginRow = 2;
	Main_Menu.EndRow = 2;
	Main_Menu.Wdw =  new constream;
	Main_Menu.Wdw->rdbuf()->setcursortype(_NOCURSOR);
	Main_Menu.Wdw->window(Main_Menu.BeginCol,Main_Menu.BeginRow,
		Main_Menu.EndCol, Main_Menu.EndRow);
	*(Main_Menu.Wdw) << setattr((BLUE << 4) | BLUE);
	Main_Menu.Wdw->clrscr();
	*(Main_Menu.Wdw) << setattr((MAGENTA << 4) | DARKGRAY);

        /* Width of the strings should be same as MAINMENUITEMWIDTH */
        strcpy(Main_Menu.Item[0].text, "| File |");
        strcpy(Main_Menu.Item[1].text, " View  |");
        strcpy(Main_Menu.Item[2].text, "  Set  |");
        strcpy(Main_Menu.Item[3].text, " Action|");
        strcpy(Main_Menu.Item[4].text ,  " Help  |");
	/* Initialize Menu Items */
	for(i=0;i<MAIN_MENU_ITEMS;i++){
	  Main_Menu.Item[i].Wdw = new constream;
	  Main_Menu.Item[i].Wdw->rdbuf()->setcursortype(_NOCURSOR);
	  Main_Menu.Item[i].BeginRow = Main_Menu.BeginRow;
	  Main_Menu.Item[i].EndRow = Main_Menu.EndRow;
	  Main_Menu.Item[i].BeginCol = Main_Menu.BeginCol +
		MAIN_MENU_ITEM_WIDTH*i;
	  Main_Menu.Item[i].EndCol = Main_Menu.Item[i].BeginCol
		+ MAIN_MENU_ITEM_WIDTH;
	Main_Menu.Item[i].Wdw->window(Main_Menu.Item[i].BeginCol,
		Main_Menu.Item[i].BeginRow,Main_Menu.Item[i].EndCol,
		Main_Menu.Item[i].EndRow);

	  Main_Menu.Item[i].state = NORMAL;
	  Main_Menu.Item[i].ForeGroundColor = WHITE;
	  Main_Menu.Item[i].BackGroundColor = MAGENTA;
	  Main_Menu.Item[i].Wdw->clrscr();
	  Main_Menu.Item[i].Wdw->rdbuf()->textbackground(MAGENTA);
	  Main_Menu.Item[i].Wdw->rdbuf()->textcolor(WHITE);
	 *(Main_Menu.Item[i].Wdw) << Main_Menu.Item[i].text;
          /* Initialize submenus */         
	  switch(i) {
		case 0:           /* File submenu */
			SubMenu[i].NoItems = 5;
			for(j=0;j<SubMenu[i].NoItems;j++)
			switch(j) {
		       case 0:
                           strcpy(SubMenu[i].Item[j].text,"Open    ");             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Load    ");             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Save As ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,"Print   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Quit    ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       default: ;
		 }
			break;
		case 1:
			SubMenu[i].NoItems = 5;
			for(j=0;j<SubMenu[i].NoItems;j++)
			switch(j) {
		       case 0:
                           strcpy(SubMenu[i].Item[j].text,"Schedule");             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Audit   ");             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,"RmChart ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,"People  ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Finals  ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       default: ;
		 }

			break;
		case 2:
			SubMenu[i].NoItems = 5;
			for(j=0;j<SubMenu[i].NoItems;j++)
			switch(j) {
		       case 0:
                           strcpy(SubMenu[i].Item[j].text,"Semester");             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Vars    ");             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Dir     ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,"TBA     ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       default: ;
		 }

			break;
		case 3:
			SubMenu[i].NoItems = 5;
			for(j=0;j<SubMenu[i].NoItems;j++)
			switch(j) {
		       case 0:
                           strcpy(SubMenu[i].Item[j].text,"Shell   ");             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,"New Sem ");             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       default: ;
		 }

			break;
		case 4:
			SubMenu[i].NoItems = 5;
			for(j=0;j<SubMenu[i].NoItems;j++)
			switch(j) {
		       case 0:
                           strcpy(SubMenu[i].Item[j].text,"Info    ");             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Man Page");             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,"xxxxx   ");             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       default: ;
		 }

			break;

		default: ;
	   }
	   SubMenu[i].CurrentItem = 0;
	   SubMenu[i].BeginCol = Main_Menu.Item[i].BeginCol;
	   SubMenu[i].EndCol = Main_Menu.Item[i].BeginCol + MAIN_MENU_ITEM_WIDTH;
	   SubMenu[i].BeginRow = Main_Menu.BeginRow + 1;
	   SubMenu[i].EndRow = Main_Menu.BeginRow + SubMenu[i].NoItems;
	   for(j=0; j< SubMenu[i].NoItems;j++){
		SubMenu[i].Item[j].BeginRow =
		     SubMenu[i].BeginRow + j;
		SubMenu[i].Item[j].EndRow =
		      SubMenu[i].Item[j].BeginRow;
		SubMenu[i].Item[j].BeginCol =
		      SubMenu[i].BeginCol;
		SubMenu[i].Item[j].EndCol =
		       SubMenu[i].BeginCol + MAIN_MENU_ITEM_WIDTH;
		SubMenu[i].Item[j].ForeGroundColor = WHITE;
		SubMenu[i].Item[j].BackGroundColor = MAGENTA;
		SubMenu[i].Item[j].Wdw =  new constream;
		SubMenu[i].Item[j].Wdw->window( 
		       SubMenu[i].Item[j].BeginCol,
		       SubMenu[i].Item[j].BeginRow,
		       SubMenu[i].Item[j].EndCol,
		       SubMenu[i].Item[j].EndRow);
		SubMenu[i].Item[j].Wdw->rdbuf()->setcursortype(_NOCURSOR);
		}

	  }
     
// Set up and display the title line at top of screen

	Title_Line.window(1,1,80,1);
	Title_Line.clrscr();
	Title_Line << setattr((BLUE << 4) | YELLOW );
	Title_Line.rdbuf()->setcursortype(_NOCURSOR);
	Title_Line << "                                                                                   ";
        Title_Line << TITLE << " -- " << GetShell("VERSION") << " -- ";
	
	// Initialize the Main_Window

	Main_Window.window(1,3,80,24);
	Main_Window.clrscr();
	Main_Window << setattr((BLUE << 4) | WHITE);

	FirstScreen(openfile,&Main_Window);
	Status(&Status_Bar,&mystatus); // update status bar     
/////////////////////////////////////////////
//  MAIN LOOP 
/////////////////////////////////////////////
	while(1){                   /* Main Loop */
	  
		if(kbhit() !=0){    /* Wait for Keystroke */
		// clear any status bar message
			strcpy(mystatus.Message, "   <esc> = Menu Activate");
			Status(&Status_Bar,&mystatus); // update status bar     

		 
/*

  Low bytes of longkey are nonzero for ascii characters. For special
  keys the high byte has a code identifying the key      */

			longkey = _bios_keybrd(_KEYBRD_READ);
			key = longkey;
			if (key == 0) key = longkey >> 8;

			switch(CurrentFocus){
			    case MENUFOCUS:
				switch(key){

				case RIGHTARROW:
					Main_Menu.Item[Main_Menu.CurrentItem].state 
					 = NORMAL;
					Main_Menu.CurrentItem = 
					 (Main_Menu.CurrentItem + 1) % MAIN_MENU_ITEMS;
					Main_Menu.Item[Main_Menu.CurrentItem].state 
					  = SELECTED;
					DisplayMainMenu(&Main_Menu);
					break;

				case LEFTARROW:
					 Main_Menu.Item[Main_Menu.CurrentItem].state 
					   = NORMAL;
					 Main_Menu.CurrentItem = 
					(Main_Menu.CurrentItem 
					  + MAIN_MENU_ITEMS - 1) % MAIN_MENU_ITEMS;
					  Main_Menu.Item[Main_Menu.CurrentItem].state 
					  = SELECTED;
					 DisplayMainMenu(&Main_Menu);
					 break;

				case ESCAPE:
					CurrentFocus = WINDOWFOCUS;
					 Main_Menu.Item[Main_Menu.CurrentItem].state 
					   = NORMAL;
					DisplayMainMenu(&Main_Menu);
					Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);      
					break;
				case LINEFEED:
				case RETURN:
				case DOWNARROW:
					CurrentFocus = SUBMENUFOCUS;
					SubMenu[Main_Menu.CurrentItem].CurrentItem = 0;
					SubMenu[Main_Menu.CurrentItem].Item[0].state = SELECTED;
					DisplaySubMenu(Main_Menu.CurrentItem,SubMenu);
					break;
				case 'q':
					Main_Window.window(1,1,80,25);
					Main_Window.clrscr();
					Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
					fclose(openfile);
					return 0;
				default:
					printf("\a"); /* beep */
					break;
				}     // end switch
			break;      // end case ( Menu Focus )
			case SUBMENUFOCUS:
				switch(key){

				case DOWNARROW:
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					 = NORMAL;
					SubMenu[Main_Menu.CurrentItem].CurrentItem = 
					 (SubMenu[Main_Menu.CurrentItem].CurrentItem + 1) 
					  % SubMenu[Main_Menu.CurrentItem].NoItems;
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					  = SELECTED;
					DisplaySubMenu(Main_Menu.CurrentItem,SubMenu);
					break;

				case UPARROW:
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					 = NORMAL;
					SubMenu[Main_Menu.CurrentItem].CurrentItem = 
					 (SubMenu[Main_Menu.CurrentItem].CurrentItem + 
					   SubMenu[Main_Menu.CurrentItem].NoItems -1 ) 
					  % SubMenu[Main_Menu.CurrentItem].NoItems;
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					  = SELECTED;
					DisplaySubMenu(Main_Menu.CurrentItem,SubMenu);
					break;

				case ESCAPE:
					CurrentFocus = MENUFOCUS;
					 SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					  = NORMAL;

					DisplayMainMenu(&Main_Menu);
					/* need to redraw main window too */
					start = 
						LinePositions[lineno-Y+1];
					fsetpos(openfile,&start);
					DisplayMainWindow(&Main_Window,openfile);
					Main_Window.rdbuf()->gotoxy(X,Y);
					start=LinePositions[lineno];
					fsetpos(openfile,&start);
					break;
				case LINEFEED:
				case RETURN:
					CurrentFocus = WINDOWFOCUS;
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					 = NORMAL;
					 Main_Menu.Item[Main_Menu.CurrentItem].state=NORMAL;
					DisplayMainMenu(&Main_Menu);

					switch(Main_Menu.CurrentItem){
                                          case 0:  /* File Menu */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
						   case 4: /* QUIT PROGRAM */
							Main_Window.window(1,1,80,25);
							Main_Window.clrscr();
							Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
							fclose(openfile);
							return 0;

						   case 0: /* OPEN FILE */
							strcpy(DialogueData.Title,"           Open a File");
                                                        strcpy(DialogueData.msg1,"Enter Path to File.");
                                                        strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");
                                                        if(MYDialogue()==0){
								tempfilehold=openfile;
								 if((openfile = fopen(DialogueData.Input,"r"))==NULL){
								 openfile = tempfilehold;
								  myerror(&mystatus,"Can't Open ",DialogueData.Input);
								  Status(&Status_Bar,&mystatus);
								  DumpCurrentPage();
								 break;
								 }
							fclose(tempfilehold);  // succeeded
							 strcpy(mystatus.OpenFile,DialogueData.Input);
							 Status(&Status_Bar,&mystatus);
							FirstScreen(openfile,&Main_Window);
						       }
						       else {
							DumpCurrentPage();   /* redraw */
							}
							break;
                                                   case 1: /* Load */
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
							break;

						   case 2: /* SAVE AS */
							strcpy(DialogueData.Title,"           Save File");
                                                        strcpy(DialogueData.msg1,"Enter  Path to Save As");
                                                        strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

							if(MYDialogue()==0){
									    // Attempt to create handle for new file
								 if((handle = creatnew(DialogueData.Input,0))==-1){
								  if(errno == EEXIST){                       // File exists. What to do?
									strcpy(buffer,DialogueData.Input);
									strcpy(DialogueData.Title,"           File Exists!");
									strcpy(DialogueData.msg1,"Overwrite [o] or Append [a] ");
									strcpy(DialogueData.msg2,"<esc> to Cancel");
                                                                        if(MYDialogue()==0){
										switch(DialogueData.Input[0]){  // depending on user input

										  case 'o':

										       if(( tempfilehold = fopen(buffer,"w"))==NULL){
											myerror(&mystatus,"Unable to open: ",buffer);
											Status(&Status_Bar,&mystatus);
											DumpCurrentPage();
											break;
											}
											break;
										  case 'a':
										     
										       if( (tempfilehold = fopen(buffer,"a"))==NULL){
											myerror(&mystatus,"Unable to open: ",buffer);
											Status(&Status_Bar,&mystatus);
											DumpCurrentPage();
											break;
											}
											break;
										  default: 
											myerror(&mystatus,"Unrecognized response: ",DialogueData.Input);
											Status(&Status_Bar,&mystatus);
											DumpCurrentPage();
											break;
										 }      // end switch
										       // Now copy the file
									fgetpos(openfile,&start);
									rewind(openfile);
									while((c=fgetc(openfile))!=EOF)
									fputc(c,tempfilehold);
									fclose(tempfilehold);
									fsetpos(openfile,&start);
									DumpCurrentPage();       // redraw screen
									break;
								  }
								  else {                     // Bailed out of File Exists Dialogue
									DumpCurrentPage();
									break;
								  }
								  }

								  // Did attempt to create new file handle generate some other error
								  // besides file exists? If so, complain and return to top level.

								  if(errno == ENOENT )
								  myerror(&mystatus,"Invalid Path: ",DialogueData.Input);
								  if(errno == EACCES)
								  myerror(&mystatus,"Permission Denied: ",DialogueData.Input);
								  if(errno == EMFILE)
								  myerror(&mystatus,"Too many open files", " ");
								  Status(&Status_Bar,&mystatus);
								  DumpCurrentPage();
								 break;
								 }

						   // OK, we have a handle
						   // for a new file
								      // open stream to it
						       tempfilehold = fdopen(handle,"w");

						       // save position in current file, copy it
						       // and restore position in current file

						       fgetpos(openfile,&start);
						       rewind(openfile);
						       while((c=fgetc(openfile))!=EOF)
							fputc(c,tempfilehold);
						       fclose(tempfilehold);
						       fsetpos(openfile,&start);
						       }
							DumpCurrentPage();   /* redraw */
							break;
						case 3:    /* Print the open file */
							/* Save and restore file position */
							strcpy(DialogueData.Title,"       Printer Setup");
							strcpy(DialogueData.msg1,"Enter n for normal, l for landscape.");
							strcpy(DialogueData.msg2,"<Ret> to apply, <esc> to cancel");
                                                        if(MYDialogue()==0){

							fgetpos(openfile,&start);
							if(DialogueData.Input[0]=='l')
								PrintOut(openfile,1);
							else PrintOut(openfile,0);
							fsetpos(openfile,&start);
							}
							DumpCurrentPage();
							break;

						default:       // default case of file submenu
							break;
						}
						break;
                                          case 1:  /* View MENU */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                                  case 0:  /* Schedule */
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"w");
                                                        strcpy(mystatus.OpenFile,TempFilePath);
                                                        numtokens = 2;
                                                        strcpy(Tokens[1]->text,Semester);
                                                        schedule(NULL,openfile,NULL,NULL);
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"r");
                                                        FirstScreen(openfile,&Main_Window);
                                                        break;

                                                  case 1:    /* Audit */
                                                        strcpy(DialogueData.Title,"           Staff Audit");
                                                        strcpy(DialogueData.msg1,"Use AY or Semester Loads[a|s]?");
                                                        strcpy(DialogueData.msg2,"AY uses half the AY load each sem.");
                                                        if(MYDialogue()==0){
                                                                  if(strcmp(DialogueData.Input,"a")!= 0 &&
                                                                     strcmp(DialogueData.Input,"s")!=0){
                                                                  myerror(&mystatus,"Unrecognized response: ",DialogueData.Input);
								  Status(&Status_Bar,&mystatus);
								  DumpCurrentPage();
                                                                  break;
								 }
                                                                 if(strcmp(DialogueData.Input,"a")==0)
                                                                        strcpy(buffer,"-at");
                                                                 else strcpy(buffer,"-t"); // buffer now has audit flag
						       }
						       else {
							DumpCurrentPage();   /* redraw */
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

                                                        take(NULL,openfile,devnull,devnull);
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"r");
                                                        FirstScreen(openfile,&Main_Window);

                                                        }
                                                        else {
                                                         DumpCurrentPage();
                                                       }
							break;
                                                     
                                                  case 2: strcpy(DialogueData.Title,"              Room Chart");
                                                          strcpy(DialogueData.msg1,"Enter date of start of time block.");
                                                          strcpy(DialogueData.msg2,"e.g. 9/11/96");
                                                          if(MYDialogue() == 0)
                                                          set("START_DATE",DialogueData.Input,3,NULL);
                                                          
                                                          else{
                                             
                                                          DumpCurrentPage();
                                                          break;
                                                          }
                                                          strcpy(DialogueData.msg1,"Enter time of start of time block.");
                                                          strcpy(DialogueData.msg2,"e.g. 8:30, or <esc> to bail out");
                                                          if(MYDialogue() == 0)
                                                          set("START_TIME",DialogueData.Input,3,NULL);
                                                          
                                                          else{
                                                          set("START_DATE","",2,NULL);
                                                          DumpCurrentPage();
                                                          break;
                                                          }
                                                          strcpy(DialogueData.msg1,"Enter date of end of time block.");
                                                          strcpy(DialogueData.msg2,"e.g. 9/11/97, or <esc> to bail out");
                                                          if(MYDialogue() == 0)
                                                          set("END_DATE",DialogueData.Input,3,NULL);
                                                          
                                                          else{
                                                          set("START_DATE","",2,NULL);
                                                          set("START_TIME","",2,NULL);

                                                          DumpCurrentPage();
                                                          break;
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
                                                          break;
                                                          }
                                                          fclose(openfile);
                                                          numtokens = 2;
                                                          strcpy(Tokens[1]->text,"scripts\\roomchar");
                                                          take(NULL,devnull,devnull,devnull);
                                                          openfile = fopen(TempFilePath,"r");
                                                          strcpy(mystatus.OpenFile,TempFilePath);
                                                          FirstScreen(openfile,&Main_Window);
							break;
                                                  
                                                  case 3: /* People */
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"w");
                                                        numtokens = 1;
                                                        dpeople(NULL,openfile,openfile,NULL);
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"r");
                                                        FirstScreen(openfile,&Main_Window);
                                                        strcpy(mystatus.OpenFile,TempFilePath);
                                                        strcpy(DialogueData.Title,"          View Personnel Data");
                                                        strcpy(DialogueData.msg1,"Enter a number, all, or a range");
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
                                                                          break;
                                                                      }
                                                                      k = atoi(ptr);
                                                                      if(k < j){
                                                                          myerror(&mystatus,"Bad Input:", DialogueData.Input);
                                                                          fclose(openfile);
                                                                          openfile = fopen(TempFilePath,"r");
                                                                          DumpCurrentPage();
                                                                          break;
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
                                                                fclose(openfile);
                                                                openfile = fopen(TempFilePath,"r");

                                                        }
                                                        else{
                                                          DumpCurrentPage();
                                                          break;
                                                        }
                                                        FirstScreen(openfile,&Main_Window);
                                                        break;
                                                   case 4:
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"w");
                                                        strcpy(mystatus.OpenFile,TempFilePath);
                                                        numtokens = 1;
                                                        dfinals(NULL,openfile,NULL,NULL);
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"r");
                                                        FirstScreen(openfile,&Main_Window);
                                                        break;
                                                   default:
                                                        break;
                                                  }
                                                  break;
                                          case 2: /* Settings Menu */

					  switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){ 
                                                case 0:   /* Set Semester */
                                                        strcpy(DialogueData.Title,"           Set Semester");
                                                        strcpy(DialogueData.msg1,"Enter Semester (e.g f97). ");
                                                        strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

                                                        if(MYDialogue()==0){
                                                              strcpy(Semester,DialogueData.Input);
						       }
							DumpCurrentPage();   /* redraw */
							break;
                                                case 1: /* View and Set Shell Variables */
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"w");
                                                        set("","",1,openfile);
                                                        fclose(openfile);
                                                        openfile = fopen(TempFilePath,"r");
                                                        DumpCurrentPage();
                                                        strcpy(mystatus.OpenFile,TempFilePath);
                                                        strcpy(DialogueData.Title,"          Set Shell Variable");
                                                        strcpy(DialogueData.msg1,"Enter Name of Variable to set.");
                                                        strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

                                                        if(MYDialogue()==0){
                                                                strcpy(buffer,DialogueData.Input);

                                                        }
                                                        else {
                                                         DumpCurrentPage();
                                                         break;
                                                        }
                                                        strcpy(DialogueData.msg1,"Enter value of variable to set.");
                                                        strcpy(DialogueData.msg2,"<enter> to apply, <esc> to cancel");

                                                        if(MYDialogue()==0){
                                                                set(buffer,DialogueData.Input,3,NULL);
                                         
                                                               fclose(openfile);
                                                               openfile = fopen(TempFilePath,"w");
                                                               set("","",1,openfile);
                                                               fclose(openfile);
                                                               openfile = fopen(TempFilePath,"r");
                                                    

                                                        }
                                                        DumpCurrentPage();
                                                      
							break;
                                                case 2:   /* Set Working Directory */
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
                                                        
							break;

                                                case 3:  /* Set Missing Data Symbol */
                                                        strcpy(DialogueData.Title," Set Missing Data Symbol");
                                                        strcpy(DialogueData.msg1,"string to stand for missing Data?");
                                                        strcpy(DialogueData.msg2,"For example, TBA or NA");
                                                        if(MYDialogue()==0){
								strcpy(MissingData,DialogueData.Input);
                                                                set("MISSINGDATA",DialogueData.Input,3,NULL);
								
                                                        }
                                                        DumpCurrentPage();
							break;
					  
                                                case 4:  
                                                        break;
                                                default:
                                                        break;
                                            }
                                            break;
                                          case 3: 
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                                   case 0:   /* Command Shell */
                                                   set("ECHO","ON",3,NULL);
                                                   system("cls");
                                                   Main_Menu.Wdw->rdbuf()->setcursortype(_SOLIDCURSOR);

                                                   Dialogue(stdin,stdout,stderr,stdout,"Interactive Command Shell(1.0)",
                                                   NULL,&shell);
                                                   set("ECHO","OFF",3,NULL);

                                                   system("cls");
                                                   Title_Line.clrscr();

                                                   Title_Line << TITLE << " -- " << GetShell("VERSION") << "--";
                                                   DisplayMainMenu(&Main_Menu);
                                                   Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
                                                   DumpCurrentPage();

							break;
                                                   case 1:   /* Run the New Semester Program */
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
                                                         DumpCurrentPage();

							break;
                                                   case 2:  
							break;
                                                   case 3:  
							break;
                                                  case 4:
                                                        break;
                                                  default:
                                                        break;
					  }
					  break;
                                          case 4:  /* Help */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                                   case 0:
                                                        fclose(openfile);
                                                        openfile = fopen("doc\\info.txt","r");
                                                        strcpy(mystatus.OpenFile,"doc\\info.txt");
                                                        FirstScreen(openfile,&Main_Window);
						     break;

                                                   case 1:
                                                        fclose(openfile);
                                                        openfile = fopen("doc\\schedule.txt","r");
                                                        strcpy(mystatus.OpenFile,"doc\\schedule.txt");
                                                        FirstScreen(openfile,&Main_Window);

						     break;

                                                   case 2: 
						     break;

                                                   case 3:  
						     break;


                                                   case 4:  
						     break;

						   default:
						     break;
						  }
					   break;
					   default:
						break;
					 }

					
					break;
				default:
					printf("\a"); /* beep */
					break;
				}     // end switch
			
			break;      // end case ( SubMenu Focus )

			case WINDOWFOCUS:        /* Main Window Focus */
				switch(key){

				case ESCAPE:
					Main_Menu.Item[Main_Menu.CurrentItem].state 
					 = SELECTED;
					CurrentFocus = MENUFOCUS;
					Main_Window.rdbuf()->setcursortype(_NOCURSOR);
					DisplayMainMenu(&Main_Menu);
					break;

				case ' ':         // dump next page
					lineno +=  DumpToScreen(openfile,&Main_Window)-1;
					start = LinePositions[lineno];
					fsetpos(openfile,&start);
			// position cursor at  bottom of screen
					Main_Window.rdbuf()->gotoxy(1,SCREENLINES);
					Y=SCREENLINES;
					break;
				case UPARROW:
					if(Y==1){  /* cursor at top */
						if(lineno == 0) 
							printf("\a");
						else {
							start = LinePositions[--lineno];
							fsetpos(openfile,&start);
							DumpToScreen(openfile,&Main_Window);
							Main_Window.rdbuf()->gotoxy(X,1);
						      }
					}
					else {
						Main_Window.rdbuf()->gotoxy(X,--Y);
						--lineno;
						start = LinePositions[lineno];
						fsetpos(openfile,&start);
					}
					break;
				case DOWNARROW:
					if(Y==SCREENLINES){  /* cursor at bottom */
						if(lineno == TotalLines-1) 
							printf("\a");
						else {
							start = LinePositions[++lineno];
							fsetpos(openfile,&start);
							ReverseDumpToScreen(openfile,&Main_Window,lineno,LinePositions);
							Main_Window.rdbuf()->gotoxy(X,SCREENLINES);
						      }
					}
					else {
						if(lineno== TotalLines-1) {
							printf("\a");
							break;
						}
						Main_Window.rdbuf()->gotoxy(X,++Y);
						++lineno;
						start = LinePositions[lineno];
						fsetpos(openfile,&start);
					}
					break;
				case RETURN:
					// put cursor at bottom of page and
					// do a down arrow
					if( lineno + SCREENLINES -Y < TotalLines -1)
					{
					lineno += SCREENLINES-Y + 1;
					start = LinePositions[lineno];
					fsetpos(openfile,&start);
					ReverseDumpToScreen(openfile,&Main_Window,lineno,LinePositions);
					Y = SCREENLINES;
					Main_Window.rdbuf()->gotoxy(X,Y);
					
					}
					else printf("\a");
					break;
				case CONTROL_HOME:
						 start = LinePositions[0];
						 lineno=0;
						 X=Y=1;
						 fsetpos(openfile,&start);
						 DumpToScreen(openfile,&Main_Window);
						 Main_Window.rdbuf()->gotoxy(X,Y);
						 break;
				case CONTROL_END:
						 start = LinePositions[TotalLines-SCREENLINES];
						 lineno=TotalLines-1;
						 X=1;
						 Y=SCREENLINES;
						 fsetpos(openfile,&start);
						 DumpToScreen(openfile,&Main_Window);
						 Main_Window.rdbuf()->gotoxy(X,Y);
						 start = LinePositions[TotalLines-1];
						 fsetpos(openfile,&start);
						 break;
				case PAGEDOWN:
						 if((lineno + SCREENLINES+1-Y) > TotalLines-1)
							printf("\a");
						 else {
							start = LinePositions[lineno+SCREENLINES+1-Y];
							fsetpos(openfile,&start);
							DumpToScreen(openfile,&Main_Window);
							if((lineno+SCREENLINES) <= TotalLines-1){
								lineno += SCREENLINES;
								start = LinePositions[lineno];
								fsetpos(openfile,&start);
								Main_Window.rdbuf()->gotoxy(X,Y);
							}
							else {
								Y += (TotalLines-lineno-SCREENLINES - 1);
								lineno = TotalLines-1;
								start = LinePositions[lineno];
								fsetpos(openfile,&start);
								Main_Window.rdbuf()->gotoxy(X,Y);
							      }
						 }
						 break;
				case PAGEUP:
						 if(( lineno - SCREENLINES + 1 - Y ) <= 0)
							if((lineno-SCREENLINES)<0)
							  printf("\a");
							else {
							  start = LinePositions[0];
							  fsetpos(openfile,&start);
							  DumpToScreen(openfile,&Main_Window);
							  lineno = Y-1;
							  Main_Window.rdbuf()->gotoxy(X,Y);
							  start = LinePositions[lineno];
							  fsetpos(openfile,&start);
							}
						 else {
							start = LinePositions[lineno-SCREENLINES+1-Y];
							fsetpos(openfile,&start);
							DumpToScreen(openfile,&Main_Window);
							lineno -= SCREENLINES;
							start = LinePositions[lineno];
							fsetpos(openfile,&start);
							Main_Window.rdbuf()->gotoxy(X,Y);
						 }
						 break;

				case RIGHTARROW:
						 if(X < LINELENGTH)Main_Window.rdbuf()->gotoxy(++X,Y);
						 else { X=1; /* code for down arrow here */
							if(Y==SCREENLINES){  /* cursor at bottom */
								if(lineno == TotalLines-1) 
								     printf("\a");
								else {
									start = LinePositions[++lineno];
									fsetpos(openfile,&start);
									ReverseDumpToScreen(openfile,&Main_Window,lineno,LinePositions);
									Main_Window.rdbuf()->gotoxy(X,SCREENLINES);
								}
							}
							else {
								if(lineno== TotalLines-1) {
								  printf("\a");
								  break;
								 }

								Main_Window.rdbuf()->gotoxy(X,++Y);
								++lineno;
								 start = LinePositions[lineno];
								  fsetpos(openfile,&start);
							  }
						   }
						   break;
				case  LEFTARROW:
					if(X > 1) Main_Window.rdbuf()->gotoxy(--X,Y);
					else {X=LINELENGTH;
						/* code for uparrow here */
						if(Y==1){  /* cursor at top */
						    if(lineno == 0) 
								printf("\a");
							else {
							    start = LinePositions[--lineno];
							   fsetpos(openfile,&start);
							  DumpToScreen(openfile,&Main_Window);
							  Main_Window.rdbuf()->gotoxy(X,1);
							 }
					 }
					 else {
						       Main_Window.rdbuf()->gotoxy(X,--Y);
						     --lineno;
						     start = LinePositions[lineno];
						    fsetpos(openfile,&start);
					 }
				      }
				      break;

				default:
					printf("\a");  /* beep */
					break;
				}     // end switch
			break;    // end case ( Window focus )
			default: ;
		    }   // end focus switch
		  Status(&Status_Bar,&mystatus); // update status bar    
		}   // end if: there is a keyboard hit       
	 
	}          // end main loop
}              // end main

// Called when user hits escape
int DisplayMainMenu(struct MENU *Main_Menu)
{
	int i;
	int temp ;
	constream PaintOver;

	PaintOver << setattr((BLUE << 4) | BLUE);
	PaintOver.rdbuf()->setcursortype(_NOCURSOR);


	for(i=0;i<MAIN_MENU_ITEMS;i++){
	 if(Main_Menu->Item[i].state == SELECTED){
	   Main_Menu->Item[i].ForeGroundColor = MAGENTA;
		 /* reverse video */
		Main_Menu->Item[i].BackGroundColor = WHITE;
	  }
	  else {
	   Main_Menu->Item[i].ForeGroundColor = WHITE;
		 /* normal */
		Main_Menu->Item[i].BackGroundColor = MAGENTA;
	  }
	  Main_Menu->Item[i].Wdw->clrscr();
	  Main_Menu->Item[i].Wdw->rdbuf()->textbackground(
		Main_Menu->Item[i].BackGroundColor);
	  Main_Menu->Item[i].Wdw->rdbuf()->textcolor(
		Main_Menu->Item[i].ForeGroundColor);
	  *(Main_Menu->Item[i].Wdw) << 
		setw(MAIN_MENU_ITEM_WIDTH) << Main_Menu->Item[i].text;
	 PaintOver.window(MAIN_MENU_ITEMS*MAIN_MENU_ITEM_WIDTH+1,
		2,MAIN_MENU_ITEMS*MAIN_MENU_ITEM_WIDTH+1,2);
	 PaintOver.clrscr();
	 if(Main_Menu->Item[i].state == SELECTED){
	  temp = Main_Menu->Item[i].ForeGroundColor;
	  Main_Menu->Item[i].ForeGroundColor =           /* reverse video */
		Main_Menu->Item[i].BackGroundColor;
	  Main_Menu->Item[i].BackGroundColor = temp;
	  }

	 }
	 return 0;
}

int DisplaySubMenu(int N,SUBMENU *SubMenu)
{
	int i;
	int temp ;
	constream PaintOver;
	PaintOver << setattr((BLUE << 4) | BLUE);
	PaintOver.rdbuf()->setcursortype(_NOCURSOR);


	for(i=0;i<(SubMenu+N)->NoItems;i++){
	 if((SubMenu+N)->Item[i].state == SELECTED){
	   (SubMenu+N)->Item[i].ForeGroundColor = MAGENTA;
		 /* reverse video */
		(SubMenu+N)->Item[i].BackGroundColor = WHITE;
	  }
	  else {
	   (SubMenu+N)->Item[i].ForeGroundColor = WHITE;
		 /* normal */
		(SubMenu+N)->Item[i].BackGroundColor = MAGENTA;
	  }
	  (SubMenu+N)->Item[i].Wdw->clrscr();
	  (SubMenu+N)->Item[i].Wdw->rdbuf()->textbackground(
		(SubMenu+N)->Item[i].BackGroundColor);
	  (SubMenu+N)->Item[i].Wdw->rdbuf()->textcolor(
		(SubMenu+N)->Item[i].ForeGroundColor);
	*( (SubMenu+N)->Item[i].Wdw) << (SubMenu+N)->Item[i].text;
	  PaintOver.window((SubMenu+N)->Item[i].EndCol,
		(SubMenu+N)->Item[i].BeginRow,
		(SubMenu+N)->Item[i].EndCol,
		(SubMenu+N)->Item[i].BeginRow);
	  PaintOver.clrscr();
	 if((SubMenu+N)->Item[i].state == SELECTED){
	  temp = (SubMenu+N)->Item[i].ForeGroundColor;
	  (SubMenu+N)->Item[i].ForeGroundColor =           /* reverse video */
		(SubMenu+N)->Item[i].BackGroundColor;
	  (SubMenu+N)->Item[i].BackGroundColor = temp;
	  }

	 }
	 return 0;
}

int DisplayMainWindow(constream *display,FILE *openfile)
{

	DumpToScreen(openfile,display);
	return 0;
}


/* Dump a screenfull of data from file pointed to by fptr. Leave the 
   stream position at the same point as when called. Word wrap. 
   Returns number of lines dumped.
*/

int DumpToScreen(FILE *fptr, constream *output){

	
	fpos_t start;
	char *CharPtr;
	int lineno=0;
	int n;

	 output->clrscr();

	/* Record the starting position of stream */
	fgetpos(fptr,&start);

	while(lineno < SCREENLINES - 1){
	/* Get the next chunk of line into buffer */

	   CharPtr = myfgets(LineBuf,LINELENGTH,fptr);
	   n = strlen(LineBuf);

	   if(CharPtr != NULL){
	/* See whether the buffer ends with a newline and act accordingly */
		if(*(CharPtr+n-1)=='\n')
			*output <<= LineBuf;
		else
			( *output <<= LineBuf ) << '\n';
	     lineno++;
	     }
	     else {
		    /* at EOF */
		lineno--;
		break;
	     }
	}

	if(CharPtr != NULL){
	/* handle last line differently */
	CharPtr = myfgets(LineBuf,LINELENGTH,fptr);
	n = strlen(LineBuf);
	if(CharPtr != NULL){
		if(*(CharPtr+n-1)=='\n'){
			*(CharPtr+n-1)='\r';
			*output <<= LineBuf;
		}
		else
			(*output <<= LineBuf) << '\r';
	}
	else lineno--;
	}
	/* restore file position */
	 fsetpos(fptr,&start);
	 return lineno + 1;
}

int ReverseDumpToScreen(FILE *fptr, constream *output, int lineno,
	fpos_t *LinePositions){

      
	fpos_t start;
	fpos_t temp;
	char *CharPtr;
	int Y=SCREENLINES;
	int n;

	 output->clrscr();

	/* Record the starting position of stream */
	fgetpos(fptr,&start);

	while(Y > 0){
	/* Get the next chunk or line into buffer */
	   temp = *(LinePositions + lineno + (Y - SCREENLINES));
	   fsetpos(fptr,&temp);
	   output->rdbuf()->gotoxy(1,Y--);
	   CharPtr = myfgets(LineBuf,LINELENGTH,fptr);
	   n = strlen(LineBuf);

	/* See whether the buffer ends with a newline and act accordingly */
		if(*(CharPtr+n-1)=='\n')
			*(CharPtr+n-1)='\0';
		(*output <<= LineBuf) << '\r';
	}

	/* restore file position */
	 fsetpos(fptr,&start);
	 return SCREENLINES;
}

/* Same as fgets, except it does not read beyond EOF */

char *myfgets(char *s, int n, FILE *fp)
{
      
	int i = 0;
	char c;
	while((i<n-1)&&(feof(fp)==0)){
		c = fgetc(fp);
		*(s+i)=c;
		i++;
		if(c=='\n') break;
	}
	*(s+i) = '\0';
	if((feof(fp)!=0)||(ferror(fp)!=0))return (char *)NULL;
	return s;
}

void myerror(struct StatusInfo *status, char * string1, char *string2) {

	strcpy(status->Message, string1);
	strcat(status->Message,string2);
	return;
}

/* Updates the status bar at the bottom of the screen */

void Status(constream *win, struct StatusInfo *mystatus)
{
	win->clrscr();
        sprintf(buffer,"%d:%d %s SEM: %s ",lineno+1,X,mystatus->OpenFile,
                Semester);
        if(GetShell("LOADED")!=NULL){
        strcat(buffer,"[");
        strcat(buffer,GetShell("LOADED"));
        strcat(buffer,"] ");
        }
        strcat(buffer,"WD: ");
        strcat(buffer, WorkingDirectory);
        strcat(buffer, mystatus->Message);

        /* Make sure the buffered string will fit in 79 columns */
        *(buffer + 79) = '\0';
        *win << buffer;
	Main_Window.rdbuf()->gotoxy(X,Y);
	
	return;
}

/* Dumps the very first screen-full of a newly opened file */

void FirstScreen(FILE *openfile, constream *Main_Window)
{

	fpos_t start;

        /* Make sure the cursor is turned on */
        Main_Window->rdbuf()->setcursortype(_NORMALCURSOR);
      /* read through the file and fill the line position array */
	lineno = 0;

	fgetpos(openfile,&start);           // first line
	LinePositions[0]=start;
	while(myfgets(LineBuf,LINELENGTH,openfile) != NULL){
	 fgetpos(openfile,&start);   
	 LinePositions[++lineno] = start;
	 }

	TotalLines = lineno;   /* number of logical lines in file */
	// Dump the first screenfull

	rewind(openfile);
	lineno =  DumpToScreen(openfile,Main_Window) - 1;
	start = LinePositions[lineno];
	fsetpos(openfile,&start);
       // position cursor at  bottom of screen
	Main_Window->rdbuf()->gotoxy(1,lineno+1);

	Y=lineno+1;
	X=1;
	return;
}

/* It is the calling programs responsibility to paint over the Dialogue Box */
int MYDialogue()
{
       
	char *inputptr;
	int longkey;
	char key;
	int xx,promptlen;

	xx =  promptlen = strlen(DialogueData.Prompt);
	inputptr = DialogueData.Input;

	DialogueBox.clrscr();
	DialogueBox << DialogueData.Title << endl;
	DialogueBox << DialogueData.Prompt << endl;
	DialogueBox << endl;
	DialogueBox << DialogueData.msg1 << endl << endl;
	DialogueBox << DialogueData.msg2 << endl;

	DialogueBox.rdbuf()->gotoxy(++xx,2);

	// get user input
	while(1) {
	if(kbhit() != 0){
	longkey = _bios_keybrd(_KEYBRD_READ);
	key = longkey;
	if(key == 0) key =  longkey >> 8;
	switch(key){
		case ESCAPE:
			Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
			return 1;
		case RETURN:
			*(inputptr) = '\0';
			Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
			return 0;
		case BACKSPACE:
			if(xx > promptlen + 1) {
				DialogueBox.rdbuf()->gotoxy(--xx,2);
				DialogueBox << " ";
				DialogueBox.rdbuf()->gotoxy(xx,2);
				*(inputptr--)='\0';
				}
				break;
		default:
			*(inputptr++)=key;
			DialogueBox << key;
			 DialogueBox.rdbuf()->gotoxy(++xx,2);
			 break;
	 }
	 }
	 }
       
}

/* Redraw the current screen: Leave cursor in the original place, leave
file position in the same place */

void DumpCurrentPage()
{

	fpos_t start;

	start = LinePositions[lineno - Y + 1];
	fsetpos(openfile,&start);
	DumpToScreen(openfile,&Main_Window);
	start = LinePositions[lineno];
	fsetpos(openfile,&start);
	Main_Window.rdbuf()->gotoxy(X,Y);
	return;
}


ostream& operator <<= (ostream &os, char *InputString) {
	char curr;
	int count =0;
	char *ptr;
	ptr = InputString;

	buffer[0]='\0';
	while((curr = *ptr++)!= '\0'){
		if(curr == '\t')
			strcat(buffer,TAB);
		else {
			count = strlen(buffer);
			*(buffer + count) = curr;
			*(buffer + count + 1) ='\0';
		}
	}
	 os << buffer;
	 return os;
}

int PrintOut(FILE *tempfile, int setup)
 {

       if(setup == 1) // landscape
    /* Initialize Printer for landscape and CR translation*/
        fprintf(stdprn,GetShell("LANDSCAPE"));
       else fprintf(stdprn,GetShell("NORMALPRINTER"));
	   // Send tempfile to the printer
	rewind(tempfile);
	while(fgets(buffer,BUFSIZ,tempfile)!=NULL)
		fprintf(stdprn,buffer);

       // restore default printer settings
        fprintf(stdprn,GetShell("NORMALPRINTER"));
	return 0;
}
