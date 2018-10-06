#pragma once

//#include "Gdiplusheaders.h"
//#include "Gdiplus.h"
//#include "GdiPlusImaging.h"

#define gif_declare_member() Gdiplus::Image* gif_image;\
GUID gif_Guid;\
UINT gif_frameCount;\
UINT gif_framePos;	\
UINT_PTR gif_draw_timerID;\
BOOL gif_show;\
ULONG_PTR gif_gdiplusToken;


#define gif_init_member() gif_image = nullptr;\
gif_frameCount = 0;\
gif_framePos = 0;\
gif_show = true;

#define gif_destory_member() Gdiplus::GdiplusShutdown(gif_gdiplusToken);

#define gif_show(b) {gif_show=b;}



//https://blog.csdn.net/sz76211822/article/details/51507616
#define gif_loadGif(filename, interval) {\
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;\
	Gdiplus::GdiplusStartup(&gif_gdiplusToken, &gdiplusStartupInput, NULL);\
	WCHAR strModule[MAX_PATH * 2] = { 0 };\
	GetModuleFileNameW(NULL, strModule, MAX_PATH * 2);\
	::PathRemoveFileSpecW(strModule);\
	wsprintfW(strModule + wcslen(strModule), L"\\%s", filename);\
	gif_image = Gdiplus::Image::FromFile(strModule);\
	UINT count = gif_image->GetFrameDimensionsCount();\
	GUID *pDimensionIDs = (GUID*)new GUID[count];\
	gif_image->GetFrameDimensionsList(pDimensionIDs, count);\
	WCHAR strGuid[39];\
	StringFromGUID2(pDimensionIDs[0], strGuid, 39);\
	gif_frameCount = gif_image->GetFrameCount(&pDimensionIDs[0]);\
	delete[] pDimensionIDs;\
	UINT FrameDelayNums = gif_image->GetPropertyItemSize(PropertyTagFrameDelay);\
	Gdiplus::PropertyItem* lpPropertyItem = new Gdiplus::PropertyItem[FrameDelayNums];\
	gif_image->GetPropertyItem(PropertyTagFrameDelay, FrameDelayNums, lpPropertyItem);\
	gif_Guid = Gdiplus::FrameDimensionTime;\
	gif_image->SelectActiveFrame(&gif_Guid, gif_framePos);\
	gif_draw_timerID = SetTimer(TIMER_ID_DRAW_GIF, interval, NULL);\
};

#define TIMER_ID_DRAW_GIF	20181006
#define gif_draw_timer(x, y)	{\
	if(gif_image && TIMER_ID_DRAW_GIF == nIDEvent && gif_show){\
		Gdiplus::Graphics gh(m_hWnd);\
		gh.DrawImage(gif_image, x, y, gif_image->GetWidth(), gif_image->GetHeight());\
		gif_image->SelectActiveFrame(&gif_Guid, gif_framePos);\
		if(++gif_framePos == gif_frameCount)gif_framePos=0;\
	};\
}