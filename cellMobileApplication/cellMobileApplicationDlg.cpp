
// cellMobileApplicationDlg.cpp : 瀹炵幇鏂囦欢
//

#include "stdafx.h"
#include "cellMobileApplication.h"
#include "cellMobileApplicationDlg.h"
#include "QRCodePayDialog.h"
#include "afxdialogex.h"

#include "commonMicro.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include "TopDialog.h"
#include "ResultPayDialog.h"
#include "QRCodePayDialog.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CcellMobileApplicationDlg::CcellMobileApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcellMobileApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcellMobileApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MENU, m_tabMenu);
}

BEGIN_MESSAGE_MAP(CcellMobileApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CcellMobileApplicationDlg::OnBnClickedOk)
	ON_MESSAGE(UM_TIPS_MESSAGE, &CcellMobileApplicationDlg::OnTipsMessage)
	ON_MESSAGE(UM_PAY_DOWNLOAD_WAITING_NOTIFY, &CcellMobileApplicationDlg::OnDownloadWaiting)
	
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, &CcellMobileApplicationDlg::OnTcnSelchangeTabMenu)
END_MESSAGE_MAP()


// CcellMobileApplicationDlg 娑堟伅澶勭悊绋嬪簭


BOOL CcellMobileApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	

	sellMobileSystemInstance->setMchInfo("1000000013", "qu9k3vxsy2uc69u86iybirpu14coj34z");
	sellMobileSystemInstance->setMainDialogHwnd(GetSafeHwnd());
	curlManagerInstance;


	m_tabMenu.InsertItem(0, L"订单");
	m_tabMenu.InsertItem(1, L"退款");
	m_tabMenu.InsertItem(2, L"设置");

	m_CurSelTab = 0;

	m_menuDownloadOrderDlg.Create(IDD_DIALOG_MENU_DOWNLOAD_ORDER, GetDlgItem(IDC_TAB_MENU));
	CRect rc;
	m_tabMenu.GetClientRect(rc);
	rc.top += 37;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_menuDownloadOrderDlg.MoveWindow(&rc);
	m_menuDownloadOrderDlg.ShowWindow(m_CurSelTab == 0);

	m_menuRefundOrderDlg.Create(IDD_DIALOG_MENU_REFUND_ORDER, GetDlgItem(IDC_TAB_MENU));	
	m_menuRefundOrderDlg.MoveWindow(&rc);
	m_menuRefundOrderDlg.ShowWindow(m_CurSelTab == 1);
	
	m_menuSettingDlg.Create(IDD_DIALOG_MENU_SETTING, GetDlgItem(IDC_TAB_MENU));
	m_menuSettingDlg.MoveWindow(&rc);
	m_menuSettingDlg.ShowWindow(m_CurSelTab == 2);

	m_tabMenu.SetCurSel(m_CurSelTab);

	CQRCodePayDialog dlg;
	dlg.DoModal();

	return TRUE;
}

void CcellMobileApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CcellMobileApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CcellMobileApplicationDlg::OnBnClickedOk()
{

#if 1
	

	//dlg.DoModal();
	//sellMobileSystemInstance->requestDownloadOrder();
	//bool bl = ::SendMessage(::GetActiveWindow(), UM_TIPS_MESSAGE, 0, 0);
#else
	//showTipsDialog("tt");
#endif
	

	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


LRESULT CcellMobileApplicationDlg::OnTipsMessage(WPARAM wParam, LPARAM lParam)
{
	const char* msg = (const char*)(wParam);
	if (msg)
	{
		//::MessageBoxW(NULL, L"这是一个最简单的消息框！", NULL, MB_OK);
		TopDialog dlg;
		dlg.setTipsMessage((char*)msg);
		dlg.DoModal();
		safe_delete(msg);
	}
	return 0;
}

void CcellMobileApplicationDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_ID_DOWNLOAD_WAITING:
	{
		sellMobileSystemInstance->requestDownloadOrder();
	}break;
	
	default:
		break;
	}
}

LRESULT CcellMobileApplicationDlg::OnDownloadWaiting(WPARAM wParam, LPARAM lParam)
{
	m_timer_downloadWaiting = SetTimer(TIMER_ID_DOWNLOAD_WAITING, 1000*30, NULL);
	return 0;
}

void CcellMobileApplicationDlg::OnTcnSelchangeTabMenu(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_CurSelTab = m_tabMenu.GetCurSel();

	m_menuDownloadOrderDlg.ShowWindow(m_CurSelTab == 0 ? SW_SHOW : SW_HIDE);
	m_menuRefundOrderDlg.ShowWindow(m_CurSelTab == 1 ? SW_SHOW : SW_HIDE);
	m_menuSettingDlg.ShowWindow(m_CurSelTab == 2 ? SW_SHOW : SW_HIDE);
	

	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


BOOL CcellMobileApplicationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DOWN) && pMsg->wParam)
		{
			if (m_CurSelTab == 0)
			{
				m_menuDownloadOrderDlg.setTimeControlFocus();
			}
			else if (m_CurSelTab == 1)
			{
				m_menuRefundOrderDlg.SetFocus();
			}
			return TRUE;
		}
		else if (pMsg->wParam == VK_TAB && pMsg->wParam)
		{
			m_CurSelTab = m_CurSelTab + 1;
			if (m_CurSelTab == 3)
				m_CurSelTab = 0;
			m_tabMenu.SetCurSel(m_CurSelTab);
			m_menuDownloadOrderDlg.ShowWindow(m_CurSelTab == 0 ? SW_SHOW : SW_HIDE);
			m_menuRefundOrderDlg.ShowWindow(m_CurSelTab == 1 ? SW_SHOW : SW_HIDE);
			m_menuSettingDlg.ShowWindow(m_CurSelTab == 2 ? SW_SHOW : SW_HIDE);
		}
	}
		

	return __super::PreTranslateMessage(pMsg);
}
