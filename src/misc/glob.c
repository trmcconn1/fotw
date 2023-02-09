/* glob.c: routines for shell filename generation */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1


int match(char *, char *);
char *is_first_equal(const char *, const char *);

char possibles[BUFSIZ];
char buffer[BUFSIZ];


/* Main for testing */
int main(int argc, char **argv)
{
	char linebuf[80];

	while(gets(linebuf) != NULL)
	{
		if(match(argv[1],linebuf))
			printf("It's a match!\n");
		else printf("It ain't no match!\n");
	}
	return 0;
}

/* match: returns true or false according as the input matches the pattern
   according to the usual rules of filenname pattern matching. */

int match( char *pattern, char *input)
{

	int invert_sense = FALSE;
	char *word;
	char p;
	int i,c;
	char *(*tester)(const char *, const char *); 
					/* Pointer to a function which
                                            returns a pointer to an 
                                            occurence in string of
                                            of a char in string pointed to  */


/* Handle a couple of trivial cases */
	if((pattern == NULL) || (strlen(pattern)==0)) return TRUE;
	if( input == NULL) return FALSE;


	word = input;
	tester = &is_first_equal;

	while(TRUE){
	switch( p = *pattern++)
	{
		/*  the pattern is all used up */
		case '\0':  
			    if(*word == '\0') return TRUE;
			    return FALSE;

		case '*':   
			    if(*pattern=='\0')return TRUE;
			    tester = &strpbrk;
			    break;

		case '?':   word++;
			    break;

		case '[':  

			/* Fill possibles array with all characters described
                             by the range */
			    i = 0;
			    if( *pattern == '^'){
				invert_sense = TRUE;
				pattern++;
			    }
			    while(*pattern != ']'){
				if(*pattern == '\0')return ERROR;
				if((strlen(pattern) >= 3) &&
					(*(pattern + 1) == '-')){
					if(*pattern <= *(pattern+2))
					 for(c=*pattern; c <= *(pattern+2); c++)
						possibles[i++] = c;
					else
					 for(c=*pattern; c >= *(pattern+2); c--)
						possibles[i++] = c;
					pattern += 3;
					continue;
					}
				possibles[i++] = *pattern++;		
			      }
			      pattern++; /* skip over the ] */
			      possibles[i] = '\0';
			      if(invert_sense){
				i = 0;
				for(c=0; c < 128; c++)
					if(strchr(possibles,c) == (char *)NULL)
						buffer[i++] = c;
				buffer[i] = '\0';
				invert_sense = 0;
				strcpy(possibles,buffer);
			       }

		           /* Now do the test  */    
			       if((word = tester(word, possibles))==NULL)
					return FALSE;
			   /*  have we emptied the word ? */
				if(*(++word) == '\0') {
					if((*pattern == '\0') ||
					  (strcmp(pattern,"*")==0) )return TRUE;
					return FALSE;
				}

			        tester = &is_first_equal;
				break;

		default:   /* Literal char to be matched */
			        possibles[0] = p;
				possibles[1] = '\0';
				if((word = tester(word,possibles))==NULL)
					return FALSE;
				if(*(++word) == '\0') {
					if((*pattern == '\0') ||
					  (strcmp(pattern,"*")==0) )return TRUE;
					return FALSE;
				}
			        tester = &is_first_equal;
		} /* End switch */
	} /* End while */	
	return TRUE;       /* Should not be reached */
}



/* Tests whether the first character in search_str matches any of
   the characters in target. Returns search_str if so, NULL if not. */

char *is_first_equal(const char *search_str, const char *target)
{
	char *ptr;

	ptr = target;
	while(*target != '\0')
		if(*search_str == *target++) return search_str;
	return NULL;
}
				
					
				

		
