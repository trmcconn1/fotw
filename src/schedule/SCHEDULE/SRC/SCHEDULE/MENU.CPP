/* menu.cpp: C++ implementation of a generic document-view interface     */

/* Things specific to this implementation are declared in actions.h  and
    implemented in actions.cpp */

/* By Terry McConnell, 1997 */


/* The following Macro is executed whenever a cursor movement key is
   pressed while an edit is in progress. This will exit editmode,
   paste the portion of Main_Buffer previously cut for editing,
   update buffer position based upon position within the editing buffer,
   and redraw the screen.
*/

#define ENDEDIT AlterFlag = 0;\
		if(InsertMode){\
	          Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);\
		  InsertMode = 0;\
		}\
  		PasteAhead(CutBuffer,buf_pos);\
                buf_pos += cutbuf_pos;\
		Reset(buf_pos);
		

#include "gui.h"
#include "actions.h"
#include <ctype.h>
#include <dos.h>    
#include <dir.h>
#include <string.h>
#include <bios.h>
#include <io.h>
#include <errno.h>


/* Global Variables */


int lineno = 0;
int TotalLines;  // Number of logical lines in file
/* Logical lines are lines on the screen. They differ from actually lines
   int the open file, i.e., strings delimited by the newline character,
   because of tab expansion and word-wrap */

struct DialogueDataStruct DialogueData;
constream Main_Window;
constream DialogueBox;
constream Status_Bar;
constream Title_Line;
struct SUBMENU SubMenu[MAIN_MENU_ITEMS];
struct MENU  Main_Menu;
struct StatusInfo mystatus;

char LineBuf[LINELENGTH+1];
int X=1,Y=1; /* cursor positions */
unsigned LinePositions[MAXLINES]; /* offsets of each logical line in input buffer*/
int TabStops[] = {1,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80};
int MAXTABS = (sizeof TabStops)/sizeof(int);
int InsertMode = 0;     // 1 means insert mode is on
int DirtyFile = 0; // 1 means Main buffer differs from original file
char Landscape[50]=DEFAULTLANDSCAPE;
char Normalprinter[50]=DEFAULTNORMALPRINTER;
extern char TempFilePath[];

char MainBuffer[MAXBYTES]; // The open file is read into this for editing
unsigned int buf_pos;      // size of input file limited by this
unsigned int buf_end;

// Prevents mangle of name
extern "C" char *bufgets(char *s, int n, const char *bp, unsigned int start, unsigned int size);



enum MenuItemState { NORMAL, SELECTED, DIMMED };
enum Focus { MENUFOCUS, WINDOWFOCUS , SUBMENUFOCUS};




/* My own extractor that does tab to spaces conversion */

ostream& operator <<= (ostream&, char *);


char *myfgets(char *s, int n, FILE *fp);
char *bufgets(char *s, int n, const char *bp, unsigned int start, unsigned int size);
extern "C" int LogicalX(char  *line,int offset);
int Offset(int line, int logicalx);
extern "C" int Cut(char *dest, int offset,int n);
extern "C" int Paste(const char *source,int);
extern "C" int PasteAhead(const char *source,int);
extern  "C" { void far scroll(char,char,char); }
extern "C" { void RefreshLine(char *);}
extern "C" { void Reset(int);}
extern "C" { int BackSpace(void);}
extern "C" { int Delete(void);}
extern "C" { void Insert(char);}

static char buffer[BUFSIZ];
extern char CutBuffer[];

extern "C" int menu();        // so as not to mangle the name.


int
menu()
{
        unsigned longkey;
	unsigned key;
        int refreshstatus;
	int c;
        char *ptr;
        int i,j,k,n;
	int CurrentFocus = WINDOWFOCUS;
        constream PaintOver;
        FILE *openfile;
        int handle;
        FILE *tempfilehold;    /* used to store pointer while changing */

// Do any needed initialization here

        initialize(&openfile,&errorstream);

        // Initialize the status bar at bottom of screen
        strcpy(mystatus.OpenFile,TempFilePath);
        strcpy(mystatus.Message, DEFAULTMESSAGE);
	Status_Bar.window(1,25,80,25);
	Status_Bar << setattr((BROWN << 4) | WHITE);
        Status_Bar.width(80);
        Status_Bar.setf(ios::left,ios::adjustfield);
	// PaintOver:
	// This silly thing is needed because when you print the
	// text of a menu item, the cursor always falls one place beyond
	// the end and changes the screen color there. (even though the
	// cursor itself is invisible.) This stream is then painted over
	// that part in the main screen color to render it invisible.
        // At least, that's what I think is happening :-)

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
        strcpy(Main_Menu.Item[0].text, MENU0);
        strcpy(Main_Menu.Item[1].text, MENU1);
        strcpy(Main_Menu.Item[2].text, MENU2);
        strcpy(Main_Menu.Item[3].text, MENU3);
        strcpy(Main_Menu.Item[4].text , MENU4);
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
                           strcpy(SubMenu[i].Item[j].text,MENU00);             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,MENU01);             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,MENU02);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,MENU03);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,MENU04);             
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
                           strcpy(SubMenu[i].Item[j].text,MENU10);             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,MENU11);             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,MENU12);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,MENU13);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,MENU14);             
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
                           strcpy(SubMenu[i].Item[j].text,MENU20);             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,MENU21);             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,MENU22);             

			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,MENU23);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,MENU24);             
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
                           strcpy(SubMenu[i].Item[j].text,MENU30);             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,MENU31);             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,MENU32);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,MENU33);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,MENU34);             
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
                           strcpy(SubMenu[i].Item[j].text,MENU40);             
			   SubMenu[i].Item[j].state = SELECTED;
			   break;
		       case 1:
                           strcpy(SubMenu[i].Item[j].text,MENU41);             
			   SubMenu[i].Item[j].state = NORMAL;
			   break;
		       case 2:
                           strcpy(SubMenu[i].Item[j].text,MENU42);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 3:
                           strcpy(SubMenu[i].Item[j].text,MENU43);             
			   SubMenu[i].Item[j].state = NORMAL; 
			   break;
		       case 4:
                           strcpy(SubMenu[i].Item[j].text,MENU44);             
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
        Title_Line << TITLE;
	
	// Initialize the Main_Window

	Main_Window.window(1,3,80,24);
	Main_Window.clrscr();
	Main_Window << setattr((BLUE << 4) | WHITE);

	FirstScreen(openfile,&Main_Window);
        UpdateMessage(mystatus.Message);
        Status(&Status_Bar,&mystatus,0);
        refreshstatus = 0;

/////////////////////////////////////////////
//  MAIN LOOP 
/////////////////////////////////////////////
	while(1){                   /* Main Loop */
	  
		// Needs to be done here at the top of the loop because
                // the loop below normally ends with a break
                if(refreshstatus){
                  UpdateMessage(mystatus.Message);
		  Status(&Status_Bar,&mystatus,0); // update status bar
                  refreshstatus = 0;
                }
		if(kbhit() !=0){    /* Wait for Keystroke */
                   refreshstatus = 1; 
/*

  Low byte of longkey is nonzero for ascii characters. For special
  keys the high byte has a code identifying the key      */

			longkey = _bios_keybrd(_KEYBRD_READ);
			if ((longkey % 256) == 0) key = (longkey >> 8);
			else key = (longkey % 256);

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
                                        buf_pos = 
						LinePositions[lineno-Y+1];
                                        DumpToScreen(&Main_Window);
					Main_Window.rdbuf()->gotoxy(X,Y);
                                        buf_pos=LinePositions[lineno];
					break;
				case LINEFEED:
				case RETURN:
					CurrentFocus = WINDOWFOCUS;
                                        Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
					SubMenu[Main_Menu.CurrentItem].Item[
					  SubMenu[Main_Menu.CurrentItem].CurrentItem].state 
					 = NORMAL;
					 Main_Menu.Item[Main_Menu.CurrentItem].state=NORMAL;
					DisplayMainMenu(&Main_Menu);

					switch(Main_Menu.CurrentItem){
                                          case 0:  /* File Menu */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
						   case 4: /* QUIT PROGRAM */
                                                        if(DirtyFile){
                                                        strcpy(DialogueData.Title,"           No Save");
                                                        strcpy(DialogueData.msg1,"File has changed. Save [y/n]?");
                                                        strcpy(DialogueData.msg2,"<esc> to return to pgm");

                                                        if(MYDialogue()!=0){
                                                                DumpCurrentPage();
                                                                break;
                                                        }
                                                        if(DialogueData.Input[0]=='y'){
                                                                fclose(openfile);
                                                                openfile = fopen(TempFilePath,"w");
                                                                for(i=0;i<buf_end;i++)
                                                                        if(fputc(MainBuffer[i],openfile)==EOF){
                                                                          myerror(&mystatus,"Write error on ",TempFilePath);
                                                                          break;
                                                                        }
                                                                fclose(openfile);
                                                         }
							}
							Main_Window.window(1,1,80,25);
							Main_Window.clrscr();
							Main_Window.rdbuf()->setcursortype(_NORMALCURSOR);
							fclose(openfile);
							return 0;

                                                   case 1: /* SAVE FILE */
							strcpy(DialogueData.Title,"           Save File");
                                                        strcpy(DialogueData.msg1,"Save file [y/n]?");
                                                        strcpy(DialogueData.msg2,"<esc> to cancel");

                                                        if((MYDialogue()==0) && (DialogueData.Input[0] == 'y')){
                                                                fclose(openfile);
                                                                openfile = fopen(TempFilePath,"w");
                                                                for(i=0;i<buf_end;i++)
                                                                        if(fputc(MainBuffer[i],openfile)==EOF){
                                                                          myerror(&mystatus,"Write error on ",TempFilePath);
                                                                          break;
                                                                        }
                                                                fclose(openfile);
                                                                openfile = fopen(TempFilePath,"r");
                                                                DirtyFile = 0;
                                                         }
                                                         DumpCurrentPage();
                                                         break;

						   case 0: /* OPEN FILE */
							strcpy(DialogueData.Title,"           Open a File");
                                                        strcpy(DialogueData.msg1,"Enter Path to File.");
                                                        strcpy(DialogueData.msg2,"Enter to apply, <esc> to cancel");
                                                        if(MYDialogue()==0){
								tempfilehold=openfile;
								 if((openfile = fopen(DialogueData.Input,"r"))==NULL){
								 openfile = tempfilehold;
								  myerror(&mystatus,"Can't Open ",DialogueData.Input);
								  Status(&Status_Bar,&mystatus,0);
								  DumpCurrentPage();
								 break;
								 }
							fclose(tempfilehold);  // succeeded
							 strcpy(mystatus.OpenFile,DialogueData.Input);
                                                         strcpy(TempFilePath,DialogueData.Input);
							 Status(&Status_Bar,&mystatus,0);
							FirstScreen(openfile,&Main_Window);
						       }
						       else {
							DumpCurrentPage();   /* redraw */
							}
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
											Status(&Status_Bar,&mystatus,0);
											DumpCurrentPage();
											break;
											}
											break;
										  case 'a':
										     
										       if( (tempfilehold = fopen(buffer,"a"))==NULL){
											myerror(&mystatus,"Unable to open: ",buffer);
											Status(&Status_Bar,&mystatus,0);
											DumpCurrentPage();
											break;
											}
											break;
										  default: 
											myerror(&mystatus,"Unrecognized response: ",DialogueData.Input);
											Status(&Status_Bar,&mystatus,0);
											DumpCurrentPage();
											break;
										 }      // end switch
										       // Now copy the file
                                                                        ptr=MainBuffer;
                                                                        while((c=*ptr++)!='\0')
                                                                         fputc(c,tempfilehold);
									fclose(tempfilehold);
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
								  Status(&Status_Bar,&mystatus,0);
								  DumpCurrentPage();
								 break;
								 }

						   // OK, we have a handle
						   // for a new file
								      // open stream to it
						       tempfilehold = fdopen(handle,"w");
                                                       ptr = MainBuffer;
                                                       while((c=*ptr++)!='\0')
							fputc(c,tempfilehold);
						       fclose(tempfilehold);
                                             
						       }
							DumpCurrentPage();   /* redraw */
							break;
						case 3:    /* Print the open file */
                                                     
							strcpy(DialogueData.Title,"       Printer Setup");
							strcpy(DialogueData.msg1,"Enter n for normal, l for landscape.");
							strcpy(DialogueData.msg2,"<Ret> to apply, <esc> to cancel");
                                                        if(MYDialogue()==0){
							if(DialogueData.Input[0]=='l')
                                                                PrintOut(MainBuffer,1);
                                                        else PrintOut(MainBuffer,0); }
							 DumpCurrentPage();
							 break; 
							default:       // default case of file submenu 
							break; }
							 break;
				 case 1:  /* Edit MENU */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                    case 0:
                                           menu10(openfile);
                                           break;
                                    case 1: 
                                           menu11(openfile);
                                           break;

                                    case 2:
                                           menu12(&openfile);
                                           break;
                                    case 3:
                                           menu13(openfile);
                                           break;
                                    case 4:
                                           menu14(openfile);
                                           break;

                                    default:
                                           DumpCurrentPage();
                                           break;
                                    }
                                    break;
						
                            case 2: 

                               switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){ 
                                     case 0:
                                          menu20();
                                          break;
                                     case 1:
                                          menu21(&openfile);
                                          break;

                                     case 2:
                                          menu22();
                                          break;

                                     case 3:
                                          menu23();
                                          break;

                                     case 4:
                                          menu24();
                                          break;
                                     default:
                                          DumpCurrentPage();
                                          break;
                                    }
                                    break;
                             case 3: 
                                    switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                      case 0:
                                          menu30();
                                          break;

                                      case 1:
                                          menu31();
                                          break;
                                      case 2:
                                          menu32(openfile,openfile);
                                          break;
                                      case 3:  
                                          menu33();
                                          break;
                                      case 4:
                                          menu34();
                                          break;
                                      default:
                                          DumpCurrentPage();
                                          break;         
					  }
					  break;
                                          case 4:  /* Help */
						switch(SubMenu[Main_Menu.CurrentItem].CurrentItem){
                                                   case 0:
                                                        fclose(openfile);
                                                        openfile = fopen(INFO,"r");
                                                        strcpy(mystatus.OpenFile,INFO);
                                                        FirstScreen(openfile,&Main_Window);
						     break;

                                                   case 1:
                                                        fclose(openfile);
                                                        openfile = fopen(MAN,"r");
                                                        strcpy(mystatus.OpenFile,MAN);
                                                        FirstScreen(openfile,&Main_Window);

						     break;

                                                   case 2:
                                                      menu42();
                                                      break;
                                                   case 3:
                                                      menu43();
                                                      break;
                                                   case 4:
                                                      menu44();
                                                      break;
						   default:
                                                     
                                                     DumpCurrentPage();
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

                                // The values of these keys collide with
                                // u and w after tranlation from bios longkey
                                // Also there are a number of these collisions with
                                // the shift key pressed. e.g. H is up arrow. The
                                // bioskey command tests for shift press.

                                if(InsertMode && ((key == CONTROL_END) ||
                                   (key == CONTROL_HOME) || (bioskey(2) & 0x03))){
                                        Insert(key);
                                        break;
                                }
				switch(key){
				case INSERT:
					InsertMode = 1;
                                        DirtyFile = 1;
					Main_Window.rdbuf()->setcursortype(_SOLIDCURSOR);
                                        break;
				case DELETE:
                                        DirtyFile = 1;
					if(Delete() == 1)
						printf("\a");
					break;
                                case BACKSPACE:
                                        DirtyFile = 1;
                                        if(BackSpace() == 1)
                                             printf("\a"); /* beep */
                                        break;
				case ESCAPE:
                                        if(AlterFlag){
						ENDEDIT
                                        }
					Main_Menu.Item[Main_Menu.CurrentItem].state 
					 = SELECTED;
					CurrentFocus = MENUFOCUS;
					Main_Window.rdbuf()->setcursortype(_NOCURSOR);
					DisplayMainMenu(&Main_Menu);
					break;

				case UPARROW:
                                      
                                        if(AlterFlag){
						ENDEDIT
                                        }

					if(Y==1){  /* cursor at top */
						if(lineno == 0) 
							printf("\a");
						else {
                                                        buf_pos = LinePositions[--lineno];
                                                        Main_Window.rdbuf()->gotoxy(1,1);

                                          /* We  use the scroll function (implemented in scroll.asm)
                                             which uses bios video services for better performance */

                                                        scroll((char)SCROLL_DOWN,(char)1,2);
                                                        ptr=bufgets(LineBuf,LINELENGTH,MainBuffer,buf_pos,buf_end);
                                                        n = strlen(LineBuf);
                                                        if(*(ptr+n-1)=='\n')i=n-1;
                                                        else i = n;  // don't clobber last char if not a newline
                                                        *(ptr+i)='\0';
                                                        if(n==1)strcat(LineBuf," "); // can't put empty line on screen
                                                        ( Main_Window <<= LineBuf );
                                                        Main_Window.rdbuf()->gotoxy(X,1);
                                                        buf_pos += Offset(lineno,X);
						      }
					}
					else {
                                                Main_Window.rdbuf()->gotoxy(X,--Y);
						--lineno;
                                                buf_pos = LinePositions[lineno] + Offset(lineno,X);
         
					}
					break;
				case DOWNARROW:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

					if(Y==SCREENLINES){  /* cursor at bottom */
						if(lineno == TotalLines-1) 
							printf("\a");
						else {
                                                        buf_pos = LinePositions[++lineno];
                                                        Main_Window.rdbuf()->gotoxy(1,SCREENLINES);
                                                        scroll((char)SCROLL_UP,(char)1,2);
                                                        ptr=bufgets(LineBuf,LINELENGTH,MainBuffer,buf_pos,buf_end);
                                                        n = strlen(LineBuf);
                                                        if(*(ptr+n-1)=='\n')i=n-1;
                                                        else i = n;  // don't clobber last char if not a newline
                                                        *(ptr+i)='\0';
                                                        if(n==1)strcat(LineBuf," "); // can't put empty line on screen
                                                        ( Main_Window <<= LineBuf );
                                                        Main_Window.rdbuf()->gotoxy(X,SCREENLINES);
                                                        buf_pos += Offset(lineno,X);
						      }
					}
					else {
						if(lineno== TotalLines-1) {
							printf("\a");
							break;
						}
                                             Main_Window.rdbuf()->gotoxy(1,++Y);
                                             ++lineno;
                                             buf_pos = LinePositions[lineno] + Offset(lineno,X);
					}
					break;
				case CONTROL_HOME:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

                                                 buf_pos = LinePositions[0];
						 lineno=0;
						 X=Y=1;
                                                 DumpToScreen(&Main_Window);
						 Main_Window.rdbuf()->gotoxy(X,Y);
						 break;
				case CONTROL_END:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

                                                 if(TotalLines < SCREENLINES)buf_pos = 0;
                                                 else
                                                   buf_pos = LinePositions[TotalLines-SCREENLINES];
						 lineno=TotalLines-1;
						 X=1;
                                                 DumpToScreen(&Main_Window);
                                                 if(TotalLines < SCREENLINES)Y=TotalLines;
						 else Y=SCREENLINES;
						 Main_Window.rdbuf()->gotoxy(X,Y);
                                                 buf_pos = LinePositions[TotalLines-1];
						 break;
				case PAGEDOWN:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

						 if((lineno + SCREENLINES+1-Y) > TotalLines-1)
							printf("\a");
						 else {
                                                        Main_Window.clrscr();
                                                        buf_pos = LinePositions[lineno+SCREENLINES+1-Y];
                                                        DumpToScreen(&Main_Window);
							if((lineno+SCREENLINES) <= TotalLines-1){
								lineno += SCREENLINES;
                                                                buf_pos = LinePositions[lineno] + Offset(lineno,X);
								Main_Window.rdbuf()->gotoxy(X,Y);
							}
							else {
								Y += (TotalLines-lineno-SCREENLINES - 1);
								lineno = TotalLines-1;
                                                                buf_pos = LinePositions[lineno] + Offset(lineno,X);
								Main_Window.rdbuf()->gotoxy(X,Y);
							      }
						 }
						 break;
				case PAGEUP:

                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

						 if(( lineno - SCREENLINES + 1 - Y ) <= 0)
							if((lineno-SCREENLINES)<0)
							  printf("\a");
							else {
                                                          buf_pos = LinePositions[0];
                                                          DumpToScreen(&Main_Window);
							  lineno = Y-1;
							  Main_Window.rdbuf()->gotoxy(X,Y);
                                                          buf_pos = LinePositions[lineno] + Offset(lineno,X);
                          
							}
						 else {
                                                        buf_pos = LinePositions[lineno-SCREENLINES+1-Y];
                                                        DumpToScreen(&Main_Window);
							lineno -= SCREENLINES;
                                                        buf_pos = LinePositions[lineno] + Offset(lineno,X);
							Main_Window.rdbuf()->gotoxy(X,Y);
						 }
						 break;

				case RIGHTARROW:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

                                                 if(buf_pos >= buf_end-1){
                                                       printf("\a");
                                                       break;
                                                 }
                                                 if(lineno == TotalLines-1){
                                                   X = LogicalX(MainBuffer+LinePositions[lineno],++buf_pos-LinePositions[lineno]);
                                                   Main_Window.rdbuf()->gotoxy(X,Y);
                                                   break;
                                                 }
                                                 if(X < LogicalX(MainBuffer+LinePositions[lineno],LinePositions[lineno+1]-
                                                      LinePositions[lineno]-2)){
                                                   X = LogicalX(MainBuffer+LinePositions[lineno],++buf_pos - LinePositions[lineno]);
                                                   Main_Window.rdbuf()->gotoxy(X,Y);
                                                 }
						 else { X=1; /* code for down arrow here */
							if(Y==SCREENLINES){  /* cursor at bottom */
								if(lineno == TotalLines-1) 
								     printf("\a");
								else {
                                                                        buf_pos = LinePositions[++lineno];
                                                                        Main_Window.rdbuf()->gotoxy(X,SCREENLINES);
                                                                        scroll((char)SCROLL_UP,(char)1,2);
                                                                        ptr=bufgets(LineBuf,LINELENGTH,MainBuffer,buf_pos,buf_end);
                                                                        n = strlen(LineBuf);
                                                                        if(*(ptr+n-1)=='\n')i=n-1;
                                                                        else i = n;  // don't clobber last char if not a newline
                                                                        *(ptr+i)='\0';
                                                                        if(n==1)strcat(LineBuf," "); // can't put empty line on screen
                                                                        ( Main_Window <<= LineBuf );
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
                                                                 buf_pos = LinePositions[lineno];
							  }
						   }
						   break;
                                case END:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                       

                                        if(lineno >= TotalLines-1)
                                                buf_pos = buf_end - 1;
                                        else buf_pos = LinePositions[lineno+1]-1;
                                        if(buf_pos-LinePositions[lineno] ==0)X=1;
                                        else
                                        X = LogicalX(MainBuffer+LinePositions[lineno],(buf_pos - LinePositions[lineno])) -1;
                                        Main_Window.rdbuf()->gotoxy(X,Y);
                                        break;
                                 case HOME:
					if(AlterFlag){
						ENDEDIT
                                        }
                                        

                                        buf_pos = LinePositions[lineno];
                                        X=1;
                                        Main_Window.rdbuf()->gotoxy(X,Y);
                                        break;

				case  LEFTARROW:
                                        if(AlterFlag){
						ENDEDIT
                                        }
                                        

                                        if(X > 1){
                                           X = LogicalX(MainBuffer+LinePositions[lineno],(--buf_pos - LinePositions[lineno]));
                                           Main_Window.rdbuf()->gotoxy(X,Y);

                                        }
                                        else {
						/* code for uparrow here */
						if(Y==1){  /* cursor at top */
						    if(lineno == 0) 
								printf("\a");
							else {
                                                              buf_pos--;
                                                              lineno--;
                                                              if(buf_pos - LinePositions[lineno] == 0)X=1;
                                                              else
                                                              X = LogicalX(MainBuffer+LinePositions[lineno],buf_pos-LinePositions[lineno])-1;
                                                              scroll((char)SCROLL_DOWN,(char)1,2);
                                                              ptr=bufgets(LineBuf,LINELENGTH,MainBuffer,buf_pos,buf_end);
                                                              n = strlen(LineBuf);
                                                              if(*(ptr+n-1)=='\n')i=n-1;
                                                              else i = n;  // don't clobber last char if not a newline
                                                              *(ptr+i)='\0';
                                                              if(n==1)strcat(LineBuf," "); // can't put empty line on screen
                                                             ( Main_Window <<= LineBuf );
                                                             Main_Window.rdbuf()->gotoxy(X,1);
							 }
					         }
                                                 else {
                                                       --lineno;
                                                       buf_pos--;
                                                       if(buf_pos - LinePositions[lineno] == 0)X=1;
                                                       else
                                                       X = LogicalX(MainBuffer+LinePositions[lineno],buf_pos-LinePositions[lineno]) -1;
						       Main_Window.rdbuf()->gotoxy(X,--Y);
					          }
				      }
				      break;

				default:
					if(InsertMode)Insert(key);
					else
					  printf("\a");  /* beep */
					break;
				}     // end switch
			break;    // end case ( Window focus )
			default: ;
		    }   // end focus switch
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
                2,80,2);
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


/* Dump a screenfull of data from Main buffer. 
   Leave the
   buffer position at the same point as when called. Word wrap. 
   Returns number of lines on screen.
*/

int DumpToScreen( constream *output){

	
        unsigned int start;
	char *CharPtr;
        int line=0;
        int i,n;

        start = buf_pos;          // record starting position
        output->rdbuf()->gotoxy(1,1);
        *output << setattr((BLUE << 4) | WHITE);
   /*   output->clrscr();    */
        while(line <= SCREENLINES - 1){

	/* Get the next chunk of line into buffer */

           CharPtr = bufgets(LineBuf,LINELENGTH,MainBuffer,start,buf_end);
	   n = strlen(LineBuf);
           start+= (unsigned)n;

	   if(CharPtr != NULL){
             if(*(CharPtr+n-1)=='\n')i=n-1;
             else i = n;  // don't clobber last char if not a newline
             *(CharPtr+i)='\0'; 
             ( *output <<= LineBuf );
             output->rdbuf()->clreol();

             if(line == SCREENLINES -1) output->rdbuf()->gotoxy(1,line+1);
             else output->rdbuf()->gotoxy(1,line+2);
             line++;
	     }
	     else {
		    /* at EOF */
                line--;
		break;
	     }
	}
        return line;
}

/* Same as fgets, except it reads from memory. Returns null if
   read reaches end of buffer pointed to by bp. The size of the buffer
   is passed as the last argument , and the offset from the start
   to begin reading is passed as start. Reads at most n "logical"
   characters. This may be different than the actual number because of
   Tabs. See also the LogicalX routine below. */

char *bufgets(char *s, int n, const char *bp, unsigned start, unsigned size)
{
      
        int i = 0;  // actual number of characters copied
        int j=0;    // index on "logical" line, allowing for tab expansion
        int tab=0;    // number of last tab

	char c;
        if(start >= size)return NULL;   // "EOF"
        while((j<n)&&(start<size)){
                c = *(bp+start++);
		*(s+i)=c;
                if(c=='\t'){
                     if(j >= TabStops[MAXTABS -2])break;
                     j=TabStops[++tab]-1;
                }
                else {
                        j++;
                        if(j>=TabStops[tab+1]-1)tab++;
                }
		i++;
		if(c=='\n') break;
	}
	*(s+i) = '\0';
	return s;
}

/* Same as bufgets above, except it reads from a file */

char *myfgets(char *s, int n, FILE *fp)
{
      
	int i = 0;
        int j = 0;
        int tab = 0;
	char c;
        while((j<n)&&(feof(fp)==0)){
		c = fgetc(fp);
		*(s+i)=c;
                if(c=='\t'){
                     if(j >= TabStops[MAXTABS -2]){j=LINELENGTH-1;break;}
                     j=TabStops[++tab]-1;
                }
                else {
                        j++;
                        if(j>=TabStops[tab+1]-1)tab++;
                }
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
/* calling with redraw = 1 forces a redraw of the status bar */

void Status(constream *win, struct StatusInfo *mystatus,int redraw)
{
        int i,j,n;
        static int first = 1;
       	char newbuf[80];
        static char oldbuf[80];

        if(first){                  /* first call: initialize */
                newbuf[79]='\0';
                for(i=0;i<79;i++)oldbuf[i]=' ';
                oldbuf[79]='\0';
                first=0;
        }

        /* Build new status line in newbuf */

        sprintf(newbuf,"%d:%d ",lineno+1,X);
        j=strlen(newbuf);
        strncat(newbuf,mystatus->OpenFile,79-j);
        j=strlen(newbuf);
        strncat(newbuf,mystatus->Message,79-j);
        j=strlen(newbuf);
        for(i=j;i<79;i++)            /* fill out with blanks */
                *(newbuf + i) = ' ';

        /* skip over stuff at end of status line that hasn't changed */

        for(i=78;i>=0;i--)
                if(oldbuf[i] != newbuf[i]) break;

        if(redraw)i=78;

        /* write out changed initial segment of newbuf and update oldbuf */

        for(j=0;j<=i;j++){
                oldbuf[j]=newbuf[j];
                *win << newbuf[j];
        }
        win->rdbuf()->gotoxy(1,1);   // position for next time
	Main_Window.rdbuf()->gotoxy(X,Y);
	
	return;
}

/* Reads a a newly opened file into memory, sets the line pointers,
    and dumps the first screen */

void FirstScreen(FILE *openfile, constream *Main_Window)
{

        unsigned int start = 0;
        unsigned int bytecount = 0;

        /* Make sure the cursor is turned on */
        Main_Window->rdbuf()->setcursortype(_NORMALCURSOR);
        Main_Window->clrscr();
      /* read through the file and fill the line position array, etc */
	lineno = 0;
        MainBuffer[0] = '\0';

	LinePositions[0]=start;
        while(myfgets(LineBuf,LINELENGTH,openfile) != NULL){
        bytecount += strlen(LineBuf);
        if(bytecount >= MAXBYTES){
                strcpy(mystatus.Message," File truncated--too big!");
		Status(&Status_Bar,&mystatus,0);
                break;
        }
         strcat(MainBuffer,LineBuf);
         start += (unsigned) strlen(LineBuf);
	 LinePositions[++lineno] = start;
	 }

	TotalLines = lineno;   /* number of logical lines in file */
        buf_end = (unsigned) strlen(MainBuffer);
        /* Don't count DOS's end of file marker, if present */
        if(MainBuffer[buf_end-1]==CONTROL_Z)buf_end--;

	// Dump the first screenfull

	rewind(openfile);
        buf_pos = 0;
        lineno =  DumpToScreen(Main_Window) - 1;
        buf_pos = LinePositions[lineno];
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
	unsigned longkey;
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

        unsigned save;

        save = buf_pos;
        buf_pos = LinePositions[lineno-Y+1]; // top of current page
        DumpToScreen(&Main_Window);
        buf_pos = save;
	Main_Window.rdbuf()->gotoxy(X,Y);
	return;
}

/* This should only be used to extract entire lines. Its function
   is handle the tab character when printing to the screen.
*/

ostream& operator <<= (ostream &os, char *InputLine) {
	char curr;
	int count =0;
	char *ptr;
        int tab = 0;
        int n,i;

        ptr = InputLine;

	buffer[0]='\0';
	while((curr = *ptr++)!= '\0'){
                if(curr == '\t'){
                   /* How many spaces to get to next tab stop? */
                        n = TabStops[++tab] - strlen(buffer);
                        for(i=1;i<n;i++)strcat(buffer," ");
                }
		else {
			count = strlen(buffer);
                  /* If we have reached next tab stop, increase tab number */
                        if(count >= TabStops[tab+1])tab++;
			*(buffer + count) = curr;
			*(buffer + count + 1) ='\0';
		}
	}

	 os << buffer;
	 return os;
}

/* Prints the entire MainBuffer on stdprn */

int PrintOut(char *buffer, int setup)
 {

       if(setup == 1) // landscape
    /* Initialize Printer for landscape and CR translation*/
        fprintf(stdprn,Landscape);
       else fprintf(stdprn,Normalprinter);
       fprintf(stdprn,buffer);

       // restore default printer settings
        fprintf(stdprn,Normalprinter);
	return 0;
}

/* LogicalX: computes position of cursor on screen  from position
   in char array. This is necessary because of tab expansion */

int LogicalX(char *line,int offset)
{
        int i=0,n=1;
        int tab=0;
        while(i < offset){
                if(*(line+i)=='\t')
                        n = TabStops[++tab];
                else {
                        n++;
                        if(n == TabStops[tab+1])tab++;
                }
                i++;
        }
        return n;
}

/* Offset: The "inverse" of the above Logical X function */

int Offset(int line,int logicalx)
{
        int i=0,n=1;
        int tab=0;
        while(n < logicalx){
                if(MainBuffer[LinePositions[line]+i]=='\t')
                         n = TabStops[++tab];
                else  {
                        n++;
                        if(n == TabStops[tab+1])tab++;
                 }
                 i++;
         }
         return i;
}


/* RefreshLine: print the contents of the string as the current screen
   line. If the line is too long it gets
   cut off, so this is only useful as a quick way of updating the current
   line during delete operations.
*/

void RefreshLine(char *stringbuf)
{

      int m;
      char LocalBuf[2*LINELENGTH + 1];

      strcpy(LocalBuf,stringbuf);
      m = strlen(LocalBuf);

      // if line ends with newline replace with space
      // to avoid writing over the line below.
      if(*(LocalBuf+m-1)=='\n')*(LocalBuf+m-1)=' ';

      Main_Window.rdbuf()->gotoxy(1,Y);  // cursor at start of current line
      Main_Window <<= LocalBuf;
      Main_Window.rdbuf()->clreol();   // clear to end of line
      Main_Window.rdbuf()->gotoxy(X,Y);  // reposition cursor
      return;
}


/* Reset: Resets LinePostions array, buf_end, lineno, sets buf_pos to the
value passed, and sets X and Y appropriately. Also Dumps Current Page */

void Reset( int new_pos )
{
        int i = 0;
        int temp = 0;

       
        LinePositions[0]=0;
        buf_end = (unsigned) strlen(MainBuffer);
        while(bufgets(LineBuf,LINELENGTH,MainBuffer,i,buf_end) != NULL){
         if(LinePositions[temp] <= new_pos)lineno = temp;
         i += (unsigned) strlen(LineBuf);
         LinePositions[++temp] = i;
	 }

        TotalLines = temp;   /* number of logical lines in file */
        /* Don't count DOS's end of file marker, if present */
        if(MainBuffer[buf_end-1]==CONTROL_Z)buf_end--;

        /* Reset cursor positions X and Y */
        /* Y = lineno % SCREENLINES + 1; */
        X = LogicalX(MainBuffer+LinePositions[lineno],new_pos - LinePositions[lineno]);

        /* Redraw */
    
        buf_pos = new_pos;

        DumpCurrentPage();
}
