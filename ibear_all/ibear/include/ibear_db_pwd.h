/*
 * 2016/12/21 15:50:56
 * 自定义解密方式
 **/
#ifndef _IBEAR_DBCFG_DEFINEPWD_H_
#define _IBEAR_DBCFG_DEFINEPWD_H_

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <string>
#include <map>

using namespace std;

typedef struct
{
	char sDBName[128];
	char sDBUser[128];
	char sDBPwd[128];
}stDBSection;

class ibear_db_pwd
{
private:
	FILE *pFile;//文件指针
	char *m_Content;//文件内容
	int m_Len;//文件长度
	
public:
	ibear_db_pwd();
	~ibear_db_pwd();
	int Custom_decryption(void (*getDBSection)(char *,stDBSection *),char *dbnameKey,stDBSection *dbsection);
private:
	char *ibear_trim(char *S);
	int ibear_OpenFile();
	int ibear_SearchString(const char *Str,const int start,const int end);
	int ibear_SearchChar(char C, int start, int end);
	int ibear_getConfigRecord(const char *Section, const char *KeyName,char *KeyVal);
	int ibear_getDBNameKey(char *fileName,char *dbnameKey);
};

#endif	//_IBEAR_DBCFG_DEFINEPWD_H_
