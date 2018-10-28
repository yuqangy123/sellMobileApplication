#pragma once
#include "resource.h"


// CMenuRefundOrderDialog 对话框

class CMenuRefundOrderDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMenuRefundOrderDialog)

public:
	CMenuRefundOrderDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMenuRefundOrderDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU_REFUND_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSure();
	virtual BOOL OnInitDialog();
	CEdit m_orderNoCtrl;
	CEdit m_totalFeeCtrl;
	CEdit m_feeCtrl;
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	void updateOrderAndFee();
};
