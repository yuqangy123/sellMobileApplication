// HookDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "HookDll.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma  data_seg("MySec")  //定义一个新节，将全局变量放入这个节中，用于共享
HWND g_hWnd = NULL;
#pragma  data_seg()
#pragma  comment(linker,"/section:MySec,RWS") //设置为共享的节

#define UM_HOOK_KEYBOARD_SHOW_HIDE		(WM_USER + 100)

HHOOK g_hMouse = NULL;
//HHOOK g_hKeyBoard = NULL;
HHOOK g_hKeyBoardLL = NULL;
std::vector<int> hookKeyboardArray;


//鼠标钩子回调函数
LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam) {
	return 1; //return ture说明全部屏蔽
}
/*
//键盘钩子回调函数
LRESULT CALLBACK KeyBoardProc(int code, WPARAM wParam, LPARAM lParam){
if (VK_F2==wParam)
{
//当按下F2则退出调用这个dll的应用程序
::SendMessage(g_hWnd, WM_CLOSE, 0, 0);
UnhookWindowsHookEx(g_hMouse);
UnhookWindowsHookEx(g_hKeyBoard);
}
return 1; //return ture说明全部屏蔽
}
*/
//低级键盘钩子函数用来屏蔽功能键以及组合键
LRESULT CALLBACK KeyBoardLLProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HC_ACTION)
	{
		KBDLLHOOKSTRUCT *kblp = (KBDLLHOOKSTRUCT*)lParam; //这个结构体暂时没怎么搞明白，官方msdn也写的不是很明白，有待琢磨
		if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)  //如果按键为按下状态
		{
			for (auto itr = hookKeyboardArray.begin(); itr != hookKeyboardArray.end(); ++itr)
			{
				if (kblp->vkCode == *itr)//VK_F2
				{
					//当按下F2则退出调用这个dll的应用程序
					::SendMessage(g_hWnd, UM_HOOK_KEYBOARD_SHOW_HIDE, kblp->vkCode, 0);
					//UnhookWindowsHookEx(g_hMouse);
					//UnhookWindowsHookEx(g_hKeyBoardLL);
					return TRUE;
				}
			}
		}
	}
	
	//传给下一个钩子
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
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CHookDllApp

BEGIN_MESSAGE_MAP(CHookDllApp, CWinApp)
END_MESSAGE_MAP()


// CHookDllApp 构造

CHookDllApp::CHookDllApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CHookDllApp 对象

CHookDllApp theApp;


// CHookDllApp 初始化

BOOL CHookDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
