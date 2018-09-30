
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
}

BEGIN_MESSAGE_MAP(CcellMobileApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CcellMobileApplicationDlg::OnBnClickedOk)
	ON_MESSAGE(UM_TIPS_MESSAGE, &CcellMobileApplicationDlg::OnTipsMessage)
	ON_MESSAGE(UM_ORDER_QUERY, &CcellMobileApplicationDlg::OnOrderQuery)
	ON_MESSAGE(UM_PAY_SUCCESS_NOTIFY, &CcellMobileApplicationDlg::OnPaySuccess)
	
	
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
	sellMobileSystemInstance->requestDownloadOrder();
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

LRESULT CcellMobileApplicationDlg::OnOrderQuery(WPARAM wParam, LPARAM lParam)
{

	m_timer_orderQuery = SetTimer(TIMER_ID_ORDER_QUERY, 3000, NULL);

	sellMobileSystemInstance->requestOrderQuery();

	return 0;
}

void CcellMobileApplicationDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_ID_ORDER_QUERY:
	{
		sellMobileSystemInstance->requestOrderQuery();
	}break;
	default:
		break;
	}
}

LRESULT CcellMobileApplicationDlg::OnPaySuccess(WPARAM wParam, LPARAM lParam)
{
	KillTimer(m_timer_orderQuery);

	TopDialog dlg;
	dlg.setTipsMessage("支付成功");
	dlg.DoModal();

	return 0;
}
