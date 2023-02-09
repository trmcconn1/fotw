%{
/* CAPS_to_Caps: filter to change a word which is all caps to a word with
		just the first letter capitalized
*/
%}

%%
[A-Z]+  {int i; putchar(yytext[0]);for(i=1;i<yyleng;i++)
		putchar(yytext[i] -'A' + 'a');}
.	{putchar(yytext[0]);}
\n	{printf("\n");}
%%
