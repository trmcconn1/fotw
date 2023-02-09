/* edit.c , by Terry R. McConnell */

/* Implement a simple line editor for storing user input in the
   given window into the given array. Returns \t, \n, esc, or EOF
   if they are typed. Other chars are added to the array or ignored.

   If limit is 0, only react to \n,\t,esc, or EOF. All other chars
   are ignored in this case. 

   This allows characters to be added and deleted from the end of
   a string, and scrolls window left or right as needed. It does not
   implement cursor movement and insertion */

#include "edit.h"
#include <string.h>
#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

int 
line_edit(WINDOW *mywin, int limit, char *mychars)
{
	int c, cx,cy,vx,i,j;
	int first_char = 0;  /* flag first char entered */
	int first_try = 1;
	char *p,*q;  /* pointer to where next char will go */
	int fl;  /* File descriptor flags for non-blocking IO */
	char buffer[16];

	/* first, clear the window */
	wmove(mywin,0,0);   
	for(i=0;i<limit;i++)waddch(mywin,' ');
	touchwin(mywin);
	wrefresh(mywin);

	/* Display chars now in mychar. Position cursor at
           end, set cx and cy (cy never changes) */

	if(limit){
#ifndef XPLORE   /* xplore doesn't use this feature of line_edit */
		/* If there are already characters in mychars, highlight
                   the window, so user is alerted that typing a character
                   will cause the contents of window to be deleted before
                   the character is added. This is standard behaviour which
                   everybody expects now.
		*/
		if(strlen(mychars)){
			wstandout(mywin);
			first_char = 1;
		}
#endif

		/* put characters already there in window. If there are
                   too many for the width of the window, display the
                   last few. */
		wmove(mywin,0,0);
		i = strlen(mychars);
		j = i/limit;
		waddstr(mywin,mychars+j*limit);
		getyx(mywin,cy,vx);  /* see below */
		p = mychars + strlen(mychars);
		cx = j*limit + vx; /* cx is  1 + count of chars in buffer */

#ifndef XPLORE 
		if(strlen(mychars)) wstandend(mywin);
#endif
		
		wrefresh(mywin);
	}

	 /* vx is the position of the cursor in the "visible window",
		   which may be less wide than the data buffer we are inputing
                   into. vx and cx are used to implement horizontal
                   scrolling */

	while((c=wgetch(mywin))!=EOF){
		switch(c){
			case '\r':
			case '\n':
			case '\t':
#ifdef XPLORE
			case '':
#endif
				return c;
			case '':

				/* try to do the right thing if this
                                   introduces an ansi escape sequence. 
				   Ignore everything except ^[[C 
                                   or ^[[B which
                                   we treat like tabs. */

				/* Save current state of stdin flags 
                                   and set non-blocking mode */
				fl=fcntl(0,F_GETFL);
				fcntl(0,F_SETFL,fl|O_NONBLOCK); 
#ifdef _MINIX
		/* I'm not sure why it's necessary to call
                   getch in minix rather than the direct read
                   command below. The read returns -1 even when
                   there are 2 characters still to be read, e.g.
                   the rest of an escape sequence like [A. The read
                   below seems more portable to me, in case there are
                   setups where the arrow keys return longer sequences.
                   For minix, of course, there's no need to be portable.*/
				if((buffer[0] = getch()) == EOF) {
					fcntl(0,F_SETFL,fl);
					clearerr(stdin);
					return c;
				}
				if((buffer[1] = getch()) == EOF) {
					fcntl(0,F_SETFL,fl);
					clearerr(stdin);
					return c;
				}
				buffer[2] = '\0';
#else
do_again:
				q = buffer;
				if(read(0,q++,1)!=1) {
					if(first_try){
						first_try = 0;
						sleep(1);
						goto do_again;
					}
					fcntl(0,F_SETFL,fl);
					return c;
				}
		/* Bug: We rely on escape sequences being 3 characters long
                   here */
				if(read(0,q++,1)!=1){
					fcntl(0,F_SETFL,fl);
					return c;
				}
				*q = '\0';

#endif
				if((strcmp(buffer,DOWN_ARROW)==0)||(strcmp(buffer,RIGHT_ARROW)==0)) 
					{
					fcntl(0,F_SETFL,fl);
					return '\t';
				}
				fcntl(0,F_SETFL,fl);
				buffer[0]='\0';
				break;

			case '':
			case '\x7F':
				if(limit==0)return c;
#ifndef XPLORE
				if(first_char){
					wstandout(mywin);
					wmove(mywin,0,0); 
					waddstr(mywin,mychars);
					wstandend(mywin);
					touchwin(mywin);
					first_char = 0;
				}
#endif
				if(vx <= 0)
					if(cx == 0){
						beep();	
						break;
					}
					else {
					/* scroll window to the left */
						wmove(mywin,0,0);
						j =  p-mychars > 8 ? 8 : p - mychars;
						for(i=j;i>0;i--)
							waddch(mywin,
							 mychars[p-mychars-i]);
						vx += j;
					}
				*--p = '\0';
				wmove(mywin,0,--vx);
				waddch(mywin,' ');
				wmove(mywin,0,vx);
				cx--;
				break;

			default:
				if(limit==0)return c;
				if(p-mychars >= MAX_NAME-1){
					beep();
					break;
				}
#ifndef XPLORE 
				if(first_char){ /* erase window and put cursor
                                                  at beginning */
					touchwin(mywin);
					while(vx>0){
						wmove(mywin,0,--vx);
						waddch(mywin,' ');
						wmove(mywin,0,vx);
					}
					cx = 0;
					first_char = 0;
					p = mychars;
				}
#endif
				if((c >= ' ')&&(c <= 127)){
					if(p-mychars >= limit + cx-vx){
						/* Need to scroll left. We
                                                   scroll 8 chars */
						vx -= 8;
						wmove(mywin,0,0);
						for(i=cx-vx;i<p-mychars;i++)
							waddch(mywin,mychars[i]);
						for(i=0;i<8;i++)
							waddch(mywin,' ');
						wmove(mywin,0,vx);
					}
					*p++ = c;
					*p = '\0';
					waddch(mywin,c);
					cx++;
					vx++;
				}
				break;
				
		}		
		wrefresh(mywin);
	}
	return c;
}

