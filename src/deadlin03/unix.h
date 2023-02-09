/* Header file for unix.c: system dependent routines */

#ifdef _GNU_SOURCE
#define CIFLAG (ICRNL | IXON | IXANY)
#define CLFLAG (ECHO | ECHOCTL)
#else
#define CIFLAG (ICRNL | IXON )
#define CLFLAG ECHO
#endif

#ifdef _BSD
#define PRINT_COMMAND "lpr"
#else
#define PRINT_COMMAND "lp"
#endif

#ifdef _MINIX
#undef CLFLAG
#define CLFLAG ECHO
#endif

#ifdef _LINUX
#undef PRINT_COMMAND
#define PRINT_COMMAND "lpr"
#endif
