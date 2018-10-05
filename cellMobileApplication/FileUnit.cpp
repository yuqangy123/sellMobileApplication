#include "stdafx.h"
#include "FileUnit.h"

DECLARE_SINGLETON_MEMBER(CFileUnit);
CFileUnit::CFileUnit()
{
}


CFileUnit::~CFileUnit()
{
}

bool CFileUnit::ForceDirectories(std::string &Dir)
{
	if (Dir == "")
		return false;

	Dir = ExcludeTrailingPathDelimiter(Dir);
	if ((Dir.length() < 3) || DirectoryExists(Dir.c_str()) ||
		ExtractFilePath(Dir) == Dir)
	{
		return true;
	}
	return ForceDirectories(ExtractFilePath(Dir)) && CreateDirectoryA(Dir.c_str(), NULL);
}

std::string CFileUnit::ExcludeTrailingPathDelimiter(const std::string &s)
{
	if ("\\" == s.substr(s.length() - 1, s.length()))
	{
		return std::string(s.substr(0, s.length() - 1));
	}
	else
	{
		if ("/" == s.substr(s.length() - 1, s.length()))
		{
			return std::string(s.substr(0, s.length() - 1));
		}
		else
		{
			return s;
		}
	}
}

bool CFileUnit::DirectoryExists(const char *ApFileName)
{
	if (!ApFileName)
		return false;
	DWORD code = GetFileAttributesA(ApFileName);
	return ((-1 != code) && (FILE_ATTRIBUTE_DIRECTORY & code));
}

std::string CFileUnit::ExtractFilePath(const std::string &AFileName)
{
	std::string str_a = Getwstring(AFileName, ("\\"), true);
	std::string str_b = Getwstring(AFileName, ("/"), true);
	if (str_a.empty())
		return str_b;
	return str_a;
}

std::string CFileUnit::ExtractFileName(const std::string &AFileName)
{
	std::string str_a = Getwstring(AFileName, ("\\"), false);
	std::string str_b = Getwstring(AFileName, ("/"), false);
	if (str_a.empty())
		return str_b;
	return str_a;
}

std::string CFileUnit::Getwstring(const std::string &s, const std::string &Sing, bool Forwar /*= true*/)
{
	std::string::size_type pos = s.rfind(Sing);
	if (std::string::npos == pos)
	{
		return "";
	}
	else
	{
		if (Forwar)
			return s.substr(0, pos + 1);
		else
			return s.substr(pos + 1, s.length());
	}
}