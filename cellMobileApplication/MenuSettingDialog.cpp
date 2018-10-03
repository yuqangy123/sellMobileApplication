// MenuSettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuSettingDialog.h"
#include "afxdialogex.h"


// CMenuSettingDialog 对话框

IMPLEMENT_DYNAMIC(CMenuSettingDialog, CDialogEx)

CMenuSettingDialog::CMenuSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_REFUND_ORDER, pParent)
{

}

CMenuSettingDialog::~CMenuSettingDialog()
{
}

void CMenuSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMenuSettingDialog, CDialogEx)
END_MESSAGE_MAP()


// CMenuSettingDialog 消息处理程序


BOOL CMenuSettingDialog::PreTranslateMessage(MSG* pMsg)
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


