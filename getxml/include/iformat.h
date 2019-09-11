/**************************
 * ��������:iformat
 * ��������:����ͷ�ļ�
 * ����汾:V1.0
 * ��������:2014/10/20 11:23:53
 * ������Ա:
 **************************/
#ifndef _IFORMAT_FUNC_H_
#define _IFORMAT_FUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>
#include <iconv.h>
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <time.h>
#include <sys/time.h>

#include "libxml/libxml.h"

using namespace std;

typedef struct
{
	char key[20+1];//�ڵ���
	char value[50+1];//�ڵ�ֵ
	char parent_key[20+1];//���ڵ���
} stXMLMode;

//��ʼ������xmlģ�壬���ص��ڴ�
extern int iformat_init(char *path);
//ת��xml
extern int iformat_getxml(int event_id,map<string,string> &values,char *xml);

extern int iformat_analysis(char *buff,vector<stXMLMode*> &values);

extern xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath);

#endif
