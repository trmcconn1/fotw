
/* edit.c: implements all functions which allow the user to change
   the Main Buffer (core image of file being edited.) Delete, Backspace,
   Cut, Paste, and Insert.
*/

#define __CSOURCE__

#include "edit.h"
#include <stdlib.h>
#include <string.h>

/* The following  functions are defined in gui.cpp: */
/* Don't forget that any function defined in the C++ code must also
   be declared extern "C" there in order to prevent name mangling. */

extern void RefreshLine(char *);
extern void Reset(int);
extern int LogicalX(char *line,int offset);


/* These add and delete single chars from a char array. */
void Slash(char *ptr, int offset);
void Stash(char *ptr, int offset, char new);

/* Assembly code in scroll.asm: */
extern void far scroll(char,char,char);

/* Paste: inserts the source text into MainBuffer just after the
   offset passed . Adjusts buf_end and LinePositions array. One should
   do a DumpCurrentPage after this, or the screen will no longer correspond
   with reality.  Returns number of characters successfully pasted. */

int Paste(const char *source, int offset)
{
	unsigned int n,i,j;

	n = (unsigned) strlen(source);
	if( buf_end + n > MAXBYTES - 1)
		n = MAXBYTES - 1 - buf_end;
	// n is number of bytes to insert

       // Copy end of MainBuffer to itself, leaving gap of size n
       for(i=buf_end;i>offset;i--)
		MainBuffer[i+n] = MainBuffer[i];

       // Copy new text into place
       for(i=0;i<n;i++)
		MainBuffer[offset+i+1]=*(source+i);

      // Update parameters of MainBuffer
      buf_end += n;
      MainBuffer[buf_end] = '\0';
      j = (i = 0);
      LinePositions[i] = j;
      while(bufgets(LineBuf,LINELENGTH,MainBuffer,j,buf_end)!= NULL){
		j += strlen(LineBuf);
		LinePositions[++i] = j;
      }
      TotalLines = i;
      return n;
}

/* PasteAhead: Same as Paste, only pastes in front of offset */
int PasteAhead(const char *source, int offset){

	unsigned int n,i,j;

	if(offset != 0) return Paste(source,offset-1);
	else {  /* Unfortunately, this part must be done from scratch. */

		n = (unsigned) strlen(source);
		if( buf_end + n > MAXBYTES - 1)
			n = MAXBYTES - 1 - buf_end;
		// n is number of bytes to insert

		// Copy end of MainBuffer to itself, leaving gap of size n
		for(i=buf_end;i>0;i--)
			MainBuffer[i+n] = MainBuffer[i];
		MainBuffer[n] = MainBuffer[0];
		// Copy new text into place
		for(i=0;i<n;i++)
		 MainBuffer[offset+i]=*(source+i);

		// Update parameters of MainBuffer
		buf_end += n;
		MainBuffer[buf_end] = '\0';
		j = (i = 0);
		LinePositions[i] = j;
		while(bufgets(LineBuf,LINELENGTH,MainBuffer,j,buf_end)!= NULL){
			j += strlen(LineBuf);
			LinePositions[++i] = j;
		}
		TotalLines = i;
		return n;
	}
}

/* Cut: Removes the n characters beginning at offset and places them in dest.
	Updates buf_end and LinePositions. Returns number of characters cut.
	Does not check that there is room in dest! buf_pos is left pointing
	at first char after those removed, or at EOF.
*/

int Cut(char *dest, int offset,  int n)
{
	unsigned int i,j;

	if(offset + n >= buf_end)
		n =  buf_end - offset ;

	// n is number of bytes to cut
	strncpy(dest,MainBuffer+offset,n);
	*(dest + n) = '\0';

	// remove the n characters starting at index buf_pos 
       //  from MainBuffer
       for(i=offset;i<= buf_end-n;i++)
		MainBuffer[i]=MainBuffer[i+n];

      // Update Main Buffer Parameters
       buf_end -= n;
      j = i = 0;
      LinePositions[i] = j;
      while(bufgets(LineBuf,LINELENGTH,MainBuffer,j,buf_end)!= NULL){
		j += strlen(LineBuf);
		LinePositions[++i] = j;
      }
      TotalLines = i;
      return n;

}


/* Below are implementations of edit-key action functions. The basic idea
is only to alter a small part of the MainBuffer at a time and refresh just one
screen line at a time. When an edit action key is first pressed, the
current line of the file is pulled into a buffer local to this file. Here
is its definition:
*/


char CutBuffer[2*LINELENGTH + 1];
int cutbuf_pos;

/* As long as the user continues the current type of edit action (for example
a sequence of backspaces,) the Cut buffer is udated and the current line
is refreshed. Throughout the edit, buf_pos always points to the character
just beyond the part that was cut.

We need a flag to indicate the current state: */

int AlterFlag = 0; /* The main key processing loop needs to know this   */

/* When any other edit action is begun, or any cursor movement key is
pressed, the CutBuffer is pasted back into the file (MainBuffer)at the
position from which it was cut, all global variables are updated and the
full screen is redrawn. (The same occurs if any key would take us to a
different line.) Then, the new line is cut. */

/* Implementation of BackSpace Key action. Returns 0 if operation
   was successful, 1 if not. */

int BackSpace(void){

	int n;

	if((AlterFlag==0) && (buf_pos == 0))return 1;      /* At beginning of file?    */
	if(AlterFlag && (buf_pos == 0)&&(cutbuf_pos==0))return 1;
   
	/* If we are starting an edit or deleting past the beginning of
	    the cutbuffer during an edit, we need to (possibly) paste
	    what's left of cutbuffer, and cut a new line into it.
	*/
	if((AlterFlag==0)||((AlterFlag==1) && (cutbuf_pos == 0))){
		Slash(MainBuffer,--buf_pos);
		buf_end--;
		if(AlterFlag){   /* were at beginning of a line */
			buf_pos--;
			Paste(CutBuffer,buf_pos);
			buf_pos++;
		}

		Reset(buf_pos);          /* reset buffer parameters and
						redraw screen */
		/* Cut current line */
		/* n is length of current line */
		cutbuf_pos = buf_pos -
				      LinePositions[lineno];
		buf_pos = LinePositions[lineno]; /* ends up just after
						    cut line */
		n = ( lineno < TotalLines - 1 ? LinePositions[lineno+1] -
				    buf_pos : buf_end - buf_pos );
		Cut(CutBuffer,buf_pos,n);
	}
	else  {
		Slash(CutBuffer,--cutbuf_pos);
		X = LogicalX(CutBuffer,cutbuf_pos);
		RefreshLine(CutBuffer);
	}
	AlterFlag = 1;
	return 0;
}


/* Implementation of Delete Key action. Returns 0 if operation
   was successful, 1 if not. */

int Delete(void){

	int n;

	if((AlterFlag == 0) && (buf_pos == buf_end))return 1;      /* At end of file?    */
   
	if(AlterFlag==0){
		Slash(MainBuffer,buf_pos);
		buf_end--;
		Reset(buf_pos);          /* reset buffer parameters and
						redraw screen */
		/* Cut current line */
		/* n is length of current line */
		cutbuf_pos = buf_pos -
				      LinePositions[lineno];
		buf_pos = LinePositions[lineno]; /* ends up just after
						    cut line */
		n = ( lineno < TotalLines - 1 ? LinePositions[lineno+1] -
				    buf_pos : buf_end - buf_pos );
		Cut(CutBuffer,buf_pos,n);
	}
	else  {
		Slash(CutBuffer,cutbuf_pos);
		if(cutbuf_pos == strlen(CutBuffer)){
			if(buf_pos == buf_end) return 1;
			PasteAhead(CutBuffer,buf_pos);
			Reset(buf_pos);
			buf_pos = LinePositions[lineno]; /* ends up just after
						    cut line */
			n = ( lineno < TotalLines - 1 ? LinePositions[lineno+1] -
				    buf_pos : buf_end - buf_pos );
			Cut(CutBuffer,buf_pos,n);
		}
		X = LogicalX(CutBuffer,cutbuf_pos);
		RefreshLine(CutBuffer);
	}
	AlterFlag = 1;
	return 0;
}

/* Implementation of Insert Key action. Characters are added to the small
    local cut buffer until the cursor moves to a new line. Then the
    cut buffer is pasted back into the the Main Buffer, the cursor is
    advanced to the beginning of the new line, and the contents of that
    line are cut. Everything below the original line is scrolled to make
    room. A flag is used to make sure the scroll is only done once. */

void Insert(char inschr){

	int n,m;
	static int scrolled = 0;

	if((AlterFlag == 0)|| (inschr == RETURN)|| (X==LINELENGTH)){ 
		if(AlterFlag == 0){     /* First-time entry */
		  scrolled = 0;
		  if(inschr == RETURN)
			Stash(MainBuffer,buf_pos++,'\n');
		  else
			Stash(MainBuffer,buf_pos++,inschr);
		  buf_end++;
		}
		else 
		  if(inschr == RETURN)
		    Stash(CutBuffer,cutbuf_pos++,'\n');
		  else Stash(CutBuffer,cutbuf_pos++,inschr);

		/* If we are continuing an edit and the cursor will
		move onto a new line, we need to paste the cut buffer and
		redraw the screen before proceeding. There are two ways
		this can happen: by entering a return, or by filling up
		the line. */

		if(AlterFlag)PasteAhead(CutBuffer,buf_pos);
		Reset(buf_pos);          /* reset buffer parameters and
						redraw screen */
 
		if(AlterFlag == 0){
			cutbuf_pos = buf_pos -
				      LinePositions[lineno];
			buf_pos = LinePositions[lineno];
		}
		else  {  /* an extra line has been added */
			cutbuf_pos = 0;
			Y++;
	       /* Note that, after the paste above, bufpos now points at
		  the beginning of the line on which the insert begin. 
		  Since that line has now spilled over onto a new line,
		  incrementing lineno will point to the beginning of that
		 new line, which is going to be the next one cut. */
			buf_pos = LinePositions[++lineno];
		}

		n = ( lineno < TotalLines - 1 ? LinePositions[lineno+1] -
				    buf_pos : buf_end - buf_pos );
		Cut(CutBuffer,buf_pos,n);
	}
	else  {  /* Continuing to insert on the same line */
		Stash(CutBuffer,cutbuf_pos++,inschr);
		X = LogicalX(CutBuffer,cutbuf_pos);

		/* Scroll if necessary to make room for a new line */
		if(LogicalX(CutBuffer,strlen(CutBuffer))>LINELENGTH)
			if(scrolled);   /* done already */
                        else {          
                          if(Y==SCREENLINES)
                           /* at bottom of screen: wrap will push us up
                           one line */
                           scroll(SCROLL_UP,1,Y--);
                           else {
			/* Add 2 to Y because of title line and menu */
			   scroll(SCROLL_DOWN,1,Y+2);
			   scrolled = 1;
                         }
			}
		RefreshLine(CutBuffer);
	}
	AlterFlag = 1;
}


/* Slash: Removes a character from the given char array at the given
   index. Needs to be used sparingly for large arrays.
*/

void Slash(char *ptr, int offset)
{

	char c;
	int i=0;

	while((c = *(ptr + offset + i + 1)) != '\0')
		*(ptr + offset + i++) = c;
	*(ptr + offset + i) = '\0';
	return;
}

/* Stash: Adds new character to the given array at the given offset. The
   following characters are pushed 1 unit to the right to make room.
*/

void Stash(char *ptr, int offset, char new)
{

	int i,n;

	n = strlen(ptr);
	for(i=n;i>=offset;i--)
		*(ptr + i + 1) = *(ptr + i);
	*(ptr + offset ) = new;
	return;
}

