#pragma once
#include "resource.h"
#include "afxdtctl.h"
#include "afxwin.h"

// CMenuDownloadOrderDialog 对话框

class CMenuDownloadOrderDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMenuDownloadOrderDialog)

public:
	CMenuDownloadOrderDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMenuDownloadOrderDialog();
	void setTimeControlFocus();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU_DOWNLOAD_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CDateTimeCtrl m_payTimeCtrl;
	CComboBox m_payTypeComboBox;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDownload();
	CDateTimeCtrl m_beginDataTimePicker;
	CDateTimeCtrl m_endDataTimePicker;
	afx_msg void OnIdok();
};
