#include "stdafx.h"
#include "DataManager.h"
#include "ParseIni.h"
#include "md5.h"
//#include "afxwin.h"
#include "FileUnit.h"

static _RecordsetPtr m_pRecordset;
static _ConnectionPtr m_pConnection;

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
	
	OrderStaticEnd = "000001";

	CParseIniFile iniParse;
	map<string, string> valueMap;
	iniParse.ReadConfig("SellSystem.ini", valueMap, "SYSTEM");
	NodeCode = valueMap["NodeCode"];
	PosFontEndIP = valueMap["PosFontEndIP"];

	valueMap.clear();
	iniParse.ReadConfig("sellMobileAppConfig.ini", valueMap, "SYSTEM");
	MchId = valueMap["mch_id"];
	MchKey = valueMap["mch_key"];
	Domain = valueMap["domain"];
	ErpCode = valueMap["ErpCode"];
	
	

	char strModule[MAX_PATH * 2] = { 0 };
	GetModuleFileNameA(NULL, strModule, MAX_PATH * 2);
	::PathRemoveFileSpecA(strModule);
	wsprintfA(strModule + strlen(strModule), "\\bill");
	DownloadOrderFilePath.assign(strModule);

	GetModuleFileNameA(NULL, strModule, MAX_PATH * 2);
	::PathRemoveFileSpecA(strModule);

	if (valueMap.find("paySystemPath") == valueMap.end())
		PaySystemPath.assign(strModule);
	else
	{
		PaySystemPath = valueMap["paySystemPath"];
		if (!FileUnitInstance->DirectoryExists(PaySystemPath.c_str()))
			PaySystemPath.assign(strModule);
	}

	//ODBCConnect(CString(strModule) + L"\\SaleBill_20180905.0012");//test code
	wsprintfA(strModule + strlen(strModule), "\\sellMobileLog");
	LogFilePath.assign(strModule);
	CreateDirectoryA(strModule, NULL);


	::CoInitialize(NULL);
	if (m_pConnection.CreateInstance(__uuidof(Connection)) != S_OK)
	{
		AfxMessageBox(L"_ConnectionPtr init fail");
	}
	if (m_pRecordset.CreateInstance(_uuidof(Recordset)) != S_OK)
	{
		AfxMessageBox(L"_RecordsetPtr init fail");
	}
	//::CoUninitialize();


	AfxOleInit();

}

CDataManager::~CDataManager()
{
}

void CDataManager::getGoodsInfoOrder(std::string& ret, std::string& systemOrder)
{
	char tmpbuff[MAX_PATH] = { 0 };
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	sprintf_s(tmpbuff, "%s\\log\\GoodsInfoS%04d%02d%02d.Log", PaySystemPath.c_str(), systm.wYear, systm.wMonth, systm.wDay);
	ifstream infile(tmpbuff);
	if (!infile)
	{
		printf("file open error! %s", tmpbuff);
		return;
	}

	string line, lastline;
	while (getline(infile, line))
		lastline = line;
	infile.close();

	auto spaceIndex = lastline.find(" ");
	if (-1 == spaceIndex)
	{
		return;
	}

	auto dIndex = lastline.find(",");
	if (-1 == dIndex)
	{
		return ;
	}

	systemOrder = lastline.substr(spaceIndex + 1, dIndex - spaceIndex - 1);
	
	sprintf_s(tmpbuff, "%s%s%s%s", ErpCode.c_str(), NodeCode.c_str(), systemOrder.c_str(), OrderStaticEnd.c_str());
	ret.assign(tmpbuff);
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
				break;
			}
			sprintf_s(ql, "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password='www.bizcent.com'", strDBFileA.GetString());
			m_pConnection->Open(ql, "", "", adModeUnknown);

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
		}
		catch (_com_error * e)
		{
			AfxMessageBox(e->ErrorMessage());
			break;
		}

		csTotalFee.Format(L"%.2f", totalFee);
	} while (false);
	m_pConnection->Close();

	
	return true;
}