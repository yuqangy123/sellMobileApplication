#pragma once
#include "afxwin.h"


// CQRCodePayDialog 对话框

class CQRCodePayDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CQRCodePayDialog)

public:
	CQRCodePayDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQRCodePayDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_QRCODE_PAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//将窗口调用到最上层，然后获取鼠标焦点
	void SetActive(HWND hWnd);
	void updateEditFocus(int n);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_outTradeNoCtrl;
	CEdit m_willPayFeeCtrl;
	CEdit m_authCodeCtrl;
	CEdit m_payFeeCtrl;
	int m_keyUpDownIndex;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
