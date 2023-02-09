/* System dependent routines: DOS */
/* This is included for completeness: its kind of silly of build an edlin
   clone under DOS. */


#ifdef CPM68
#include <cpm.h>
#endif

#include "deadlin.h"
/* #include <stdlib.h> */
#ifndef CPM68
#include <stdio.h>
int init_printer(void)
{
 return(0);
}
int flush_printer(void)
{
 return(0);
}
#endif

/*#include <io.h> */
#include <errno.h>

char FilePath[14];
char DriveLetter[3];
char FileExt[5];
char BaseName[11];  /* everything but extension */
extern int Status;

/* Breaks path into components. Checks whether file exists and sets
   new file flag if not. Opens file for read if it does exist. Returns 0
   in the case of an error, 1 else.
*/

int OpenFile(char *path,FILE **stream)
{
	char *ptr;
        char Path[MAXPATH];

/* Extract directory name from path */

	strcpy(Path,path);
	if((ptr = strchr(Path,'\\'))!= NULL)
		*(ptr+1) = '\0';
	else strcpy(Path,".");

/* Extract name,  extension, and drive letter from the path */
/* For example, if path = C:\doc\foo.txt then FilePath = foo.txt,
   BaseName = foo, and FileExt = txt. We define an extension as a string
   beginning with . that occurs at the end of a filename.  */

	if(Path[1]==':'){
		DriveLetter[0]=Path[0];
		DriveLetter[1]=':';
		DriveLetter[2]='\0';
	}
        if((ptr = strchr(path,'\\'))!=NULL)
			strcpy(FilePath,ptr+1);
	else strcpy(FilePath,path);
			
	strcpy(BaseName,FilePath);
        if((ptr = strchr(BaseName,'.'))!=NULL){ /* . starts extension */
		    strcpy(FileExt,ptr+1);
		    *ptr = '\0';
	}

	if(strcmp(FileExt,"bak")==0){
		fprintf(stderr,"Cannot edit .BAK file--rename file\n");
		exit(0);
	}

	/* Check whether the file exists */
/*
	if(chdir(Path)==-1)
                if(errno==ENOENT){
			fprintf(stderr,"Invalid Path\n");
			exit(1);
		}
*/
	if(access(FilePath,0)==-1)
		if(errno==ENOENT){
			FLAG(NEW_FILE);
			return 1; 
		}

	if((*stream = fopen(FilePath,"r")) == NULL) return 0;
	return 1;

}


/* init_tty: Trivial routine in DOS */

int
init_tty(int fd){
	return 0;
}



/* Do backspaces on the screen. Move Cursor left, print a space to
   cover up existing character, Move cursor left again. The array
      BackSpace holds the control sequence necessary to move the cursor
         left one space.
            Almost every terminal in existance uses le=^H (termcap) to
               backspace. On a weird terminal change the definition of BackSpace. */
               
int DoBackspace(int fd,int reps){
 int i;
 char BackSpace[] = "\x06";
 char Space[]=" ";
 for(i=0;i<reps;i++){
  write(fd,BackSpace,1); /*6 is backspace!*/
  write(fd,Space,1);
  write(fd,BackSpace,1);
 }
 return 0;
}