// SelectBillsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SelectBillsDialog.h"
#include "afxdialogex.h"
#include "DataManager.h"
#include "commonMicro.h"

// CSelectBillsDialog dialog

IMPLEMENT_DYNAMIC(CSelectBillsDialog, CDialogEx)

CSelectBillsDialog::CSelectBillsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectBillsDialog::IDD, pParent)
{

}

CSelectBillsDialog::~CSelectBillsDialog()
{
}

void CSelectBillsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BILL, m_billsListbox);
}


BEGIN_MESSAGE_MAP(CSelectBillsDialog, CDialogEx)
END_MESSAGE_MAP()


// CSelectBillsDialog message handlers


BOOL CSelectBillsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	std::vector<CString> vtr;
	int cnt = 10;
	DataMgrInstanceEx.getlastBills(vtr, cnt);
	for(int n = 0; n < vtr.size(); ++n)
	{
		if (vtr[n].IsEmpty())
			break;
		m_billsListbox.AddString(vtr[n]);
	}
	if (vtr.size() > 0)
	{
		m_billsListbox.SetCapture();
		m_billsListbox.SetCurSel(0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CSelectBillsDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		int sel = m_billsListbox.GetCurSel();
		if(sel >= 0)
			m_billsListbox.GetText(sel, m_selectBill);
		//return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && pMsg->wParam)
	{
		//::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		::PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), UM_HOOK_KEYBOARD_SHOW_HIDE, ctrl_key_cov | VK_F4, 12);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
