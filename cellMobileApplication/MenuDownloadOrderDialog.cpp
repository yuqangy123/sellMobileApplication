// MenuDownloadOrderDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MenuDownloadOrderDialog.h"
#include "afxdialogex.h"
#include "DownloadOrderResultDialog.h"
#include "DownloadOrderResultDialog.h"

// CMenuDownloadOrderDialog �Ի���

IMPLEMENT_DYNAMIC(CMenuDownloadOrderDialog, CDialogEx)

CMenuDownloadOrderDialog::CMenuDownloadOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_DOWNLOAD_ORDER, pParent)
{

}

CMenuDownloadOrderDialog::~CMenuDownloadOrderDialog()
{
}

void CMenuDownloadOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PAYTYPE, m_payTypeComboBox);
	DDX_Control(pDX, IDC_DATETIMEPICKER_BEGIN, m_beginDataTimePicker);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_endDataTimePicker);
}


BEGIN_MESSAGE_MAP(CMenuDownloadOrderDialog, CDialogEx)
	ON_WM_CHAR()
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CMenuDownloadOrderDialog::OnBnClickedButtonDownload)
END_MESSAGE_MAP()


// CMenuDownloadOrderDialog ��Ϣ�������


void CMenuDownloadOrderDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CMenuDownloadOrderDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//����ESC�رմ���/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return TRUE;
	}
	//���λس��رմ���,���ᵼ�»س��ڴ�����ʧЧ.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMenuDownloadOrderDialog::setTimeControlFocus()
{
	m_beginDataTimePicker.SetFocus();
}

BOOL CMenuDownloadOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	m_payTypeComboBox.AddString(L"���н���");
	m_payTypeComboBox.AddString(L"֧���ɹ��Ľ���");
	m_payTypeComboBox.AddString(L"�˿�Ľ���");
	m_payTypeComboBox.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

#include "QRCodePayDialog.h"
void CMenuDownloadOrderDialog::OnBnClickedButtonDownload()
{
#if 1
	CQRCodePayDialog dlgtest;
	dlgtest.DoModal();
	return;
#endif

	CTime timeBegin;
	CString startDate;
	m_beginDataTimePicker.GetTime(timeBegin);
	startDate.Format(L"%04d%02d%02d", timeBegin.GetYear(), timeBegin.GetMonth(), timeBegin.GetDay());

	CTime timeEnd;
	CString endDate;
	m_endDataTimePicker.GetTime(timeEnd);
	endDate.Format(L"%04d%02d%02d", timeEnd.GetYear(), timeEnd.GetMonth(), timeEnd.GetDay());

	int selectPayType = m_payTypeComboBox.GetCurSel() + 1;
	CString payType;
	payType.Format(L"%d", selectPayType);

	CDownloadOrderResultDialog dlg;
	dlg.requestDownloader(startDate, endDate, payType);
	dlg.DoModal();

	
}
