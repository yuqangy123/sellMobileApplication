// RefundResultDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "RefundResultDialog.h"
#include "afxdialogex.h"


// CRefundResultDialog 对话框

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


// CRefundResultDialog 消息处理程序


HBRUSH CRefundResultDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_REFUND_DESC)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


BOOL CRefundResultDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	switch (m_refundState)
	{
	case REFUND_OK: {
		m_replayRefundBtn.ShowWindow(SW_HIDE);
		m_refundResultCtrl.SetWindowText(L"退款成功");
	}break;
	case REFUND_FAIL: {
		m_replayRefundBtn.ShowWindow(SW_SHOW);
		m_refundResultCtrl.SetWindowText(L"退款失败");
	}break;
	default:
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRefundResultDialog::OnBnClickedButtonReplayRefund()
{
	
	// TODO: 在此添加控件通知处理程序代码
}
