#include "stdafx.h"
#include "curlManager.h"
#include <pthread.h>
#include "commonMicro.h"
#include <functional>

#include <openssl/crypto.h>
#include <time.h>
#include <curl.h>
#include <string>
#include "cellMobileApplicationDlg.h"

#include "FileUnit.h"

static curlManager* g_curManagerInstance = nullptr;

/* we have this global to let the callback get easy access to it */
static pthread_mutex_t *g_lockarray;
static pthread_mutex_t g_mutex;
static pthread_cond_t g_work_cond;//条件变量





static void lock_callback(int mode, int type, char *file, int line)
{
	(void)file;
	(void)line;
	if (mode & CRYPTO_LOCK) {
		pthread_mutex_lock(&(g_lockarray[type]));
	}
	else {
		pthread_mutex_unlock(&(g_lockarray[type]));
	}
}

static pthread_t thread_id(void)
{
	unsigned long ret;
	pthread_t ptr = pthread_self();
	return ptr;
}

static void init_locks(void)
{
	int i;

	g_lockarray = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() *
		sizeof(pthread_mutex_t));
	for (i = 0; i<CRYPTO_num_locks(); i++) {
		pthread_mutex_init(&(g_lockarray[i]), NULL);
	}

	CRYPTO_set_id_callback((unsigned long(*)())thread_id);
	CRYPTO_set_locking_callback((void(*)())lock_callback);
}

static void kill_locks(void)
{
	int i;

	CRYPTO_set_locking_callback(NULL);
	for (i = 0; i<CRYPTO_num_locks(); i++)
		pthread_mutex_destroy(&(g_lockarray[i]));

	OPENSSL_free(g_lockarray);
}



static void *wait_task_thread(void *url)
{
	while (true)
	{
		pthread_mutex_lock(&g_mutex);
		pthread_cond_wait(&g_work_cond, &g_mutex);

		const curl_http_args_st* const st = curlManager::instance()->getCurlHttpArgs();
		if(st->file_name[0] == 0)
			curlManager::instance()->curl_http_post();
		else
			curlManager::instance()->curl_http_download_file();
		
		pthread_mutex_unlock(&g_mutex);

	}

	pthread_cond_destroy(&g_work_cond);
	pthread_mutex_destroy(&g_mutex);
}


size_t curl_write_data_cb(void *buffer, size_t size, size_t nmemb, void *stream)
{
	int len = size * nmemb;
	struct curl_http_args_st *args = (struct curl_http_args_st*)stream;

	if (stream)
	{
		if (args->file_name[0])	// 要写文件
		{
			if (!args->file_fd)
			{
				fopen_s(&args->file_fd, args->file_name, "wb");
				if (args->file_fd == NULL)
				{
					printf("%s[%d]: open file[%s] failed!!\n", __FUNCTION__, __LINE__, args->file_name);
					return 0;
				}
			}
			fwrite(buffer, size, nmemb, args->file_fd);
		}
		args->data = (char*)realloc(args->data, args->data_len + len + 1);	// 多分配一个字节, 以保存\0 
		if (!args->data)
		{
			printf("%s[%d]: realloc failed!!\n", __FUNCTION__, __LINE__);
			return 0;
		}
		memcpy(args->data + args->data_len, buffer, len);
		args->data_len += len;
	}

	return len;
}

int my_progress_func(char *progress_data,
	double t, /* dltotal */
	double d, /* dlnow */
	double ultotal,
	double ulnow)
{
	printf("%s %g / %g (%g %%)\n", progress_data, d, t, d*100.0 / t);
	return 0;
}


int curlManager::curl_http_post()
{
	int ret = -1;
	do
	{
		false_break(curlManagerInstance->isRunning());

		struct curl_http_args_st *args = curlManager::instance()->m_curl_args;
		false_break(args);


		//创建curl对象 
		CURL *curl;
		CURLcode return_code;
		struct curl_httppost *formpost = NULL;	// POST 需要的参数
		struct curl_httppost *lastptr = NULL;

		int post_type = 1; // POST 可以有三种方法

		// 如果要保存为文件, 先建立文件目录
		//if (args->file_name) create_dir(args->file_name);

		//curl初始化 
		curl = curl_easy_init();
		if (!curl)
		{
			printf("%s[%d]: curl easy init failed\n", __FUNCTION__, __LINE__);
			false_break(false);
		}

		if (strncmp(args->url, "https://", 8) == 0)
		{
#if 1	
			// 方法1, 设定为不验证证书和HOST
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#else
			// 方法2, 设定一个SSL判别证书
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-cert.pem"); 	// TODO: 设置一个证书文件
#endif 
		}

		curl_easy_setopt(curl, CURLOPT_HEADER, 0);	//设置httpheader 解析, 不需要将HTTP头写传入回调函数

		curl_easy_setopt(curl, CURLOPT_URL, args->url);	//设置远端地址 

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);	// TODO: 打开调试信息

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);	//设置允许302  跳转

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_cb); 	//执行写入文件流操作 的回调函数

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, args);	// 设置回调函数的第4 个参数

		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);	//设备为ipv4类型

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); 	//设置连接超时，单位s, CURLOPT_CONNECTTIMEOUT_MS 毫秒

		// curl_easy_setopt(curl,CURLOPT_TIMEOUT, 5);			// 整个CURL 执行的时间, 单位秒, CURLOPT_TIMEOUT_MS毫秒

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);		//linux多线程情况应注意的设置(防止curl被alarm信号干扰)

		if (post_type == 1)
		{
			// 方法1, 普通的POST , application/x-www-form-urlencoded
			curl_easy_setopt(curl, CURLOPT_POST, 1);		// 设置 为POST 方法
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->post_data);		// POST 的数据内容
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(args->post_data));	// POST的数据长度, 可以不要此选项
		}
		else if (post_type == 2)
		{
			//方法2, multipart/formdata请求, POST args->post_data 中的数据, 也可以是将文件内容读取到post_data中		
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);	// 设置POST 参数
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, args->post_data, CURLFORM_CONTENTSLENGTH, strlen(args->post_data), CURLFORM_END);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		}
		else if (post_type == 3)
		{
			//添加内容Content-Disposition: form-data; name="reqformat"....plain 
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);// 设置POST 参数
			// 添加上传文件,  Content-Disposition: form-data; name="file"; filename="1.jpg"; filename为默认的名字, content-type 为默认curl识别的
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, args->post_file, CURLFORM_END);
			// 添加上传文件,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filename为指定的名字, content-type 为默认curl识别的
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_END); 
			// 添加上传文件,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filename为指定的名字, content-type为指定的类型
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_CONTENTTYPE, "image/jpeg", CURLFORM_END);

			// 引用页:  http://blog.csdn.net/zxgfa/article/details/8302059
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		}

		return_code = curl_easy_perform(curl);
		if (CURLE_OK != return_code)
		{
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(return_code));
			ret = 0;
			char cmsg[64] = { 0 };
			sprintf_s(cmsg, "request error(%d)", return_code);
			stackHttpCallback(std::string(cmsg));
		}

		if (args->file_fd)		// 若需要再次处理写入的文件, 在此可以直接使用
		{
			//关闭文件流
			fclose(args->file_fd);
		}
		if (args->data)		// 若要对返回的内容进行处理, 可在此处理
		{
			std::string ansi = KS_UTF8_to_ANSI(args->data);
			const char* xmlend = "</xml>";
			std::size_t found = ansi.find(xmlend);
			if (found != std::string::npos)
				ansi = ansi.substr(0, found+strlen(xmlend));
			stackHttpCallback(ansi);
			
			free(args->data);
			args->data = NULL;
		}

		curl_easy_cleanup(curl);

		if (post_type == 2 || post_type == 3)	// 用这两种方法需要释放POST数据. 
			curl_formfree(formpost);
	} while (false);

	safe_delete(m_curl_args);
	setRunning(false);

	return ret;
}

int curlManager::curl_http_download_file()
{
	int ret = -1;
	do
	{
		false_break(curlManagerInstance->isRunning());

		struct curl_http_args_st *args = curlManager::instance()->m_curl_args;
		false_break(args);


		//创建curl对象 
		CURL *curl;
		CURLcode return_code;
		// 如果要保存为文件, 先建立文件目录
		//if (args->file_name) create_dir(args->file_name);

		//curl初始化 
		curl = curl_easy_init();
		if (!curl)
		{
			printf("%s[%d]: curl easy init failed\n", __FUNCTION__, __LINE__);
			false_break(false);
		}

		false_break(args->file_name[0] != 0);

		FileUnitInstance->ForceDirectories( FileUnitInstance->ExtractFilePath(args->file_name) );
		
		fopen_s(&args->file_fd, args->file_name, "wb");
		false_break(args->file_fd);
		/*
		// 方法1, 普通的POST , application/x-www-form-urlencoded
		curl_easy_setopt(curl, CURLOPT_POST, 1);		// 设置 为POST 方法
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->post_data);		// POST 的数据内容
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(args->post_data));	// POST的数据长度, 可以不要此选项
		*/
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);	//设置httpheader 解析, 不需要将HTTP头写传入回调函数

		curl_easy_setopt(curl, CURLOPT_URL, args->url);	//设置远端地址 

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);	// TODO: 打开调试信息

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);	//设置允许302  跳转

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_cb); 	//执行写入文件流操作 的回调函数

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, args);	// 设置回调函数的第4 个参数

		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);	//设备为ipv4类型

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); 	//设置连接超时，单位s, CURLOPT_CONNECTTIMEOUT_MS 毫秒

		// curl_easy_setopt(curl,CURLOPT_TIMEOUT, 5);			// 整个CURL 执行的时间, 单位秒, CURLOPT_TIMEOUT_MS毫秒

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);		//linux多线程情况应注意的设置(防止curl被alarm信号干扰)

		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);//设为false 下面才能设置进度响应函数
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_progress_func);//进度响应函数
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, "*");//指定的参数将作为CURLOPT_PROGRESSFUNCTION指定函数的参数.

		return_code = curl_easy_perform(curl);
		if (CURLE_OK != return_code)
		{
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(return_code));
			char cmsg[64] = { 0 };
			sprintf_s(cmsg, "request error(%d)", return_code);
			stackHttpCallback(std::string(cmsg));
			ret = 0;
		}

		if (args->file_fd)		// 若需要再次处理写入的文件, 在此可以直接使用
		{
			//关闭文件流
			fclose(args->file_fd);
		}
		if (args->data)		// 若要对返回的内容进行处理, 可在此处理
		{
			std::string ansi(args->file_name);
			stackHttpCallback(ansi);

			free(args->data);
			args->data = NULL;
		}

		curl_easy_cleanup(curl);

	} while (false);

	safe_delete(m_curl_args);
	setRunning(false);

	return ret;
}

curlManager::curlManager()
{
	/* Must initialize libcurl before any threads are started */
	curl_global_init(CURL_GLOBAL_ALL);
}

curlManager::~curlManager()
{
	curl_global_cleanup();
}

curlManager* curlManager::instance()
{
	if (nullptr == g_curManagerInstance)
	{
		g_curManagerInstance = new curlManager();
		g_curManagerInstance->init();
	}
	return g_curManagerInstance;
}

void curlManager::init()
{
	pthread_mutex_init(&g_mutex, NULL);
	pthread_cond_init(&g_work_cond, NULL);

	pthread_t				waitTid;
	int error = pthread_create(&waitTid,
		NULL, /* default attributes please */
		wait_task_thread,
		NULL);

	if (0 != error)
	{
		fprintf(stderr, "Couldn't run mainWait thread errno %d\n", error);
	}

	//error = pthread_join(tid, NULL);//可结合线程，阻塞
	pthread_detach(waitTid);//分离线程，非阻塞， 或者在线程里调用pthread_detach(pthread_self());
}

bool curlManager::request(const char* url, const char* requestData, curl_method requestType, std::function<void(const std::string& data)>* callback)
{
	//init_locks();

	if (isRunning())
	{
		printf("curlManager request error, is running\r\n");
		return false;
	}

	safe_delete(m_curl_args);
	m_curl_args = new curl_http_args_st();

	strncpy_s(m_curl_args->url, url, sizeof(m_curl_args->url));
	strncpy_s(m_curl_args->post_data, requestData, sizeof(m_curl_args->post_data));
	m_curl_args->curl_method = requestType;
	m_curl_args->requestCallback = callback;

	
	pthread_mutex_lock(&g_mutex);

	setRunning(true);

	pthread_cond_signal(&g_work_cond);

	pthread_mutex_unlock(&g_mutex);

	//kill_locks();

	return true;
}

bool curlManager::download(const char* url, const char* requestData, const char* savefile, std::function<void(const std::string& data)>* callback)
{
	if (isRunning())
	{
		printf("curlManager download error, is running\r\n");
		return false;
	}

	safe_delete(m_curl_args);
	m_curl_args = new curl_http_args_st();

	strncpy_s(m_curl_args->url, url, sizeof(m_curl_args->url));
	strncpy_s(m_curl_args->post_data, requestData, sizeof(m_curl_args->post_data));
	strncpy_s(m_curl_args->file_name, savefile, sizeof(m_curl_args->file_name));
	m_curl_args->requestCallback = callback;


	pthread_mutex_lock(&g_mutex);

	setRunning(true);

	pthread_cond_signal(&g_work_cond);

	pthread_mutex_unlock(&g_mutex);

	//kill_locks();

	return true;
}

struct stackHttpCallback_args
{
	stackHttpCallback_args() :callback(nullptr){};
	std::function<void(const std::string& data)>* callback;
	std::string msg;
};
void *stack_http_callback_thread(void *param)
{
	stackHttpCallback_args* args = (stackHttpCallback_args*)param;
	if (args->callback)
	{
		(*args->callback)(args->msg);
		safe_delete(args->callback);
	}
	safe_delete(args);
	return 0;
}
void curlManager::stackHttpCallback(std::string& msg)
{
	pthread_t tid;
	stackHttpCallback_args* args = new stackHttpCallback_args();
	args->callback = m_curl_args->requestCallback;
	args->msg = msg;

	int error = pthread_create(&tid,
		NULL, /* default attributes please */
		stack_http_callback_thread,
		(void *)args);

	if (0 != error)
	{
		fprintf(stderr, "Couldn't run stackHttpCallback thread errno %d\n", error);
	}

	pthread_detach(tid);
}