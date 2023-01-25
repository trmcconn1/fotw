
VS generated boilerplate is left in below.

Flashcard is my foreign language study tool. It presents the user with a sequence of foreign words or of english words
and he/she is supposed to supply the corresponding english/foreign word or phrase. You don't actually supply them other than
in your head. Click the button to fill in the other window and you will see if you got it right or wrong. (More or less :-)
By default, items are presented in the order they appear in the cards database file, but they can also be presented in random
order by checking the random box at the bottom. You should mark yourself right or wrong because the database tracks how
many times an item has been gotten wrong and items that have been gotten wrong a lot are more likely to be presented to the
user in the random mode of operation. The two other check boxes at the bottom are mutually exclusive and control whether it is
the foreign word or the English word that gets presented. The check boxes can be changed whenever you want while the program is
running. 

Default values for the various choices are controlled by the settings file named
My Documents\Flash Card\flash1.txt

Here is a typical flash1.txt (one setting to a line with a single space separating the setting name (first field) from the
rest of the line (the setting value):
CardFile \My Documents\Flash Card\latin.txt
NativeLanguage English
ForeignLanguage Latin
SelectionMode Consecutive
Direction ForeignToNative

(SelectionMode could also be Random and Direction could be NativeToForeign. The other settings have no restrictions except that the
CardFile must point to the actual location of a database file.) 

Currently the program supports up to 3 configuration files (flash1.txt, flash2.text, and flash3.txt) and asks the user which
one to use at startup. Thus you can have 1, 2, or 3 different languages to choose from, but you could make some or all of these
files the same if you want to. All 3 must exist. 

Both settings file and database file can be edited in notepad (or other text editor) but the format is fussy. The settings
files should be encoded in UTF-8 ("plain text") but the database file needs to be Unicode (UTF-16 LE) in order to allow for
the diacritical marks of various languages. (It's ok if the format is BOM, i.e., the file may contain a "magic number" Bit Order
Marker at the top if your text editor puts it there.) 

Each line of a database has four fields separated by the / character. For example, a line from latin.txt might read
/To love/amo, amare, amavi, amatus-a-um/5/7/

The last two fields record the number of rights and wrongs respectively. When you quit the program it will ask if you want to
update these fields. It is a good idea to say yes, but you can edit them yourself if you want to "cheat". 

When you start the program it reads the settings file and then the database and presents you with your first card. What happens
after that is up to you.

You can have as many database files as you want to create and select the ones you want by editing the CardFile entries.

================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : flashcard Project Overview
===============================================================================

The application wizard has created this flashcard application for 
you.  This application not only demonstrates the basics of using the Microsoft 
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your flashcard application.

flashcard.vcproj
    This is the main project file for VC++ projects generated using an application wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    application wizard.

flashcard.h
    This is the main header file for the application.  It includes other
    project specific headers and declares the CflashcardApp application class.

flashcard.cpp
    This is the main application source file that contains the application
    class CflashcardApp.


flashcardppc.rc
    This is the project's main resource file listing of all of the Microsoft Windows
    resources that the project uses when compiling for the Pocket PC platform, or a
    platform that supports the same user interface model.  It includes the icons,
    bitmaps, and cursors that are stored in the RES subdirectory.  This file can be 
    directly edited in Microsoft Visual C++. Your project resources are in 
    1033. When the .rc file is persisted, the defines in the data section
    are persisted as the hexadecimal version of the numeric value they are defined to
    rather than the friendly name of the define.

res\flashcardppc.rc2
    This file contains resources that are not edited by Microsoft 
    Visual C++. You should place all resources not editable by
    the resource editor in this file.


res\flashcard.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file.


/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Cmdbar.bmp
    This bitmap file is used to create tiled images for the command bar.
    The initial command bar is constructed in the CMainFrame class. 
    Edit this cmdbar bitmap along with the array in MainFrm.cpp
    to add more command bar buttons.

/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

flashcardDoc.h, flashcardDoc.cpp - the document
    These files contain your CflashcardDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CflashcardDoc::Serialize).

flashcardView.h, flashcardView.cpp - the view of the document
    These files contain your CflashcardView class.
    CflashcardView objects are used to view CflashcardDoc objects.




/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named flashcard.pch and a precompiled types file named StdAfx.obj.

Resourceppc.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is in a 
language other than the operating system's current language, you will need 
to copy the corresponding localized resources MFC80XXX.DLL to your application
directory ("XXX" stands for the language abbreviation.  For example,
MFC80DEU.DLL contains resources translated to German.)  If you don't do this,
some of the UI elements of your application will remain in the language of the
operating system.

/////////////////////////////////////////////////////////////////////////////
