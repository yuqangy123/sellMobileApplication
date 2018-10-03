// RefundOrderDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuRefundOrderDialog.h"
#include "afxdialogex.h"
#include "RefundResultDialog.h"

// CMenuRefundOrderDialog 对话框

IMPLEMENT_DYNAMIC(CMenuRefundOrderDialog, CDialogEx)

CMenuRefundOrderDialog::CMenuRefundOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_REFUND_ORDER, pParent)
{

}

CMenuRefundOrderDialog::~CMenuRefundOrderDialog()
{
}

void CMenuRefundOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMenuRefundOrderDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SURE, &CMenuRefundOrderDialog::OnBnClickedButtonSure)
END_MESSAGE_MAP()


// CMenuRefundOrderDialog 消息处理程序

BOOL CMenuRefundOrderDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽ESC关闭窗体/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return TRUE;
	}
	//屏蔽回车关闭窗体,但会导致回车在窗体上失效.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMenuRefundOrderDialog::OnBnClickedButtonSure()
{
	// TODO: 在此添加控件通知处理程序代码
	CRefundResultDialog dlg;
	dlg.DoModal();

}
