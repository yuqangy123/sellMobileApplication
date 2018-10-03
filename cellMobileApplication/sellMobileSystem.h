#pragma once
#include <string>
#include <map>
#include "tinyxml2.h"

enum sellState 
{
	none = 0,
	paying,
	orderquerying,
	refunding,
	refundquerying,
	downloadOrder,
	fail,
};

#define sellMobileSystemInstance sellMobileSystem::instance()
class sellMobileSystem
{
public:
	sellMobileSystem();
	~sellMobileSystem();
	static sellMobileSystem* instance();
	bool requestMicropay(HWND objHwnd, const char* order_no, const char* auth_code);
	bool requestOrderQuery();
	bool requestRefundOrder();
	bool requestRefundQuery();
	bool requestDownloadOrder();

	void setMchInfo(const char* id, const char* key);
	sellState getState() { return m_state; }
	void setMainDialogHwnd(HWND hnd);

protected:
	void init();
	void setState(sellState st) { m_state = st; };
	std::string makeSign(const std::map<std::string, std::string>& paramsMap);
	std::string mapToXml(const std::map<std::string, std::string>& paramsMap);
	std::string getXmlNode(tinyxml2::XMLDocument *pDoc, const char* nodename);
	void sendTipsMessage(const char* msg, unsigned int len);

protected:
	std::string m_mch_id;
	std::string m_mch_key;
	std::string m_order_no;
	std::string m_nonce_str;

	std::map<std::string, std::string> m_api;

	sellState m_state;

	HWND m_mainHwnd;
	HWND m_microPayHwnd;
};

