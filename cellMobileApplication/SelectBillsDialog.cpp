// SelectBillsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SelectBillsDialog.h"
#include "afxdialogex.h"


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
	m_billsListbox.AddString(L"1");
	m_billsListbox.AddString(L"2");
	m_billsListbox.AddString(L"3");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CSelectBillsDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		CString   strText;
		m_billsListbox.GetText(m_billsListbox.GetCurSel(), strText);
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
