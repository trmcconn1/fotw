# 1 "parse.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "parse.c"




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
# 6 "parse.c" 2
# 1 "strtok.h" 1

char* strtok(register char *, register const char *);
# 7 "parse.c" 2
# 1 "strstr.h" 1

char *
strstr(register const char *, register const char *);
# 8 "parse.c" 2
# 1 "strpbrk.h" 1
char * strpbrk(
 register const char *,
 register const char *);
# 9 "parse.c" 2







# 1 "/home/dtz/8BIT/68K/CPM1.3/DISK4/ctype.h" 1
# 31 "/home/dtz/8BIT/68K/CPM1.3/DISK4/ctype.h"
extern char ___atab();
extern char __atab[];
# 17 "parse.c" 2
# 1 "deadlin.h" 1
# 18 "parse.c" 2
# 1 "dosedit.h" 1
# 19 "parse.c" 2
# 46 "parse.c"
extern char **Lines;
extern char *LineBuf;
extern int Status;
extern int MaxLines;
extern int MemAvail;
extern int MemStart;
extern int LinesToAppend;
extern char ControlChars[][3];




extern int CurrentLine,LastLine,CutStart,CutEnd,DestLine,Count;
extern char FileExt[];
extern char FilePath[];
extern char BaseName[];
extern struct _iobuf *tempfile;



extern void* MyMalloc(int);
extern int StringFree(void *);
extern int ReadLine(struct _iobuf *,char *);
extern int cleanup(void);
extern int DoBackspace(int,int);
void PrintLine(char *);
int GetNextCmd(char *, char *);
int GetCmdLetter(char *,int *);
int GetRange(char *command, char letter);
int myatoi(char *);
void InputError(void);
void Unescape(char *);
char *ReplaceString(char *,char *, char *,int);
# 105 "parse.c"
int Parse(struct _iobuf *input)
{
 int number = 0;
 int cmdlen = 0;
 int lastlen = 0;
 int rval = 0;
 int charpos;
 char c,*ptr;
 int i,j,k,l;
 static char PrevLine[255];
 static char buffer[255];
 static char Search[255];
 static char Replace[255];
 struct _iobuf *sourcefile;
 struct _iobuf *ostream;


    if(PrevLine[0] == '\0'){

       fprintf((&_iob[2]),"%s","*");
       if(!ReadLine(input,LineBuf))

  return 0;
 else strcpy(PrevLine,LineBuf);
    }




    lastlen = strlen(PrevLine);

    if((cmdlen = GetNextCmd(PrevLine,buffer))){
   switch( c = GetCmdLetter(buffer,&charpos) ) {
  case 6:
   break;



         case 'A':
  case 'a':
     switch(GetRange(buffer,c)){
                                case 0x01:
                                        LinesToAppend = CutStart+1;
                                        break;
                                case 0x00:
     LinesToAppend = MaxLines;
                                        break;
                                case 0xFF:
                                default:
                                        InputError();
                                        goto reset;
                        }





   rval = '\12';
   break;

  case 'w':
  case 'W':
   switch(GetRange(buffer,c)){
                                case 0x01:
     k = CutStart+1;
                                        break;
                                case 0x00:
                                        k = LastLine+1;
                                        break;
                                case 0xFF:
                                default:
                                        InputError();
                                        goto reset;
                        }

   strcat(BaseName,".$$$");



     ostream = fopen(BaseName,"a");

   for(i=0;i<k;i++){
    if(4*j >= 3*MemStart)break;
    fprintf(ostream,"%s",Lines[i]);



    j += StringFree(Lines[i]);
   }
   fclose(ostream);
   BaseName[strlen(BaseName)-4]='\0';

   for(j=i;j<=LastLine;j++)
    Lines[j-i] = Lines[j];
   LastLine -=i;
   break;

  case 'I':
  case 'i':
   switch(GetRange(buffer,c)){
    case 0x01:
         CurrentLine = CutStart;
     break;
    case 0xFF:
     goto reset;
    case 0x00:
     break;
    default:
     InputError();
     goto reset;
   }

   ( (Status) |= (0x02) );
   break;
  case 'Q':
  case 'q':
   printf("Abort edit(Y/N)? ");
   j = fgetc(input);
   if((j=='y')||(j=='Y')){
        printf("\n");
        cleanup();
        exit(0);
   }
   fflush(input);
   return 0;
  case 'e':
  case 'E':
   return '\11';
  case 2:
   number = myatoi(
    strtok(buffer," \t\n;" ));
          if(strchr(buffer,'+'))
            CurrentLine += number;
          else if(strchr(buffer,'-'))
    CurrentLine += number;
         else if(number) CurrentLine = number -1;
    else CurrentLine++;
   if(CurrentLine < 0)CurrentLine = 0;
   if(CurrentLine > LastLine){
     CurrentLine = LastLine+1;
     break;
   }
   if((CurrentLine)==CurrentLine)printf("%9d:*",(CurrentLine)+1); else printf("%9d: ",(CurrentLine)+1);
   PrintLine(Lines[CurrentLine]);
   if((CurrentLine)==CurrentLine)printf("%9d:*",(CurrentLine)+1); else printf("%9d: ",(CurrentLine)+1);
   j = ReadLine((&_iob[0]),LineBuf);

   if((j==254) &&
    (LineBuf[254]!='\n'))
    {
      LineBuf[254] =
     '\n';
      fprintf((&_iob[2]),"\a");
    }
   if(j>1)
     if(ptr = (char *)MyMalloc(strlen(LineBuf)+1)){
      StringFree(Lines[CurrentLine]);
      strcpy(ptr, LineBuf);
      Lines[CurrentLine] = ptr;
            }

  break;

  case 'l':
  case 'L':
   switch(GetRange(buffer,c)){
    case 0x01:
         CutEnd = CutStart + 22;
     break;
    case 0x00:
    case 0x02:
    case 0x01|0x02:
     break;
    case 0xFF:
     CutStart = ( (CurrentLine - 11) > 0 ? CurrentLine - 11 : 0 );
     CutEnd = CurrentLine + 11;
     break;
    default:
     InputError();
     goto reset;
   }
   CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
   for(i=CutStart;i<= CutEnd;i++){
    if((i)==CurrentLine)printf("%9d:*",(i)+1); else printf("%9d: ",(i)+1);
           PrintLine(Lines[i]);
   }
   break;

  case 'p':
  case 'P':
   switch(GetRange(buffer,c)){
    case 0x02:
      CutStart = CurrentLine + 1;
     break;
    case 0x01:
     CutEnd = CutStart + 22;
     break;
    case 0x00:
     CutStart = CurrentLine + 1;
     CutEnd = CutStart + 22;
     break;
    case 0x01|0x02:
     break;
    case 0xFF:
     goto reset;
    default:
     InputError();
     goto reset;
   }
   CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
   CurrentLine = CutEnd;
   for(i=CutStart;i<= CutEnd;i++){
    if((i)==CurrentLine)printf("%9d:*",(i)+1); else printf("%9d: ",(i)+1);
           PrintLine(Lines[i]);
   }
   break;

  case 'd':
  case 'D':
   switch(GetRange(buffer,c)){
    case 0x02:
     CutStart = CurrentLine;
     break;
    case 0x01:
     CutEnd = CutStart;
     break;
    case 0x00:
     CutEnd = CutStart = CurrentLine;
     break;
    case 0x01|0x02:
     break;
    case 0xFF:
     goto reset;
    default:
     InputError();
     goto reset;
   }

   for(i=CutStart;i<=CutEnd;i++)
    StringFree(Lines[i]);

   for(i=CutEnd+1;i<=LastLine;i++)
    Lines[CutStart + i - CutEnd-1] =
     Lines[i];

   for(i=CutStart+LastLine-CutEnd;i<=LastLine;i++)
    Lines[i]= 0;
   LastLine -= (CutEnd - CutStart+1);
   CurrentLine = CutStart;
   break;

  case 'c':
  case 'C':
   i=GetRange(buffer,c);
   if(i==0xFF)goto reset;
   if(!(i & 0x01)) CutStart = CurrentLine;
   if(!(i & 0x02)) CutEnd = CurrentLine;
   if((CutEnd >= DestLine)&&(CutStart <= DestLine)){
    InputError();
    break;
   }







   k = (CutEnd-CutStart+1)*Count;
   k = ( k < MaxLines - LastLine - 1 ? k :
     MaxLines - LastLine -1);





   for(i=0;i< k/(CutEnd-CutStart+1);i++)
    for(j=0;j< (CutEnd-CutStart+1);j++){
     if(!(Lines[MaxLines-k + i*(CutEnd-CutStart+1) + j]
      = (char *)MyMalloc(strlen(Lines[CutStart
                        +j])+1)))goto failed;
     strcpy(Lines[MaxLines-k+i*(CutEnd-CutStart+1)+j],
      Lines[CutStart+j]);
    }
   for(j=0;j<(k%(CutEnd-CutStart+1));j++){
    if(!(Lines[MaxLines-k + i*(CutEnd-CutStart+1) + j]
     = (char *)MyMalloc(strlen(Lines[CutStart
                       +j])+1)))goto failed;
    strcpy(Lines[MaxLines-k+i*(CutEnd-CutStart+1)+j],
     Lines[CutStart+j]);
   }
failed: k = i*(CutEnd-CutStart+1) + j ;
# 409 "parse.c"
    for(i=0;i<k;i++){


     ptr = Lines[MaxLines - k + i];


     for(j=LastLine+i;j>=DestLine+i;j--)
      Lines[j+1]=Lines[j];

     Lines[DestLine+i] = ptr;
    }
   CurrentLine = DestLine;
   LastLine += k;
   break;

  case 'm':
  case 'M':
   i=GetRange(buffer,c);
   if(i==0xFF)goto reset;
   if(!(i & 0x01)) CutStart = CurrentLine;
   if(!(i & 0x02)) CutEnd = CurrentLine;
   if((CutEnd >= DestLine)&&(CutStart <= DestLine)){
    InputError();
    break;
   }


        if(DestLine > CutEnd){
   for(i=CutEnd;i>=CutStart;i--){
    ptr = Lines[i];


    for(j=i;j<=DestLine-(CutEnd-i)-2;j++)
     Lines[j]=Lines[j+1];

    Lines[DestLine-(CutEnd-i)-1] = ptr;
    }
   CurrentLine = DestLine-CutEnd+CutStart-1;
        }
        else {
   for(i=CutStart;i<=CutEnd;i++){
    ptr = Lines[i];


    for(j=i;j>=DestLine+(i-CutStart)+1;j--)
     Lines[j]=Lines[j-1];

    Lines[DestLine+(i-CutStart)] = ptr;
    }
   CurrentLine = DestLine;
        }
   break;

  case 't':
  case 'T':
   switch(GetRange(buffer,c)){
    case 0x01:
         CurrentLine = CutStart;
     break;
    case 0xFF:
     goto reset;
    case 0x00:
     break;
    default:
     InputError();
     goto reset;
   }




   i=charpos+1;
   if((sourcefile =
         fopen(strtok(buffer+i," \t\n;"),"r"))==0){
    fprintf((&_iob[2]),"File not found\n");
    goto reset;
   }
   while(fgets(LineBuf,254,sourcefile)){
    ptr = (char *)MyMalloc(strlen(LineBuf)+1);
    if(!ptr)break;
    strcpy(ptr,LineBuf);

    for(j=LastLine;j>=CurrentLine;j--)
     Lines[j+1]=Lines[j];
    Lines[CurrentLine++]=ptr;
    LastLine++;
   }
   fclose(sourcefile);
   break;

  case 5:
  case 's':
  case 'S':
   switch(GetRange(buffer,c)){
    case 0x01:
     CutEnd = LastLine;
     break;
    case 0x02:
     CutStart = CurrentLine+1;
     CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
     break;
    case 0x00:
     CutStart = CurrentLine+1;
     CutEnd = LastLine;
     break;
    case 0x01|0x02:
     CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
     break;
    case 0xFF:
     goto reset;
    default:
     InputError();
     goto reset;
   }
   i=charpos+1;
   if(strlen(buffer+i) > 1){
    strcpy(Search,strtok(buffer+i,"\n"));


    Unescape(Search);
   }
   while(1){
     for(j=CutStart;j<=CutEnd;j++)
    if(ptr=strstr(Lines[j],Search))break;
     if(ptr){
    if((j)==CurrentLine)printf("%9d:*",(j)+1); else printf("%9d: ",(j)+1);;
    PrintLine(Lines[j]);
    if(c==5){
     printf("O.K.? ");
     if(((i=fgetc((&_iob[0])))=='Y')||(i=='y')||(i
      =='\n')){
       CurrentLine = j;
       printf("\n");
       break;
     }
                CutStart = j+1;
     printf("\n");
    }
    else {
     CurrentLine = j;
     break;
    }
     }
     else {
    printf("Not found\n");
    break;
            }
   }
   break;

  case 'R':
  case 'r':
  case 4:
   switch(GetRange(buffer,c)){
    case 0x01:
     CutEnd = LastLine;
     break;
    case 0x02:
     CutStart = CurrentLine+1;
     CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
     break;
    case 0x00:
     CutStart = CurrentLine+1;
     CutEnd = LastLine;
     break;
    case 0x01|0x02:
     CutEnd = ( (CutEnd) < LastLine ? CutEnd : LastLine );
     break;
    case 0xFF:
     goto reset;
           default:
     InputError();
     goto reset;
   }

   i=charpos+1;







   k=0;
   if(buffer[cmdlen-1]==''){
    while(((j=PrevLine[cmdlen++])!='\n')
                                         &&(j!=''))Replace[k++]=j;
                     Replace[k] = '\0';
       cmdlen++;
    Unescape(Replace);
   }
   if(strlen(buffer+i) > 1){
    strcpy(Search,strtok(buffer+i,"\n"));



    if(!k)Replace[0]='\0';
    Unescape(Search);
   }
# 617 "parse.c"
   ptr = Replace;
   l = 0;
   while(ptr = strstr(ptr,Search)){
    l++;
    ptr += strlen(Search);
   }

          for(j=CutStart;j<=CutEnd;j++){
     k=1;
     while(ptr=ReplaceString(Lines[j],Search,Replace,k)){
    if((j)==CurrentLine)printf("%9d:*",(j)+1); else printf("%9d: ",(j)+1);;
    printf("%s",ptr);
    if(c==4){
     printf("O.K.? ");
     if(((i=fgetc((&_iob[0])))=='Y')||(i=='y')||(i
      =='\n')){
       StringFree(Lines[j]);
       Lines[j]=ptr;
       k+=l;
     }
     else k++;
     printf("\n");
    }
    else {
     StringFree(Lines[j]);
     Lines[j]=ptr;
     k+=l;
     break;
    }
     }
   }
   break;
  case 0:
  default:
   InputError();


   }



reset:
    for(i=0;i<=lastlen-cmdlen;i++)
 PrevLine[i] = PrevLine[cmdlen+i];
    PrevLine[i]='\0';
    }
    else
 PrevLine[0]='\0';
    return rval;
}
# 677 "parse.c"
int
GetNextCmd(char *instr, char *buffer)
{

 char *ptr;
 int c,d;
 char delimit[5] = "\n";


 c = GetCmdLetter(instr,&d);
 switch(c){
  case 'r':
  case 'R':
  case 4:
  case 5:
  case 'S':
  case 's':
   break;
  case 't':
  case 'T':
  case 2:
   delimit[2]=';';
   delimit[3]='\0';
   break;
  case 6:
   delimit[2]=';';
   delimit[3]='\0';
   break;
  default:
   delimit[2]=';';
   delimit[3]=c;
   delimit[4] = '\0';
 }

 strcpy(buffer, instr);
 ptr = strpbrk(buffer,delimit);
 if(ptr) {


    if((*ptr != ';')&&(*ptr != '\n')&&(*ptr != '')){
                ptr++;
  while((*(ptr) == ' ')||(*(ptr) == '\t') )(ptr)++;
  if((*ptr != '\n')&&(*ptr != ';')&&(*ptr != ''))ptr--;
    }
    *(ptr+1) = '\0';
     return strlen(buffer);
 }
 return 0;
}






int
GetCmdLetter(char *command, int *charpos)
{
 int state = 3;
 int i;
 char *ptr = command;




 while((*(ptr) == ' ')||(*(ptr) == '\t') )(ptr)++;
 if((*ptr == '\n')||(*ptr == ';')||(*ptr == '')) return 2;
 if((*ptr=='.')||(*ptr=='#')){
  ptr++;
  while((*(ptr) == ' ')||(*(ptr) == '\t') )(ptr)++;
  if((*ptr == '\n')||(*ptr == ';'))
          return 2;
  else state = 1;
 }

 if(state == 3)
 while((ptr - command) < strlen(command))
  if(!(__atab[*ptr] & 04)) {
    switch(*ptr){
   case '\n':
   case ';':
   case '':
    break;
   case '+':
   case '-':
    ptr++;
    continue;
   default:
   state = 1;
   break;
    }
  break;
  }
  else ptr++;
 if(state == 3) return 2;




 ptr = strpbrk(command,"aAcCdDeEiIlLmMpPqQrRsStTwW?");
 if(ptr){
    if(*ptr == '?'){

  ptr++;
  while((*(ptr) == ' ')||(*(ptr) == '\t') )(ptr)++;
  *charpos = ptr - command;
  if((*ptr == 'r') || (*ptr == 'R'))
   return 4;
  if((*ptr == 's') || (*ptr == 'S'))
   return 5;
  return 0;
    }
    *charpos = ptr - command;
    return *ptr;
 }
 return 0;
}
# 807 "parse.c"
int
GetRange(char *command, char letter)
{
 int sign;
 int fields = 0;
 int currentfieldbit=1;
 int j;
 char buffer[255];
 char delimit[5] = ",?";
 char *ptr,c;



 ptr = buffer;
 if((letter == 4)||(letter == 5))
  letter = '?';
 while(*command != '\0'){
  if((*command == ' ')||(*command == '\t')){
   command++;
   continue;
  }
  *ptr = *command++;
  if(*ptr++ == letter)break;
 }
 *ptr = '\0';

 ptr = buffer;




 CutStart = ( (CurrentLine - 11) > 0 ? CurrentLine - 11 : 0 );
 CutEnd = ( (22 + CutStart) <= LastLine + 1 ? 22 + CutStart : LastLine + 1);

 Count = 1;

 if((*ptr == letter)||(strlen(ptr) == 0))return fields;



      delimit[2] = letter;
      delimit[3]='\0';


 if(*ptr == ','){
  currentfieldbit = currentfieldbit << 1;
  ptr++;
  if(*ptr == ',')
   currentfieldbit = currentfieldbit << 1;
 }

 ptr = strtok(buffer,delimit);
 if(strlen(ptr)) fields |= currentfieldbit;

 while(1){
 if(strlen(ptr)) {
  switch(c = *ptr){

   case '+':
    if(currentfieldbit==0x08){
     InputError();
     return 0xFF;
    }
    sign = 1;
    ptr++;
    break;
   case '-':
    if(currentfieldbit==0x08){
     InputError();
     return 0xFF;
    }
    sign = -1;
    ptr++;
    break;
   default:
    sign = 0;
  }
  j = myatoi(ptr)-1;
  if(sign)
   j = CurrentLine + sign*(j+1);

  switch(currentfieldbit){
   case 0x01:
    CutStart = ( (( (j) > 0 ? j : 0 )) <= LastLine + 1 ? ( (j) > 0 ? j : 0 ) : LastLine + 1);
    break;
   case 0x02:
    if(j<CutStart){

     return 0xFF;
    }
    CutEnd = ( (j) <= LastLine + 1 ? j : LastLine + 1);
    break;
   case 0x04:
    DestLine = ( (( (j) > 0 ? j : 0 )) <= LastLine + 1 ? ( (j) > 0 ? j : 0 ) : LastLine + 1);
    break;
   case 0x08:
    Count = j+1;
    break;
   default:
    InputError();
           return 0xFF;
  }
 }




 if(*(ptr+strlen(ptr)+1)==',') currentfieldbit = currentfieldbit << 1;


 ptr = strtok((char *)0,delimit);



 if(!ptr) return fields;

 currentfieldbit = currentfieldbit << 1;
 if(strlen(ptr))
       fields |= currentfieldbit;
 }
}







int
myatoi(char *astring) {
 if(astring == 0) return 0;
 if(strcmp(astring,"#") == 0)return LastLine + 2;
 if(strcmp(astring,".") == 0)return CurrentLine + 1;
 return atoi(astring);
}

void
InputError(void)
{
 fprintf((&_iob[2]),"Entry error\n");
 return;
}





char
*ReplaceString(char *dest,char *old, char *new,int n){

 char *ptr = dest,*newdest;
 int k;


 for(k=0;k<n;k++){
   if((ptr = strstr(ptr,old))==0)return 0;
   if (k == n-1)break;
   ptr += strlen(old);
 }



 if((k= strlen(dest) + strlen(new)-strlen(old))>254)
  return 0;
 if((newdest = (char *)MyMalloc(k + 1))
  ==0)return 0;




  for(k=0;k<(ptr-dest);k++)*(newdest+k)=*(dest+k);


  *(newdest+k)='\0';
  strcat(newdest,new);


  strcat(newdest,ptr+strlen(old));

  return newdest;
}




void
Unescape(char *strng)
{
 int i;
 int j=0;
 int flag = 0;

 for(i=0;i<strlen(strng);i++){
  if(strng[i]==''){
   flag = 1;
   continue;
  }
  if(flag){
    if(strng[i] == 'D')
    strng[j++] = '';
   else {
    strng[j++]='';
    strng[j++] = strng[i];
   }
  flag = 0;
  }
  else strng[j++] = strng[i];
 }
 strng[j] = '\0';
 return;
}




void
PrintLine(char *strng)
{
 int j;
 for(j=0;j<strlen(strng)-1;j++)
  if(*(strng+j)<32){
   if(*(strng+j)=='\t')
    printf("\t");
   else printf("%c^",ControlChars[(int)*(strng+j)][1]);
  }
  else printf("%c",*(strng+j));
 printf("\n");
 return;
}
