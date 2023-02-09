#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<float.h>

/* The confidence level for roundoff error */
#ifndef ALPHA
#define ALPHA 0.05
#endif

/* Add other compilers for which C99 support is available */
#ifdef __GNU_C  /* gcc 2.7 supports C99 in some sense. */
#define __USE_GNU
#define __USE_ISO_C99 1
#define C99
#include<tgmath.h>

#define DIGITS_PRECISION LDBL_DIG
typedef long double real_number;

/* This will be called with truncation error and a large value xxx. If the
 * large value requires too many decimal points to represent, then almost
 * certainly we don't have enough digits of floating point precision to
 * represent numbers of that size to within the desired truncation error.
 * This macro is used in generating precision loss warnings. 
 */
#define INEXACT(xxx,err)  ((int)log10((xxx)) + (int)log10(1.0/(err)) > LDBL_DIG ? 1 : 0)
#define RFMT "Lf"
#else
#define DIGITS_PRECISION DBL_DIG
typedef double real_number;
#define INEXACT(xxx,err) ((int)log10((xxx)) + (int)log10(1.0/(err)) > DBL_DIG ? 1 : 0)
#define RFMT "f"
#endif


#define MAXLINE 256
#define T_ERROR .0000001
#define XML_HEADER "<?xml version=\"1.0\" standalone=\"yes\"?>\n\
<!DOCTYPE function_table \n\
[\n\
]>\n\
<?xml-stylesheet type=\"text/css\" href=\"function_table.css\"?>\n\
<function_table>\n\
<title> Hypergeometric Function F(a,b,c;x) </title>\n"
#define XML_FOOTER "</function_table>\n"


/* Types for my_data fields for communicating with user.c routines */

#define INTEGER 0
#define DOUBLE 1
#define STRING 2

#define PI 3.14159265358979323846264338328L
union my_data {
	int i;
	real_number d;
	char *s;
};

extern int getuser(union my_data *data, int type, char *message);
extern char *build_line_format(int,int,real_number,int,int,real_number);
extern char *build_header(int,int,real_number,int,int,real_number,...);
extern char *build_hline(int,int,real_number,int,int,real_number);
extern int decimals(int n, ...);
extern int handle_error(void);
extern void get_libf_version(char *);
extern void exception_check(void);
extern int initialize(void);
extern unsigned long mults;
extern unsigned long divs;
extern unsigned long adds;
extern unsigned long subs;
extern void reset_roundoff(void);
extern void report_roundoff(void);
extern int units_error_roundoff(void);
extern void roundoff_check(real_number);
