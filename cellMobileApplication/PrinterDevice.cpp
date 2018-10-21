#include "stdafx.h"
#include "PrinterDevice.h"
#include "DataManager.h"

static HANDLE hPort = INVALID_HANDLE_VALUE;
OVERLAPPED m_OverlappedRead;
OVERLAPPED m_OverlappedWrite;
const char print_Command = 0x0A;

DECLARE_SINGLETON_MEMBER(CPrinterDevice);

BOOL WriteCommByte(const char ucByte)
{
	
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	/*
	bWriteStat = WriteFile(hPort, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_OverlappedWrite);
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
		
	}
	*/
	if (0 == (WriteFile(hPort, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_OverlappedWrite)))
		return FALSE;

	return TRUE;
}

int AsynSendData(const char *buffer, int size)
{
	DWORD dwBytesWritten = 0;
	int i;
	for (i = 0; i<size; i++) {
		if (!WriteCommByte(buffer[i]))
		{
			AfxMessageBox(L"writefile 失败");
		}
		dwBytesWritten++;
	}
	//WriteCommByte(buffer, size);
	WriteCommByte(print_Command);
	++dwBytesWritten;
	return((int)dwBytesWritten);

}

//根据进程名获取进程PID
#include"tlhelp32.h"
#include <afx.h>
#include <winternl.h>
typedef struct FILE_BASIC_INFORMATION {
	LARGE_INTEGER  CreationTime;
	LARGE_INTEGER  LastAccessTime;
	LARGE_INTEGER  LastWriteTime;
	LARGE_INTEGER  ChangeTime;
	ULONG  FileAttributes; //文件属性
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;


HANDLE GetProcessIDFromName(wchar_t *name)
{
	HANDLE hSanpshot;
	hSanpshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSanpshot)
	{
		return NULL;
	}

	PROCESSENTRY32 pe;
	BOOL bOk;
	pe.dwSize = sizeof(pe);

	bOk = Process32First(hSanpshot, &pe);
	if (!bOk)
		return NULL;

	do {
		if (!wcscmp(pe.szExeFile, name))
		{
			return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		}
		bOk = Process32Next(hSanpshot, &pe);
	} while (bOk);

	return NULL;
}

unsigned __stdcall thread(void * lpPragma)
{
	while (1)
	{
		Sleep(500);
	}

	return 0;
}


typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength;
WCHAR FileName[MAX_PATH];
}FILE_NAME_INFORMATION,*PFILE_NAME_INFORMATION;

typedef LONG (CALLBACK* ZWQUERYINFORMATIONFILE)(
	HANDLE FileHandle,
	IO_STATUS_BLOCK *IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	ULONG FileInformationClass);

CPrinterDevice::CPrinterDevice()
{
	hPort = INVALID_HANDLE_VALUE;
	HANDLE sellSystemPID = GetProcessIDFromName(L"SellSystem.exe");

	HANDLE hThread;
	HANDLE hTarget;
	hThread = (HANDLE)_beginthreadex(NULL, 0, thread, NULL, 0, NULL);
	//进程间复制句柄
	if (DuplicateHandle((HANDLE)sellSystemPID, hThread, GetCurrentProcess(),
		&hTarget, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		hPort = hTarget;
		AfxMessageBox(L"获取文件句柄成功");

		//查询文件基本信息
		HMODULE hNtDLL = LoadLibraryW(L"ntdll.dll");
		if (hNtDLL == 0x00)
		{
			AfxMessageBox(L"loadLibrary ntdll.dll fail");
			return;
		}
		ZWQUERYINFORMATIONFILE ZwQueryInformationFile = (ZWQUERYINFORMATIONFILE)GetProcAddress(hNtDLL, "ZwQueryInformationFile");
		if (ZwQueryInformationFile == 0x00) 
		{ 
			AfxMessageBox(L"get ZwQueryInformationFile fail");
			return;
		}
		FILE_NAME_INFORMATION fni;
		IO_STATUS_BLOCK isb;
		const int FileNameInformation = 9;  // enum FILE_INFORMATION_CLASS; Defined in winddk.h
		NTSTATUS status = ZwQueryInformationFile(hTarget, &isb, &fni, sizeof(FILE_BASIC_INFORMATION), FileNameInformation);
		if (NT_SUCCESS(status))
		{
			fni.FileName[fni.FileNameLength / sizeof(WCHAR)] = 0x00;
		}
		
	}

}


CPrinterDevice::~CPrinterDevice()
{
	closeDevice();
}

void CPrinterDevice::closeDevice()
{
	return ;//test code
	if (INVALID_HANDLE_VALUE != hPort)
		CloseHandle(hPort);
	hPort = INVALID_HANDLE_VALUE;
}

bool CPrinterDevice::initDevice()
{
	return true;//test code
	do
	{
		if (INVALID_HANDLE_VALUE != hPort)
			//return true;
			break;

		CString portName = L"COM1";
		if (DataMgrInstanceEx.PrintPortName.length() > 0)
			portName = DataMgrInstanceEx.PrintPortName.c_str();
		
		//hPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
		hPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hPort)
		{
			int nError = GetLastError();
			CString err;
			err.Format(L"%s端口打开失败（%d）", portName.GetString(), nError);
			AfxMessageBox(err);
			return false;
		}

		//设置端口缓冲
		//SetupComm(hPort, 1024, 1024);

		// 设定通讯端口超时参数
		COMMTIMEOUTS CommTimeOuts;
		CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
		CommTimeOuts.ReadTotalTimeoutConstant = 0;
		CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
		CommTimeOuts.WriteTotalTimeoutConstant = 20;
		CommTimeOuts.WriteTotalTimeoutMultiplier = 5000;
		SetCommTimeouts(hPort, &CommTimeOuts);

		//设定通讯端口通讯参数
		DCB dcb;
		BOOL bol = TRUE;

		dcb.DCBlength = sizeof(dcb);
		bol = GetCommState(hPort, &dcb);
		if (bol == 0)
		{
			int nError = GetLastError();
			CString err;
			err.Format(L"%s串口信息获取失败（%d）", portName.GetString(), nError);
			//AfxMessageBox(err);
		}
		dcb.BaudRate = 9600;
		dcb.ByteSize = 8;
		dcb.StopBits = ONESTOPBIT;
		dcb.Parity = NOPARITY;

		bol = SetCommState(hPort, &dcb); //配置串口
		if (bol == 0)
		{
			int nError = GetLastError();
			CString err;
			err.Format(L"%s设置串口信息失败（%d）", portName.GetString(), nError);
			//AfxMessageBox(err);
		}

		// 清除通讯端口缓存
		PurgeComm(hPort, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);

		/*
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
		*/

		char chSend[20];

		//初始化打印ESC @
		sprintf_s(chSend, "%c%c", 0x1B, 0x40);
		AsynSendData(chSend, 2);


		//半切
		sprintf_s(chSend, "%c%c", 0x1B, 0x69);
		AsynSendData(chSend, 2);

		/*
		sprintf_s(chSend, "%c", 0x0A);
		AsynSendData(chSend, 1);
		*/
	} while (false);

	return true;
}

void CPrinterDevice::printData(const char* data, int len)
{
	AsynSendData(data, len);
}

bool CPrinterDevice::printPayOrder(const char* tradeType, const char* orderNo, const char* tradeNo, const char* fee,
	const char* date, const char* time)
{
	if (!initDevice())
		return false;

	
	if (0 == strcmp(tradeType, "WXPAY.MICROPAY"))
	{
		const char* printBuff = "	微信签购单";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}
	else if (0 == strcmp(tradeType, "ALIPAY.MICROPAY"))
	{
		const char* printBuff = "	支付宝签购单";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}
	else
	{
		return false;
	}


	{
		const char* printBuff = "交易类型：支付";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}


	{
		char printBuff[128];
		sprintf_s(printBuff, "部门编码：%s", DataMgrInstanceEx.NodeCode.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "终端号：%s", DataMgrInstanceEx.TerminalCode.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "单据号：%s", tradeNo);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "中心流水：%s", orderNo);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		float nfee = atof(fee);
		nfee = nfee / 100;
		sprintf_s(printBuff, "交易金额：%.2f", nfee);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "交易日期：%s", date);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "交易时间：%s", time);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	for(int n=0;n<3;++n)printerDeviceInstanceEx.printData("", 0);

	closeDevice();

	return true;
}

bool CPrinterDevice::printRefundOrder(const refundOrderInfo* info)
{
	if (!initDevice())
		return false;


	if (0 == strcmp(info->tradeType.c_str(), "WXPAY.MICROPAY"))
	{
		const char* printBuff = "	微信签购单";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}
	else if (0 == strcmp(info->tradeType.c_str(), "ALIPAY.MICROPAY"))
	{
		const char* printBuff = "	支付宝签购单";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}
	else
	{
		return false;
	}


	{
		const char* printBuff = "交易类型：退货";
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}


	{
		char printBuff[128];
		sprintf_s(printBuff, "部门编码：%s", DataMgrInstanceEx.NodeCode.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "终端号：%s", DataMgrInstanceEx.TerminalCode.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "单据号：%s", info->refundOrderNo.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "中心流水：%s", info->tradeNo.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		float nfee = atof(info->fee.c_str());
		nfee = nfee / 100;
		sprintf_s(printBuff, "交易金额：%.2f", nfee);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	SYSTEMTIME system;
	GetLocalTime(&system);
	{
		char printBuff[128];
		sprintf_s(printBuff, "交易日期：%04d%02d%02d", system.wYear, system.wMonth, system.wDay);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "交易时间：%02d%02d%02d", system.wHour, system.wMinute, system.wSecond);
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "原交易日期：%s", info->payDate.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	{
		char printBuff[128];
		sprintf_s(printBuff, "原单据号：%s", info->payOrderNo.c_str());
		int prinLen = strlen(printBuff);
		printerDeviceInstanceEx.printData(printBuff, prinLen);
	}

	for (int n = 0; n<3; ++n)printerDeviceInstanceEx.printData("", 0);

	closeDevice();

	return true;
}

bool CPrinterDevice::checkDevice()
{
	bool ret = initDevice();
	closeDevice();
	return ret;
}