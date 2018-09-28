#pragma once
#include <string>

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define empty_break(obj)	{if(!(obj)) break;}
#define random(x) (rand()%x)

std::string KS_UTF8_to_ANSI(const char* szUTF8);
char* KS_ANSI_to_UTF8(const char* szAnsi);