
// cellMobileApplicationDlg.h : 头文件
//

#pragma once

#include "Node.h"
#include "resource.h"

#define UM_TIPS_MESSAGE			(WM_USER + 100)
#define UM_ORDER_QUERY			(WM_USER + 101)
#define UM_PAY_SUCCESS_NOTIFY	(WM_USER + 102)

#define TIMER_ID_ORDER_QUERY	1

// CcellMobileApplicationDlg 对话框
class CcellMobileApplicationDlg : public CDialogEx , public CNode
{
// 构造
public:
	CcellMobileApplicationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CELLMOBILEAPPLICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	//msg event
	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTipsMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderQuery(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPaySuccess(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();

protected:
	UINT_PTR m_timer_orderQuery;
};
