// flashcardView.cpp : implementation of the CflashcardView class
//

#include "stdafx.h"
#include "flashcard.h"

#include "flashcardDoc.h"
#include "flashcardView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CflashcardView

IMPLEMENT_DYNCREATE(CflashcardView, CView)

BEGIN_MESSAGE_MAP(CflashcardView, CView)
END_MESSAGE_MAP()

// CflashcardView construction/destruction

CflashcardView::CflashcardView()
{
	// TODO: add construction code here

}

CflashcardView::~CflashcardView()
{
}

BOOL CflashcardView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


// CflashcardView drawing
void CflashcardView::OnDraw(CDC* /*pDC*/)
{
	CflashcardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}



// CflashcardView diagnostics

#ifdef _DEBUG
void CflashcardView::AssertValid() const
{
	CView::AssertValid();
}

CflashcardDoc* CflashcardView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CflashcardDoc)));
	return (CflashcardDoc*)m_pDocument;
}
#endif //_DEBUG


// CflashcardView message handlers
