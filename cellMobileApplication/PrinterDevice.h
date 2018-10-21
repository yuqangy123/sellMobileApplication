#pragma once
#include "Singleton.h"
#include <string>
#include <iostream>
#include "DataManager.h"



#define printerDeviceInstanceEx CPrinterDevice::InstanceEx()
class CPrinterDevice :public CSingleton<CPrinterDevice>
{
public:
	CPrinterDevice();
	~CPrinterDevice();

	
	bool printPayOrder(const char* tradeType, const char* orderNo, const char* tradeNo, const char* fee,
		const char* date, const char* time);

	bool printRefundOrder(const refundOrderInfo* info);

	bool checkDevice();

protected:
	void printData(const char* data, int len);
	bool initDevice();
	void closeDevice();
};

