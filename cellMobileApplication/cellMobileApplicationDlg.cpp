
// cellMobileApplicationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "cellMobileApplication.h"
#include "cellMobileApplicationDlg.h"
#include "afxdialogex.h"

#include "sellMobileSystem.h"
#include "curlManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
typedef void CURL;
#define curl_printf printf
#define CURL_BUF_MAX_LEN  1024
#define CURL_NAME_MAX_LEN 128
#define CURL_URL_MAX_LEN  128
enum curl_method{	
	CURL_METHOD_GET  = 1,	CURL_METHOD_POST = 2,
}; 
struct curl_http_args_st
{	int  curl_method;	// curl 方法命令,
	enum curl_method;
	char url[CURL_URL_MAX_LEN];		// URL 		
	char file_name[CURL_NAME_MAX_LEN];	// 返回数据保存为文�?
	FILE *file_fd;						// 文件所指向的描述符, 用完后需要手动fclose
	int  data_len;						// 文件数据保存在内存中的长�?
	char *data;							// 文件数据保存在内存中的指�? 用完后手动free  	
	char post_data[CURL_BUF_MAX_LEN];	// POST 表单数据	
	char post_file[CURL_NAME_MAX_LEN];	// POST 文件�?
};

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
					curl_printf("%s[%d]: open file[%s] failed!!\n", __FUNCTION__, __LINE__, args->file_name);
					return 0;
				}
			}
			fwrite(buffer, size, nmemb, args->file_fd);
		}
		args->data = (char*)realloc(args->data, args->data_len + len + 1);	// 多分配一个字�? 以保存\0 
		if (!args->data)
		{
			curl_printf("%s[%d]: realloc failed!!\n", __FUNCTION__, __LINE__);
			return 0;
		}
		memcpy(args->data + args->data_len, buffer, len);
		args->data_len += len;
	}

	return len;
}
int curl_http_post(struct curl_http_args_st *args)
{
	//创建curl对象 
	CURL *curl;
	CURLcode return_code;
	struct curl_httppost *formpost = NULL;	// POST 需要的参数
	struct curl_httppost *lastptr = NULL;
	int ret = -1;
	int post_type = 1; // POST 可以有三种方�?

	// 如果要保存为文件, 先建立文件目�?
	//if (args->file_name) create_dir(args->file_name);

	//curl初始�?
	curl = curl_easy_init();
	if (!curl)
	{
		curl_printf("%s[%d]: curl easy init failed\n", __FUNCTION__, __LINE__);
		return ret;;
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
		curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-cert.pem"); 	// TODO: 设置一个证书文�?
#endif 
	}

	curl_easy_setopt(curl, CURLOPT_HEADER, 0);	//设置httpheader 解析, 不需要将HTTP头写传入回调函数

	curl_easy_setopt(curl, CURLOPT_URL, args->url);	//设置远端地址 

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);	// TODO: 打开调试信息

	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);	//设置允许302  跳转

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_data_cb); 	//执行写入文件流操�?的回调函�?

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, args);	// 设置回调函数的第4 个参�?

	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);	//设备为ipv4类型

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30); 	//设置连接超时，单位s, CURLOPT_CONNECTTIMEOUT_MS 毫秒

	// curl_easy_setopt(curl,CURLOPT_TIMEOUT, 5);			// 整个CURL 执行的时�? 单位�? CURLOPT_TIMEOUT_MS毫秒

	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);		//linux多线程情况应注意的设�?防止curl被alarm信号干扰)

	if (post_type == 1)
	{
		// 方法1, 普通的POST , application/x-www-form-urlencoded
		curl_easy_setopt(curl, CURLOPT_POST, 1);		// 设置 为POST 方法
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->post_data);		// POST 的数据内�?
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(args->post_data));	// POST的数据长�? 可以不要此选项
	}
	else if (post_type == 2)
	{
		//方法2, multipart/formdata请求, POST args->post_data 中的数据, 也可以是将文件内容读取到post_data�?	
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);	// 设置POST 参数
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, args->post_data, CURLFORM_CONTENTSLENGTH, strlen(args->post_data), CURLFORM_END);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	}
	else if (post_type == 3)
	{
		//添加内容Content-Disposition: form-data; name="reqformat"....plain 
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);// 设置POST 参数
		// 添加上传文件,  Content-Disposition: form-data; name="file"; filename="1.jpg"; filename为默认的名字, content-type 为默认curl识别�?
		//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, args->post_file, CURLFORM_END);
		// 添加上传文件,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filename为指定的名字, content-type 为默认curl识别�?
		//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_END); 
		// 添加上传文件,  //Content-Disposition: form-data; name="file"; filename="1.jpg".,   filename为指定的名字, content-type为指定的类型
		//curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, err_file, CURLFORM_FILENAME, "1.jpg", CURLFORM_CONTENTTYPE, "image/jpeg", CURLFORM_END);

		// 引用�?  http://blog.csdn.net/zxgfa/article/details/8302059
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	}

	return_code = curl_easy_perform(curl);
	if (CURLE_OK != return_code)
	{
		curl_printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(return_code));
		ret = 0;
	}

	if (args->file_fd)		// 若需要再次处理写入的文件, 在此可以直接使用
	{
		//关闭文件�?
		fclose(args->file_fd);
	}
	if (args->data)		// 若要对返回的内容进行处理, 可在此处�?
	{
		curl_printf("data_len:%d\n%s\n", args->data_len, args->data);
		free(args->data);
		args->data = NULL;
	}

	curl_easy_cleanup(curl);

	if (post_type == 2 || post_type == 3)	// 用这两种方法需要释放POST数据. 
		curl_formfree(formpost);

	return ret;
}
*/
CcellMobileApplicationDlg::CcellMobileApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcellMobileApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CcellMobileApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CcellMobileApplicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CcellMobileApplicationDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CcellMobileApplicationDlg 消息处理程序


BOOL CcellMobileApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标�? 当应用程序主窗口不是对话框时，框架将自动
	//  执行此操�?
	SetIcon(m_hIcon, TRUE);			// 设置大图�?
	SetIcon(m_hIcon, FALSE);		// 设置小图�?

	sellMobileSystemInstance->setMchInfo("1000000013", "qu9k3vxsy2uc69u86iybirpu14coj34z");;
	curlManagerInstance;


	return TRUE;
}

void CcellMobileApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示�?
HCURSOR CcellMobileApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CcellMobileApplicationDlg::OnBnClickedOk()
{
	if (sellState::none == sellMobileSystemInstance->getState())
		sellMobileSystemInstance->requestMicropay();
	else if (sellState::paying == sellMobileSystemInstance->getState())
		sellMobileSystemInstance->requestOrderQuery();

	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}
