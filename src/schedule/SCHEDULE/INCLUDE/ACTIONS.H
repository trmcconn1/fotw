/* actions.h: declarations of menu actions defined in actions.cpp
     and things specific to this implementation of the gui */

#include <stdio.h>

#define TITLE "             Mathematics Department Scheduling Program"
#define __MENU__
#define INFO "doc\\info.txt"
#define MAN  "doc\\schedule.1"

/* Menu Titles */
#define MENU0 "| FILE |"
#define MENU00 "OPEN    "
#define MENU01 "SAVE    "
#define MENU02 "SAVE AS "
#define MENU03 "PRINT   "
#define MENU04 "QUIT    "
#define MENU1  "DISPLAY|"
#define MENU10 "SCHEDULE"
#define MENU11 "AUDIT   "
#define MENU12 "RMCHART "
#define MENU13 "PEOPLE  "
#define MENU14 "FINALS  "
#define MENU2  " PREFS |"
#define MENU20 "SEM     "
#define MENU21 "SET     "
#define MENU22 "DIR     "
#define MENU23 "TBA     "
#define MENU24 "xxxxxxxx"
#define MENU3  "ACTIONS|"
#define MENU30 "SHELL   "
#define MENU31 "NEWSMSTR"
#define MENU32 "LOAD    "
#define MENU33 "UNLOAD  "
#define MENU34 "xxxxxxxx"
#define MENU4  " HELP  |"
#define MENU40 "INFO    "
#define MENU41 "MANUAL  "
#define MENU42 "xxxxxxxx"
#define MENU43 "xxxxxxxx"
#define MENU44 "xxxxxxxx"

extern void initialize(FILE **, FILE **);
extern void UpdateMessage(char *);

/* Menu actions */
extern void menu10(FILE *);
extern void menu11(FILE *);
extern void menu12(FILE **);
extern void menu13(FILE *);
extern void menu14(FILE *);
extern void menu20();
extern void menu21(FILE **);
extern void menu22(void);
extern void menu23(void);
extern void menu24(void);

extern void menu30(void);
extern void menu31(void);
extern void menu32(FILE *, FILE *);
extern void menu33(void);
extern void menu34(void);
extern void menu42(void);
extern void menu43(void);
extern void menu44(void);

 extern "C" {                                                                 
 #include "shell.h"                                                            
 }
extern "C" int load(FILE *, FILE *, FILE *, FILE *);
extern "C" int take(FILE *, FILE *, FILE *, FILE *);                          
extern "C" int schedule(FILE *, FILE *, FILE *, FILE *);                      
extern "C" int audit(FILE *, FILE *, FILE *, FILE *);                         
extern "C" int shell(FILE *, FILE *, FILE *, FILE *,char *errmsg, void *data);
extern "C" int cd(FILE *, FILE *, FILE *, FILE *);                            
extern "C" int dpeople(FILE *,FILE *,FILE *, FILE *);                         
extern "C" int dfinals(FILE *,FILE *,FILE *, FILE *);                         
extern "C" int Dialogue(FILE *, FILE *, FILE *,FILE *,                        
char *title, void *data,                                                                                                                                               
                int ( *DialogueHandler ) ( FILE *, FILE *, FILE *,FILE *,
                        char *errmsg, void *data) );
extern "C" char *GetShell(char *);

extern struct TokenType *Tokens[];
extern "C" char TempFilePath[];

