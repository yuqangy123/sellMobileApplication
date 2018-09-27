#include "stdafx.h"
#include "sellMobileSystem.h"
#include "curlManager.h"
#include <vector>
#include <algorithm>
#include "md5.h"

using namespace std;

static sellMobileSystem* g_sellMobileInstance = nullptr;
sellMobileSystem::sellMobileSystem()
{
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

}

void sellMobileSystem::setMchInfo(const char* id, const char* key)
{
	mch_id = id;
	mch_key = key;
}

bool sellMobileSystem::requestMicropay()
{
	//mch_id: 1000000013
	//mch_key : qu9k3vxsy2uc69u86iybirpu14coj34z

	if (mch_id.empty() || mch_key.empty())
	{
		printf("mch info is empty");
		return false;
	}


	map<string, string> params;
	params["mch_id"] = "1000000013";
	params["trade_type"] = "WXPAY.MICROPAY";
	params["out_trade_no"] = "1217752501201407033233368018";
	params["total_fee"] = "888";
	params["auth_code"] = "120061098828009406";
	params["body"] = "春季服装";
	params["spbill_create_ip"] = "123.12.12.123";
	params["nonce_str"] = "5K8264ILTKCH16CQ";
	params["sign"] = makeSign(params);
	

	curlManager::instance()->request("https://api.speedpos.cn/micropay", "<xml><retcode>0</retcode></xml>", CURL_METHOD_POST);
}

bool comp(const string &a, const string &b){
	
	int av = a.at(0);
	int bv = b.at(0);
	return av > bv;
}
string sellMobileSystem::makeSign(const std::map<std::string, std::string>& paramsMap)
{
	//参数按字典升序排序，生成urlparam
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
		if (!itr->compare("sign") && !itr->empty() && !itr->compare("_url"))
		{
			string& key = *itr;
			
			//paramsMap[""];
			//sprintf_s(parone, "%s=%s&", itr, *);
			urlParams += parone;
		}
	}

	//在后面加上商户key
	if (!urlParams.empty())
	{
		urlParams += "&key=";
		urlParams += mch_key;
		//urlParams = urlParams.substr(0, urlParams.size() - 1);
	}
	else
	{
		urlParams += mch_key;
	}

	MD5 md5;
	md5.update(urlParams);
	string res = md5.toString();
	transform(res.begin(), res.end(), res.begin(), ::toupper);

	return res;
}