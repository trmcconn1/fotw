/* Exception.c: support for detecting floating point exceptions (C99 only) */

#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

static int fpe = 0; 

sighandler_t my_fpe_handler(int signal){
	fpe = signal;
	return 0;
}

#ifdef C99
#include <fenv.h>

/* On the Intel pentium and successors the following floating point
 * exceptions can occur:
 *
 * Invalid Operation: occurs, e.g, with 0 * infinity and other meaningless
 * arithmetic operations. 
 *
 * Division by zero
 *
 * Overflow: when the result's exponent exceeds that of the largest
 * representable number in the current format.
 *
 * Underflow: when the result's exponent becomes more negative than
 * that of the smallest nonzero representable number. Smaller numbers can
 * be represented in denormalized form with loss of precision.
 *
 * Inexact: Generated whenever the result of an operation cannot be 
 * represented exactly. Since this happens often normally, we ignore this
 * exception. */

/* Portable check for occurence of an exception since the last reset.
 * complain about serious exceptions. Reset fpu. */

void exception_check( void )
{
	int excepts_wanted = FE_ALL_EXCEPT;
	fexcept_t excepts;

	if(fpe){
		fprintf(stderr,"Floating Point Exception %d\n",fpe);
		fpe = 0;
	}
	if(fegetexceptflag(&excepts,excepts_wanted)){
		fprintf(stderr,"Failed to get floating point exception flag\n");
		return;
	}

	if(excepts & FE_OVERFLOW)
		fprintf(stderr,"warning: Floating point overflow exception.\n");
	if(excepts & FE_UNDERFLOW)
		fprintf(stderr,"warning: Floating point underflow exception.\n");
	/* Ignore this one 
	 *
	if(excepts & FE_INEXACT)
		fprintf(stderr,"warning: Floating point inexact exception.\n");
	*/
	if(excepts & FE_INVALID)
		fprintf(stderr,"warning: Floating point invalid operation exception.\n");

	if(excepts & FE_DIVBYZERO)
		fprintf(stderr,"warning: Floating point division by zero exception.\n");

	feclearexcept(excepts_wanted);
	return;
}

int initialize(void){

#ifdef __USE_GNU

	/* gcc seems to ignore exceptions by default. We'd like to
	 * hear about all of them. */
	feenableexcept(FE_ALL_EXCEPT);
#endif
	fesetround(FE_TONEAREST); /* Probably enabled by default, but it
				     it doesn't hurt to make sure. */
	/* install signal hander for floating point exception */

	if(signal(SIGFPE,(sighandler_t) my_fpe_handler)==SIG_ERR)
		fprintf(stderr,"Unable to install FPE handler\n");

        return fesetenv(FE_DFL_ENV); 	
}

#else
void exception_check(void)
{
	if(fpe){
		fprintf(stderr,"Floating Point Exception %d\n",fpe);
		fpe = 0;
	}
	return;
}
int initialize(void){

	if(signal(SIGFPE,my_fpe_handler)==SIG_ERR)
		fprintf(stderr,"Unable to install FPE handler\n");
	return;
}
#endif


	

