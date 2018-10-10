#pragma once
#include "Singleton.h"
#include <string>

#define FileUnitInstance CFileUnit::Instance()
class CFileUnit : public CSingleton<CFileUnit>
{
public:
	CFileUnit();
	~CFileUnit();

	//强制创建目录,一层层创建
	bool ForceDirectories(std::string &Dir);

	//目录是否存在
	bool DirectoryExists(const char *ApFileName);

	bool FileExists(const char *ApFileName);

	/*获得文件的路径,如: E:\VCProjects\FileOperation\1\Debug\1.bsc => E:\VCProjects\FileOperation\1\Debug\ */
	std::string ExtractFilePath(const std::string &AFileName);
	std::string ExtractFileName(const std::string &AFileName);

protected:
	////如果文件最后一个字符为: \,则将它去掉后返回,否则返回原字符串
	std::string ExcludeTrailingPathDelimiter(const std::string &s);

	std::string Getwstring(const std::string &s, const std::string &Sing, bool Forwar = true);
};

