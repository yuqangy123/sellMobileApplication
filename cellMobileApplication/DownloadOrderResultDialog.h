#pragma once
#include "resource.h"
#include "afxwin.h"


enum {
	DOWNLOAD_OK,
	DOWNLOAD_FAIL,
	DOWNLOAD_DOWNLOADING,
};

#define TIMER_ID_DOWNLOADING_UPDATEUI		101
#define TIMER_ID_DOWNLOAD_REPEAT_QUREY		102
// CDownloadOrderResultDialog 对话框

class CDownloadOrderResultDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDownloadOrderResultDialog)

public:
	CDownloadOrderResultDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDownloadOrderResultDialog();
	void requestDownloader(const CString& beginDate, const CString& endDate, const CString& type);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RESULT_DOWNLOAD_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	void updateUI_DoDataExchange();
	void downloaderFail();
	void startDownloader();

public:
	CStatic m_picutreCtrl;
	CStatic m_downloadResultCtrl;
	CStatic m_downloadFileCtrl;
	CButton m_replayDownloadBtn;
	afx_msg void OnBnClickedButtonReplayDownload();
	afx_msg LRESULT OnDownloadRequestNotify(WPARAM wParam, LPARAM lParam);

public:
	int m_state;
	UINT_PTR m_timer_downloading_updateUI;
	int m_downloadingUpdateUI_countdown;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	CStringA m_beginDate;
	CStringA m_endDate;
	CStringA m_type;
	UINT_PTR m_timer_downloadWaiting;
	CStringA m_downloadFilePath;
};
