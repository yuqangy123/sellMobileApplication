#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

//订单付款数据
struct payOrderInfo
{
	std::string orderNo;
	std::string tradeNo;
	std::string fee;
	std::string date;
	std::string time;
	std::string tradeType;

	static payOrderInfo* create(const char* pOrderNo, const char* pTradeNo, const char* pFee,
		const char* timeEnd, const char* pTradeType)
	{
		payOrderInfo* info = new payOrderInfo;
		
		info->orderNo.assign(pOrderNo);
		info->tradeNo.assign(pTradeNo);
		info->fee.assign(pFee);
		info->tradeType.assign(pTradeType);

		int year = 0;
		int month = 0;
		int day = 0;
		int hour = 0;
		int min = 0;
		int sec = 0;

		sscanf_s(timeEnd, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);

		char cdate[16] = { 0 };
		char ctime[16] = { 0 };
		sprintf_s(cdate, "%04d%02d%02d", year, month, day);
		sprintf_s(ctime, "%02d%02d%02d", hour, min, sec);

		info->date.assign(cdate);
		info->time.assign(ctime);

		return info;
	}
};

//退款数据
typedef struct refundOrderInfo_
{
	std::string refundOrderNo;//单据号
	std::string payOrderNo;//原单据号
	std::string tradeNo;//中心流水
	std::string fee;
	std::string payDate;
	std::string tradeType;


	static refundOrderInfo_* create(const char* pRefundOrderNo, const char* pTradeNo,
		const char* pPayOrderNo, const char* pFee)
	{
		refundOrderInfo_* info = new refundOrderInfo_;

		info->refundOrderNo.assign(pRefundOrderNo);
		info->payOrderNo.assign(pPayOrderNo);
		info->tradeNo.assign(pTradeNo);
		info->fee.assign(pFee);
		return info;
	}
}refundOrderInfo;


#define DataMgrInstanceEx CDataManager::InstanceEx()
class CDataManager : public CSingleton<CDataManager>
{
public:
	CDataManager();
	~CDataManager();

	//获取单号
	std::string getSystemOrder();
	//获取单号，销售单号
	void getGoodsInfoOrder(std::string& sellOrder, std::string& systemOrder);
	//根据单号获取销售单号
	CStringA getSellOrderWithSystemOrder(CStringA systemOrder);
	//获取订单总金额
	BOOL getGoodsInfoTotalFee(const CString& BillNumber, CString& csTotalFee);
	//获取最后x个订单
	BOOL getlastBills(std::vector<double>& billVtr, int wantNum);

	void guidToString(std::string& ret);

	void writeLog(const char* ret);

	double getBillByTesserImage(const char* args="-psm 7 -l chi_sim");


protected:
	void init();
	std::vector<std::string> m_logBuff;

public:
	std::string MchId;
	std::string MchKey;
	std::string Domain;
	
	std::string NodeCode;//4位部门编码
	std::string TerminalCode;//注册终端号
	std::string OrderStaticEnd;//46位数字
	std::string PosFontEndIP;//机器IP
	std::string DownloadOrderFilePath;//下载对账单的地址
	std::string LogFilePath;//交易信息日志
	std::string ErpCode;//erp系统商编码
	std::string PaySystemPath;//支付系统路径
	std::string PrinterDeviceName;//打印机名
	int printPageNum;//一次性打印张数
	std::string store_id;//门店ID
	std::string cashier_id;//收银员ID
	int EscKeyTag;
	int customKeyboard;//收款界面快捷键
	int resetHookTimes;//重置hook的条件次数

	//图像识别
	int tesser_x;
	int tesser_y;
	int tesser_w;
	int tesser_h;
	CString tesserImagePath;
	CStringA tesserResultPath;
	static bool tessing;

	map<unsigned char, int> replaceMap;
};

void captureTesserImage(int x, int y, int w, int h, wchar_t* pImgPath);