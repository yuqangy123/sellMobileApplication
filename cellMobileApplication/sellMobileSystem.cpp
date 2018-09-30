#include "stdafx.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include <vector>
#include <algorithm>
#include <functional>
#include "md5.h"
#include "commonMicro.h"
#include "cellMobileApplicationDlg.h"

using namespace std;

static const char* domain = "https://api.speedpos.cn/";
static const char* domain_test = "http://betaapi.speedpos.snsshop.net/";

static sellMobileSystem* g_sellMobileInstance = nullptr;
sellMobileSystem::sellMobileSystem()
{
	m_state = sellState::none;
}


sellMobileSystem::~sellMobileSystem()
{
}


sellMobileSystem* sellMobileSystem::instance()
{
	if (nullptr == g_sellMobileInstance)
	{
		g_sellMobileInstance = new sellMobileSystem();
		g_sellMobileInstance->init();
	}
	return g_sellMobileInstance;
}

void sellMobileSystem::init()
{
	string sysDomain(domain_test);
	char parone[256] = { 0 };

	sprintf_s(parone, "%smicropay", sysDomain.c_str());
	m_api["micropay"] = parone;

	sprintf_s(parone, "%sorderquery", sysDomain.c_str());
	m_api["orderquery"] = parone;

	sprintf_s(parone, "%srefundorder", sysDomain.c_str());
	m_api["refundorder"] = parone;

	sprintf_s(parone, "%srefundquery", sysDomain.c_str());
	m_api["refundquery"] = parone;
	
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

//提交刷卡支付
bool sellMobileSystem::requestOrderQuery()
{
	if (sellState::paying != getState())
	{
		setState(sellState::none);
		return false;
	}

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["out_trade_no"] = m_order_no;
	params["nonce_str"] = m_nonce_str;
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		printf("%d,%s", data.length(), data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			printf("xml 解析失败");
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
					setState(sellState::none);
					::PostMessage(m_mainHwnd, UM_PAY_SUCCESS_NOTIFY, 0, 0);
				}
			}
			else
			{
				setState(sellState::none);
				sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
			//::PostMessage(m_mainHwnd, UM_ORDER_QUERY, 0, 0);
		}
		
	};
	curlManager::instance()->request(m_api["orderquery"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	setState(sellState::orderquerying);
}

//查询订单
bool sellMobileSystem::requestMicropay()
{
	char randChar[32] = {0};
	long rd = getRandom(99999999);
	sprintf_s(randChar, "121775250120070233368%d", getRandom(99999999));
	m_order_no = randChar;
	
	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["trade_type"] = "WXPAY.MICROPAY";
	params["out_trade_no"] = m_order_no;
	params["total_fee"] = "1";
	params["auth_code"] = "135009603620112104";
	params["body"] = "chunjifuzhuang";
	params["spbill_create_ip"] = "123.12.12.123";
	params["nonce_str"] = m_nonce_str;
	params["sign"] = makeSign(params);
	
	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>(); 
	*responseCallback = [this](const std::string& data)
	{
		printf("%d,%s", data.length(), data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			printf("xml 解析失败");
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret || -2010 == nret)
			{
				::PostMessage(m_mainHwnd, UM_ORDER_QUERY, 0, 0);
			}
			else
			{
				setState(sellState::none);
				sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
		}
	};
	curlManager::instance()->request(m_api["micropay"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	setState(sellState::paying);	
	return true;
}

//申请退款
bool sellMobileSystem::requestRefundOrder()
{
	char randChar[32] = { 0 };
	long rd = getRandom(99999999);
	sprintf_s(randChar, "121775250120070233368%d", rd);
	m_order_no = randChar;

	char out_refund_no[32] = { 0 };
	rd = getRandom(99999999);
	sprintf_s(out_refund_no, "12w2s753s70233368%d", rd);

	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["nonce_str"] = m_nonce_str;
	params["out_trade_no"] = m_order_no;
	params["out_refund_no"] = out_refund_no;//由三部分组成
	params["refund_fee"] = "100";
	params["total_fee"] = "100";
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		printf("%d,%s", data.length(), data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			printf("xml 解析失败");
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret)
			{
				//::PostMessage(m_mainHwnd, UM_ORDER_QUERY, 0, 0);
			}
			else
			{
				setState(sellState::none);
				sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
		}
	};
	curlManager::instance()->request(m_api["refundorder"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	setState(sellState::refunding);
	return true;
}

//查询退款
bool sellMobileSystem::requestRefundQuery()
{
	char randChar[32] = { 0 };
	long rd = getRandom(99999999);
	sprintf_s(randChar, "121775250120070233368%d", rd);
	m_order_no = randChar;

	char refund_no[32] = { 0 };
	rd = getRandom(99999999);
	sprintf_s(refund_no, "12w2s753s70233368%d", rd);

	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["nonce_str"] = m_nonce_str;
	params["out_trade_no"] = m_order_no;
	params["refund_no"] = refund_no;//由三部分组成
	params["sign"] = makeSign(params);

	function<void(const std::string& data)>* responseCallback = new function<void(const std::string& data)>();
	*responseCallback = [this](const std::string& data)
	{
		printf("%d,%s", data.length(), data.c_str());
		tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
		tinyxml2::XMLError errorId = pDoc->Parse(data.c_str(), data.length());
		if (errorId != 0) {
			printf("xml 解析失败");
		}

		string retcode = getXmlNode(pDoc, "retcode");
		string retmsg = getXmlNode(pDoc, "retmsg");
		if (!retcode.empty())
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret || -2010 == nret)
			{
				//::PostMessage(m_mainHwnd, UM_ORDER_QUERY, 0, 0);
			}
			else
			{
				setState(sellState::none);
				sendTipsMessage(retmsg.c_str(), retmsg.length());
			}
		}
	};
	curlManager::instance()->request(m_api["refundquery"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	setState(sellState::refundquerying);
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