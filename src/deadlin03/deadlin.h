/* Deadlin.h: main header file for the edlin clone deadlin    */

#define VERSION 0.3
#define TRUE 1
#define FALSE 0
#ifdef _MSDOS
#define OPSEP '/'
#else
#define OPSEP '-'
#endif

#define MAX_LINES 65529
#define DEFAULT_BRK 524288    /* 512K */
#define MAX_LINE_LENGTH 255  
#define DEFAULT_MAX_BYTES 65529

/* Useful Macros */
#define FLAGGED(aflag) ( (Status) & (aflag) )
#define FLAG(aflag)    ( (Status) |= (aflag) )
#define UNFLAG(aflag)  ( (Status) &= (~(aflag)) )
#define STARTLINE(x) if((x)==CurrentLine)printf("%9d:*",(x)+1);\
			else printf("%9d: ",(x)+1);
#define STARTUNNUMBERED printf("%11s"," ");

/* Bitmaps */

#define DIRTY_FILE 0x01
#define INPUT_MODE 0x02
#define NEW_FILE 0x04
#define NOT_RESIDENT 0x08   /* File not fully read into memory */
#define IGNORE_EOF 0x10
#define LOG_PRINTER 0x20
#define INPUT_SET 0x40
#define INSERT_MODE 0x80

/* Return values from Parse */
/* Don't use 0-10 here */

#define END_EDIT '\11'
#define APPEND_LINES '\12'


