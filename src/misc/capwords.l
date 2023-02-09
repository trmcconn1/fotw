%s FIRSTWORD LATERWORDS
%%
%{
	BEGIN FIRSTWORD;
%}

<FIRSTWORD>[a-z] {yytext[0] += 'A'-'a'; printf("%s",yytext);
			BEGIN LATERWORDS;}
<FIRSTWORD>[^ \t\na-z] {ECHO; BEGIN LATERWORDS;}
<LATERWORDS>[ \t\n]+[a-z] { yytext[yyleng-1] += 'A'-'a'; printf("%s",yytext);}
<LATERWORDS>-[\n][a-z] {ECHO;  /* hyphenated */}

%%

