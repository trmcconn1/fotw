/* libarbp.c: Supporting arbitrary precision routines for by-hand arithmetic
*             problems
*
* By Terry R. McConnell 12/2003
*
*/

#include "global.h"

/* Subtracts digits and stores result in res. Returns indicator of whether
   or not borrow is needed.
*/

int
sub( char x, char y, char *res)
{
	if( x >= y){
		 *res = '0' + x - y;
		 return FALSE;
	}
	*res = '0' + x - y + 10;
	return TRUE;
}

/* Isgte: returns TRUE if first digit string is >=  the second */

int
isgte( char *A, char *B)
{
	int a,b,i;
	char *AA, *BB;

	if(B==NULL) return TRUE;
	if(A==NULL) return FALSE;

/* Normalize by stripping off leading zeros */

	AA = strpbrk(A,NONZERO_DIGITS);
	BB = strpbrk(B,NONZERO_DIGITS);

	if(BB == NULL) return TRUE;
	if(AA == NULL) return FALSE;  

	if((b=strlen(BB))>(a=strlen(AA)))return FALSE;
	if(a > b) return TRUE;
	
	for(i=0;i<b;i++){
		if(AA[i]>BB[i]) return TRUE;
		if(AA[i]<BB[i]) return FALSE;
	}
	return TRUE;
}	
		 
	

/* Given character strings of digits with mend >= shend, this returns a pointer
to a string corresponding to minuend - subtrahend. Returns NULL if
minuend < subtrahend. Result array is padded with leading zeros.  

The terminology comes from assembly language mnemonics

*/

char *sbc(char *mend, char *shend)
{
	int need_borrow = FALSE;
	int lm,ls,i;
	char *res;
	char *mnd,*p;

	if(!isgte(mend,shend))return NULL;

	lm = strlen(mend);
	ls = strlen(shend);
	res = (char *)malloc(strlen(mend)+1);
	if(res == NULL){
		fprintf(stderr,"sbc: Unable to malloc space for result\n");
		exit(1);
	}
	p = mnd = (char *)malloc(strlen(mend)+1);
	if(mnd == NULL){
		fprintf(stderr,"sbc: Unable to malloc\n");
		exit(1);
	}
	strcpy(mnd,mend);

	/* Fill result array with digit zero */

	for(i=0;i<lm;i++)res[i]='0';
	res[lm] = '\0';


	for(i = 0; i<lm; i++){
		if(need_borrow){
			if(mnd[lm-i-1] == '0')
				mnd[lm-i-1] = '9'; /* need_borrow stays TRUE */
			else {
				--mnd[lm-i-1];
				need_borrow = FALSE;
			}
		}
		if( i < ls )
		 need_borrow |= sub(mnd[lm-i-1],shend[ls-i-1],res+lm-i-1);
		else
		 need_borrow |= sub(mnd[lm-i-1],'0',res+lm-i-1);
	}
		
	free(p);
	return res;
}

/* Multiplies digit string by digit character. Returns string pointing
   to answer. Leading zeros are removed.
*/

char *
times_digit(char *mcand, char dgt)
{

	int i,len;
	int carry=0,idgt,curdgt,prod;
	char *res,*tmp;

	if(mcand == NULL) return NULL;

	idgt = dgt - '0';
	len = strlen(mcand);
	tmp = (char *)malloc(len+2);
	if(tmp==NULL){
		fprintf(stderr,"Unable to malloc partial product array\n");
		exit(1);
	}
	
	tmp[len+1]='\0';

	for(i=0;i<len;i++){
		curdgt = mcand[len-i-1] - '0';
		prod = curdgt*idgt + carry;
		tmp[len-i] = '0' + (prod % RADIX);	
		carry = prod/RADIX;
	}
	tmp[0] = '0' + carry;
	if(tmp[0]=='0'){
		res = (char *)malloc(len+1);
		if(res == NULL){
			fprintf(stderr,"Unable to malloc partial product array\n");
			exit(1);
		}
		res[len]='\0';
		strcpy(res,tmp+1);
		free(tmp);
	}
	else res = tmp;
	return res;
}

/* Allocates a new string consisting of front and back glued together */

char *
splice(char *front, char *back)
{

	char *res;
	int len;

	if(front == NULL)return back;
	if(back == NULL) return front;  
	len = strlen(front)+strlen(back);

	res = (char *)malloc((len+1)*sizeof(char));
	if(res == NULL) {
		fprintf(stderr,"splice: unable to malloc\n");
		return NULL;
	}
	strcpy(res,front);
	strcat(res,back);
	return res;
}

/* Creates a new string by cutting the given number of characters off
   the front of source */

char *
cut_off(char *source, int n)
{
	int k;
	char *res;

	if((n <= 0)||(source == NULL))return NULL;
	k = strlen(source);
	k = ( n > k ? k : n );
	res = (char *)calloc(sizeof(char),(k+1));
	if(res == NULL){
		fprintf(stderr,"cut_off: Unable to malloc\n");
		return NULL;
	}
	strncpy(res,source,k);
	return res;
}

/* Shift source one unit left and fill with trailing '0' char. Return pointer
   to resulting string */

char *
shift_left(char *s) 

{

	char *res;
	int n;

	if(!s) return NULL;
	n = strlen(s);
	res = (char *)calloc(sizeof(char),(n+2));
	if(res == NULL){
		fprintf(stderr,"shift_left: Unable to malloc\n");
		return NULL;
	}
	strcpy(res,s);
	*(res+n) = '0';
	return res;

}

/* multiply the natural numbers represented by m1 and m2 and return a
 * pointer to a string representing the answer. Return pointer to
 * array of strings representing partial products if the pointer
 * argument is non-null */

char *multiply(char *m1, char *m2, char ***p)
{

	int i,n,j,k,l;
	int carry=0,v;
	char *(*partials)[];
	char *res,*temp;
	char *mpr,*mcnd;
	char c;

	if(m1 == NULL || m2 == NULL) return NULL;
	n = strlen(m1);
	i = strlen(m2);
	if(i < n){
	       	n = i;
		mpr = m2;
		mcnd = m1;
	}
	else {
		mpr = m1;
		mcnd = m2;
	}
	if(n == 0) return NULL;

	partials = (char *(*)[])malloc(n*sizeof(char *));
	if(!partials){
		fprintf(stderr,"Unable to malloc pointer array\n");
		exit(1);
	}
	for(i=0;i<n;i++){

		temp = times_digit(mcnd,mpr[n-i-1]);
		if(!temp){
			for(j=0;j<i;j++)free((*partials)[j]);
			free(partials);
			return NULL;
		}

		for(j=0;j<i;j++){
			(*partials)[i]=shift_left(temp);
			free(temp);
			temp = (*partials)[i];
		}
		(*partials)[i] = temp;
	}	
	k = strlen((*partials)[n-1])+1; /* This, or this - 1, is the number of
				      of digits in the result */
	res = (char *)calloc(sizeof(char),k+1);
	if(res == NULL){
		fprintf(stderr,"shift_left: Unable to malloc\n");
		return NULL;
	}
	for(i=k;i>1;i--){
		/* figure out the ith digit (from left) of result */
		v = 0;
		for(j=0;j<n;j++){ /* loop over partials, adding up digits */
			l = strlen((*partials)[j]);
			if(k-i < l)
				c = (*partials)[j][l-1-(k-i)];
			else c = '0';
			v += (c - '0');
		}
		v += carry;
		res[i-1] = ( v % 10 ) + '0';
		carry = v/10;
	}	
	if(carry)
		res[0] = carry + '0';
	else {
		res[0]=' ';
		for(i=0;i<strlen(res)-1;i++)res[i]=res[i+1];
		res[i]=0;
	}

	if(p)
		*p = (char **)partials;
	else {
		for(i=0;i<n;i++)free((*partials)[i]);
		free(partials);
	}
	return res;
}

/* Add with carry */

char *add(char *A, char *B)
{
	char *S,*L,*R;
	int n,m,a,b,c,s,i,N;

	if(!A)return NULL;
	if(!B)return NULL;

	n = strlen(A);
	m = strlen(B);
	if(n<m)
		N = m; /* max (m,n) -> N */
	else N = n;
	/* allocate memory to store the result, small and large operand */
	S = (char *)malloc((N+1)*sizeof(char));
	L = (char *)malloc((N+1)*sizeof(char));
	R = (char *)malloc((N+2)*sizeof(char));
	if(!S||!L||!R) {
		fprintf(stderr,"add: cannot allocate memory\n");
		exit(1);
	}
	S[N]=0;L[N]=0;
	if(n < m){
		for(i=0;i<n;i++){
			S[N-i-1]=A[n-i-1];
			L[N-i-1]=B[N-i-1];
		}
		for(i=n;i<N;i++){
			S[N-i-1]='0';
			L[N-i-1]=B[N-i-1];
		}
	}
	else {
		for(i=0;i<m;i++){
			S[N-i-1]=B[m-i-1];
			L[N-i-1]=A[N-i-1];
		}
		for(i=m;i<N;i++){
			S[N-i-1]='0';
			L[N-i-1]=A[N-i-1];
		}
	}
	c = 0;
	for(i=N-1;i>=0;i--){
		a = S[i]-'0';
		b = L[i]-'0';
		s = (a + b + c)%10;
		c = (a + b + c)/10;
		R[i+1] = s + '0';
	}
	if(c){ /* carry out of sum */
		R[0]=c+'0';
		R[N+1] = 0;
	}
	else { /* Avoid returning a leading zero */
		for(i=0;i<N;i++)
			R[i]=R[i+1];

		R[N]=0; /* terminate string */
	}
	free(S); free(L);
	return R;
}
