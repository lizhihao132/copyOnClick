#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
using std::string;
#include <Windows.h>
#include <cassert>
#include "file.h"
#include "kmp.h"

//deprecated : 是正确的，但是是自己实现的。可以直接使用系统函数 GetFullPathName
bool fullPath_Deprecated(const string &thePath,string &full)
{
	string path = formatPath(thePath);

	if(-1 != path.find_first_of(':'))	//已经是绝对路径
	{
		full = string(path);
	}

	if(0 == KMP("./").indexOf(path.c_str()))	//当前目录
	{
		path = path.substr(2,path.length() - 2);
	}
	
	char pBuf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,pBuf);
	string curDir = formatPath(pBuf);
	if ('/' != curDir[curDir.length()-1])
	{
		curDir += "/";
	}
	printf("work dir : %s\r\n",curDir.c_str());
	int upLevel = 0;
	int pos = 0;
	KMP kmp("../");

	while (string::npos != (pos = kmp.indexOf(path.c_str(),pos)))
	{
		pos += 3;
		++ upLevel;
	}
	if(0 == upLevel)	//当前目录下
	{
		full = curDir + path;
		return true;
	}
	else	//上 upLevel 层目录
	{
		
		pos = kmp.indexOf(path.c_str());	//pos 指向 ../ 中的 /
		string coreDir;
		if(pos == path.length()-1)
			coreDir = "";
		else
			coreDir = path.substr(pos + 1,path.length() - pos + 1);
		string parentDir(curDir);
		for (int i = 0;i < upLevel;++ i)
		{
			if(!getParentDir(parentDir,parentDir))
			{
				return false;
			}
		}
		if ('/' != parentDir[parentDir.length()-1])
		{
			parentDir += "/";
		}
		full = parentDir + coreDir;
		return true;
	}
}

bool fullPath(const string &thePath,string &full)
{
	char theDir[MAX_PATH];
	char theFull[MAX_PATH] = {0};
	char *namePos = NULL;
	memcpy_s(theDir,MAX_PATH,thePath.c_str(),MAX_PATH);
	if(::GetFullPathName(theDir,MAX_PATH,theFull,&namePos) == 0)
	{
		int errorCode = GetLastError();
		printf("getLastError : %d",errorCode);
		return false;
	}
	else
	{
		full = formatPath(theFull);
		return true;
	}
}


string formatPath(const string& path)
{
	char tmp[MAX_PATH];

	int len = path.length();
	
	if (0 == len) {
		return "";
	}

	
	memset(tmp, '\0', sizeof(tmp));
	memcpy_s(tmp,len,path.c_str(),len);

	for (int i = 0;i < len;++ i)
	{
		if(tmp[i] == '\\')
			tmp[i] = '/';
	}
	
	return string(tmp);
}


bool isDirectory(const string& path)
{
	WIN32_FIND_DATA fd;  
	bool ret = false;  
	HANDLE hFind = FindFirstFile(path.c_str(), &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))  
	{  
		ret = TRUE;  
	}  
	FindClose(hFind);  
	return ret;
}

bool isFile(const string& path)
{
	WIN32_FIND_DATA fd;  
	bool ret = false;  
	HANDLE hFind = FindFirstFile(path.c_str(), &fd);
	if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & 
		(FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)))
	{  
		ret = TRUE;  
	}  
	FindClose(hFind);  
	return ret;
}

bool DirectoryExist(const string& path)  
{  
	return isDirectory(path);
} 

void mkdirs(const char *dir)
{
	if(DirectoryExist(string(dir)))
		return;
	char tmp[1024];
	char *p;

	if (strlen(dir) == 0 || dir == NULL) {
		printf("strlen(dir) is 0 or dir is NULL.\n");
		return;
	}

	int len = strlen(dir);
	memset(tmp, '\0', sizeof(tmp));
	memcpy_s(tmp,len,dir,len);

	if (tmp[0] == '/') 
		p = strchr(tmp + 1, '/');
	else 
		p = strchr(tmp, '/');

	if (p) 
	{
		*p = '\0';
		_mkdir(tmp);
		_chdir(tmp);
	} else 
	{
		_mkdir(tmp);
		_chdir(tmp);
		return;
	}

	mkdirs(p + 1);
}

int copyFile(const string& source,const string& target)
{
	FILE *sfile = NULL;
	fopen_s(&sfile,source.c_str(),"rb");
	if(NULL == sfile)
	{
		return 1;
	}
	fseek(sfile,0,SEEK_END);
	long sfileSize = ftell(sfile);
	fseek(sfile,0,SEEK_SET);

	FILE* tfile = NULL;
	fopen_s(&tfile,target.c_str(),"w+b");
	if(NULL == tfile)
	{
		return 2;
	}
	char buff[1024];

	size_t total = 0;
	size_t rc = 0;
	while (0 != (rc = fread_s(buff,1024,1,1024,sfile)))
	{
		total += rc;
		size_t wc = fwrite(buff,1,rc,tfile);
		assert(rc == wc);
	}
	fclose(sfile);
	fclose(tfile);
	assert(total == sfileSize);
	return 0;
}

void createDirRecursion(const string& dir)
{
	mkdirs(dir.c_str());
}

bool getParentDir(const string& thePath,string &dir)
{
	string path;
	if(!fullPath(thePath,path))
	{
		return false;
	}
	
	int pos = path.find_last_of('/');
	if(-1 == pos)
		return false;
	else
	{
		if(pos == path.length()-1)
			pos = path.substr(0,path.length()-2).find_last_of('/');
		if(-1 == pos)
			return false;
		dir = path.substr(0,pos);	//0 ~ pos-1
		return true;
	}
}

