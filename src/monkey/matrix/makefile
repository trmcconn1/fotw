#################################################################
#   Monkey Program Makefile (Linux Version)                     #
#        By Terry McConnell                                     #
#        January, 1997                                          #
#################################################################

VERSION = 10
FLAGS =  -I. -I.. 
TXT = 
LIBNAME = libmatrix.a
AR = ar
RANLIB = ranlib


.c.o:
	${CC} ${FLAGS} -c $<

objects: build.h
	for file in matrix*.c ; do \
	echo Compiling $$file ...;\
        ${CC} ${FLAGS} -c $$file;\
	done
	 
matrix: objects
	for obj in *.o; do \
	${AR} rv ${LIBNAME} $$obj;\
	done
	${RANLIB} ${LIBNAME}

clean:
	rm -f matrix*.c *.o


