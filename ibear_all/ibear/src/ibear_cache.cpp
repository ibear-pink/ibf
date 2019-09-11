/**************************
 * file_name:ibear_cache.h
 * function:public
 * program edition:V1.0
 * coding date:2018/8/16
 * programer:wangzhia
 **************************/

#include "ibear_cache.h"

ibear_cache::ibear_cache()
{
}

ibear_cache::~ibear_cache()
{
}

void ibear_cache::clearTmpData()
{
	memset(m_MemType, 0, sizeof(m_MemType));
	memset(m_TableName, 0, sizeof(m_TableName));
	memset(m_RuleNames, 0, sizeof(m_RuleNames));
	memset(m_ResultNames, 0, sizeof(m_ResultNames));
	m_RuleValue = "";
	if (m_ResultNameList.size() > 0)
	{
		m_ResultNameList.clear();
	}
	if (m_RuleNameList.size() > 0)
	{
		m_RuleNameList.clear();
	}
	if (m_MergeResultNum.size() > 0)
	{
		m_MergeResultNum.clear();
	}
	if (m_QueryResultNum.size() > 0)
	{
		m_QueryResultNum.clear();
	}
	if (m_Rows.size() > 0)
	{
		m_Rows.clear();
	}
}

int ibear_cache::loadDesc(char *memType,char *sql,int dbNo,int resNum,map<string,void*> inParams,int maxsize)
{
	m_DbNo = dbNo;
	m_ResNum = resNum;
	m_IsMerge = false;
	m_judgeFlag = IBEAR_MEMCACHE_GETDATA;
	clearTmpData();
	//sql是否需要缓存
	strcpy(m_MemType,memType);
	if (strcasecmp(m_MemType,"false") == 0)
	{
		return IBEAR_MEMCACHE_NONEED;
	}
	//检查是否超过需要缓存的最大内存
	ClearMemData(maxsize*1024*1024);
	
	//解析sql，获得表名、字段名；
	if (ibear_SQLResults(sql,m_TableName,&m_ResultNameList,m_ResultNames) < 0)
	{
		return IBEAR_MEMCACHE_ERROR;
	}
	//解析sql，获得条件名；
	if (ibear_SQLRules(sql,m_RuleNames,&m_RuleNameList) < 0)
	{
		return IBEAR_MEMCACHE_ERROR;
	}
	sprintf(m_TableName,"%s%d",xmlTrim(m_TableName),m_DbNo);
	xmlTrim(m_ResultNames);
	xmlTrim(m_RuleNames);
#ifdef _DEBUG_INFO_
	printf("tableName:%s\n",m_TableName);
	printf("resultName:%s\n",m_ResultNames);
	printf("ruleName:%s\n",m_RuleNames);
#endif
	judgeCacheDesc();
	return judgeCacheValue(inParams);
}

void ibear_cache::ClearMemData()
{
	if (m_tableValue.size() > 0)
	{
		if (strcasecmp(m_MemType,"forever") == 0)
		{
			for (map<string,map<string,vector<IBEAR_MemData> > >::iterator it = m_tableValue.begin();it!=m_tableValue.end();it++)
			{
				if (strcmp((it->first).c_str(),m_TableName) != 0)
				{
					it->second.clear();
				}
			}
		}
		else
		{
			m_tableValue.clear();
		}
	}
}

void ibear_cache::ClearMemConf()
{
	if (m_tableKey.size() > 0)
	{
		if (strcasecmp(m_MemType,"forever") == 0)
		{
			for (map<string,map<string,string> >::iterator it = m_tableKey.begin();it!=m_tableKey.end();it++)
			{
				if (strcmp((it->first).c_str(),m_TableName) != 0)
				{
					it->second.clear();
				}
			}
		}
		else
		{
			m_tableKey.clear();
		}
	}
}

void ibear_cache::ClearMemData(string table_name)
{
	if (m_tableValue.size() > 0)
	{
		map<string,map<string,vector<IBEAR_MemData> > >::iterator it = m_tableValue.find(table_name);
		if(it != m_tableValue.end())
		{
			(it->second).clear();
		}
	}
}

void ibear_cache::ClearMemConf(string table_name)
{
	if (m_tableKey.size() > 0)
	{
		map<string,map<string,string> > ::iterator it = m_tableKey.find(table_name);
		if (it!=m_tableKey.end())
		{
			(it->second).clear();
		}
	}
}

void ibear_cache::ClearMemData(long mem_size)
{
	long cur_size = 0;
	long columnsize = 0;
	long rowsize = 0;
	long indexsize = 0;
	for(map<string,map<string,vector<IBEAR_MemData> > >::iterator it_tableValue = m_tableValue.begin(); it_tableValue!=m_tableValue.end(); it_tableValue++)
	{
		map<string,vector<IBEAR_MemData> > Values = it_tableValue->second;
		for(map<string,vector<IBEAR_MemData> >::iterator it_ColumValues = Values.begin(); it_ColumValues!=Values.end(); it_ColumValues++)
		{
			vector<IBEAR_MemData> value = it_ColumValues->second;
			for (vector<IBEAR_MemData>::iterator it_row = value.begin(); it_row != value.end(); it_row++)
			{
				IBEAR_MemData row = *it_row;
				columnsize = row.size() * 24;
				continue;
			}
			cur_size += (value.size() * columnsize + 24);
		}
		cur_size += (cur_size + 24);
	}
#ifdef _DEBUG_INFO_
	printf("当前内存：%ld，清理阈值：%ld\n",cur_size,mem_size);
#endif
	if (mem_size <= cur_size && mem_size > 0)
	{
		ClearMemData();
		ClearMemConf();
	}
}

int ibear_cache::addCacheValue(vector<vector<void*> > *list)
{
	int iRet = 0;
	//找到缓存数据
	if (m_judgeFlag == IBEAR_MEMCACHE_GETDATA)
	{
		if (strcasecmp(m_MemType,"true") == 0)
		{
#ifdef _DEBUG_INFO_
			printf("找到缓存数据\n");
#endif
		}
		return m_judgeFlag;
	}
	
	vector<IBEAR_MemData> rows;
	rows = *list;
	if (m_judgeFlag == IBEAR_MEMCACHE_TABLENOTFOUND || m_judgeFlag == IBEAR_MEMCACHE_KEYNOTFOUND)
	{
		//没有找到缓存模型
		map<string,vector<IBEAR_MemData> > columns;
		columns.insert(pair<string,vector<IBEAR_MemData> >(m_RuleValue,rows));
		m_tableValue.insert(pair<string,map<string,vector<IBEAR_MemData> > >(string(m_TableName),columns));
	}
	else if (m_judgeFlag == IBEAR_MEMCACHE_INDEXNOTFOUND)
	{
		//没有找到缓存数据索引
		map<string,map<string,vector<IBEAR_MemData> > >::iterator c = m_tableValue.find(string(m_TableName));
		if (c != m_tableValue.end())
		{
			(c->second).insert(pair<string,vector<IBEAR_MemData> >(m_RuleValue,rows));
		}
		else
		{
			map<string,vector<IBEAR_MemData> > columns;
			columns.insert(pair<string,vector<IBEAR_MemData> >(m_RuleValue,rows));
			m_tableValue.insert(pair<string,map<string,vector<IBEAR_MemData> > >(string(m_TableName),columns));
		}
	}
	else if (m_judgeFlag == IBEAR_MEMCACHE_NEEDMERGE)
	{
		//数据合并
		map<string,map<string,vector<IBEAR_MemData> > >::iterator it_tableValue = m_tableValue.find(string(m_TableName));
		if (it_tableValue != m_tableValue.end())
		{
			map<string,vector<IBEAR_MemData> >::iterator it_columnValue = (it_tableValue->second).find(m_RuleValue);
			for (vector<int>::iterator it_num = m_MergeResultNum.begin(); it_num != m_MergeResultNum.end(); it_num++)
			{
				int num = *it_num;
				for (int i = 0; i < it_columnValue->second.size(); i++)
				{
#ifdef _DEBUG_INFO_
					//printf("合并[%d,%d]%s\n",num,i,(*list)[i][num]);
#endif
					(it_columnValue->second)[i].push_back((*list)[i][num]);
				}
			}
		}
	}
	//不做缓存
	else
	{
		rows.clear();
	}
	
	return IBEAR_MEMCACHE_SUCCESS;
}

int ibear_cache::judgeCacheDesc()
{
	bool notExist = true;
	map<string,map<string,string> >::iterator it_desc = m_tableKey.find(string(m_TableName));
	if (it_desc != m_tableKey.end())
	{
		map<string,string>::iterator it_result = (it_desc->second).find(string(m_RuleNames));
		if (it_result != (it_desc->second).end())
		{
			notExist = false;
#ifdef _DEBUG_INFO_
			if (strcmp((it_result->second).c_str(),m_ResultNames) != 0)
			{
				printf("结果需要合并[%s]\n",(it_result->second).c_str());
			}
			else
			{
				printf("结果直接展示[%s]\n",(it_result->second).c_str());
			}
#endif
			int resultNum = 0;
			for (vector<string>::iterator it_row = m_ResultNameList.begin();it_row != m_ResultNameList.end();it_row++)
			{
				string rowname = *it_row;
				string::size_type pos = (it_result->second).find(rowname+",");
				if (pos == string::npos)
				{
					it_result->second = it_result->second + rowname + ",";
					m_IsMerge = true;
					m_MergeResultNum.push_back(resultNum);
				}
				else if (pos == 0)
				{
					//printf("%s is 0\n",rowname.c_str());
					m_QueryResultNum.push_back(0);
				}
				else
				{
					char crowname[64] = {0};
					char cresult[1024] = {0};
					strcpy(crowname,rowname.c_str());
					strcpy(cresult,(it_result->second).c_str());
					int num = ibear_getRowNum(cresult,crowname);
					//printf("%s is %d\n",crowname,num);
					m_QueryResultNum.push_back(num);
				}
				resultNum ++;
			}
		}
	}
	
	if (notExist)
	{
		//desc 不存在
		map<string,string> columns;
		columns.insert(pair<string,string>(string(m_RuleNames),string(m_ResultNames)));
		m_tableKey.insert(pair<string,map<string,string> >(string(m_TableName),columns));
		m_judgeFlag = IBEAR_MEMCACHE_KEYNOTFOUND;
	}
	
#ifdef _DEBUG_INFO_
	for (map<string,map<string,string> >::iterator it = m_tableKey.begin();it!=m_tableKey.end();it++)
	{
		for (map<string,string>::iterator it1= (it->second).begin();it1!=(it->second).end();it1++)
		{
			printf("key[%s][%s][%s]\n",(it->first).c_str(),(it1->first).c_str(),(it1->second).c_str());
		}
	}
#endif
	return IBEAR_MEMCACHE_SUCCESS;
}

int ibear_cache::judgeCacheValue(map<string,void*> inParams)
{
	//根据入参绑定变量获取排序后的组合字符串；
	if (inParams.size()==0)
	{
		m_RuleValue = "NULL";
	}
	for (map<string,void*>::iterator param = inParams.begin(); param != inParams.end(); param++)
	{
		string value = string((char*)param->second);
		m_RuleValue += value+",";
	}
	
#ifdef _DEBUG_INFO_
	printf("m_RuleValue=%s\n",m_RuleValue.c_str());
	for(map<string,map<string,vector<IBEAR_MemData> > >::iterator it_tableValue = m_tableValue.begin(); it_tableValue!=m_tableValue.end(); it_tableValue++)
	{
		string tableName = it_tableValue->first;
		map<string,vector<IBEAR_MemData> > Values = it_tableValue->second;
		printf("[%s]size=%ld\n",tableName.c_str(),Values.size());
		for(map<string,vector<IBEAR_MemData> >::iterator it_ColumValues = Values.begin(); it_ColumValues!=Values.end(); it_ColumValues++)
		{
			string index = it_ColumValues->first;
			vector<IBEAR_MemData> value = it_ColumValues->second;
			for (vector<IBEAR_MemData>::iterator it_row = value.begin(); it_row != value.end(); it_row++)
			{
				IBEAR_MemData row = *it_row;
				char pvalue[1024] = {0};
				int mergeNum = 0;
				for (int i = 0; i < m_ResNum + m_MergeResultNum.size(); i++)
				{
					if (row.size() == i)
					{
						break;
					}
					sprintf(pvalue, "%s|%s",pvalue,row[i]);
				}
				printf("\t[%s][%s][%s]\n",tableName.c_str(),index.c_str(),pvalue);
			}
		}
	}
#endif

	
	if (m_IsMerge)
	{
		m_judgeFlag = IBEAR_MEMCACHE_NEEDMERGE;
	}
	else
	{
		map<string,map<string,vector<IBEAR_MemData> > >::iterator it_tableValue = m_tableValue.find(string(m_TableName));
		if (it_tableValue != m_tableValue.end())
		{
			map<string,vector<IBEAR_MemData> > tableValues = it_tableValue->second;
			map<string,vector<IBEAR_MemData> >::iterator it_columValue = tableValues.find(m_RuleValue);
			if (it_columValue != tableValues.end())
			{
				vector<IBEAR_MemData> value = it_columValue->second;
				for (int i = 0; i < value.size(); i++)
				{
					IBEAR_MemData row = value[i];
					IBEAR_MemData row_new;
					for (vector<int>::iterator it_num = m_QueryResultNum.begin();it_num!=m_QueryResultNum.end();it_num++)
					{
						int num = *it_num;
#ifdef _DEBUG_INFO_
						printf("\tvalue[%d]%s\n",num,row[num]);
#endif
						row_new.push_back(row[num]);
					}
					m_Rows.push_back(row_new);
				}
			}
			else
			{
				//未找到索引
				m_judgeFlag = IBEAR_MEMCACHE_INDEXNOTFOUND;
			}
		}
		else
		{
			//未找到表名
			m_judgeFlag = IBEAR_MEMCACHE_TABLENOTFOUND;
		}
	}
	
#ifdef _DEBUG_INFO_
	printf("m_judgeFlag=%d\n",m_judgeFlag);
#endif
	return m_judgeFlag;
}

vector<IBEAR_MemData> ibear_cache::getRows()
{
	return m_Rows;
}

