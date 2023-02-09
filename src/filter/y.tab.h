typedef union {
	char *str;
	char chr;
	int integer;
	} YYSTYPE;
#define	ENDFILE	258
#define	ENDCOMMANDS	259
#define	OPTION	260
#define	ARG_TAG	261
#define	RIGHTARG	262
#define	LEFTARG	263
#define	LITERAL_BEGIN	264
#define	LITERAL_END	265
#define	CHAR	266
#define	BACKSLASHES	267
#define	SLASH	268
#define	C_ESCAPE	269
#define	BACKDOLLAR	270


extern YYSTYPE yylval;
