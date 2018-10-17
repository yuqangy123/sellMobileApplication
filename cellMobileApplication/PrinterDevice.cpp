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
			err.Format(L"COM1�˿ڴ�ʧ�ܣ�%d��", nError);
			AfxMessageBox(err);
			break;
		}

		//���ö˿ڻ���
		SetupComm(hPort, 1024, 1024);

		// �趨ͨѶ�˿ڳ�ʱ����
		COMMTIMEOUTS tmouts;
		tmouts.ReadIntervalTimeout = 100;
		tmouts.ReadTotalTimeoutMultiplier = 100;
		tmouts.ReadTotalTimeoutConstant = 100;
		tmouts.WriteTotalTimeoutConstant = 100;
		tmouts.WriteTotalTimeoutMultiplier = 100;
		SetCommTimeouts(hPort, &tmouts);

		//�趨ͨѶ�˿�ͨѶ����
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
		bol = SetCommState(hPort, &dcb); //���ô���
		// ���ͨѶ�˿ڻ���
		PurgeComm(hPort, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);

		// ��ʼ���ص�IO����
		OVERLAPPED m_OverlappedRead;
		OVERLAPPED m_OverlappedWrite;
		HANDLE m_hStopCommEvent;
		HANDLE m_hDataReady;
		memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
		m_OverlappedRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));
		m_OverlappedWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// ��ʼ���¼�����
		m_hStopCommEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hDataReady = CreateEvent(NULL, FALSE, FALSE, NULL);

		//��ʼ����ӡESC @
		DWORD iBytesLength;
		char chInitCode[] = "\\x0D\\x1B\\x40";
		WriteFile(hPort, chInitCode, (DWORD)3L, &iBytesLength, NULL);

		CloseHandle(hPort);
		AfxMessageBox(L"�ر�LPT1�˿�");
	} while (false);
	
}


CPrinterDevice::~CPrinterDevice()
{
}
