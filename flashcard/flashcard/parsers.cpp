/* Parsers.c:  */

#define __CPLUSPLUS__
#include <windows.h>    
#include <string.h>
#include "Error.h"
#include "cards.h"


extern int direction;
extern "C" int selectionmode;

struct CARDINFO {
	wchar_t Foreign[64];       /* Word or phrase in foreign language */
	wchar_t English[64];       /* Corresponding word or phrase in English */
	int correct;              /* Number of times this word has been gotten
							  right */
	int incorrect;
};
#define LINESIZ 80
#define BUFFERSIZ 256
#define DELIMITER "     \n"
#define COMMENT '*'
#define FIELDSEPS " /-  " /* delimiters in a valid date */

extern int line;
extern char CardFile[];
wchar_t NativeLanguage[32]=L"English";
wchar_t ForeignLanguage[32]=L"Foreign";

void ParseDate( char *DateString , struct tm *Date);
int tokenize(char *line,char **tokens); 


int
ParseCardFile(FILE *cardfile, struct CARDINFO *cinfo)
{
        wchar_t buffer[BUFFERSIZ];
		wchar_t *p;
       

		if(!cardfile)return -1;
		if(!cinfo)return -1;
		
		// Get next line from card file 
		if(fgetws(buffer,BUFFERSIZ,cardfile)==NULL)return -1;
		// ErrorNonFatal(buffer);

		// skip lines beginning with carriage return and/or line feed
		if((buffer[0]==L'\r')||(buffer[0]==L'\n'))return 1;
		
		
		// Break line into tokens delimited by / character
	
		p = wcstok(buffer,L"/");
		if(!p){
			wsprintf(buffer,L"Unable to parse line %d of card file",line+1);
			ErrorExit(buffer);
			return 1;
		}
		wcscpy((cinfo->English),p);

		p=wcstok(NULL,L"/");
		if(!p){
			wsprintf(buffer,L"Unable to parse line %d = %s of card file",line+1,p);
			ErrorExit(buffer);
			return 1;
		}
		wcscpy((cinfo->Foreign),p);
		p=wcstok(NULL,L"/");
		if(!p){
			wsprintf(buffer,L"Unable to parse line %d of card file",line+1);
			ErrorExit(buffer);
			return 1;
		}
		cinfo->correct = _wtoi(p);
		p=wcstok(NULL,L"/");

		if(!p){
			wsprintf(buffer,L"Unable to parse line %d of card file",line+1);
			ErrorExit(buffer);
			return 1;
		}
		cinfo->incorrect = _wtoi(p);
		return 0;   
}                  




// Parse the file flash.txt in the current directory 

int
ParseRcFile(FILE *rcfile, char *CardFile)
{
        char LineBuffer[256];
        char buffer[80], buffer2[80];
        int i;
          while(fgets(LineBuffer,256,rcfile)!=NULL){
          if((LineBuffer[0] != COMMENT)&&(
                LineBuffer[0] != '\n')){ // skip comments and blank lines 
          sscanf(LineBuffer,"%s %[^\n]",buffer,buffer2);
                
                if(strcmp(buffer,"CardFile")==0){
                        strcpy(CardFile,buffer2);
                        continue;
                        }
				if(strcmp(buffer,"NativeLanguage")==0){
                        mbstowcs(NativeLanguage,buffer2,32);
                        continue;
                        }
				if(strcmp(buffer,"ForeignLanguage")==0){
                        mbstowcs(ForeignLanguage,buffer2,32);
                        continue;
                        }
				if(strcmp(buffer,"SelectionMode")==0){
                        if(strcmp(buffer2,"Random")==0)
							selectionmode = RANDOM;
						if(strcmp(buffer2,"Consecutive")==0)
							selectionmode = CONSECUTIVE;
                        continue;
                        }
				if(strcmp(buffer,"Direction")==0){
                        if(strcmp(buffer2,"NativeToForeign")==0)
							direction = NATIVETOFOREIGN;
						if(strcmp(buffer2,"ForeignToNative")==0)
							direction = FOREIGNTONATIVE;
						if(strcmp(buffer2,"Random")==0)
							direction = RANDOM;
                        continue;
                        }
                        // Nothing recognizable found
                        return 1;
                       
                    }     // end if: skip comments 
                }           // end while for parsing score.ini
                return 0;
} // end routine
