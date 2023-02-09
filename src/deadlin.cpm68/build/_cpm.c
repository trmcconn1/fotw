# 1 "cpm.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "cpm.c"






# 1 "/home/dtz/8BIT/68K/CPM1.3/cpm.h" 1







# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/stdio.h" 1
# 11 "/home/dtz/8BIT/68K/CPM1.3/DISK4/stdio.h"
# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/portab.h" 1
# 12 "/home/dtz/8BIT/68K/CPM1.3/DISK4/stdio.h" 2






struct _iobuf {
 int _fd;
 int _flag;
 char *_base;
 char *_ptr;
 int _cnt;
};

extern struct _iobuf _iob[16];
# 9 "/home/dtz/8BIT/68K/CPM1.3/cpm.h" 2
# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/portab.h" 1
# 10 "/home/dtz/8BIT/68K/CPM1.3/cpm.h" 2


int __BDOS(int,long);
int rename(char *,char *);
char * strchr(char *, char);
char * strrchr(char *, char);

extern int errno;


void abort(int);

int access(char *,int);
int atoi(char *);
long atol(char *);
int brk(char *);

char * calloc(int,int);
char * malloc(int);
char * realloc(char *,int);
void free(char *);
int chmod(char *,int);
int chown(char *,int,int);
int close(int);
int creat(char *,int );
int creata(char *,int );
int creatb(char *,int );
void exit(int);
void _exit(int);
int fclose(struct _iobuf *);
int fflush(struct _iobuf *);




struct _iobuf * fopen(char *,char *);
struct _iobuf * fopenb(char *,char *);
struct _iobuf * fopena(char *,char *);
struct _iobuf * freopen(char *,char *,struct _iobuf *);
struct _iobuf * freopenb(char *,char *,struct _iobuf *);
struct _iobuf * freopena(char *,char *,struct _iobuf *);
struct _iobuf * fdopen(int,char *);
int fseek(struct _iobuf *,long,int);
int rewind(struct _iobuf *);
long ftell(struct _iobuf *);




int fgetc(struct _iobuf *);
int getw(struct _iobuf *);
long getl(struct _iobuf *);
char * getpass(char *);
int getpid();
char * gets(char *);
char * fgets(char *,int,struct _iobuf *);
char * index(char *,char);
char * rindex(char *,char);
int isatty(int);
long lseek(int,long,int);
long tell(int);
char * mktemp(char *);
int open(char *,int);
int opena(char *,int);
int openb(char *,int);
int perror(char *);
int printf(char *,...);
int fprintf(struct _iobuf *,char *,...);
char * sprintf(char *,char *,...);

int fputc(char,struct _iobuf *);
int putw(int,struct _iobuf *);
long putl(long,struct _iobuf *);
int puts(char *);
int fputs(char *,struct _iobuf *);

int srand(int);
int rand();
int read(int,char *,int);
int scanf(char *,...);
int fscanf(struct _iobuf *,char *,...);
int sscanf(char *,char *,...);

# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/setjmp.h" 1
# 35 "/home/dtz/8BIT/68K/CPM1.3/DISK4/setjmp.h"
typedef long jmp_buf[13];
# 94 "/home/dtz/8BIT/68K/CPM1.3/cpm.h" 2
int setjmp(jmp_buf);
void longjmp(jmp_buf,int);

char * strcat(char *,char *);
char * strncat(char *,char *, int);
int strcmp(char *,char *);
int strncmp(char *,char *,int);
char * strcpy(char *,char *);
char * strncpy(char *,char *,int);
int strlen(char *);
int swapb(char *,char *,int);
char * ttyname(int);
char ungetc(char,struct _iobuf *);
int unlink(char *);
int write(int,char *,int);
# 8 "cpm.c" 2


# 1 "deadlin.h" 1
# 11 "cpm.c" 2
# 25 "cpm.c"
# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/errno.h" 1
# 26 "cpm.c" 2

char FilePath[14];
char DriveLetter[3];
char FileExt[5];
char BaseName[11];
extern int Status;






int OpenFile(char *path,struct _iobuf **stream)
{
 char *ptr;
        char Path[14];



 strcpy(Path,path);
 if((ptr = strchr(Path,'\\'))!= 0)
  *(ptr+1) = '\0';
 else strcpy(Path,".");






 if(Path[1]==':'){
  DriveLetter[0]=Path[0];
  DriveLetter[1]=':';
  DriveLetter[2]='\0';
 }
        if((ptr = strchr(path,'\\'))!=0)
   strcpy(FilePath,ptr+1);
 else strcpy(FilePath,path);

 strcpy(BaseName,FilePath);
        if((ptr = strchr(BaseName,'.'))!=0){
      strcpy(FileExt,ptr+1);
      *ptr = '\0';
 }

 if(strcmp(FileExt,"bak")==0){
  fprintf((&_iob[2]),"Cannot edit .BAK file--rename file\n");
  exit(0);
 }
# 83 "cpm.c"
 if(access(FilePath,0)==-1)
  if(errno==2){
   ( (Status) |= (0x04) );
   return 1;
  }

 if((*stream = fopen(FilePath,"r")) == 0) return 0;
 return 1;

}




int
init_tty(int fd){
 return 0;
}
# 111 "cpm.c"
int DoBackspace(int fd,int reps){
 int i;
 char BackSpace[] = "\x06";
 char Space[]=" ";
 for(i=0;i<reps;i++){
  write(fd,BackSpace,1);
  write(fd,Space,1);
  write(fd,BackSpace,1);
 }
 return 0;
}
