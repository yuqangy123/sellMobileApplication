// DownloadOrderResultDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "DownloadOrderResultDialog.h"
#include "afxdialogex.h"


// CDownloadOrderResultDialog 对话框

IMPLEMENT_DYNAMIC(CDownloadOrderResultDialog, CDialogEx)

CDownloadOrderResultDialog::CDownloadOrderResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_DOWNLOAD_ORDER, pParent)
{
	m_state = DOWNLOAD_FAIL;
}

CDownloadOrderResultDialog::~CDownloadOrderResultDialog()
{
}

void CDownloadOrderResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_PAY_OK, m_picutreCtrl);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_OK, m_downloadResultCtrl);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_FILEPATH, m_downloadFileCtrl);
	DDX_Control(pDX, IDC_BUTTON_REPLAY_DOWNLOAD, m_replayDownloadBtn);

	CString imgPath = L"";
	switch (m_state)
	{
	case DOWNLOAD_OK: {
		m_replayDownloadBtn.ShowWindow(SW_HIDE);
		CImage image;
		image.Load(L"res/payok.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_downloadResultCtrl.SetWindowTextW(L"导出成功");
		m_downloadFileCtrl.SetWindowText(L"文件名：xxx.rar\r\n导出目录：C://Desktop");

	}break;
	case DOWNLOAD_FAIL: {
		CImage image;
		image.Load(L"res/payfail.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_downloadFileCtrl.ShowWindow(SW_HIDE);
		m_downloadResultCtrl.SetWindowTextW(L"导出失败");
	}break;
	default:
		break;
	}
}


BEGIN_MESSAGE_MAP(CDownloadOrderResultDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REPLAY_DOWNLOAD, &CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDownloadOrderResultDialog 消息处理程序


void CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
}


BOOL CDownloadOrderResultDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CDownloadOrderResultDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}




HBRUSH CDownloadOrderResultDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	switch (m_state)
	{
	case DOWNLOAD_OK: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_DOWNLOAD_FILEPATH)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}break;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	// TODO:  在此更改 DC 的任何特性

	return hbr;
}
