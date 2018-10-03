#pragma once
#include "afxwin.h"
#include "resource.h"

// CResultPayDialog �Ի���

enum PAY_RESULT{
	PAY_OK,
	PAY_FAIL,
	PAY_PAYING,
};

#define TIMER_ID_ORDER_QUERY		101
#define TIMER_ID_DOWNLOAD_WAITING	102
#define TIMER_ID_BUTTON_COUNTDOWN	103

class CResultPayDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CResultPayDialog)

public:
	CResultPayDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CResultPayDialog();
	//void setState(PAY_RESULT r) { m_payResult = r; };
	void requestOrder(const CString &orderNo, const CString &authCode);

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_PAY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonClose();
	afx_msg LRESULT OnOrderQuery(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnPaySuccess(WPARAM wParam, LPARAM lParam);

protected:
	UINT_PTR m_close_timer_ID;
	int m_countdownTime;
	PAY_RESULT m_payResult;

	UINT_PTR m_timer_orderQuery;
	UINT_PTR m_timer_downloadWaiting;
public:
	CStatic m_payOK_ctrl;
	CStatic m_payResultCtrl;
	CStatic m_payFeeCtrl;
	CStatic m_payingCtrl;
	CButton m_btn;
};
