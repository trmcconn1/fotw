/* freq.c: Read input and report statistics on it. 

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define ALPHABET_SIZE 256

#define VERSION "1.0"
#define USAGE "freq [-ahvinb -c <nn>] "
#ifndef _SHORT_STRINGS
#define HELP "\nfreq [-ahvinb -c <nn>]\n\n\
Read stdin and report statistical information.\n\n\
-a: Display information for alphabetic characters only.\n\
-b: Display information for all binary characters (printable default).\n\
-v: Print version number and exit. \n\
-i: Ignore case of alphabet letters. \n\
-n: Don't sort by frequency. Display in ascii order.\n\
-c: Don't list characters with frequency < nn%.\n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

struct char_info {

	int me;
	int count;

} counts[ALPHABET_SIZE];

/* Integer comparison for qsort */

int compare_by_count(const struct char_info *p, const struct char_info *q)
{
	if(!p){
		if(!q)return 0;
		else return 1;
	}
	if(!q){
		if(!p)return 0;
		else return -1;
	}
	if(p->count == q->count)return 0;
	if(p->count < q->count)return 1;
	return -1;
}

/* Display characters. Return number of characters in display. */

int display(int c)
{
	if(isprint(c)){
		printf("%c",c);
		return 1;
	}
	switch(c){

		case '\n':
			printf("\\n");
			return 2;
		case '\t':
			printf("\\t");
			return 2;
		case ' ':
			printf(" ");
			return 1;
		default:
			printf("\\x%X",c);
			if(c<=0xF)return 3;
			return 4;
	}
	return;
}

int main(int argc, char **argv)
{

	int i,j=0,n=0,k;
	int lo_i=0,hi_i=255;
	int sort_by_freq = 1;
	int ignore_case = 0;
	int printable_only = 1;
	int alphabetic_only = 0;
	double f;
	double cutoff = 0.0;
	int c;

	for(i=0;i<ALPHABET_SIZE;i++){

		(counts[i]).me = i;
		(counts[i]).count = 0;	

	}

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case 'i':
					ignore_case = 1;
					break;
				case 'b':
					printable_only =0;
					break;
				case 'n':
					sort_by_freq = 0;
					break;
				case 'a':
					alphabetic_only = 1;
					break;
				case 'c':
					j++;
					cutoff = atof(argv[j])/100.0;
					break;
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"freq: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}

	while((c = fgetc(stdin))!=EOF){

		if(printable_only && ( (c < 0x20 ) || (c > 0x7E)))continue;
		if(alphabetic_only && !isalpha(c))continue;
		if(ignore_case) if(isupper(c))c = tolower(c);
		(counts[c].count)++;
		n++; /* keeps track of total counted */

	}

	if(sort_by_freq) qsort(counts,ALPHABET_SIZE,sizeof(struct char_info ),
			(int(*)(const void*,const void*))compare_by_count);

	j=1;
	for(i=lo_i;i<hi_i;i++){
		f = ((double)(counts[i].count))/((double)n);
		if(f < cutoff)continue; /* skip very low frequency chars */
		if(printable_only)if(!isprint(counts[i].me))continue;
		if(alphabetic_only)if(!isalpha(counts[i].me))continue;
		if(ignore_case)if(isupper(counts[i].me))continue;
		printf("[");
		k = display(counts[i].me);
		/* Take care of alignment issues */
		if(!alphabetic_only)for(;k<4;k++)printf(" ");
		printf("] ");
		printf("%3.1f%%\t",100.0*f);
		if(j%5 == 0)printf("\n");
		j++;
	}
	printf("\n");
	return 0;
}
