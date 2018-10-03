// MenuDownloadOrderDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuDownloadOrderDialog.h"
#include "afxdialogex.h"
#include "DownloadOrderResultDialog.h"

// CMenuDownloadOrderDialog 对话框

IMPLEMENT_DYNAMIC(CMenuDownloadOrderDialog, CDialogEx)

CMenuDownloadOrderDialog::CMenuDownloadOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_DOWNLOAD_ORDER, pParent)
{

}

CMenuDownloadOrderDialog::~CMenuDownloadOrderDialog()
{
}

void CMenuDownloadOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_payTimeCtrl);
	DDX_Control(pDX, IDC_COMBO_PAYTYPE, m_payTypeComboBox);
}


BEGIN_MESSAGE_MAP(CMenuDownloadOrderDialog, CDialogEx)
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CMenuDownloadOrderDialog::OnBnClickedButtonDownload)
END_MESSAGE_MAP()


// CMenuDownloadOrderDialog 消息处理程序


void CMenuDownloadOrderDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CMenuDownloadOrderDialog::PreTranslateMessage(MSG* pMsg)
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

void CMenuDownloadOrderDialog::setTimeControlFocus()
{
	m_payTimeCtrl.SetFocus();
}

BOOL CMenuDownloadOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	m_payTypeComboBox.AddString(L"所有交易");
	m_payTypeComboBox.AddString(L"个人交易");
	m_payTypeComboBox.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMenuDownloadOrderDialog::OnBnClickedButtonDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	CDownloadOrderResultDialog dlg;
	dlg.DoModal();
}
