#pragma once
#include "resource.h"
#include "afxwin.h"

// CRefundResultDialog �Ի���

enum {
	REFUND_OK,
	REFUND_FAIL,
};

class CRefundResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CRefundResultDialog)

public:
	CRefundResultDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRefundResultDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_REFUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	bool m_refundState;
	CStatic m_pictureCtrl;
	CStatic m_refundResultCtrl;
	CStatic m_resultDescCtrl;
	CButton m_replayRefundBtn;
	afx_msg void OnBnClickedButtonReplayRefund();
};
