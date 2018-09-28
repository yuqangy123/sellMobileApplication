#pragma once
#include <string>
#include <map>

enum sellState 
{
	none = 0,
	paying,
	orderquerying,
	fail,
};

#define sellMobileSystemInstance sellMobileSystem::instance()
class sellMobileSystem
{
public:
	sellMobileSystem();
	~sellMobileSystem();
	static sellMobileSystem* instance();
	bool requestMicropay();
	bool requestOrderQuery();
	void setMchInfo(const char* id, const char* key);
	sellState getState() { return m_state; }

protected:
	void init();
	void setState(sellState st) { m_state = st; };
	std::string makeSign(const std::map<std::string, std::string>& paramsMap);
	std::string mapToXml(const std::map<std::string, std::string>& paramsMap);

protected:
	std::string m_mch_id;
	std::string m_mch_key;
	std::string m_order_no;
	std::string m_nonce_str;

	std::map<std::string, std::string> m_api;

	sellState m_state;
};

