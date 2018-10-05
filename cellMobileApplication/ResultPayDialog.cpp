// ResultPayDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "ResultPayDialog.h"
#include "afxdialogex.h"
#include "resource.h"
#include "sellMobileSystem.h"
#include "commonMicro.h"

// CResultPayDialog 对话框

IMPLEMENT_DYNAMIC(CResultPayDialog, CDialogEx)

CResultPayDialog::CResultPayDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_PAY, pParent)
{
	m_payResult = PAY_PAYING;
}

CResultPayDialog::~CResultPayDialog()
{
}

void CResultPayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_PAY_OK, m_payOK_ctrl);

	
	updateUI_DoDataExchange();

	DDX_Control(pDX, IDC_STATIC_PAY_FEE, m_payFeeCtrl);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btn);
	DDX_Control(pDX, IDC_STATIC_PAY_RESULT, m_payResultCtrl);
	DDX_Control(pDX, IDC_STATIC_PAYING, m_payingCtrl);
}

void CResultPayDialog::updateUI_DoDataExchange()
{
	CString imgPath = L"";
	switch (m_payResult)
	{
	case PAY_OK: {
		imgPath = L"res/payok.png";
		KillTimer(m_close_timer_ID);
		m_close_timer_ID = SetTimer(TIMER_ID_BUTTON_COUNTDOWN, 1000 * 1, NULL);
		m_countdownTime = 3;
		CImage image;
		image.Load(imgPath);
		HBITMAP hBmp = image.Detach();
		m_payOK_ctrl.SetBitmap(hBmp);
		m_btn.SetFocus();
	}break;
	case PAY_FAIL: {
		imgPath = L"res/payfail.png";
		CImage image;
		image.Load(imgPath);
		HBITMAP hBmp = image.Detach();
		m_payOK_ctrl.SetBitmap(hBmp);
	}break;
	default:
		break;
	}
}

void CResultPayDialog::updateUI_InitDialog()
{
	switch (m_payResult)
	{
	case PAY_OK: {
		m_payingCtrl.ShowWindow(SW_HIDE);

		m_payResultCtrl.SetWindowText(L"收款成功");
		m_payResultCtrl.ShowWindow(SW_SHOW);
		m_payFeeCtrl.ShowWindow(SW_SHOW);
		m_btn.ShowWindow(SW_SHOW);

		CString freeStr(m_payTotalFee);
		freeStr.Insert(0, L"￥");
		m_payFeeCtrl.SetWindowText(freeStr);
		
	}break;
	case PAY_FAIL: {
		m_payingCtrl.ShowWindow(SW_HIDE);
		m_payResultCtrl.SetWindowText(L"收款失败");
		m_payFeeCtrl.SetWindowText(CString(m_payResultDesc));
		m_btn.SetWindowText(L"重新查询");

		m_payResultCtrl.ShowWindow(SW_SHOW);
		m_payFeeCtrl.ShowWindow(SW_SHOW);
		m_btn.ShowWindow(SW_SHOW);
	}break;
	case PAY_PAYING: {
		m_payingCtrl.ShowWindow(SW_SHOW);
		m_payResultCtrl.ShowWindow(SW_HIDE);
		m_payFeeCtrl.ShowWindow(SW_HIDE);
		m_btn.ShowWindow(SW_HIDE);
	}break;
	default:
		break;
	}
}

BEGIN_MESSAGE_MAP(CResultPayDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CResultPayDialog::OnBnClickedButtonClose)
	ON_MESSAGE(UM_ORDER_REQUEST_OK_NOTIFY, &CResultPayDialog::OnOrderRequestOk)
	ON_MESSAGE(UM_PAY_SUCCESS_NOTIFY, &CResultPayDialog::OnPaySuccess)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CResultPayDialog 消息处理程序


BOOL CResultPayDialog::OnInitDialog()
{
	
	CDialogEx::OnInitDialog();


	// TODO:  在此添加额外的初始化
	updateUI_InitDialog();

	sellMobileSystemInstance->requestMicropay(GetSafeHwnd(), m_payTotalFee.GetString(), m_strAOrderNoCode.GetString(), m_strAAuthCode.GetString());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CResultPayDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	switch (m_payResult)
	{
	case PAY_OK: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PAY_FEE)
		{
			CFont font;
			font.CreatePointFont(120, _T("宋体"));
			pDC->SelectObject(&font);
		}
	}break;
	case PAY_PAYING: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PAYING)
		{
			CFont font;
			font.CreatePointFont(120, _T("宋体"));
			pDC->SelectObject(&font);
		}
	}break;
	default:
		break;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	
	
	

	return hbr;
}


void CResultPayDialog::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	switch (m_payResult)
	{
	case PAY_OK: {
		::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
	}break;
	case PAY_FAIL: {
		m_payResult = PAY_PAYING;
		updateUI_InitDialog();
		sellMobileSystemInstance->requestMicropay(GetSafeHwnd(), m_payTotalFee.GetString(), m_strAOrderNoCode.GetString(), m_strAAuthCode.GetString());
	}break;
	default:
		break;
	}
}


void CResultPayDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMER_ID_ORDER_QUERY: {
		sellMobileSystemInstance->requestOrderQuery(GetSafeHwnd());
	}break;
	case TIMER_ID_BUTTON_COUNTDOWN: {
		m_countdownTime = m_countdownTime - 1;
		CString str;
		str.Format(L"关闭(%d)", m_countdownTime);
		m_btn.SetWindowText(str);
		if (m_countdownTime < 0)
		{
			KillTimer(m_close_timer_ID);
			::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
		}
			
	}break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CResultPayDialog::requestPay(const CString& fee, const CString &orderNo, const CString &authCode)
{
	m_payTotalFee = fee;
	m_strAOrderNoCode = orderNo;
	m_strAAuthCode = authCode;
	m_payResult = PAY_PAYING;	
}


LRESULT CResultPayDialog::OnOrderRequestOk(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
	{
		m_timer_orderQuery = SetTimer(TIMER_ID_ORDER_QUERY, 1000 * 3, NULL);
		sellMobileSystemInstance->requestOrderQuery(GetSafeHwnd());
	}
	else if(-1 == wParam)
	{
		char* msg = (char*)lParam;
		m_payResultDesc = msg;
		safe_delete(msg);
		KillTimer(m_timer_orderQuery);
		m_payResult = PAY_FAIL;
		updateUI_DoDataExchange();
		updateUI_InitDialog();
	}
	
	return 0;
}


LRESULT CResultPayDialog::OnPaySuccess(WPARAM wParam, LPARAM lParam)
{
	if (0 == wParam)
	{
		if (m_payResult == PAY_PAYING)
		{
			m_payResult = PAY_OK;

			KillTimer(m_timer_orderQuery);
			updateUI_DoDataExchange();
			updateUI_InitDialog();

		}
	}
	else if(-1 == wParam)
	{
		char* msg = (char*)lParam;
		m_payResultDesc = msg;
		safe_delete(msg);
		m_payResult = PAY_FAIL;

		KillTimer(m_timer_orderQuery);
		updateUI_DoDataExchange();
		updateUI_InitDialog();
	}

	
	return 0;
}


void CResultPayDialog::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}


void CResultPayDialog::OnDestroy()
{
	CDialogEx::OnDestroy();
	CWnd* parent = GetParent();
	::PostMessage(parent->GetSafeHwnd(), WM_CLOSE, 0, 0);

	// TODO: 在此处添加消息处理程序代码
}
