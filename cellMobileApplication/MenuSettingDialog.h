#pragma once
#include "resource.h"


// CMenuSettingDialog �Ի���

class CMenuSettingDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMenuSettingDialog)

public:
	CMenuSettingDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMenuSettingDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU_REFUND_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};
