// MenuSettingDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MenuSettingDialog.h"
#include "afxdialogex.h"


// CMenuSettingDialog �Ի���

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


// CMenuSettingDialog ��Ϣ�������


BOOL CMenuSettingDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//����ESC�رմ���/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return TRUE;
	}
	//���λس��رմ���,���ᵼ�»س��ڴ�����ʧЧ.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


