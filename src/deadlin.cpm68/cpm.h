/* This is the file that defines CP/M 68 specific functions which
are semi-build-in Acyclon C (native C compiler) to use them with
another ANSI compiler */

#ifndef CPMHINCLUDED
#define CPMHINCLUDED

#include <stdio.h>
#include <portab.h>

/* Unclear where from, but that's right! (they really are in CLIB file)*/
WORD __BDOS(WORD,LONG);
WORD rename(BYTE *,BYTE *);
BYTE * strchr(BYTE *, BYTE);
BYTE * strrchr(BYTE *, BYTE);

extern WORD errno;
/* from CPM 68K C Programmers Guide Jun 83*/

void abort(WORD);
/* WORD abs(WORD); Defined as macro */
WORD access(BYTE *,WORD);
WORD atoi(BYTE *);
LONG atol(BYTE *);
WORD brk(BYTE *);
/* BYTE * sbrk(); brrr whats inc? */
BYTE * calloc(WORD,WORD);
BYTE * malloc(WORD);
BYTE * realloc(BYTE *,WORD);
void free(BYTE *);
WORD chmod(BYTE *,WORD);
WORD chown(BYTE *,WORD,WORD);
WORD close(WORD);
WORD creat(BYTE *,WORD );
WORD creata(BYTE *,WORD );
WORD creatb(BYTE *,WORD );
void exit(WORD);
void _exit(WORD);
WORD fclose(FILE *);
WORD fflush(FILE *);
/*WORD feof(FILE *); Macro in stdio */
/* WORD ferror(FILE *); Macro in stdio */
/* clearerr(FILE *); Macro in stdio */
/* WORD fileno(FILE *); Macro */
FILE * fopen(BYTE *,BYTE *);
FILE * fopenb(BYTE *,BYTE *);
FILE * fopena(BYTE *,BYTE *);
FILE * freopen(BYTE *,BYTE *,FILE *);
FILE * freopenb(BYTE *,BYTE *,FILE *);
FILE * freopena(BYTE *,BYTE *,FILE *);
FILE * fdopen(WORD,BYTE *);
WORD fseek(FILE *,LONG,WORD);
WORD rewind(FILE *);
LONG ftell(FILE *);
/* getc 
getchar

This function is implemented as a macro in <stdio.h>, and arguments should not have side effects. */
WORD fgetc(FILE *);
WORD getw(FILE *);
LONG getl(FILE *);
BYTE * getpass(BYTE *);
WORD getpid();
BYTE * gets(BYTE *);
BYTE * fgets(BYTE *,WORD,FILE *);
BYTE * index(BYTE *,BYTE);
BYTE * rindex(BYTE *,BYTE);
WORD isatty(WORD);
LONG lseek(WORD,LONG,WORD);
LONG tell(WORD);
BYTE * mktemp(BYTE *);
WORD open(BYTE *,WORD);
WORD opena(BYTE *,WORD);
WORD openb(BYTE *,WORD);
WORD perror(BYTE *);
WORD printf(BYTE *,...);
WORD fprintf(FILE *,BYTE *,...);
BYTE * sprintf(BYTE *,BYTE *,...);
/* putc, putchar are macros */
WORD fputc(BYTE,FILE *);
WORD putw(WORD,FILE *);
LONG putl(LONG,FILE *);
WORD puts(BYTE *);
WORD fputs(BYTE *,FILE *);
/* WORD qsort(BYTE *,WORD,WORD,WORD);  last is WORD is function  call*/
WORD srand(WORD);
WORD rand();
WORD read(WORD,BYTE *,WORD); 
WORD scanf(BYTE *,...);
WORD fscanf(FILE *,BYTE *,...);
WORD sscanf(BYTE *,BYTE *,...);

#include <setjmp.h>
WORD setjmp(jmp_buf);
void longjmp(jmp_buf,WORD);
/* signal */
BYTE * strcat(BYTE *,BYTE *);
BYTE * strncat(BYTE *,BYTE *, WORD);
WORD strcmp(BYTE *,BYTE *);
WORD strncmp(BYTE *,BYTE *,WORD);
BYTE * strcpy(BYTE *,BYTE *);
BYTE * strncpy(BYTE *,BYTE *,WORD);
WORD strlen(BYTE *);
WORD swapb(BYTE *,BYTE *,WORD);
BYTE * ttyname(WORD);
BYTE ungetc(BYTE,FILE *);
WORD unlink(BYTE *);
WORD write(WORD,BYTE *,WORD);


#endif