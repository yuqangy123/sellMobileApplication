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

// CQRCodePayDialog 对话框

IMPLEMENT_DYNAMIC(CQRCodePayDialog, CDialogEx)

CQRCodePayDialog::CQRCodePayDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_QRCODE_PAY, pParent)
{

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
END_MESSAGE_MAP()


// CQRCodePayDialog 消息处理程序


BOOL CQRCodePayDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	std::string order, systemOrder;
	DataMgrInstanceEx.getGoodsInfoOrder(order, systemOrder);
	m_outTradeNoCtrl.SetWindowText(CString(order.c_str()));

	CString csTotalFee;
	DataMgrInstanceEx.getGoodsInfoTotalFee(CString(systemOrder.c_str()), csTotalFee);
	m_willPayFeeCtrl.SetWindowText(csTotalFee);
	m_payFeeCtrl.SetWindowText(csTotalFee);



	m_authCodeCtrl.SetFocus();

	SetActive(m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CQRCodePayDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return CDialogEx::PreTranslateMessage(pMsg);
	}
	//屏蔽回车关闭窗体,但会导致回车在窗体上失效.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		CString strOrderNoCode;
		m_outTradeNoCtrl.GetWindowText(strOrderNoCode);
		
		CString strAuthCode;
		m_authCodeCtrl.GetWindowText(strAuthCode);
		
		CString strFee;
		m_payFeeCtrl.GetWindowText(strFee);
		if (!stringIsNumber(strFee.GetString()))
		{
			MessageBox(L"支付金额输入有误，请重新输入", L"提示");
			m_payFeeCtrl.SetFocus();
			return TRUE;
		}
		CResultPayDialog dlg;
		//test code
		//dlg.requestPay(strFee, strOrderNoCode, strAuthCode);
		dlg.requestPay(L"0.01", L"0200990012009980000017000001", strAuthCode);
		
		dlg.DoModal();

		//
		return TRUE;
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
