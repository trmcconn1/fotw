NAME = num_to_name
CC = gcc
YACC = bison
LEX = flex


${NAME}: ${NAME}.parse.c ${NAME}.lex.c
	cp ${NAME}.parse.h y.tab.h
	${CC} -o ${NAME} ${NAME}.parse.c ${NAME}.lex.c -lfl 

${NAME}.parse.c:  ${NAME}.y
	${YACC} -d ${NAME}.y
	mv ${NAME}.tab.c ${NAME}.parse.c
	cp ${NAME}.tab.h ${NAME}.parse.h

${NAME}.lex.c:  ${NAME}.l ${NAME}.tab.h
	$(LEX) ${NAME}.l
	mv lex.yy.c ${NAME}.lex.c
