/* gui.cc: Linux/ncurses port of user interface for noontime running
	program   */

/* Color capabilities only work at a console. (I guess this is because
    the library uses hardware to change colors when term = linux. ) If
    term = linux but you are not at the console (e.g, dialed up from
    another linux box,) then there is a terrible gotcha: the library 
    cannot do standout mode, because if term = linux it assumes you're
    at a console and tries to do standout using the hardware instead of
    terminal escape sequences. Workaround: set term = vt100 first. */
 
#include <errno.h>
#include "team.h"
#include <iostream.h>
#include <fstream.h>
#include <ncurses/curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>


#define UPARROWSYMBOL  "(UP)"  
#define DOWNARROWSYMBOL "(DWN)" 
#define MAIN_MENU_ITEMS 5
#define MAX_ITEMS 6
#define MAIN_MENU_ITEM_WIDTH 8
#define ESCAPE '\x1B'
#define BACKSPACE ''

#define BLANKLINE "\
                                                                                \
"
#define MAXLINES 5000
#define LINELENGTH  256      // Longest possible line


/* These files are assumed to be in the working directory */
#define INFOFILE "info.txt"
#define AIMSFILE "aims.txt"
#define RULESFILE "rules.txt"
#define MANFILE "man.txt"
#define SOURCEFILE "source.txt"
#define ERRLOG "errlog"

/* The FOO in the following is thrown in so that there is no collision
   with case newline = ascii 10 in a switch statement. Boy, is that ever
   bad programming practise. */

enum SpecialKeys {UPARROW,RIGHTARROW,DOWNARROW,LEFTARROW,PAGEUP,PAGEDOWN,
	END,HOME,CONTROL_PAGEUP,CONTROL_PAGEDOWN,FOO,CONTROL_HOME,
	CONTROL_END};
enum MenuItemState { NORMAL, SELECTED, DIMMED };
enum Focus { MENUFOCUS, WINDOWFOCUS , SUBMENUFOCUS};

int GetKey(int);
int DisplayMainMenu();
void cursoroff();
void cursoron();

struct StatusInfo {   // Data used on status bar 
char OpenFile[80];
char SortType[20];
char SortDirection[9];
char Message[80];
};
struct StatusInfo mystatus;

 struct MenuItem{
WINDOW *Wdw;
int state;
char text[20];
int Rows;
int Cols;
int BeginRow;
int BeginCol;
} ;

struct SUBMENU {
int NoItems;
int CurrentItem;
WINDOW *Wdw;
struct MenuItem Item[MAX_ITEMS];
int Rows;
int Cols;
int BeginRow;
int BeginCol;
} ;

struct MENU {
  int CurrentItem;
  WINDOW *Wdw;
  struct MenuItem Item[MAIN_MENU_ITEMS];
  int Rows;
  int Cols;
  int BeginRow;
  int BeginCol;
        };

struct DialogueDataStruct {
char Title[40];
char Prompt[10];
char Input[40];
char msg1[40];
char msg2[45];
}DialogueData;

struct SUBMENU SubMenu[MAIN_MENU_ITEMS];
struct MENU  Main_Menu;

int DisplaySubMenu(int ,SUBMENU *);

int die();  /* routine to handle control C */
	       /* does nothing in crmode */
int Status();  // Updates the status bar at bottom of screen
int FirstScreen(FILE *);
int DumpToScreen(FILE *);
int ReverseDumpToScreen(FILE *,int, fpos_t *);
void myerror(struct StatusInfo *, char * , char *);
int Dialogue();

extern  int scorall(fstream *);
extern int reload();
extern int RunnerReport(char *, fstream *);
extern int Superlatives(fstream *,char *);
extern int PrintOut(FILE *,int);

WINDOW *Main_Window,*Status_Bar,*DialogueBox;
extern char TempFile[];
int TotalLines;  // Number of logical lines in file
int lineno;
int Cursor = 1;
extern int direction;
int depth = 10;  // default depth for bests and worsts display.
extern int SortFlag;
extern int number_teams;
extern Team *teams[];
char LineBuf[LINELENGTH+1];
int X=1,Y=1; /* cursor positions */
fpos_t LinePositions[MAXLINES]; /* offsets of each logical line in file */
                               /* Logical lines count parts of lines which
                                  are split off by word wrap   */

FILE *openfile;   /* The current file being displayed in Main Window */

extern char TeamsFile[];
extern char RosterFile[];
extern char ResultsFile[];

int gui()
{
	WINDOW *Title_Line;
	char Title[] = "                 Noontime Running League Scoring Program";
	int key;
	int i,j,temp;
	int CurrentFocus = WINDOWFOCUS;
	fpos_t start;
	FILE *tempfilehold;
	int handle;
	char buffer[256];
	char c;
	fstream *tempwrite;

	openfile = fopen(TempFile,"r");  
	if(!openfile)return 1;

/* load customized keyboard mapping for this program. See the 
	GetKeys routine below for documentation  */

//	system("loadkeys /usr/lib/kbd/keytables/score.map 2> /dev/null");

	/* initialize screen */
	initscr();
	start_color();  
	keypad(curscr,TRUE);
        cbreak();  /* You probably want this. */
	signal(SIGINT, (__sighandler_t ) die); 

	noecho();  /* Don't echo characters typed at keyboard */

/* Initialize the Title Line at the top of the screen */

	Title_Line = subwin(stdscr,1,COLS,0,0);
	if(has_colors()){
		init_pair(1,COLOR_YELLOW,COLOR_BLUE);
		wattron(Title_Line,COLOR_PAIR(1)|A_BOLD);
	}
	else
		wattron(Title_Line,A_BOLD);
	werase(Title_Line);
	wmove(Title_Line,0,0);
	waddstr(Title_Line,Title);	
	touchwin(Title_Line);
	wrefresh(Title_Line);

 // Initialize the status bar at bottom of screen

        strcpy(mystatus.OpenFile,TempFile);
        sprintf(mystatus.SortType,"%s", "Last Name ");
        sprintf(mystatus.SortDirection,"%s",UPARROWSYMBOL);
        strcpy(mystatus.Message, "   <esc> = Menu Activate");
	Status_Bar = subwin(stdscr,1,COLS,LINES-1,0);
	if(has_colors()){
		init_pair(3,COLOR_WHITE,COLOR_RED);
		wattron(Status_Bar,COLOR_PAIR(3));
	}
	else {
		wattron(Status_Bar,A_BOLD);
		wstandout(Status_Bar);
	}
	werase(Status_Bar);
	leaveok(Status_Bar,0);
	Status();

/* Set up Dialogue Box */

	DialogueBox =subwin(stdscr,10,45,13,23);
	if(has_colors()){
		init_pair(5,COLOR_YELLOW,COLOR_RED);
		wattron(DialogueBox,COLOR_PAIR(5)|A_BOLD);
	}
	else {
		wattron(DialogueBox,A_BOLD);
		wstandout(DialogueBox);
	}
        strcpy(DialogueData.msg1," ");
        strcpy(DialogueData.msg2,"<Enter> to apply, <esc> to dismiss");
        strcpy(DialogueData.Title, "Dialogue Box");
        strcpy(DialogueData.Prompt,"Score>");

/* Initialize the Main Window */

	Main_Window = newwin(LINES-3,COLS,2,0);
	init_pair(2,COLOR_WHITE,COLOR_BLUE);
	wattron(Main_Window,COLOR_PAIR(2));
	touchwin(Main_Window);
	werase(Main_Window);

/* Initialize the main menu */
	Main_Menu.CurrentItem=0;
        Main_Menu.Rows = 1;
        Main_Menu.Cols = COLS;
        Main_Menu.BeginRow = 1;
        Main_Menu.BeginCol = 0;
	Main_Menu.Wdw = subwin(stdscr,Main_Menu.Rows, Main_Menu.Cols, Main_Menu.BeginRow,
		Main_Menu.BeginCol);

/* Clear to end of menu line in main window color */

	wattron(Main_Menu.Wdw,COLOR_PAIR(2));
	werase(Main_Menu.Wdw);
	touchwin(Main_Menu.Wdw);
	wrefresh(Main_Menu.Wdw);
        strcpy(Main_Menu.Item[0].text, "| File |");
        strcpy(Main_Menu.Item[1].text, "  View |");
        strcpy(Main_Menu.Item[2].text, "  Sort |");
        strcpy(Main_Menu.Item[3].text, "Options|");
        strcpy(Main_Menu.Item[4].text , "  Help |");

/* Initialize Menu Items */
     /* Colors for Menu Items */
	init_pair(4,COLOR_BLACK,COLOR_MAGENTA);
	
        for(i=0;i<MAIN_MENU_ITEMS;i++){
          Main_Menu.Item[i].Rows = Main_Menu.Rows;
	  Main_Menu.Item[i].Cols = MAIN_MENU_ITEM_WIDTH;
          Main_Menu.Item[i].BeginRow = Main_Menu.BeginRow;
          Main_Menu.Item[i].BeginCol = Main_Menu.BeginCol +
                MAIN_MENU_ITEM_WIDTH*i;
          Main_Menu.Item[i].Wdw=subwin(stdscr,Main_Menu.Item[i].Rows,
                Main_Menu.Item[i].Cols,Main_Menu.Item[i].BeginRow,
                Main_Menu.Item[i].BeginCol);
          wattrset(Main_Menu.Item[i].Wdw,COLOR_PAIR(4));
	  werase(Main_Menu.Item[i].Wdw);
          Main_Menu.Item[i].state = NORMAL;
	  wmove(Main_Menu.Item[i].Wdw,0,0);
          waddstr(Main_Menu.Item[i].Wdw, Main_Menu.Item[i].text);
	  leaveok(Main_Menu.Item[i].Wdw,0);
	  wrefresh(Main_Menu.Item[i].Wdw);

/* Initialize submenus as we go */
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
                           strcpy(SubMenu[i].Item[j].text,"Reload  ");
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
                           strcpy(SubMenu[i].Item[j].text,"Exit    ");
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

                           strcpy(SubMenu[i].Item[j].text,"Errors  ");
                           SubMenu[i].Item[j].state = SELECTED;
                           break;
                       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Runner  ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Team    ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 3:
                           strcpy(SubMenu[i].Item[j].text,"Scorall ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Bests   ");
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
                           strcpy(SubMenu[i].Item[j].text,"Name    ");
                           SubMenu[i].Item[j].state = SELECTED;
                           break;
                       case 1:
			   strcpy(SubMenu[i].Item[j].text,"Points  ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Time    ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 3:
                           strcpy(SubMenu[i].Item[j].text,"Up/Down ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Depth   ");
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
                           strcpy(SubMenu[i].Item[j].text,"Teams   ");
                           SubMenu[i].Item[j].state = SELECTED;
                           break;
                       case 1:
                           strcpy(SubMenu[i].Item[j].text,"Roster  ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Results ");

                     SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 3:
                           strcpy(SubMenu[i].Item[j].text,"Year    ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Events  ");
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
                           strcpy(SubMenu[i].Item[j].text,"Aims    ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 2:
                           strcpy(SubMenu[i].Item[j].text,"Rules   ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       case 3:
                           strcpy(SubMenu[i].Item[j].text,"Man Page");
                           SubMenu[i].Item[j].state = NORMAL;

                      break;
                       case 4:
                           strcpy(SubMenu[i].Item[j].text,"Source  ");
                           SubMenu[i].Item[j].state = NORMAL;
                           break;
                       default: ;
                 }

                        break;

                default: ;
           }  // end of switch on i: loop over main menu items

	 SubMenu[i].CurrentItem = 0;
           SubMenu[i].BeginCol = Main_Menu.Item[i].BeginCol;
           SubMenu[i].Cols = MAIN_MENU_ITEM_WIDTH;
           SubMenu[i].BeginRow = Main_Menu.BeginRow + 1;
           SubMenu[i].Rows = SubMenu[i].NoItems;
           for(j=0; j< SubMenu[i].NoItems;j++){
                SubMenu[i].Item[j].BeginRow =
                     SubMenu[i].BeginRow + j;
                SubMenu[i].Item[j].Rows = 1;
                SubMenu[i].Item[j].BeginCol =
                      SubMenu[i].BeginCol;
                SubMenu[i].Item[j].Cols = MAIN_MENU_ITEM_WIDTH;
                SubMenu[i].Item[j].Wdw =  subwin(stdscr, 
                       SubMenu[i].Item[j].Rows,
                       SubMenu[i].Item[j].Cols,
                       SubMenu[i].Item[j].BeginRow,
                       SubMenu[i].Item[j].BeginCol);
          	wattrset(SubMenu[i].Item[j].Wdw,COLOR_PAIR(4));
		leaveok(SubMenu[i].Item[j].Wdw,0);
		werase(SubMenu[i].Item[j].Wdw);
                }

	}  // end of loop on i: over main menu items

// All Menus and Windows are now initialized.

// Output the first screen of the open file

Y = FirstScreen(openfile);

/* Main Loop */
	while(1){

/* Try to do the right thing with the cursor */
	switch(CurrentFocus){
		case MENUFOCUS:
			if(Cursor == 1) cursoroff();
			break;
		case SUBMENUFOCUS:
			if(Cursor == 1) cursoroff();
			break;
		case WINDOWFOCUS:
			if(Cursor == 0) cursoron();
			
		default:
			;
		}
	Status();


/* Put cursor where we think it is */

	wmove(Main_Window,Y-1,X-1);
	wrefresh(Main_Window);

	key = GetKey(CurrentFocus);  // await and interpret keystrike

/* Clear any error message and update status bar */
        strcpy(mystatus.Message," <esc> = Activate Menu");
        Status();


	if(key == -1) {     /* bad key: beep */
		 fprintf(stderr,"\a");
		 continue;}

	switch(CurrentFocus){
	    case MENUFOCUS:     /* First of 3 cases. Valid keystrikes here
                                   move us around in main menu and pull down
                                   submenus  */
		
		switch(key){

			case RIGHTARROW:
				Main_Menu.Item[Main_Menu.CurrentItem].state 
					 = NORMAL;
				Main_Menu.CurrentItem = 
				 (Main_Menu.CurrentItem + 1) % MAIN_MENU_ITEMS;
				Main_Menu.Item[Main_Menu.CurrentItem].state 
					  = SELECTED;
				DisplayMainMenu();
				break;

			case LEFTARROW:
				 Main_Menu.Item[Main_Menu.CurrentItem].state 
				   = NORMAL;
				 Main_Menu.CurrentItem = 
				(Main_Menu.CurrentItem 
				  + MAIN_MENU_ITEMS - 1) % MAIN_MENU_ITEMS;
				  Main_Menu.Item[Main_Menu.CurrentItem].state 
				  = SELECTED;
				 DisplayMainMenu();
				 break;

			case ESCAPE:     // Gets back to main window focus
				CurrentFocus = WINDOWFOCUS;
				 Main_Menu.Item[Main_Menu.CurrentItem].state 
				   = NORMAL;
				DisplayMainMenu();
				wrefresh(Main_Window);
				break;
			case '\n':
			case DOWNARROW:
				CurrentFocus = SUBMENUFOCUS;
				SubMenu[Main_Menu.CurrentItem].CurrentItem = 0;
				SubMenu[Main_Menu.CurrentItem].Item[0].state = SELECTED;
				DisplaySubMenu(Main_Menu.CurrentItem,SubMenu);
				break;

			default:
				break;
		}     // end switch
		break;
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

			/* need to redraw main window too */
			touchwin(Main_Window);
			wrefresh(Main_Window);
			DisplayMainMenu();
			break;

			case '\n':  // This is where all the action is!
			CurrentFocus = WINDOWFOCUS;
       		        SubMenu[Main_Menu.CurrentItem].Item[
          		SubMenu[Main_Menu.CurrentItem].CurrentItem].state
         			= NORMAL;
         		Main_Menu.Item[Main_Menu.CurrentItem].state=NORMAL;
        		DisplayMainMenu();

       	        	 switch(Main_Menu.CurrentItem){
          			case 0:  /* FILE MENU */
                		switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                		   case 4: /* QUIT PROGRAM */
                        	 	fclose(openfile);
					noraw();
					nocbreak();
					endwin();
					die();
                                        return 0;
				    case 0: /* OPEN FILE */
                			strcpy(DialogueData.Title,"           Open a File");
                			strcpy(DialogueData.msg1,"Enter Full Path to File.");
                			if(Dialogue()==0){
                        			tempfilehold=openfile;
                         			if((openfile = fopen(DialogueData.Input,"r"))==NULL){
                         			openfile = tempfilehold;
                         			 myerror(&mystatus,"Can't Open ",DialogueData.Input);
                          			Status();
					wrefresh(Main_Window);
                         		break;
                         		}
                			fclose(tempfilehold);  // succeeded
                 			strcpy(mystatus.OpenFile,DialogueData.Input);
                 		        Status();
                		        FirstScreen(openfile);
               				}
               				else {
				        touchwin(Main_Window);
                			wrefresh(Main_Window);
                			}
                			break;

				case 1: /* RELOAD */
                               		 if( reload() == 1){
                                    	 myerror(&mystatus,"Reload Failed. "," ");
                                    	 Status();
                                	}
                                	openfile = fopen(ERRLOG,"r");
					FirstScreen(openfile);
                                	break;

				      case 2:
					strcpy(DialogueData.Title,"           Save File");
					strcpy(DialogueData.msg1,"Enter Full Path to Save As");
					if(Dialogue()==0){
       /* Copy Filename for later use */
					strcpy(buffer,DialogueData.Input);
                    			// Attempt to create handle for new file
         				if((handle = open(DialogueData.Input,
						O_CREAT|O_EXCL|O_WRONLY,0644))==-1){
          				if(errno == EEXIST){                     
 					    strcpy(buffer,DialogueData.Input);
                			    strcpy(DialogueData.Title,"           File Exists!");
                			    strcpy(DialogueData.msg1,"Overwrite [o] or Append [a] ");
                		        	strcpy(DialogueData.msg2,"<esc> to Cancel");
                			  if(Dialogue()==0){
                        			switch(DialogueData.Input[0]){  // depending on user inp
                          				case 'o':

                               				if(( tempfilehold = fopen(buffer,"w"))==NULL){
                                			myerror(&mystatus,"Unable to open: ",buffer);
                                			Status();
							touchwin(Main_Window);
							wrefresh(Main_Window);
                                			break;
                                			}

			                                break;
                       				       case 'a':

                               				if( (tempfilehold = fopen(buffer,"a"))==NULL){
                                			 myerror(&mystatus,"Unable to open: ",buffer);
                                			   Status();
							 touchwin(Main_Window);
							 wrefresh(Main_Window);				
                                			 break;
                                			}
                                			break;
                          				default:
							myerror(&mystatus,"Unrecognized Response"," ");
                                               		 Status();
							touchwin(Main_Window);
							wrefresh(Main_Window);
                                                	break;
                                         	}      // end switch
                                               // Now copy the file
                                		fgetpos(openfile,&start);
                                		rewind(openfile);
                                		while((c=fgetc(openfile))!=EOF)
                                		fputc(c,tempfilehold);
                                		fclose(tempfilehold);
                                		fsetpos(openfile,&start);
						touchwin(Main_Window);
						wrefresh(Main_Window);
                                		break;
                          			}
                          		else {                     // Bailed out of File Exist                               
					 touchwin(Main_Window);
					 wrefresh(Main_Window);
                                	break;
                          		}
                          	}

                          // Did attempt to create new file handle generate some                       

                          if(errno == ENOENT )
                          myerror(&mystatus,"Invalid Path: ",DialogueData.Input);                         
			  if(errno == EACCES)
                          myerror(&mystatus,"Permission Denied: ",DialogueData.Input);                         
			 if(errno == EMFILE)
                          myerror(&mystatus,"Too many open files", " ");
                          Status();
			  touchwin(Main_Window);
			  wrefresh(Main_Window);
                         break;
			}
			   // OK, we have a handle
                           // for a new file
                           // open stream to it
			close(handle);
                        tempfilehold = fopen(buffer,"w");

                               // save position in current file, copy it
                               // and restore position in current file

                               fgetpos(openfile,&start);
                               rewind(openfile);
                               while((c=fgetc(openfile))!=EOF)
                                fputc(c,tempfilehold);
                               fclose(tempfilehold);
                               fsetpos(openfile,&start);
                               }
				touchwin(Main_Window);
				wrefresh(Main_Window);
                                break;

						
				case 3:    /* Print the open file */
				/* Save and restore file position */
				strcpy(DialogueData.Title,"       Printer Setup");
				strcpy(DialogueData.msg1,"Enter n for normal, l for landscape.");
				strcpy(DialogueData.msg2,"<Ret> to apply, <esc> to cancel");
				if(Dialogue()==0){

				fgetpos(openfile,&start);
				if(DialogueData.Input[0]=='l')                 
				PrintOut(openfile,1);
				else PrintOut(openfile,0);
				fsetpos(openfile,&start);
				}
				touchwin(Main_Window);
				wrefresh(Main_Window);
				break;


				} // End File Menu Switch
			 break;
		         case 1:  /* VIEW MENU */		
			switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){

		           case 0:  /* Errors */
			     tempfilehold = openfile;
			     if((openfile = fopen(ERRLOG,"r"))==NULL){
					openfile = tempfilehold;
					myerror(&mystatus,"Can't Open ",ERRLOG);
					Status();
					touchwin(Main_Window);
					wrefresh(Main_Window);
					break;
				     }
				     fclose(tempfilehold);  // succeeded
				     strcpy(mystatus.OpenFile,ERRLOG);
				     Status();
				     FirstScreen(openfile);
				     break;

				  case 1: /* Runner Report */
					strcpy(DialogueData.Title,"        Choose a Runner");
					strcpy(DialogueData.msg1," Enter name exactly as in roster.");
					strcpy(DialogueData.msg2," <esc> to cancel");
					if(Dialogue()==0){

						fclose(openfile);
						tempwrite = new fstream;
						tempwrite->open(TempFile,ios::out);

						if(RunnerReport(DialogueData.Input,tempwrite)!=0){
						   myerror(&mystatus,DialogueData.Input," not in roster.");    
						   Status();
						   tempwrite->rdbuf()->close();
						   openfile = fopen(TempFile,"r");
						   touchwin(Main_Window);
						   wrefresh(Main_Window);
						   break;
						   }
					      tempwrite->rdbuf()->close();
					      delete tempwrite; 
					      if((openfile = fopen(TempFile,"r"))==NULL){

						 myerror(&mystatus,"Can't Open ",TempFile);
						 Status();
						   touchwin(Main_Window);
						   wrefresh(Main_Window);
						break;
						}
					      strcpy(mystatus.OpenFile,TempFile);
					      Status();
					      FirstScreen(openfile);
					      break;
					     }
					     else {
						   touchwin(Main_Window);
						   wrefresh(Main_Window);
						break;
					     }
					break;

				  case 2: /* Team Report */
				     strcpy(DialogueData.Title,"         Choose a Team");
				     strcpy(DialogueData.msg1,"Enter the number of team");
				     strcpy(DialogueData.msg2,"");
				     for( i=0;i<number_teams;i++){
					sprintf(buffer,"[%d]%s ",i,teams[i]->GetShortName());
					strcat(DialogueData.msg2,buffer);
				     }
				     if(Dialogue() == 0){
				      if(!isdigit(DialogueData.Input[0])){
				       myerror(&mystatus,"Must be a digit."," ");
				       Status();
				       touchwin(Main_Window);
					wrefresh(Main_Window);
							break;
						       }
				       if(atoi(DialogueData.Input)>=number_teams){
				       myerror(&mystatus,"Choice out of range."," ");
				       Status();
				       touchwin(Main_Window);
					wrefresh(Main_Window);
							break;
				      }
				      fclose(openfile);
				      tempwrite = new fstream;
				      tempwrite->open(TempFile,ios::out);
				      teams[atoi(DialogueData.Input)]->PrintAll(tempwrite);
				      tempwrite->rdbuf()->close();
				      delete tempwrite;
				      openfile = fopen(TempFile,"r");
				      strcpy(mystatus.OpenFile,TempFile);
				      Status();

				      FirstScreen(openfile);
				      break;
				      }
				      else {
				       touchwin(Main_Window);
					wrefresh(Main_Window);
					break;
				      }
				  break;

				  case 3:  /* Scorall */
				      fclose(openfile);
				     tempwrite = new fstream;
				     tempwrite->open(TempFile,ios::out);
				      scorall(tempwrite);
				      tempwrite->rdbuf()->close();
				     delete tempwrite;
		 
				     if((openfile = fopen(TempFile,"r"))==NULL){
		  
					myerror(&mystatus,"Can't Open ",TempFile);
					Status();
				        touchwin(Main_Window);
					wrefresh(Main_Window);
					break;
					   }
				FirstScreen(openfile);	
				break;
						      

 				   case 4: // Superlatives 
				strcpy(DialogueData.Title,"    Superlatives: Choose an event");
				strcpy(DialogueData.msg1,"Change sort order, direction and depth");
				strcpy(DialogueData.msg2,"from the sort menu. <esc> to cancel.");
				if(Dialogue()==0){
					fclose(openfile);
					tempwrite = new fstream;
					tempwrite->open(TempFile,ios::out);
					if(Superlatives(tempwrite,DialogueData.Input)
						== -1 ){
					 myerror(&mystatus,"Can't do that event.","");
					 Status();
					 touchwin(Main_Window);
					 wrefresh(Main_Window);
				         break;
					}
					tempwrite->rdbuf()->close(); 
					delete tempwrite;
					openfile = fopen(TempFile,"r");
					strcpy(mystatus.OpenFile,TempFile);
					Status();
					FirstScreen(openfile);
					break;
				}
				else {
					touchwin(Main_Window);
					wrefresh(Main_Window);
					break;
				}

			}
			break;
   		  case 2:  /* SORT MENU */
		  switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){ 
			case 0:  // Name
				strcpy(mystatus.SortType,"Last Name");
				SortFlag = BYLASTNAME;
				touchwin(Main_Window);
				wrefresh(Main_Window);
				Status();
				break;
			case 1:  // Points
				strcpy(mystatus.SortType,"Points");
				SortFlag = BYEVENTPOINTS;
				touchwin(Main_Window);
				wrefresh(Main_Window);
				Status();
				break;
			case 2:  // Name
				strcpy(mystatus.SortType,"Time");
				SortFlag = BYTIMES;
				touchwin(Main_Window);
				wrefresh(Main_Window);
				Status();
				break;

			case 3:   // direction
				direction = 1 - direction;  // toggle
				if(direction == 0)
				sprintf(mystatus.SortDirection,"%s",UPARROWSYMBOL);
				else
				sprintf(mystatus.SortDirection,"%s",DOWNARROWSYMBOL);
				reload();
				touchwin(Main_Window);
				wrefresh(Main_Window);
				Status();
				break;
					  
			case 4:   // depth
				strcpy(DialogueData.Title,"       Set Depth");
				strcpy(DialogueData.msg1,"Enter number of runners to display");
				strcpy(DialogueData.msg2,"in View/Bests. <esc> to cancel");
				if(Dialogue()==0)
					depth = atoi(DialogueData.Input);
					if(depth < 1 || depth > 500) {            // sanity check
					       myerror(&mystatus,"???"," 10 assumed");
					       Status();
					       depth = 10;
				       }
				touchwin(Main_Window);
				wrefresh(Main_Window);
				Status();
				break;
						
					  
		  }
		  break;

		  case 3:  /* OPTIONS MENU */
			switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
			   case 0:  /*  Teams */
				strcpy(DialogueData.Title,"       Set Teams File");
				strcpy(DialogueData.msg1,"Enter path from WD to teams file.");
				strcpy(DialogueData.msg2,"<Enter> to apply. <esc> to cancel.");
				if(Dialogue()==0){
					strcpy(TeamsFile,DialogueData.Input);
				}
				touchwin(Main_Window);
				wrefresh(Main_Window);
				break;
			 case 1:  /*  Roster */
				strcpy(DialogueData.Title,"       Set Roster File");
				strcpy(DialogueData.msg1,"Enter path from WD to roster file.");
				strcpy(DialogueData.msg2,"<Enter> to apply. <esc> to cancel.");
				if(Dialogue()==0){
					strcpy(RosterFile,DialogueData.Input);
				}
				touchwin(Main_Window);
				wrefresh(Main_Window);
				break;
			   case 2:  /* Results  */
				strcpy(DialogueData.Title,"       Set Results File");
				strcpy(DialogueData.msg1,"Enter path from WD to results file.");
				strcpy(DialogueData.msg2,"<Enter> to apply. <esc> to cancel.");
				if(Dialogue()==0){
					strcpy(ResultsFile,DialogueData.Input);
				}
				touchwin(Main_Window);
				wrefresh(Main_Window);
				break;
			   case 3:  /*  Competition Year */
				strcpy(DialogueData.Title,"       Set Competition Year");
				strcpy(DialogueData.msg1,"Enter Year (e.g. 1996, 2001)");
				strcpy(DialogueData.msg2,"<Enter> to apply. <esc> to cancel.");
				if(Dialogue()==0){
					temp = CompetitionYear;
				       CompetitionYear = atoi(DialogueData.Input);
					if ((CompetitionYear < 1970) ||
						(CompetitionYear > 2050)){
				myerror(&mystatus,"Year not in [1970-2050] ","");
				CompetitionYear = temp;
				touchwin(Main_Window);
				Status();
				break;
				}
				}
				touchwin(Main_Window);
				wrefresh(Main_Window);
				break;
			  case 4: /* Events */
				strcpy(DialogueData.Title,"      Change list of excluded events");
				strcpy(DialogueData.msg1," You will be prompted for each event.");
				strcpy(DialogueData.msg2,"<ret> to continue, <esc> to bail out.");
				if(Dialogue() == 0){
					Excluded.number = 0;
					ExcludedRelays.number=0;  // start with a clean slate
					strcpy(DialogueData.Title,"      Exclude an event (non-relay)");
							
					strcpy(DialogueData.msg2,"[yn], or <esc> to quit");
					for(i=0;i<NOEVENTS-NORELAYS;i++){
						strcpy(DialogueData.msg1,"Exclude  ");

						strcat(DialogueData.msg1,EventNames[i]);
						strcat(DialogueData.msg1," ?");
						if(Dialogue()==0){

						  if(strcmp(DialogueData.Input,"y")==0)
						  Excluded.events[Excluded.number++]=i;
						}
						else break;
					}
					strcpy(DialogueData.Title,"    Exclude a relay event");
					for(i=NOEVENTS-NORELAYS; i < NOEVENTS;i++){
						strcpy(DialogueData.msg1,"Exclude  ");

						strcat(DialogueData.msg1,EventNames[i]);
						strcat(DialogueData.msg1," ?");
						if(Dialogue()==0){

	   				  if(strcmp(DialogueData.Input,"y")==0)
										  Excluded.events[Excluded.number++]=i; 
					  ExcludedRelays.events[ExcludedRelays.number++]= i-NOEVENTS+NORELAYS;

					}
					else break;
					}
					}
					touchwin(Main_Window);
					wrefresh(Main_Window);
						break;

					  }
					  break;

			  case 4:  /* HELP MENU */
			switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
				  case 0:  /* INFO */
				     tempfilehold = openfile;
				     if((openfile = fopen(INFOFILE,"r"))==NULL){
					openfile = tempfilehold;
       				myerror(&mystatus,"Can't Open ",INFOFILE);
				touchwin(Main_Window);
				Status();
				break;
			     }
			     fclose(tempfilehold);  // succeeded
			     strcpy(mystatus.OpenFile,INFOFILE);
			     Status();
			     FirstScreen(openfile);
			     break;

			   case 1:  /* AIMS */
			     tempfilehold = openfile;
			     if((openfile = fopen(AIMSFILE,"r"))==NULL){
				openfile = tempfilehold;
				myerror(&mystatus,"Can't Open ",AIMSFILE);
				touchwin(Main_Window);
				Status();
				break;
			     }
			     fclose(tempfilehold);  // succeeded
			     strcpy(mystatus.OpenFile,AIMSFILE);
			     Status();
			     FirstScreen(openfile);
			     break;

			   case 2:  /* RULES */
			     tempfilehold = openfile;
			     if((openfile = fopen(RULESFILE,"r"))==NULL){
					openfile = tempfilehold;
					myerror(&mystatus,"Can't Open ",RULESFILE);
				touchwin(Main_Window);
					Status();
					break;
			     }
			     fclose(tempfilehold);  // succeeded
			     strcpy(mystatus.OpenFile,RULESFILE);
			     Status();
			     FirstScreen(openfile);
			     break;

			   case 3:  /* MAN PAGE */
			     tempfilehold = openfile;
			     if((openfile = fopen(MANFILE,"r"))==NULL){
				openfile = tempfilehold;
				myerror(&mystatus,"Can't Open ",MANFILE);
				touchwin(Main_Window);
				Status();
				break;
			     }
			     fclose(tempfilehold);  // succeeded
			     strcpy(mystatus.OpenFile,MANFILE);
			     Status();
			     FirstScreen(openfile);
			     break;


			   case 4:  /* SOURCE */
			     tempfilehold = openfile;
			     if((openfile = fopen(SOURCEFILE,"r"))==NULL){
				openfile = tempfilehold;
				myerror(&mystatus,"Can't Open ",SOURCEFILE);
				touchwin(Main_Window);
				Status();
				break;
			     }
			     fclose(tempfilehold);  // succeeded
			     strcpy(mystatus.OpenFile,SOURCEFILE);
			     Status();
			     FirstScreen(openfile);
			     break;

			   default:
			     break;
			  } // end of File menu switch
					
			 }  // End of Case Return switch			
			 break;
	}  // end of SUBMENU FOCUS SWITCH

	break;
	case WINDOWFOCUS:        /* Main Window Focus */
		switch(key){

		case ESCAPE:
			Main_Menu.Item[Main_Menu.CurrentItem].state 
			 = SELECTED;
			CurrentFocus = MENUFOCUS;
			DisplayMainMenu();
			break;

		case ' ':         // dump next page: one line overlap
			lineno +=  DumpToScreen(openfile)-1;
			start = LinePositions[lineno];
			fsetpos(openfile,&start);
	// position cursor at  bottom of screen
			Y=LINES-3;
			wmove(Main_Window,LINES-4,0);
			wrefresh(Main_Window);
			break;
		case UPARROW:
			if(Y==1){  /* cursor at top */
				if(lineno == 0) 
					fprintf(stderr,"\a");
				else {
					start = LinePositions[--lineno];
					fsetpos(openfile,&start);
                                        wmove(Main_Window,0,X-1);
					DumpToScreen(openfile);
				      }
			}
			else {
				wmove(Main_Window,(--Y)-1,X-1);
				--lineno;
				start = LinePositions[lineno];
				fsetpos(openfile,&start);
				wrefresh(Main_Window);
			}
			break;
		case DOWNARROW:
			if(Y==LINES-3){  /* cursor at bottom */
				if(lineno == TotalLines-1) 
					fprintf(stderr,"\a");
				else {
					start = LinePositions[++lineno];
					fsetpos(openfile,&start);
					ReverseDumpToScreen(openfile,lineno,LinePositions);
                                        wmove(Main_Window,LINES-4,X-1);
					wrefresh(Main_Window);
				      }
			}
			else {
				if(lineno== TotalLines-1) {
					fprintf(stderr,"\a");
					break;
				}
				wmove(Main_Window,(++Y)-1,X-1);
				++lineno;
				start = LinePositions[lineno];
				fsetpos(openfile,&start);
				wrefresh(Main_Window);
			}
			break;
		case '\n':
			// put cursor at bottom of page and
			// do a down arrow
			if( lineno + LINES-3 -Y < TotalLines -1)
			{
			lineno += LINES -3 -Y + 1;
			start = LinePositions[lineno];
			fsetpos(openfile,&start);
			Y = LINES-3;
			ReverseDumpToScreen(openfile,lineno,LinePositions);
			wmove(Main_Window,Y-1,X-1);
			wrefresh(Main_Window);
			}
			else fprintf(stderr,"\a");
			break;
		case CONTROL_HOME:
				 start = LinePositions[0];
				 lineno=0;
				 X=Y=1;
				 fsetpos(openfile,&start);
				 DumpToScreen(openfile);
				 wmove(Main_Window,Y-1,X-1);
				 wrefresh(Main_Window);
				 break;
		case CONTROL_END:
				 start = LinePositions[TotalLines-LINES+3];
				 lineno=TotalLines-1;
				 X=1;
				 Y=LINES-3;
				 fsetpos(openfile,&start);
				 DumpToScreen(openfile);
				 wmove(Main_Window,Y-1,X-1);
				 wrefresh(Main_Window);
				 start = LinePositions[TotalLines-1];
				 fsetpos(openfile,&start);
				 break;
		case PAGEDOWN:
                                 // will cursor end up beyond eof?
				 if((lineno + LINES-3+1-Y) > TotalLines-1)
					fprintf(stderr,"\a");
				 else {
					start = LinePositions[lineno+LINES-3+1-Y];
					fsetpos(openfile,&start);
					DumpToScreen(openfile);
					if((lineno+LINES-3) <= TotalLines-1){
						lineno += LINES-3;
						start = LinePositions[lineno];
						fsetpos(openfile,&start);
						wmove(Main_Window,Y-1,X-1);
						wrefresh(Main_Window);
					}
					else {
						Y += (TotalLines-lineno-LINES+2);
						lineno = TotalLines-1;
						start = LinePositions[lineno];
						fsetpos(openfile,&start);
						wmove(Main_Window,Y-1,X-1);
						wrefresh(Main_Window);
					      }
				 }
				 break;
		case PAGEUP:
				 if(( lineno - LINES +3 + 1 - Y ) <= 0)
					if((lineno-LINES-3)<0)
					  fprintf(stderr,"\a");
					else {
					  start = LinePositions[0];
					  fsetpos(openfile,&start);
					  DumpToScreen(openfile);
					  wmove(Main_Window,Y-1,X-1);
					  wrefresh(Main_Window);
					  lineno = Y-1;
					  start = LinePositions[lineno];
					  fsetpos(openfile,&start);
					}
				 else {
					start = LinePositions[lineno-LINES+3+1-Y];
					fsetpos(openfile,&start);
					DumpToScreen(openfile);
					wmove(Main_Window,Y-1,X-1);
					wrefresh(Main_Window);
					lineno -= (LINES-3);
					start = LinePositions[lineno];
					fsetpos(openfile,&start);
				 }
				 break;

		case RIGHTARROW:
				 if(X < COLS){wmove(Main_Window,Y-1,(++X)-1);
					wrefresh(Main_Window);
				}
				 else { X=1; /* code for down arrow here */
					if(Y==LINES-3){  /* cursor at bottom */
						if(lineno == TotalLines-1) 
						     fprintf(stderr,"\a");
						else {
							start = LinePositions[++lineno];
							fsetpos(openfile,&start);
							ReverseDumpToScreen(openfile,lineno,LinePositions);
							wmove(Main_Window,LINES-4,X-1);					
							wrefresh(Main_Window);
                                                         
						}
					}
					else {
						if(lineno== TotalLines-1) {
						  fprintf(stderr,"\a");
						  break;
						 }

                                                wmove(Main_Window,(++Y)-1,X-1); 
						wrefresh(Main_Window);
						++lineno;
						 start = LinePositions[lineno];
						  fsetpos(openfile,&start);
					  }
				   }
				   break;
		case  LEFTARROW:
			if(X > 1){ wmove(Main_Window,Y-1,(--X)-1);
				wrefresh(Main_Window);
			}
			else {X=COLS;
				/* code for uparrow here */
				if(Y==1){  /* cursor at top */
				    if(lineno == 0) 
						fprintf(stderr,"\a");
					else {
					    start = LinePositions[--lineno];
					   fsetpos(openfile,&start);
					  DumpToScreen(openfile);
					  wmove(Main_Window,0,X-1);
					  wrefresh(Main_Window);
					 }
			 }
			 else {
					wmove(Main_Window,(--Y)-1,X-1);
					wrefresh(Main_Window);
				     --lineno;
				     start = LinePositions[lineno];
				    fsetpos(openfile,&start);
			 }
		      }
		      break;

		default:
			fprintf(stderr,"\a");  /* beep */
			break;
		}     // end switch
	break;    // end case ( Window focus )
	}  // end switch (Focus)
   }  // end main loop


	noraw();
	nocbreak();
	die();
	return 0;
}

// Die: clean up and exit program gracefully

int die()
{
/*      ignore interupts so we can get this done  */
	signal(SIGINT,SIG_IGN);

/* move cursor to lower left  */
	wattron(stdscr,COLOR_PAIR(2));  // clear blue screen
	clear();
	refresh();
	mvcur(0,COLS-1,LINES-1,0);

/*      make terminal the way it was */
	endwin();
	cursoron();
	return 0;          /* exit with exit status 0  */
}

int Status()
{
	char buffer[256];
	
	waddstr(Status_Bar,BLANKLINE);
	wrefresh(Status_Bar);
	sprintf(buffer,"%d:%d ",lineno+1,X);
        wmove(Status_Bar,0,0);
	waddstr(Status_Bar,buffer);
	waddstr(Status_Bar,mystatus.OpenFile);	
	waddstr(Status_Bar,"  ");
	waddstr(Status_Bar,mystatus.SortType);	
	waddstr(Status_Bar,mystatus.SortDirection);	
	waddstr(Status_Bar,"  ");
	waddstr(Status_Bar,mystatus.Message);	
	touchwin(Status_Bar);
	wrefresh(Status_Bar);
	return 0;
}

/* GetKey: Depending on the focus, get the next key strike and tell
us what it was. For example, in a menu we are only looking for 
vt100 escape sequences. Return -1 if an invalid key is struck */

int GetKey(int focus)
{
	int key;
	key =  wgetch(curscr);  /* using getch here with some versions
                                   of the library clears the screen and
                                   leaves it blank while waiting for char.
                                   Bug??? This seems to work, but not
                                   sure why */
	switch(focus) {
		case MENUFOCUS:
			switch(key){
				case '\n':
					return '\n';
				case BACKSPACE:
				case KEY_LEFT:
					return LEFTARROW;
				case KEY_RIGHT:
					return RIGHTARROW;
				case KEY_DOWN:
					return DOWNARROW; 
				case ESCAPE:
					return ESCAPE;
				default:
					return -1;
			}
#if 0
	/* This is now obsolete, because we're going to assume we're
           always using ncurses. I'll leave this one example in place. */
			if(key != ESCAPE) return -1;
			key = wgetch(curscr);
			if(key == ESCAPE) return ESCAPE;
			if(key != '[') return -1;
			key = wgetch(curscr);
			switch(key) {
				case 'D':
					return LEFTARROW;
				case 'B' :
					return DOWNARROW;
				case 'C': 
					return RIGHTARROW;
				default:
					return -1;
			}
#endif
		case SUBMENUFOCUS:
			switch(key){
				case '\n':
					return '\n';
				case KEY_DOWN:
					return DOWNARROW; 
				case KEY_UP:
					return UPARROW;
				case ESCAPE:
					return ESCAPE;
				default:
					return -1;
			}
		case WINDOWFOCUS:
			switch(key){
				case '\n':
					return '\n';
				case ' ':
					return ' ';
				case BACKSPACE:
				case KEY_LEFT:
					return LEFTARROW;
				case KEY_RIGHT:
					return RIGHTARROW;
				case KEY_DOWN:
					return DOWNARROW; 
				case KEY_UP:
					return UPARROW;
				case ESCAPE:
					return ESCAPE;
				case KEY_NPAGE:
					return PAGEDOWN;
				case KEY_PPAGE:
					return PAGEUP;
				case KEY_HOME:
				case KEY_A1:
					return CONTROL_HOME;
				case KEY_END:
				case KEY_C1:
					return CONTROL_END;
			/* allow some vi cursor movement keys */
			        case 'j':
					return DOWNARROW; 
			        case 'k': 
				         return UPARROW; 
				case 'h':
					 return LEFTARROW;
			        case 'l': 
					 return RIGHTARROW;
			        case 'G':
					 return CONTROL_END;
			        case 'F': 
					 return PAGEDOWN;
			        case 'B': 
					 return PAGEUP;
				default:
					return -1;
			}

		default:
			return -1;
	}
}	

int DisplayMainMenu()
{
	int i;

        for(i=0;i<MAIN_MENU_ITEMS;i++){

         if(Main_Menu.Item[i].state == SELECTED)
           //wattron(Main_Menu.Item[i].Wdw,A_REVERSE);
	     wstandout(Main_Menu.Item[i].Wdw);
          else {
           //wattroff(Main_Menu.Item[i].Wdw,A_REVERSE);
	   wstandend(Main_Menu.Item[i].Wdw);
	   wattrset(Main_Menu.Item[i].Wdw,COLOR_PAIR(4));  
	  }
	 werase(Main_Menu.Item[i].Wdw);
         waddstr(Main_Menu.Item[i].Wdw, 
                Main_Menu.Item[i].text);
	 
	 wrefresh(Main_Menu.Item[i].Wdw);
         }
         return 0;
}

int DisplaySubMenu(int N,SUBMENU *SubMenu)
{
        int i;

        for(i=0;i<(SubMenu+N)->NoItems;i++){
	         if((SubMenu+N)->Item[i].state == SELECTED)
       		    wattron((SubMenu+N)->Item[i].Wdw,A_REVERSE);
                 /* reverse video */
          	else {
           	wattroff((SubMenu+N)->Item[i].Wdw,A_REVERSE);
	   	wattrset((SubMenu+N)->Item[i].Wdw,COLOR_PAIR(4));  
                 /* normal */
          	}
	  	werase((SubMenu+N)->Item[i].Wdw);
          	waddstr((SubMenu+N)->Item[i].Wdw , (SubMenu+N)->Item[i].text);
	 	 wrefresh((SubMenu+N)->Item[i].Wdw);

         	}
         	return 0;
}

/* Dumps the very first screen-full of a newly opened file */

int FirstScreen(FILE *openfile)
{

        fpos_t start;

      /* read through the file and fill the line position array */
        lineno = 0;

        fgetpos(openfile,&start);           // first line
        LinePositions[0]=start;
        while(fgets(LineBuf,COLS,openfile) != NULL){
         fgetpos(openfile,&start);
         LinePositions[++lineno] = start;
         }

        TotalLines = lineno;   /* number of logical lines in file */
        // Dump the first screenfull

        rewind(openfile);
        lineno =  DumpToScreen(openfile) - 1;
        start = LinePositions[lineno];
        fsetpos(openfile,&start);
       // position cursor at  bottom of screen
	wmove(Main_Window,lineno,0);
	wrefresh(Main_Window);

        Y=lineno+1;
        X=1;
        return Y;
}

/* Dump a screenfull of data from file pointed to by fptr. Leave the
   stream position at the same point as when called. Word wrap.
   Returns number of lines dumped.
*/

int DumpToScreen(FILE *fptr){

        fpos_t start;
        char *CharPtr;
        int lineno=0;
        int n;
	int x,y;

      werase(Main_Window); 

        /* Record the starting position of stream */
        fgetpos(fptr,&start);

        while(lineno < LINES - 4){
        /* Get the next chunk of line into buffer */

           CharPtr = fgets(LineBuf,COLS,fptr);
           n = strlen(LineBuf);

           if(CharPtr != NULL){
        /* See whether the buffer ends with a newline and act accordingly */
                if(*(CharPtr+n-1)=='\n'){
			getyx(Main_Window,y,x);
			*(CharPtr+n-1) = '\0';         // take out newline
			waddstr(Main_Window,LineBuf);
			clrtoeol();               // blank rest of line
			wmove(Main_Window,y+1,0);  // move cursor to beginning
                                                  //  of next line
		}
                else{
			getyx(Main_Window,y,x);
			waddstr(Main_Window,LineBuf);
			wmove(Main_Window,y+1,0);  // move cursor to beginning
		}
	
             lineno++;
             }
           else {
                    /* at EOF */
		clrtobot();
		lineno--;
                break;
             }
        }

        if(CharPtr != NULL){
        /* handle last line differently */
        CharPtr = fgets(LineBuf,COLS,fptr);
        n = strlen(LineBuf);
        if(CharPtr != NULL){
                if(*(CharPtr+n-1)=='\n'){
			*(CharPtr+n-1) = '\0';         // take out newline
			waddstr(Main_Window,LineBuf);
			clrtoeol();               // blank rest of line
			getyx(Main_Window,y,x);
			wmove(Main_Window,y,0);  // move cursor to beginning
						// of same line	
                }
                else {
			getyx(Main_Window,y,x);
			waddstr(Main_Window,LineBuf);
			wmove(Main_Window,y,0);  // move cursor to beginning
		}
        }
	else lineno--;
        }
        /* restore file position */
         fsetpos(fptr,&start);
	wrefresh(Main_Window);
         return lineno + 1;
}


int ReverseDumpToScreen(FILE *fptr,int lineno,
	fpos_t *LinePositions){

      
	fpos_t start;
	fpos_t temp;
	char *CharPtr;
	int Y=LINES-3;
	int n;

	werase(Main_Window);

	/* Record the starting position of stream */
	fgetpos(fptr,&start);

	while(Y > 0){
	/* Get the next chunk or line into buffer */
	   temp = *(LinePositions + lineno + (Y - LINES+3));
	   fsetpos(fptr,&temp);
	   wmove(Main_Window,(Y--)-1,0);
	   CharPtr = fgets(LineBuf,COLS,fptr);
	   n = strlen(LineBuf);

	/* See whether the buffer ends with a newline and act accordingly */
	if(*(CharPtr+n-1)=='\n'){
                        *(CharPtr+n-1) = '\0';         // take out newline
                        waddstr(Main_Window,LineBuf);
                        clrtoeol();               // blank rest of line
			waddch(Main_Window,'\r');
                }
                else{
                        waddstr(Main_Window,LineBuf);
           }

	}

	wrefresh(Main_Window);

	/* restore file position */
	 fsetpos(fptr,&start);
	 return LINES-3;
}


/* This displays a dialogue box and collects user input */
/* It is the calling program's responsibility to paint over the Dialogue Box */

int Dialogue()
{

        char *inputptr;
        char key;
        int xx,yy,i,promptlen;
	static char blankline[] = "                                            ";

        xx =  promptlen = strlen(DialogueData.Prompt);
        inputptr = DialogueData.Input;

	cursoron();
	
/* Fill the box with blanks */

	for(i=0;i<9;i++){
		wmove(DialogueBox,i,0);
		waddstr(DialogueBox,blankline);
	}
	touchwin(DialogueBox);
	wrefresh(DialogueBox);

	wmove(DialogueBox,0,0);
        waddstr(DialogueBox, DialogueData.Title);
	wmove(DialogueBox,1,0);
        waddstr(DialogueBox, DialogueData.Prompt) ;
	wmove(DialogueBox,3,0);
        waddstr(DialogueBox, DialogueData.msg1) ;
	wmove(DialogueBox,5,0);
        waddstr(DialogueBox, DialogueData.msg2 );
	wmove(DialogueBox,1,xx);
	wrefresh(DialogueBox);

        // get user input
        while(1) {
        key = wgetch(DialogueBox);
        switch(key){
                case ESCAPE:
			cursoroff();
			wrefresh(Main_Window);
                        return 1;
                case '\n':

			wrefresh(Main_Window);
                        *(inputptr) = '\0';
			cursoroff();
                        return 0;
                case BACKSPACE:
                        if(xx >= promptlen + 1) {
				getyx(DialogueBox,yy,xx);	
				xx--;
				wmove(DialogueBox,yy,xx);
				waddch(DialogueBox,' ');  // awkward
				wmove(DialogueBox,yy,xx);
                                *(inputptr--)='\0';
				wrefresh(DialogueBox);
                                }
                                break;
                default:
                        *(inputptr++)=key;
			xx++;
                        waddch(DialogueBox,  key);
			wrefresh(DialogueBox);
                         break;
         }
         }

}

void myerror(struct StatusInfo *status, char * string1, char *string2) {

        strcpy(status->Message, string1);
        strcat(status->Message,string2);
        return;
}

void cursoroff()
{
	printf("\x1B[?25l");
	Cursor = 0;
}

void cursoron()
{
	printf("\x1B[?25h");
	Cursor = 1;
}
