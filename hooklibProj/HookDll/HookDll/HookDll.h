// HookDll.h : HookDll DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include<windows.h>
#include<stdio.h>
#define DLL_API extern "C" __declspec(dllimport) //extern "C"��֤���������ĺ������������ı�
DLL_API void  SetHook(HWND hwnd);


// CHookDllApp
// �йش���ʵ�ֵ���Ϣ������� HookDll.cpp
//

class CHookDllApp : public CWinApp
{
public:
	CHookDllApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
