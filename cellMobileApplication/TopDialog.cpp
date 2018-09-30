// TopDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TopDialog.h"
#include "afxdialogex.h"


// TopDialog dialog

IMPLEMENT_DYNAMIC(TopDialog, CDialogEx)

TopDialog::TopDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(TopDialog::IDD, pParent)
{

}

TopDialog::~TopDialog()
{
}

void TopDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TopDialog, CDialogEx)
END_MESSAGE_MAP()


// TopDialog message handlers
void TopDialog::setTipsMessage(const char* msg)
{
	m_msg = msg;

}

BOOL TopDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CWnd* w = GetDlgItem(IDC_STATIC_MSG);
	w->SetWindowText(m_msg);

	return TRUE;
}