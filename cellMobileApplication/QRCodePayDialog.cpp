// QRCodePayDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "QRCodePayDialog.h"
#include "afxdialogex.h"
#include "resource.h"
#include "sellMobileSystem.h"
#include "commonMicro.h"
#include "ResultPayDialog.h"

// CQRCodePayDialog �Ի���

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


// CQRCodePayDialog ��Ϣ�������


BOOL CQRCodePayDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CQRCodePayDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return CDialogEx::PreTranslateMessage(pMsg);
	}
	//���λس��رմ���,���ᵼ�»س��ڴ�����ʧЧ.
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
