stupid: stupid.o caller1.o caller2.o foo1.a foo2.a 
	gcc -L. -o temp1.o -nostartfiles -nostdlib -Xlinker -r stupid.o caller1.o -lfoo1  
	gcc -L. -o temp2.o -nostartfiles -nostdlib -Xlinker -r  caller2.o -lfoo2  
	gcc  -o stupid -Xlinker -m temp1.o temp2.o 
	rm temp1.o temp2.o

foo1.a: foo1.o
	ar rv libfoo1.a foo1.o
	ranlib libfoo1.a

foo2.a: foo2.o
	ar rv libfoo2.a foo2.o
	ranlib libfoo2.a
