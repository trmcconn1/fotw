#
####################################################
#  truename makefile
####################################################

INSTALL_DIR = /usr/local/bin
MAN_DIR = /usr/man/man1
CFLAGS = -O2  -D__HAVE_STRDUP__

.SUFFIXES : .c .o
.c.o: 
	${CC} $(CFLAGS) -c $<

truename: truename.o
	${CC} -o $@ $?
	strip $@

install:
	cp truename $(INSTALL_DIR)
	cp truename.1 $(MAN_DIR)

clean:
	rm *.o
	
	
