%{
/* Tutney.l: translates stdin into tut-ney. */
/*	Tut-ney is a spelling language my parents used to 
	prevent me from following delicate discussions when I was young.

	Build: lex tutey.l
	       cc -o tutney lex.yy.c -ll
*/
%}

%%
[aeiouAEIOU]	{printf("%s-",yytext); /* Vowels unchanged */ }
[hH]	{printf("hash-"); /* Exceptional consonants */ }
[jJ]	{printf("judge-");}
[qQ]	{printf("quack-");}
[wW]	{printf("wac-");}
[yY]	{printf("yack-");}
[a-zA-Z] {printf("%su%s-",yytext,yytext);/* Normal consonant: repeat and
						interpose u */ }
.	{ ECHO;/* Pass everything else through */ }
%%

main()
{
	yylex();
}
