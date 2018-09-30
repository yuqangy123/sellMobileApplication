#pragma once
#include "resource.h"

// TopDialog dialog

class TopDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TopDialog)

public:
	TopDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TopDialog();
	void setTipsMessage(const char* msg);

// Dialog Data
	enum { IDD = IDD_TIPS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	CString m_msg;
};
