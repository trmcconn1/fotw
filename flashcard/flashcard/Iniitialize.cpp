/* Initialization routines for flash card program */

/* The following Data file must be declared in flash.txt

	CardFile:   Definitions for each flash card */

/**********************************************************
		HEADER
*********************************************************/
#define __CPLUSPLUS__

#include "Error.h"
#include <assert.h>
#include <ctype.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "cards.h"
#include <wchar.h>
#include <tchar.h>
#include "mtwist.h"

extern "C" void initializecards();
extern int currentcardnumber;
extern "C" struct cardstruct mycards[];
extern "C" int ncards;
extern "C" int wrongtotal;


extern int ParseRcFile(FILE *,char *);
extern "C" void krqsort( void *v[],int , int ,
 int (*)(void *,void *));    // Generic sort routine
extern "C" void *binsearch(void *,void *v[],
int, int (*) (void *, void *));  // Generic binary search

int load();            // loads card database

#ifdef UNDER_CE
SYSTEMTIME now;
#else
time_t now;
#endif

char buffer[1024];
char buffer2[1024];          //  buffers

wint_t magicnumber = 0;
int loadsuccess = 0;

char TodaysDate[64];
char TimeStamp[64];

// Stuff used by ParseCardFile

struct CARDINFO {
	wchar_t Foreign[64];       /* Word or phrase in foreign language */
	wchar_t English[64];       /* Corresponding word or phrase in English */
	int correct;              /* Number of times this word has been gotten
							  right */
	int incorrect;
} cinfo;
extern int ParseCardFile(FILE *,  struct CARDINFO *);       // Parses results file


// Stuff read in from flash.txt:
                                  
char CardFile[256];           // path to card file 

// Used in the load module
int line;


// Files and Streams
FILE *cardfile;     /* fd for card file */                                       
FILE *rcfile;

/************************************************************
		 END OF HEADER
************************************************************/
int myInitialize(FILE *rcfile)
{
	
	  int i;

  if(ParseRcFile( rcfile,CardFile) != 0){
		ErrorExit(L"Initialize: error in settings file");
			return 1;
		}
		fclose(rcfile);
	


#ifdef UNDER_CE
	   GetLocalTime(&now);
	   sprintf(TodaysDate,"%d/%d/%d",now.wMonth,now.wDay,now.wYear);
	   sprintf(TimeStamp,"%d:%d:%d",now.wHour,now.wMinute,now.wSecond);
#else                          // CE does not support tm functions
	   now = time(NULL);
	   strftime(TodaysDate,40,"%d %B %Y",localtime(&now));
#endif
	
	  // seed the random number  generator
	   //i = (int)now.wMilliseconds;
	   //i = (int)GetTickCount();
	   //srand(i);
	   mt_seed();

	   /* Load card data base from flat file */
	   load();
	 
	   /* Initialize cards */

	   initializecards();
	   wrongtotal = 0;
	   for(i=0;i<ncards;i++)
		   wrongtotal += mycards[i].nincorrect;
		  
// LOAD DATA


	  return 0; /* Not needed, but prevents compiler warning */
}
/****************************************************************************
	 LOAD DATA SECTION: parse and load data on flash cards
***************************************************************************/

int load()
{
	int i;
	
	/* open results file: binary, since it is unicode, not mb */
	/* Reads from binary files are as wide-characters and reads
	from text files are multi-byte. We want wide-characters.
	2022: Getting this to work on both the simulator and 
	the device was really a problem. What now seems to work
	is that the card files are UTF-16 LE BOM encoded.  Notepad++
	will save files in this encoding but the latest notepad seems
	to save files in UTF-8. Don't forget that the source code 
	encoding matters as well since it determines how constant 
	strings are encoded. VS-2008 only allows two choices in
	the general settings - "unicode" and "multi-byte. You
	should choose unicode. */
	
	if((cardfile = fopen(CardFile,"rb"))==NULL){ 
		(L"load:Problem opening card file");
	}

	/* Some text editors, e.g., notepad, put a magic number at
	the beginning of a Unicode file called the Bit Order Marker. If 
	it is there, we need to skip over it */
	
	magicnumber = fgetwc(cardfile);
	if(iswalpha(magicnumber)){
		
		ungetwc(magicnumber,cardfile); // put it back
		magicnumber = 0; // we restore this if file is updated
	}

	line = 0;
	ncards = 0;
	while((i=ParseCardFile(cardfile,&cinfo))!= -1){// -1 at EOF

		if(ncards==MAX_CARDS){
			ErrorExit(L"Maximum number of cards reached");
			break;
		}

		if(i==0){// 0 return if card is good
			wcscpy(mycards[ncards].english,cinfo.English);
			wcscpy(mycards[ncards].foreign,cinfo.Foreign);
			mycards[ncards].ncorrect = cinfo.correct;
			mycards[ncards].nincorrect = cinfo.incorrect;
			line++;
			ncards++;
		}

		

	}
	loadsuccess = 1;
                      
/*************************************************************************
       END DATA LOAD SECTION
*************************************************************************/
return 0;
}

int myFinalize(){

	int j;
	wchar_t ExistingFileName[256];
	wchar_t NewFileName[256];

	mbstowcs(ExistingFileName,CardFile,256);
	wcscpy(NewFileName,ExistingFileName);
	wcstok(NewFileName,L".");
	wcscat(NewFileName,L".bak.txt");	

	// make a backup copy of the card database file
	CopyFile(ExistingFileName,NewFileName,FALSE);

	if(loadsuccess){

	// Write changes to data file

	FILE *cardfile = fopen(CardFile,"wb");
	
		if(magicnumber)fputwc(magicnumber,cardfile);
		for(j=0;j<ncards;j++)
			fwprintf(cardfile,L"%s/%s/%d/%d/\r\n",mycards[j].english,
				mycards[j].foreign,mycards[j].ncorrect,mycards[j].nincorrect);
	}
	fclose(cardfile);
	return 0;

}