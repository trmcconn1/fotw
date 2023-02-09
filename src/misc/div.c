
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

/* Convert 5 byte array of '0's and '1's to an int. Array can be
	either <= 4 bytes + null terminator, or 5 bytes without null
	terminator.  */
int
bin2d( char *binary)
{
	int num = 0;
	int i = 0,j;
	
	/* Handle the case binary is a null terminated array */
	for(j=0;j<5;j++)
		if(binary[j] == '\0'){
		    while(i<j) num = 2*num + (*(binary + i++)-'0');
		    return num;
		}
	while(i<5) num = 2*num + (*(binary + i++)-'0');
	return num;
}


/* Divide a long array of zeros and ones by 10 = "1010". Return remainder as
   an int and quotient in array pointed to. Dividend arg is destroyed. This
   implements binary "long division".  NB: quotient must point to an area
   at least as large as dividend. */

int
divby10(char *dividend, char *quotient)
{
	int i,j;
	char *l;    /* pointer to leading one in dividend */

	/* Fill the quotient array with digit zeros */
	for(i=0;i<strlen(dividend);i++) quotient[i] = '0';
	quotient[i] = '\0';

	/* Find first '1' in dividend */
		l = strchr(dividend,'1');
		if(l==NULL) return 0;

	while((strlen(l)>5) || ((j=bin2d(l)) > 9)){
				    /* only done when first fails */


	/* See if l(l+1)(l+2)(l+3) > 9, equivalently,
		l(l+1)...(l+4) > 19 */

		if((strlen(l) <= 4)||(bin2d(l) > 19)){ 
			quotient[ l - dividend + 3 ] = '1';

	/* We must do the subtraction:

		l(l+1)(l+2)(l+3)
	      -	1  0    1    0
	      ___________________
		0  ?    ?   (l+3) 
	*/

			*l = '0';
			if( *(l+2) == '0' ) { /* Then we borrow from l+1, which
						 _must_ be 1 */
				*(l+1) = '0';
				*(l+2) = '1';	
			}
			else *(l+2) = '0';  /* and leave l+1 alone */
		}
		else {
			quotient[ l - dividend + 4 ] = '1';

	/* We must do the subtraction:

		l(l+1)(l+2)(l+3)(l+4)
	       -   1    0    1    0                UGLY!!!!
		--------------------
		0  ?    ?    ?   (l+4) 
	*/


			*l = '0';
			if(*(l+3)=='0'){ /* Must try to borrow from l+2 */
				*(l+3) = '1';
				if(*(l+2) == '1'){
					*(l+2) = '0';
					*(l+1) = '1'; /* Must have to borrow
                                                        from l, else we would
							not be in this case */
				}
				else { /* Must try to borrow from l+1 */
					*(l+2) = '1';
					if(*(l+1)=='1') *(l+1) = '1';
						else *(l+1) = '0';
				} 
			}
			else { /* l+3 is a 1 */
				*(l+3) = '0';
				/* l+2 is unchanged and l+1 _must_ be 0 */
				*(l+1) = '1';
			}
		}

	/* Find next '1' in dividend */
		l = strchr(dividend,'1');
		if(l==NULL) return 0;
	} /* end while */

	return j;   /* = bin2d(dividend) */
}

/* Print an arbitrarily long string of '0's and '1's, interpreted as
   a binary number, in base 10 format */

int
PrintAsDecimal(char *long_num)
{
	char *quotient;
	char *dec_buf;
	char *l;
	int j, i =0;

	l = strchr(long_num,'1');
	if(l==NULL){
		printf("0");
		return 0;
	}

	quotient = (char *) malloc(strlen(long_num)+1);

	/* Buffer to hold decimal digits as they're generated. We don't need
           it this long, of course */

	dec_buf = (char *) malloc(strlen(long_num)+1);

	if((quotient == NULL) || (dec_buf == NULL)){
		fprintf(stderr,"PrintAsDecimal: Unable to malloc space \n");
		return 1;
	}
	while((strlen(l) > 5)||((j=bin2d(l)) >9)){
		dec_buf[i++] = (char)divby10(long_num,quotient);
		strcpy(long_num,quotient);
		l = strchr(long_num,'1');
	}
	dec_buf[i++]=(char)j;

/* Print dec_buf in reverse order */
	while(i--)printf("%d",dec_buf[i]);

	free(quotient);
	free(dec_buf);
	return 0;
}

int
main(int argc, char **argv)
{

	printf("Number is  ");
	PrintAsDecimal(argv[1]);
	printf("\n");
	return 0;
}

