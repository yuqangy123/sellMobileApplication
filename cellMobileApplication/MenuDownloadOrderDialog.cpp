// MenuDownloadOrderDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MenuDownloadOrderDialog.h"
#include "afxdialogex.h"
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
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_payTimeCtrl);
	DDX_Control(pDX, IDC_COMBO_PAYTYPE, m_payTypeComboBox);
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
	m_payTimeCtrl.SetFocus();
}

BOOL CMenuDownloadOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	m_payTypeComboBox.AddString(L"���н���");
	m_payTypeComboBox.AddString(L"���˽���");
	m_payTypeComboBox.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CMenuDownloadOrderDialog::OnBnClickedButtonDownload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDownloadOrderResultDialog dlg;
	dlg.DoModal();
}
