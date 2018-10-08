#pragma once
#include "afxwin.h"
#include "resource.h"
#include "GifHelper.h"

// CResultPayDialog 对话框

enum PAY_RESULT{
	PAY_OK,
	PAY_FAIL,
	PAY_PAYING,
};

#define TIMER_ID_ORDER_QUERY		101
#define TIMER_ID_BUTTON_COUNTDOWN	102

class CResultPayDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CResultPayDialog)

public:
	CResultPayDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CResultPayDialog();
	//void setState(PAY_RESULT r) { m_payResult = r; };
	void requestPay(const CString& fee, const CString &orderNo, const CString &authCode);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_PAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonClose();
	afx_msg LRESULT OnOrderRequestOk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPaySuccess(WPARAM wParam, LPARAM lParam);

protected:
	void updateUI_DoDataExchange();
	void updateUI_InitDialog();

protected:
	UINT_PTR m_close_timer_ID;
	int m_countdownTime;
	PAY_RESULT m_payResult;

	UINT_PTR m_timer_orderQuery;

public:
	CStatic m_payOK_ctrl;
	CStatic m_payResultCtrl;
	CStatic m_payFeeCtrl;
	CStatic m_payingCtrl;
	CButton m_btn;
	CRect m_gifrt;

protected:
	CStringA  m_strAOrderNoCode;
	CStringA  m_strAAuthCode;
	CStringA  m_payResultDesc;
	CStringA  m_payTotalFee;
	gif_declare_member()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
