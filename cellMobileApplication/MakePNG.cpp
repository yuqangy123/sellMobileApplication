//MakePNG.cpp
 
#include "StdAfx.h"
#include "MakePNG.h"
 
CMakePNG::CMakePNG(void)
{
	GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL);
}
 
CMakePNG::~CMakePNG(void)
{
}
 
/***************************************************************************/
/*	功能：		根据rect屏幕抓图，保存为文件名为strFilePath的PNG图像文件   */
/*	输入参数：	HDC	hDC					屏幕HDC；                          */
/*				CRect rect				需要的矩形;                        */
/*				CString	strFilePath		保存文件全路径（含后缀名）;        */
/***************************************************************************/
BOOL CMakePNG::MakePNG(HDC hDC, CRect rect, CString strFilePath)
{
	BITMAP bmp;
	PBITMAPINFO pbmi; 
	PBITMAPINFOHEADER pbih;     // bitmap info-header 
	BITMAPFILEHEADER  hdr;      // bitmap file-header
	WORD    cClrBits; 
	LPBYTE lpBits;              // memory pointer
	DWORD dwTmp; 
	DWORD cb;                   // incremental count of bytes 
	BYTE *hp;                   // byte pointer 
	HANDLE hfile;               // file handle 
	CString szBMPFilename = strFilePath.Left(strFilePath.GetLength() - 3) + _T("bmp");//先保存成位图
	HDC hdcCompatible = CreateCompatibleDC(hDC);
	HBITMAP hbmScreen = CreateCompatibleBitmap(hDC, rect.Width(), rect.Height());
 
	if (hbmScreen == NULL)
	{
	    AfxMessageBox(_T("CreateCompatibleBitmap() error")); 
		return FALSE;
	}
 
	// Select the bitmaps into the compatible DC. 
 
	if (!SelectObject(hdcCompatible, hbmScreen)) 
	{
		AfxMessageBox(_T("Compatible Bitmap Selection error")); 
		return FALSE;
	}
 
	//Copy color data for the entire display into a 
	//bitmap that is selected into a compatible DC. 
 
	if (!BitBlt(hdcCompatible, 
		0,0, 
		rect.Width(), rect.Height(), 
		hDC, 
		rect.left,rect.top, 
		SRCCOPY)) 
	{
        AfxMessageBox(_T("Screen to Compat Blt Failed"));
		return FALSE;
	}
 
 
 
	// Retrieve the bitmap's color format, width, and height. 
	if (!GetObject(hbmScreen, sizeof(BITMAP), (LPSTR)&bmp))
	{
		AfxMessageBox(L"GetObject()出错!");
		return FALSE;
	}
	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 
 
	// Allocate memory for the BITMAPINFO structure. (This structure 
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
	// data structures.) 
 
	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 
 
	// There is no RGBQUAD array for the 24-bit-per-pixel format. 
 
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 
 
	// Initialize the fields in the BITMAPINFO structure. 
 
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
 
	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 
 
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
	// Set biClrImportant to 0, indicating that all of the device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
 
	pbih = (PBITMAPINFOHEADER) pbmi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
 
	if (!lpBits) 
	{
	    AfxMessageBox(L"内存分配错误");
		return FALSE;
	}
	// Retrieve the color table (RGBQUAD array) and the bits 
	// (array of palette indices) from the DIB. 
	if (!GetDIBits(hDC, hbmScreen, 0, (WORD) pbih->biHeight, lpBits, pbmi, 
		DIB_RGB_COLORS)) 
	{
		AfxMessageBox(_T("GetDIBits() error"));
		return FALSE;
	}
 
	// Create the .BMP file. 
	hfile = CreateFile(szBMPFilename, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
	if (hfile == INVALID_HANDLE_VALUE) 
	{
		 AfxMessageBox(_T("创建文件失败"));
		return false;
	}
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 
 
	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 
 
	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hfile, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
		(LPDWORD) &dwTmp,  NULL))
	{
		AfxMessageBox(L"");
		return FALSE;
	}
 
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hfile, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL))) 
	{
		//AfxMessageBox(L"写bmp文件头失败");
		return FALSE;
	}
 
	// Copy the array of color indices into the .BMP file. 
	cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hfile, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
	{
		AfxMessageBox(_T("写入BMP文件失败"));
		return FALSE;
	}
 
	// Close the .BMP file. 
	if (!CloseHandle(hfile)) 
	{
		 AfxMessageBox(_T("Can't close BMP file.")); 
	}
 
		// Free memory. 
		GlobalFree((HGLOBAL)lpBits);
	
	//转换成PNG
	if(!BMptoPNG(szBMPFilename,strFilePath))
	{
		DeleteFile(szBMPFilename);
		return FALSE;
	}
	DeleteObject(hbmScreen);
	DeleteFile(szBMPFilename);
	return TRUE;
}
// //转换BMP文件为PNG文件            
BOOL CMakePNG::BMptoPNG(LPCWSTR StrBMp,LPCWSTR StrPNG)
{
	CLSID encoderClsid;
	Status stat;
	Image* image = NULL;
	image = Bitmap::FromFile(StrBMp,TRUE);
	if (!GetEncoderClsid(L"image/png",&encoderClsid))
	{
		return FALSE;
	}
	stat = image->Save(StrPNG,&encoderClsid,NULL);
	if (stat != Ok)
	{
		return FALSE;
	}
	delete image;
	return TRUE;
}
 
//	功能描述:		转换PNG文件为BMP文件      
BOOL CMakePNG::PNGtoBMp(LPCWSTR StrPNG,LPCWSTR StrBMp)
{
	CLSID encoderClsid;
	Status stat;
	Image* pImage;
	pImage = Bitmap::FromFile(StrPNG,TRUE);
	if (!GetEncoderClsid(L"image/bmp",&encoderClsid))
	{
		return FALSE;
	}
	stat = pImage->Save(StrBMp,&encoderClsid,NULL);
	if (stat != Ok)
	{
		return FALSE;
	}
	delete pImage;
	return TRUE;
}
 
BOOL CMakePNG::GetEncoderClsid(WCHAR* pFormat,CLSID* pClsid)
{
	UINT num = 0,size = 0;
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num,&size);
	if (size == 0)
	{
		return FALSE;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
	{
		return FALSE;
	}
	GetImageEncoders(num,size,pImageCodecInfo);
	BOOL bfound = FALSE;
	for (UINT i = 0;!bfound && i < num;  i++)
	{
		if (_wcsicmp(pImageCodecInfo[i].MimeType,pFormat) == 0)
		{
			*pClsid = pImageCodecInfo[i].Clsid;
			bfound = TRUE;
		}
	}
	free(pImageCodecInfo);
	return bfound;
}
