// RefundResultDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RefundResultDialog.h"
#include "afxdialogex.h"
#include "commonMicro.h"
#include "sellMobileSystem.h"
#include "PrinterDevice.h"

// CRefundResultDialog �Ի���

IMPLEMENT_DYNAMIC(CRefundResultDialog, CDialogEx)

CRefundResultDialog::CRefundResultDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_REFUND, pParent)
{
	m_info = nullptr;
	m_refundState = REFUND_REFUNDING;
	gif_init_member()
}

CRefundResultDialog::~CRefundResultDialog()
{
	gif_destory_member()
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
	ON_MESSAGE(UM_PAY_SUCCESS_NOTIFY, &CRefundResultDialog::OnOrderQuery)
	ON_MESSAGE(UM_REFUND_QUERY_ORDER_TIME_NOTIFY, &CRefundResultDialog::OnOrderQuery)
	
	ON_WM_TIMER()
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
	updateUI_OnInitDialog();
	sellMobileSystemInstance->requestRefundOrder(GetSafeHwnd(), m_orderNo.GetString(), m_outOrderNo.GetString(), m_outRefundNo.GetString(), m_totalfee.GetString(), m_fee.GetString());

	gif_loadGif(L"res//loading.gif", 100);
	gif_show(true);

	m_pictureCtrl.GetWindowRect(&m_gifrt); //���window����
	ScreenToClient(&m_gifrt); //ת��client
	m_gifrt.bottom -= 15.f;
	m_gifrt.right -= 15.f;

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
		gif_show(false);
	}break;
	case REFUND_FAIL: {
		m_replayRefundBtn.ShowWindow(SW_SHOW);
		m_replayRefundBtn.SetFocus();
		m_refundResultCtrl.SetWindowText(L"�˿�ʧ��");
		m_resultDescCtrl.SetWindowText(m_desc);
		gif_show(false);
	}break;

	case REFUND_REFUNDING: {
		m_replayRefundBtn.ShowWindow(SW_HIDE);
		m_replayRefundBtn.SetFocus();
		m_refundResultCtrl.SetWindowText(L"�˿���...");
		m_resultDescCtrl.SetWindowText(L"");
		gif_show(true);
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
		m_pictureCtrl.ShowWindow(SW_SHOW);
	}break;
	case REFUND_FAIL: {
		CImage image;
		image.Load(L"res/payfail.png");
		HBITMAP hBmp = image.Detach();
		m_pictureCtrl.SetBitmap(hBmp);
		m_pictureCtrl.ShowWindow(SW_SHOW);
	}break;
	case REFUND_REFUNDING: {
		m_pictureCtrl.ShowWindow(SW_HIDE);
	}break;
	default:
		break;
	}
}

void CRefundResultDialog::OnBnClickedButtonReplayRefund()
{
	m_refundState = REFUND_REFUNDING;
	updateUI_OnInitDialog();
	sellMobileSystemInstance->requestRefundOrder(GetSafeHwnd(), m_orderNo.GetString(), m_outOrderNo.GetString(), m_outRefundNo.GetString(), m_totalfee.GetString(), m_fee.GetString());
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		m_refundState = REFUND_FAIL;
		updateUI_OnInitDialog();
		safe_delete(retmsg);
	}break;
	case 1: {
		m_info = (refundOrderInfo*)lParam;
		
		m_timer_orderQuery = SetTimer(TIMER_ID_ORDER_QUERY, 1000 * 3, NULL);
		sellMobileSystemInstance->requestOrderQuery(GetSafeHwnd(), m_orderNo.GetString());
	}break;
	}

	return 0;
}

void CRefundResultDialog::requestRefundOrder(const CString& order_no, const CString& out_order_no, const CString& out_refund_no, const CString& totalfee, const CString& fee)
{
	m_orderNo = order_no;
	m_outOrderNo = out_order_no;
	m_outRefundNo = out_refund_no;
	m_totalfee = totalfee;
	m_fee = fee;
}

void CRefundResultDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	gif_draw_timer(m_gifrt.right, m_gifrt.bottom);
	switch (nIDEvent)
	{
	case TIMER_ID_ORDER_QUERY: {
		sellMobileSystemInstance->requestOrderQuery(GetSafeHwnd(), m_orderNo.GetString());
	}break;
	}
	

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CRefundResultDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		OnBnClickedButtonReplayRefund();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CRefundResultDialog::OnOrderQuery(WPARAM wParam, LPARAM lParam)
{
	KillTimer(m_timer_orderQuery);
	if (0 == wParam)
	{
		payOrderInfo* info = (payOrderInfo*)lParam;
		
		m_refundState = REFUND_OK;
		updateUI_OnInitDialog();

		if (m_info)
		{
			m_info->tradeType = info->tradeType;
			m_info->payDate = info->date;
		}

		printerDeviceInstanceEx.printRefundOrder(m_info);

		safe_delete(m_info);
		safe_delete(info);
	}
	else if (-1 == wParam)
	{
		OnRefundOrderNotify(-1, lParam);

		safe_delete(m_info);
	}


	return 0;
}