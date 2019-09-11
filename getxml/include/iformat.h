/**************************
 * 服务名称:iformat
 * 功能描述:公共头文件
 * 程序版本:V1.0
 * 编码日期:2014/10/20 11:23:53
 * 编码人员:
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
	char key[20+1];//节点名
	char value[50+1];//节点值
	char parent_key[20+1];//父节点名
} stXMLMode;

//初始化，读xml模板，加载到内存
extern int iformat_init(char *path);
//转换xml
extern int iformat_getxml(int event_id,map<string,string> &values,char *xml);

extern int iformat_analysis(char *buff,vector<stXMLMode*> &values);

extern xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath);

#endif
