#pragma once
#include <string>
#include "DataManager.h"


//msg define
#define UM_HOOK_KEYBOARD_SHOW_HIDE		(WM_USER + 100)//隐藏显示主界面
#define UM_TIPS_MESSAGE					(WM_USER + 101)
#define UM_ORDER_REQUEST_OK_NOTIFY		(WM_USER + 102)//订单提交完成
#define UM_PAY_SUCCESS_NOTIFY			(WM_USER + 103)//支付成功
#define UM_PAY_DOWNLOAD_REQUEST_NOTIFY	(WM_USER + 104)//历史订单下载请求
#define UM_REFUND_ORDER_NOTIFY			(WM_USER + 105)//申请退款
#define UM_NETWORK_STATE_NOTIFY			(WM_USER + 106)//网络变化
#define UM_SHOWQRCODE_PAY_NOTIFY		(WM_USER + 107)//显示支付界面
#define UM_ESC_KEYBOARD_NOTIFY			(WM_USER + 108)//Esc键通知
#define UM_REFUND_QUERY_ORDER_TIME_NOTIFY			(WM_USER + 103)//退款查询原支付时间

const int shift_key_cov = 1 << 17;
const int alt_key_cov = 1 << 18;
const int ctrl_key_cov = 1 << 19;

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define false_break(obj)	{if(!(obj)) break;}


std::string KS_UTF8_to_ANSI(const char* szUTF8);
char* KS_ANSI_to_UTF8(const char* szAnsi);
int getRandom(unsigned int r);
bool stringIsNumber(const char* c);
bool stringIsNumber(const wchar_t* c);