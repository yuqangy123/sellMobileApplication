// QRCodePayDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "QRCodePayDialog.h"
#include "afxdialogex.h"
#include "resource.h"
#include "sellMobileSystem.h"
#include "commonMicro.h"
#include "ResultPayDialog.h"
#include "DataManager.h"
#include "SelectBillsDialog.h"

// CQRCodePayDialog 对话框

IMPLEMENT_DYNAMIC(CQRCodePayDialog, CDialogEx)

CQRCodePayDialog::CQRCodePayDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_QRCODE_PAY, pParent)
{
	m_keyUpDownIndex = 1;
}

CQRCodePayDialog::~CQRCodePayDialog()
{
}

void CQRCodePayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_XIAOSHOUDANHAO, m_outTradeNoCtrl);
	DDX_Control(pDX, IDC_EDIT_YINGFUJINER, m_willPayFeeCtrl);
	DDX_Control(pDX, IDC_EDIT_FUKUANMA, m_authCodeCtrl);
	DDX_Control(pDX, IDC_EDIT_ZHIFUJINER, m_payFeeCtrl);
}


BEGIN_MESSAGE_MAP(CQRCodePayDialog, CDialogEx)
	ON_EN_SETFOCUS(IDC_EDIT_ZHIFUJINER, &CQRCodePayDialog::OnSetfocusEditZhifujiner)
	ON_EN_SETFOCUS(IDC_EDIT_FUKUANMA, &CQRCodePayDialog::OnSetfocusEditFukuanma)
END_MESSAGE_MAP()


// CQRCodePayDialog 消息处理程序


BOOL CQRCodePayDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//this->SetWindowText(_T("CQRCodePayDialog"));

	std::string systemOrder;
	systemOrder = DataMgrInstanceEx.getSystemOrder();
	m_outTradeNoCtrl.SetWindowText(CString(systemOrder.c_str()));
	

	CString csTotalFee;
	
	//DataMgrInstanceEx.getGoodsInfoTotalFee(CString(systemOrder.c_str()), csTotalFee);
	
	double bills = DataMgrInstanceEx.getBillByTesserImage();
	csTotalFee.Format(L"%.2f", bills);

	m_willPayFeeCtrl.SetWindowText(csTotalFee);
	m_payFeeCtrl.SetWindowText(csTotalFee);

	m_outTradeNoFont.CreatePointFont(180, L"宋体");
	m_outTradeNoCtrl.SetFont(&m_outTradeNoFont);

	m_payFeeFont.CreatePointFont(180, L"宋体");
	m_payFeeCtrl.SetFont(&m_payFeeFont);

	m_authCodeFont.CreatePointFont(180, L"宋体");
	m_authCodeCtrl.SetFont(&m_authCodeFont);

	m_willPayFeeFont.CreatePointFont(180, L"宋体");
	m_willPayFeeCtrl.SetFont(&m_willPayFeeFont);
	

	setBestFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CQRCodePayDialog::PreTranslateMessage(MSG* pMsg)
{
	static bool CtrlIndex = false;
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		//::PostMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		//::PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), UM_HOOK_KEYBOARD_SHOW_HIDE, ctrl_key_cov | VK_F4, 11);
		//return true;
		return CDialogEx::PreTranslateMessage(pMsg);
	}
	//屏蔽回车关闭窗体,但会导致回车在窗体上失效.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		CString strSystemOrder;
		m_outTradeNoCtrl.GetWindowText(strSystemOrder);
		CStringA strSystemOrderA(strSystemOrder);
		
		CString strAuthCode;
		m_authCodeCtrl.GetWindowText(strAuthCode);
		CStringA strAuthCodeA(strAuthCode);
		
		CString strFee;
		m_payFeeCtrl.GetWindowText(strFee);
		CStringA strFeeA(strFee);
		if (!stringIsNumber(strFee.GetString()))
		{
			MessageBox(L"支付金额输入有误，请重新输入", L"提示");
			m_payFeeCtrl.SetFocus();
			return TRUE;
		}

		CString strWillFee;
		m_willPayFeeCtrl.GetWindowText(strWillFee);
		CStringA strWillFeeA(strWillFee);

		char qrLog[512] = {0};
		sprintf_s(qrLog, "销售单号：%s, 应付金额：%s, 支付金额：%s\n", strSystemOrderA.GetString(), strWillFeeA.GetString(), strFeeA.GetString());
		DataMgrInstanceEx.writeLog(qrLog);

		CResultPayDialog dlg;
		CStringA cellOrderA = DataMgrInstanceEx.getSellOrderWithSystemOrder(strSystemOrderA);
		dlg.requestPay(strFee, CString(cellOrderA), strAuthCode);
		dlg.DoModal();

		//
		return TRUE;
	}
	//上下键盘
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN && pMsg->wParam)
	{
		updateEditFocus(1);
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP && pMsg->wParam)
	{
		updateEditFocus(-1);
	}
	//
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL && pMsg->wParam)
	{
		CtrlIndex = true;
	}
	else if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_CONTROL && pMsg->wParam)
	{
		CtrlIndex = false;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F6 && pMsg->wParam)
	{
		CtrlIndex = false;
		CSelectBillsDialog dlg2;
		dlg2.DoModal();
		CString systemOrder = dlg2.getSelectBill();
		if (!systemOrder.IsEmpty())
		//if (false)
		{
			m_outTradeNoCtrl.SetWindowText(systemOrder);

			CString csTotalFee;
			DataMgrInstanceEx.getGoodsInfoTotalFee(systemOrder, csTotalFee);
			m_willPayFeeCtrl.SetWindowText(csTotalFee);
			m_payFeeCtrl.SetWindowText(csTotalFee);

		}
		SetActive(m_hWnd);
		::PostMessage(m_authCodeCtrl.m_hWnd, WM_SETFOCUS, 0, 0);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CQRCodePayDialog::SetActive(HWND hWnd)
{
	DWORD dwForeID;
	DWORD dwCurID;
	HWND hForeWnd = ::GetForegroundWindow();
	dwCurID = ::GetCurrentThreadId();
	dwForeID = ::GetWindowThreadProcessId( hForeWnd, NULL );
	::AttachThreadInput( dwCurID, dwForeID, TRUE); //获取输入焦点?
	::SetWindowPos(hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); //设置Z-Order?
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); //还原Z-Order?
	::SetForegroundWindow(hWnd);
	::AttachThreadInput( dwCurID, dwForeID, FALSE); 
}


void CQRCodePayDialog::updateEditFocus(int n)
{

	m_keyUpDownIndex += n;

	if (m_keyUpDownIndex < 1)m_keyUpDownIndex = 2;
	if (m_keyUpDownIndex > 2)m_keyUpDownIndex = 1;

	switch (m_keyUpDownIndex)
	{
	case 1:{
			   m_authCodeCtrl.SetFocus();
	}break;
	case 2:{
			   m_payFeeCtrl.SetFocus();
	}break;
	}
}


void CQRCodePayDialog::OnSetfocusEditZhifujiner()
{
	// TODO: 在此添加控件通知处理程序代码
	m_payFeeCtrl.SetSel(0, -1);
}


void CQRCodePayDialog::OnSetfocusEditFukuanma()
{
	// TODO: 在此添加控件通知处理程序代码
	m_authCodeCtrl.SetSel(0, -1);
}

void CQRCodePayDialog::setBestFocus()
{
	m_authCodeCtrl.SetFocus();

	SetActive(m_hWnd);
}