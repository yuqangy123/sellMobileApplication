#pragma once
#include "resource.h"
#include "afxwin.h"


// CMenuSettingDialog 对话框

class CMenuSettingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMenuSettingDialog)

public:
	CMenuSettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMenuSettingDialog();
	void checkNetwork();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU_REFUND_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNetworkStateNotify(WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();
	static void* pthread_checkNet(void* arg);

protected:
	bool m_bConnect;
	bool m_bPrinter;
	bool m_bScanner;
	CStatic m_networkStateCtrl;
	CStatic m_MachIdCtrl;
	CStatic m_printerStateCtrl;
	CStatic m_scannerStateCtrl;

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
};
