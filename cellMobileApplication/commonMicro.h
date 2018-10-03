#pragma once
#include <string>
#include "MessageQueue.h"

//msg define
#define UM_TIPS_MESSAGE					(WM_USER + 100)
#define UM_ORDER_QUERY					(WM_USER + 101)//订单查询
#define UM_PAY_SUCCESS_NOTIFY			(WM_USER + 102)//支付成功
#define UM_PAY_DOWNLOAD_WAITING_NOTIFY	(WM_USER + 103)//支付成功

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define false_break(obj)	{if(!(obj)) break;}


std::string KS_UTF8_to_ANSI(const char* szUTF8);
char* KS_ANSI_to_UTF8(const char* szAnsi);
int getRandom(unsigned int r);