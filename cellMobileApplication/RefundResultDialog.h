#pragma once
#include "resource.h"
#include "afxwin.h"
#include "GifHelper.h"
#include "DataManager.h"

// CRefundResultDialog 对话框

enum {
	REFUND_OK,
	REFUND_FAIL,
	REFUND_REFUNDING
};

#define TIMER_ID_ORDER_QUERY		101
class CRefundResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CRefundResultDialog)

public:
	CRefundResultDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRefundResultDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_REFUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	CStatic m_pictureCtrl;
	CStatic m_refundResultCtrl;
	CStatic m_resultDescCtrl;
	CButton m_replayRefundBtn;
	afx_msg void OnBnClickedButtonReplayRefund();
	afx_msg LRESULT OnRefundOrderNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderQuery(WPARAM wParam, LPARAM lParam);
	void updateUI_OnInitDialog();

	//中心流水，原单据号，数据库取的数据
	void requestRefundOrder(const CString& order_no, const CString& out_order_no, const CString& out_refund_no, const CString& totalfee, const CString& fee);

protected:
	int m_refundState;
	CString m_desc;
	CStringA m_orderNo;
	CStringA m_outOrderNo;
	CStringA m_outRefundNo;
	CStringA m_totalfee;
	CStringA m_fee;
	CRect m_gifrt;
	refundOrderInfo* m_info;
	UINT_PTR m_timer_orderQuery;

	gif_declare_member()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
