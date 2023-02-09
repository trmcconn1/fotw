fgn: fgn.y fgn.l
	bison -y -d fgn.y
	flex -Cfr fgn.l
	gcc  -D__NEXT__ -o fgn -O y.tab.c lex.yy.c fgnlib.o -lfl

lex.yy.c: ff.out.l
	flex -Cfa ff.out.l

filter: lex.yy.c
	gcc -o filter  lex.yy.c -lfl
