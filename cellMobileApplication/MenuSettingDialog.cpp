// MenuSettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuSettingDialog.h"
#include "afxdialogex.h"
#include "commonMicro.h"

#include <Sensapi.h>
#include <Wininet.h> 
#include "pthread.h"
#include <functional>

#pragma comment(lib, "Sensapi.lib")
#pragma comment(lib, "Wininet.lib")  

// CMenuSettingDialog 对话框

IMPLEMENT_DYNAMIC(CMenuSettingDialog, CDialogEx)

CMenuSettingDialog::CMenuSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_SETTING, pParent)
{
	m_bConnect = false;
}

CMenuSettingDialog::~CMenuSettingDialog()
{
}

void CMenuSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NETWORK_STATE, m_networkStateCtrl);
	
}


BEGIN_MESSAGE_MAP(CMenuSettingDialog, CDialogEx)
	ON_MESSAGE(UM_NETWORK_STATE_NOTIFY, &CMenuSettingDialog::OnNetworkStateNotify)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMenuSettingDialog 消息处理程序
LRESULT CMenuSettingDialog::OnNetworkStateNotify(WPARAM wParam, LPARAM lParam)
{
	m_networkStateCtrl.SetWindowText(m_bConnect ? L"已连接" : L"网络异常");
	return 0;
}

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

void* CMenuSettingDialog::pthread_checkNet(void* arg)
{
	CMenuSettingDialog* dlg = (CMenuSettingDialog*)arg;
	
	BOOL isConnect;
	DWORD dw;
	isConnect = ::IsNetworkAlive(&dw);
	int connTimeout = 0;
	if (!isConnect)
	{
		dlg->m_bConnect = false;
		::PostMessage(dlg->GetSafeHwnd(), UM_NETWORK_STATE_NOTIFY, 0, 0);
		return 0;
	}
	while (connTimeout <=10)
	{
		DWORD dw2;
		BOOL ret = InternetGetConnectedState(&dw2, 0);

		if (!ret)
		{
			Sleep(2000);
			continue;
		}

		BOOL bConnected = InternetCheckConnection(_T("http://www.baidu.com"), FLAG_ICC_FORCE_CONNECTION, 0);
		if (bConnected)
		{
			dlg->m_bConnect = true;
			::PostMessage(dlg->GetSafeHwnd(), UM_NETWORK_STATE_NOTIFY, 0, 0);
			return 0;
		}
		Sleep(2000);
		connTimeout += 1;
	}

	dlg->m_bConnect = false;
	::PostMessage(dlg->GetSafeHwnd(), UM_NETWORK_STATE_NOTIFY, 0, 0);
	return 0;
}

void CMenuSettingDialog::checkNetwork()
{
	pthread_t tid;
	int error = pthread_create(&tid,
		NULL, /* default attributes please */
		pthread_checkNet,
		(void *)this);

	if (0 != error)
	{
		fprintf(stderr, "Couldn't run stackHttpCallback thread errno %d\n", error);
	}

	pthread_detach(tid);
}

BOOL CMenuSettingDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	checkNetwork();
	OnNetworkStateNotify(0, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CMenuSettingDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NETWORK_STATE)
	{
		pDC->SetTextColor(m_bConnect ? RGB(34, 177, 76) : RGB(255, 0, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
