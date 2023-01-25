// flashcardDoc.h : interface of the CflashcardDoc class
//


#pragma once

class CflashcardDoc : public CDocument
{
protected: // create from serialization only
	CflashcardDoc();
	DECLARE_DYNCREATE(CflashcardDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
	virtual void Serialize(CArchive& ar);
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT

// Implementation
public:
	virtual ~CflashcardDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


