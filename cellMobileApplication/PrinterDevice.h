#pragma once
#include "Singleton.h"
#include <string>
#include <iostream>




#define printerDeviceInstanceEx CPrinterDevice::InstanceEx()
class CPrinterDevice :public CSingleton<CPrinterDevice>
{
public:
	CPrinterDevice();
	~CPrinterDevice();

	void printData(const char* data, int len);
};

