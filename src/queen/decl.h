/* Convenient place for declarations. global.h must be included first */


extern integer my_gcd ( integer, integer, integer *, integer *);
extern integer inverse( integer, integer);
extern integer associate( integer, integer);
extern integer legendre_symbol(integer,integer);
extern integer normalize(integer, integer);
extern integer sqrtp(integer, integer);
extern struct factorization *factor_integer(integer,int);
extern integer chinese(integer *, integer *, int, integer *);
