# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.c"
# 12 "main.c"
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
# 13 "main.c" 2

# 1 "deadlin.h" 1
# 15 "main.c" 2


# 1 "unix.h" 1
# 18 "main.c" 2


# 1 "dosedit.h" 1
# 21 "main.c" 2
# 31 "main.c"
char **Lines;
char *LineBuf;
int Status;
int LinesToAppend;





long BrkVal = 65535;
long MemAvail;
long MemStart;

int MaxLines = 8191;



int CurrentLine=0,LastLine=-1,CutStart,CutEnd,DestLine,Count;
extern int TabSize;
extern char FileExt[];
extern char FilePath[];
extern char BaseName[];
# 62 "main.c"
void *MyMalloc(int);
long StringFree(void *);
extern int Parse(struct _iobuf *);
extern int ReadLine(struct _iobuf *,char *);
extern int OpenFile(char *,struct _iobuf **);
extern int init_tty(int);
extern int cleanup(void);
int init_mem(void);

int main(int argc, char **argv)
{
 struct _iobuf *istream = (&_iob[0]);
 struct _iobuf *ostream ;



        long fpos;

 char *ptr;
 int i=0;




 if(argc > 1) {
  while(argc > 1)
    if((*++argv)[0]=='-')
   switch((*argv)[1]){
    case 'b':
    case 'B':
     ( (Status) |= (0x10) );
     argc--;
     break;
    case 'T':
    case 't':
     ++argv;
     TabSize = atoi(*argv);
     argc = argc -2;
     break;
    case 'm':
    case 'M':
     ++argv;

                                        BrkVal = atoi(*argv);



     argc = argc - 2;
     break;
    default:
     fprintf((&_iob[2]),"Invalid Parameter\n");
     exit(1);
   }
  else {
   if(( (Status) & (0x40) )){
    fprintf((&_iob[2]),"Invalid Parameter\n");
    exit(1);
   }
   ( (Status) |= (0x40) );
          if(!OpenFile(*argv,&istream)){
    ( (Status) |= (0x04) );
    fprintf((&_iob[2]),"Error Opening %s\n",FilePath);
   }
   argc--;
  }
 }
 else {
  fprintf((&_iob[2]),"File name must be specified\n");
  exit(1);
 }
 if(!( (Status) & (0x40) )){
  fprintf((&_iob[2]),"File name must be specified\n");
  exit(1);
 }



 init_tty(0);



 init_mem();
 MemStart = MemAvail;

 if(( (Status) & (0x04) ))
  fprintf((&_iob[2]),"New File\n");

 else {



                fpos = ftell(istream);

  while(ReadLine(istream,LineBuf)){
                        if(ptr = (char *)MyMalloc(strlen(LineBuf)+1)){
    Lines[i] = ptr;
    strcpy(Lines[i++],
      LineBuf);




                                fpos = ftell(istream);

    }

   else {



                                fpos = ftell(istream);

    ( (Status) |= (0x08) );
    break;
   }
  }
  if(!( (Status) & (0x08) ))
        fprintf((&_iob[2]),"End of Input File\n");
  LastLine = i-1;
 }



 while((1))
 {
  if(( (Status) & (0x02) )) {
   if((CurrentLine)==CurrentLine)printf("%9d:*",(CurrentLine)+1); else printf("%9d: ",(CurrentLine)+1);
   while(ReadLine((&_iob[0]),LineBuf)){
     if(LineBuf[0]=='')break;


     if((LineBuf[0]=='')&&(LineBuf[1]=='D')){
    LineBuf[0]='';
    LineBuf[1]='\0';
    strcat(LineBuf,LineBuf+2);
     }






                                if(ptr= (char *)MyMalloc(strlen(LineBuf)+1)){



     for(i=LastLine++;i>=CurrentLine;i--)
      Lines[i+1] = Lines[i];

     Lines[CurrentLine] = ptr;
     strcpy(Lines[CurrentLine++],
      LineBuf);
    }
    else {
     ( (Status) |= (0x08) );
     break;
    }
   if((CurrentLine)==CurrentLine)printf("%9d:*",(CurrentLine)+1); else printf("%9d: ",(CurrentLine)+1);
   }
  ( (Status) &= (~(0x02)) );
  }
  else {

   switch(Parse((&_iob[0]))){
    case '\11':



     if(!( (Status) & (0x04) )){
      rewind(istream);
      strcat(BaseName,".bak");
      ostream = fopen(BaseName,"w");



      while(ReadLine(istream,LineBuf))
       fprintf(ostream,"%s",LineBuf);
      fclose(istream);
      fclose(ostream);

                    BaseName[strlen(BaseName)-4]
       = '\0';
     }






     strcat(BaseName,".$$$");
     ostream = fopen(BaseName,"a");




     for(CurrentLine=0; CurrentLine <= LastLine;
      CurrentLine++)
      fprintf(ostream,"%s",Lines[CurrentLine]);



      if((LastLine>=0)&&(*(Lines[LastLine] +
       strlen(Lines[LastLine])-1)
       != '\n'))
       fprintf(ostream,"\n");




     fprintf(ostream,"");

     fclose(ostream);
     unlink(FilePath);
     rename(BaseName,FilePath);




     BaseName[strlen(BaseName)-4]='\0';
     cleanup();
     return 0;

    case '\12':
     i=1;



                                        fpos = ftell(istream);

            ( (Status) &= (~(0x08)) );
     while(ReadLine(istream,LineBuf)){
      if(i>LinesToAppend){
                                                        ( (Status) |= (0x08) );
       break;
      }
                                                if(ptr = (char *)MyMalloc(strlen(LineBuf)+1)){
                                    Lines[LastLine+i] = ptr;
                                    strcpy(Lines[LastLine + i++],
                                                                    LineBuf);



                                                  fpos = ftell(istream);

                                         }
                                                else {



                                                 fpos = ftell(istream);


                                                 ( (Status) |= (0x08) );
                                                 break;
                                                }
                                        }
                                        if(!( (Status) & (0x08) ))
                                           fprintf((&_iob[2]),"End of Input File\n");
                                        LastLine += i-1;
     break;

    case 0:
    default:
     break;
   }
  }
 }
}






void *MyMalloc(int bytes)
{
 void *ptr;




 ptr = malloc(bytes);
 if( ptr == (void *)0) {

  fprintf((&_iob[2]),"deadlin: Could not alloc %d bytes\n",bytes);
  exit(1);
 }
 MemAvail -= bytes;
 return ptr;
}







long
StringFree(void *ptr)
{
 long i;
 i = (long)strlen(ptr);
 MemAvail += i+1;
 free(ptr);
 return i;
}
# 376 "main.c"
int
init_mem(void)
{
        void *ptr;



        int MemGot;





        MemGot = BrkVal;



        while( (ptr = malloc(MemGot)) == 0 ){


                MemGot = MemGot/2;
                if(MemGot == 0)break;
        }
        MemAvail = MemGot;
        free(ptr);






 LineBuf = (char *)MyMalloc( ((int) ((254 + 1)*sizeof(char)) ) );


 if(LineBuf==0){
  fprintf((&_iob[2]),"Fatal: unable to allocate enough memory to run!\n" );
  exit(1);
 }

 MemGot = 8191*sizeof(char *);

 while((Lines = MyMalloc(MemGot))==0){
  MemGot = MemGot/2;
  if(MemGot == 0)break;
 }
 MaxLines = MemGot/sizeof(char *);



 if(BrkVal != 65535)
  fprintf((&_iob[2]),"Requested %ld bytes, Available = 3/4 of %ld bytes, MaxLines = %d\n",
   BrkVal,MemAvail,MaxLines);

        return (1);
}






int
init_printer(void)
{
# 448 "main.c"
 return 1;

}

int
flush_printer(void)
{
# 463 "main.c"
 return 1;

}


int
cleanup(void)
{






        strcat(BaseName,".$$$");
        unlink(BaseName);
}
