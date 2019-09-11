/*
 * ��������:
 * ����ʱ��:2016/3/31 9:43:55
 * ������Ա:wangzhia
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
	 * queryForList���غ���
	 * �α귽ʽ��ȡ����,���fetchRowʹ��
	 */
	int queryForList(const char *sql_key,map<string,string> &inParams);
	int fetchRow(const char *sql_key,vector<string> *out_row);
	
	/*
	 * 1.5�汾����
	 * queryForList���غ���
	 * �α귽ʽ��ȡ����,���fetchRowʹ��
	 * ����֧��ȫ������
	 */
	int queryForList(const char *sql_key,map<string,void*> &inParams);
	
	int fetchRow(const char *sql_key,vector<void*> *out_row);
	
	/*
	 * ��ȡ�������ݺ���
	 * ������lob�ֶ�
	 */
	int queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list);
	
	/*
	 * 1.5�汾����
	 * queryForList���غ���
	 * ����֧��ȫ������
	 */
	int queryForList(const char *sql_key,map<string,void*> &inParams,vector<vector<void*> > *list);
	
	/*
	 * ��1.5�汾���ѹ�ʱ
	 * queryForList���غ���
	 * ���ΰ���clobʱʹ��,numΪclob����λ��(��1��ʼ)
	 */
	attribute_deprecated int queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list,int num);
	
	/*
	 * ��ȡ�������ݺ���
	 * ������lob�ֶ�
	 */
	int queryForObject(const char *sql_key,map<string,string> &inParams,vector<string> *object);
	
	/*
	 * 1.5�汾����
	 * queryForObject���غ���
	 * ����֧��ȫ������
	 */
	int queryForObject(const char *sql_key,map<string,void*> &inParams,vector<void*> *object);
	
	/*
	 * 1.5�汾����
	 * DML���غ���
	 * ��ΰ���lobʱʹ��
	 */
	int DML(const char *sql_key,map<string,void*> &inParams);
	
	/*
	 * DML���ݺ���
	 * ������lob�ֶ�
	 */
	int DML(const char *sql_key,map<string,string> &inParams);
	
	/*
	 * DML���ݺ���
	 * ������lob�ֶ�,rownumΪ��Ӱ������
	 */
	int DML(const char *sql_key,map<string,string> &inParams,int *rownum);
	
	/*
	 * 1.5�汾����
	 * DML���غ���
	 * ��ΰ���lobʱʹ��,������Ӱ������
	 */
	int DML (const char *sql_key,map<string,void*> &inParams,int *rownum);
	
	/*
	 * �Զ���sql����
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
	 * ���ݼ�����
	 */
	void clearDatas (vector<vector<void*> > &vList);
	void clearDatas (vector<void*> &mList);
public:
	virtual	int getRouteDef(void *rules,int *db_no)
	{
		printf("����[%s]\n",(char*)rules);
		return 0;
	}

private:
	void init();
	
	/*
	 * 1.5�汾����
	 * ˽�к���
	 * ԭʼ����,�������sql��������
	 */
	int queryByDef(int queryType,stSqlMap **sqlmap,map<string,void*> &inParams,vector<vector<void*> > *list,int lobnum);
	
	/*
	 * 1.5�汾�޸�
	 * ˽�к���
	 * ���غ���,��Է�lob����sql��������
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
