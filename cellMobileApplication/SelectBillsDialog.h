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
	CString getSelectBill() { return m_selectBill; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CListBox m_billsListbox;
	CFont m_billsListFont;
	CString m_selectBill;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
