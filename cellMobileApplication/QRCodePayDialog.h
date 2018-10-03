#pragma once
#include "afxwin.h"


// CQRCodePayDialog �Ի���

class CQRCodePayDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CQRCodePayDialog)

public:
	CQRCodePayDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CQRCodePayDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_QRCODE_PAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_outTradeNoCtrl;
	CEdit m_willPayFeeCtrl;
	CEdit m_authCodeCtrl;
	CEdit m_payFeeCtrl;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
