// FlashDlg.cpp : implementation  file
//

#define __CPLUSPLUS__
#include <windows.h>
#include "stdafx.h"
#include "flashcard.h"
#include "FlashDlg.h"
#include "Error.h"
#include "cards.h"

extern "C" int newcardnumber(int current);
extern "C" struct cardstruct mycards[];
extern "C" int wrongtotal;
extern "C" int ncards;
extern "C" int coinflip();

int currentcardnumber;
extern int myFinalize();

extern wchar_t NativeLanguage[];
extern wchar_t ForeignLanguage[];


int direction = NATIVETOFOREIGN;
extern "C" int selectionmode;

extern int myInitialize(FILE *);
extern int myFinalize();


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

wchar_t *Edit1TextBuffer;
wchar_t *Edit2TextBuffer;
wchar_t Edit3TextBuffer[256];


// CScoreDlg dialog

CFlashDlg::CFlashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlashDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFlashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFlashDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CFlashDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFlashDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFlashDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CFlashDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK1, &CFlashDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CFlashDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CFlashDlg::OnBnClickedCheck3)
	ON_LBN_SELCHANGE(IDC_LIST2, &CFlashDlg::OnLbnSelchangeList2)
END_MESSAGE_MAP()


// CScoreDlg message handlers
BOOL CFlashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dia0log
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(MessageBox(L"Use Card Set 1?",
		L"Init Dialog Msg",MB_YESNO)==IDYES){
			FILE *rcfile = fopen("\\My Documents\\Flash Card\\flash1.txt","r");
			if(rcfile == NULL)ErrorExit(L"Cannot open \\My Documents\\Flash Card\\flash1.txt");
			myInitialize(rcfile);
	}
	else {
		if(MessageBox(L"Card Set 2?",
			L"Init Dialog Msg",MB_YESNO)==IDYES){
				FILE *rcfile = fopen("\\My Documents\\Flash Card\\flash2.txt","r");
				if(rcfile == NULL)ErrorExit(L"Cannot open \\My Documents\\Flash Card\\flash2.txt");
				myInitialize(rcfile);
		}
		else {
			FILE *rcfile = fopen("\\My Documents\\Flash Card\\flash3.txt","r");
				if(rcfile == NULL)ErrorExit(L"Cannot open \\My Documents\\Flash Card\\flash3.txt");
				myInitialize(rcfile);
		}
	}

	// Initially mark the checkbox indicating selection of cards
	// mode (either RANDOM or CONSECUTIVE). The default is RANDOM
	// but this can be changed in the initialization file

	CButton *mySelectModeBox = (CButton *) GetDlgItem(IDC_CHECK1);
	if(selectionmode == RANDOM)mySelectModeBox->SetCheck(BST_CHECKED);

	// Initially mark the checkboxes indication direction of 
	// Translation 

	mySelectModeBox = (CButton *) GetDlgItem(IDC_CHECK2);
	if(direction == NATIVETOFOREIGN)
		mySelectModeBox->SetCheck(BST_CHECKED);

	mySelectModeBox = (CButton *) GetDlgItem(IDC_CHECK3);
	if(direction == FOREIGNTONATIVE)
		mySelectModeBox->SetCheck(BST_CHECKED);

	// display the first flash card
	
	currentcardnumber = newcardnumber(0);
	if(direction == NATIVETOFOREIGN){
		Edit1TextBuffer = mycards[currentcardnumber].english;
		SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);
	}
	else {
		Edit1TextBuffer = mycards[currentcardnumber].foreign;
		SetDlgItemText(IDC_EDIT2,Edit1TextBuffer);
	}
	wsprintf(Edit3TextBuffer,L"%s and %s %d cards",NativeLanguage,ForeignLanguage,ncards);
	SetDlgItemText(IDC_EDIT3,Edit3TextBuffer);
	
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CFlashDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	DRA::RelayoutDialog(
		AfxGetInstanceHandle(), 
		this->m_hWnd, 
		DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_POCKETPC_WIDE) : 
			MAKEINTRESOURCE(IDD_POCKETPC_SQUARE));
}
#endif


void CFlashDlg::OnBnClickedButton1()
{
	
	Edit1TextBuffer = mycards[currentcardnumber].english;
	SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);

	
}

void CFlashDlg::OnBnClickedButton4()
{
	
	Edit2TextBuffer = mycards[currentcardnumber].foreign;
	SetDlgItemText(IDC_EDIT2,Edit2TextBuffer);

	
}
void CFlashDlg::OnBnClickedButton2()
{
	
	int i;
	int mydirection;

	/* Mark the current card as correct, blank the definition
	window, select new card. */

	mycards[currentcardnumber].ncorrect++;
	i = newcardnumber(currentcardnumber);
	if(direction==RANDOM)
		mydirection = coinflip();
	else mydirection = direction;
	if(mydirection==NATIVETOFOREIGN){
		Edit2TextBuffer = L"";
		SetDlgItemText(IDC_EDIT2,Edit2TextBuffer);
		Edit1TextBuffer = mycards[i].english;
		SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);
	}
	else {
		Edit1TextBuffer = L"";
		SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);
		Edit2TextBuffer = mycards[i].foreign;
		SetDlgItemText(IDC_EDIT2,Edit2TextBuffer);
	}
	currentcardnumber = i;
}

void CFlashDlg::OnBnClickedButton3()
{ 
	int i;
	int mydirection;

	mycards[currentcardnumber].nincorrect++;
	wrongtotal++;
	i = newcardnumber(currentcardnumber);
	if(direction==RANDOM)
		mydirection = coinflip();
	else mydirection = direction;
	if(mydirection==NATIVETOFOREIGN){
		Edit2TextBuffer = L"";
		SetDlgItemText(IDC_EDIT2,Edit2TextBuffer);
		Edit1TextBuffer = mycards[i].english;
		SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);
	}
	else {
		Edit1TextBuffer = L"";
		SetDlgItemText(IDC_EDIT1,Edit1TextBuffer);
		Edit2TextBuffer = mycards[i].foreign;
		SetDlgItemText(IDC_EDIT2,Edit2TextBuffer);
	}
	currentcardnumber = i;
	
}

void CFlashDlg::OnBnClickedCheck1()
{
	selectionmode = RANDOM;
}

/* These two are linked */
void CFlashDlg::OnBnClickedCheck2()
{
	direction = NATIVETOFOREIGN;
	CButton *mySelectModeBox = (CButton *) CFlashDlg::GetDlgItem(IDC_CHECK3);
		mySelectModeBox->SetCheck(BST_UNCHECKED);

}
void CFlashDlg::OnBnClickedCheck3()
{
	direction = FOREIGNTONATIVE;
	CButton *mySelectModeBox = (CButton *) CFlashDlg::GetDlgItem(IDC_CHECK2);
		mySelectModeBox->SetCheck(BST_UNCHECKED);

}
void CFlashDlg::OnOK()
{
	if(MessageBox(L"Update Card Database File?",
		L"Exit Message",MB_YESNO)==IDYES)myFinalize();

	CDialog::OnOK();
}

void CFlashDlg::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
}
