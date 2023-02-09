/* enquote.c  leave numbers alone, surround non-numbers with */
/* quotes. Useful for import into Lotus 123 */

#include <stdio.h>

#define QUOTE '\"'
#define MAXLEN 20

void
main()
{
     char ff = 0; /* field flag -- in word or not ? */
     char nf = 1; /* number flag -- in number or not? */
     char df = 0; /* dec. pt. flag */
     char temp; /* storage for character */
     char word[MAXLEN]; /* storage for string */
     char nxtchr; /* storage for char */
     int count = 0;

     while ((nxtchr = getchar()) != EOF) {
                
          if(nxtchr == ' '|| nxtchr == '\t' || nxtchr == '\n'){
                 /* output section */
               
            if(ff == 0) putchar(nxtchr);
            else { temp = nxtchr;
                   ff = 0;
                   if (nf == 0) {
                     word[count] = '\0';
                     putchar(QUOTE);
                     printf("%s",word); /* output quoted word */ 
                     putchar(QUOTE);
                     putchar(temp);
                     nf = 1;}
                    else { word[count] = '\0';
                           printf("%s",word);  /* output number*/
                           putchar(temp);
                         }
                     ff = 0;
                    nf = 1;
                    df = 0;
                     count = 0;
                 }
            }
                 /* Parse input and build word          */
            else {   ff = 1;
                     if (nxtchr == '.'|| ('0'<= nxtchr && nxtchr
                         <= '9')) {
                         if (nf == 1) {
                           if (nxtchr == '.') {
                              if (df == 1 ) nf = 0;
                              else df = 1;
                           }
                         }
                      }
                      else nf = 0;
                      word[count] = nxtchr;
                      count++;
                  }
     }  /* endwhile */
} /* end main */
