//MakePNG.h
 
#pragma once
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"GdiPlus.lib") 
 
class CMakePNG
{
public:
	CMakePNG(void);
	~CMakePNG(void);
 
	BOOL MakePNG(HDC hDC,CRect rect,CString strFilePath);
	BOOL BMptoPNG(LPCWSTR StrBMp,LPCWSTR StrPNG);
	BOOL PNGtoBMp(LPCWSTR StrPNG,LPCWSTR StrBMp);
	BOOL GetEncoderClsid(WCHAR* pFormat,CLSID* pClsid);
private:
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_pGdiToken;
};
