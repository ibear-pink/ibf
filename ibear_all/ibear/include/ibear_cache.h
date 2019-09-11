/*
 * 功能描述:
 * 创建时间:2018/8/16 9:43:55
 * 编码人员:wangzhia
 */

#ifndef _CIBEAR_CACHE_H_
#define _CIBEAR_CACHE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ibearpub.h"
#include "ibear_com_type.h"
#include "ibear_db_conn.h"
#include "ibear_err_define.h"

using namespace std;

#define IBEAR_MEMCACHE_SUCCESS 0
#define IBEAR_MEMCACHE_GETDATA 0
#define IBEAR_MEMCACHE_NONEED 999
#define IBEAR_MEMCACHE_KEYNOTFOUND 1400
#define IBEAR_MEMCACHE_INDEXNOTFOUND 1401
#define IBEAR_MEMCACHE_TABLENOTFOUND 1402
#define IBEAR_MEMCACHE_NEEDMERGE 3
#define IBEAR_MEMCACHE_ERROR -1

class ibear_cache
{
public:
	ibear_cache();
	~ibear_cache();
	int loadDesc(char *memType,char *sql,int dbNo,int resNum,map<string,void*> inParams,int maxsize);
	int addCacheValue(vector<vector<void*> > *list);
	void ClearMemData();
	void ClearMemConf();
	void ClearMemData(string table_name);
	void ClearMemConf(string table_name);
	void ClearMemData(long mem_size);
	vector<IBEAR_MemData> getRows();

private:
	int judgeCacheValue(map<string,void*> inParams);
	int judgeCacheDesc();
	void clearTmpData();
	
private:
	int m_DbNo;//库标签
	int m_ResNum;//
	char m_TableName[128];//表名
	char m_MemType[10];//缓存类型（xml配置）
	char m_RuleNames[1024];//解析sql条件，拼接字符串，逗号分隔
	char m_ResultNames[1024];//解析sql列，拼接字符串，逗号分隔
	vector<string> m_ResultNameList;//解析sql列list
	vector<string> m_RuleNameList;//解析sql条件list
	vector<IBEAR_MemData> m_Rows;//存放本次查询数据
	int m_judgeFlag;//处理结果标示
	bool m_IsMerge;//合并标示
	string m_RuleValue;
	vector<int> m_MergeResultNum;//需要合并的列
	vector<int> m_QueryResultNum;//需要展示的列
	//总体数据
	map<string,map<string,string> > m_tableKey;
	map<string,map<string,vector<IBEAR_MemData> > > m_tableValue;
};

#endif
