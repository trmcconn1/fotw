#ifndef BISON_CGI_TAB_H
# define BISON_CGI_TAB_H

#ifndef YYSTYPE
typedef union {
	char *str;
	} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	__AMPERSAND	257
# define	__EQUALS	258
# define	__TEXT	259
# define	__HEX	260
# define	__ENDFILE	261


extern YYSTYPE cgilval;

#endif /* not BISON_CGI_TAB_H */
