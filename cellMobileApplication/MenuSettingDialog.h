#pragma once
#include "resource.h"


// CMenuSettingDialog 对话框

class CMenuSettingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMenuSettingDialog)

public:
	CMenuSettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMenuSettingDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU_REFUND_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};
