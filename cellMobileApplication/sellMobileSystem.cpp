#include "stdafx.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include <vector>
#include <algorithm>
#include <functional>
#include "md5.h"
#include "commonMicro.h"
#include "DataManager.h"
#include <math.h>

using namespace std;

static sellMobileSystem* g_sellMobileInstance = nullptr;
sellMobileSystem::sellMobileSystem()
{
	m_state = sellState::none;
}


sellMobileSystem::~sellMobileSystem()
{
}

std::string atoFee(const char* fee)
{
	double nfee = atof(fee);
	nfee *= 100;
	char cfee[16] = { 0 };
	sprintf_s(cfee, "%d", (int)nfee);
	return string(cfee);
}

void writeRequestParamsLog(const char* apiName, const map<string, string>& params)
{
	const size_t bufsize = 1024;
	char buf[bufsize] = { 0 };
	int dx = 0;
	int len = strlen(apiName);
	memcpy_s(buf + dx, bufsize - dx, apiName, len);
	dx += len;

	char onePar[128] = { 0 };
	for (auto itr = params.begin(); itr != params.end(); ++itr)
	{
		sprintf_s(onePar, "%s:%s\n", itr->first.c_str(), itr->second.c_str());
		len = strlen(onePar);
		memcpy_s(buf + dx, bufsize - dx, onePar, len);
		if (dx+len >= bufsize)break;
		dx += len;
	}
	buf[dx>=bufsize ? bufsize-1:dx] = 0;

	DataMgrInstanceEx.writeLog(buf);
}

sellMobileSystem* sellMobileSystem::instance()
{
	if (nullptr == g_sellMobileInstance)
	{
		g_sellMobileInstance = new sellMobileSystem();
	}
	return g_sellMobileInstance;
}

void sellMobileSystem::init(const char* domain)
{
	string sysDomain(domain);
	char parone[256] = { 0 };

	sprintf_s(parone, "%smicropay", sysDomain.c_str());
	m_api["micropay"] = parone;

	sprintf_s(parone, "%sorderquery", sysDomain.c_str());
	m_api["orderquery"] = parone;

	sprintf_s(parone, "%srefundorder", sysDomain.c_str());
	m_api["refundorder"] = parone;

	sprintf_s(parone, "%srefundquery", sysDomain.c_str());
	m_api["refundquery"] = parone;
	
	sprintf_s(parone, "%sdownload", sysDomain.c_str());
	m_api["queryDownloadOrder"] = parone;
}

void sellMobileSystem::setMchInfo(const char* id, const char* key)
{
	m_mch_id = id;
	m_mch_key = key;
}

void sellMobileSystem::setMainDialogHwnd(HWND hnd)
{
	m_mainHwnd = hnd;
}

//订单查询
bool sellMobileSystem::requestOrderQuery(HWND objHwnd)
{
	m_orderQueryHwnd = objHwnd;
	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["out_trade_no"] = m_order_no;
	params["nonce_str"] = m_nonce_str;
	params["trade_type"] = getTradeTypeWithOrderNo(m_order_no.c_str());
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		DataMgrInstanceEx.writeLog(data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			char* msgbuff = new char[256];
			memset(msgbuff, 0x0, 256);
			memcpy_s(msgbuff, 256, data.c_str(), data.length());
			::PostMessage(m_orderQueryHwnd, UM_PAY_SUCCESS_NOTIFY, -1, (LPARAM)msgbuff);
			setState(sellState::none);
			return;
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		string trade_state = getXmlNode(pDoc, "trade_state");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 == nret)
			{
				//requestOrderQuery();
				if (!trade_state.compare("SUCCESS"))
				{
					::PostMessage(m_orderQueryHwnd, UM_PAY_SUCCESS_NOTIFY, 0, 0);
				}
			}
			else
			{
				setState(sellState::none);

				char* msgbuff = new char[256];
				memset(msgbuff, 0x0, 256);
				memcpy_s(msgbuff, 256, retmsg.c_str(), retmsg.length());
				::PostMessage(m_microPayHwnd, UM_PAY_SUCCESS_NOTIFY, -1, (LPARAM)msgbuff);
				//sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
			
		}
		
	};
	curlManager::instance()->request(m_api["orderquery"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	writeRequestParamsLog("orderquery", params);
	setState(sellState::orderquerying);
	return true;
}

//提交刷卡支付
bool sellMobileSystem::requestMicropay(HWND objHwnd, const char* fee, const char* order_no, const char* auth_code)
{
	m_order_no = order_no;
	m_nonce_str = "5K8264ILTKCH16CQ";
	m_microPayHwnd = objHwnd;

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["trade_type"] = getTradeTypeWithOrderNo(m_order_no.c_str());
	params["out_trade_no"] = m_order_no;
	params["total_fee"] = atoFee(fee);
	params["auth_code"] = auth_code;
	params["body"] = "chunjifuzhuang";
	params["spbill_create_ip"] = DataMgrInstanceEx.PosFontEndIP;
	params["nonce_str"] = m_nonce_str;
	params["sign"] = makeSign(params);
	

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>(); 
	*responseCallback = [this](const std::string& data)
	{
		DataMgrInstanceEx.writeLog(data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			char* msgbuff = new char[256];
			memset(msgbuff, 0x0, 256);
			memcpy_s(msgbuff, 256, data.c_str(), data.length());
			::PostMessage(m_microPayHwnd, UM_ORDER_REQUEST_OK_NOTIFY, -1, (LPARAM)msgbuff);
			setState(sellState::none);
			return;
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret || -2010 == nret)
			{
				::PostMessage(m_microPayHwnd, UM_ORDER_REQUEST_OK_NOTIFY, 0, 0);
			}
			else
			{
				setState(sellState::none);

				char* msgbuff = new char[256];
				memset(msgbuff, 0x0, 256);
				memcpy_s(msgbuff, 256, retmsg.c_str(), retmsg.length());
				::PostMessage(m_microPayHwnd, UM_ORDER_REQUEST_OK_NOTIFY, -1, (LPARAM)msgbuff);
				//sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
			
		}
	};
	curlManager::instance()->request(m_api["micropay"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	writeRequestParamsLog("orderquery", params);
	setState(sellState::paying);
	return true;
}

//申请退款
bool sellMobileSystem::requestRefundOrder(HWND objHwnd, const char* order_no, const char* refund_no, const char* ctotalfee, const char* cfee)
{
	m_refundorderHwnd = objHwnd;

	m_nonce_str = "5K8264ILTKCH16CQ";
	string strfee = atoFee(cfee);
	string strtotalfee = atoFee(ctotalfee);

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["nonce_str"] = m_nonce_str;
	params["out_trade_no"] = order_no;
	params["out_refund_no"] = refund_no;//由三部分组成
	params["refund_fee"] = strfee;
	params["total_fee"] = strtotalfee;
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		DataMgrInstanceEx.writeLog(data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			char* msgbuff = new char[256];
			memset(msgbuff, 0x0, 256);
			memcpy_s(msgbuff, 256, data.c_str(), data.length());
			::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, -1, (LPARAM)msgbuff);
			setState(sellState::none);
			return;
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 == nret)
			{
				bool refundSuccess = !retmsg.compare("SUCCESS");
				::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, refundSuccess ? 1 : 0, 0);
			}
			else
			{
				setState(sellState::none);
				char* msgbuff = new char[256];
				memset(msgbuff, 0x0, 256);
				memcpy_s(msgbuff, 256, retmsg.c_str(), retmsg.length());
				::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, -1, (LPARAM)msgbuff);
			}
		}
	};
	curlManager::instance()->request(m_api["refundorder"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	writeRequestParamsLog("refundorder", params);
	setState(sellState::refunding);
	return true;
}

//查询退款
bool sellMobileSystem::requestRefundQuery(HWND objHwnd, const char* order_no, const char* refund_no)
{
	m_refundorderHwnd = objHwnd;

	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["nonce_str"] = m_nonce_str;
	params["out_trade_no"] = order_no;
	params["refund_no"] = refund_no;//由三部分组成
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		DataMgrInstanceEx.writeLog(data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			char* msgbuff = new char[256];
			memset(msgbuff, 0x0, 256);
			memcpy_s(msgbuff, 256, data.c_str(), data.length());
			::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, -1, (LPARAM)msgbuff);
			setState(sellState::none);
			return;
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		string refund_status = getXmlNode(pDoc, "refund_status");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 == nret)
			{
				setState(sellState::none);
				bool refundSuccess = !refund_status.compare("SUCCESS");
				::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, refundSuccess ? 1:-1, 0);
			}
			else
			{
				setState(sellState::none);
				char* msgbuff = new char[256];
				memset(msgbuff, 0x0, 256);
				memcpy_s(msgbuff, 256, retmsg.c_str(), retmsg.length());
				::PostMessage(m_refundorderHwnd, UM_REFUND_ORDER_NOTIFY, -1, (LPARAM)msgbuff);

			}
		}
	};
	curlManager::instance()->request(m_api["refundquery"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	writeRequestParamsLog("refundquery", params);
	setState(sellState::refundquerying);
	return true;
}

//下载对账单
bool sellMobileSystem::requestDownloadOrder(HWND objHwnd, const char* startDate, const char* endDate, const char* type)
{
	m_downloadorderHwnd = objHwnd;

	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["nonce_str"] = m_nonce_str;
	params["start_date"] = startDate;
	params["end_date"] = endDate;
	params["billtype"] = type;
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		DataMgrInstanceEx.writeLog(data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			char* msgbuff = new char[256];
			memset(msgbuff, 0x0, 256);
			memcpy_s(msgbuff, 256, data.c_str(), data.length());
			::PostMessage(m_downloadorderHwnd, UM_PAY_DOWNLOAD_REQUEST_NOTIFY, -1, (LPARAM)msgbuff);
			setState(sellState::none);
			return;
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		string status = getXmlNode(pDoc, "status");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 == nret)//开始下载，之后30秒查询一次
			{
				::PostMessage(m_downloadorderHwnd, UM_PAY_DOWNLOAD_REQUEST_NOTIFY, 0, 0);
			}
			else if(10002 == nret)//文件已生成，进行下载
			{
				if (status == "1")
				{
					string httppath = getXmlNode(pDoc, "httppath");
					string sftppath = getXmlNode(pDoc, "sftppath");
					sftppath.insert(0, DataMgrInstanceEx.DownloadOrderFilePath.c_str());
					function<void(const std::string& data)>* doloadCallback = new function<void(const std::string& data)>();
					*doloadCallback = [this](const std::string& data)
					//*doloadCallback = [](const std::string& data)//test code
					{
						DataMgrInstanceEx.writeLog(data.c_str());
						setState(sellState::none);

						char* msgbuff = new char[256];
						memset(msgbuff, 0x0, 256);
						memcpy_s(msgbuff, 256, data.c_str(), data.length());
						::PostMessage(m_downloadorderHwnd, UM_PAY_DOWNLOAD_REQUEST_NOTIFY, 1, (LPARAM)msgbuff);
					};
					curlManager::instance()->download(httppath.c_str(), "", sftppath.c_str(), doloadCallback);
				}					
			}
			else if (10012 == nret)//表示正在生成中
			{

			}
			else
			{
				setState(sellState::none);
				//sendTipsMessage(retmsg.c_str(), retmsg.length());
				::PostMessage(m_downloadorderHwnd, UM_PAY_DOWNLOAD_REQUEST_NOTIFY, -1, 0);
			}
		}
	};
	curlManager::instance()->request(m_api["queryDownloadOrder"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	writeRequestParamsLog("queryDownloadOrder", params);
	setState(sellState::downloadOrder);
	return true;
}

bool comp(const string &a, const string &b){
	
	int av = a.at(0);
	int bv = b.at(0);
	return av < bv;
}

string sellMobileSystem::makeSign(const std::map<std::string, std::string>& paramsMap)
{
	vector<string> keyVtr;
	for (auto itr = paramsMap.begin(); itr != paramsMap.end(); ++itr)
	{
		keyVtr.push_back(itr->first);
	}
	sort(keyVtr.begin(), keyVtr.end(), comp);

	string urlParams = "";
	char parone[256] = { 0 };
	for (auto itr = keyVtr.begin(); itr != keyVtr.end(); ++itr)
	{
		if (itr->compare("sign") && !itr->empty() && itr->compare("_url"))
		{
			sprintf_s(parone, "%s=%s&", itr->c_str(), paramsMap.find(*itr)->second.c_str());
			urlParams += parone;
		}
	}


	urlParams += "key=";
	urlParams += m_mch_key;

	MD5 md5;
	md5.update(urlParams);
	string res = md5.toString();
	transform(res.begin(), res.end(), res.begin(), ::toupper);

	return res;
}

std::string sellMobileSystem::mapToXml(const std::map<std::string, std::string>& paramsMap)
{
	string res = "<xml>";
	char parone[256] = { 0 };
	for (auto itr = paramsMap.begin(); itr != paramsMap.end(); ++itr)
	{
		const string& k = itr->first;
		const string& v = itr->second;
		sprintf_s(parone, "<%s>%s</%s>", k.c_str(), v.c_str(), k.c_str());
		res += parone;
	}
	
	res += "</xml>";
	return res;
}

std::string sellMobileSystem::getXmlNode(tinyxml2::XMLDocument *pDoc, const char* nodename)
{
	string res;
	do
	{
		tinyxml2::XMLElement *rootEle = pDoc->RootElement();
		ValueMap map = getValueMapFromXmlElement(rootEle);
		Value xmlValue = map["xml"];
		false_break(xmlValue.getType() == Value::Type::MAP);

		auto retcode_itr = xmlValue.asValueMap().find(nodename);
		false_break(retcode_itr != xmlValue.asValueMap().end());

		auto retcodeVtr = retcode_itr->second;
		false_break(retcodeVtr.getType() == Value::Type::VECTOR);

		auto retcodeMap_itr = retcodeVtr.asValueVector()[0];
		false_break(retcodeMap_itr.getType() == Value::Type::MAP);

		auto retcodeMapVtr = retcodeMap_itr.asValueMap().find(nodename);
		false_break(retcodeMapVtr != retcodeMap_itr.asValueMap().end());

		res = retcodeMapVtr->second.asString();
	} while (false);

	return res;
}

void sellMobileSystem::sendTipsMessage(const char* msg, unsigned int len)
{
	char * tmpBuf = new char[len + 1];
	sprintf_s(tmpBuf, len + 1, "%s", msg);
	::PostMessage(m_mainHwnd, UM_TIPS_MESSAGE, (UINT_PTR)tmpBuf, 0);
}

const char* sellMobileSystem::getTradeTypeWithOrderNo(const char* orderNo)
{
	if (nullptr == orderNo)
		return "";

	size_t orderLen = strlen(orderNo);
	if (18 == orderLen)
	{
		if ('1' == orderNo[0] && '0' <= orderNo[1] && '5' >= orderNo[1])
		{
			return "WXPAY.MICROPAY";
		}
	}
	return "ALIPAY.MICROPAY";
}