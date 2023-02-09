/* format.c: Ugly details involving output formatting. Build format strings 
 * and column titles for printing at various levels of
 * precision with various numbers of variables and function values in each
 * table row. */


#include "global.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include<float.h>
#include<math.h>

#define PAD "  "   /* padding between fields */
#define HLINE_CHAR '-'
#define MAX_COL_TITLE 32  /* Longest column header name */

/* Build a format string for printing a line containing  arguments and values
 * of functions. The number of arguments is passed as nargs and the number of
 * functions is fd. arg_c and f_c tell how many places to reserve to the 
 * left of the decimal (c is for "characteristic".)  If these are negative
 * we reserve an extra place for the minus sign. The number of places to
 * print the right of the decimal is determined by argd and fd for arguments
 * and functions respectively. The true value of a function lies in a range
 * of +/- fd. Thus we need about log(1/fd) digits, where log is log to the
 * base 10. */

char *build_line_format(int nargs, int arg_c, real_number arg_d, int nfs, int f_c, real_number f_d)
{

	int f_decimals, arg_decimals;
	int f_sign = 0;
	int v_sign = 0;
	int i;
	char *s;
	char buf[30];


	f_decimals = (int)floor(0.1+log10(1.0/f_d));
	if(f_c < 0) {
		f_sign = 1;
		f_c = -f_c;
	}
	if(arg_c < 0) {
		v_sign = 1;
		arg_c = -arg_c;
	}
	/* We have to allow room for one digit to the
	 * left of decimal, even if it is zero */
	if(f_c == 0)f_c = 1;
	if(f_decimals <= 0)f_decimals=0;
	arg_decimals = (int)floor(0.1+log10(1.0/arg_d));

	/* How long a format string is required? It will have the form
	 * %a.bf...a.bf\n, where there is one a.bf for each arg and function.
	 * Let's be generous and figure 20 for each of these */

	s = (char *)malloc(20*(nargs+nfs)*sizeof(char));
	if(!s){
		fprintf(stderr,"Cannot allocate format string.\n");
		exit(1);
	}

	/* For args we'll figure a = PAD + arg_c + 1 + arg_decimals and
	 * b = arg_decimals */

	s[0] = '\0';
	/* No padding for first field */
	sprintf(s,"%%%d.%d"RFMT,1 + +arg_c + v_sign + arg_decimals,arg_decimals);
	sprintf(buf,"%%%d.%d"RFMT,strlen(PAD)+v_sign+arg_c+1+arg_decimals,arg_decimals);
	for(i=1;i<nargs;i++)
		strcat(s,buf);

	/* Similarly for function values */	

	sprintf(buf,"%%%d.%d"RFMT,strlen(PAD)+1+f_sign + f_decimals,f_decimals-f_c);
	for(i=0;i<nfs;i++)
		strcat(s,buf);

	strcat(s,"\n");

	return s;
}

/* Build a string for the table column header. 
   After the last listed argument there should follow nargs + nfs
 * strings which give the titles to print. The titles are placed so that
 * the first letter is over the decimal point. */ 

char *build_header(int nargs, int arg_c, real_number arg_d, int nfs, int f_c, real_number f_d,...)
{

	int f_decimals, arg_decimals;
	int i,j,k,L,lp;
	int f_sign=0,v_sign=0;
	char *s;
	char *t;
	va_list titles;


	va_start(titles,f_d);

	if(f_c < 0) {
		f_sign = 1;
		f_c = -f_c;
	}
	if(arg_c < 0) {
		v_sign = 1;
		arg_c = -arg_c;
	}
	f_decimals = (int)floor(0.1+log10(1.0/f_d));   /* decimal places in func */
	if(f_c == 0)f_c = 1;
	if(f_decimals < 0)f_decimals = 0;
	arg_decimals = (int)floor(0.1+log10(1.0/arg_d)); /* decimal places in arg */
	lp = strlen(PAD);

	/* Figure out the length of the heading: f_c and arg_c are the
	 * numbers of digits to the left of the decimal. The + 3 accounts
	 * for decimal point and a possible sign, plus one for 
	 * good measure  :-) */

	L = (f_decimals + f_c + strlen(PAD) + 3)*nfs +
		(arg_decimals + arg_c + strlen(PAD) + 3)*nargs;

	s = (char *)malloc(L*sizeof(char));
	if(!s){
		fprintf(stderr,"Cannot allocate header string.\n");
		exit(1);
	}

	/* Write the first heading. It's a bit different than the others
	 * since there is no padding */

	t = va_arg(titles,char *);
	for(i=0;i<arg_c+v_sign;i++)s[i]=' ';
	while((i-arg_c-v_sign < 1+arg_decimals)&&(i-arg_c-v_sign<strlen(t))){
		s[i] = t[i-arg_c-v_sign];
		i++;
	}
	while(i-arg_c-v_sign < 1 + arg_decimals) /* additional spaces after title */
		s[i++]= ' ';

	/* Now repeat for the remaining  nfs+nargs-1 headings */
	k = i; /* k runs across s. i resets with each new heading */
	for(j=0;j<nargs-1;j++){
		t = va_arg(titles,char *);
		for(i=0;i<arg_c+v_sign+lp;i++,k++)s[k]=' ';
		while((i-arg_c-v_sign-lp < 1 + arg_decimals)&&(i-arg_c-v_sign-lp<strlen(t))){
			s[k] = t[i-arg_c-v_sign-lp];
			i++,k++;
		}
		while(i-arg_c-v_sign-lp < 1 + arg_decimals){
			s[k++] = ' ';
			i++;
		}
	}
	for(j=0;j<nfs;j++){
		t = va_arg(titles,char *);
		for(i=0;i<f_c+lp+f_sign;i++,k++)s[k]=' ';
		while((i-f_sign-lp < 1 + f_decimals)&&(i-f_c-f_sign-lp<strlen(t))){
			s[k] = t[i-f_c-f_sign-lp];
			i++,k++;
		}
		while(i-f_sign -lp < 1 + f_decimals){
			s[k++] = ' ';
			i++;
		}

	}
	s[k]='\0';
	strcat(s,"\n");

	va_end(titles);
	return s;
}

/* Build a string for horizontal line separating the header from
 * the body of the table. */

char *build_hline(int nargs, int arg_c, real_number arg_d, int nfs, int f_c, real_number f_d)
{

	int f_decimals, arg_decimals;
	int f_sign=0,v_sign=0;
	int i,n,L;
	char *s;


	if(f_c < 0) {
		f_sign = 1;
		f_c = -f_c;
	}
	if(arg_c < 0) {
		v_sign = 1;
		arg_c = -arg_c;
	}
	f_decimals = (int)floor(0.1+log10(1.0/f_d));
	if(f_c == 0)f_c = 1;
	if(f_decimals < 0)f_decimals = 0;
	arg_decimals = (int)floor(0.1+log10(1.0/arg_d));

	/* See build_header above */
	L = (f_decimals + f_c+f_sign + strlen(PAD) + 3)*nfs +
		(arg_decimals + arg_c +v_sign + strlen(PAD) + 3)*nargs;

	s = (char *)malloc(L*sizeof(char));
	if(!s){
		fprintf(stderr,"Cannot allocate hline string.\n");
		exit(1);
	}

	n = 0;
	n += arg_c+v_sign+1+arg_decimals;
	for(i=1;i<nargs;i++)
		n += strlen(PAD) + arg_c + v_sign + 1 + arg_decimals;
	for(i=0;i<nfs;i++)
		n += strlen(PAD) +  f_sign + 1 + f_decimals;

	for(i=0;i<n;i++)
		s[i]=HLINE_CHAR;
	s[i]='\0';

	strcat(s,"\n");

	return s;
}

/* Significant: attempt to find the number of "significant" digits in x to
 * the right of the decimal place. For example x = -21.02350 should return
 * 4. This is trickier than one might expect since the actual precision of
 * a real_number is platform dependent. Moreover, the "obvious" approach, i.e.,
 * multiply and round down until the result is 0.0, fails because garbage
 * digits can creep in "from the right" */

int significant(real_number x)
{
	real_number y;
	int n,i;
	int ct=0;
	char shift_char = '0';
#ifdef C99
	char buffer[LDBL_DIG+3];
#else
	char buffer[DBL_DIG+3];
#endif
	char format[256]; /* overkill */

	/* Normalize x to lie in the unit interval */

	y = fabs(x);
	i = (int)floor(y);
	if(i > 0) 
		ct = 3 + (int)log10(floor(y)); /* characteristic digits */
	y = y - floor(y);

	/* Round to nearest DBL_DIG - ct In theory, this ought to
	 * give us back the number of digits the user thought he typed. */
#ifdef C99
	for(i=0;i<LDBL_DIG-ct;i++)
		y *= 10.0;
	if( y - floor(y) >= .5 )
		y += 1.0;
	for(i=0;i<LDBL_DIG-ct;i++)
		y /= 10.0;
#else
	for(i=0;i<DBL_DIG-ct;i++)
		y *= 10.0;
	if( y - floor(y) >= .5 )
		y += 1.0;
	for(i=0;i<DBL_DIG-ct;i++)
		y /= 10.0;
#endif

	/* Build format string for sprintf'ing y with the field width set
	 * to allow for the maximum number of significant digits. We use
	 * DBL_DIG-1 because the last digit seems to have garbage in it
	 * sometimes. Why? */

#ifdef C99
	sprintf(format,"%%%d.%d"RFMT,LDBL_DIG-ct+2,LDBL_DIG-ct);
	                        /*       ^^^ allow for leading 0 and decimal */
#else
	sprintf(format,"%%%d.%d"RFMT,DBL_DIG-ct+2,DBL_DIG-ct);

#endif
	/* Print y into the buffer */

	sprintf(buffer,format,y); /* buffer contains 0.ddddddd ...d */

#ifdef DEBUG
printf(" significant: y = %s\n",buffer);
#endif

	n = strlen(buffer);

	/* See what the last digit d is. If it's a 9, we'll assume that
	 * y ends in ...9999 */

	if(buffer[n-1] == '9') shift_char = '9';

	for(i=n-2;i>1;i--)
		if(buffer[i] != shift_char)break;

	return i-1;
}


/* Figure the appropriate number of digits to the right of the decimal
 * point needed to display all variables, taking into account variable
 * increments. 
 *
 * Variable n tells how many variable pairs (real_number x, real_number inc) are passed. 
 *
 * The values passed as x are assumed to be starting values for variables
 * in a table, and inc is the desired increment in that variable between rows.
 * Presumably, if the user bothers to specify x to k decimals, say, then she
 * wants to see all of them. OTOH, even if x is a round number, the number
 * of decimals should be sufficient to display the differences between
 * consecutive rows. Thus, inc must also be considered.
 *
 * The heavy lifting is done by the significant routine above.
 */

int
decimals(int n, ...)
{

	real_number x, inc;
	int m=0,k=0,i;
	va_list data;

	va_start(data,n);

	for(i=0;i<n;i++){

		/* Each pass through the loop considers an (x,inc) pair
		 * and sets k to the number of decimals needed to display
		 * this pair. The variable m keeps track of the largest k
		 * seen so far. At the end, this is the value returned. */

		/* Get the next variable and its increment */

		x = va_arg(data,real_number);
		inc = va_arg(data,real_number);

		/* m keeps track of most number of digits seen so far */
		k = significant(inc);

		if(k > m) m = k;
		k = significant(x);
		if(k > m) m = k;

	}
	va_end(data);
	return m;
}
/* main program for testing */
#if 0

int
main(int argc, char **argv){

	real_number x;

	x = 1.10124300021l;

	printf("Number of digits to right of decimal in %Lf = %d\n",
			x,significant(x));
	return 0;
}
#endif
