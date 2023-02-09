/*  columnar.c:  Implementation of a simple columnar transposition cipher. 
 *  Input is arranged in rows, and output is by columns, resulting in a 
 *  scrambling of the input. For example, suppose the plain text is: 
 *
 *  fairis
 *  foulan
 *  dfouli
 *  sfair    
 *
 *  If we take the columns in order 34215 we get the ciphertext
 *  iuoarluiaoffffdssni. 
 *
 *  In this implementation the number of columns will be the length of the
 *  key. The order of use of the columns is given by the order of the letters
 *  in the keyword. For example, the relative (alphabetic) order of the 
 *  letters in the
 *  word "moles" is 34215, so this keyword would yield the encryption above. 
 *  When letters in the keyword are repeated, subsequent instances of a given
 *  letter are treated as if the letter had been repeated in the order. Thus
 *  ababa gives 14253 as the order of column use. 
 *
 *  As usual, our alphabet consists of all 95 printable characters. 
 *
 *  A simple columnar substitution is very easy to crack by itself; and easy
 *  to recognize, since letter frequencies are preserved. It becomes much
 *  stronger if used multiple times, especially in conjunction with one
 *  or more of the substitution methods.
 *
 *  The only complication in this implementation has to do with the fact
 *  that there will usually be an incomplete final block to be encyphered.
 *  This causes very unpleasant indexing complications. One way around this
 *  would be to complete the final block with some special marker character
 *  or perhaps with random characters. This has two drawbacks: first, it is
 *  impossible to do this without altering the original plaintext upon
 *  decryption (there will be a certain number of garbage characters at the
 *  end.) Even worse, the bad guy could get very useful clues by examining
 *  the end of the cyphertext. Accordingly, we have chosen not to use 
 *  this workaround.
*/

  
#include "global.h"

static int N; /* We read and encrypt in NxN chunks */

static int *column_order;

int my_encrypt(char *input, char *output, int n)
{
	int i,j,k,l,m; 
	int nrows,odd_chars,nuls;
	int c;
	static int firstcall = 1;

        if(firstcall){
		k = 0;
                N =  m = strlen(key);
		/* allocate space to store column order */
		column_order = (int *)malloc(m*sizeof(int));
		if(!column_order){
			fprintf(stderr,"my_encrypt: cannot allocate memory\n");
			return -1;
		}

		/* Set up column order */
                for(i=0;i<m;i++){ 

			c = key[i];
			/* run through key, counting how many are before or
			 * equal to the current letter */
			k=0;
			for(j=0;j<m;j++){
				if(key[j]<c)k++;
				if((key[j]==c)&&(j<=i))k++;
			}
			column_order[i] = k-1;
                }

                firstcall = 0;
#ifdef DEBUG
	/* Dump keyorder */
		fprintf(stderr,"Debug: dumping column order:\n");
		for(i=0;i<m;i++)
			fprintf(stderr,"%d",column_order[i]);	
		fprintf(stderr,"\n");
#endif  /* DEBUG */
		return 0;
        }

	/* If the number n of chars in the block to be encrypted are fewer 
	 * than NxN, figure out the number of complete rows (nrows) and
	 * the number of leftover chars in any final incomplete row.
	 */

	nrows = n/N;
	odd_chars = n%N;

	/* Loop done for each block of text to be encrypted/decrypted */

	m = 0; /* Count of letters encyphered, for return to caller */

	if(encrypt_on){
		for(j=0;j<N;j++){ /* loop over cols of input(=rows of output) */
			k = column_order[j]; 
			for(i=0;i<N;i++){ /* write kth row of output from
				         jth column of input */
				if((i<nrows)||((i==nrows)&&(j<odd_chars))){ 
					output[k*N+i] = input[i*N+j]; 
					m++;
				}
				else /* in case of incomplete block, mark empty
				       output position with a null */	
					output[k*N+i] = 0;
			}
		}
		if(n < N*N){ /* "compact" output by removing nulls when block
				 is incomplete. */
			k=0;
			for(i=0;i<N;i++)
				for(j=0;j<N;j++)
					if(output[i*N+j])output[k++]=output[i*N+j];
		}
	}
	else {    /* decrypting */
		if(n < N*N){ /* Incomplete block! */
		/* We do the inverse of the compacting operation above.
		 * Unfortunately, this is considerably more complicated 
		 * because the order of the columns has been changed. */
			for(i=0;i<N;i++){
			/* Find out which column, k, of the original data gave
			 * this row of cypher text */
				k=0;
				while(column_order[k]!=i)k++;
			/* how many nuls should we insert at the end of this
			 * row ? */
				if(odd_chars-1 < k) nuls = N - nrows;
				else nuls = N - nrows - 1;
				/* Mark positions of nuls in expanded array */
				for(j=0;j<N;j++)
					if(j >= N - nuls) output[i*N+j]=0;
					else output[i*N+j]=1;
			}

	        	/* Using output array as a scratchpad, copy input
			 * array in place of the 1's. Then copy the whole 
			 * thing back to input */
			k=0;
			for(i=0;i<N;i++)
				for(j=0;j<N;j++)
					if(output[i*N+j])output[i*N+j]=input[k++];
			for(i=0;i<N;i++)
				for(j=0;j<N;j++)
					input[i*N+j]=output[i*N+j];

		} /* Incomplete block case */

		for(i=0;i<N;i++){ /* loop over cols of output = rows of input */
	
			k = column_order[i];
			/* ith column of output is kth row of input */

			for(j=0;j<N;j++) 
				if(input[k*N+j]){
					output[j*N+i] = input[k*N+j]; 
					m++;
				}
		}
	}
	return m;
}
	
/* Slurp input in nxn blocks, where n is the key length */

int set_block_size(int *ip)
{
	int n;

	n = strlen(key);
	*ip = n*n;
	return 0;
}
	
