
// cellMobileApplicationDlg.cpp : 瀹炵幇鏂囦欢
//

#include "stdafx.h"
#include "cellMobileApplication.h"
#include "cellMobileApplicationDlg.h"
#include "afxdialogex.h"

#include "commonMicro.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include "TopDialog.h"
#include "QRCodePayDialog.h"
#include "DataManager.h"
#include "PrinterDevice.h"

#include "HookDll\HookDll.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool st_unHideDlg = false;

CcellMobileApplicationDlg::CcellMobileApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcellMobileApplicationDlg::IDD, pParent)
{
	b_show = true;
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
	ON_MESSAGE(UM_HOOK_KEYBOARD_SHOW_HIDE, &CcellMobileApplicationDlg::OnHookKeboardShowHide)
	ON_MESSAGE(UM_SHOWQRCODE_PAY_NOTIFY, &CcellMobileApplicationDlg::OnShowQRCodeDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MENU, &CcellMobileApplicationDlg::OnTcnSelchangeTabMenu)
	ON_MESSAGE(UM_ESC_KEYBOARD_NOTIFY, &CcellMobileApplicationDlg::OnEscKeyboardNotify)
END_MESSAGE_MAP()

HWND m_hWndTarget = 0;
CRect rtWin(0, 0, 0, 0);
BOOL PtInWinRect(HWND hWnd, CPoint pt)
{
	GetWindowRect(hWnd, &rtWin);

	wchar_t winText[128] = { 0 };
	GetWindowText(hWnd, winText, 128 );
	return PtInRect(&rtWin, pt);
}

BOOL CALLBACK EnumChildRealTimeProc(HWND hWnd, LPARAM lParam)
{
	if (!PtInWinRect(hWnd, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))) return TRUE;

	//if (ShouldWinBeFiltered(hwnd)) return TRUE;

	m_hWndTarget = hWnd;
	EnumChildWindows(hWnd, EnumChildRealTimeProc, lParam);

	return FALSE;
}

BOOL CALLBACK EnumWindowsRealTimeProc(HWND hwnd, LPARAM lParam)
{
	if (!PtInWinRect(hwnd, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))) return TRUE;

	//if (ShouldWinBeFiltered(hwnd))  return TRUE;

	//m_hWndTarget = hwnd;

	//if (CWinFilterTraits::IsTargetPopupWindow()) return FALSE; //this is the target window, exit search

	EnumChildWindows(hwnd, EnumChildRealTimeProc, lParam);

	return FALSE;
}

BOOL CcellMobileApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	/*
	//test code
	DWORD dw = GetProcessIDFromName(L"SellSystem.exe");
	HWND sellSystemHwnd = getWindowHandleByPID(dw);
	::EnumChildWindows(sellSystemHwnd, EnumChildRealTimeProc, 0);
	//GetWindowedChildCount();

	HWND lhwnd = ::FindWindow(L"TFrmBalance",NULL);

	POINT p;
	p.x = 1920/2; p.y = 1080/2;
	CWnd* hwnd = WindowFromPoint(p);

	EnumWindows(EnumWindowsRealTimeProc, MAKELPARAM(p.x, p.y));
	if (hwnd)
	{
		CRect rctB;
		::GetWindowRect(hwnd->GetSafeHwnd(), &rctB);
		int n = 0;
		++n;
		::EnumChildWindows(hwnd->GetSafeHwnd(), EnumChildRealTimeProc, 0);
	}
	//lhwnd = sellSystemHwnd;
	if (NULL != lhwnd)
	{
		CRect rctA;
		::GetWindowRect(lhwnd, &rctA);
		
		CWnd* deskHwnd = GetDesktopWindow();
		CRect rctB;
		::GetWindowRect(deskHwnd->GetSafeHwnd(), &rctB);
		int n=0;
		++n;
	}
	*/
	/*
	CStringA testchar;
	testchar = "中心流水：810201810291232381964";
	
	int len = testchar.GetLength();

	CStringA lefStr = testchar.Left(30);
	CStringA rigStr = testchar.Right(len-30);
	*/
	

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	DataMgrInstanceEx;
	printerDeviceInstanceEx;

	sellMobileSystemInstance->init(DataMgrInstanceEx.Domain.c_str());
	sellMobileSystemInstance->setMchInfo(DataMgrInstanceEx.MchId.c_str(), DataMgrInstanceEx.MchKey.c_str());
	sellMobileSystemInstance->setMainDialogHwnd(GetSafeHwnd());
	curlManagerInstance;


	m_tabMenu.InsertItem(0, L"订单(ctrl+F1)");
	m_tabMenu.InsertItem(1, L"退款(ctrl+F2)");
	m_tabMenu.InsertItem(2, L"设置(ctrl+F3)");

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
	
	setHook();
	
	

	return TRUE;
}

void CcellMobileApplicationDlg::setHook()
{
	//hook keyboard
	typedef void(*HOOKPROC)(HWND hwnd, const char* hookKeyboard, int hookKeyboardLen);
	HOOKPROC lpfnDllFuncHook;    // Function pointer
	HMODULE hDLL = LoadLibrary(_T("HookDll.dll"));//加载动态链接库
	if (hDLL != NULL) {
		lpfnDllFuncHook = (HOOKPROC)GetProcAddress(hDLL, "SetHook");
		if (lpfnDllFuncHook != NULL) {			// call the function
			char hookKeyboardList[256] = { 0 };
			sprintf_s(hookKeyboardList, "%d,%d,%d,%d", DataMgrInstanceEx.customKeyboard, ctrl_key_cov | VK_F1, ctrl_key_cov | VK_F2, ctrl_key_cov | VK_F3);

			lpfnDllFuncHook(m_hWnd, hookKeyboardList, strlen(hookKeyboardList));
		}
	}
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

void CcellMobileApplicationDlg::selectShowTabMenu(int index)
{
	m_CurSelTab = index;
	if (m_CurSelTab >= 3 || m_CurSelTab < -1)
		m_CurSelTab = 0;

	m_tabMenu.SetCurSel(m_CurSelTab);
	m_menuDownloadOrderDlg.ShowWindow(m_CurSelTab == 0 ? SW_SHOW : SW_HIDE);
	m_menuRefundOrderDlg.ShowWindow(m_CurSelTab == 1 ? SW_SHOW : SW_HIDE);
	m_menuSettingDlg.ShowWindow(m_CurSelTab == 2 ? SW_SHOW : SW_HIDE);

	
	if (1 == m_CurSelTab)
	{
		m_menuRefundOrderDlg.updateOrderAndFee();
	}
		
	SetFocus();
}

BOOL CcellMobileApplicationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			/*
			UINT nRet = MessageBox(L"是否需要退出程序？", L"提示", MB_OKCANCEL);
			if (IDOK == nRet)
			{
				PostQuitMessage(0);
			}*/
			char log[128];
			sprintf_s(log, "CcellMobileApplicationDlg::PreTranslateMessage DataMgrInstanceEx.EscKeyTag %d,", DataMgrInstanceEx.EscKeyTag);
			DataMgrInstanceEx.writeLog(log);
			if(0 == DataMgrInstanceEx.EscKeyTag && !DataMgrInstanceEx.tessing)
				::PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), UM_HOOK_KEYBOARD_SHOW_HIDE, ctrl_key_cov | VK_F4, 11);
			DataMgrInstanceEx.EscKeyTag = 0;
			return TRUE;
		}
		else if ((pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_SPACE) && pMsg->wParam)
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
			selectShowTabMenu(m_CurSelTab + 1);
		}
	}
		

	return __super::PreTranslateMessage(pMsg);
}


LRESULT CcellMobileApplicationDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	//https://www.cnblogs.com/carekee/articles/3096713.html
	if (message == 133)
	{
		this->ShowWindow(b_show ? SW_NORMAL : SW_MINIMIZE);
	};
	return __super::DefWindowProc(message, wParam, lParam);
}

LRESULT CcellMobileApplicationDlg::OnHookKeboardShowHide(WPARAM wParam, LPARAM lParam)
{
	if (wParam == DataMgrInstanceEx.customKeyboard)
	{
		DataMgrInstanceEx.writeLog("PostMessage(UM_SHOWQRCODE_PAY_NOTIFY, 0, 0);");
		PostMessage(UM_SHOWQRCODE_PAY_NOTIFY, 0, 0);
		return 0;
	}

	switch (wParam)
	{
	case ctrl_key_cov | VK_F3: {
		CWnd *pWnd = FindWindow(nullptr, _T("请确认金额后，扫码顾客二维码"));
		if (pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
			st_unHideDlg = true;
		}	

		OnHookKeboardShowHide(ctrl_key_cov | VK_F4, 12);
		selectShowTabMenu(2);
	}break;

	case ctrl_key_cov | VK_F2: {
		CWnd *pWnd = FindWindow(nullptr, _T("请确认金额后，扫码顾客二维码"));
		if (pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
			st_unHideDlg = true;
		}

		OnHookKeboardShowHide(ctrl_key_cov | VK_F4, 12);
		selectShowTabMenu(1);
	}break;

	case ctrl_key_cov | VK_F1: {
		CWnd *pWnd = FindWindow(nullptr, _T("请确认金额后，扫码顾客二维码"));
		if (pWnd)
		{
			pWnd->SendMessage(WM_CLOSE);
			st_unHideDlg = true;
		}

		OnHookKeboardShowHide(ctrl_key_cov | VK_F4, 12);
		selectShowTabMenu(0);
	}break;

	case ctrl_key_cov | VK_F4:{
		b_show = !b_show;
		if (11 == lParam)
			b_show = false;
		else if (12 == lParam)
			b_show = true;

		this->ShowWindow(b_show ? SW_NORMAL : SW_MINIMIZE);
	}break;
	}
	
	return 0;
}

LRESULT CcellMobileApplicationDlg::OnShowQRCodeDlg(WPARAM wParam, LPARAM lParam)
{
	if (DataMgrInstanceEx.tessing)
		return 0;

	DataMgrInstanceEx.writeLog("CcellMobileApplicationDlg::OnShowQRCodeDlg begin");
	static CQRCodePayDialog* st_pDialog = nullptr;
	if (nullptr == st_pDialog)
	{
		CQRCodePayDialog dlgtest;
		st_pDialog = &dlgtest;
		dlgtest.DoModal();
		ShowWindow(SW_NORMAL);	
		DataMgrInstanceEx.writeLog("CcellMobileApplicationDlg::OnShowQRCodeDlg end");
		if (!st_unHideDlg)
		{
			::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), UM_HOOK_KEYBOARD_SHOW_HIDE, ctrl_key_cov | VK_F4, 11);
			st_unHideDlg = false;
		}
		static int test = 0;
		if (DataMgrInstanceEx.resetHookTimes != 0 && test++ >= DataMgrInstanceEx.resetHookTimes)
		{
			test = 0;
			//setHook();
		}
		setHook();
		
		//::PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_KILLFOCUS, 0, 0);
		st_pDialog = nullptr;
	}
	else
	{
		st_pDialog->setBestFocus();
	}
	
	return 0;
}

LRESULT CcellMobileApplicationDlg::OnEscKeyboardNotify(WPARAM wParam, LPARAM lParam)
{
	if (0 == wParam)
	{
		selectShowTabMenu(0);
	}
	else if (1 == wParam)
	{
		selectShowTabMenu(1);
	}

	return 0;
}