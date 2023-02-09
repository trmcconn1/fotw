/* Bessel.c:
 
   Stand alone main program for computation of the  Bessel Function of first
   and second type, of arbitrary real order, and Bessel Functions of imaginary
   argument, of first and second type. We support only real variable
   calculations.

   Compile: cc -Llib -Iinclude -o bessel bessel.c user.c  -lm -lf

   See comments in library routine source lib/bessel.c for details on
   the computation.

   For usage information, give the command bessel -h or see USAGE defined
   below.

   library routine usage:

	extern double besselJ(double nu, double x,  
		double t_error);

	 	resp. besselN, besselI, besselK.

   Bugs:
		- We only support real number computations.

		- The number of displayed digits in the answer doesn't depend 
                  on the precision.

   Author: Terry R. McConnell
	   trmcconn@syr.edu

	   In programs that compute functions we try to meet the following
	   design goals:

	   	- Have a standalone version of the function that can be linked
		  and called externally.

		- Have a wrapper main function that processes arguments and
		  allows the function to be computed from the command line.

		- Have my standard version and help options.

		- Have an interactive (-i) option that will allow the program
		  to be run from an interactive dialogue.
		
		- Have an option to generate xml output
		  that uses a style sheet named function_table.css. 

		- Fully document the source with comments, especially 
		  providing references or full details to key mathematical
		  ideas. 

		- The normal output should be one or more tables of values.

*/


#include "global.h"
#include <math.h> 

#define VERSION "1.0"
#define PROGRAMNAME "Bessel"

#define USAGE "bessel [-hvi -p <n> -nu|x <n> -N|K|I -dx|n -n <n> -xml ]"   

#define HELP "\n"USAGE"\n\n\
-h:   print this helpful message.\n\
-v:   print version number and exit.\n\
-i:   interactive mode. Supply values at prompts.\n\
-p:   obtain values to accuracy of  < n (default .0000001.)\n\
-nu:   specify starting value of order to be n (default 0.0.)\n\
-x:   specify starting value of variable (default 0.0.)\n\
-dx:  specify increment in variable  to be n (default = 0.1.)\n\
-dn:  specify increment in nu (default = 0.0.)\n\
-n:   specify number of times to increment, i.e. rows - 1 (default = 0.)\n\
-N:   Include a column for the Neumann function.\n\
-I:   Include a column for the function of imaginary argument of 1st kind.\n\
-K:   Include a column for the function of imaginary argument of 2nd kind.\n\
-xml: generate xml output for function_table stylesheet.\n\n\
Tabulate the Bessel function(s) J(nu,x) [N,I, or K].\n\n"

#define XML_TITLE "<title> Bessel functions[s] J(nu,x) [N,I, or K]</title>\n"

extern real_number besselJ(real_number nu, real_number x, real_number t_error);
extern real_number besselI(real_number nu, real_number x, real_number t_error);
extern real_number besselN(real_number nu, real_number x, real_number t_error);
extern real_number besselK(real_number nu, real_number x, real_number t_error);

int
main(int argc, char **argv)
{
	int n = 1,i=1,j,k;
	int v_c,v_d,f_c,f_sign,v_sign; 
	char *line_format;
	char *header;
	char *hline;
	real_number t,s,u;
	int xml=0;
	int include_K = 0;
	int include_N = 0;
	int include_I = 0;
	real_number nu = 0.0, x = 0.0, dx = 0.1, t_error = T_ERROR;
	real_number dn = 0.0;
	union my_data user_data;
	char buf[32]; /* for versioning */

	/* Do any floating point initialization */

	initialize();

	/* Process command line options */

	while((i < argc) && (argv[i][0] == '-')){
		  if(strcmp(argv[i],"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(argv[i],"-v")==0){
			get_libf_version(buf);
			printf("%s (libf.a version %s)\n",VERSION,buf);
			exit(0);
		  }
		  if(strcmp(argv[i],"-I")==0){
			include_I = 1;
			i += 1;
			continue;
		  }
		  if(strcmp(argv[i],"-K")==0){
			include_K = 1;
			i += 1;
			continue;
		  }
		  if(strcmp(argv[i],"-N")==0){
			include_N = 1;
			i += 1;
			continue;
		  }
		  if(strcmp(argv[i],"-xml")==0){
			  xml = 1;
			  i += 1;
			  continue;
		  }
		  if(strcmp(argv[i],"-nu")==0){
			nu = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-x")==0){
			 x = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-dx")==0){
			dx = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-dn")==0){
			dn = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-p")==0){
			t_error = atof(argv[i+1]);
			if(t_error <= DBL_EPSILON)
			        fprintf(stderr,"Warning: requested precision may exceed implementation limit.\n");
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-n")==0){
			n = atoi(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-i")==0){

                  /* Dialogue with user to set up parameters. We print
		   * to standard error to allow for redirection of output. */

			fprintf(stderr,"\n\n\t\t%s Version %s Interactive Mode\n\n",
					PROGRAMNAME,VERSION);

			fprintf(stderr,"\nGenerate a table of values of the Bessel functions.\n");
			fprintf(stderr,"Please enter requested data at the prompts.\n\n");
			while(getuser(&user_data,DOUBLE,"Order nu=?") && handle_error());
			nu = user_data.d;
			while(getuser(&user_data,DOUBLE,"Initial x value?") && handle_error())
				;
			x = user_data.d;
			while(getuser(&user_data,INTEGER,"Number, n, of table rows to create?") && handle_error())
				;
			n = user_data.i;
			while(getuser(&user_data,DOUBLE,"Increment, dx, in x between successive rows?") && handle_error())
				;
			dx = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, dn, in nu?") && handle_error())
				;
			dn = user_data.d;
			while(getuser(&user_data,STRING,"Include Neumann function[yn]?")&&handle_error());
			if(strcmp(user_data.s,"y")==0)
				include_N = 1;
			while(getuser(&user_data,STRING,"Include function I[yn]?")&&handle_error());
			if(strcmp(user_data.s,"y")==0)
				include_I = 1;
			while(getuser(&user_data,STRING,"Include function K[yn]?")&&handle_error());
			if(strcmp(user_data.s,"y")==0)
				include_K = 1;
			fprintf(stderr,"\n\n");
			i+=1;
		  	continue;
		  }
		  fprintf(stderr, "%s: Unknown or unimplemented option %s\n",PROGRAMNAME, argv[i]);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
		}

	/* do sanity checks here. Warn the user when there is a problem
	 * but blunder onward anyway. */

	if(((x<0.0)||(x+n*dx < 0.0))&&(floor(nu)!=nu))
		fprintf(stderr,"Warning: x range negative for non-integral order.\n");

	if(include_N && ((x <= 0.0)||(x + n*dx <= 0.0)))
		fprintf(stderr,"Warning: x range negative for Neumann function.\n\n");

	/* Determine field widths for variables and function values */

	v_d = decimals(2,x,dx,nu,dn);

	/* Run a quick and dirty trial calculation to figure  
	 * field widths */

	t = x, s = nu, v_c = 1, f_c = 1, f_sign = 0, v_sign = 0;

	for(i=0;i<n;i++){

		if(t < 0.0) v_sign = 1;
		if(fabs(t) > 1.0){
			j = (int)ceil(log10(fabs(t)));
			if(j>v_c)v_c = j;
		}
		if(fabs(s) > 1.0){
			j = (int)ceil(log10(fabs(s)));
			if(j>v_c)v_c = j;
		}

		u = besselJ(s,t,1.0);
		if(u<0.0) f_sign = 1;
		if(fabs(u) > 1.0){
			k = (int)ceil(log10(fabs(u)));
	        	if(k > f_c)f_c = k;
		}

		t += dx;
		s += dn;
	}

	f_c += f_sign;
	v_c += v_sign;

	line_format = build_line_format(2,v_c,pow(.1,(real_number)v_d),
			1,f_c,t_error); 
	header = build_header(2,v_c,pow(.1,(real_number)v_d),1,f_c,
				t_error,"nu","x","J_nu(x)");
	hline = build_hline(2,v_c,pow(.1,(real_number)v_d),1,f_c,
				t_error);

	if(xml){
		printf(XML_HEADER);
		printf(XML_TITLE);
		printf("<header>\n\
	<cell width=\"5\">x</cell>\n\
	<cell width=\"5\">a</cell>\n\
	<cell width=\"5\">c</cell>\n\
	<cell width=\"11\">J_nu,x)</cell>\n\
</header> \n");
	}
	else {
		printf("          The Bessel Function[s]\n\n");
		printf(header);
		printf(hline);
		free(header);
		free(hline);
	}
	i = 1;
	do {
		if(xml){
	printf("<row>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"11\">%.7"RFMT"</cell>\n\
</row>\n",nu,x,besselJ(nu,x,t_error));
		}
		else
			printf(line_format,nu,x,besselJ(nu,x,t_error));

		/* Check for exceptions, excessive roundoff error, and
		 * reset operation counters for next round */

		exception_check();
		roundoff_check(t_error);
		reset_roundoff();
		x += dx;
		nu += dn;
	} while(i++ < n);
	
	if(xml)printf(XML_FOOTER);
	return 0;
}
