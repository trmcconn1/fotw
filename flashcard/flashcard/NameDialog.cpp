// NameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Score.h"
#include "NameDialog.h"

extern wchar_t wRunnerNameBuf[];

// NameDialog dialog

IMPLEMENT_DYNAMIC(NameDialog, CDialog)

NameDialog::NameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NameDialog::IDD, pParent)
{

}

NameDialog::~NameDialog()
{
}

void NameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NameDialog, CDialog)
	ON_BN_CLICKED(IDOK,OnOKClicked)
END_MESSAGE_MAP()


// NameDialog message handlers
void NameDialog::OnOKClicked(){
	NameDialog::GetDlgItemText(IDC_EDIT1,(LPTSTR)&wRunnerNameBuf,128);
	NameDialog::EndDialog(1);
}