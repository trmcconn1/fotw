/* browse.c: implementation of a basic filesystem browser.

Layout:

	---------------------------------------------------------
        |     Top_Panel                                          |
	|--------------------------------------------------------|
	|                |                                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	| Left Panel     |     Right Panel                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	|                |                                       |
	----------------------------------------------------------


Called as:  int browse(void *path)
Returns: An integer denoting what happened as defined in browse.h
         If the user made a single selection then the selected path is
         returned via the pointer, which points to a string. If the
         user selected multiple choices, it points to an array of
         strings which must be NULL terminated. 

Current directory path is displayed in top pane. Subdirectories of
current directory are displayed in left pane, files and directories
are displayed in right pane. Left pane is used to navigate (use arrow
keys), right pane is used to select. Move between panes with left/right
arrows. (Escape sequences that define arrow keys are defined in browse.h.)
Hit return on a directory in left pane to select new current directory,
hit return on directory or file in left pane to cause browser to return 
with that selection.

If you hit escape while in right pane you're returned to left pane. If
you hit return while in right pane the browser returns to the caller
with an empty path. (i.e, this is a "cancel").

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <curses.h>
#include <dirent.h>
#include <string.h>
#include "browse.h"  /* browse.h includes <sys/stat.h> */

/* extension for xplore application */
#ifdef XPLORE
#include "edit.h"
#include "history.h"
#define MAX_ASSOCIATIONS 64
#define EXT_ASSOC 0
#define FILE_ASSOC 1
#define PSUEDO_FILE 2

#define min(a,b) ((a) > (b) ? (b) : (a))
#define max(a,b) ((a) > (b) ? (a) : (b))

struct association {
int type;
char name[MAX_NAME];
char value[MAX_NAME];
};
extern struct association assocs[MAX_ASSOCIATIONS];
extern int nassocs;
#endif

/* Global variables */

static char CurrentPath[MAX_PATH]; /* The current directory, and what shows
					in top pane */
static char pathbuf[MAX_PATH];
static int CurPane;   /* Left or Right */
static char **LeftStrings;  /* Array of strings displayed in panes */
static char **RightStrings;
static int LeftNStrings;   /* Numbers of strings displayed in panes */
static int RightNStrings;
static int RightSelected = -1;  /* Setting these to -1 suppresses highlighting */
static int LeftSelected;        /*  of current selection. */
static int VLeftTop;    /* Top visible string index. Incrementing this
                             scrolls down. */
static int VRightLeft;  /* Leftmost visible column in right pane.
                           Incrementing this scrolls right. */
static int AnchorOn = 0; /* Toggle for multiple selection */
static int AnchorIndex;  /* Records where multiple selection started */
static	WINDOW *box_top,*box_left,*box_right;
static  WINDOW *top_pane,*left_pane, *right_pane;

/* Subroutines */

char **LoadStrings(char *path, int,int *,int);
void FreeLeft(void);
void FreeRight(void);
void DisplayLeft();
void DisplayRight();
void DisplayTop(char *);
void UpdateSelected();
int my_getch();
int my_chdir(char *);
int compare_alpha(char **, char **);

int
browse(void *path, char *title)
{

	int i,j,k;
	int c;
	int hold;
	int rval;
	int CurPane = NLEFT_PANEL;
	char *p;

	clear();  /* clear crap off screen */
	AnchorOn = 0; /* make sure multiple selection is turned off */

#ifdef _MINIX
	/* Unfortunately, this is useless because the minix terminal
           lacks the vi, vs and ve capabilities which change the
           appearance of the cursor */
	keypad(stdscr,TRUE);
	curs_set(0);
#else
	leaveok(stdscr,TRUE); /* Get rid of cursor. I don't understand why
                                 it works, but it seems to. */
#endif
	strcpy(CurrentPath,(char *)path);

        /* Normalize initial path so it ends in /. */ 

	if(CurrentPath[strlen(CurrentPath)-1]!='/')
		strcat(CurrentPath,"/");

	
	/* Load in directories and files in current directory, which will
           be displayed in the left and right panes. It is a show-stopper
           if this fails */
	if(!(LeftStrings = LoadStrings(CurrentPath,S_IFDIR,&LeftNStrings,NLEFT_PANEL)))return BROWSER_ERROR;
	if(!(RightStrings = LoadStrings(CurrentPath,S_IFREG|S_IFDIR|S_IFIFO,&RightNStrings,NRIGHT_PANEL)))return BROWSER_ERROR;

	RightSelected = -1;  /* Highlight nothing in right pane */
	LeftSelected = 1;    /* Highlight second entry (..) in left pane */

	/* Create the enclosing windows and outline them. This has
           no visible effect if colors are being used. */

	box_top = subwin(stdscr,TOP_DY,TOP_DX,TOP_Y,TOP_X);
	box_left = subwin(stdscr,BOT_DY,LEFT_DX,TOP_Y+TOP_DY,TOP_X);
	box_right = subwin(stdscr,BOT_DY,RIGHT_DX,TOP_Y+TOP_DY,TOP_X+LEFT_DX);

	/* Create the content windows */

	left_pane = subwin(box_left,BOT_DY-2,LEFT_DX-2,TOP_Y+TOP_DY+1,
			TOP_X+1);
	right_pane = subwin(box_right,BOT_DY-2,RIGHT_DX-2,TOP_Y+TOP_DY+1,
			TOP_X+LEFT_DX+1);
	top_pane = subwin(box_top,TOP_DY-2,TOP_DX-2,TOP_Y+1,TOP_X+1);

/* A convenient macro */
#define CLEAN   delwin(top_pane);\
		delwin(left_pane);\
		delwin(right_pane);\
		delwin(box_top);\
		delwin(box_left);\
		delwin(box_right);\
		FreeLeft();\
		FreeRight();

#ifdef NCURSES 
	keypad(stdscr,TRUE);
	/* fill panels with blanks so colors will have effect */
	if(has_colors()){
		wattron(box_left,COLOR_PAIR(LEFT_PAIR));
		for(i=0;i<BOT_DY;i++){
			wmove(box_left,i,0);
			for(j=0;j<LEFT_DX;j++)
				waddch(box_left,' ');
		}
		wattron(box_right,COLOR_PAIR(RIGHT_PAIR));
		for(i=0;i<BOT_DY;i++){
			wmove(box_right,i,0);
			for(j=0;j<RIGHT_DX;j++)
				waddch(box_right,' ');
		}
		wattron(box_top,COLOR_PAIR(TOP_PAIR));

		for(i=0;i<TOP_DY;i++){
			wmove(box_top,i,0);
			for(j=0;j<TOP_DX;j++)
				waddch(box_top,' ');
		}
	}
	else { /* draw boxes around if colors are not being used */
		box(box_top,VERT,HOR);
		box(box_left,VERT,HOR);
		box(box_right,VERT,HOR);
	}
#else
	box(box_top,VERT,HOR);
	box(box_left,VERT,HOR);
	box(box_right,VERT,HOR);
#endif
	DisplayRight();
	DisplayTop(title);
	refresh();
	DisplayLeft(); 

	/* Normally, this loop is exited by a return to the caller
           after the user hits return when CurPane = NRIGHT_PANEL */

	while((c = my_getch())!= EOF){
		if(c == ''){  /* redraw screen */
				touchwin(stdscr);
				refresh();
				continue;
		}
		if(CurPane == NLEFT_PANEL){
			switch(c) {

				case UNUSED_CHAR:
					continue;
				case 'q':
				case '':
				case '':
					/* return original empty string to
                                           caller. User cancels. */
					rval = BROWSER_EXIT;
					break;

				case '\r':
				case '\n': 
					UpdateSelected(); /* Called just to
                                                             renormalize path */
					rval = BROWSER_LSEL;
					break;
#ifdef XPLORE
				case 'f':
				case 'F':
					rval = BROWSER_FWD;
					break;
				case 'b':
				case 'B':
					rval = BROWSER_BACK;
					break;
#endif
#ifdef _DEBUG
				case 'd':
				case 'D':
					rval = BROWSER_DUMP;
					break;
#endif

				case 'j':
				case DOWN_CODE:  /* Move selection down */
				case '\t':
					if(LeftSelected < LeftNStrings - 1){
					   LeftSelected++;
					   if(LeftSelected-VLeftTop >= BOT_DY-2)
						VLeftTop++; /* scroll */	
					   DisplayLeft();
					}
					else { /* already at bottom */
						beep();
						wrefresh(left_pane);
					}
					continue;
				case 'k':
				case UP_CODE:  /* Move selection up */
				case '':
				case '\x7F':
					/* If at visible top and below actual
                                           top, scroll up */
					if(LeftSelected <= VLeftTop)
						if(VLeftTop > 0)VLeftTop--;
						else {
							beep();
							wrefresh(left_pane);
							break;
						}
					LeftSelected--;
					DisplayLeft();
					continue;
				case 'l':
				case RIGHT_CODE: /* Move into right pane.
                                         Store current selection in left
                                         pane so it can be restored when
                                         focus returns. */
					CurPane = NRIGHT_PANEL;
					RightSelected = 0;
					hold = LeftSelected;
					LeftSelected = -1;
					DisplayLeft();
					DisplayRight();
					continue;
				case 'a':
				case 'A':
				case '':
					/* move to right pane and select
                                           all there */
					CurPane = NRIGHT_PANEL;
					RightSelected = RightNStrings - 1;
					AnchorOn = 1;
					AnchorIndex = 0;
					hold = LeftSelected;
					LeftSelected = -1;
					DisplayLeft();
					DisplayRight();
					continue;
				default: /* No other characters are 
                                            meaningful */
					beep();
					wrefresh(left_pane);
					continue;
			} /* end switch */
			strcpy(path,CurrentPath);
			CLEAN
			return rval;
		}
		else 
			switch(c) {

				case UNUSED_CHAR:
					break;

			        case 'm': /* user wants to exec selection and
                                           pipe output through more */
				case 'M':
				case '\r':
				case '\n': /* User wants to go home.
						Tack current selection onto
                                                CurrentPath and return it.
                                            */

					if(AnchorOn){
						char **ptr = (char **)path;

						for(i=min(RightSelected,AnchorIndex);
						    i<=max(RightSelected,AnchorIndex);
						    i++){
							*ptr = (char *)malloc(strlen(CurrentPath)
							   + strlen(RightStrings[i]+1)+1);
							if(!*ptr)
								fatal("Cannot malloc\n");
							strcpy(*ptr,CurrentPath);
							strcat(*ptr++,strdup((RightStrings[i])+1));
						}
						*ptr = NULL;	
						rval = BROWSER_MULTIPLE;	
					}
					else {
						strcat(CurrentPath,
                                          		(RightStrings[RightSelected])+1);
						strcpy(path,CurrentPath);
						if((c == 'm')||(c == 'M'))
							rval = BROWSER_MORE;
						else
						 	rval = BROWSER_RSEL;
					}
					CLEAN
					return rval;
				case 'e':
				case 'E':
					  /* User wants to force an edit
                                             of selection */
					strcat(CurrentPath,
                                          (RightStrings[RightSelected])+1);
					CLEAN
					strcpy(path,CurrentPath);
					return BROWSER_EDIT;
				case '':  /* Escape back to left pane */
					CurPane = NLEFT_PANEL;
					RightSelected = -1;
					/* Remember what was selected there */
					LeftSelected = hold;
					/* Turn selection off */
					AnchorOn = 0;
					DisplayRight();
					DisplayLeft();
					break;
				case 'j':
				case DOWN_CODE: /* move selection down */
				case '\t': /* scroll right if necessary */
					if(RightSelected < RightNStrings - 1){
						RightSelected++;
						if(RightSelected - VRightLeft*(BOT_DY-2)
							>= RIGHT_COLS*(BOT_DY-2))
							VRightLeft++; /* scroll */
						DisplayRight();
					}
					else {
						beep();
						wrefresh(right_pane);
					}
					break;
				case 'k':
				case UP_CODE:
				case '':   /* move selection up. scroll */
				case '\x7F': /* left if necessary */
					if(RightSelected <= VRightLeft*(BOT_DY-2))
					   if(VRightLeft > 0)VRightLeft--;
					   else {
						beep();
						wrefresh(right_pane);
						break;
					   }
					RightSelected--;
					DisplayRight();
					break;
				case 'h':
				case LEFT_CODE: /* move left a whole column */
					        /* scroll left if necessary */
					if(RightSelected < (VRightLeft+1)*(BOT_DY-2)){
					   if(VRightLeft){
						VRightLeft--;
						RightSelected -= (BOT_DY-2);
					   }
                                         /* If in first col, move back into
                                            left pane */
					   else {
						CurPane=NLEFT_PANEL;
						RightSelected = -1;
						LeftSelected = hold;
						DisplayRight();
						DisplayLeft();
						break;
					   }
					}
					else RightSelected -= (BOT_DY-2);
					DisplayRight();
					break;	
				case 'l':
				case RIGHT_CODE: /* move right a whole column */
					if(RightSelected +BOT_DY-2 < RightNStrings - 1){
						RightSelected += (BOT_DY-2);
						if(RightSelected - VRightLeft*(BOT_DY-2)
							>= RIGHT_COLS*(BOT_DY-2))
							VRightLeft++; /* scroll */
						DisplayRight();
					}
					else {
						beep();
						wrefresh(right_pane);
					}
					break;
				case 'p': /* print selected file */ 
				case 'P':
					p = malloc(strlen(CurrentPath)
						+strlen(RightStrings[RightSelected]+1)+10);
					/* Don't print unless it's an ordinary
                                            file */
					if(RightStrings[RightSelected][0] == NORMAL_TAG){
						sprintf(p,"%s %s%s",LPR,CurrentPath
							,(RightStrings[RightSelected])+1);
						system(p);
					}
					else {
						beep();
						wrefresh(right_pane);
					}
					free(p);
					/* Turn selection off */
					AnchorOn = 0;
					break;
				case '.':  /* here we implement highlighting
                                              for multiple selection */
					if(AnchorOn)
						AnchorOn = 0;
					else {
						AnchorOn = 1;
						AnchorIndex = RightSelected;
					}
					DisplayRight();
					break;
				case 'a': /* select/unselect all */
				case 'A':
				case '':
					if(AnchorOn)
						AnchorOn = 0;
					else {
						AnchorOn = 1;
						AnchorIndex = 0;
						RightSelected = RightNStrings - 1;
					}
					DisplayRight();
					break;
				default:
					beep();
					wrefresh(right_pane);
					break;
			}
	}
	
	/* Normally, we don't come here */
	CLEAN
	return BROWSER_ERROR;	
}


/* Stat the path. It must be a directory. If so, allocate space for
 array and fill it with the filenames in this directory whose 
 mode matches the flag */

char ** LoadStrings(char *path,int flag,int *nstrings,int panel)
{

	int i,n;
	DIR *my_dir;
	struct dirent *my_dirent;
	struct stat my_stat;
	char buffer[MAX_PATH];
	char **parray;
	int count = 0;

	if(chdir(path)==-1)return NULL;

	if((my_dir = opendir(path))==NULL)return NULL;

	while(my_dirent = readdir(my_dir)){
			if(stat(my_dirent->d_name,&my_stat)==-1)continue;
			if(flag&my_stat.st_mode)count++;
	}

/* extension to interface with xplore application */
#ifdef XPLORE 
	/* Add in count of psuedo-file associations */
	if(panel == NRIGHT_PANEL){
		n=0;
		for(i=0;i<nassocs;i++)
			if(assocs[i].type == PSUEDO_FILE)n++;
		count += n;
	}
#endif
	
	parray = (char **)malloc(count * sizeof(char *));
	if(!parray){
		closedir(my_dir);
		return NULL;
	}

	
	rewinddir(my_dir);
	count = 0;
	while(my_dirent = readdir(my_dir)){
#ifdef S_ISLNK
		if(lstat(my_dirent->d_name,&my_stat)==-1)continue;
#else
		if(stat(my_dirent->d_name,&my_stat)==-1)continue;
#endif
		if(!(flag&my_stat.st_mode))continue;
		n = strlen(my_dirent->d_name);
		parray[count] = (char *)malloc(n*sizeof(char)+3);
		if(parray[count]==NULL){
			closedir(my_dir);
			return NULL;			
		}
		strcpy((parray[count])+1,my_dirent->d_name);
		parray[count][0]=NORMAL_TAG;

		/* Store a special character recording what type of
                   file as the first character of the filename string.
                   This is a kludge */
		if(IS_DIRECTORY(my_stat.st_mode))parray[count][0]=DIR_TAG;
		else
		if(IS_EXECUTABLE(my_stat.st_mode))parray[count][0]=EXEC_TAG;
		
		if(IS_LINK(my_stat.st_mode))parray[count][0]=LINK_TAG;
		else
		if(IS_SPECIAL(my_stat.st_mode))parray[count][0] = SPECIAL_TAG;
		else
		if(IS_FIFO(my_stat.st_mode))parray[count][0] = FIFO_TAG;
		count++;
	}

#ifdef XPLORE
	if(panel == NRIGHT_PANEL)
		for(i=0;i<nassocs;i++)
			if(assocs[i].type == PSUEDO_FILE){
				n = strlen(assocs[i].name);
				parray[count] = (char *)malloc(n*sizeof(char)+2);
				parray[count][0] = ' ';
				strcpy((parray[count++])+1,assocs[i].name);
			}
#endif
	*nstrings = count;

	/* sort the array alphabetically */
	qsort((void *)parray,count,sizeof(char *),
	  (int(*)(const void *,const void*))compare_alpha);
	closedir(my_dir);
	return parray;		
}

/* Display strings in LeftStrings for as many as fit.
*/

void
DisplayLeft()
{
	int i,j;
	int cy,cx;
	char buffer[LEFT_DX-1];

	buffer[LEFT_DX-2]='\0';

#ifdef NCURSES
	wattron(left_pane,COLOR_PAIR(LEFT_PAIR));
#endif
	for(i=0;i<BOT_DY-2;i++){
		wmove(left_pane,i,0);
		for(j=0;j<LEFT_DX-2;j++)
			waddch(left_pane,' ');
	}

	
	for(i=VLeftTop,j=0;j<BOT_DY-2;j++,i++){
		if(i>= LeftNStrings)break;
		wmove(left_pane,j,0);
		strncpy(buffer,(LeftStrings[i])+1,LEFT_DX-2);
		if(i==LeftSelected){
			wstandout(left_pane);
			getyx(left_pane,cy,cx);
		}
#ifdef NCURSES
		wattron(left_pane,COLOR_PAIR(LEFT_PAIR)); 
#endif
		waddch(left_pane,' ');
		waddstr(left_pane,buffer);
		wstandend(left_pane);
	}
	wmove(left_pane,cy,cx);
	touchwin(left_pane);
	wrefresh(left_pane);
		
}

/* Make a multicolumn display of RightStrings in the right panel */

void
DisplayRight()
{
	int i,j,k;
	int cy,cx;
	char buffer[RIGHT_MAXLEN+2];

	buffer[0]=' ';
	buffer[RIGHT_MAXLEN+1]='\0';

#ifdef NCURSES
	wattron(right_pane,COLOR_PAIR(RIGHT_PAIR));
#endif
	for(i=0;i<BOT_DY-2;i++){
		wmove(right_pane,i,0);
		for(j=0;j<RIGHT_DX-2;j++)
			waddch(right_pane,' ');
	}

	k = (BOT_DY-2)*VRightLeft;    /* starting index */
	for(j=0;j<RIGHT_COLS;j++){
		for(i=0;i<BOT_DY-2;i++){
			if(k>= RightNStrings)break;
			if(AnchorOn){
				if(k >= min(RightSelected,AnchorIndex))
					wstandout(right_pane);
				if(k > max(RightSelected,AnchorIndex))
					wstandend(right_pane);
			}
			else 
				if(k > RightSelected)
					wstandend(right_pane);
		        wmove(right_pane,i,j*(RIGHT_MAXLEN+1));
			strncpy(buffer+1,(RightStrings[k])+1,RIGHT_MAXLEN);
			if(k==RightSelected){
				wstandout(right_pane);
				getyx(right_pane,cy,cx);
			}
#ifdef NCURSES
			wattron(right_pane,COLOR_PAIR(RIGHT_PAIR));
#endif
			waddstr(right_pane,buffer);
			/* add char to distinguish file type which was
                           stored as the first char of the string */
			waddch(right_pane,RightStrings[k][0]);
			k++;
		}
	}
	wmove(right_pane,cy,cx);
	touchwin(right_pane);
	wrefresh(right_pane);
	wstandend(right_pane); /* so it isn't left on for next call */
}

/* Update current path */

void
UpdateSelected()
{

	if(LeftSelected > 0)
		strcat(CurrentPath,(LeftStrings[LeftSelected])+1);

	/* Normalize path so it ends in a / */
	if(CurrentPath[strlen(CurrentPath)-1]!='/')
	      strcat(CurrentPath,"/");
	return;
}

/* Display Current Path in top panel */
void
DisplayTop(char *txt)
{
	int i,j;

#ifdef NCURSES
	wattron(top_pane,COLOR_PAIR(TOP_PAIR));
#endif
	for(i=0;i<TOP_DY-2;i++){
		wmove(top_pane,i,0);
		for(j=0;j<TOP_DX-2;j++)
			waddch(top_pane,' ');
	}

	wmove(top_pane,0,1);
	waddstr(top_pane,txt);
	touchwin(top_pane);
	wrefresh(top_pane);
}

void
FreeLeft(void)
{

	int i;
	for(i=0;i<LeftNStrings;i++)
		free(LeftStrings[i]);
	free(LeftStrings);
	LeftNStrings = 0;
	return;
}	

void
FreeRight(void)
{
	int i;
	for(i=0;i<RightNStrings;i++)
		free(RightStrings[i]);
	free(RightStrings);
	RightNStrings = 0;
	return;
}

/* This routine tries to interpret ANSI escape sequences produced
   by cursor movement keys. Basically, if a character is an escape
   character, then the routine snarfs up all remaining characters in
   non-blocking mode. It then interprets the input as an escape sequence.
*/ 
   
int my_getch()
{

	int c=0;
	int n=0;
	int i;
	int first_try = 1;
	int fl; /* file descriptor flags */
	char buffer[16];
	char *p;


	c = getch();

#ifdef NCURSES

		/* This method has the disadvantage of introducing a
                   big delay if only escape is pressed. */
		switch(c){
			case KEY_UP:
				return UP_CODE;
			case KEY_DOWN:
				return DOWN_CODE;
			case KEY_RIGHT:
				return RIGHT_CODE;
			case KEY_LEFT:
				return LEFT_CODE;
			default:
				return c;
		}
#else
	if(c ==''){

	/* Save current state of stdin flags 
           and set non-blocking mode. In case this escape char starts
           an escape sequence, try to read the rest of it. */

		fl=fcntl(0,F_GETFL);
		fcntl(0,F_SETFL,fl|O_NONBLOCK);

#ifdef _MINIX
		if((buffer[0] = getch()) == EOF){
			fcntl(0,F_SETFL,fl);
			clearerr(stdin);
			return c;
		}
		if((buffer[1] = getch()) == EOF){
			fcntl(0,F_SETFL,fl);
			clearerr(stdin);
			return c;
		}
		buffer[2] = '\0';
#else
			 
try_again:
		p = buffer;
		if(read(0,p++,1)!=1) {
			if(first_try){
				first_try = 0;
				sleep(1);
				goto try_again;
			}
			fcntl(0,F_SETFL,fl);
			return c;
		}
		/* Bug: We rely on escape sequences being 3 characters long
                   here */
		if(read(0,p++,1)!=1){
			fcntl(0,F_SETFL,fl);
			return c;
		}
		*p = '\0';
#endif

		if(strcmp(buffer,UP_ARROW)==0) c = UP_CODE;
		else
		  if(strcmp(buffer,DOWN_ARROW)==0) c = DOWN_CODE;
		  else
		    if(strcmp(buffer,RIGHT_ARROW)==0) c = RIGHT_CODE;
		    else
			if(strcmp(buffer,LEFT_ARROW)==0) c = LEFT_CODE;
			else c = UNUSED_CHAR;
 
		fcntl(0,F_SETFL,fl);
	}
	return c;
#endif
}

/* kludged strcmp for qsort */
int
compare_alpha(char **first, char **second)
{
	return strcmp(*first,*second);
}

/* Our own wrapper for chdir. This prevents an interesting gotcha: if you
   can cd to a directory but cannot read it, then the browser is stuck
   there because it won't have any directory content strings to present
   in the left panel.
*/

int
my_chdir(char *path)
{
	DIR *my_dir;

	if(!(my_dir = opendir(path)))return -1;
	if(!readdir(my_dir)) return -1;
	closedir(my_dir);
	return chdir(path);

}

