#pragma once
#include "Singleton.h"
#include <string>


#define DataMgrInstanceEx CDataManager::InstanceEx()
class CDataManager :public CSingleton<CDataManager>
{
public:
	CDataManager();
	~CDataManager();

	void getGoodsInfoOrder(std::string& ret);
	void guidToString(std::string& ret);

protected:
	void init();

public:
	std::string MchId;
	std::string MchKey;

	std::string ErpSysId;//��6λ������erpϵͳ���ṩ��
	std::string NodeCode;//4λ���ű���
	std::string OrderStaticEnd;//46λ����
	std::string PosFontEndIP;//����IP
};

