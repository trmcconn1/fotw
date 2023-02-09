%{

/*	reverse.ff: reverse input */
/*      options: default: reverse by word = string of alphanumerics
		-P:  reverse by  Word = string of non whitespace
		-p:  reverse by Word, except leave period in place
		-l:  reverse by line
		-s:  reverse by sentence
*/
#include <stdlib.h>
#include <stdio.h>
#define BUFSIZE 256
#define INFO "                       CHICKEN HAVEN SOFTWARE  (T. McConnell)
                          fgn version 1.1 
	reverse: reverse input.
	options: default: reverse by word = string of alphanumerics.
		-P:  reverse by  Word = string of non whitespace
		-p:  reverse by Word, except leave period in place
		-l:  reverse by line
		-s:  reverse by sentence
		-n:  next argument (k) means reverse blocks of k chars."
static char buf[BUFSIZE];
int charcnt=0;
void reverse(char s[]);
#include <string.h>
#define NVARS 10
char *var[NVARS];
char *clvar;
%}
%s s__
%s _0
%s s_p
%s p0
%s s_P
%s P0
%s s_l
%s l0
%s s_s
%s s1
%s s0
%s s_n
%s n0
%%
<s__>[a-zA-Z0-9]+	{BEGIN _0; yyless(0);}
<_0>[a-zA-Z0-9]+	{var[0]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);printf("%s",buf);BEGIN s__;}
<_0>.|[\n]	{ECHO;}
<s_p>[^. \t\n]+	{BEGIN p0; yyless(0);}
<p0>[^. \t\n]+	{var[0]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);printf("%s",buf);BEGIN s_p;}
<p0>.|[\n]	{ECHO;}
<s_P>[^ \t\n]+	{BEGIN P0; yyless(0);}
<P0>[^ \t\n]+	{var[0]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);printf("%s",buf);BEGIN s_P;}
<P0>.|[\n]	{ECHO;}
<s_l>.+	{BEGIN l0; yyless(0);}
<l0>.+	{var[0]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);printf("%s",buf);BEGIN s_l;}
<l0>.|[\n]	{ECHO;}
<s_s>[^.!?]+[.?!]	{BEGIN s0; yyless(0);}
<s0>[^.!?]+	{var[0]=strdup(yytext);BEGIN s1;}
<s0>.|[\n]	{ECHO;}
<s1>[.?!]	{var[1]=strdup(yytext);strcpy(buf, var[0] );reverse(buf);
	printf("%s",buf);printf("%s",  var[1] );BEGIN s_s;}
<s1>.|[\n]	{ECHO;}
<s_n>.	{BEGIN n0; yyless(0);}
<n0>.	{var[0]=strdup(yytext); if(charcnt == atoi(clvar)-1){strcat(buf, var[0] );reverse(buf);
		printf("%s",buf);charcnt=0; buf[0]='\0';}
		 else {strcat(buf, var[0] );
		charcnt++;}BEGIN s_n;}
<n0>.|[\n]	{ECHO;}
<s_n><<EOF>>	{printf("%s",buf);return 0;BEGIN s_n;}
.|[\n]	{ECHO;}
%%
struct options{
char *name;
int code;
} options[] =
{ "+n",1,
"-s",2,
"-l",3,
"-P",4,
"-p",5,
"-_",6,
"-", 0
};
#ifndef INFO
#define INFO "Sorry, no information available"
#endif
int find_opt(char *);
int cmdlinearg=0;
int main(int argc, char **argv)
{
int c;
char *info = INFO;
char argstr[80] = "-";
if(--argc > 0){
if((*++argv)[0]=='-'){
if((*argv)[1]=='-')c=find_opt("-_");
else{ strcat(argstr,++*argv);
--argc;
while((*++argv)[0]=='-'&&(*argv)[1]!='-'){
--argc;
strcat(argstr,++*argv);}
c=find_opt(argstr);}
switch(c){
case 0: printf("%s\n",info);
return 0;
case 1: 
BEGIN s_n;
break;
case 2: 
BEGIN s_s;
break;
case 3: 
BEGIN s_l;
break;
case 4: 
BEGIN s_P;
break;
case 5: 
BEGIN s_p;
break;
case 6: 
BEGIN s__;
break;
default:
fprintf(stderr,"%s: illegal option\n",argstr);
return 1;
}
}}
else BEGIN s__;
if(cmdlinearg==1){
if(argc>0){clvar=*argv;
argc--;++argv;}
else {fprintf(stderr,"Option argument expected\n");
exit(1);}}
if(argc>0) {
if((yyin=fopen(*argv,"r"))==NULL){
printf("Cannot open %s\n",*argv);
return 1;}
}
yylex();
return 0;
}
int find_opt(char *word)
{
int count=0;
if(strcmp(word,"-help")==0)return 0;
while(strcmp(options[count].name,"-")!=0){
if(strcmp(options[count].name+1,word+1)==0)
if(options[count].name[0]=='+'){
cmdlinearg=1;
}
return options[count].code;
count++;
}
return -1;
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
