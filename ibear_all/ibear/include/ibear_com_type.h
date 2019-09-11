#ifndef _IBEAR_COM_TYPE_
#define _IBEAR_COM_TYPE_

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include <string>

using namespace std;

#define MAX_MAP_NUMS 				100/*sqlMapConfig.conf֧�ֵ�����ļ���*/
#define MAX_EMPTY_NUM				10/*sql��֧�ֵ����$E[]������*/
#define MAX_PARAM_NUM				64
#define MAX_RESULT_NUM				64

#if defined(__cplusplus)
extern "C" {
#endif
	
	struct stDataParam
	{
		int dataNum;
		char name[64];
		char type[10];
		int len;
	};
	
	struct stSQLParam
	{
		int num;
		int name_type;//0,param_value; 1,table_name��2,delete��3,������ʽ��4,������ʱ
		char key[40];
		char type[10];
		//char value[4000+1];
		void *values;
		int isEmpty;//0����Ϊ�գ�1��Ϊ�գ�2Ϊ��
		int len;//sizeof(value)
	};
	
	typedef struct
	{
		bool testModel;//����ģʽ
		int autoCommit;//�Զ��ύ
		char defaultDBNum[5];//Ĭ�Ͽ�
		char CrossDBNums[24];//��Ҫ���Ŀ��ʾ
		char cacheModelsEnabled[10];//sql����
		char autoClearEnabled[20];//�Զ�����(TransationClear/OverflowClear)
		int maxOverflaw;//�������ֵ
		int performanceAnalyzeEnabled;
		char performanceLogPath[128];
		int performanceThreshold;
		int reConnect;
	} stSettings;
	
	typedef struct
	{
		stSettings set;
		int map_num;
		char sqlsrc_map[MAX_MAP_NUMS][1024];
	} stSqlMapConfig;
	
	typedef struct
	{
		char sql[1024];
		char new_sql[1024];
		char id[1024];
		char param[64];
		char res_num[2+1];
		char db_no_real[5];
		char db_no[5];
		map<int,stDataParam*> outParams;
		map<string,stSQLParam*> inParams;
		char memType[10];//��������
		bool testModel;//����ģʽ
	} stSqlMap;
	
#if defined(__cplusplus)
}
#endif

#endif

