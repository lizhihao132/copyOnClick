#include <stdio.h>
#include <stdlib.h>
#include <string>
using std::string;
#include <Windows.h>

#include "file.h"


FILE *logger = NULL;

void logLine(const string& content)
{
	fwrite(content.c_str(),content.length(),1,logger);
	fwrite("\r\n",2,1,logger);
}

void testCreateDir(const char * dir)
{
	createDirRecursion(string(dir));
}

void testFullPath(const char * dir)
{
	string full;
	
	if(fullPath(formatPath(dir),full))
	{
		printf("full : %s\r\n",full.c_str());
	}
	else
	{
		printf("full error\r\n");
	}
}

void copyFileToESave(const string& inSource)
{
	string source;
	if(!fullPath(inSource,source))
	{
		logLine("get fullpath error");
		return;
	}
	if(!isFile(source))
	{
		logLine(source + " is not file");
		logLine("---------- failed");
		return;
	}

	string parentDir;
	if(!getParentDir(source,parentDir))
	{
		logLine(source + " get parent dir failed");
		return;
	}
	string parentEDir = string("E") + parentDir.substr(1,parentDir.length() - 1);
	string ESavePath = string("E") + source.substr(1,source.length() - 1);//首字母为盘符
	createDirRecursion(parentEDir);
	logLine(string("create dir succss : ") + parentEDir);
	logLine(string("copy file ") + source + string(" to ") + ESavePath);
	int errorCode = 0;
	if(0 != (errorCode = copyFile(source,ESavePath)))
	{
		if(1 == errorCode)
		{
			logLine(("打开源文件失败 : ") + source);
		}
		else if(2 == errorCode)
		{
			logLine(("写目标文件失败 : ") + ESavePath);
		}
		else{
			logLine(("末知错误 : ") + errorCode);
		}
		errorCode = GetLastError();
		logLine(("GetLastError = ") + errorCode);
		logLine("---------- failed");
		return;
	}
	logLine("------------ done");
}

int main(int argc,char *argv[])
{
	if (argc < 2)
	{
		printf("请输入文件名\r\n");
		return -1;
	}
	logger = NULL;
	fopen_s(&logger,"e:/save/log.txt","w");
	if(NULL == logger)
	{
		printf("log init failed.\r\n");
		return -1;
	}
	copyFileToESave(argv[1]);
	
	fclose(logger);
}
