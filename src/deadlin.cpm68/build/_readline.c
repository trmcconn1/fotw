# 1 "readline.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "readline.c"
# 16 "readline.c"
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
# 17 "readline.c" 2




# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/ctype.h" 1
# 31 "/home/dtz/8BIT/68K/CPM1.3/DISK4/ctype.h"
extern char ___atab();
extern char __atab[];
# 22 "readline.c" 2

# 1 "deadlin.h" 1
# 24 "readline.c" 2
# 1 "dosedit.h" 1
# 25 "readline.c" 2







extern char **Lines;
extern char *LineBuf;
extern int CurrentLine,LastLine,CutStart,CutEnd;
extern int Status;

int ParseAltNum(struct _iobuf *);
extern int DoBackspace(int,int);

extern int init_printer(void);
extern int flush_printer(void);



char ControlChars[][3] = { "^@","^A","^B","^C","^D","^E","^F","^G",
"^H","^I","^J","^K","^L","^M","^N","^O","^P","^Q","^R","^S","^T","^U",
"^V","^W","^X","^Y","^Z","^[","^\\","^]","^^","^_"};

int TabSize = 8;

int ReadLine(struct _iobuf *input, char *buffer)
{

 char *s,*p,*pl;
 char c;
 int i,j;
 int escaped = 0;
 static char PreviousLine[254 +1];
 static char CharSizes[254 +1];




 s = buffer;
 pl = PreviousLine;


 if(input == (&_iob[0]))
   while(1){

  c = fgetc(input);





  if((s-buffer >= 254 -1)&&(c != '\n')){
   DoBackspace(1,1);
   fprintf((&_iob[2]),"\a");
   continue;
  }





  if(escaped && (s-buffer<254 -1)){
    escaped = 0;
    if(!( (Status) & (0x80) ))pl++;
    if(c == 'D'){



      CharSizes[((s)-(buffer))]=2;
      *s++ = '';
      CharSizes[((s)-(buffer))]=2;
      *s++ = 'D';
      continue;
    }


    if(c == '\n'){
         CharSizes[((s)-(buffer))]=1;
         *s++ = c;
         continue;
           }




    if(c<32){
         CharSizes[((s)-(buffer))]=2;
         *s++='';
         CharSizes[((s)-(buffer))]=2;
         *s++=c;
         continue;
           }
           i=0;
    while((c != ControlChars[i][1])&&(i<32))i++;
    if(i<32){
        CharSizes[((s)-(buffer))] = 3;
        *s++=i;
        continue;
    }
    CharSizes[((s)-(buffer))]=2;
    *s++ = '';
    CharSizes[((s)-(buffer))]=1;
    *s++ = c;
    continue;
  }

  switch(c){

    case 0x03:
      ((input)->_flag &= ~0x10);
             fprintf((&_iob[2]),"\n");
      return 0;

    case '	':







     j=11;
     for(i=0;i< ((s)-(buffer));i++)
      j+=CharSizes[i];
     CharSizes[((s)-(buffer))] = TabSize-(j%TabSize);





     *s++=c;
            if(!( (Status) & (0x80) ))pl++;
     continue;

    case '':
     DoBackspace(1,2);
     if(( (Status) & (0x20) )){
      ( (Status) &= (~(0x20)) );
      flush_printer();
     }
     else{
      if(init_printer()){
       fprintf((&_iob[2]),"Unable to open PRN device\n");
       continue;
      }
     ( (Status) |= (0x20) );
     }
     continue;


    case '\n':
    case 0x0D:
     *s++ = '\n';
     *s = '\0';
     strcpy(PreviousLine,buffer);
     return strlen(buffer);
    case '':
     DoBackspace(1,2);




     if(s-buffer == 0)continue;
          s--;
                 if(!( (Status) & (0x80) ))pl--;
          else ( (Status) &= (~(0x80)) );
     DoBackspace(1,CharSizes[((s)-(buffer))]);
     continue;
    case '':
     DoBackspace(1,2);
     if(i=ParseAltNum(input)){
      if(i<32)
                CharSizes[((s)-(buffer))]=2;
      else CharSizes[((s)-(buffer))]=1;
      *s++=i;
     }
     continue;

    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     if(pl<PreviousLine+strlen(PreviousLine))
      pl++;
     continue;
    case '':
     DoBackspace(1,2);
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     else ( (Status) |= (0x80) );
     continue;
    case '':
     DoBackspace(1,2);
     printf("\\\n");
     printf("%11s"," ");
     s=buffer;
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     pl=PreviousLine;
     continue;
    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     if(pl-PreviousLine < strlen(PreviousLine)-1){
      *s = *pl++;
      if(*s<32){
       CharSizes[((s)-(buffer))]=2;
                     printf("%c^",ControlChars[*s][1]);
      }
      else {
       CharSizes[((s)-(buffer))]=1;
                     printf("%c",*s);
      }
      s++;
     }
     continue;
    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     if(p=strchr(PreviousLine,fgetc(input))){
      DoBackspace(1,1);
      while(pl<p){
        *s = *pl++;
        if(*s<32){
       CharSizes[((s)-(buffer))]=2;
                     printf("%c^",ControlChars[*s][1]);
         }
         else {
       CharSizes[((s)-(buffer))]=1;
                     printf("%c",*s);
         }
       s++;
      }
     }
     continue;
    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     while(pl-PreviousLine<strlen(PreviousLine)-1){
      *s = *pl++;
        if(*s<32){
       CharSizes[((s)-(buffer))]=2;
                     printf("%c^",ControlChars[*s][1]);
         }
         else {
       CharSizes[((s)-(buffer))]=1;
                     printf("%c",*s);
         }
       s++;
     }
     continue;
    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     if(p=strchr(PreviousLine,fgetc(input)))
      pl = p;
     DoBackspace(1,1);
     continue;
    case '':
     if(( (Status) & (0x80) ))( (Status) &= (~(0x80)) );
     DoBackspace(1,2);
     printf("@\n");
     printf("%11s"," ");;
     *s++ = '\n';
     *s = '\0';
     strcpy(PreviousLine,buffer);
     s=buffer;
     pl=PreviousLine;
     continue;
    case '':
     escaped=1;
     continue;
    default:



     if(c<32)
      CharSizes[((s)-(buffer))]=2;
     else
         CharSizes[((s)-(buffer))] = 1;
     *s++ = c;
     if(!( (Status) & (0x80) ))pl++;
     continue;
   }
   }
 else {
  while(1){
   c = fgetc(input);
   if(c == (-1)){
    if( s > buffer ){
     *s =='\0';
     return(strlen(buffer));
    }
    else return 0;
   }

          if((s-buffer >= 254 -1)||(c=='\n')){
    *s++ = c;
    *s = '\0';
    return(strlen(buffer));
   }
   if(c== '')
     if(!( (Status) & (0x10) )){
    *s = '\0';
    return 0;
     }
   *s++ = c;
  }
 }
# 341 "readline.c"
}
# 354 "readline.c"
int
ParseAltNum(struct _iobuf *input)
{
 int result = 0;
 int c;
 int stage = 0;

 while(c=fgetc(input)){
  if((stage<=2)&&((c=='')||(__atab[c] & 04))){
   if(c == '') {
    DoBackspace(1,2);
    continue;
   }
          result = 10*result + (c - '0');
   stage++;
   DoBackspace(1,1);
   continue;
  }
  else {
   if(stage >= 2){
      if(c == ''){
    DoBackspace(1,2);
    if(result <= 255)
       if(result > 31)
               printf("%c",result);
       else printf("%c^",ControlChars[result][1]);
    return result <= 255 ? result : 0;
             }
      else {
    DoBackspace(1,1);
    if(result<=255)
        if(result > 31)
            printf("%c",result);
         else printf("%c^",ControlChars[result][1]);
           printf("%c",c);
    ungetc(c,input);
    return result <= 255 ? result : 0;
      }
   }
   else {
    ungetc(c,input);
    return stage > 0 ? result : 0;
   }
  }
 }
}
