#pragma once
#include <string>
#include "MessageQueue.h"



#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define false_break(obj)	{if(!(obj)) break;}


std::string KS_UTF8_to_ANSI(const char* szUTF8);
char* KS_ANSI_to_UTF8(const char* szAnsi);
int getRandom(unsigned int r);