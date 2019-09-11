/*
 * 2016/12/21 15:50:56
 * �Զ�����ܷ�ʽ
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
	FILE *pFile;//�ļ�ָ��
	char *m_Content;//�ļ�����
	int m_Len;//�ļ�����
	
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
