/* Confluent.c:
 
   Stand alone main program for computation of the confluent hypergeometric 
   functions of first and second type of real parameters and a real variable. 

   These functions are sometimes called the Kummer functions.

   Compile: cc -Llib -Iinclude -o confluent confluent.c user.c  -lm -lf

   See comments in library routine source lib/confluent.c for details on
   the computation.

   For usage information, give the command confluent -h or see USAGE defined
   below.

   library routine usage:

	extern double hyper(confluent a, double c, double x, 
		double t_error);

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

#define VERSION "1.2"
#define PROGRAMNAME "Confluent"

#define USAGE "confluent [-hvi2 -p <n> -a|c|x <n> -dx|a|c -n <n> -xml ]"   

#define HELP "\n"USAGE"\n\n\
-h:   print this helpful message.\n\
-v:   print version number and exit.\n\
-i:   interactive mode. Supply values at prompts.\n\
-2:   include function of 2nd kind (positive x range only) (Slow!)\n\
-p:   obtain values to accuracy of  < n (default .0000001.)\n\
-a:   specify starting value of parameter a to be n (default 1.0.)\n\
-c:   specify starting value of parameter c (default 1.0.)\n\
-x:   specify starting value of variable (default 0.0.)\n\
-dx:  specify increment in variable  to be n (default = 0.1.)\n\
-da:  specify increment in a (default = 0.0.)\n\
-dc:  specify increment in c (default = 0.0.)\n\
-n:   specify number of times to increment, i.e. rows - 1 (default = 0.)\n\
-xml: generate xml output for function_table stylesheet.\n\n\
Tabulate the confluent hypergeometric function(s) Phi(a,c;x) [Psi(a,c;x)].\n\n"

#define XML_TITLE "<title> Confluent Hypergeometric (Kummer) Function[s] Phi(a,c;x) [Psi]</title>\n"

extern real_number confluent(real_number a, real_number c, real_number x, real_number t_error);
extern real_number confluent2(real_number a, real_number c, real_number x, real_number t_error);

int
main(int argc, char **argv)
{
	int n = 1,i=1,j,k;
	int xml=0;
	int include_2 = 0;
	real_number a = 1.0, c=1.0, x = 0.0, dx = 0.1, t_error = T_ERROR;
	real_number da = 0.0, dc = 0.0,t;
	union my_data user_data;
	char buf[32]; /* for versioning */
	int v_c,v_d,f_c,f_sign,v_sign; 
	char *line_format;
	char *header;
	char *hline;
	real_number s1,s2,u;


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
		  if(strcmp(argv[i],"-2")==0){
			include_2 = 1;
			i += 1;
			continue;
		  }
		  if(strcmp(argv[i],"-a")==0){
			a = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-xml")==0){
			  xml = 1;
			  i += 1;
			  continue;
		  }
		  if(strcmp(argv[i],"-c")==0){
			c = atof(argv[i+1]);
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
		  if(strcmp(argv[i],"-da")==0){
			da = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-dc")==0){
			dc = atof(argv[i+1]);
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

			fprintf(stderr,"\nGenerate a table of values of the confluent hypergeometric function Phi(a,c;x).\n");
			fprintf(stderr,"Please enter requested data at the prompts.\n\n");
			while(getuser(&user_data,DOUBLE,"Parameter a=?") && handle_error());
			a = user_data.d;
			while(getuser(&user_data,DOUBLE,"Parameter c=?") && handle_error())
				;
			c = user_data.d;
			while(getuser(&user_data,DOUBLE,"Initial x value?") && handle_error())
				;
			x = user_data.d;
			while(getuser(&user_data,INTEGER,"Number, n, of table rows to create?") && handle_error())
				;
			n = user_data.i;
			while(getuser(&user_data,DOUBLE,"Increment, dx, in x between successive rows?") && handle_error())
				;
			dx = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, da, in a?") && handle_error())
				;
			da = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, dc, in c?") && handle_error())
				;
			dc = user_data.d;
			while(getuser(&user_data,STRING,"Include 2nd type function[yn]?")&&handle_error());
			if(strcmp(user_data.s,"y")==0)
				include_2 = 1;
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

	i=0,t=c;
	while(i++<n){
		if((t <= 0.0)&&((-t)-floor(-t)==0))
		fprintf(stderr,"Warning: c parameter can be 0 or negative integer.\n\n");
		t += dc;
	}
	if(include_2 && ((x < 0.0)||(x + n*dx < 0.0)))
		fprintf(stderr,"Warning: x range negative for type 2 function.\n\n");


	/* Determine field widths for variables and function values */

	v_d = decimals(3,x,dx,a,da,c,dc);

	/* Run a quick and dirty trial calculation to figure  
	 * field widths */

	t = x, s1=a,s2=c, v_c = 1, f_c = 1, f_sign = 0, v_sign = 0;

	for(i=0;i<n;i++){

		if(t < 0.0) v_sign = 1;
		j = (int)ceil(log10(fabs(t)));
		if(j>v_c)v_c = j;
		j = (int)ceil(log10(fabs(s1)));
		if(j>v_c)v_c = j;
		j = (int)ceil(log10(fabs(s2)));
		if(j>v_c)v_c = j;

		u = confluent(s1,s2,t,1.0);
		if(u<0.0) f_sign = 1;
		k = (int)ceil(log10(fabs(u)));
	        if(k > f_c)f_c = k;

		t += dx;
		s1 += da;
		s2 += dc;
	}

	f_c += f_sign;
	v_c += v_sign;

	if(include_2){
		line_format = build_line_format(3,v_c,pow(.1,(real_number)v_d),
			2,f_c,t_error); 
		header = build_header(3,v_c,pow(.1,(real_number)v_d),2,f_c,
				.0000000001,"a","c","x","Phi(a,c;x)","Psi(a,c;x)");
		hline = build_hline(3,v_c,pow(.1,(real_number)v_d),2,f_c,
				.0000000001);
	}
	else {
		line_format = build_line_format(3,v_c,pow(.1,(real_number)v_d),
			1,f_c,t_error); 
		header = build_header(3,v_c,pow(.1,(real_number)v_d),1,f_c,
				.0000000001,"a","c","x","Phi(a,c;x)");
		hline = build_hline(3,v_c,pow(.1,(real_number)v_d),1,f_c,
				.0000000001);

	}
	if(xml){
		printf(XML_HEADER);
		printf(XML_TITLE);
		printf("<header>\n\
	<cell width=\"5\">x</cell>\n\
	<cell width=\"5\">a</cell>\n\
	<cell width=\"5\">c</cell>\n\
	<cell width=\"11\">Phi(a,c;x)</cell>\n\
</header> \n");
	}
	else {
		if(include_2) printf("          The Confluent Hypergeometric (Kummer) Functions\n\n");
		else printf("          The Confluent Hypergeometric (Kummer) Function\n\n");
		printf(header);
		printf(hline);
		free(header);
		free(hline);

	}
	i = 1;
	do {
		if(xml){
			if(include_2)
			printf("<row>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"11\">%.7"RFMT"</cell>\n\
	<cell width=\"11\">%.7"RFMT"</cell>\n\
</row>\n",x,a,c,confluent(a,c,x,t_error),confluent2(a,c,x,t_error));
			else
			printf("<row>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"5\">%.3"RFMT"</cell>\n\
	<cell width=\"11\">%.7"RFMT"</cell>\n\
</row>\n",x,a,c,confluent(a,c,x,t_error));
		}
		else
			if(include_2)
		       printf(line_format,x,a,c,confluent(a,c,x,t_error),confluent2(a,c,x,t_error));
			else
		       printf(line_format,x,a,c,confluent(a,c,x,t_error));
		x += dx;
		a += da;
		c += dc;
	} while(i++ < n);
	
	if(xml)printf(XML_FOOTER);
	return 0;
}
