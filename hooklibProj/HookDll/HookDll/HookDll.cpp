// HookDll.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "HookDll.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma  data_seg("MySec")  //����һ���½ڣ���ȫ�ֱ�������������У����ڹ���
HWND g_hWnd = NULL;
#pragma  data_seg()
#pragma  comment(linker,"/section:MySec,RWS") //����Ϊ����Ľ�

#define UM_HOOK_KEYBOARD_SHOW_HIDE		(WM_USER + 100)

HHOOK g_hMouse = NULL;
//HHOOK g_hKeyBoard = NULL;
HHOOK g_hKeyBoardLL = NULL;
std::vector<int> hookKeyboardArray;


//��깳�ӻص�����
LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam) {
	return 1; //return ture˵��ȫ������
}
/*
//���̹��ӻص�����
LRESULT CALLBACK KeyBoardProc(int code, WPARAM wParam, LPARAM lParam){
if (VK_F2==wParam)
{
//������F2���˳��������dll��Ӧ�ó���
::SendMessage(g_hWnd, WM_CLOSE, 0, 0);
UnhookWindowsHookEx(g_hMouse);
UnhookWindowsHookEx(g_hKeyBoard);
}
return 1; //return ture˵��ȫ������
}
*/
//�ͼ����̹��Ӻ����������ι��ܼ��Լ���ϼ�
LRESULT CALLBACK KeyBoardLLProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION)
	{
		KBDLLHOOKSTRUCT *kblp = (KBDLLHOOKSTRUCT*)lParam; //����ṹ����ʱû��ô�����ף��ٷ�msdnҲд�Ĳ��Ǻ����ף��д���ĥ
		if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)  //�������Ϊ����״̬
		{
			for (auto itr = hookKeyboardArray.begin(); itr != hookKeyboardArray.end(); ++itr)
			{
				if (kblp->vkCode == *itr)//VK_F2
				{
					//������F2���˳��������dll��Ӧ�ó���
					::SendMessage(g_hWnd, UM_HOOK_KEYBOARD_SHOW_HIDE, kblp->vkCode, 0);
					//UnhookWindowsHookEx(g_hMouse);
					//UnhookWindowsHookEx(g_hKeyBoardLL);
					return TRUE;
				}
			}
		}
	}
	
	//������һ������
	return CallNextHookEx(g_hKeyBoardLL, code, wParam, lParam);
}

void parseKeyboard(const char* hookKeyboard, int begin, int end)
{
	if (nullptr == hookKeyboard || end <= begin)
		return;

	char number[32];
	memset(number, 0x0, 32);
	char* pnum = number;
	int size = end - begin + 1;
	int strat = 0;
	while ((*pnum++ = *(hookKeyboard + begin + strat)) && ++strat < size)
	{
		int n = 0;
	}
	*pnum = 0;
	hookKeyboardArray.push_back(atoi(number));
}

void SetHook(HWND hwnd, const char* hookKeyboard, int hookKeyboardLen) {
	g_hWnd = hwnd;
	if (nullptr != hookKeyboard)
	{
		int begin = 0, end = 0;		
		while (hookKeyboardLen > end)
		{
			if (*(hookKeyboard + end) == ',' && begin < end)
			{
				parseKeyboard(hookKeyboard, begin, end-1);
				begin = end + 1;
				end = begin;
			}
			else
			{
				++end;
			}
		}
		parseKeyboard(hookKeyboard, begin, end-1);
	}
	//g_hMouse = SetWindowsHookEx(WH_MOUSE, MouseProc, GetModuleHandle(L"HookDll"), 0);
	g_hKeyBoardLL = SetWindowsHookEx(WH_KEYBOARD_LL, KeyBoardLLProc, GetModuleHandle(L"HookDll"), 0);
	//g_hKeyBoard = SetWindowsHookEx(WH_KEYBOARD, KeyBoardProc, GetModuleHandle("HookDll"), 0);
}

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CHookDllApp

BEGIN_MESSAGE_MAP(CHookDllApp, CWinApp)
END_MESSAGE_MAP()


// CHookDllApp ����

CHookDllApp::CHookDllApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CHookDllApp ����

CHookDllApp theApp;


// CHookDllApp ��ʼ��

BOOL CHookDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
