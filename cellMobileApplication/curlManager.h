#pragma once
#include <string>
#include <functional>




#define CURL_BUF_MAX_LEN  4096
#define CURL_NAME_MAX_LEN 256
#define CURL_URL_MAX_LEN  256 

enum curl_method
{
	CURL_METHOD_GET = 1,
	CURL_METHOD_POST = 2,
};

struct curl_http_args_st
{
	curl_http_args_st()
	{
		curl_method = 0;
		memset(url, 0x0, CURL_URL_MAX_LEN);
		memset(file_name, 0x0, CURL_NAME_MAX_LEN);
		file_fd = nullptr;
		data_len = 0;
		data = nullptr;
		memset(post_data, 0x0, CURL_BUF_MAX_LEN);
		memset(post_file, 0x0, CURL_NAME_MAX_LEN);
		requestCallback = nullptr;
	}

	int  curl_method;	// curl 方法命令,enum curl_method
	char url[CURL_URL_MAX_LEN];		// URL 

	char file_name[CURL_NAME_MAX_LEN];	// 返回数据保存为文件
	FILE *file_fd;						// 文件所指向的描述符, 用完后需要手动fclose

	int  data_len;						// 文件数据保存在内存中的长度
	char *data;							// 文件数据保存在内存中的指针, 用完后手动free 

	char post_data[CURL_BUF_MAX_LEN];	// POST 表单数据
	char post_file[CURL_NAME_MAX_LEN];	// POST 文件名

	std::function<void(const std::string& data)>* requestCallback;
};

#define curlManagerInstance curlManager::instance()
class curlManager
{
public:
	curlManager();
	~curlManager();

	static curlManager* instance();
	bool request(const char* url, const char* requestData, curl_method requestType, std::function<void(const std::string& data)>* callback = nullptr);
	bool download(const char* url, const char* requestData, const char* savefile, std::function<void(const std::string& data)>* callback = nullptr);
	int curl_http_post();
	int curl_http_download_file();
	bool isRunning(){ return m_running; }
	const curl_http_args_st* const getCurlHttpArgs() { return m_curl_args; }

protected:
	void init();
	void setRunning(bool b){ m_running = b; }
	void stackHttpCallback(std::string& msg);

protected:
	curl_http_args_st*		m_curl_args;
	bool					m_running;
	
};

