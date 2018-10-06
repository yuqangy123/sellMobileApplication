// MenuDownloadOrderDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuDownloadOrderDialog.h"
#include "afxdialogex.h"
#include "DownloadOrderResultDialog.h"
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
	DDX_Control(pDX, IDC_COMBO_PAYTYPE, m_payTypeComboBox);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BEGIN, m_beginDataTimePicker);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_endDataTimePicker);
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
	m_beginDataTimePicker.SetFocus();
}

BOOL CMenuDownloadOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	m_payTypeComboBox.AddString(L"所有交易");
	m_payTypeComboBox.AddString(L"支付成功的交易");
	m_payTypeComboBox.AddString(L"退款的交易");
	m_payTypeComboBox.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

#include "QRCodePayDialog.h"
void CMenuDownloadOrderDialog::OnBnClickedButtonDownload()
{
#if 1
	CQRCodePayDialog dlgtest;
	dlgtest.DoModal();
	return;
#endif

	CTime timeBegin;
	CString startDate;
	m_beginDataTimePicker.GetTime(timeBegin);
	startDate.Format(L"%04d%02d%02d", timeBegin.GetYear(), timeBegin.GetMonth(), timeBegin.GetDay());

	CTime timeEnd;
	CString endDate;
	m_endDataTimePicker.GetTime(timeEnd);
	endDate.Format(L"%04d%02d%02d", timeEnd.GetYear(), timeEnd.GetMonth(), timeEnd.GetDay());

	int selectPayType = m_payTypeComboBox.GetCurSel() + 1;
	CString payType;
	payType.Format(L"%d", selectPayType);

	CDownloadOrderResultDialog dlg;
	dlg.requestDownloader(startDate, endDate, payType);
	dlg.DoModal();

	
}
