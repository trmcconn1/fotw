#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif
# define	TOKEN	257
# define	KEYWORD	258
# define	HARDENDFILE	259
# define	SOFTENDFILE	260


extern YYSTYPE yylval;

#endif /* not BISON_Y_TAB_H */
