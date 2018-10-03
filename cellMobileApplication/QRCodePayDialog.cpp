// QRCodePayDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "QRCodePayDialog.h"
#include "afxdialogex.h"
#include "resource.h"
#include "sellMobileSystem.h"
#include "commonMicro.h"
#include "ResultPayDialog.h"

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

	// TODO:  在此添加额外的初始化
#ifdef DEBUG_MODE	
	char randChar[32] = { 0 };
	long rd = getRandom(99999999);
	sprintf_s(randChar, "121775250120070233368%d", rd);	
	m_outTradeNoCtrl.SetWindowText(CString(randChar));
	m_willPayFeeCtrl.SetWindowText(CString("1"));
	m_payFeeCtrl.SetWindowText(CString("1"));

#endif

	m_authCodeCtrl.SetFocus();

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
		CString strTradeNo;
		m_outTradeNoCtrl.GetWindowText(strTradeNo);

		CString strOrderNoCode;
		m_outTradeNoCtrl.GetWindowText(strOrderNoCode);
		CStringA strAOrderNoCode(strOrderNoCode);

		CString strAuthCode;
		m_authCodeCtrl.GetWindowText(strAuthCode);
		CStringA strAAuthCode(strAuthCode);

		CResultPayDialog dlg;
		dlg.DoModal();

		//
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
