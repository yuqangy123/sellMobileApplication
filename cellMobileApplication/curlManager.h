#pragma once
#include <string>


typedef void(*https_request_callback)(const std::string& data);


#define CURL_BUF_MAX_LEN  4096
#define CURL_NAME_MAX_LEN 128
#define CURL_URL_MAX_LEN  128 

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
	}

	int  curl_method;	// curl ��������,enum curl_method
	char url[CURL_URL_MAX_LEN];		// URL 

	char file_name[CURL_NAME_MAX_LEN];	// �������ݱ���Ϊ�ļ�
	FILE *file_fd;						// �ļ���ָ���������, �������Ҫ�ֶ�fclose

	int  data_len;						// �ļ����ݱ������ڴ��еĳ���
	char *data;							// �ļ����ݱ������ڴ��е�ָ��, ������ֶ�free 

	char post_data[CURL_BUF_MAX_LEN];	// POST ������
	char post_file[CURL_NAME_MAX_LEN];	// POST �ļ���
};

#define curlManagerInstance curlManager::instance()
class curlManager
{
public:
	curlManager();
	~curlManager();

	static curlManager* instance();
	bool request(const char* url, const char* requestData, curl_method requestType, https_request_callback* callback = nullptr);
	void test();
	int curl_http_post();
	bool isRunning(){ return m_running; }
	

protected:
	void init();
	void setRunning(bool b){ m_running = b; }

protected:
	https_request_callback* m_requestCallback = nullptr;
	curl_http_args_st*		m_curl_args = nullptr;
	bool					m_running = false;
	
};

