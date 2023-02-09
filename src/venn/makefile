venn: parse.y scan.l
	yacc -d parse.y
	flex scan.l
	gcc -o venn y.tab.c lex.yy.c -lfl -ly
