/*
 * 功能描述:
 * 创建时间:2016/3/31 9:43:55
 * 编码人员:wangzhia
 */

#ifndef _CIBEAR_H_
#define _CIBEAR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ibearpub.h"
#include "ibear_com_type.h"
#include "ibear_db_conn.h"
#include "ibear_db_pwd.h"
#include "ibear_err_define.h"
#include "ibear_cache.h"

#define     IBEAR_VERSION_NUMBER   "1.5.2.1"
#define     IBEAR_RELEASE_DATE     "Sep 11 2019"

typedef struct
{
	int db_id[MAX_IBEAR_DB_NUM];
}bear_db;

using namespace std;

extern void IBEAR_ver(char *version, char *r_date);

class ibear
{
public:
	ibear();
	void initbear(char *fileName,bear_db *pInfo,char *host_id);
	ibear(bear_db *pInfo,map<string,stSqlMap*> sqlconfs,stSettings *set);
	ibear(char *fileName,bear_db *pInfo);
	ibear(char *fileName,bear_db *pInfo,char *host_id);
	~ibear();
	void getDBP(int db_no,void *dbp);
	int queryRoute(int (*getRoute)(void *,int *),void *rules,int *db_no);
	
	/*
	 * queryForList重载函数
	 * 游标方式获取数据,配合fetchRow使用
	 */
	int queryForList(const char *sql_key,map<string,string> &inParams);
	int fetchRow(const char *sql_key,vector<string> *out_row);
	
	/*
	 * 1.5版本新增
	 * queryForList重载函数
	 * 游标方式获取数据,配合fetchRow使用
	 * 出参支持全部类型
	 */
	int queryForList(const char *sql_key,map<string,void*> &inParams);
	
	int fetchRow(const char *sql_key,vector<void*> *out_row);
	
	/*
	 * 获取批量数据函数
	 * 不包含lob字段
	 */
	int queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list);
	
	/*
	 * 1.5版本新增
	 * queryForList重载函数
	 * 出参支持全部类型
	 */
	int queryForList(const char *sql_key,map<string,void*> &inParams,vector<vector<void*> > *list);
	
	/*
	 * 在1.5版本中已过时
	 * queryForList重载函数
	 * 出参包含clob时使用,num为clob所在位置(从1开始)
	 */
	attribute_deprecated int queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list,int num);
	
	/*
	 * 获取单条数据函数
	 * 不包含lob字段
	 */
	int queryForObject(const char *sql_key,map<string,string> &inParams,vector<string> *object);
	
	/*
	 * 1.5版本新增
	 * queryForObject重载函数
	 * 出参支持全部类型
	 */
	int queryForObject(const char *sql_key,map<string,void*> &inParams,vector<void*> *object);
	
	/*
	 * 1.5版本新增
	 * DML重载函数
	 * 入参包含lob时使用
	 */
	int DML(const char *sql_key,map<string,void*> &inParams);
	
	/*
	 * DML数据函数
	 * 不包含lob字段
	 */
	int DML(const char *sql_key,map<string,string> &inParams);
	
	/*
	 * DML数据函数
	 * 不包含lob字段,rownum为受影响行数
	 */
	int DML(const char *sql_key,map<string,string> &inParams,int *rownum);
	
	/*
	 * 1.5版本新增
	 * DML重载函数
	 * 入参包含lob时使用,返回受影响行数
	 */
	int DML (const char *sql_key,map<string,void*> &inParams,int *rownum);
	
	/*
	 * 自定义sql函数
	 */
	int queryForListBySQL(int db_no_in,int res_num,char *sql_in,map<string,string> &inParams,vector<vector<string> > *list);
	int queryForObjectBySQL(int db_no_in,int res_num,char *sql_in,map<string,string> &inParams,vector<string> *object);
	int DMLBySQL(int db_no_in,char *sql_in,map<string,string> &inParams);
	int queryForListBySQL(int db_no_in,char *sql_in,map<string,string> &inParams,vector<vector<string> > *list);
	int queryForObjectBySQL(int db_no_in,char *sql_in,map<string,string> &inParams,vector<string> *object);
	
	int Commit();
	int Commit(int db_num);
	int Rollback();
	int Rollback(int db_num);
	void Quit();
	void ClearAllMemData();
	void ClearTableMemData(string table);
	char* getDBNum(const char *sql_key);
	int getDBNum_real(const char *sql_key);
	void clearPrepareSql(int db_no);
	/*
	 * 数据集清理
	 */
	void clearDatas (vector<vector<void*> > &vList);
	void clearDatas (vector<void*> &mList);
public:
	virtual	int getRouteDef(void *rules,int *db_no)
	{
		printf("父类[%s]\n",(char*)rules);
		return 0;
	}

private:
	void init();
	
	/*
	 * 1.5版本新增
	 * 私有函数
	 * 原始函数,针对所有sql操作调用
	 */
	int queryByDef(int queryType,stSqlMap **sqlmap,map<string,void*> &inParams,vector<vector<void*> > *list,int lobnum);
	
	/*
	 * 1.5版本修改
	 * 私有函数
	 * 重载函数,针对非lob情形sql操作调用
	 */
	int queryByDef(int queryType,stSqlMap **sqlmap,map<string,string> &inParams,vector<vector<string> > *list,int lobnum);
	
public:
	IBEAR_DB_INFO m_dbDsc;
	char sql_error[1024];
	char no_error[128];
	int error_code;
	map<string,stSqlMap*> m_Sqlconfs;
	ibear_cache *m_IbearCache;
	
private:
	int *dsc_Nums;
	bool defDscType;
	map<string,IBEAR_sql*> m_IbearSql;
	stSettings *m_Settings;
	bear_db *m_Info;
	int routeNo;
	string log_name;
	int crossFlag;
	vector<int> CrossDBNums;
};

#endif
