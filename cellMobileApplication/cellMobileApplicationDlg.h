
// cellMobileApplicationDlg.h : 头文件
//

#pragma once

#include "Node.h"
#include "resource.h"
#include "afxcmn.h"
#include "MenuDownloadOrderDialog.h"
#include "MenuRefundOrderDialog.h"
#include "MenuSettingDialog.h"




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
	afx_msg LRESULT OnHookKeboardShowHide(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowQRCodeDlg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();

protected:
	int m_CurSelTab;
	bool b_show;
public:
	CTabCtrl m_tabMenu;
	CMenuDownloadOrderDialog m_menuDownloadOrderDlg;
	CMenuRefundOrderDialog m_menuRefundOrderDlg;
	CMenuSettingDialog m_menuSettingDlg;

	afx_msg void OnTcnSelchangeTabMenu(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
