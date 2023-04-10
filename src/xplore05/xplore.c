/*  xplore main program: A no-frills text mode filesystem explorer

	By Terry R. McConnell
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef NO_STRDUP
char *strdup(char *);
#endif
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <curses.h>
#include <termios.h>
#include  "browse.h" /* includes <sys/stat.h> */
#include "edit.h"
#include "history.h"

/* Portability issue: signal handlers return int on some systems, so
edit if necessary. */
#ifndef __sighandler_t 
#define __sighandler_t void (*)(int)
#endif

#define VERSION ".05"
#define USAGE "xplore [ -d <path> -h -l -v]"
#define HELP  "-h: print this helpful information.\n\
-d: start program in directory given by path argument\n\
-v: give program version and exit\n\
-l: print current file extension associations and exit\n"

#define BEEP  {beep();\
	      refresh();\
	      continue;}
#ifndef ARG_MAX
#define ARG_MAX 256
#endif
#ifndef MAXLINE
#define MAXLINE 1024 
#endif

#ifndef MORE
#define MORE "/usr/bin/less"
#endif

#define DO_NOTHING  "/usr/bin/false"

/* The following stuff concerns our implementation of file associations.
   There are three types:
	1) File extension associations, which tell which application to
           use to open a file with the given extension;

	2) Filename associations, which tell which command line to run
           when the user chooses a file of the given name; and

	3) Psuedofiles, which are labels that appear in the browser that
           can be used to trigger desired actions.

	Examples:

		.c  vi %s   # open c source files in an editor
		makefile  make all # filename association
                *SHELL-OUT /bin/sh   # psuedo-file

*/
#define MAX_ASSOCIATIONS 64
#define EXT_ASSOC 0
#define FILE_ASSOC 1
#define PSUEDO_FILE 2

struct association {
int type;
char name[MAX_NAME];
char value[MAX_NAME];
};
struct association assocs[MAX_ASSOCIATIONS];
int nassocs;  /* actual size of nassocs array */

int die(void);
void fatal(char *);
static char *gettext(char *);
static char *getfile(char *);

static char arg_buf[MAX_NAME+MAX_PATH+MAXLINE+1];
static char tmp_name[] = "/tmp/xplore05XXXXXX";   
static char tmp_buf[MAXLINE]; 
static char default_editor[MAX_NAME]="vi";  
#ifdef _DEBUG
void debug_msg(char *msg,int i);
static char logname[]="./xplore_log";
static	FILE *logfile;
#endif

static char buffer[MAX_PATH+MAXLINE + 1];	

int main( int argc, char *argv[])
{
	int i=0,j,c;
	char *path = NULL,*p;
	char *pptr;
	int browser_rval;
	struct stat my_stat;
	WINDOW *editwin;
	char *new_argv[ARG_MAX];
	char *token;
	struct termios tio_stdin;
	struct termios tio_stdout;
	struct termios tio_stderr;
	struct termios curs_tio_stdin;
	struct termios curs_tio_stdout;
	struct termios curs_tio_stderr;
	int child_status;
	int child_pid;
	int use_execvp = 0;
	FILE *rcfile;
	int n;
	int state;
	int do_once;
	char **pbptr;
	int is_psuedo = 0;
	int cur_hist;
	int rv;
#ifdef _DEBUG
	FILE *history_out;
	logfile = fopen(logname,"a+");
#endif

	/* Open and process rc file */
	/* Look for .xplorerc in the usual place ... */
	buffer[0]='\0';
	if((p=(char *)getenv("HOME")))
		strcpy(buffer,p);
	strcat(buffer,"/.xplorerc");
	if((rcfile = fopen(buffer,"r"))!=0){
		while(fgets(buffer,MAXLINE,rcfile)!=NULL){

			/* Each line may contain an association which
                           should be added to the array assocs. We parse
                           the lines assuming they have the form

			    [.|*]string  string

			   the second string may have embedded whitespace.
                           Anything after a # is to be ignored */

			/* Make sure we can hold another association */
			if(nassocs >= MAX_ASSOCIATIONS){
				fprintf(stderr,"Too many associations. Recompile with bigger limit\n");
				exit(1);
			}
			
			p = strchr(buffer,'#'); /* ignore # ... */
			if(p) *p = '\0';

			p = buffer;
			/* skip over initial whitespace */
			while((*p == ' ' || *p == '\t')&&(*p != '\0'))p++;	
			if(*p == '\0' || *p == '\n') continue; /* empty line */

			/* first non-whitespace char determines type of
                            association */
			switch(*p){
				case '.':
					assocs[nassocs].type = EXT_ASSOC;
					break;
				case '*':
					assocs[nassocs].type = PSUEDO_FILE;
					break;
				default:
					assocs[nassocs].type = FILE_ASSOC;
			}
			i=0;
			while((*p != ' ')&&(*p != '\t')&&(*p != '\n')&&(*p != '\0'))
				(assocs[nassocs].name)[i++]=*p++;
			(assocs[nassocs].name)[i] = '\0';
			while((*p == ' ' || *p == '\t')&&(*p != '\0'))p++;	
			i=0;
			while((*p != '\n')&&(*p != '\0'))
				(assocs[nassocs].value)[i++]=*p++;
			(assocs[nassocs++].value)[i] = '\0';	
		}
		fclose(rcfile);
	}

	/* Process command line */
	for(i=1;i<argc;i++){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'h':
					fprintf(stderr,"%s\n",USAGE);
					fprintf(stderr,HELP);
					exit(0);
				case 'v':
					fprintf(stderr,"%s\n",VERSION);
					exit(0);
				case 'd':
					if(chdir(argv[i+1])==-1){
						fprintf(stderr,"Cannot cd to %s\n",
							argv[i+1]);
						exit(1);
					}
					break;
				case 'l': 
					for(i=0;i<nassocs;i++)
						printf("%s= %s, type = %d\n",
						  assocs[i].name,
						  assocs[i].value,
						  assocs[i].type);
					exit(0);
				default:
					fprintf(stderr,"xplore: unrecognized option %s\n",
						argv[i]);
					exit(1);
			}
		}
	}

	/* Record terminal settings at startup */
	tcgetattr(0,&tio_stdin);
	tcgetattr(1,&tio_stdout);
	tcgetattr(2,&tio_stderr);

	/* See if the environment contains the name of an editor. Save
           it if so. */

	if((p=(char *)getenv("EDITOR")))strncpy(default_editor,p,MAX_NAME);

	/* initialize screen */
	initscr();
#ifdef NCURSES
	start_color();
	init_pair(TOP_PAIR,COLOR_RED,COLOR_BLACK);
	init_pair(LEFT_PAIR,COLOR_MAGENTA,COLOR_CYAN);
	init_pair(RIGHT_PAIR,COLOR_YELLOW,COLOR_BLUE);
	init_pair(EDIT_PAIR,COLOR_WHITE,COLOR_RED);
#endif


#ifdef _MINIX
	cbreak();
#else
        crmode();  
#endif
	noecho();
	signal(SIGINT, (__sighandler_t ) die); 

	/* initialize history list */
	n_hist = 0;
	cur_hist = 0;
	getcwd(buffer,256); 
	add_to_history(strdup(buffer)); 
	/* See history.c and history.h */
	while(1){

		do_once = -1;
		if(!getcwd(buffer,256))
			fatal("Cannot cd before browse\n"); 

		/* Call the browser: to support multiple selection, 
                   the buffer will hold either a string or an array
                   of strings terminated by NULL. The latter only
                   happens if the return value is BROWSER_MULTIPLE */
		browser_rval=browse(buffer,buffer);
		if(browser_rval == BROWSER_EXIT)break;
		switch(browser_rval){

#ifdef _DEBUG
			/* This entry is for debugging only, although it
                           could be used in the future to implement
                           preserving history across sessions. It just
                           dumps info about history into a file named
                           history.out in the current directory */

			case BROWSER_DUMP:
				history_out = fopen("history.out","w+");
				if(!history_out)
					fatal("cannot open history output file");
				fprintf(history_out,"Max size %d, Current size %d, current entry %d\n",
						MAX_HISTORY,n_hist,cur_hist);
				
				for(j=0;j<n_hist;j++)
					fprintf(history_out,"%d. %s\n",
						j,history[j]);
				fclose(history_out);
				continue;
#endif

			case BROWSER_BACK:
				if(cur_hist == 0){
					BEEP	
					/* includes continue */
				}
				cur_hist--;
				if(chdir(history[cur_hist])==-1){
					BEEP
				}
				continue;
			case  BROWSER_FWD:
				if(cur_hist == n_hist-1){
					BEEP
				}
				cur_hist++;
				if(chdir(history[cur_hist])==-1){
					BEEP
				}
				continue;
			case BROWSER_ERROR:
				fatal("browser error");
			
			case BROWSER_MULTIPLE:
			case BROWSER_MORE:
			case BROWSER_RSEL:
			case BROWSER_LSEL:
			case BROWSER_EDIT:
				break;

			default:
				fatal("Unknown return value from browser");
		}
	
/* If multiple selection, the following loops over pointers returned
   in buffer, otherwise it runs once */

               if(browser_rval != BROWSER_MULTIPLE)
			do_once = 1;
	       else  /* do_once stays -1 and ... */ 
		 	pbptr = (char **)buffer;	
	       while(do_once--){
		if(browser_rval == BROWSER_MULTIPLE){
			path = *pbptr;
			pbptr++;
		}
		else
			path = buffer;
		if(path == NULL)break;

		/* Figure out what type of file was selected */
		if(*getfile(path) == '*')is_psuedo = 1;
		else is_psuedo = 0;
		if((stat(path,&my_stat)==-1) && (!is_psuedo)){
			BEEP
		}
		/* If it's a directory, cd there and return to the browser */
		if(!is_psuedo && S_ISDIR(my_stat.st_mode)){

			if(cur_hist == n_hist-1){ /* top of hist */
			
			/* if selected directory is different than the
                           previous one, add it at top of history list */
	
				if(strcmp(path,history[cur_hist])!=0){
					add_to_history(strdup(path));
					cur_hist = n_hist - 1;	
				}
			}
			else { /* if we're currently buried in the history
                                  list, free everything above the current
                                  entry and make selected directory the
                                  new top of list. */

					free_history(cur_hist);
					add_to_history(strdup(path));
					cur_hist++;
			}
			if(chdir(path)==-1){
				BEEP
			}
			continue;
		}
		else {  /* not a directory */

			/* Selection is executable or not. In either
                           case we will spawn an external program: in the
			   executable case it will be the file itself; in
                           other cases it will an editor or associated
                           application. In all cases we must build a
                           command line for the exec */
		   
		   use_execvp = 0;  /* flag to choose the right kind of
                                       exec */
                           
		   if((browser_rval != BROWSER_EDIT) && 
				 !is_psuedo && IS_EXECUTABLE(my_stat.st_mode)){ 

			/* Create a window which is placed in the
                           top panel of the browser window so that the
                           user can enter command line arguments. 
		        */

#ifdef _MINIX
			curs_set(1); /* appears not to work anyway */
#else
			leaveok(stdscr,FALSE); /* restore cursor */
#endif
			editwin = newwin(2,TOP_DX,TOP_Y+1,TOP_X);
#ifdef NCURSES
			wattron(editwin,COLOR_PAIR(EDIT_PAIR));
#endif
			if(!editwin)fatal("Could not create window");
			strcpy(arg_buf,path);
			while(TRUE){
			  i=line_edit(editwin,TOP_DX,arg_buf);
			  switch(i){
				char *p;

				case '':  /* back out */
					strcpy(buffer,DO_NOTHING);
					break;
				case '\n':
				case '\r':
					break;
				case '':
					/* browse for remaining args */
					p = (char *)malloc(MAXLINE*sizeof(char *));
					while(TRUE){

					/* loop the browser as long as the
                                           user is selecting a directory from
                                           the left pane. 
					*/
						getcwd(p,256);
						rv = browse(p,arg_buf);	
						if(rv == BROWSER_ERROR)break;
						if(rv == BROWSER_RSEL)break;
						if(rv == BROWSER_EXIT)break;
						if((stat(p,&my_stat)==-1)||
							!S_ISDIR(my_stat.st_mode))
							break;
						if(chdir(p)==-1)break;
					}
					if(rv == BROWSER_RSEL){
						strcat(arg_buf," ");
						strcat(arg_buf,p);
					}
					if(rv == BROWSER_MULTIPLE){
						char **ptr = (char **)p;
						while(*ptr){
							strcat(arg_buf," ");
							strcat(arg_buf,*ptr);
							free(*ptr);
							ptr++;
						}
					}
					free(p);
					if(rv == BROWSER_EXIT)break;
					continue;
				default:
					arg_buf[0]='\0';
					break;
			  } /* end switch */
			  break;
			}  /* end line_edit while true */
			delwin(editwin);
		   }  /* End if file is executable */
		   else  if(browser_rval != BROWSER_EDIT) {  
			     /* file is not executable. Determine whether
                              we have an association for this file */

			char *p,*q;

			use_execvp = 1;
			p = gettext(path);
			q = getfile(path);
			rv = BROWSER_NEVER;
			for(i=0;i<nassocs;i++){
				if(p && (assocs[i].type==EXT_ASSOC)
				     && (strcmp(p,assocs[i].name)==0)){
					sprintf(arg_buf,assocs[i].value,
						path);
					break;
				}
				if(q && (assocs[i].type==FILE_ASSOC)
				     && (strcmp(q,assocs[i].name)==0)){
					sprintf(arg_buf,assocs[i].value,
						path);
					break;
				}
				if(q && (assocs[i].type==PSUEDO_FILE)
				     && (strcmp(q,assocs[i].name)==0)){

					/* see whether this psuedo-file
                                           expects an argument. If so,
                                           open the top pane browser etc,
                                           just as for an executable */

					if(strchr(assocs[i].value,'%')){

					
						char *tmp_ptr;
						char *tmp_buf;

					tmp_buf = (char *) malloc(MAX_NAME);
#ifdef _MINIX
					curs_set(1); /* appears not to work anyway */
#else
		 			leaveok(stdscr,FALSE); /* restore cursor */
#endif
					strcpy(tmp_buf,assocs[i].value);
					tmp_ptr = strchr(tmp_buf,'%');
					*tmp_ptr = '\0';
					editwin = newwin(2,TOP_DX,TOP_Y+1,TOP_X+1);
#ifdef NCURSES
					wattron(editwin,COLOR_PAIR(EDIT_PAIR));
#endif
					if(!editwin)fatal("Could not create window");
					while(TRUE){
			  			switch(line_edit(editwin,TOP_DX,tmp_buf)){
							char *p;

							case '\n':
							case '\r':
							break;
							case '':
							/* browse for remaining args */
							p = (char *)malloc(MAXLINE*sizeof(char *));
							while(TRUE){

							/* loop the browser as long as the
                                           		user is selecting a directory from
                                           		the left pane. 
							*/
								getcwd(p,256);
								rv = browse(p,tmp_buf);	
								if(rv == BROWSER_EXIT)
;
								if(rv == BROWSER_ERROR)break;
								if(rv == BROWSER_RSEL)break;
								if((stat(p,&my_stat)==-1)||
									!S_ISDIR(my_stat.st_mode))
									break;
								if(chdir(p)==-1)break;
								}
								if(rv == BROWSER_RSEL){
								strcat(tmp_buf," ");
								strcat(tmp_buf,p);
								}
								if(rv == BROWSER_MULTIPLE){
									char **ptr = (char **)p;
									while(*ptr){
									strcat(tmp_buf," ");
									strcat(tmp_buf,*ptr);
									free(*ptr);
									ptr++;
								}
							}
							free(p);
							if(rv == BROWSER_EXIT)break;
							continue;
							default:
							break;
			  			} /* end switch */
			  			break;
						}
						delwin(editwin);
						sprintf(arg_buf,assocs[i].value,
							tmp_ptr);
						free(tmp_buf);
					}
					else
					sprintf(arg_buf,"%s",
						assocs[i].value);
					break;
				}
				if(rv == BROWSER_EXIT)break;
				
                        } 

			/* Otherwise, open the file in the default
                           editor */

			if(i==nassocs)
			   	sprintf(arg_buf,"%s %s",default_editor,path);
		   }
		   else {  /* Forced edit */
			   use_execvp = 1;
			   sprintf(arg_buf,"%s %s",default_editor,path);
		   }

		   /* arg_buf now contains command to be exec'ed. Build
                      new argument vector for the exec by splitting the
                      contents of arg_buf on whitespace */

		  token = strtok(arg_buf," \t");
		  if(!(new_argv[0] = (char *)malloc(strlen(token)+1)))
			fatal("Cannot allocate argument memory");
		  strcpy(new_argv[0],token);
		  i=1;
		  while((token = strtok(NULL," \t"))){
			if(!(new_argv[i] = (char *)malloc(strlen(token)+1)))
				fatal("Cannot allocate argument memory");
				strcpy(new_argv[i++],token);
				
		  }
		  new_argv[i] = NULL;

		 /* Record current (curses) settings of terminal */
	         /* They will be restored after the exec       */

		  tcgetattr(0,&curs_tio_stdin);
		  tcgetattr(1,&curs_tio_stdout);
		  tcgetattr(2,&curs_tio_stderr);

		  /* clear screen, restore cursor */
#ifdef _MINIX
		  curs_set(1);
#else
		  leaveok(stdscr,FALSE);
#endif
		  clear();
		  mvcur(0,COLS-1,LINES-1,0);
		  touchwin(stdscr);
		  refresh();

		  /* Restore terminal settings at program startup */

		  tcsetattr(0,TCSANOW,&tio_stdin);
		  tcsetattr(1,TCSANOW,&tio_stdout);
		  tcsetattr(2,TCSANOW,&tio_stderr);

		  if(browser_rval == BROWSER_MORE){

			/* output from program is going to collected in a
                           temporary file and later piped through more */

			close(mkstemp(tmp_name));  
		  }

		  if((child_pid = fork())==-1) fatal("Fork error"); 

		  if(child_pid){ /* we are parent */


				wait(&child_status); 

				/* more stuff stored in temporary file
                                   by child process if MORE option was
                                   selected */

				if(browser_rval == BROWSER_MORE){
					sprintf(arg_buf,"%s %s",MORE,tmp_name);
					system(arg_buf);
					remove(tmp_name); 
				}
				
				/* clean up */
				for(j=0;j<i;j++)free(new_argv[j]);

				/* Restore curses terminal settings */

				tcsetattr(0,TCSANOW,&curs_tio_stdin);
				tcsetattr(1,TCSANOW,&curs_tio_stdout);
				tcsetattr(2,TCSANOW,&curs_tio_stderr);

		   }
		   else {

				/* Code that runs in child process */

				/* switch output to temporary file name
                                   stored in tmp_name */
				if(browser_rval == BROWSER_MORE){
					if(freopen(tmp_name,"a+",stdout) == NULL)fatal("Could not open temp file for stdout");;
					if(freopen(tmp_name,"a+",stderr) == NULL)fatal("Could not open temp file for stderr");
				}

				if(use_execvp)
					execvp(new_argv[0],new_argv);
				else 
					execv(path,new_argv);
				fatal("Exec error");

		  }
		} /* not a dir */
	   } /* loop over multiple selections */
	   if(browser_rval == BROWSER_MULTIPLE){
			pbptr = (char **)buffer;
			while(*pbptr)free(*pbptr++);
	   }	
	}  /* main while loop  */


/* clean up and exit */
#ifdef _MINIX
	nocbreak();
	echo();
#else
	nocrmode();
#endif
	clear();
	refresh();
	endwin();
	printf("\n\f");
	exit(0);
}

int die()
{
/*      ignore interupts so we can get this done  */
	signal(SIGINT,(__sighandler_t )1);
/*      make terminal the way it was */
	endwin();
	exit(0);          /* exit with exit status 0  */
}

/* Prints messages to external file because no reliable
way to do it with windows in place */

#ifdef _DEBUG
void debug_msg(char *msg,int i)
{
	fprintf(logfile,"%s %i\n",msg,i);
}
#endif


#ifndef _MINIX  /* Minix lcurses already defines this */
void fatal(char *msg){

/* move cursor to lower left  */
	mvcur(0,COLS-1,LINES-1,0);

/*      make terminal the way it was */
	endwin();
	fprintf(stderr,"%s\n",msg);
	fprintf(stderr,"errno = %d\n",errno);
	fprintf(stderr,"He's dead, Jim\n");
	exit(1);          /* exit with exit status 1  */

}
#endif


/* Return pointer to filename extension, or NULL if there
is none */

static char *
gettext(char *path)
{
	char *p;

	p = strrchr(path,'.');
	if(p==path)return NULL;  /* It's a dot file, not an extension */
	return p;
}

/* Return pointer to last (filename) component of path */

static char *
getfile(char *path)
{
	char *p;

	p = strrchr(path,'/');
	if(p==NULL)p=path;
	return p+1;
}

#ifdef NO_STRDUP
char *strdup(char *strng){
	char *p;

	p = (char *)malloc(strlen(strng)+1);
	if(!p)
		fatal("Cannot allocate memory in strdup");
	strcpy(p,strng);
	return p;
}
#endif

