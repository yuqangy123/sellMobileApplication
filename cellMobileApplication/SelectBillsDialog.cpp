// SelectBillsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SelectBillsDialog.h"
#include "afxdialogex.h"
#include "DataManager.h"
#include "commonMicro.h"
#include <algorithm>

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

bool compFunc(const double& a, const double& b)
{
	return a < b;
}

BOOL CSelectBillsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// TODO:  Add extra initialization here
	std::vector<double> vtr;
	int cnt = 20;
	DataMgrInstanceEx.getlastBills(vtr, cnt);
	sort(vtr.begin(), vtr.end(), compFunc);

	for(int n = 0; n < vtr.size(); ++n)
	{
		if (vtr[n] <= 0.0)
			break;

		CString str;
		long long x = vtr[n];
		str.Format(L"%I64d", x);
		while (str.GetLength() < 12)
			str.Insert(0, L"0");
		
		m_billsListbox.AddString(str);
	}
	if (vtr.size() > 0)
	{
		m_billsListbox.SetCapture();
		m_billsListbox.SetCurSel(vtr.size()-1);
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
		DataMgrInstanceEx.EscKeyTag = 1;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
