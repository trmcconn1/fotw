// flashcardDoc.cpp : implementation of the CflashcardDoc class
//

#include "stdafx.h"
#include "flashcard.h"

#include "flashcardDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CflashcardDoc

IMPLEMENT_DYNCREATE(CflashcardDoc, CDocument)

BEGIN_MESSAGE_MAP(CflashcardDoc, CDocument)
END_MESSAGE_MAP()

// CflashcardDoc construction/destruction

CflashcardDoc::CflashcardDoc()
{
	// TODO: add one-time construction code here

}

CflashcardDoc::~CflashcardDoc()
{
}

BOOL CflashcardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CflashcardDoc serialization

#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
void CflashcardDoc::Serialize(CArchive& ar)
{
	(ar);
}
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT


// CflashcardDoc diagnostics

#ifdef _DEBUG
void CflashcardDoc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif //_DEBUG


// CflashcardDoc commands

