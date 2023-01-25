// flashcardView.h : interface of the CflashcardView class
//


#pragma once

class CflashcardView : public CView
{
protected: // create from serialization only
	CflashcardView();
	DECLARE_DYNCREATE(CflashcardView)

// Attributes
public:
	CflashcardDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// Implementation
public:
	virtual ~CflashcardView();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in flashcardView.cpp
inline CflashcardDoc* CflashcardView::GetDocument() const
   { return reinterpret_cast<CflashcardDoc*>(m_pDocument); }
#endif

