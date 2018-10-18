#include "stdafx.h"
#include "PrinterDevice.h"
static HANDLE hPort = INVALID_HANDLE_VALUE;
OVERLAPPED m_OverlappedRead;
OVERLAPPED m_OverlappedWrite;

DECLARE_SINGLETON_MEMBER(CPrinterDevice);

BOOL WriteCommByte(const char* ucByte, size_t len)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(hPort, (LPSTR)ucByte, len, &dwBytesWritten, &m_OverlappedWrite);
	DWORD nError = GetLastError();
	
	if (!bWriteStat && (nError == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_OverlappedWrite.hEvent, 1000)) { dwBytesWritten = 0; AfxMessageBox(L"WaitForSingleObject"); }
		else {
			GetOverlappedResult(hPort, &m_OverlappedWrite, &dwBytesWritten, FALSE);
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}
	else
	{
		CString str; 
		str.Format(L"WriteFilefail(%d)", nError);
		AfxMessageBox(str);
	}

	return(TRUE);

}

int AsynSendData(const char *buffer, int size)
{
	DWORD dwBytesWritten = 0;
	int i;
	for (i = 0; i<size; i++) {
		//WriteCommByte(buffer[i]);
		dwBytesWritten++;
	}
	WriteCommByte(buffer, size);
	return((int)dwBytesWritten);

}

CPrinterDevice::CPrinterDevice()
{
	do
	{
		CString portName = L"COM1";
		hPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		if (INVALID_HANDLE_VALUE == hPort)
		{
			int nError = GetLastError();
			CString err;
			err.Format(L"%s端口打开失败（%d）", portName.GetString(), nError);
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

		COMMTIMEOUTS CommTimeOuts;
		CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
		CommTimeOuts.ReadTotalTimeoutConstant = 0;
		CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
		CommTimeOuts.WriteTotalTimeoutConstant = 5000;
		SetCommTimeouts(hPort, &CommTimeOuts);

		//设定通讯端口通讯参数
		DCB dcb;
		BOOL bol = TRUE;
		
		//dcb.DCBlength = sizeof(dcb);
		bol = GetCommState(hPort, &dcb);
		dcb.BaudRate = 9600;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		dcb.Parity = NOPARITY;
		
		bol = SetCommState(hPort, &dcb); //配置串口
		// 清除通讯端口缓存
		PurgeComm(hPort, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);

		// 初始化重叠IO对象		
		HANDLE m_hStopCommEvent;
		HANDLE m_hDataReady;
		memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
		m_OverlappedRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
		m_OverlappedWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// 初始化事件对象
		m_hStopCommEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hDataReady = CreateEvent(NULL, FALSE, FALSE, NULL);

		char chSend[20];

		//初始化打印ESC @
		sprintf_s(chSend, "%c%c", 0x1B, 0x40);
		AsynSendData(chSend, 2);

		
		sprintf_s(chSend, "%c", 0x0A);
		AsynSendData(chSend, 1);
		AsynSendData(chSend, 1);
		AsynSendData(chSend, 1);
	} while (false);
	
}


CPrinterDevice::~CPrinterDevice()
{

	if (INVALID_HANDLE_VALUE != hPort)
		CloseHandle(hPort);
}
