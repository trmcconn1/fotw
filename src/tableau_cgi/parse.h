#define VARIABLE 0
#define OR 1
#define AND 2
#define IMPLIES 3
#define EQUIVALENCE 4
#define NAND 5
#define STROKE 6
#define NOT 7
#define RIGHT_DELIMITER 8
#define LEFT_DELIMITER 9
#define LEAF 10
#define END_OF_INPUT 11
#define NEWLINE 12
#define TRUE_SIGN 13
#define FALSE_SIGN 14

#define MAX_PRECEDENCE 10

/* Associativity */
#define LEFT_ASSOCIATIVE 0
#define RIGHT_ASSOCIATIVE 1
#define ASSOCIATIVE 2

#ifdef PARSE_C
#define OFFICIAL_SYMBOLS
char *written_as[] = {"VAR","+","^","->","<->","NAND","|","~",")","("};
#endif

extern char *written_as[];



#define YY_DECL int yylex(void)
extern int yylex(void);
extern char current_token[];
extern void discard(void);
extern void myunput(void);
FILE *yyin;
