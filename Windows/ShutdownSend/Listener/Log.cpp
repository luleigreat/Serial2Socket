#include "Log.h"
#include <windows.h>
#include <sstream>
#include <fstream>

std::string GetProgramDir()
{
	wchar_t exeFullPath[MAX_PATH]; // Full path   
	std::string strPath = "";

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	char CharString[MAX_PATH];
	size_t convertedChars = 0;
	wcstombs_s(&convertedChars, CharString, MAX_PATH, exeFullPath, _TRUNCATE);

	strPath = (std::string)CharString;    // Get full path of the file   

	int pos = strPath.find_last_of('\\', strPath.length());
	return strPath.substr(0, pos);  // Return the directory without the file name   
}

int Log(std::string msg)
{
	SYSTEMTIME sys;
	GetLocalTime(&sys); //取系统时间
	std::stringstream ss;
	ss << sys.wYear << "-" << sys.wMonth << "-" << sys.wDay << " " << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;
	std::string dtime = ss.str();

	std::ofstream outfile;
	std::string fullPath = GetProgramDir() + "\\log.txt";
	outfile.open(fullPath, std::ios::app); //文件的物理地址，文件的打开方式
	if (outfile.is_open())
	{
		outfile << dtime << " "<< msg << "\n";
		outfile.close();
		return 0;
	}
	else
	{
		return 1;
	}
}

int Log(std::string msg, int code)
{
	std::string str = msg + ",Err code:";
	str += std::to_string(code);
	return Log(str);
}
