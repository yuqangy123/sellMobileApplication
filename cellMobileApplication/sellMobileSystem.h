#pragma once
#include <string>
#include <map>


#define sellMobileSystemInstance sellMobileSystem::instance()
class sellMobileSystem
{
public:
	sellMobileSystem();
	~sellMobileSystem();
	static sellMobileSystem* instance();
	bool requestMicropay();
	void setMchInfo(const char* id, const char* key);

protected:
	void init();
	std::string makeSign(const std::map<std::string, std::string>& paramsMap);

protected:
	std::string mch_id;
	std::string mch_key;
};

