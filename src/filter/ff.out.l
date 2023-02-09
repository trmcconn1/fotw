%{

/*	reverse.ff: reverse input */
/*      options: default: reverse by word = string of alphanumerics
		-W:  reverse by  Word = string of non whitespace
		-w:  reverse by Word, except leave period in place
		-l:  reverse by line
		-s:  reverse by sentence
		-d:  next argument is a string. Reverse blocks delimited
		     by this string. The string is assumed to occur just
		     before the beginning of the file, but not just after
		     the end.
		-n:  next argument (k) means reverse blocks of k chars
		-f:  reverse by file. Next argument is larger than size
			of file (to allocate a big buffer.)
*/
#define BUFSIZE 8192
#define INFO "                       CHICKEN HAVEN SOFTWARE  (T. McConnell)
                          fgn, version 1.2 
	reverse: reverse input.
	usage:   reverse [-Wwlsnfd-] [arg] [input-file]
	options: default: reverse by word = string of alphanumerics.
		-W:  reverse by  Word = string of non whitespace
		-w:  reverse by Word, except leave period in place
		-l:  reverse by line
		-s:  reverse by sentence
		-n:  next argument (k) means reverse blocks of k chars.
		-d:  next argument is a string. Reverse blocks delimited
		     by this string. The string is assumed to occur just
		     before the beginning of the file, but not just after
		     the end.
		-f:  reverse by file. The next argument give size of
	             buffer to allocate. Must be big enough to hold
		     the input file, or you will be in a world of hurt.
Reads from stdin if no filename or - is given as a filename."

static char buf[BUFSIZE]; /* buffer for line or word-based reverse */
char *fbuf; /* buffer for file reverse-allocated dynamically */
char *bufend = buf;
char *bufptr;
int charcnt=0;
int length;
void reverse(char s[]);
char *strdup(char *);
#define NVARS 10
char *var[NVARS];
char *clvar;
%}
%s s__
%s s_w
%s s_W
%s s_l
%s s_s
%s s1
%s s0
%s s_d
%s s_n
%s s_f
%%
<s__>[a-zA-Z0-9]+	{strcpy(buf,yytext);reverse(buf);printf("%s",buf);}
<s__>.|[\n]	{ECHO;}
<s_w>[^. \t\n]+	{strcpy(buf,yytext);reverse(buf);printf("%s",buf);}
<s_w>.|[\n]	{ECHO;}
<s_W>[^ \t\n]+	{strcpy(buf,yytext);reverse(buf);printf("%s",buf);}
<s_W>.|[\n]	{ECHO;}
<s_l>.+	{strcpy(buf,yytext);reverse(buf);printf("%s",buf);}
<s_l>.|[\n]	{ECHO;}
<s_s>[^.!?]+[.?!]	{BEGIN s0; yyless(0);}
<s0>[^.!?]+	{var[0]=strdup(yytext);BEGIN s1;}
<s0>.|[\n]	{ECHO;}
<s1>[.?!]	{var[1]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);
	printf("%s",buf);printf("%s",  var[1] );BEGIN s_s;}
<s1>.|[\n]	{ECHO;}
<s_d>.|[\n]	{length=strlen(clvar);
		if((charcnt>=length)&&(strcmp(bufend-length,
		clvar)==0)){reverse(buf);
		printf("%s",buf);charcnt=0;*buf='\0';bufend=buf;}else{
		strcat(buf,yytext);++bufend;++charcnt;}}
<s_d>.|[\n]	{ECHO;}
<s_d><<EOF>>	{printf("%s",buf);return 0;}
<s_d>.|[\n]	{ECHO;}
<s_n>.	{ if(charcnt == atoi(clvar)-1){strcat(buf,yytext);reverse(buf);
		printf("%s",buf);charcnt=0; buf[0]='\0';}
		 else {strcat(buf,yytext);
		charcnt++;}}
<s_n>.|[\n]	{ECHO;}
<s_n><<EOF>>	{printf("%s",buf);return 0;}
<s_n>.|[\n]	{ECHO;}
<s_f>.|[\n]	{ if(charcnt==0)if((fbuf=(char *)malloc(atoi(clvar)))==NULL){
		printf("Unable to allocate buffer\n");exit(1);}else
			bufptr=fbuf;
		*fbuf++=yytext[0];charcnt++;}
<s_f>.|[\n]	{ECHO;}
<s_f><<EOF>>	{*fbuf='\0';reverse(bufptr);printf("%s",bufptr);return 0;}
<s_f>.|[\n]	{ECHO;}
.|[\n]	{ECHO;}
%%
struct options{
char *name;
int code;
} options[] =
{ "+f",1,
"+n",2,
"+d",3,
"-s",4,
"-l",5,
"-W",6,
"-w",7,
"-_",8,
"-", 0
};
#ifndef INFO
#define INFO "Sorry, no information available"
#endif
char **fileList;
 unsigned nFiles;
 unsigned currentFile = 0;
int find_opt(char *);
int cmdlinearg=0;
int main(int argc, char **argv)
{
int c;
char *info = INFO;
char argstr[80] = "-";
if(--argc > 0){
if((*++argv)[0]=='-'){
argc--;
if((*argv)[1]=='-'){c=find_opt("-_");++argv;}
else{ strcat(argstr,++*argv);
while((argc>0)&&((*++argv)[0]=='-')&&((*argv)[1]!='-')){
argc--;
strcat(argstr,++*argv);}
c=find_opt(argstr);}
switch(c){
case 0: printf("%s\n",info);
return 0;
case 1: 
BEGIN s_f;
break;
case 2: 
BEGIN s_n;
break;
case 3: 
BEGIN s_d;
break;
case 4: 
BEGIN s_s;
break;
case 5: 
BEGIN s_l;
break;
case 6: 
BEGIN s_W;
break;
case 7: 
BEGIN s_w;
break;
case 8: 
BEGIN s__;
break;
default:
fprintf(stderr,"%s: illegal option\n",argstr);
return 1;
}
}else BEGIN s__;}
else BEGIN s__;
if(cmdlinearg==1){
if(argc>0){clvar=*argv;argc--;
++argv;}
else {fprintf(stderr,"Option argument expected\n");
exit(1);}}
if(argc>0){
argc--;
if(strcmp(*argv,"-")!=0){
if((yyin=fopen(*argv,"r"))==NULL){
fprintf(stderr,"Cannot open %s\n",*argv);
return 1;}}}
nFiles=argc;
fileList=argv+1;
yylex();
return 0;
}
int find_opt(char *word)
{
int count=0;
if(strcmp(word,"-help")==0)return 0;
while(strcmp(options[count].name,"-")!=0){
if(strcmp(options[count].name+1,word+1)==0){
if(options[count].name[0]=='+'){
cmdlinearg=1;
}
return options[count].code;}
count++;
}
return -1;
}
yywrap()
{
FILE *file;
if((nFiles>=1)&&(currentFile < nFiles))
fclose(yyin);
else{ fclose(yyin);
return 1;
}
while(fileList[currentFile]!=(char *)0){
if(strcmp(fileList[currentFile],"-")==0){
currentFile++;
file=stdin;
yyin=file;
break;
}
file=fopen(fileList[currentFile++],"r");
if(file != NULL ){
yyin=file;
break;
}
fprintf(stderr,"Cannot open %s \n",fileList[currentFile-1]);
}
return(file ? 0 : 1);
}

/* reverse: reverse a string s in place */
/* See K&R, p. 62 */
void
reverse(char s[])
{
	int c,i,j;

	for(i=0,j=strlen(s)-1;i<j;i++,j--){
		c = s[i];
		s[i]=s[j];
		s[j]=c;
	}

}
/* strdup : not built in on the NeXT */
char *strdup( char *s)
{
        char *p;

        p = (char *) malloc(strlen(s)+1);
        if(p != NULL)
                strcpy(p,s);
        return p;
}
