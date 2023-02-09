/* Poor man's dos2unix: convert text files in place, removing all ^M characters    By text file, we mean ones having only printable characters in addition
   to ^M and ^J */

#include<stdio.h>
#include<stdlib.h>



#define USAGE "usage: dos2unix filename [ ... ]\n"

int main(int argc, char **argv)
{
	int c,i, n=0;
	FILE *input,*output;
	char *buffer,*b;

	argc--; argv++;
	if(argc==0){
		 fprintf(stderr,"%s\n",USAGE);
		 return 1;
	}

/* Loop over files on the command line */

	for(i=0;i<argc;i++){ 
		input = fopen(argv[i],"r");
		if(!input){
			fprintf(stderr,"cannot open %s, skipping\n",
				argv[i]);
			continue;
		}
	/* Make a first pass to see how many chars there are */
			
		while((c=fgetc(input))!=EOF) n++;

		/* Make a buffer to hold converted file */

		buffer = (char *)malloc((n+1)*sizeof(char));
		if(!buffer){
			fprintf(stderr,"Cannot alloc %d bytes\n",n+1);
			fclose(input);
			return 1;
		}

		/* Make a second pass while we convert and write to 
		  the buffer */

		rewind(input);
	       	b = buffer; 
		n = 0;
		while((c=fgetc(input))!=EOF){
			if(c=='\r') continue; 
		        else { *b++ = c; n++; } 
		}
		fclose(input);

		/* Reopen file for write and prepare to copy buffer */
		output = fopen(argv[i],"w");
		b = buffer;
		if(!output){ /* well, skip it, but now there is cleanup */

			fprintf(stderr,"Unable to open %s for write\n",argv[i]);			
			free(buffer);
			continue;
		}

		/* Copy buffer to file */
		for(i=0;i<n;i++)fputc(*b++,output);
		fclose(output);
		free(buffer);
		
	} /* end loop over command line files */
	return 0;
}

