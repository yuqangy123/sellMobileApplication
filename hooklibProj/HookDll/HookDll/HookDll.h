// HookDll.h : HookDll DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include<windows.h>
#include<stdio.h>
#define DLL_API extern "C" __declspec(dllimport) //extern "C"保证导出函数的函数名不发生改变
DLL_API void  SetHook(HWND hwnd);


// CHookDllApp
// 有关此类实现的信息，请参阅 HookDll.cpp
//

class CHookDllApp : public CWinApp
{
public:
	CHookDllApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
