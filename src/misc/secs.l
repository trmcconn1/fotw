%{
/*	secs.l: convert a time string to a number of seconds */

#include <stdlib.h>
#include <stdio.h>

int nofields=0;
float secs=0.0;


%}

%s TIMESTR

%%
<TIMESTR>[ \t\n]+  {printf("%.1f%s",secs,yytext);BEGIN
	 	0;secs=0.0;nofields =0;}
[ \t\n]+       /* ignore blanks when not in time string */ ;

<TIMESTR>[0-9]+[.]?[0-9]*  {secs = secs + atof(yytext);}

<TIMESTR>:	{secs = secs*60.0;nofields++;}

.*[^0-9.]+[^ \t]* {ECHO; BEGIN 0;nofields=0;}

[0-9]+[.]?[0-9]+ {BEGIN TIMESTR; secs = atof(yytext);}
%%

main()
{
	yylex();
}
