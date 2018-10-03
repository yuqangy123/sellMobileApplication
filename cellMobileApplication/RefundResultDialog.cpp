// RefundResultDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RefundResultDialog.h"
#include "afxdialogex.h"


// CRefundResultDialog �Ի���

IMPLEMENT_DYNAMIC(CRefundResultDialog, CDialogEx)

CRefundResultDialog::CRefundResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_REFUND, pParent)
{

}

CRefundResultDialog::~CRefundResultDialog()
{
}

void CRefundResultDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_REFUND_OK, m_pictureCtrl);
	DDX_Control(pDX, IDC_STATIC_REFUND_RESULT, m_refundResultCtrl);
	DDX_Control(pDX, IDC_STATIC_REFUND_DESC, m_resultDescCtrl);
	DDX_Control(pDX, IDC_BUTTON_REPLAY_REFUND, m_replayRefundBtn);

	CString imgPath = L"";
	switch (m_refundState)
	{
	case REFUND_OK: {
		CImage image;
		image.Load(L"res/payok.png");
		HBITMAP hBmp = image.Detach();
		m_pictureCtrl.SetBitmap(hBmp);
	}break;
	case REFUND_FAIL: {
		CImage image;
		image.Load(L"res/payfail.png");
		HBITMAP hBmp = image.Detach();
		m_pictureCtrl.SetBitmap(hBmp);
	}break;
	default:
		break;
	}
}


BEGIN_MESSAGE_MAP(CRefundResultDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_REPLAY_REFUND, &CRefundResultDialog::OnBnClickedButtonReplayRefund)
END_MESSAGE_MAP()


// CRefundResultDialog ��Ϣ�������


HBRUSH CRefundResultDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_REFUND_DESC)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


BOOL CRefundResultDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	switch (m_refundState)
	{
	case REFUND_OK: {
		m_replayRefundBtn.ShowWindow(SW_HIDE);
		m_refundResultCtrl.SetWindowText(L"�˿�ɹ�");
	}break;
	case REFUND_FAIL: {
		m_replayRefundBtn.ShowWindow(SW_SHOW);
		m_refundResultCtrl.SetWindowText(L"�˿�ʧ��");
	}break;
	default:
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CRefundResultDialog::OnBnClickedButtonReplayRefund()
{
	
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
