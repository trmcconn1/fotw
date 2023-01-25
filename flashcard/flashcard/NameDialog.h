#pragma once


// NameDialog dialog

class NameDialog : public CDialog
{
	DECLARE_DYNAMIC(NameDialog)

public:
	NameDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~NameDialog();
	virtual void OnOKClicked();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
