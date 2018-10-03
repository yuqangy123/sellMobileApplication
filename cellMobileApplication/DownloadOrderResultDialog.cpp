// DownloadOrderResultDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DownloadOrderResultDialog.h"
#include "afxdialogex.h"


// CDownloadOrderResultDialog �Ի���

IMPLEMENT_DYNAMIC(CDownloadOrderResultDialog, CDialogEx)

CDownloadOrderResultDialog::CDownloadOrderResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_DOWNLOAD_ORDER, pParent)
{
	m_state = DOWNLOAD_FAIL;
}

CDownloadOrderResultDialog::~CDownloadOrderResultDialog()
{
}

void CDownloadOrderResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_PAY_OK, m_picutreCtrl);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_OK, m_downloadResultCtrl);
	DDX_Control(pDX, IDC_STATIC_DOWNLOAD_FILEPATH, m_downloadFileCtrl);
	DDX_Control(pDX, IDC_BUTTON_REPLAY_DOWNLOAD, m_replayDownloadBtn);

	CString imgPath = L"";
	switch (m_state)
	{
	case DOWNLOAD_OK: {
		m_replayDownloadBtn.ShowWindow(SW_HIDE);
		CImage image;
		image.Load(L"res/payok.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_downloadResultCtrl.SetWindowTextW(L"�����ɹ�");
		m_downloadFileCtrl.SetWindowText(L"�ļ�����xxx.rar\r\n����Ŀ¼��C://Desktop");

	}break;
	case DOWNLOAD_FAIL: {
		CImage image;
		image.Load(L"res/payfail.png");
		HBITMAP hBmp = image.Detach();
		m_picutreCtrl.SetBitmap(hBmp);
		m_downloadFileCtrl.ShowWindow(SW_HIDE);
		m_downloadResultCtrl.SetWindowTextW(L"����ʧ��");
	}break;
	default:
		break;
	}
}


BEGIN_MESSAGE_MAP(CDownloadOrderResultDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_REPLAY_DOWNLOAD, &CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDownloadOrderResultDialog ��Ϣ�������


void CDownloadOrderResultDialog::OnBnClickedButtonReplayDownload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
}


BOOL CDownloadOrderResultDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDownloadOrderResultDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}




HBRUSH CDownloadOrderResultDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	switch (m_state)
	{
	case DOWNLOAD_OK: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_DOWNLOAD_FILEPATH)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
		}
	}break;
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	// TODO:  �ڴ˸��� DC ���κ�����

	return hbr;
}
