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

	//获取订单号
	void getGoodsInfoOrder(std::string& ret, std::string& systemOrder);
	//获取订单总金额
	BOOL getGoodsInfoTotalFee(const CString& BillNumber, CString& csTotalFee);

	void guidToString(std::string& ret);

	void writeLog(const char* ret);

	

protected:
	void init();
	std::vector<std::string> m_logBuff;

public:
	std::string MchId;
	std::string MchKey;
	std::string Domain;
	
	std::string NodeCode;//4位部门编码
	std::string OrderStaticEnd;//46位数字
	std::string PosFontEndIP;//机器IP
	std::string DownloadOrderFilePath;//下载对账单的地址
	std::string LogFilePath;//交易信息日志
	std::string ErpCode;//erp系统商编码
	std::string PaySystemPath;//支付系统路径
};

