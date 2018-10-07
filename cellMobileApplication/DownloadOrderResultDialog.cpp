// DownloadOrderResultDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "DownloadOrderResultDialog.h"
#include "afxdialogex.h"
#include "commonMicro.h"
#include "sellMobileSystem.h"
#include "FileUnit.h"

// CDownloadOrderResultDialog 对话框

IMPLEMENT_DYNAMIC(CDownloadOrderResultDialog, CDialogEx)

CDownloadOrderResultDialog::CDownloadOrderResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_DOWNLOAD_ORDER, pParent)
{
	m_state = DOWNLOAD_DOWNLOADING;
	gif_init_member()
}

CDownloadOrderResultDialog::~CDownloadOrderResultDialog()
{
	gif_destory_member()
}

void CDownloadOrderResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_PAY_OK, m_picutreCtrl);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_OK, m_downloadResultCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_downloadFileCtrl);
	DDX_Control(pDX, IDC_BUTTON_REPLAY_DOWNLOAD, m_replayDownloadBtn);

	updateUI_DoDataExchange();
	
}


BEGIN_MESSAGE_MAP(CDownloadOrderResultDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REPLAY_DOWNLOAD, &CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_MESSAGE(UM_PAY_DOWNLOAD_REQUEST_NOTIFY, &CDownloadOrderResultDialog::OnDownloadRequestNotify)
END_MESSAGE_MAP()


// CDownloadOrderResultDialog 消息处理程序


void CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	//::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
	startDownloader();
}


BOOL CDownloadOrderResultDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	startDownloader();
	
	gif_loadGif(L"res//loading.gif", 100);
	gif_show(true);

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
		if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}break;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	// TODO:  在此更改 DC 的任何特性

	return hbr;
}

void CDownloadOrderResultDialog::requestDownloader(const CString& beginDate, const CString& endDate, const CString& type)
{
	m_beginDate = beginDate;
	m_endDate = endDate;
	m_type = type;
}

void CDownloadOrderResultDialog::updateUI_DoDataExchange()
{
	CString imgPath = L"";
	switch (m_state)
	{
	case DOWNLOAD_OK: {
		KillTimer(m_timer_downloading_updateUI);
		KillTimer(m_timer_downloadWaiting);
		m_replayDownloadBtn.ShowWindow(SW_HIDE);
		CImage image;
		image.Load(L"res/payok.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_picutreCtrl.ShowWindow(SW_SHOW);
		m_downloadResultCtrl.SetWindowText(L"导出成功");
		m_downloadResultCtrl.ShowWindow(SW_SHOW);
		//m_downloadFilePath
		CStringA desc;
		std::string path = FileUnitInstance->ExtractFilePath(m_downloadFilePath.GetString());
		std::string name = FileUnitInstance->ExtractFileName(m_downloadFilePath.GetString());
		desc.Format("文件名：%s\r\n\r\n导出目录：\r\n%s", name.c_str(), path.c_str());
		m_downloadFileCtrl.SetWindowText(CString(desc));
		m_downloadFileCtrl.ShowWindow(SW_SHOW);
		gif_show(false);

	}break;
	case DOWNLOAD_FAIL: {
		KillTimer(m_timer_downloading_updateUI);
		KillTimer(m_timer_downloadWaiting);
		CImage image;
		image.Load(L"res/payfail.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_picutreCtrl.ShowWindow(SW_SHOW);
		m_downloadFileCtrl.ShowWindow(SW_HIDE);
		m_downloadResultCtrl.SetWindowText(L"导出失败");
		m_downloadResultCtrl.ShowWindow(SW_SHOW);
		m_replayDownloadBtn.ShowWindow(SW_SHOW);
		m_replayDownloadBtn.SetFocus();
		gif_show(false);

	}break;

	case DOWNLOAD_DOWNLOADING: {
		m_downloadResultCtrl.SetWindowText(L"导出中...60s");
		m_downloadResultCtrl.ShowWindow(SW_SHOW);
		m_timer_downloading_updateUI = SetTimer(TIMER_ID_DOWNLOADING_UPDATEUI, 1000, NULL);
		m_downloadingUpdateUI_countdown = 60;
		m_replayDownloadBtn.ShowWindow(SW_HIDE);
		m_picutreCtrl.ShowWindow(SW_HIDE);
		m_downloadFileCtrl.ShowWindow(SW_HIDE);
		gif_show(true);
	}break;
	default:
		break;
	}
}

void CDownloadOrderResultDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
		case TIMER_ID_DOWNLOADING_UPDATEUI: 
		{
			if (DOWNLOAD_DOWNLOADING == m_state)
			{
				m_downloadingUpdateUI_countdown -= 1;
				if (m_downloadingUpdateUI_countdown < 0)
				{
					downloaderFail();
				}
				else
				{
					CString str;
					str.Format(L"导出中...%ds", m_downloadingUpdateUI_countdown);
					m_downloadResultCtrl.SetWindowText(str);
				}
			}
		}break;

		case TIMER_ID_DOWNLOAD_REPEAT_QUREY: {
			sellMobileSystemInstance->requestDownloadOrder(GetSafeHwnd(), m_beginDate.GetString(), m_endDate.GetString(), m_type.GetString());
		}break;
	}

	gif_draw_timer(190, 66)
	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CDownloadOrderResultDialog::OnDownloadRequestNotify(WPARAM wParam, LPARAM lParam)
{
	
	if (0 == wParam)
	{
		m_timer_downloadWaiting = SetTimer(TIMER_ID_DOWNLOAD_REPEAT_QUREY, 1000 * 30, NULL);
	}
	else if (-1 == wParam)
	{
		char* pmsg = (char*)lParam;
		safe_delete(pmsg);
		downloaderFail();
	}
	else if (1 == wParam)
	{
		m_state = DOWNLOAD_OK;
		char* pchar = (char*)lParam;
		m_downloadFilePath = pchar;
		updateUI_DoDataExchange();
		safe_delete(pchar);
	}
	
	return 0;
}

void CDownloadOrderResultDialog::downloaderFail()
{
	m_state = DOWNLOAD_FAIL;
	updateUI_DoDataExchange();
}

void CDownloadOrderResultDialog::startDownloader()
{
	m_state = DOWNLOAD_DOWNLOADING;
	sellMobileSystemInstance->requestDownloadOrder(GetSafeHwnd(), m_beginDate.GetString(), m_endDate.GetString(), m_type.GetString());
	updateUI_DoDataExchange();
}
