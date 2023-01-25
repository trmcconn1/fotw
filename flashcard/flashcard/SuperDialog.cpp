// SuperDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Score.h"
#include "SuperDialog.h"

extern wchar_t wEventNameBuf[];

// NameDialog dialog

IMPLEMENT_DYNAMIC(SuperDialog, CDialog)

SuperDialog::SuperDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SuperDialog::IDD, pParent)
{

}

SuperDialog::~SuperDialog()
{
}

void SuperDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SuperDialog, CDialog)
	ON_BN_CLICKED(IDOK,OnOKClicked)
END_MESSAGE_MAP()


// NameDialog message handlers
void SuperDialog::OnOKClicked(){
	SuperDialog::GetDlgItemText(IDC_EDIT1,(LPTSTR)&wEventNameBuf,128);
	SuperDialog::EndDialog(1);
}