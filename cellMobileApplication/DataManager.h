#pragma once
#include "Singleton.h"
#include <string>


#define DataMgrInstanceEx CDataManager::InstanceEx()
class CDataManager :public CSingleton<CDataManager>
{
public:
	CDataManager();
	~CDataManager();

protected:
	void init();

public:
	std::string MchId;
	std::string MchKey;

	std::string ErpSysId;//这6位是我们erp系统商提供的
	std::string NodeCode;//4位部门编码
	std::string OrderStaticEnd;//46位数字
};

