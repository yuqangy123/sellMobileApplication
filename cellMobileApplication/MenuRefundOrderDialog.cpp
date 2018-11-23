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
	m_keyUpDownIndex = 1;
}

CMenuRefundOrderDialog::~CMenuRefundOrderDialog()
{
}

void CMenuRefundOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ORDER_ID, m_orderNoCtrl);
	DDX_Control(pDX, IDC_EDIT_PAY_FEE1, m_totalFeeCtrl);
	DDX_Control(pDX, IDC_EDIT_REFUND_SERIAL1, m_serialCtrl);
}


BEGIN_MESSAGE_MAP(CMenuRefundOrderDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SURE, &CMenuRefundOrderDialog::OnBnClickedButtonSure)
	ON_WM_SETFOCUS()
	ON_EN_SETFOCUS(IDC_EDIT_ORDER_ID, &CMenuRefundOrderDialog::OnSetfocusEditOrderId)
	ON_EN_SETFOCUS(IDC_EDIT_REFUND_SERIAL1, &CMenuRefundOrderDialog::OnSetfocusEditRefundSerial1)
	ON_EN_SETFOCUS(IDC_EDIT_PAY_FEE1, &CMenuRefundOrderDialog::OnSetfocusEditPayFee1)
END_MESSAGE_MAP()


void CMenuRefundOrderDialog::updateEditFocus(int n)
{
	
	m_keyUpDownIndex += n;

	if (m_keyUpDownIndex < 1)m_keyUpDownIndex = 3;
	if (m_keyUpDownIndex > 3)m_keyUpDownIndex = 1;

	switch (m_keyUpDownIndex)
	{
	case 1:{
			   m_orderNoCtrl.SetFocus();
	}break;
	case 2:{
			   m_serialCtrl.SetFocus();
	}break;
	case 3:{
			   m_totalFeeCtrl.SetFocus();
	}break;
	}
}

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
	//上下键盘
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN && pMsg->wParam)
	{
		updateEditFocus(1);
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP && pMsg->wParam)
	{
		updateEditFocus(-1);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMenuRefundOrderDialog::OnBnClickedButtonSure()
{
	

	// TODO: 在此添加控件通知处理程序代码
	CRefundResultDialog dlg;
	CString orderNo;
	m_orderNoCtrl.GetWindowText(orderNo);

	/*std::string strGuid;
	DataMgrInstanceEx.guidToString(strGuid);
	CString refundNo(strGuid.c_str());*/

	CString totalfee;
	m_totalFeeCtrl.GetWindowText(totalfee);
	if (!stringIsNumber(totalfee.GetString()))
	{
		MessageBox(L"退款金额输入有误，请重新输入", L"提示");
		m_totalFeeCtrl.SetFocus();
		return;
	}

	CString serialNo;
	m_serialCtrl.GetWindowText(serialNo);
	
	//serialNo中心流水，orderNo原单据号
	dlg.requestRefundOrder(serialNo, orderNo, m_outRefundNo, totalfee, totalfee);
	dlg.DoModal();
	

	/*
	//执行指令
	CString editStr;
	m_orderNoCtrl.GetWindowText(editStr);
	CStringA editStrA(editStr);

	int hookKeyboardLen = editStrA.GetLength();
	const char* hookKeyboard = editStrA.GetString();

	bool bPrintTestFile = true;
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
	if (prinLen>0)bPrintTestFile = false;

	//打印数据
	m_totalFeeCtrl.GetWindowText(editStr);
	editStrA = editStr;
	sprintf_s(printBuff, "%s%c", editStrA.GetString(), 0x0A);
	
	prinLen = strlen(printBuff);
	printerDeviceInstanceEx.printData(printBuff, prinLen);
	if (prinLen>1)bPrintTestFile = false;

	if (bPrintTestFile)
	{
		//读取打印文本
		char filename[MAX_PATH * 2] = { 0 };
		GetModuleFileNameA(NULL, filename, MAX_PATH * 2);
		::PathRemoveFileSpecA(filename);
		wsprintfA(filename + strlen(filename), "\\testPrint.txt");

		ifstream infile(filename);
		if (!infile)
		{
			AfxMessageBox(L"file open error!");
			return;
		}
		string line, key, value;
		int pos = 0;

		bool flag = false;
		while (getline(infile, line))
		{
			//const char* ch_ansi = KS_ANSI_to_UTF8(ch);
			sprintf_s(printBuff, "%s%c", line.c_str(), 0x0A);
			prinLen = strlen(printBuff);
			printerDeviceInstanceEx.printData(printBuff, prinLen);
		}
		for (int n = 0; n<3; ++n) printerDeviceInstanceEx.printData("\n", 1);

		infile.close();
	}	
	*/
}

BOOL CMenuRefundOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	return true;
}

void CMenuRefundOrderDialog::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
}

void CMenuRefundOrderDialog::updateOrderAndFee()
{

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
	m_keyUpDownIndex = 1;

	std::string order, systemOrder;
	DataMgrInstanceEx.getGoodsInfoOrder(order, systemOrder);
	m_outRefundNo = order.c_str();
	m_orderNoCtrl.SetWindowText(L"");
	m_serialCtrl.SetWindowText(L"");

	CString csTotalFee;
	DataMgrInstanceEx.getGoodsInfoTotalFee(CString(systemOrder.c_str()), csTotalFee);
	double dTotalFee = fabs( _wtof(csTotalFee.GetString()) );
	csTotalFee.Format(L"%.2f", dTotalFee);
	m_totalFeeCtrl.SetWindowText(csTotalFee);
	
}

void CMenuRefundOrderDialog::OnSetfocusEditOrderId()
{
	// TODO: Add your control notification handler code here
 	m_orderNoCtrl.SetSel(0, -1);
}


void CMenuRefundOrderDialog::OnSetfocusEditRefundSerial1()
{
	// TODO: Add your control notification handler code here
	m_serialCtrl.SetSel(0, -1);
}


void CMenuRefundOrderDialog::OnSetfocusEditPayFee1()
{
	// TODO: Add your control notification handler code here
	m_totalFeeCtrl.SetSel(0, -1);
}
