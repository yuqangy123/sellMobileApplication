#include "stdafx.h"
#include "commonMicro.h"


std::string KS_UTF8_to_ANSI(const char* szUTF8)
{
	if (szUTF8 == NULL)
		return "";

	int     nLen = ::MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, 0);
	WCHAR   * pWstr = new WCHAR[nLen + 1];
	ZeroMemory(pWstr, sizeof(WCHAR) * (nLen + 1));
	::MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, pWstr, nLen);
	std::string     strAnsi(_bstr_t((wchar_t*)pWstr));
	delete[] pWstr;
	return strAnsi;
}

char* KS_ANSI_to_UTF8(const char* szAnsi)
{
	if (szAnsi == NULL)
		return NULL;

	_bstr_t   bstrTmp(szAnsi);
	int       nLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, NULL, 0, NULL, NULL);
	char      * pUTF8 = new char[nLen + 1];
	ZeroMemory(pUTF8, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, pUTF8, nLen, NULL, NULL);
	return pUTF8;
}

int getRandom(unsigned int r)
{
	srand(time(NULL));
	return rand() % r;
}

bool stringIsNumber(const char* c)
{
	if (nullptr != c)
	{
		const char* pc = c;
		bool rt = true;
		do
		{
			if (!((*pc >= '0' && *pc <= '9') || '.' == *pc))
			{
				rt = false;
				break;
			}
		} while (*(++pc) != 0);
		return rt;
	}
	return false;
}

bool stringIsNumber(const wchar_t* c)
{
	if (nullptr != c)
	{
		const wchar_t* pc = c;
		bool rt = true;
		do
		{
			if (!((*pc >= L'0' && *pc <= L'9') || L'.' == *pc))
			{
				rt = false;
				break;
			}
		} while (*(++pc) != 0);
		return rt;
	}
	return false;
}