%s FIRSTLINE LATERLINES

%%
%{
	BEGIN FIRSTLINE;
%}
<FIRSTLINE>[ \t\n]*[a-z] { yytext[yyleng-1] += 'A'-'a'; printf("%s",yytext); 
				BEGIN LATERLINES;}
<FIRSTLINE>[ \t\n]*[^a-z] { printf("%s",yytext); BEGIN
				LATERLINES;}
<LATERLINES>\.[ \t\n]+[a-z] {  yytext[yyleng-1] 
				+= 'A'-'a'; printf("%s",yytext); }
<LATERLINES>\n	{ECHO;}
<LATERLINES>.	{ECHO;}
%%
