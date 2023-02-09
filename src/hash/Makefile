all: primitive 3half baker rdata

primitive: primitive.c totient.c
	cc -o primitive primitive.c totient.c

3half: 3half.c 
	cc -o  3half 3half.c 

baker: baker.c
	cc -o baker baker.c

rdata: rdata.c mtwist.h mtwist.o
	cc -o rdata rdata.c mtwist.o
