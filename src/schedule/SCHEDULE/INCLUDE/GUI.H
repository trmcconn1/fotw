#include "edit.h"
#include <constrea.h>
#include <iomanip.h>
#include <fstream.h>
#include <conio.h>

struct MenuItem{
constream *Wdw;
int state;
char text[20];
char ForeGroundColor;
char BackGroundColor;
int BeginRow;
int BeginCol;
int EndRow;
int EndCol;
} ;

struct SUBMENU {
int NoItems;
int CurrentItem;
constream *Wdw;
struct MenuItem Item[MAX_ITEMS];
int BeginRow;
int BeginCol;
int EndRow;
int EndCol;
} ;


struct MENU {
  int CurrentItem;
  constream *Wdw;
  struct MenuItem Item[MAIN_MENU_ITEMS];
  int BeginRow;
  int BeginCol;
  int EndRow;
  int EndCol;
	};

extern int DisplayMainMenu(struct MENU *);
extern int DisplaySubMenu(int,struct SUBMENU *);
extern void DumpCurrentPage();

struct StatusInfo {
char OpenFile[80];
char Message[80];
};

extern struct StatusInfo mystatus; 

extern int DumpToScreen( constream *);
extern  int MYDialogue();

extern void Status(constream *,struct StatusInfo *,int);
extern void myerror(struct StatusInfo *,char *, char *);
extern void FirstScreen(FILE *, constream *);
extern int PrintOut(char *buffer, int setup);

/* Stuff that's from the user during a dialogue */

struct DialogueDataStruct {
char Title[40];
char Prompt[10];
char Input[40];
char msg1[40];
char msg2[40];
};

extern struct DialogueDataStruct DialogueData;
extern constream Main_Window;
extern constream DialogueBox;
extern constream Status_Bar;
extern constream Title_Line;
extern struct SUBMENU SubMenu[MAIN_MENU_ITEMS];
extern struct MENU  Main_Menu;

