// RefundResultDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RefundResultDialog.h"
#include "afxdialogex.h"
#include "commonMicro.h"
#include "sellMobileSystem.h"

// CRefundResultDialog �Ի���

IMPLEMENT_DYNAMIC(CRefundResultDialog, CDialogEx)

CRefundResultDialog::CRefundResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_REFUND, pParent)
{
	m_refundState = REFUND_REFUNDING;
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

	
}


BEGIN_MESSAGE_MAP(CRefundResultDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_REPLAY_REFUND, &CRefundResultDialog::OnBnClickedButtonReplayRefund)
	ON_MESSAGE(UM_REFUND_ORDER_NOTIFY, &CRefundResultDialog::OnRefundOrderNotify)
END_MESSAGE_MAP()


// CRefundResultDialog ��Ϣ��������


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

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��
	updateUI_OnInitDialog();
	sellMobileSystemInstance->requestRefundOrder(GetSafeHwnd(), m_orderNo.GetString(), m_refundNo.GetString(), m_fee.GetString());


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

void CRefundResultDialog::updateUI_OnInitDialog()
{
	switch (m_refundState)
	{
	case REFUND_OK: {
		m_replayRefundBtn.ShowWindow(SW_HIDE);
		m_refundResultCtrl.SetWindowText(L"�˿�ɹ�");
		m_resultDescCtrl.SetWindowText(L"�����ѹ˿�ע�����");
	}break;
	case REFUND_FAIL: {
		m_replayRefundBtn.ShowWindow(SW_SHOW);
		m_refundResultCtrl.SetWindowText(L"�˿�ʧ��");
		m_resultDescCtrl.SetWindowText(m_desc);
	}break;

	case REFUND_REFUNDING: {
		m_replayRefundBtn.ShowWindow(SW_HIDE);
		m_refundResultCtrl.SetWindowText(L"�˿���...");
		m_resultDescCtrl.SetWindowText(L"");
	}break;
	default:
		break;
	}

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

void CRefundResultDialog::OnBnClickedButtonReplayRefund()
{
	m_refundState = REFUND_REFUNDING;
	updateUI_OnInitDialog();
	sellMobileSystemInstance->requestRefundOrder(GetSafeHwnd(), m_orderNo.GetString(), m_refundNo.GetString(), m_fee.GetString());
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
}
LRESULT CRefundResultDialog::OnRefundOrderNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 0: {
		m_refundState = REFUND_REFUNDING;
		updateUI_OnInitDialog();
	}break;
	case -1: {
		char* retmsg = (char*)lParam;
		m_desc = retmsg;
		safe_delete(retmsg);
		m_refundState = REFUND_FAIL;
		updateUI_OnInitDialog();
	}break;
	case 1: {
		m_refundState = REFUND_OK;
		updateUI_OnInitDialog();
	}break;
	}

	return 0;
}

void CRefundResultDialog::requestRefundOrder(const CString& order_no, const CString& refund_no, const CString& fee)
{
	m_orderNo = order_no;
	m_refundNo = refund_no;
	m_fee = fee;
}