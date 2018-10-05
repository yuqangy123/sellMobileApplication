#pragma once
#include <string>
#include "MessageQueue.h"

//msg define
#define UM_HOOK_KEYBOARD_SHOW_HIDE		(WM_USER + 100)
#define UM_TIPS_MESSAGE					(WM_USER + 101)
#define UM_ORDER_REQUEST_OK_NOTIFY		(WM_USER + 102)//�����ύ���
#define UM_PAY_SUCCESS_NOTIFY			(WM_USER + 103)//֧���ɹ�
#define UM_PAY_DOWNLOAD_REQUEST_NOTIFY	(WM_USER + 104)//��ʷ������������
#define UM_REFUND_ORDER_NOTIFY			(WM_USER + 105)//�����˿�
#define UM_NETWORK_STATE_NOTIFY			(WM_USER + 106)//����仯

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define false_break(obj)	{if(!(obj)) break;}


std::string KS_UTF8_to_ANSI(const char* szUTF8);
char* KS_ANSI_to_UTF8(const char* szAnsi);
int getRandom(unsigned int r);