#include "stdafx.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include <vector>
#include <algorithm>
#include <functional>
#include "md5.h"
#include "tinyxml2.h"
#include "commonMicro.h"

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
}

void sellMobileSystem::setMchInfo(const char* id, const char* key)
{
	m_mch_id = id;
	m_mch_key = key;
}

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

		bool bRet = false;
		string retcode = "-9999";
		do
		{
			tinyxml2::XMLElement *rootEle = pDoc->RootElement();
			ValueMap map = getValueMapFromXmlElement(rootEle);
			Value xmlValue = map["xml"];
			empty_break(xmlValue.getType() == Value::Type::MAP);

			auto retcode_itr = xmlValue.asValueMap().find("retcode");
			empty_break(retcode_itr != xmlValue.asValueMap().end());

			auto retcodeVtr = retcode_itr->second;
			empty_break(retcodeVtr.getType() == Value::Type::VECTOR);

			auto retcodeMap_itr = retcodeVtr.asValueVector()[0];
			empty_break(retcodeMap_itr.getType() == Value::Type::MAP);

			auto retcodeMapVtr = retcodeMap_itr.asValueMap().find("retcode");
			empty_break(retcodeMapVtr != retcodeMap_itr.asValueMap().end());

			retcode = retcodeMapVtr->second.asString();
			bRet = true;
		} while (false);

		if (bRet)
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret || -2010 == nret)
			{
				
			}
			else
			{
				setState(sellState::fail);
			}
		}
	};
	curlManager::instance()->request(m_api["orderquery"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
}

bool sellMobileSystem::requestMicropay()
{
	char randChar[32] = {0};
	sprintf_s(randChar, "121775250120070233368%d", random(99999999));
	m_order_no = randChar;
	
	m_nonce_str = "5K8264ILTKCH16CQ";

	map<string, string> params;
	params["mch_id"] = m_mch_id;
	params["trade_type"] = "WXPAY.MICROPAY";
	params["out_trade_no"] = m_order_no;
	params["total_fee"] = "1";
	params["auth_code"] = "134547618301809295";
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

		bool bRet = false;
		string retcode="-9999";
		do
		{
			tinyxml2::XMLElement *rootEle = pDoc->RootElement();
			ValueMap map = getValueMapFromXmlElement(rootEle);
			Value xmlValue = map["xml"];
			empty_break(xmlValue.getType() == Value::Type::MAP);

			auto retcode_itr = xmlValue.asValueMap().find("retcode");
			empty_break(retcode_itr != xmlValue.asValueMap().end());

			auto retcodeVtr = retcode_itr->second;
			empty_break(retcodeVtr.getType() == Value::Type::VECTOR);
			
			auto retcodeMap_itr = retcodeVtr.asValueVector()[0];
			empty_break(retcodeMap_itr.getType() == Value::Type::MAP);

			auto retcodeMapVtr = retcodeMap_itr.asValueMap().find("retcode");
			empty_break(retcodeMapVtr != retcodeMap_itr.asValueMap().end());

			retcode = retcodeMapVtr->second.asString();
			bRet = true;
		} while (false);

		if (bRet)
		{
			int nret = atoi(retcode.c_str());
			if (0 <= nret || -2010 == nret)
			{
				//requestOrderQuery();
			}
			else
			{
				setState(sellState::fail);
			}
		}
	};
	curlManager::instance()->request(m_api["micropay"].c_str(), mapToXml(params).c_str(), CURL_METHOD_POST, responseCallback);
	setState(sellState::paying);

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

