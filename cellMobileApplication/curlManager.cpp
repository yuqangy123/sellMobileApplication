#include "stdafx.h"
#include "curlManager.h"
#include <pthread.h>



#include <openssl/crypto.h>
#include <time.h>
#include <curl.h>
#include <string>

static curlManager* g_curManagerInstance = nullptr;

/* we have this global to let the callback get easy access to it */
static pthread_mutex_t *g_lockarray;
static pthread_mutex_t g_mutex;
static pthread_cond_t g_work_cond;//��������

#define safe_delete(p)		{if(p != nullptr){delete p; p = nullptr;};}
#define empty_break(obj)	{if(!obj) break;}



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

		curlManager::instance()->curl_http_post();
		
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
		if (args->file_name[0])	// Ҫд�ļ�
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
		args->data = (char*)realloc(args->data, args->data_len + len + 1);	// �����һ���ֽ�, �Ա���\0 
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
int curlManager::curl_http_post()
{
	int ret = -1;
	do
	{
		empty_break(curlManagerInstance->isRunning());

		struct curl_http_args_st *args = curlManager::instance()->m_curl_args;
		empty_break(args);


		//����curl���� 
		CURL *curl;
		CURLcode return_code;
		struct curl_httppost *formpost = NULL;	// POST ��Ҫ�Ĳ���
		struct curl_httppost *lastptr = NULL;

		int post_type = 1; // POST ���������ַ���

		// ���Ҫ����Ϊ�ļ�, �Ƚ����ļ�Ŀ¼
		//if (args->file_name) create_dir(args->file_name);

		//curl��ʼ�� 
		curl = curl_easy_init();
		if (!curl)
		{
			printf("%s[%d]: curl easy init failed\n", __FUNCTION__, __LINE__);
			empty_break(false);
		}

		if (strncmp(args->url, "https://", 8) == 0)
		{
#if 1	
			// ����1, �趨Ϊ����֤֤���HOST
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#else
			// ����2, �趨һ��SSL�б�֤��
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-cert.pem"); 	// TODO: ����һ��֤���ļ�
#endif 
		}

		curl_easy_setopt(curl, CURLOPT_HEADER, 0);	//����httpheader ����, ����Ҫ��HTTPͷд����ص�����

		curl_easy_setopt(curl, CURLOPT_URL, args->url);	//����Զ�˵�ַ 

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);	// TODO: �򿪵�����Ϣ

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);	//��������302  ��ת

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_cb); 	//ִ��д���ļ������� �Ļص�����

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, args);	// ���ûص������ĵ�4 ������

		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);	//�豸Ϊipv4����

		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); 	//�������ӳ�ʱ����λs, CURLOPT_CONNECTTIMEOUT_MS ����

		// curl_easy_setopt(curl,CURLOPT_TIMEOUT, 5);			// ����CURL ִ�е�ʱ��, ��λ��, CURLOPT_TIMEOUT_MS����

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);		//linux���߳����Ӧע�������(��ֹcurl��alarm�źŸ���)

		if (post_type == 1)
		{
			// ����1, ��ͨ��POST , application/x-www-form-urlencoded
			curl_easy_setopt(curl, CURLOPT_POST, 1);		// ���� ΪPOST ����
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->post_data);		// POST ����������
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(args->post_data));	// POST�����ݳ���, ���Բ�Ҫ��ѡ��
		}
		else if (post_type == 2)
		{
			//����2, multipart/formdata����, POST args->post_data �е�����, Ҳ�����ǽ��ļ����ݶ�ȡ��post_data��		
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);	// ����POST ����
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, args->post_data, CURLFORM_CONTENTSLENGTH, strlen(args->post_data), CURLFORM_END);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		}
		else if (post_type == 3)
		{
			//�������Content-Disposition: form-data; name="reqformat"....plain 
			curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);// ����POST ����
			// ����ϴ��ļ�,  Content-Disposition: form-data; name="file"; filename="1.jpg"; filenameΪĬ�ϵ�����, content-type ΪĬ��curlʶ���
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, args->post_file, CURLFORM_END);
			// ����ϴ��ļ�,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filenameΪָ��������, content-type ΪĬ��curlʶ���
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_END); 
			// ����ϴ��ļ�,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filenameΪָ��������, content-typeΪָ��������
			//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_CONTENTTYPE, "image/jpeg", CURLFORM_END);

			// ����ҳ:  http://blog.csdn.net/zxgfa/article/details/8302059
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		}

		return_code = curl_easy_perform(curl);
		if (CURLE_OK != return_code)
		{
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(return_code));
			ret = 0;
		}

		if (args->file_fd)		// ����Ҫ�ٴδ���д����ļ�, �ڴ˿���ֱ��ʹ��
		{
			//�ر��ļ���
			fclose(args->file_fd);
		}
		if (args->data)		// ��Ҫ�Է��ص����ݽ��д���, ���ڴ˴���
		{
			//printf("data_len:%d\n%s\n", args->data_len, args->data);
			if (nullptr != m_requestCallback)
			{
				std::string responseData;
				(*m_requestCallback)(responseData);
			}
			free(args->data);
			args->data = NULL;
		}

		curl_easy_cleanup(curl);

		if (post_type == 2 || post_type == 3)	// �������ַ�����Ҫ�ͷ�POST����. 
			curl_formfree(formpost);
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

	//error = pthread_join(tid, NULL);//�ɽ���̣߳�����
	pthread_detach(waitTid);//�����̣߳��������� �������߳������pthread_detach(pthread_self());
}

void curlManager::test()
{
	
}

bool curlManager::request(const char* url, const char* requestData, curl_method requestType, https_request_callback* callback)
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
	m_requestCallback = callback;

	
	pthread_mutex_lock(&g_mutex);

	setRunning(true);

	pthread_cond_signal(&g_work_cond);

	pthread_mutex_unlock(&g_mutex);

	//kill_locks();

	return true;
}