#include "stdafx.h"
#include "PrinterDevice.h"

DECLARE_SINGLETON_MEMBER(CPrinterDevice);
CPrinterDevice::CPrinterDevice()
{
	do
	{
		HANDLE hPort = CreateFile(L"COM1", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hPort)
		{
			int nError = GetLastError();
			CString err;
			err.Format(L"COM1端口打开失败（%d）", nError);
			AfxMessageBox(err);
			break;
		}

		//设置端口缓冲
		SetupComm(hPort, 1024, 1024);

		// 设定通讯端口超时参数
		COMMTIMEOUTS tmouts;
		tmouts.ReadIntervalTimeout = 100;
		tmouts.ReadTotalTimeoutMultiplier = 100;
		tmouts.ReadTotalTimeoutConstant = 100;
		tmouts.WriteTotalTimeoutConstant = 100;
		tmouts.WriteTotalTimeoutMultiplier = 100;
		SetCommTimeouts(hPort, &tmouts);

		//设定通讯端口通讯参数
		DCB dcb;
		BOOL bol = TRUE;
		/*
		//dcb.DCBlength = sizeof(dcb);
		bol = GetCommState(hPort, &dcb);
		dcb.BaudRate = device.BawdRate;
		dcb.ByteSize = device.DataBits;
		dcb.StopBits = device.StopBits;
		dcb.Parity = device.Parity;
		*/
		bol = SetCommState(hPort, &dcb); //配置串口
		// 清除通讯端口缓存
		PurgeComm(hPort, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);

		// 初始化重叠IO对象
		OVERLAPPED m_OverlappedRead;
		OVERLAPPED m_OverlappedWrite;
		HANDLE m_hStopCommEvent;
		HANDLE m_hDataReady;
		memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
		m_OverlappedRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
		m_OverlappedWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// 初始化事件对象
		m_hStopCommEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hDataReady = CreateEvent(NULL, FALSE, FALSE, NULL);

		//初始化打印ESC @
		DWORD iBytesLength;
		char chInitCode[] = "\\x0D\\x1B\\x40";
		WriteFile(hPort, chInitCode, (DWORD)3L, &iBytesLength, NULL);

		CloseHandle(hPort);
		AfxMessageBox(L"关闭LPT1端口");
	} while (false);
	
}


CPrinterDevice::~CPrinterDevice()
{
}
