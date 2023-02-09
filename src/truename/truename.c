/* truename.c: give the name of a file at the end of a chain of links 

  Gives an equivalent name for a file in the form
       DIR1/DIR2/.../DIRN/F  where none of DIR1...DIRN or F is a
       symbolic link. Here DIR1 is relative to CWD   */

/* usage: truename [ -vda ] path1 path2 ...   (prints to stdout)  */
/*         -v: gives a play-by-play.
	   -a: result is an absolute pathname (otherwise it is relative
	       to the cwd.) 
	-d: Don't clean up by removing /.. and ./ (default is to clean up.) */

/* algorithm:   truename ( path )
		split path into dir/file or dir/ or file
		if dir = NULL or dir = root
		  if file is a link
			file --> newdir/newfile
			return truename(newdir/newfile)
		  else return path
		else 
		 if file is non-null and is a link
			file --> newdir/newfile
			if newdir begins with / return truename(newdir/newfile)
			else  return truename(dir/newdir/newfile)
		 else return truename(dir)/file

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>


#ifdef __NeXT__
#define S_ISLNK(mode)  (((mode) & (_S_IFMT)) == (_S_IFLNK))
#endif

#define BUFSIZE 1024
#define VERSION "1.2"
#define MAX_PATH_LENGTH 256
#define MAXLINKS 20 /* Max number of successive links. Prevent circular links */
#define MAX_ARG_LEN 80             /* Max length of combined options */
#define INFO1  "usage: truename [-vda] path: give an equivalent name for path which"
#define INFO2 "contains no symbolic links."
#define INFO3 "-v: Give a play-by-play"
#define INFO4 "-a: Result is an absolute pathname. (Else it is relative to cwd.)"
#define INFO5 "-d: Don't clean up by removing /.. and ./ (default is to clean up.)"
#define INFO6 "-version: Print version number and exit."
#define INFO7 "-h: Print this helpful message and exit."

int find_opt(char *option_string);
static char progname[BUFSIZE];
static char root[1] = "/";
int verbose = 0;
int depth = 0;
int links = 0;
int absolute = 0;
int dirty = 0;

char *clearbuf(char *, int);
void myerror(char *,char *);
char *truename( char *path);
char *squeezedot(char *);
char *SqueezeAllDots(char *);
char *isostrstr(char *, char *);
char *head(char *path);
char *tail(char *path);
char *join(char *dir, char* file);
char *parent(char *);
char cwd[BUFSIZE];
#ifndef __HAVE_STRDUP__
char *strdup(char *);
#endif

/* main is just a wrapper for subroutine truename */

int
main(int argc, char **argv)
{
	
	char path[BUFSIZE];
	char buffer[BUFSIZ];
	char *p;
	int c=-1;                /* numeric option code */
	char argstr[MAX_ARG_LEN] = "-";

	strcpy(progname,*argv); /* for error messages */
#ifdef __BSD__
	getwd(cwd);
#else
	if(!getcwd(cwd,BUFSIZE)){
		fprintf(stderr,"truename: unable to get working directory\n");
		return 1;
	}
#endif


	if(--argc > 0){  /* Anything on command line ? */
		if((*++argv)[0]=='-'){      /* already the end of options */
			if((*argv)[1]=='-'){
				++argv;
			}
			else strcat(argstr,++*argv); /* start building argstr*/
			argc--;
			while((argc>0)&&((*++argv)[0]=='-')&&((*argv)[1]!='-')){
/* combine all options into a single string */
				argc--;
				strcat(argstr,++*argv);
			}
			c=find_opt(argstr); /* look up code */
		}
		switch(c){

			case -1: break; /* default, e.g if no options */

			case 8: printf("%s\n",VERSION);
				return 0;
			case 0: 
				printf("%s\n",INFO1);
				printf("%s\n",INFO2);
				printf("%s\n",INFO3);
				printf("%s\n",INFO4);
				printf("%s\n",INFO5);
				printf("%s\n",INFO6);
				printf("%s\n",INFO7);
				return 0;
		        case 7: dirty=1;	
			case 3: absolute =1;
			case 1: verbose =1;
				break;
			case 6: verbose=1;dirty=1;break;
			case 5: dirty =1;
			case 2: absolute =1;
				break;
			case 4: dirty = 1;
				break;
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				return 1;
			}
	}

	while (argc-- > 0) {
		depth=links=0;
		strcpy(path,*argv++);


		if(dirty==0){
/* Remove all . and .. references from path */
			if((p=SqueezeAllDots(path))==NULL){
				myerror("Invalid path -- ", path);
				return 1;
			}	
			p = SqueezeAllDots(truename(p));
		}
		else    p = truename(path);	
		if(absolute==1&& *p != '/'){
			if(strcmp(cwd,"/") !=0)printf("%s/",cwd);
			else printf("/");
		}
		printf("%s\n",p);
		if( verbose == 1) { printf("Total links = %d\n",links);
			printf("Current working directory = %s\n",cwd);
		}
	}
	return 0;
}


/* truename: recursive routine to squeeze the links out of path */
/* See description of algorithm above */
	
char *truename( char *path)
{
	char *dir, *newdir, *file, *newfile;
	char *charbuf;
	struct stat *mystat;
	int i;

	charbuf = (char *) malloc( BUFSIZE + 1);
	mystat  = (struct stat *) malloc( sizeof(struct stat) );

        if(++depth  > MAX_PATH_LENGTH) myerror("Path too long.","\n");

	if (path == NULL) { --depth; return NULL;}
	if (strcmp(path,"/") ==0) { --depth; return path;}

/* split path into dir/file   */

	file = tail(path);
	dir =  head(path);

	if(verbose == 1){
		 for(i=0;i<=depth;i++)printf(" ");	/* indent */ 
		 printf("%s= ( %s ) + ( %s )\n",path,dir,file);
	}

	if( dir == NULL || dir == root ){
		if(lstat(path,mystat)!=0)
			myerror("unable to stat ",path);
		if(S_ISLNK(mystat->st_mode)){
		  if(++links > MAXLINKS)
			myerror("Too many links."," Circular links? \n");
		  clearbuf(charbuf,BUFSIZE);
		  if(readlink(path,charbuf,(size_t )BUFSIZE)== -1)
			myerror("unable to read link ",path);
		   --depth; 
	           return  strdup(truename(strdup(charbuf)));
		
	}
		else  { --depth; return strdup(path);}

	}

       else {             /* directory and file are not null */

		if(lstat(path,mystat)!=0)myerror("unable to stat " ,path);
		if(file != NULL && S_ISLNK(mystat->st_mode)) {

/* file is a link */

			clearbuf(charbuf,BUFSIZE);
			if(readlink(path,charbuf,(size_t )BUFSIZE)== -1)
				myerror("unable to read link ",path);
		
			if ( ++links > MAXLINKS)
				myerror("Too many links."," Circular Links?\n");

		/* split file into newdir newfile */

			newfile = tail(charbuf); 
			newdir = head(charbuf);
			
			if(newdir != NULL && *newdir == '/')  /* absolute path */
				 return strdup(truename(join(newdir,newfile)));
			else  return
			       strdup(truename(join(join(dir,newdir),newfile)));
		}
/* file is not a link */
		else  return join(strdup(truename(dir)),file);
		
	}
}


/* clearbuf: fill a buffer with nulls */
char *clearbuf( char *buffer, int size)
{
	int i; /* counter */

	for(i=0;i<size;i++)buffer[i]='\0';
	return buffer;
}

void
myerror(char *msg1, char*msg2)
{
	fprintf(stderr,"%s: %s%s \n",progname,msg1,msg2);
	exit(1);
}

	

/* strdup: why doesn't every unix have this ??? */
/*
char *strdup(char *s)
{

        char *p;

        p = (char *)malloc(strlen(s)+1);
        if (p != NULL)
                strcpy(p,s);
        return p;
}
*/

/* head: return dir/ if given path = dir/file, NULL iff path is a file. */

char *head( char *path)
{
	char *tail;
	char *charbuf;

	charbuf = (char *)malloc(BUFSIZE +1);

	tail = (char *) strrchr(path,'/');
        if(tail == NULL) return NULL;   /* It's a file: no / */
        else if(strcmp(tail,"/")==0){
		 strcpy(charbuf,path);
		 charbuf[strlen(path)-1] = '\0';  /* Get rid of trailing / */
		 return strdup(charbuf);
	}
	else if (tail==path) return root;  
        else {
                 clearbuf(charbuf,BUFSIZE); /* strncpy won't null terminate*/
		 strncpy(charbuf,path,tail-path);
                 return strdup(charbuf);
        }

}

/* tail: return file, given path = dir/file, NULL if path is a dir. */

char *tail( char *path)
{
	char *dirend;
	char *charbuf;

	charbuf = (char *)malloc(BUFSIZE+1);

	strcpy(charbuf,path);
	dirend = (char *) strrchr(charbuf,'/');
	if(dirend == NULL) return strdup(charbuf);
	if(dirend == path) return path + 1;
	else if(strcmp(dirend,"/")==0) return NULL;
	else return strdup(++dirend); /* omit leading / */
}

/* join: return string dir/file given dir/ and file */

char *join( char *dir, char *file)
{
	char *buffer;
	
	buffer = (char *) malloc(BUFSIZE+1);
	clearbuf(buffer,BUFSIZE);
	if(file == NULL ) return strdup(dir);
	else if (dir == NULL) return strdup(file);
	else {
		strcpy(buffer,dir);
		strcat(buffer,"/");
		strcat(buffer,file);
		return strdup(buffer);
	}
}


/* return a unique code for each option and option combo */

int find_opt(char *word)
{
	if(strcmp(word,"-help")==0)return 0;
	if(strcmp(word,"-h")==0)return 0;
	if(strcmp(word,"-v")==0)return 1;
	if(strcmp(word,"-a")==0)return 2;
	if(strcmp(word,"-av")==0)return 3;
	if(strcmp(word,"-va")==0)return 3;
	if(strcmp(word,"-d")==0)return 4;
	if(strcmp(word,"-ad")==0)return 5;
	if(strcmp(word,"-da")==0)return 5;
	if(strcmp(word,"-dv")==0)return 6;
	if(strcmp(word,"-vd")==0)return 6;
	if(strcmp(word,"-avd")==0)return 7;
	if(strcmp(word,"-adv")==0)return 7;
	if(strcmp(word,"-vad")==0)return 7;
	if(strcmp(word,"-vda")==0)return 7;
	if(strcmp(word,"-dav")==0)return 7;
	if(strcmp(word,"-dva")==0)return 7;
	if(strcmp(word,"-version")==0)return 8;


/* arg not found */
	return -2;
}


		
/* parent: return a pointer to a string containing the name of
   the parent directory, or NULL if none exists */

/* path should not contain any . or .. references */

char *parent(char *path)
{
	char buffer[BUFSIZ];
	char *sptr;
	char *bptr;
	char *dptr;
	char *cptr;

	if(path == NULL || strlen(path) == 0) return NULL;
	if(strcmp(path,"/") == 0) return NULL;

	/* point at last char in path */
	bptr = path + strlen(path) -1;

        /* walk back to find first previous / */
	for(dptr = bptr-1;dptr > path;dptr--)
		if(*dptr == '/')break;

	/* if we got back to beginning of path return root or cwd
             depending on whether path is absolute or relative */
	if(dptr == path) 
		if(*path == '/')return strdup("/");
		else return strdup(cwd);

	/* Throw away the last directory to get parent otherwise */
	sptr = buffer;
	for(cptr = path;cptr < dptr; cptr++)*sptr++ = *cptr;
	*sptr = '\0';
	return strdup(buffer);
}

/* Below are routines to clean up the paths by removing current and
parent directory references. This is complicated by the fact that . and ..
can occur in a file or directory name, e.g. .cshrc ! This is the reason
for the isostrstr routine below: it finds only "isolated instances of .
and .. */

/* squeezedot: remove all current directory references "." from a
   string and return a pointer to the new string. The string must
   not contain any parent directory references, ".." */

char *squeezedot(char *path)
{
	char buffer[256];
	char *dotptr, *slashdotptr;
	char *cptr;

	if((dotptr = isostrstr(path,"."))==NULL)   /* No references */
		return path;

	/* Handle the case that path starts with . */
	if(dotptr==path) {
		strcpy(buffer,cwd);  /* expand it to cwd */
		strcat(buffer,path+1); /* and append the rest of path */
		return squeezedot(strdup(buffer)); /* Call self on new string */
	}

	/* Handle the case that path equals /. */
	if(strcmp(path,"/.")==0) return strdup("/");

	/* Path may be written A/.B where A is nonempty and free of . */
	/* In such cases remove the /. and call self on the remaining string */

	if((slashdotptr = isostrstr(path,"."))==NULL)return NULL;
	slashdotptr--;  /* point at the / */
	for(cptr=path;cptr < slashdotptr;cptr++)
		*(buffer + (cptr - path)) = *cptr;
	*(buffer + (cptr - path)) = '\0';
	strcat(buffer,slashdotptr+2);
	return squeezedot(strdup(buffer));
}

/* SqueezeAllDots: remove all . and .. references from path and return
   a pointer to the new string, NULL if there is an error */

char *SqueezeAllDots(char *path)
{	
	char buffer[256];
	char *_2dotptr, *slashdotdotptr,*subpath;
	char *cptr;

	if(path == NULL) return NULL;
	if((_2dotptr = isostrstr(path,".."))==NULL) return squeezedot(path);

	/* Handle the case that path starts with .. */
	if(_2dotptr == path) {
		strcpy(buffer,parent(cwd));
		strcat(buffer,path+2);
		return SqueezeAllDots(strdup(buffer));
	}

	/* Path should now have a /.. */
	if((slashdotdotptr = isostrstr(path,".."))==NULL)return NULL;
	slashdotdotptr--;
	if(slashdotdotptr == path)return NULL;
	
	/* The path can be written A/..B where A is nonempty and free
               of .. Remove all . from A */

	for(cptr=path;cptr < slashdotdotptr;cptr++)
		*(buffer + (cptr - path)) = *cptr;
	*(buffer + (cptr - path)) = '\0';
	subpath = squeezedot(buffer);   /* A = subpath */
	if(parent(subpath)==NULL)return NULL;
	strcpy(buffer,parent(subpath));
	if(strcmp(buffer,"/") == 0){
		if(*(slashdotdotptr+3) != '\0')
			strcpy(buffer,slashdotdotptr+3);
	}
	else strcat(buffer,slashdotdotptr + 3);
	return SqueezeAllDots(strdup(buffer));
}

/* isostrstr: returns pointer to first occurence of substring in string
	having no adjacent chars other than '/', or NULL if there is no
        such.
*/

char *isostrstr(char *main, char *sub)
{
	char *p;
	int len;	

	if(main == NULL) return NULL;  /* paranoia */
	if( sub == NULL ) return main + strlen(main);

	len = strlen(sub);
	
	if((p = strstr(main,sub)) == NULL) return NULL;
	if( p == main )
		if(( *(p+len) != '\0') && ( *(p+len) !=	'/'))
			return NULL;
		else return p;
	if( *(p - 1) != '/') return NULL;
	if(( *(p+len) != '\0') && ( *(p+len) !=	'/'))
			return NULL;
	else return p;
}

#ifndef __HAVE_STRDUP__

/* strdup:  for implementations that don't have it. */

char *strdup( char *s)
{
        char *p;

        p = (char *) malloc(strlen(s)+1);
        if(p != NULL)
                strcpy(p,s);
        return p;
}
#endif
