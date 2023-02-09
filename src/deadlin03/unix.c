/* System dependent routines: Unix */
/* Handling the terminal is always difficult to do in a portable way.
   Our philosophy is to run in cbreak mode which allows us to intercept
   and process all control characters in readline.c. We allow the
   driver to echo control characters as ^C etc, and to
   do \n -> CR + LF translation on output. 
*/

#include "deadlin.h"
#include <limits.h>
#include <sys/types.h>
#include<unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#ifdef _POSIX
#include <termios.h>
#else
#include <sys/ioctl.h>
#endif
#include "unix.h"

char FilePath[PATH_MAX];
char FileExt[5];
char BaseName[PATH_MAX];  /* everything but extension */
extern int Status;

/* Breaks path into components. Checks whether file exists and sets
   new file flag if not. Opens file for read if it does exist. Returns 0
   in the case of an error, 1 else.
*/

int OpenFile(char *path,FILE **stream)
{
	char *ptr;
	struct stat MyStatBuf;
	char Path[PATH_MAX];

/* Extract directory name from path */

	strcpy(Path,path);
	if((ptr = strchr(Path,'/'))!= NULL)
		*(ptr+1) = '\0';
	else strcpy(Path,".");

/* Extract name and extension from the path */
/* For example, if path = /usr/bin/foo.txt then BaseName = /usr/bin/foo,
   and FileExt = txt. We define an extension as a string of length <= 4
   beginning with . that occurs at the end of a filename. Moreover, 
   something must precede the . */

	strcpy(FilePath,path);
	strcpy(BaseName,FilePath);
	if((ptr = strrchr(BaseName,'.'))){ /* Last . starts extension */
		if((strlen(ptr)<=4)&&(ptr > BaseName)&&(*(ptr-1)!='/')&&
		(strrchr(ptr,'/')==NULL)){
		    strcpy(FileExt,ptr+1);
		    *ptr = '\0';
		}
	}

	if(strcmp(FileExt,"bak")==0){
		fprintf(stderr,"Cannot edit .BAK file--rename file\n");
		exit(0);
	}

	/* Check whether the file exists */

	if(stat(path,&MyStatBuf)==-1)
		if(errno==ENOENT){
			FLAG(NEW_FILE);
			return 1; 
		}

	if((*stream = fopen(FilePath,"r")) == NULL) return 0;
	if(chdir(Path)==-1)return 0;
	return 1;

}


/* init_tty: Initializes the tty: input mode is cbreak */

#ifdef _POSIX
struct termios MyTermiosStructOrig;
#endif

int
init_tty(int fd){


#ifdef _POSIX
	struct termios MyTermiosStruct;
#else 
	struct sgttyb MySgttyb;
#endif
#ifdef _NEXT
	struct ltchars MyLtchars;
#endif

#ifdef _POSIX
	tcgetattr(fd,&MyTermiosStruct);
	tcgetattr(fd,&MyTermiosStructOrig);

	MyTermiosStruct.c_iflag = CIFLAG;
#ifdef _MINIX
        /* Not sure why, but this seems to work */
	MyTermiosStruct.c_oflag |= (~OPOST );	
#else
	MyTermiosStruct.c_oflag = (OPOST | ONLCR );	
#endif
	MyTermiosStruct.c_lflag = CLFLAG ;
	MyTermiosStruct.c_cc[VMIN] = 1; /* Otherwise the value will be 4 = ^D */
	MyTermiosStruct.c_cc[VTIME]=0; 

	if(tcsetattr(fd,TCSANOW,&MyTermiosStruct)!=0){
		fprintf(stderr,"Unable to intitialize terminal\n");
		exit(1);
	}
#else
	ioctl(0,TIOCGETP,&MySgttyb);
	MySgttyb.sg_flags = CBREAK | ECHO | CRMOD ;
	ioctl(0,TIOCSETP,&MySgttyb);

#endif /* POSIX */

#ifdef _NEXT
	ioctl(0,TIOCGLTC,&MyLtchars);
	MyLtchars.t_rprntc = -1;
	MyLtchars.t_lnextc = -1;
	MyLtchars.t_werasc = -1;
	ioctl(0,TIOCSLTC,&MyLtchars);
#endif


	setbuf(stdout,NULL);  /* Turn off output buffering. */
	return 0;
}

int
restore_tty(int fd){

#ifdef _POSIX
	tcsetattr(fd,TCSANOW,&MyTermiosStructOrig);
#endif

	return 0;
}

	

/* Do backspaces on the screen. Move Cursor left, print a space to
   cover up existing character, Move cursor left again. The array
   BackSpace holds the control sequence necessary to move the cursor
   left one space.
   Almost every terminal in existance uses le=^H (termcap) to 
   backspace. On a weird terminal change the definition of BackSpace. */

int 
DoBackspace(int fd,int reps){

	int i,r;
	char BackSpace[] = "";

	for(i=0;i<reps;i++){
	  r = write(fd,(void *)BackSpace,strlen(BackSpace));
	  r = write(fd,(void *)" ",1);
	  r = write(fd,(void *)BackSpace,strlen(BackSpace));
	}
	return 0;
}

int RawWrite(int fd, char c){

	int r;
	char Output[] = " ";
	Output[0] = c;
	r = write(fd,(void *)Output,1);
	return 0;
}
	
