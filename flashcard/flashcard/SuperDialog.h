#pragma once


// SuperDialog dialog

class SuperDialog : public CDialog
{
	DECLARE_DYNAMIC(SuperDialog)

public:
	SuperDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~SuperDialog();
	virtual void OnOKClicked();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
