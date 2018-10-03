// ResultPayDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ResultPayDialog.h"
#include "afxdialogex.h"
#include "resource.h"
#include "sellMobileSystem.h"
#include "commonMicro.h"

// CResultPayDialog �Ի���

IMPLEMENT_DYNAMIC(CResultPayDialog, CDialogEx)

CResultPayDialog::CResultPayDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RESULT_PAY, pParent)
{
	m_payResult = PAY_PAYING;
}

CResultPayDialog::~CResultPayDialog()
{
}

void CResultPayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICURE_PAY_OK, m_payOK_ctrl);

	CString imgPath = L"";
	switch (m_payResult)
	{
	case PAY_OK: {
		imgPath = L"res/payok.png";
		m_close_timer_ID = SetTimer(TIMER_ID_BUTTON_COUNTDOWN, 1000 * 1, NULL);
		m_countdownTime = 3;
		CImage image;
		image.Load(imgPath);
		HBITMAP hBmp = image.Detach();
		m_payOK_ctrl.SetBitmap(hBmp);
	}break;
	case PAY_FAIL: {
		imgPath = L"res/payfail.png";
		CImage image;
		image.Load(imgPath);
		HBITMAP hBmp = image.Detach();
		m_payOK_ctrl.SetBitmap(hBmp);
	}break;
	default:
		break;
	}

	

	DDX_Control(pDX, IDC_STATIC_PAY_FEE, m_payFeeCtrl);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btn);
	DDX_Control(pDX, IDC_STATIC_PAY_RESULT, m_payResultCtrl);
	DDX_Control(pDX, IDC_STATIC_PAYING, m_payingCtrl);
}


BEGIN_MESSAGE_MAP(CResultPayDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CResultPayDialog::OnBnClickedButtonClose)
	ON_MESSAGE(UM_ORDER_QUERY, &CResultPayDialog::OnOrderQuery)
	ON_MESSAGE(UM_PAY_SUCCESS_NOTIFY, &CResultPayDialog::OnPaySuccess)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CResultPayDialog ��Ϣ�������


BOOL CResultPayDialog::OnInitDialog()
{
	
	CDialogEx::OnInitDialog();


	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	switch (m_payResult)
	{
	case PAY_OK: {
		m_payingCtrl.ShowWindow(SW_HIDE);
	}break;
	case PAY_FAIL: {
		m_payingCtrl.ShowWindow(SW_HIDE);
		m_payResultCtrl.SetWindowText(L"�տ�ʧ��");
		m_payFeeCtrl.SetWindowText(L"ʧ��ԭ������ʱ");
		m_btn.SetWindowText(L"���²�ѯ");
	}break;
	case PAY_PAYING: {
		m_payResultCtrl.ShowWindow(SW_HIDE);
		m_payFeeCtrl.ShowWindow(SW_HIDE);
		m_btn.ShowWindow(SW_HIDE);
	}break;
	default:
		break;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


HBRUSH CResultPayDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	switch (m_payResult)
	{
	case PAY_OK: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PAY_FEE)
		{
			CFont font;
			font.CreatePointFont(120, _T("����"));
			pDC->SelectObject(&font);
		}
	}break;
	case PAY_PAYING: {
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PAYING)
		{
			CFont font;
			font.CreatePointFont(120, _T("����"));
			pDC->SelectObject(&font);
		}
	}break;
	default:
		break;
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	
	
	

	return hbr;
}


void CResultPayDialog::OnBnClickedButtonClose()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch (m_payResult)
	{
	case PAY_OK: {
	}break;
	case PAY_FAIL: {
		 
	}break;
	default:
		break;
	}
	::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
	
}


void CResultPayDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case TIMER_ID_ORDER_QUERY: {
		sellMobileSystemInstance->requestOrderQuery();
	}break;
	case TIMER_ID_DOWNLOAD_WAITING:
	{
		sellMobileSystemInstance->requestDownloadOrder();
	}break;
	case TIMER_ID_BUTTON_COUNTDOWN: {
		m_countdownTime = m_countdownTime - 1;
		CString str;
		str.Format(L"�ر�(%d)", m_countdownTime);
		m_btn.SetWindowText(str);
		if (m_countdownTime < 0)
		{
			KillTimer(m_close_timer_ID);
			::SendMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
		}
			
	}break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CResultPayDialog::requestOrder(const CString &orderNo, const CString &authCode)
{
	CStringA  strAOrderNoCode(orderNo);
	CStringA  strAAuthCode(authCode);
	sellMobileSystemInstance->requestMicropay(GetSafeHwnd(), strAOrderNoCode.GetString(), strAAuthCode.GetString());
}


LRESULT CResultPayDialog::OnOrderQuery(WPARAM wParam, LPARAM lParam)
{

	m_timer_orderQuery = SetTimer(TIMER_ID_ORDER_QUERY, 1000 * 3, NULL);

	sellMobileSystemInstance->requestOrderQuery();

	return 0;
}


LRESULT CResultPayDialog::OnPaySuccess(WPARAM wParam, LPARAM lParam)
{
	if (m_payResult == PAY_PAYING)
	{
		KillTimer(m_timer_orderQuery);
		m_payResult = PAY_OK;
	}
	


	return 0;
}
