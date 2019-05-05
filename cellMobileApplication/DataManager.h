#pragma once
#include "Singleton.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

//������������
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

//�˿�����
typedef struct refundOrderInfo_
{
	std::string refundOrderNo;//���ݺ�
	std::string payOrderNo;//ԭ���ݺ�
	std::string tradeNo;//������ˮ
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

	//��ȡ����
	std::string getSystemOrder();
	//��ȡ���ţ����۵���
	void getGoodsInfoOrder(std::string& sellOrder, std::string& systemOrder);
	//���ݵ��Ż�ȡ���۵���
	CStringA getSellOrderWithSystemOrder(CStringA systemOrder);
	//��ȡ�����ܽ��
	BOOL getGoodsInfoTotalFee(const CString& BillNumber, CString& csTotalFee);
	//��ȡ���x������
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
	
	std::string NodeCode;//4λ���ű���
	std::string TerminalCode;//ע���ն˺�
	std::string OrderStaticEnd;//46λ����
	std::string PosFontEndIP;//����IP
	std::string DownloadOrderFilePath;//���ض��˵��ĵ�ַ
	std::string LogFilePath;//������Ϣ��־
	std::string ErpCode;//erpϵͳ�̱���
	std::string PaySystemPath;//֧��ϵͳ·��
	std::string PrinterDeviceName;//��ӡ����
	int printPageNum;//һ���Դ�ӡ����
	std::string store_id;//�ŵ�ID
	std::string cashier_id;//����ԱID
	int EscKeyTag;
	int customKeyboard;//�տ�����ݼ�
	int resetHookTimes;//����hook����������

	//ͼ��ʶ��
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