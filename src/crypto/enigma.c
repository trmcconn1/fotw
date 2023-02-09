/* enigma.c: implementation of encryption routine for an algorithm similar to
   that of the German WWII enigma cipher. 

   The enigma machine was invented by Arthur Scherbius, a German engineer,
   in the 1920s. It was originally intended for commercial use, but found
   its most famous applications with the German military during WWII. There
   is an enormous bibliography on the engima machines, so we won't go into
   further historical details here. See the following website for further
   technical information, including precise wiring diagrams of the actual
   machines:

	http://codesandciphers.org.uk/enigma/enigma1.htm

   How to use the key to set up the machine state is somewhat problematic
   because the German "key" was only 3 letters, used to set the initial rotor
   positions. (The "ring" settings.) The choice of rotors and the plugboard
   settings were determined by a book of daily settings distributed ahead of
   time. In order to shoehorn this reality into the overall package 
   philosophy, we have decided upon the following compromises:

	- the key must be at least 3 characters long. The first three 
          characters determine the initial ring settings.

	- the key is otherwise arbitrary, but the user can, if he wishes, set
          the daily settings explicitly by giving a key in the following
          format:

		xyz[abc](s1t1)(s2t2)...(s36t36)[...]

	  where xyz are the required 3 initial characters, abc give the
          selected rotor numbers from 0-4, and the rest are pairs of characters
          up to a limit of 36 pairs which give transpositions to be inserted
          into the plug board. 

	- If the 2nd 3 characters cannot be interpreted as a valid choice
          rotors, then a warning is printed and the first three rotors in
          order are used. 

   The wiring diagrams of the real enigma are based upon a 26 letter
   alphabet, whereas, as in all programs in this package, we use the alphabet
   of 95 printable characters.  In our emulation, we decided to simply scale 
   up the cycle lengths of the rotor permutations to the actual alphabet size,
   in order to preserve the "spirit" of the rotor wirings. Thus, e.g, the 
   real enigma rotor 1 had a permutation of the 26 letter alphabet having 
   cycles of length 10,4,4,3,2,2, and 1. Our rotor has cycles approximately 
   95/26 as long: 36,15,15,11,7,7,4. To ease the tedium of entering 95 letters 
   in each of 5 rotor arrays plus reflector, we created a tool in the utils
   directory (utils/mkrotor,) which creates a random rotor with given cycle
   lengths. 

   One historical question of interest is whether there were any mathematical
   principles followed in choosing the actual rotor permutations? Perhaps
   they were nearly chosen at random? (The author would appreciate
   hearing from anyone who can shed some light on this question.)

   What turned out to be a weakness of the original enigma machine is that 
   no character could be enciphered into itself. This is a consequence of the
   fact that the reflector (for a 26 letter alphabet) consisted only of
   transpositions and the fact that, apart from the transposition, the 
   permutation performed on the way back through the machine is the inverse
   of the one on the way forward. We have, by default, a 95 letter alphabet.
   Since 95 is odd, there must be one fixed point, though it changes 
   with each plaintext position. It may be that this is even more
   a weakness than the historical behavior. Anyone who wants a more
   authentic enigma could do the following: rebuild utils/mkrotor, and then
   this file with NCHARS=94. (Note that all the rotors and reflector need
   to be rebuilt.) This effectively eliminates character number 126 (~) from
   the alphabet. One then needs to eschew this character in plain-text.

   One other historical curiosity concerns the use of the enigma, rather
   than the machine design itself: The german code clerks were supposed to
   choose the ring settings themselves "at random" and transmit their chosen
   settings as the first 3 characters of the message encoded with the settings
   set forth in the daily codebook. It was understood that the recipient would
   decode these with the settings of the day and then change the settings to
   the encoded ones. These were then used to decipher the rest of the
   message (which had, of course, been encyphered accordingly.) We decided
   not to try to build this behavior into the emulation because it was a
   matter of practise, not design.         

*/  
  
#include "global.h"

/* These contain indices, not char values */

/* utils/mkrotor -g 36 -g 15 -g 15 -g 11 -g 7 -g 7 -g 4 
Cycle 1: o{<Nk5CeB=mY'2lpP(4RT/08j`_Sf~3VzM#h   (length=36)
Cycle 2: dGL*g>itOU]"\wQ   (length=15)
Cycle 3: .&cEvD)A-%^n$6s   (length=15)
Cycle 4: u|9@!}Zqb7F   (length=11)
Cycle 5: J:+Ix;W   (length=7)
Cycle 6: Xa? ry1   (length=7)
Cycle 7: [HK,   (length=4)
*/

static rotor1[] = 
{82,93,60,72,22,62,67,18,20,33,71,41,59,5,6,16,24,56,76,54,50,35,83,38,74,32,
11,55,46,77,73,0,1,13,29,69,9,86,85,44,43,88,26,12,10,3,75,53,8,68,52,70,15,
61,90,42,65,7,81,40,87,2,78,51,63,31,23,37,39,34,94,30,79,84,64,21,80,57,4,
91,48,66,89,14,47,92,36,49,27,17,45,28,25,58,19,};

/* utils/mkrotor -g 29 -g 26 -g 11 -g 7 -g 7 -g 7 -g 4 -g 4
Cycle 1: Kp nq(DiP+8&4$%;MUxOR0m>S*t-e   (length=29)
Cycle 2: H=5<A!.JQwvY2|d6~y^Wo)}{l@   (length=26)
Cycle 3: Tz1[aVjkc/h   (length=11)
Cycle 4: gI:_Cbr   (length=7)
Cycle 5: ?G#BF\f   (length=7)
Cycle 6: 7]`3X'Z   (length=7)
Cycle 7: N,E9   (length=4)
Cycle 8: u"Ls   (length=4)
*/

static rotor2[] =
{78,14,44,34,5,27,20,58,36,93,84,24,37,69,42,72,77,59,92,56,4,28,94,61,6,46,
63,45,33,21,51,39,40,1,38,66,73,25,60,3,29,26,49,80,83,53,12,50,11,87,16,10,
90,88,74,79,7,18,23,65,70,64,55,35,19,54,82,15,22,43,31,41,52,48,75,67,32,30,
81,9,0,8,71,85,13,2,57,86,47,62,17,76,68,91,89,};

/* utils/mkrotor -g 29 -g 62 -g 4 
Cycle 1: xd{!kwX~cJ\(DZ`bB.$KWh?"e ,yq   (length=29)
Cycle 2: vfjV^lGTAF>t/I9m2'4=+]#r*&R05Q|-@ig%S;_)n6zsP<YoEu8LO7C[:}aHMU(len=62)
Cycle 3: 3p1N   (length=4)
*/

static rotor3[] =
{12,75,69,82,43,51,50,20,36,78,6,61,89,32,4,41,21,46,7,80,29,49,90,35,44,77,
93,63,57,11,84,2,73,38,14,59,58,85,30,52,45,25,60,55,47,53,19,23,28,92,16,27,
33,86,62,72,94,79,64,26,8,3,76,9,66,40,34,42,91,0,74,5,31,71,54,87,39,18,22,
37,17,88,10,48,15,24,70,56,68,81,83,1,13,65,67,};

/* utils mkrotor -g 81 -g 7 -g 7
Cycle 1: LN,.!XT)oH"x M%(&zlmB5SD12eZ{faI8nY9F-@:WC0\q4_s+KcQ]3rg=V<;>ydPj6h$E}
AuGRJ#b|vi?   (length=81)
Cycle 2: [*w`tUp   (length=7)
Cycle 3: 7k'^O~/   (length=7)
*/

static rotor4[] =
{45,56,88,66,37,8,90,62,6,79,87,43,14,32,1,23,60,18,69,82,63,51,72,75,78,38,
55,30,27,54,89,44,26,85,21,16,17,93,13,50,2,24,3,67,46,5,12,94,74,61,42,36,9
,80,28,35,52,25,91,10,81,19,47,83,84,41,92,49,48,58,65,29,4,31,22,7,77,34,57,
40,59,20,71,11,53,39,73,64,0,68,76,70,86,33,15,};

/* utils/mkrotor -g 40 -g 33 -g 22
Cycle 1: }SQ>+GgFI14E2lH$O~(;dc o,Z7"nM:k{j*-8sW^   (length=40)
Cycle 2: %h#=VPA|NK?/.Bx9t5@b_YU`C]iXL3)[y   (length=33)
Cycle 3: 0p\fzR!Jr6a&<uwDveqTm'   (length=22)
*/

static rotor5[] =
{79,42,78,29,47,72,28,16,27,59,13,39,58,24,34,14,80,20,76,9,37,32,65,2,83,84,
75,68,85,54,11,15,66,92,88,61,86,18,41,71,4,17,82,31,19,26,43,94,33,30,1,49,
77,64,48,62,44,53,23,89,70,73,93,57,35,6,63,0,67,81,90,38,3,56,10,91,40,7,45
,12,60,52,22,55,21,87,69,36,25,5,50,74,46,51,8,};

static int stecker[NCHARS]; /* or plugboard: initialized by user */ 

/* utils mkrotor -s 314159265 -r -c
Cycle 1: g   (length 2)
Cycle 2: i!  (length 2)
Cycle 3: w"  (length 2)
Cycle 4: ;#  (length 2)
Cycle 5: r$  (length 2)
Cycle 6: .%  (length 2)
Cycle 7: _&  (length 2)
Cycle 8: C'  (length 2)
Cycle 9: 3(  (length 2)
Cycle 10: @)  (length 2)
Cycle 11: c*  (length 2)
Cycle 12: W+  (length 2)
Cycle 13: P,  (length 2)
Cycle 14: l-  (length 2)
Cycle 15: R/  (length 2)
Cycle 16: o0  (length 2)
Cycle 17: A1  (length 2)
Cycle 18: b2  (length 2)
Cycle 19: ^4  (length 2)
Cycle 20: `5  (length 2)
Cycle 21: v6  (length 2)
Cycle 22: y7  (length 2)
Cycle 23: u8  (length 2)
Cycle 24: H9  (length 2)
Cycle 25: V:  (length 2)
Cycle 26: E<  (length 2)
Cycle 27: |=  (length 2)
Cycle 28: k>  (length 2)
Cycle 29: Q?  (length 2)
Cycle 30: MB  (length 2)
Cycle 31: \D  (length 2)
Cycle 32: IF  (length 2)
Cycle 33: SG  (length 2)
Cycle 34: qJ  (length 2)
Cycle 35: {K  (length 2)
Cycle 36: [L  (length 2)
Cycle 37: dN  (length 2)
Cycle 38: ZO  (length 2)
Cycle 39: hT  (length 2)
Cycle 40: sU  (length 2)
Cycle 41: }X  (length 2)
Cycle 42: ]Y  (length 2)
Cycle 43: ta  (length 2)
Cycle 44: ne  (length 2)
Cycle 45: zf  (length 2)
Cycle 46: xj  (length 2)
Cycle 47: pm  (length 2)
Cycle 48: ~  (length 1)
*/

/* Modified output of utils/mkrotor -s 314159265 -r by swapping w and
   ~. ~ is now the fixed point */ 

static int reflector[] = 
{71,73,87,27,82,14,63,35,19,32,67,55,48,76,5,50,79,33,66,8,62,64,86,89,85,
40,54,3,37,92,75,49,9,17,45,7,60,28,41,51,25,38,81,91,59,34,68,58,12,31,15,
39,72,83,26,11,93,61,47,44,36,57,20,6,21,84,18,10,46,78,90,0,52,1,88,30,13
,80,69,16,77,42,4,53,65,24,22,2,74,23,70,43,29,56,94,};

/* These contain the current rotation positions of the rotors. They are set
initially from the key ("Ring" settings )*/

static int m1,m2,m3;

/* These contain pointers to the 3 rotors actually in use: */

static int (*r1)[], (*r2)[], (*r3)[];

/* These contain the inverses of the permutations in use */

static int r1inv[NCHARS], r2inv[NCHARS], r3inv[NCHARS]; 


/* These contain the index of "notch" on each rotor relative to the starting
   position (= char START_CHAR.)  I'm not sure I understand the physical 
   details of what the "notch" is, but I'll take it to mean that part of the
   wheel that gets "caught up to" by a projection on the next inner rotor, in
   order to knock it forward one notch. We'll assume the projection on each
   rotor is at the START_CHAR position. Thus, in the simulation, "knock on"
   actually occurs for the 2nd rotor when m1 == N + m2, and N is the
   Nj below in use for rotor 2. */

#define N1 50
#define N2 15
#define N3 11
#define N4 77
#define N5 6

/* These contain the indices of notches on the 2nd and 3rd rotors actually 
in use: */

static int n2,n3;

/* This counts the number of characters encyphered since the beginning of
   encypherment, not just for the current block. It is used to keep the
   rotors advanced by the right amounts. */

static int l; 

int my_encrypt(char *plain_text, char *cypher_text, int n)
{
	int i,j,k,a,b,d; 
	int c;
	static int firstcall = 1;
	static int key_len = 0;
		
	if(firstcall){

		/* Setup for this cypher is more tedious than most */

		key_len = strlen(key);

		/* Initialize ring settings, and which rotors
                    will be used */


		/* First three characters of key give initial rotor settings */
		/* Next three characters mod 5 select rotors */
		/* Any remaining characters go into the stecker in pairs, up
                   to a limit of 36 pairs. Remaining characters are ignored. */
                /* At least the first three characters must be supplied */

		if(key_len < 3){
			my_error("Key must be at least length 3\n");
			return -1;
		}

		m1 = key[0]-START_CHAR;
		m2 = key[1]-START_CHAR;
		m3 = key[2]-START_CHAR;

		if(key_len >= 4){ /* has user given >= 1 rotor choice ? */
			a = (key[3]-'0')%5;
			if(key_len >= 5){  /* >= 2 ? */
				b = (key[4]-'0')%5;
				if(key_len >= 6)
					d = (key[5]-'0')%6;
			        else /* choose first thing that's different */
					for(j=0;j<5;j++)
						if((j!=a)&&(j!=b)){
							d = j;
							break;
						}
			}
			else /* Need to pick two rotors on our own */
				for(j=0;j<5;j++)
					if(j!=a){
						b = j;
						for(i=0;i<5;i++)
							if((i!=a)&&(i!=b)){
								d = i;
								break;
							}
						break;
					}
								
		}
		else {
			a = 0; /* no rotors given: just take first 3 */
			b = 1;
			d = 2;
		}

	/* Check to see if a,b, and d are unique. If not, then user
           screwed up or is not giving rotors in the key.  */

	if((a==b)||(a==d)||(b==d)){
		fprintf(stderr,"No rotor choice given. Using defaults.\n");
		a = 0;
		b = 1;
	        d = 2;
	}

		/* a, b, and d now are 3 unique numbers in 0-4 range */
		switch(a){
			case 0:
				r1 = &rotor1;
				break;
			case 1:
				r1 = &rotor2;
				break;
			case 2:
				r1 = &rotor3;
				break;
			case 3:
				r1 = &rotor4;
				break;
			case 4:
				r1 = &rotor5;
				break;
			default:
				my_error("invalid rotor number\n");
				return -1;
		}
		switch(b){
			case 0:
				r2 = &rotor1;
				n2 = N1;
				break;
			case 1:
				r2 = &rotor2;
				n2 = N2;
				break;
			case 2:
				r2 = &rotor3;
				n2 = N3;
				break;
			case 3:
				r2 = &rotor4;
				n2 = N4;
				break;
			case 4:
				r2 = &rotor5;
				n2 = N5;
				break;
			default:
				my_error("invalid rotor number\n");
				return -1;
		}
		switch(d){
			case 0:
				r3 = &rotor1;
				n3 = N1;
				break;
			case 1:
				r3 = &rotor2;
				n3 = N2;
				break;
			case 2:
				r3 = &rotor3;
				n3 = N3;
				break;
			case 3:
				r3 = &rotor4;
				n3 = N4;
				break;
			case 4:
				r3 = &rotor5;
				n3 = N5;
				break;
			default:
				my_error("invalid rotor number\n");
				return -1;
		}

		/* Calculate the inverses of the chosen transformations */

		for(i=0;i<NCHARS;i++){

			r1inv[(*r1)[i]] = i;
			r2inv[(*r2)[i]] = i;
			r3inv[(*r3)[i]] = i;
		}

		/* Set up stecker from any remaining characters */
		/* First, mark all entries with -1 to signify not set yet */
		for(i=0;i<NCHARS;i++)
			stecker[i] = -1;

		k = key_len > 6 ? key_len - 6 : 0;
		k = k/2;
		k = k > 36 ? 36 : k;
		for(j=0;j<k;j++){
			stecker[key[6+2*j]-START_CHAR]
				= key[6+2*j+1]-START_CHAR;
			stecker[key[6+2*j+1]-START_CHAR]
				= key[6+2*j]-START_CHAR;
		}
		for(i=0;i<NCHARS;i++)
			if(stecker[i]==-1)stecker[i] = i;
		
		firstcall = 0;
		return 0;
	}

	/* Note: for this cypher, encryption is the same as decryption */

	for(k=0;k<n;k++)   
		switch(c=plain_text[k]){
				case '\n':
					cypher_text[k] = '\n';
					break;
				case '\t':
					cypher_text[k] = '\t';
					break;
				default:

					/* When it comes right down to it, the
                                           operation is pretty simple: */

					c -= START_CHAR; /* convert to an index */

					c = stecker[c];  /* do plug board */

					/* step the rotors */
					m1++;
					if((l+m1)%NCHARS == (n2+m2)%NCHARS)m2++;
					if((l+m2)%NCHARS == (n3+m3)%NCHARS)m3++;
					
					/* Do rotor transformations */

					c = *((*r1)+((c+m1)%NCHARS));
					c = *((*r2)+((c+m2)%NCHARS));
					c = *((*r3)+((c+m3)%NCHARS));

					/* Do reflection */

					c = reflector[c];

					/* Do inverse rotor transformations on 
                                           the way back through, in reverse
                                           order. */

					c = (NCHARS + r3inv[c] - (m3%NCHARS))%NCHARS;
					c = (NCHARS + r2inv[c] - (m2%NCHARS))%NCHARS;
					c = (NCHARS + r1inv[c] - (m1%NCHARS))%NCHARS;

					/* Do final plugboard transformation */

					c = stecker[c];

					/* Translate back to char and store */

					cypher_text[k] = c+START_CHAR;
					l++;
		}
	return n;
}
	
/* Default block size routine */
int set_block_size(int *ip)
{
	*ip = INPUT_BUFFER_SIZE;
	return 0;
}
	
