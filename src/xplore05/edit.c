
/* Implement a simple line editor for storing user input in the
   given moving window into the given array. Returns \t, \n,  or EOF
   if they are typed. Other chars are added to the array or have other
   purposes depending on the mode (vi cmd or insert).

   An exception is the escape character. It toggles between 
   command and insert mode. In command mode the h and k
   characters move the next insertion point left or right (think: vi). 
   There are also other vi moves implemented.

   In use, text can extend (hidden) beyond both visible right and left borders.
   It is not possible to move cursor left from left edge or right from
   right edge when there are no hidded characters on those ends.
   Displayed text occupies from left edge rightward for cx characters. 
   When cx reaches the maximum limit of the visible window it does not
   increase further. An attempt to move from from right edge or left from
   left edge when there are hidden characters results in a scroll of 8
   characters. (The numbers of +right and -left scrolls are kept in
   nscrolls. 

   A pointer p marks the "insertion point" and tracks the cursor movement.

   If limit is 0, only react to \n,\t,esc, or EOF. All other chars
   are ignored in this case. 

   All editing takes place in a window two lines wide. (Length is set
   by defines in browse.h) The first line has the command that is being
   edited and the second displays the mode (command or insert) and a
   few helpful reminders. 

*/

#include "browse.h"
#include "edit.h"
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

/* These mode titles need to be always the same length */
static char command_mode[]  = 
"Vi cmds (h,k,r,x,^,$,i,ZZ) q = cancel, return to browser                        ";
static char insert_mode[] =
"Insert mode (^[ = esc for cmd or cursor movement mode)                          ";

int my_wgetch(WINDOW *);
static int nscrolls; /* ++ for left, -- for right, should be >= 0  */
static int vx,cx;
static char *p,*q;  /* p is pointer to where next char will go, q is handy */

void scroll_left(WINDOW *mywin, int limit, char *mychars){

	int i,j;

 /* Need to scroll left. We
          scroll 8 chars */
	vx -= 8;
	nscrolls++;
	wmove(mywin,0,0);
	j = strlen(mychars)-8*nscrolls;
	j = (j < limit) ? j : limit;
	cx = j;
	for(i=0;i<j;i++)
		waddch(mywin,mychars[i+8*nscrolls]);
	for(;i<limit;i++)
		waddch(mywin,' ');
	wmove(mywin,0,vx);
}

void scroll_right(WINDOW *mywin,int limit, char *mychars){

	int i;

	wmove(mywin,0,0);
	if(nscrolls > 0){
		for(i=8;i>0;i--)
			waddch(mywin, mychars[p-mychars-i]);
		waddnstr(mywin,p,limit-8);
		vx += 8;
		cx = 8+strlen(p);
		if(cx > limit)cx = limit;
		nscrolls--;
		wmove(mywin,0,vx);
	}
}

/* limit gives the width, in characters, of the visible display
area and mychars points to a buffer that contains characters to display 
(although if the string it contains is longer than limit, only a portion 
of the string will be visible */

int 
line_edit(WINDOW *mywin, int limit, char *mychars)
{
	int c,cy,i,j;
	int commandmode = 0; /* toggle */ 

	/* first, clear the window */

	wmove(mywin,0,0);   
	for(i=0;i<limit;i++)waddch(mywin,' ');
	touchwin(mywin); 
	wrefresh(mywin); 

	/* Display chars now in mychars. Position cursor at
           end, set cx and cy (cy never changes) */

	if(limit){
		/* Write starting mode on mode line */
		wmove(mywin,1,0);
		waddstr(mywin,insert_mode);
		wrefresh(mywin);

		/* put characters already there in window. If there are
                   too many for the width of the window, display the
                   last chunk */

		wmove(mywin,0,0);
		i = strlen(mychars);
		j = i/limit;
		waddstr(mywin,mychars+j*limit);
		nscrolls = j*limit/8;   /* "primordial" scrolls */
		getyx(mywin,cy,vx);  /* see below */
		p = mychars + strlen(mychars);
		cx = vx;
		wrefresh(mywin);

	}

	 /* vx is the position of the cursor in the "visible window",
		   which may be less wide than the data buffer we are inputing
                   into.  */

	/* main loop */

	while((c=my_wgetch(mywin))!=EOF){
		if(commandmode){ /* cursor movement as in vi - sort of */
			switch(c){

		/* back out : cancel all use of the editor */

				case '':
				case 'q':
				case 'Q':

				        return '';

		/* Quit and submit editted line for processing: ZZ */

				case 'Z':
					c = my_wgetch(mywin);
					if(c == 'Z')
						return '\n';
					else break;

		/* Undocumented, but handy */

				case ':':
					c = my_wgetch(mywin);
						if(c == 'w')
							return '\n';
						if(c == 'q')
				        		return '';
						break;
					
		/* The next 2 leave cursor and insertion point the same */

				case 'r': /* replace character under cursor */

					c = wgetch(mywin);
					*p = c;
					waddch(mywin,c);
					wmove(mywin,0,vx);
					break;
					
				case 'x': /* delete char and close up */

					j = strlen(mychars);
					q = p;
					while(q-mychars <= j){
						*q = *(q+1); 
						q++;
					}
					*q = '\0';
					waddnstr(mywin,p,-1); /* don't wrap */
					waddch(mywin,' ');
					wmove(mywin,0,vx);
					if(strlen(mychars) <= limit+8*nscrolls)
						cx--;
					break;

		/* enter insert mode */

				case 'i':
				case 'I':
					wmove(mywin,1,0);
					waddstr(mywin,insert_mode);
					wmove(mywin,0,vx);
					commandmode = 1 - commandmode;
					break;

		/* vi-like cursor movements */

				case '4': /* in case of num lock */
				case LEFT_CODE:
				case 'h':   /* left */
					if( (vx <= 0) && (nscrolls > 0)){
						scroll_right(mywin,limit,mychars);	
					}
					if(vx)wmove(mywin,0,--vx);
					if(p>mychars)p--;
					break;
				case '6':
				case RIGHT_CODE:
				case 'k':   /* right */
					if(vx == limit - 2){
						scroll_left(mywin,limit,mychars);	
					}
					if(p-mychars < strlen(mychars)){
						 p++;
						 wmove(mywin,0,++vx);
					}
					break;
				case '^': /* move to the beginning of line */
					wmove(mywin,0,0);
					waddnstr(mywin,mychars,limit);
					vx = 0;
					cx = strlen(mychars);
					if(cx > limit)cx = limit;
					p = mychars;
					wmove(mywin,0,0);
					break;
				case '$': /* move to end of line */
					wmove(mywin,0,0);
					for(i=0;i<limit;i++)
						waddch(mywin,' ');
					wmove(mywin,0,0);
					i = strlen(mychars);
					j = i/limit;
					waddstr(mywin,mychars+j*limit);
					nscrolls = j*limit/8;   
					getyx(mywin,cy,vx); 
					p = mychars + strlen(mychars);
					cx = vx;
					break;
				default:
					break;
			}

 /* Recall: touchwin is needed when only the cursor has moved */

			touchwin(mywin);
			wrefresh(mywin);
			continue;
		}
		switch(c){ /* Else: not in command mode */

			case '\r':
			case '\n':
			case '\t':
			case '':
				return c;
			case '':   /* undocumented */
			case '':  /* back to command mode */
					commandmode = 1 - commandmode;
					wmove(mywin,1,0);
					waddstr(mywin,command_mode);
					wmove(mywin,0,vx);
					break;
			case '':     /* Back space */
			case '\x7F':
				if(limit==0)return c;
				if(vx <= 0){
					if(nscrolls == 0){
						beep();	
						break;
					}
					else {
						scroll_right(mywin,limit,mychars);	
					}
				}
				j = strlen(mychars);
				if(p-mychars < j){ /* close up */ 
					q = --p;
					while(q-mychars <= j){
						*q = *(q+1); 
						q++;
					}
					*q = '\0';
					wmove(mywin,0,--vx);
					i = 0;
					while(*(p+i)){
						waddch(mywin,*(p+i));
						i++;
					}	
					while(vx + i++ < limit)
						waddch(mywin,' ');	
					wmove(mywin,0,vx);
				}
				else {
					*--p = '\0';
					wmove(mywin,0,--vx);
					waddch(mywin,' ');
					wmove(mywin,0,vx);
				}
				if(strlen(mychars)+8*nscrolls <= limit)cx--;
				break;

			default: /* All other characters */

				if(limit==0)return c;
				if(p-mychars >= MAX_NAME-1){
					beep();
					break;
				}
				if((c >= ' ')&&(c <= 127)){
					if(vx == limit - 2){
						scroll_left(mywin,limit,mychars);	
					}
					if(vx < cx){
				/* Make room for inserted char */
						j = strlen(mychars); 
						for(i=j;i>p-mychars;i--)
					 	  mychars[i] = mychars[i-1];	
						mychars[j+1]='\0';
						for(i=cx-vx;i>0;i--){
						  wmove(mywin,0,vx+i);
						  waddch(mywin,*(p+i));
						}
					}
					*p++ = c;
					wmove(mywin,0,vx);
					waddch(mywin,c);
					if(cx < limit)cx++;
					vx++;
				}
				break;
		}		
		touchwin(mywin);
		wrefresh(mywin);
	}
	return c;
}

/* This routine tries to interpret ANSI escape sequences produced
   by cursor movement keys. Basically, if a character is an escape
   character the routine snarfs up all remaining characters in
   non-blocking mode. It then interprets the input as an escape sequence.

   Browse.c has an almost identical routine 
*/ 
   
int my_wgetch(WINDOW *mywin)
{

	int c=0;
	int n=0;
	int i;
	int first_try = 1;
	int fl; /* file descriptor flags */
	char buffer[16];
	char *p;


	c = wgetch(mywin);



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
				break;
				/* return c; */
		}

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
			return c; /* We just got escape char */
		}
		/* Bug: We rely on escape sequences being 3 characters long
                   here */
		if(read(0,p++,1)!=1){
			fcntl(0,F_SETFL,fl);
			return UNUSED_CHAR;  /* shrug */
		}
		*p = '\0';

#endif
		if((strcmp(buffer,UP_ARROW)==0)||(strcmp(buffer,UP_ARROW_AP)==0)) c = UP_CODE;
		else
		  if((strcmp(buffer,DOWN_ARROW)==0)||(strcmp(buffer,DOWN_ARROW_AP)==0)) c = DOWN_CODE;
		  else
		    if((strcmp(buffer,RIGHT_ARROW)==0)||(strcmp(buffer,RIGHT_ARROW_AP)==0)) c = RIGHT_CODE;
		    else
			if((strcmp(buffer,LEFT_ARROW)==0)||(strcmp(buffer,LEFT_ARROW_AP)==0)) c = LEFT_CODE;
			else c = UNUSED_CHAR;
 
		fcntl(0,F_SETFL,fl);
	}
	return c;
}
