/* rotate.c: left rotate an array using various methods. Essentially, this
   forms a solution to problem 2 in column 2 of

   	Programming Pearls 2nd Edition , by Jon Bentley, Addison-Wesley,
        Reading, 2000.

   By Terry R. McConnell

   compile: cc -o rotate rotate.c. Edit main to select rotation method used,
   and also if you want to build a profiling platform. */

#include <stdlib.h>
#include <stdio.h>

/* Rotate string stored in array pointed to by p left i positions. n is the
   string length. Use the first method of problem 2.3 */

int rotate1(int n, int i, char *p)
{
	char t;  /* temp holder */
	int j,m=0,gcd;

	i = i % n;
	if(i==0) return 0; /* nothing to do */

/*      Let gcd = gcd(n,i). Loop m over distinct residue classes 
        of string index mod gcd */

	do {
		j = 1;   
		t = *(p+m); /* store value stored in first copy-to location */
		while((j*i)%n) { /* Walk index m+j*i over this residue class */ 


#ifdef DEBUG
			printf("%c <-- %c\n" , p[((j-1)*i+m)%n],  p[(j*i+m)%n]);
#endif


			/*  Copy each index location into previous */
			p[((j-1)*i+m)%n] = p[(j*i+m)%n];
			j++;
		}

		if(m==0) /* store gcd value 1st time through  */
			gcd = n/j;

		p[((j-1)*i+m)%n] = t; /* copy saved first into last index
                                         of residue class */
#ifdef DEBUG
		printf("m=%d,j=%d,gcd=%d,t=%c\n",m,j,gcd,t);
#endif
	} while ( ++m < gcd ); /* next residue class */
	return 0;	
}

/* Rotate string stored in array pointed to by p left i positions. n is the
   string length. Use the second method of problem 2.3 */

int rotate2(int n, int i, char *p)
{
	char t, *dest,*src;
	int l,j;

/* Think of string p as abc */

 	i = i % n;
	if(i==0)return 0; /* nothing to do */

#ifdef DEBUG
	printf("%s n=%d, i=%d\n",p,n,i);
#endif

	if(n-i >= i){  /* |a|=|c|=i */ 
		l = i;
		src = p;
		dest = p + n - i;
	}
	else {        /*  |a|=|c|=n-i, call recursively on ba */
		l = n-i;
		src = p+i;
		dest = p;
	}

	/* swap a with c: abc --> cba */

	for(j=0;j<l;j++){
		t = dest[j];
		dest[j] = src[j];
		src[j] = t;
	}	

	if(n-i == i) return 0; /* base of recursion */
	if(n-i>i) /* call recursively on cb */
		rotate2(n-i,i,p);
	else /* call recursively on ba */
		rotate2(i,2*i-n,p+n-i);
	return 0;
}

int reverse(int first, int last, char *p)
{
	char t;
	int j;
	
	for(j=0;j<(last-first)/2;j++){
		t = p[last-j];
		p[last-j] = p[first+j];
		p[first+j]=t;
	}
	return 0;
}

/* Cleverest method of all: divide string as ab, where a has length
   equal to the amount to rotate. Reverse a. Reverse b. Then reverse the
   whole string. Wish I'd thought of that! */

int rotate3(int n, int i, char *p)
{

	i = i%n;
	if(i==0)return 0;

	reverse(0,i-1,p);
	reverse(i,n-1,p);
	reverse(0,n-1,p);
	return 0;
}

int main(void)
{
	int n;
	char my_array[] = "Four score and 7 years ago, our forefathers ...";

	n = strlen(my_array);

#if 0
	/* Method 1 */
	printf("Method 1:\n\n");
	printf("%s\n",my_array);
	rotate1(n,13,my_array);
	printf("%s\n",my_array);

	/* Method 2 */
	printf("\n\nMethod 2:\n\n");
	printf("%s\n",my_array);
	rotate2(n,13,my_array);
	printf("%s\n",my_array);
#endif
	/* Method 3 */
	printf("\n\nMethod 3:\n\n");
	printf("%s\n",my_array);
	rotate3(n,13,my_array);
	printf("%s\n",my_array);

	return 0;
}
