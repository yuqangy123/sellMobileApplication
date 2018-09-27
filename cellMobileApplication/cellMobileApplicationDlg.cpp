
// cellMobileApplicationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "cellMobileApplication.h"
#include "cellMobileApplicationDlg.h"
#include "afxdialogex.h"

#include "curl.h"
#include <pthread.h>


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
{	int  curl_method;	// curl ��������,
	enum curl_method;
	char url[CURL_URL_MAX_LEN];		// URL 		
	char file_name[CURL_NAME_MAX_LEN];	// �������ݱ���Ϊ�ļ�
	FILE *file_fd;						// �ļ���ָ���������, �������Ҫ�ֶ�fclose
	int  data_len;						// �ļ����ݱ������ڴ��еĳ���	
	char *data;							// �ļ����ݱ������ڴ��е�ָ��, ������ֶ�free  	
	char post_data[CURL_BUF_MAX_LEN];	// POST ������	
	char post_file[CURL_NAME_MAX_LEN];	// POST �ļ���
};

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
					curl_printf("%s[%d]: open file[%s] failed!!\n", __FUNCTION__, __LINE__, args->file_name);
					return 0;
				}
			}
			fwrite(buffer, size, nmemb, args->file_fd);
		}
		args->data = (char*)realloc(args->data, args->data_len + len + 1);	// �����һ���ֽ�, �Ա���\0 
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
	//����curl���� 
	CURL *curl;
	CURLcode return_code;
	struct curl_httppost *formpost = NULL;	// POST ��Ҫ�Ĳ���
	struct curl_httppost *lastptr = NULL;
	int ret = -1;
	int post_type = 1; // POST ���������ַ���

	// ���Ҫ����Ϊ�ļ�, �Ƚ����ļ�Ŀ¼
	//if (args->file_name) create_dir(args->file_name);

	//curl��ʼ�� 
	curl = curl_easy_init();
	if (!curl)
	{
		curl_printf("%s[%d]: curl easy init failed\n", __FUNCTION__, __LINE__);
		return ret;;
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
		curl_printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(return_code));
		ret = 0;
	}

	if (args->file_fd)		// ����Ҫ�ٴδ���д����ļ�, �ڴ˿���ֱ��ʹ��
	{
		//�ر��ļ���
		fclose(args->file_fd);
	}
	if (args->data)		// ��Ҫ�Է��ص����ݽ��д���, ���ڴ˴���
	{
		curl_printf("data_len:%d\n%s\n", args->data_len, args->data);
		free(args->data);
		args->data = NULL;
	}

	curl_easy_cleanup(curl);

	if (post_type == 2 || post_type == 3)	// �������ַ�����Ҫ�ͷ�POST����. 
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


// CcellMobileApplicationDlg ��Ϣ�������


BOOL CcellMobileApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CcellMobileApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CcellMobileApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CcellMobileApplicationDlg::OnBnClickedOk()
{
	
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
