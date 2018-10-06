#include "stdafx.h"
#include "DataManager.h"
#include "ParseIni.h"
#include "md5.h"

DECLARE_SINGLETON_MEMBER(CDataManager);
CDataManager::CDataManager()
{
	MchId = "1000000013";
	MchKey = "qu9k3vxsy2uc69u86iybirpu14coj34z";
	ErpSysId = "020098";
	OrderStaticEnd = "000001";

	CParseIniFile iniParse;
	map<string, string> valueMap;
	iniParse.ReadConfig("SellSystem.ini", valueMap, "SYSTEM");
	NodeCode = valueMap["NodeCode"];
	PosFontEndIP = valueMap["PosFontEndIP"];
}


CDataManager::~CDataManager()
{
}

void CDataManager::getGoodsInfoOrder(std::string& ret)
{
	char tmpbuff[MAX_PATH] = { 0 };
	SYSTEMTIME systm;
	GetLocalTime(&systm);
	sprintf_s(tmpbuff, "log/GoodsInfo%04d%02d%02d.Log", systm.wYear, systm.wMonth, systm.wDay);
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

	string systemOrder = lastline.substr(spaceIndex + 1, dIndex - spaceIndex - 1);
	
	sprintf_s(tmpbuff, "%s%s%s%s", ErpSysId.c_str(), NodeCode.c_str(), systemOrder.c_str(), OrderStaticEnd.c_str());
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