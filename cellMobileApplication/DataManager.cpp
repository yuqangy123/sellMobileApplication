#include "stdafx.h"
#include "DataManager.h"
#include "ParseIni.h"
#include "md5.h"
//#include "afxwin.h"
#include "FileUnit.h"
#include "Math.h"
#include "MakePNG.h"
#include "Winuser.h"

static _RecordsetPtr m_pRecordset;
static _ConnectionPtr m_pConnection;
bool CDataManager::tessing = false;
/*
#include "afxdb.h"
BOOL ODBCConnect(CString strDBFile)
{
	CString strConnect;
	strConnect.Format(_T("ODBC;DRIVER={MICROSOFT ACCESS DRIVER (*.mdb)};UID=sa;PWD=www.bizcent.com;DBQ=%s"), strDBFile);
	CDatabase db;
	if (db.Open(NULL, FALSE, FALSE, strConnect))
	{
		//连接数据库成功
		CRecordset rs(&db);
		CString strSql;
		strSql = L"select * from tbSaleBillDetail";
		//SQL语句
		rs.Open(AFX_DB_USE_DEFAULT_TYPE, strSql);
		//执行Sql语句（可添加　删除　查询等）
		if (rs.IsOpen())
		{
			CDBVariant variant;
			rs.MoveFirst();
			while (!rs.IsEOF())
			{
				//读取记录
				rs.GetFieldValue(_T("姓名"), variant);
				rs.MoveNext();
			}
		}
		db.Close();
		return TRUE;
	}
	return FALSE;
}
*/



DECLARE_SINGLETON_MEMBER(CDataManager);
CDataManager::CDataManager()
{
	EscKeyTag = 0;

	OrderStaticEnd = "000001";

	customKeyboard = VK_F11;

	resetHookTimes = 9;

	CParseIniFile iniParse;
	map<string, string> valueMap;

	

	
	valueMap.clear();
	iniParse.ReadConfig("sellMobileAppConfig.ini", valueMap, "SYSTEM");
	if (0 == valueMap.size())AfxMessageBox(L"sellMobileAppConfig.ini为空");

	MchId = valueMap["mch_id"];
	MchKey = valueMap["mch_key"];
	Domain = valueMap["domain"];
	ErpCode = valueMap["ErpCode"];
	PrinterDeviceName = valueMap["PrinterDeviceName"];
	
	store_id = valueMap["store_id"];
	cashier_id = valueMap["cashier_id"];

	if (!valueMap["printPageNum"].empty())
		printPageNum = atoi( valueMap["printPageNum"].c_str() );

	char strModule[MAX_PATH * 2] = { 0 };
	GetModuleFileNameA(NULL, strModule, MAX_PATH * 2);
	::PathRemoveFileSpecA(strModule);
	wsprintfA(strModule + strlen(strModule), "\\bill");
	DownloadOrderFilePath.assign(strModule);

	GetModuleFileNameA(NULL, strModule, MAX_PATH * 2);
	::PathRemoveFileSpecA(strModule);

	
	if (valueMap.find("sellSystemPath") == valueMap.end())
		PaySystemPath.assign(strModule);
	else
	{
		PaySystemPath = valueMap["sellSystemPath"];
		if (!FileUnitInstance->DirectoryExists(PaySystemPath.c_str()))
			PaySystemPath.assign(strModule);
	}

	if (valueMap.find("QRCodeKeyboard") != valueMap.end())
	{
		std::string val = valueMap["QRCodeKeyboard"];
		val = val.substr(1);
		int vali = atoi(val.c_str());
		if (1 <= vali && 12 >= vali)
		{
			customKeyboard = VK_F1 - 1 + vali;
		}
	}
	//获取图像识别的图片坐标大小
	tesser_x = tesser_x = 0;
	tesser_w = tesser_h = 1;
	if (valueMap.find("tesser_x") != valueMap.end())tesser_x = atoi(valueMap["tesser_x"].c_str());
	if (valueMap.find("tesser_y") != valueMap.end())tesser_y = atoi(valueMap["tesser_y"].c_str());
	if (valueMap.find("tesser_w") != valueMap.end())tesser_w = atoi(valueMap["tesser_w"].c_str());
	if (valueMap.find("tesser_h") != valueMap.end())tesser_h = atoi(valueMap["tesser_h"].c_str());


	if (valueMap.find("resetHookTimes") != valueMap.end())
		resetHookTimes = atoi(valueMap["resetHookTimes"].c_str());



	valueMap.clear();
	char sellSystemPath[MAX_PATH] = { 0 };
	sprintf_s(sellSystemPath, "%s//SellSystem.ini", PaySystemPath.c_str());
	iniParse.ReadConfig(sellSystemPath, valueMap, "SYSTEM");
	if (0 == valueMap.size()) AfxMessageBox(L"SellSystem.ini为空");

	//NodeCode = valueMap["NodeCode"];
	PosFontEndIP = valueMap["PosFontEndIP"];

	








	valueMap.clear();
	sellSystemPath[0] = 0;
	sprintf_s(sellSystemPath, "%s\\SellSystemConfig.ini", PaySystemPath.c_str());
	iniParse.ReadConfig(sellSystemPath, valueMap, "Terminal");
	if (0 == valueMap.size()) AfxMessageBox(L"SellSystemConfig.ini为空");

	std::string strTerminal = valueMap["TerminalCode"];
	size_t terItr = strTerminal.find(',');
	if (terItr != std::string::npos)
		TerminalCode = strTerminal.substr(terItr + 1);
	else
		TerminalCode = strTerminal;
	
	std::string strNodecode = valueMap["NodeCode"];
	terItr = strNodecode.find(',');
	if (terItr != std::string::npos)
		NodeCode = strNodecode.substr(terItr + 1);
	else
		NodeCode = strNodecode;
	
	wsprintfA(strModule + strlen(strModule), "\\sellMobileLog");
	LogFilePath.assign(strModule);
	CreateDirectoryA(strModule, NULL);
	CStringA tesserImagePathA;
	tesserImagePathA.Format("%s\\tesser.jpg", LogFilePath.c_str());
	tesserImagePath = tesserImagePathA;

	tesserResultPath.Format("%s\\result", LogFilePath.c_str());

	AfxOleInit();
	::CoInitialize(NULL);

	if (m_pConnection.CreateInstance(__uuidof(Connection)) != S_OK)
	{
		DWORD err = GetLastError();

		char strModule[MAX_PATH * 2] = { 0 };
		GetModuleFileNameA(NULL, strModule, MAX_PATH * 2);
		::PathRemoveFileSpecA(strModule);

		char cmd[1024] = { 0 };
		sprintf_s(cmd, "regsvr32 %s\\msado15.dll", strModule);
		WinExec(cmd, SW_SHOW);

		CString errStr;
		errStr.Format(L"_ConnectionPtr init fail(%d)", err);
		AfxMessageBox(errStr);
		PostQuitMessage(0);
	}
	if (m_pRecordset.CreateInstance(_uuidof(Recordset)) != S_OK)
	{
		DWORD err = GetLastError();
		CString errStr;
		errStr.Format(L"_RecordsetPtr init fail(%d)", err);
		AfxMessageBox(errStr);
		PostQuitMessage(0);
	}
	//::CoUninitialize();
	








	//读取replace.ini替换表
	valueMap.clear();
	iniParse.ReadConfig("replace.ini", valueMap, "SYSTEM");
	if (0 == valueMap.size())AfxMessageBox(L"replace.ini为空");
	for (auto itr = valueMap.begin(); itr != valueMap.end(); ++itr)
	{
		const char* c = itr->second.c_str();
		const unsigned char* k = (const unsigned char*)itr->first.c_str();
		if (*c >= '0' && *c <= '9')
		{
			replaceMap[*k] = *c - '0';
		}
	}
}

CDataManager::~CDataManager()
{
}

std::string CDataManager::getSystemOrder()
{
	char tmpbuff[MAX_PATH] = { 0 };
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	sprintf_s(tmpbuff, "%s\\log\\GoodsInfoS%04d%02d%02d.Log", PaySystemPath.c_str(), systm.wYear, systm.wMonth, systm.wDay);
	ifstream infile(tmpbuff);
	if (!infile)
	{
		char log[256];
		sprintf_s(log, "file open error! %s", tmpbuff);
		writeLog(log);
		return "";
	}

	string line, lastline;
	while (getline(infile, line))
		lastline = line;
	infile.close();

	auto spaceIndex = lastline.find(" ");
	if (-1 == spaceIndex)
	{
		writeLog("lastline.find  fail");
		return "";
	}

	auto dIndex = lastline.find(",");
	if (-1 == dIndex)
	{
		writeLog("lastline.find , fail");
		return "";
	}

	std::string systemOrder = lastline.substr(spaceIndex + 1, dIndex - spaceIndex - 1);
	return systemOrder;
}

void CDataManager::getGoodsInfoOrder(std::string& sellOrder, std::string& systemOrder)
{
	char tmpbuff[MAX_PATH] = { 0 };

	if (systemOrder.empty())
	{
		systemOrder = getSystemOrder();
	}
	
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	int m1 = int(systm.wYear / 1000) * 1000;
	m1 = systm.wYear - m1;
	systm.wYear = m1;
	m1 = int(m1 / 100) * 100;
	systm.wYear = systm.wYear - m1;

	sprintf_s(tmpbuff, "%s%s%s%02d%02d%02d", ErpCode.c_str(), NodeCode.c_str(), systemOrder.c_str(), 
		systm.wHour, systm.wMinute, systm.wSecond);

	sellOrder.assign(tmpbuff);
}
CStringA CDataManager::getSellOrderWithSystemOrder(CStringA systemOrder)
{
	CStringA ret;
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	int m1 = int(systm.wYear / 1000) * 1000;
	m1 = systm.wYear - m1;
	systm.wYear = m1;
	m1 = int(m1 / 100) * 100;
	systm.wYear = systm.wYear - m1;
	ret.Format("%s%s%s%02d%02d%02d", ErpCode.c_str(), NodeCode.c_str(), systemOrder.GetString(), 
		systm.wHour, systm.wMinute, systm.wSecond);
	return ret;
}

void CDataManager::guidToString(std::string& ret)
{
	GUID guid;
	CoCreateGuid(&guid);
	char buf[64];
	sprintf_s(buf, sizeof(buf), "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	MD5 md5;
	md5.update(buf);
	ret = md5.toString();
}

void CDataManager::writeLog(const char* ret)
{
	char logbuf[2048] = { 0 };
	SYSTEMTIME time;
	GetLocalTime(&time);

	sprintf_s(logbuf, 2048, "%04d%02d%02d-%02d:%02d:%02d\n%s\n\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, ret);


	FILE* pf=nullptr;
	char filename[MAX_PATH] = { 0 };
	sprintf_s(filename, MAX_PATH, "%s/%04d%02d%02d.txt", LogFilePath.c_str(), time.wYear, time.wMonth, time.wDay);
	fopen_s(&pf, filename, "a+");
	if (pf == NULL)
	{
		return;
	}

	fwrite(logbuf, strlen(logbuf), 1, pf);

	fflush(pf);
	fclose(pf);
}

BOOL CDataManager::getGoodsInfoTotalFee(const CString& BillNumber, CString& csTotalFee)
{
	//test code
	//csTotalFee = "0.01";
	//return true;
	HRESULT openState = S_OK+1;
	do {
		double totalFee = 0;
		try {
			char ql[512];
			CStringA strDBFileA;
			SYSTEMTIME systm;
			GetLocalTime(&systm);

			strDBFileA.Format("%s\\SaleBill_%04d%02d%02d.%s", PaySystemPath.c_str(), systm.wYear, systm.wMonth, systm.wDay, NodeCode.c_str());
			if (!FileUnitInstance->FileExists(strDBFileA.GetString()))
			{
				strDBFileA = strDBFileA + "file can't find";
				writeLog(strDBFileA.GetString());
				break;
			}
			sprintf_s(ql, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password='www.bizcent.com'", strDBFileA.GetString());
			openState = m_pConnection->Open(ql, "", "", adModeUnknown);

			if (!BillNumber.IsEmpty())
			{
				sprintf_s(ql, "select * from tbSaleBillDetail where BillNumber = '%s'", CStringA(BillNumber).GetString());
				//m_pRecordset->Close();
				m_pRecordset->Open(ql, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);

				if (!m_pRecordset->BOF)
					m_pRecordset->MoveFirst();
				else
				{
					AfxMessageBox(L"no data in the table");
					break;
				}
				_variant_t va, vaIndex;
				while (!m_pRecordset->adoEOF)
				{
					va = m_pRecordset->GetCollect("SaleEarning");

					CStringA SaleEarning;
					if (va.vt != VT_NULL)
					{
						SaleEarning = (LPCSTR)_bstr_t(va);
						totalFee += atof(SaleEarning.GetString());
					}
					m_pRecordset->MoveNext();
				}
			}
			else
			{
				writeLog("BillNumber is empty");
			}
		}
		catch (_com_error * e)
		{
			AfxMessageBox(e->ErrorMessage());
			break;
		}

		totalFee *= 10;
		double floor_t = floor(totalFee + 0.5);
		totalFee = totalFee < floor_t ? floor_t/10 : floor(totalFee)/10;

		csTotalFee.Format(L"%.2f", totalFee);
	} while (false);
	if(openState == S_OK)
		m_pConnection->Close();

	
	return true;
}

BOOL CDataManager::getlastBills(std::vector<double>& billVtr, int wantNum)
{
	//test code
	//csTotalFee = "0.01";
	//return true;
	HRESULT openState = S_OK + 1;
	do {
		double totalFee = 0;
		try {
			char ql[512];
			CStringA strDBFileA;
			SYSTEMTIME systm;
			GetLocalTime(&systm);

			strDBFileA.Format("%s\\SaleBill_%04d%02d%02d.%s", PaySystemPath.c_str(), systm.wYear, systm.wMonth, systm.wDay, NodeCode.c_str());
			if (!FileUnitInstance->FileExists(strDBFileA.GetString()))
			{
				strDBFileA = strDBFileA + "file can't find";
				writeLog(strDBFileA.GetString());
				break;
			}
			sprintf_s(ql, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password='www.bizcent.com'", strDBFileA.GetString());
			openState = m_pConnection->Open(ql, "", "", adModeUnknown);

			
			sprintf_s(ql, "select * from tbSaleBillDetail");
			//m_pRecordset->Close();
			m_pRecordset->Open(ql, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);

			if (!m_pRecordset->BOF)
				m_pRecordset->MoveLast();
			else
			{
				AfxMessageBox(L"no data in the table");
				break;
			}
			_variant_t va, vaIndex;
			char tmpBuf[128] = {};
			while (!m_pRecordset->adoEOF && wantNum>0)
			{
				va = m_pRecordset->GetCollect("BillNumber");

				if (va.vt != VT_NULL)
				{
					CStringA valStr = _bstr_t(va);
					sprintf_s(tmpBuf, "%s", valStr.GetString());
					
					double valL = 0;
					int tmpLen = strlen(tmpBuf);
					double mult = 1.0;
					while (--tmpLen >= 0)
					{
						valL += (tmpBuf[tmpLen] - '0')*mult;
						mult *= 10.0;
					}
					bool repeat = false;
					for (auto itr = billVtr.begin(); itr != billVtr.end(); ++itr)
					{
						if (valL == *itr)
						{
							repeat = true;
							break;
						}
					}
					if (!repeat)
					{
						billVtr.push_back(valL);
						--wantNum;
					}
				}
				m_pRecordset->MovePrevious();
				VARIANT_BOOL bl;
				m_pRecordset->get_BOF(&bl);
				if (-1 == bl)
					break;
			}
		}
		catch (_com_error * e)
		{
			AfxMessageBox(e->ErrorMessage());
			break;
		}
	} while (false);
	if (openState == S_OK)
		m_pConnection->Close();


	return true;
}


void captureTesserImage(int x, int y, int w, int h, wchar_t* pImgPath)
{
	HWND hDesktop = GetDesktopWindow();
	CWnd* pDesktop = CWnd::FromHandle(hDesktop);
	CDC *pDC = pDesktop->GetDC();
	CMakePNG MakePNG;
	CRect rect(x, y, x+w, y + h);
	MakePNG.MakePNG(pDC->m_hDC, rect, pImgPath);
	ReleaseDC(hDesktop, pDC->m_hDC);
	return;

	HDC hdcSrc = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	int nWidth = GetDeviceCaps(hdcSrc, HORZRES);
	int nHeight = GetDeviceCaps(hdcSrc, VERTRES);
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), x, y, w, h, hdcSrc, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(pImgPath, Gdiplus::ImageFormatJPEG);//ImageFormatJPEG
	
}

double CDataManager::getBillByTesserImage(const char* args)
{
	CDataManager::tessing = true;
	captureTesserImage(tesser_x, tesser_y, tesser_w, tesser_h, tesserImagePath.GetBuffer());
	CString csJpg(tesserImagePath.GetBuffer());
	CStringA csAJpg(csJpg);

	//writeLog("CDataManager::getBillByTesserImage begin");
	char cmd[512] = { 0 };
	//tesserResultPath.Format("result");
	//sprintf_s(cmd, "tesseract %s %s -l eng", csAJpg.GetString(), tesserResultPath.GetString());
	//sprintf_s(cmd, "tesseract %s %s -psm 7 -l chi_sim", csAJpg.GetString(), tesserResultPath.GetString());
	sprintf_s(cmd, "tesseract %s %s %s", csAJpg.GetString(), tesserResultPath.GetString(), args);
	//WinExec(cmd, SW_HIDE);

	
	// TODO: Add extra validation here
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		//MessageBox("CreatePipe Failed");
		CDataManager::tessing = false;
		return 0;
	}

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	
	if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
		//MessageBox("CreateProcess failed!");
		CDataManager::tessing = false;
		return 0;
	}
	CloseHandle(hWrite);

	char buffer[4096] = { 0 };
	CString strOutput;
	DWORD bytesRead;

	while (1)
	{
		if (NULL == ReadFile(hRead, buffer, 4095, &bytesRead, NULL))
		{
			break;
		}
		strOutput += buffer;
		
		Sleep(500);
	}
	CloseHandle(hRead);
	
	//writeLog("CDataManager::getBillByTesserImage end");
	CDataManager::tessing = false;



	
	//Sleep(1000);
	CStringA csAResult(tesserResultPath);
	csAResult.Append(".txt");
	
	FILE* pf = NULL;
	fopen_s(&pf, csAResult.GetString(), "r");
	if (NULL == pf)
	{
		CDataManager::tessing = false;
		return .0f;
	}
	
	char cResultBuff[256] = { 0 };
	fread(cResultBuff, 256, 1, pf);
	fclose(pf);

	string log("解析：");
	log.append(cResultBuff);
	
	//writeLog(log.c_str());
	//check result have some abcd... , if find, tesser only number
	//char cResultBuff[] = "7了.01";
	{
		char* tag = cResultBuff;
		bool isBak = false;
		while (*tag != 0 && *tag != '.')
		{
			/*
			//有英文
			if ((*tag >= 'a' && *tag <= 'z') ||
				(*tag >= 'A' && *tag <= 'Z'))
			{
				writeLog("发现英文");
				isBak = true;
				break;
			}*/

			//有中文
			if (*tag & 0x80 && *(tag + 1) & 0x80)
			{
				//writeLog("发现中文");
				isBak = true;
				break;
			}
			++tag;
		}
		if (isBak)
			return getBillByTesserImage("-psm 7");
	}

	//writeLog("解析整数");
	char* tag = cResultBuff;
	int izhengshu = 0;
	int ixiaoshu = 0;
	while (*tag != 0 && *tag != '.')
	{
		if (*tag >= '0' && *tag <= '9')
		{
			izhengshu *= 10;
			izhengshu += (*tag - '0');
		}
		else
		{
			unsigned char k = *tag;
			if (replaceMap.find(k) != replaceMap.end())
			{
				izhengshu *= 10;
				izhengshu += (replaceMap[k]);
			}
		}
		++tag;
	}
	if (*tag == '.')
	{
		++tag;
		while (*tag != 0 && *tag != '.')
		{
			if (*tag >= '0' && *tag <= '9')
			{
				ixiaoshu *= 10;
				ixiaoshu += (*tag - '0');
			}
			else
			{
				unsigned char k = *tag;
				if (replaceMap.find(k) != replaceMap.end())
				{
					ixiaoshu *= 10;
					ixiaoshu += (replaceMap[k]);
				}
			}
			++tag;
		}
	}

	//writeLog("解析小数");
	int xiaoshuTmp = ixiaoshu;
	int xiaoshuMult = 10;
	while (xiaoshuTmp / 10 > 0)
	{
		xiaoshuMult *= 10;
		xiaoshuTmp /= 10;
	}

	return izhengshu + (float)ixiaoshu / (float)xiaoshuMult;
}
