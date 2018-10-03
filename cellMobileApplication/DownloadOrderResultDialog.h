#pragma once
#include "resource.h"
#include "afxwin.h"


enum {
	DOWNLOAD_OK,
	DOWNLOAD_FAIL,
	DOWNLOAD_DOWNLOADING,
};

// CDownloadOrderResultDialog �Ի���

class CDownloadOrderResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDownloadOrderResultDialog)

public:
	CDownloadOrderResultDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDownloadOrderResultDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_DOWNLOAD_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picutreCtrl;
	CStatic m_downloadResultCtrl;
	CStatic m_downloadFileCtrl;
	CButton m_replayDownloadBtn;
	afx_msg void OnBnClickedButtonReplayDownload();
	

public:
	int m_state;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
