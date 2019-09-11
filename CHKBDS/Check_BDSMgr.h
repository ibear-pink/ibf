
/* 
 * ��������: 
 * ����ʱ��:
 * ������Ա:�Զ�����
*/
#ifndef _CHECK_BDS_MANAGER_H_
#define _CHECK_BDS_MANAGER_H_

#include "CPublicMgr.h"
#include "DynBase.h"

using namespace std;

//BDS��Ӧ���û���key����¼��key�����key��ϣ�
typedef struct{
	char table_name[30];
	char key_id[64];
	char dbno_key[64];
	char prkey_id[128];
	char dbno_prkeys[128];
} stKeyRelas;

typedef struct{
	char key[64];
	char value[64];
} stKeyValues;

//���ƿ��ظ�ʽ
typedef struct{
	int qrydb_nums;
	int check_nums;
	int repair_max;
	char ctrl_flag[1+1];
} stControls;

//������ݸ�ʽ
typedef struct{
	char check_sn[128+1];
	char key_id[128+1];
	char table_name[30+1];
	char check_type[2+1];
	char data_type[2+1];
	int times;
	char file_type[1+1];
} stCheckData;

//checks,�����������Ӧ��key(���)
typedef struct{
	char check_sqlname[30];
	char check_keys[64];
} stTableKey;

//�����޸�
typedef struct{
	string note;
	char dml_type[1+1];
	char check_type[1+1];
} stOptNote;

//�ȶԵ�BDS��������sqlname�Ķ�Ӧ��ϵ
typedef struct{
	char bds_sqlname[32+1];
	char bds_keys[32+1];
	char add_sqlname[32+1];
	vector<stTableKey*> checks;
} stCheckMap;

class Check_BDSMgr:public CPublicMgr,public Object
{
	DECLARE_CLASS(Check_BDSMgr)
public:
	
	Check_BDSMgr();
	~Check_BDSMgr();
    
	int init(ibear *br,char *logName,int thread_num,map<string,icfg_Map*> user_defined);
	int process();
	int ProInterface(vector<stDealTask*> *values);
	int UpdPoorStatus(stDealTask *pTask);
	int ProTimeList(char *sql_key);
	int setTask(vector<string> v_row,stDealTask *pTask);
	
	int getCheckTableMap();
	int getCheckCfg();
	int getControlInfo();
	
	int selectCompareLog();
	int getInterfaceDatas();
	int SplitStringBySP (const char *buff, char split_char, string key[]);
	int deleteInterface(char *key_value,char *table_name);
	int countCompareLog(char *key_value,char *table_name,int *count);
	int insertCompareLog(char *key_value,char *table_name,char *check_type,char *data_type);
	int updateCompareLog(char *key_value,char *table_name,char *check_type);
	int deletedateCompareLog(char *key_value,char *table_name);
	int insertAddLog(char *table_name,int num,string values[],char *dml_type,vector<stKeyValues*> kvs,char *dbno_key);
	int insertRepairLog(char *check_sn,char *table_name,char *key_value,char *check_type,char *check_note,char *is_deal,char *data_type,char *file_type);
	int insertCompareLogErr(char *key_value,char *table_name,char *ErrMsg);
	
	int getCheckDataRecord(char *table_name,vector<stKeyValues*> kvs,map<string,string> *bds_datas,map<string,string> *db_datas);
	int getDataByAllKeyValue(char *sqlname,vector<stKeyValues*> kvs,map<string,string> *db_datas,const int db_type,char *dbno_key);
	int getDataBySigKeyValue(char *table_name,char *sqlname,char *sqlkeys,char *key_value,map<string,string> *db_datas,const int db_type);
	int ProCheckDataRecord(stCheckData *checkdata,char *check_type);
	int ProCheckDataUserToRecord(stCheckData *checkdata);
	int ProRepairData(stCheckData *checkdata);
	int getCheckDataUser(char *table_name,char *key_value,map<string,string> *bds_datas,map<string,string> *db_datas);
	int CheckDataByList(stCheckData *checkdata,map<string,string> bds_datas,map<string,string> db_datas,char *check_type);
	int ProDealData(char *key_id,char *table_name,char *data_type,char *check_sn,char *check_note,char *check_type,int deal_type,char *file_type);
	int getRepairCount(char *check_table,char *count);
	int getRepairCountHis(char *check_table,char *ym,char *count);

public:
	string m_LogName;
	int m_ThreadNum;
	char m_BusinessType[1+1];
	ibear *m_Ibear;
	stControls *m_Controls;
	map<string,stKeyRelas*> m_KeyTableRelas;//���˱���������
	vector<stCheckData*> m_CheckDatas;//������ݼ�
	map<string,stCheckMap*> m_CheckTableMaps;//����Ӧ��ϵ
	char m_tableName[256];
	char m_YMD[8+1];
	char m_ErrNo[20];
	char m_ErrMsg[64];
};
#endif

