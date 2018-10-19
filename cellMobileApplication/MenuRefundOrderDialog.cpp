// RefundOrderDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "MenuRefundOrderDialog.h"
#include "afxdialogex.h"
#include "RefundResultDialog.h"
#include "commonMicro.h"
#include "DataManager.h"
#include "PrinterDevice.h"
#include <fstream>
#include <iostream>

using namespace std;

// CMenuRefundOrderDialog 对话框

IMPLEMENT_DYNAMIC(CMenuRefundOrderDialog, CDialogEx)

CMenuRefundOrderDialog::CMenuRefundOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MENU_REFUND_ORDER, pParent)
{

}

CMenuRefundOrderDialog::~CMenuRefundOrderDialog()
{
}

void CMenuRefundOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ORDER_ID, m_orderNoCtrl);
	DDX_Control(pDX, IDC_EDIT_PAY_FEE, m_totalFeeCtrl);
	DDX_Control(pDX, IDC_EDIT_REFUND_FEE, m_feeCtrl);
}


BEGIN_MESSAGE_MAP(CMenuRefundOrderDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SURE, &CMenuRefundOrderDialog::OnBnClickedButtonSure)
END_MESSAGE_MAP()


// CMenuRefundOrderDialog 消息处理程序

BOOL CMenuRefundOrderDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽ESC关闭窗体/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::PostMessage(GetActiveWindow()->m_hWnd, UM_ESC_KEYBOARD_NOTIFY, 1, 0);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		::SendMessage(::GetActiveWindow(), WM_CLOSE, 0, 0);
		return TRUE;
	}
	//屏蔽回车关闭窗体,但会导致回车在窗体上失效.
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->wParam)
	{
		OnBnClickedButtonSure();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMenuRefundOrderDialog::OnBnClickedButtonSure()
{
	/*
	// TODO: 在此添加控件通知处理程序代码
	CRefundResultDialog dlg;
	CString orderNo;
	m_orderNoCtrl.GetWindowText(orderNo);

	std::string strGuid;
	DataMgrInstanceEx.guidToString(strGuid);
	CString refundNo(strGuid.c_str());

	CString totalfee;
	m_totalFeeCtrl.GetWindowText(totalfee);
	totalfee = L"1.0";
	if (!stringIsNumber(totalfee.GetString()))
	{
		MessageBox(L"原交易金额输入有误，请重新输入", L"提示");
		m_totalFeeCtrl.SetFocus();
		return;
	}

	CString fee;
	m_feeCtrl.GetWindowText(fee);
	if (!stringIsNumber(fee.GetString()))
	{
		MessageBox(L"退款金额输入有误，请重新输入", L"提示");
		m_feeCtrl.SetFocus();
		return;
	}
	dlg.requestRefundOrder(orderNo, refundNo, totalfee, fee);
	dlg.DoModal();
	*/

	//执行指令
	CString editStr;
	m_orderNoCtrl.GetWindowText(editStr);
	CStringA editStrA(editStr);

	int hookKeyboardLen = editStrA.GetLength();
	const char* hookKeyboard = editStrA.GetString();

	int numList[128];
	int numCnt = 0;
	char num[32] = { 0 };

	for (int n = 0; n<128; ++n){ numList[n] = 0; }

	if (nullptr != hookKeyboard)
	{
		int begin = 0, end = 0;
		while (hookKeyboardLen > end)
		{
			if (*(hookKeyboard + end) == ' ' && begin < end)
			{
				memcpy_s(num, 32, hookKeyboard + begin, end - begin);
				numList[numCnt++] = atoi(num);
				begin = end + 1;
				end = begin;
			}
			else
			{
				++end;
			}
		}
		memcpy_s(num, 32, hookKeyboard + begin, end - begin);
		numList[numCnt++] = atoi(num);
	}

	char printBuff[256] = {};

	for (int n = 0; n < numCnt; ++n)
	{
		wsprintfA(printBuff + strlen(printBuff), "%c", numList[n]);
	}

	int prinLen = strlen(printBuff);
	printerDeviceInstanceEx.printData(printBuff, prinLen);


	//打印数据
	m_totalFeeCtrl.GetWindowText(editStr);
	editStrA = editStr;
	sprintf_s(printBuff, "%s%c", editStrA.GetString(), 0x0A);
	
	prinLen = strlen(printBuff);
	printerDeviceInstanceEx.printData(printBuff, prinLen);
	

	//读取打印文本
	char filename[MAX_PATH * 2] = { 0 };
	GetModuleFileNameA(NULL, filename, MAX_PATH * 2);
	::PathRemoveFileSpecA(filename);
	wsprintfA(filename + strlen(filename), "\\testPrint.txt");

	ifstream infile(filename);
	if (!infile)
	{
		AfxMessageBox(L"file open error!");
		return ;
	}
	string line, key, value;
	int pos = 0;
	
	bool flag = false;
	while (getline(infile, line))
	{
		prinLen = line.length();
		const char* ch = line.c_str();
		printerDeviceInstanceEx.printData(ch, prinLen);
	}
	infile.close();
}

BOOL CMenuRefundOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifdef DEBUG_MODE	
	char randChar[32] = { 0 };
	long rd = getRandom(99999999);
	sprintf_s(randChar, "121775250120070233368%d", rd);
	std::string strOrder;
	std::string systemOrder;
	DataMgrInstanceEx.getGoodsInfoOrder(strOrder, systemOrder);
	m_orderNoCtrl.SetWindowText(CString(strOrder.c_str()));
	m_totalFeeCtrl.SetWindowText(CString("0.01"));
	m_feeCtrl.SetWindowText(CString("0.01"));
#endif

	CString csTotalFee;
	DataMgrInstanceEx.getGoodsInfoTotalFee(CString(systemOrder.c_str()), csTotalFee);
	m_totalFeeCtrl.SetWindowText(csTotalFee);
	m_feeCtrl.SetWindowText(csTotalFee);
	
	return true;
}