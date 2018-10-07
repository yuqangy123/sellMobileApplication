#pragma once
#include "Singleton.h"
#include <string>
#include <vector>


#define DataMgrInstanceEx CDataManager::InstanceEx()
class CDataManager :public CSingleton<CDataManager>
{
public:
	CDataManager();
	~CDataManager();

	void getGoodsInfoOrder(std::string& ret);
	void guidToString(std::string& ret);
	void writeLog(const char* ret);

protected:
	void init();
	std::vector<std::string> m_logBuff;

public:
	std::string MchId;
	std::string MchKey;
	std::string Domain;
	
	std::string ErpSysId;//��6λ������erpϵͳ���ṩ��
	std::string NodeCode;//4λ���ű���
	std::string OrderStaticEnd;//46λ����
	std::string PosFontEndIP;//����IP
	std::string DownloadOrderFilePath;//���ض��˵��ĵ�ַ
	std::string LogFilePath;//������Ϣ��־
};

