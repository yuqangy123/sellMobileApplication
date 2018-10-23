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
#include <psapi.h>
const int FileNameInformation = 9;  // enum FILE_INFORMATION_CLASS; Defined in winddk.h
const int SystemHandleInformation = 16;

typedef struct FILE_BASIC_INFORMATION {
	LARGE_INTEGER  CreationTime;
	LARGE_INTEGER  LastAccessTime;
	LARGE_INTEGER  LastWriteTime;
	LARGE_INTEGER  ChangeTime;
	ULONG  FileAttributes; //文件属性
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;


DWORD GetProcessIDFromName(wchar_t *name)
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
			//return OpenProcess(PROCESS_DUP_HANDLE, FALSE, pe.th32ProcessID);
			return pe.th32ProcessID;
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

typedef struct _NM_INFO
{
	HANDLE  hFile;
	FILE_NAME_INFORMATION Info;
	WCHAR Name[MAX_PATH];
} NM_INFO, *PNM_INFO;

typedef LONG (CALLBACK* ZWQUERYINFORMATIONFILE)(
	HANDLE FileHandle,
	IO_STATUS_BLOCK *IoStatusBlock,
	PVOID FileInformation,
	ULONG Length,
	ULONG FileInformationClass);

typedef NTSTATUS(WINAPI *ZWQUERYSYSTEMINFORMATION)(unsigned long, PVOID, ULONG, PULONG);


NTSTATUS ZwQuerySystemInformation(
IN int SystemInformationClass,
OUT PVOID SystemInformation,
IN ULONG SystemInformationLength,
OUT PULONG ReturnLength OPTIONAL
);
/*
typedef struct _SYSTEM_HANDLE
{
	ULONG  uIdProcess;
	UCHAR  ObjectType;    // OB_TYPE_* (OB_TYPE_TYPE, etc.)
	UCHAR  Flags;         // HANDLE_FLAG_* (HANDLE_FLAG_INHERIT, etc.)
	USHORT  Handle;
	PVOID  pObject;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG   uCount;
	SYSTEM_HANDLE aSH[];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
*/
#define STATUS_INFO_LENGTH_MISMATCH  ((NTSTATUS)0xC0000004L)

HANDLE hHeap;
DWORD WINAPI GetFileNameThread(PVOID lpParameter)
{
	PNM_INFO NmInfo = (PNM_INFO)lpParameter;
	IO_STATUS_BLOCK IoStatus;
	HMODULE hNtDLL = LoadLibrary(L"NTDLL.DLL");
	if (!hNtDLL)
	{
		return FALSE;
	}
	ZWQUERYINFORMATIONFILE ZwQueryInformationFile = (ZWQUERYINFORMATIONFILE)GetProcAddress(hNtDLL, "ZwQueryInformationFile");
	if (ZwQueryInformationFile == NULL)
	{
		return FALSE;
	}
	ZwQueryInformationFile(NmInfo->hFile, &IoStatus, &NmInfo->Info, sizeof(NM_INFO)-sizeof(HANDLE), FileNameInformation);

	if (NmInfo->Info.FileNameLength > 0)
	{
		NmInfo->Info.FileName[NmInfo->Info.FileNameLength/2] = 0;
		CStringA strA(NmInfo->Info.FileName);
		DataMgrInstanceEx.writeLog(strA.GetString());
	}
	
	return 0;
}

void GetFileName(HANDLE hFile, PCHAR TheName)
{
	HANDLE   hThread;
	PNM_INFO Info = (PNM_INFO)HeapAlloc(hHeap, 0, sizeof(NM_INFO));
	Info->hFile = hFile;
	Info->Info.FileNameLength = 0;
	hThread = CreateThread(NULL, 0, GetFileNameThread, Info, 0, NULL);
	if (WaitForSingleObject(hThread, INFINITE) == WAIT_TIMEOUT) TerminateThread(hThread, 0);
	CloseHandle(hThread);
	memset(TheName, 0, MAX_PATH);
	WideCharToMultiByte(CP_ACP, 0, Info->Info.FileName, Info->Info.FileNameLength >> 1, TheName, MAX_PATH, NULL, NULL);
	HeapFree(hHeap, 0, Info);
}

PVOID GetInfoTable(IN ULONG ATableType)
{
	ULONG    mSize = 0x8000;
	PVOID    mPtr;
	NTSTATUS status;
	do
	{
		mPtr = HeapAlloc(hHeap, 0, mSize);
		if (!mPtr) return NULL;
		memset(mPtr, 0, mSize);

		HMODULE hNtDLL = LoadLibrary(L"NTDLL.DLL");
		if (!hNtDLL)
		{
			return FALSE;
		}

		ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hNtDLL, "ZwQuerySystemInformation");
		if (ZwQuerySystemInformation == NULL)
		{
			return FALSE;
		}

		status = ZwQuerySystemInformation(ATableType, mPtr, mSize, NULL);

		if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			HeapFree(hHeap, 0, mPtr);
			mSize = mSize * 2;
		}
	} while (status == STATUS_INFO_LENGTH_MISMATCH);
	if (NT_SUCCESS(status)) return mPtr;
	HeapFree(hHeap, 0, mPtr);
	return NULL;
}

void MyCloseHandle(DWORD pid)
{
	/*
	//获取进程中的句柄
	PSYSTEM_HANDLE_INFORMATION Info;
	ULONG r;
	CHAR Name[MAX_PATH];
	HANDLE hProcess, hFile;
	hHeap = GetProcessHeap();
	Info = (PSYSTEM_HANDLE_INFORMATION)GetInfoTable(SystemHandleInformation);
	if (Info)
	{
		for (r = 0; r < Info->uCount; r++)
		{
			
			if (Info->aSH[r].uIdProcess == pid)
			{
				hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, Info->aSH[r].uIdProcess);
				if (hProcess)
				{
					if (DuplicateHandle(hProcess, (HANDLE)Info->aSH[r].Handle, GetCurrentProcess(), &hFile, 0, FALSE, DUPLICATE_SAME_ACCESS))
					{
						GetFileName(hFile, Name);
						
						if (Name[0] != 0)
						{
							hPort = hFile;//test code
							AfxMessageBox(CString(Name));
							AsynSendData("A", 1);
						}
						

						if (strstr(Name, "Intel") != NULL)
						{
							printf("PID=%d FileHandle %d FileName=%s\n", Info->aSH[r].uIdProcess, Info->aSH[r].Handle, Name);
						}

						CloseHandle(hFile);
					}
					CloseHandle(hProcess);
				}
			}
		}
		HeapFree(hHeap, 0, Info);
	}
	printf("Duplicate Finish.\n");
	*/
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// This makro assures that INVALID_HANDLE_VALUE (0xFFFFFFFF) returns FALSE
#define IsConsoleHandle(h) (((((ULONG_PTR)h) & 0x10000003) == 0x3) ? TRUE : FALSE)
#define SystemHandleInformation (SystemProcessInformation + 11)
#define ObjectNameInformation (ObjectTypeInformation - 1)

struct OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name; // defined in winternl.h
	TCHAR NameBuffer;
};

typedef NTSTATUS(NTAPI* PNtQueryObject)(HANDLE Handle, OBJECT_INFORMATION_CLASS Info, PVOID Buffer, ULONG BufferSize, PULONG ReturnLength);

typedef LONG NTSTATUS;

typedef struct _SYSTEM_HANDLE
{
	DWORD       dwProcessId;
	BYTE        bObjectType;
	BYTE        bFlags;
	WORD        wValue;
	PVOID       pAddress;
	DWORD GrantedAccess;
}SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	DWORD         dwCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION, **PPSYSTEM_HANDLE_INFORMATION;

typedef NTSTATUS(WINAPI *PNtQuerySystemInformation)
(IN    SYSTEM_INFORMATION_CLASS SystemInformationClass,
OUT    PVOID                     SystemInformation,
IN    ULONG                     SystemInformationLength,
OUT    PULONG                     ReturnLength OPTIONAL);

// returns
// "\Device\HarddiskVolume3"                                (Harddisk Drive)
// "\Device\HarddiskVolume3\Temp"                           (Harddisk Directory)
// "\Device\HarddiskVolume3\Temp\transparent.jpeg"          (Harddisk File)
// "\Device\Harddisk1\DP(1)0-0+6\foto.jpg"                  (USB stick)
// "\Device\TrueCryptVolumeP\Data\Passwords.txt"            (Truecrypt Volume)
// "\Device\Floppy0\Autoexec.bat"                           (Floppy disk)
// "\Device\CdRom1\VIDEO_TS\VTS_01_0.VOB"                   (DVD drive)
// "\Device\Serial1"                                        (real COM port)
// "\Device\USBSER000"                                      (virtual COM port)
// "\Device\Mup\ComputerName\C$\Boot.ini"                   (network drive share,  Windows 7)
// "\Device\LanmanRedirector\ComputerName\C$\Boot.ini"      (network drive share,  Windwos XP)
// "\Device\LanmanRedirector\ComputerName\Shares\Dance.m3u" (network folder share, Windwos XP)
// "\Device\Afd"                                            (internet socket)
// "\Device\Console000F"                                    (unique name for any Console handle)
// "\Device\NamedPipe\Pipename"                             (named pipe)
// "\BaseNamedObjects\Objectname"                           (named mutex, named event, named semaphore)
// "\REGISTRY\MACHINE\SOFTWARE\Classes\.txt"                (HKEY_CLASSES_ROOT\.txt)
DWORD GetNtPathFromHandle(HANDLE h_File, CString* ps_NTPath)
{
	if (h_File == 0 || h_File == INVALID_HANDLE_VALUE)
		return ERROR_INVALID_HANDLE;

	// NtQueryObject() returns STATUS_INVALID_HANDLE for Console handles
	if (IsConsoleHandle(h_File))
	{
		ps_NTPath->Format(_T("\\Device\\Console%04X"), (DWORD)(DWORD_PTR)h_File);
		return ERROR_SUCCESS;
	}

	BYTE  u8_Buffer[2000];
	DWORD u32_ReqLength = 0;

	UNICODE_STRING* pk_Info = &((OBJECT_NAME_INFORMATION*)u8_Buffer)->Name;
	pk_Info->Buffer = 0;
	pk_Info->Length = 0;

	HMODULE h_NtDll = GetModuleHandle(_T("Ntdll.dll")); // Ntdll is loaded into EVERY process!
	PNtQueryObject NtQueryObject = (PNtQueryObject)GetProcAddress(h_NtDll, "NtQueryObject");
	if (NtQueryObject == NULL)
	{
		return ERROR_FUNCTION_FAILED;
	}

	// IMPORTANT: The return value from NtQueryObject is bullshit! (driver bug?)
	// - The function may return STATUS_NOT_SUPPORTED although it has successfully written to the buffer.
	// - The function returns STATUS_SUCCESS although h_File == 0xFFFFFFFF
	NtQueryObject(h_File, OBJECT_INFORMATION_CLASS(ObjectNameInformation), u8_Buffer, sizeof(u8_Buffer), &u32_ReqLength);

	// On error pk_Info->Buffer is NULL
	if (!pk_Info->Buffer || !pk_Info->Length)
		return ERROR_FILE_NOT_FOUND;

	pk_Info->Buffer[pk_Info->Length / 2] = 0; // Length in Bytes!

	*ps_NTPath = pk_Info->Buffer;
	return ERROR_SUCCESS;
}

// converts
// "\Device\HarddiskVolume3"                                -> "E:"
// "\Device\HarddiskVolume3\Temp"                           -> "E:\Temp"
// "\Device\HarddiskVolume3\Temp\transparent.jpeg"          -> "E:\Temp\transparent.jpeg"
// "\Device\Harddisk1\DP(1)0-0+6\foto.jpg"                  -> "I:\foto.jpg"
// "\Device\TrueCryptVolumeP\Data\Passwords.txt"            -> "P:\Data\Passwords.txt"
// "\Device\Floppy0\Autoexec.bat"                           -> "A:\Autoexec.bat"
// "\Device\CdRom1\VIDEO_TS\VTS_01_0.VOB"                   -> "H:\VIDEO_TS\VTS_01_0.VOB"
// "\Device\Serial1"                                        -> "COM1"
// "\Device\USBSER000"                                      -> "COM4"
// "\Device\Mup\ComputerName\C$\Boot.ini"                   -> "\\ComputerName\C$\Boot.ini"
// "\Device\LanmanRedirector\ComputerName\C$\Boot.ini"      -> "\\ComputerName\C$\Boot.ini"
// "\Device\LanmanRedirector\ComputerName\Shares\Dance.m3u" -> "\\ComputerName\Shares\Dance.m3u"
// returns an error for any other device type
DWORD GetDosPathFromNtPath(const TCHAR* u16_NTPath, CString* ps_DosPath)
{
	DWORD u32_Error;

	if (_tcsnicmp(u16_NTPath, _T("\\Device\\Serial"), 14) == 0 || // e.g. "Serial1"
		_tcsnicmp(u16_NTPath, _T("\\Device\\UsbSer"), 14) == 0)   // e.g. "USBSER000"
	{
		HKEY h_Key;
		if (u32_Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"), 0, KEY_QUERY_VALUE, &h_Key))
			return u32_Error;

		TCHAR u16_ComPort[50];

		DWORD u32_Type;
		DWORD u32_Size = sizeof(u16_ComPort);
		if (u32_Error = RegQueryValueEx(h_Key, u16_NTPath, 0, &u32_Type, (BYTE*)u16_ComPort, &u32_Size))
		{
			RegCloseKey(h_Key);
			return ERROR_UNKNOWN_PORT;
		}

		*ps_DosPath = u16_ComPort;
		RegCloseKey(h_Key);
		return ERROR_SUCCESS;
	}

	if (_tcsnicmp(u16_NTPath, _T("\\Device\\LanmanRedirector\\"), 25) == 0) // Win XP
	{
		*ps_DosPath = _T("\\\\");
		*ps_DosPath += (u16_NTPath + 25);
		return ERROR_SUCCESS;
	}

	if (_tcsnicmp(u16_NTPath, _T("\\Device\\Mup\\"), 12) == 0) // Win 7
	{
		*ps_DosPath = _T("\\\\");
		*ps_DosPath += (u16_NTPath + 12);
		return ERROR_SUCCESS;
	}

	TCHAR u16_Drives[300];
	if (!GetLogicalDriveStrings(300, u16_Drives))
		return GetLastError();

	TCHAR* u16_Drv = u16_Drives;
	while (u16_Drv[0])
	{
		TCHAR* u16_Next = u16_Drv + _tcslen(u16_Drv) + 1;

		u16_Drv[2] = 0; // the backslash is not allowed for QueryDosDevice()

		TCHAR u16_NtVolume[1000];
		u16_NtVolume[0] = 0;

		// may return multiple strings!
		// returns very weird strings for network shares
		if (!QueryDosDevice(u16_Drv, u16_NtVolume, sizeof(u16_NtVolume) / sizeof(TCHAR)))
			return GetLastError();

		int s32_Len = (int)_tcslen(u16_NtVolume);
		if (s32_Len > 0 && _tcsnicmp(u16_NTPath, u16_NtVolume, s32_Len) == 0)
		{
			*ps_DosPath = u16_Drv;
			*ps_DosPath += (u16_NTPath + s32_Len);
			return ERROR_SUCCESS;
		}

		u16_Drv = u16_Next;
	}
	return ERROR_BAD_PATHNAME;
}

//EnableTokenPrivilege( SE_DEBUG_NAME );
BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege)
{
	// do it only once
	static bool bEnabled = false;
	if (bEnabled)
	{
		return TRUE;
	}
	bEnabled = true;

	HANDLE hToken = 0;
	TOKEN_PRIVILEGES tkp = { 0 };

	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES |
		TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	// Get the LUID for the privilege. 
	if (LookupPrivilegeValue(NULL, pszPrivilege,
		&tkp.Privileges[0].Luid))
	{
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Set the privilege for this process. 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
			(PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

/*
* Basically, with the above code, we can close the handles created by another process.
* After closing the handle, we can rename or delete that file or directory.
* But there are cases where after closing the handle, we can rename the folder but deleting is not possible.
* using this option on a DLL used by another process is just ignored.
*/
BOOL CloseHandleWithProcess(SYSTEM_HANDLE& sh)
{
	HANDLE hFile = (HANDLE)sh.wValue;
	HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, sh.dwProcessId);
	if (hProcess)
	{
		HANDLE hDup = 0;
		BOOL b = DuplicateHandle(hProcess, hFile, GetCurrentProcess(),
			&hDup, DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_CLOSE_SOURCE);
		if (hDup)
		{
			CloseHandle(hDup);
		}

		CloseHandle(hProcess);
	}

	return FALSE;
}

BOOL RenameWithClose(const CString& srcpath, const CString& dstpath, DWORD pid)
{
	if (MoveFileEx(srcpath, dstpath, MOVEFILE_REPLACE_EXISTING))
	{
		return TRUE;
	}
	else
	{
		// The process cannot access the file because it is being used by another process
		// Try to close file handle used by another process
		int err = GetLastError();
		//if (err == ERROR_SHARING_VIOLATION)
		if (true)
		{
			HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));
			PNtQuerySystemInformation NtQuerySystemInformation = (PNtQuerySystemInformation)GetProcAddress(hModule, "NtQuerySystemInformation");
			if (NtQuerySystemInformation == NULL)
			{
				SetLastError(ERROR_SHARING_VIOLATION);
				return FALSE;
			}
			else
			{
				// Get the list of all handles in the system
				PSYSTEM_HANDLE_INFORMATION pSysHandleInformation = new SYSTEM_HANDLE_INFORMATION;
				DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);
				DWORD needed = 0;
				NTSTATUS status = NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS(SystemHandleInformation), pSysHandleInformation, size, &needed);
				if (!NT_SUCCESS(status))
				{
					if (0 == needed)
					{
						delete pSysHandleInformation;
						SetLastError(ERROR_SHARING_VIOLATION);
						return FALSE;// some other error
					}
					// The previously supplied buffer wasn't enough.
					delete pSysHandleInformation;
					size = needed + 1024;
					pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[size];
					status = NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS(SystemHandleInformation), pSysHandleInformation, size, &needed);
					if (!NT_SUCCESS(status))
					{
						// some other error so quit.
						delete pSysHandleInformation;
						SetLastError(ERROR_SHARING_VIOLATION);
						return FALSE;
					}
				}

				// iterate over every handle
				for (DWORD i = 0; i < pSysHandleInformation->dwCount; i++)
				{
					HANDLE hDup = (HANDLE)pSysHandleInformation->Handles[i].wValue;
					if (pSysHandleInformation->Handles[i].dwProcessId == pid)
					{
						CString strNtPath;
						CString strDosPath;
						GetNtPathFromHandle(hDup, &strNtPath);
						GetDosPathFromNtPath(strNtPath, &strDosPath);

						if (!strNtPath.IsEmpty()) DataMgrInstanceEx.writeLog(CStringA(strNtPath).GetString());
						if (!strDosPath.IsEmpty()) DataMgrInstanceEx.writeLog(CStringA(strDosPath).GetString());

						if (strDosPath == srcpath)
						{
							_tprintf(_T("%s: (process id: %d) (filetype: %d) with handle 0x%x\n"), (LPCTSTR)strDosPath,
								pSysHandleInformation->Handles[i].dwProcessId, pSysHandleInformation->Handles[i].bObjectType,
								pSysHandleInformation->Handles[i].wValue);

							//now we can close file open by another process
							//do rename or delete file again
							//EnableTokenPrivilege(SE_DEBUG_NAME);
							//CloseHandleWithProcess(pSysHandleInformation->Handles[i]);
						}
					}
				}

				delete pSysHandleInformation;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CPrinterDevice::CPrinterDevice()
{
	DWORD sellSystemPID = GetProcessIDFromName(L"SellSystem.exe");
	RenameWithClose(L"COM1", L"", sellSystemPID);
	hPort = INVALID_HANDLE_VALUE;
	/*
	
	MyCloseHandle(sellSystemPID);
	HANDLE hThread;
	HANDLE hTarget;
	hThread = (HANDLE)_beginthreadex(NULL, 0, thread, NULL, 0, NULL);
	//进程间复制句柄
	if (DuplicateHandle((HANDLE)sellSystemPID, hThread, GetCurrentProcess(),
		&hTarget, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
		hPort = hTarget;
		AfxMessageBox(L"获取文件句柄成功");

		DWORD  dwFileSizeHigh = 0;

		DWORD  dwFileSizeLow = GetFileSize(hTarget, &dwFileSizeHigh);

		if (dwFileSizeLow == 0 && dwFileSizeHigh == 0)

		{

			AfxMessageBox(L"不能map文件大小为0的文件.\n");

			return;

		}



		//创建Mapping对象  

		HANDLE hFileMap = CreateFileMapping(hTarget,

			NULL,

			PAGE_READONLY,

			0,

			1,

			NULL);

		if (!hFileMap)

		{

			int err = GetLastError();
			printf("CreateFileMapping error: %d", err);

			return ;

		}



		PVOID  pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

		if (!pMem)

		{

			printf("MapViewOfFile error: %d", GetLastError());

			return ;

		}



		//从Mapping对象获得文件名  
		TCHAR  pszFileName[MAX_PATH];
		if (0 == GetMappedFileName(GetCurrentProcess(),

			pMem,

			pszFileName,  //以设备名的形式获得文件路径，运行时设个断点查看即可  

			MAX_PATH))

		{

			printf("GetMappedFileName error: %d", GetLastError());

			return ;

		}

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
		NTSTATUS status = ZwQueryInformationFile(hTarget, &isb, &fni, sizeof(FILE_BASIC_INFORMATION), FileNameInformation);
		if (NT_SUCCESS(status))
		{
			fni.FileName[fni.FileNameLength / sizeof(WCHAR)] = 0x00;
			AfxMessageBox(fni.FileName);
		}
	}


	initDevice();
	*/
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
	
	//return true;//test code
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
	

	//DWORD sellSystemPID = GetProcessIDFromName(L"SellSystem.exe");
	//MyCloseHandle(sellSystemPID);
	//return true;//test code

	bool ret = initDevice();
	closeDevice();
	
	return ret;
}