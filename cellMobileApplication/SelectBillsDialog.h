#pragma once
#include "afxwin.h"
#include "resource.h"


// CSelectBillsDialog dialog

class CSelectBillsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectBillsDialog)

public:
	CSelectBillsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectBillsDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECT_BILLS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_billsListbox;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
