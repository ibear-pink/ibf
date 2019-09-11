/*
 --oracle mysql test table
 create table abc
 (
 a VARCHAR(40),
 b NUMERIC(14),
 c date,
 d varchar(40)
 );
 
 --dmdb tes table
 create table abc
 (
 a VARCHAR(40),
 b NUMERIC(14),
 c date,
 d varchar(40),
 primary key (a)
 ) route type all;
 create index abc_ind on abc(a);
 
--dmdb lob table
create table abc_lob
(
a VARCHAR(40),
b NUMBER(14),
c date,
d lob(2000),
e lob(2000),
primary key (a)
) route type all;
create index abc_ind on abc_lob(a);
*/
/*********************
* 功能描述:入口函数
* 编码日期:
* 编码人员:自动生成
*********************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "ibear.h"

using namespace std;
ibear *m_Ibear;

typedef struct
{
	char  name[2000];
	int  idNo;
}LOB_TEST;

//查询结果测试--多行void*
int selectTestAll()
{
	printf("-----selectTestAll test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<vector<void*> > list;
	char a[10] = {"abc1"};
	inParams.insert(pair<string,void*>(string("a"),&a));
	iRet = m_Ibear->queryForList("selectTestAll",inParams,&list);
	if (iRet != 0)
	{
		printf("selectTestAll ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<void*> >::iterator it=list.begin();it!=list.end();it++)
	{
		vector<void*> row = *it;
		printf("all:[%s],",(char*)row[0]);
		printf("[%s],",(char*)row[1]);
		printf("[%s],",(char*)row[2]);
		printf("[%s]\n",(char*)row[3]);
	}
	//清理内存
	m_Ibear->clearDatas(list);
	return iRet;
}

//查询结果测试--单行void*
int selectTestObject()
{
	printf("-----selectTestObject test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<void*> row;
	char a[10] = {"abc2"};
	inParams.insert(pair<string,void*>(string("a"),&a));
	iRet = m_Ibear->queryForObject("selectTest",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTestObject ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	if (row.size() > 0)
	{
		printf("one:[%s],",(char*)row[0]);
		printf("[%s],",(char*)row[1]);
		printf("[%s],",(char*)row[2]);
		printf("[%s]\n",(char*)row[3]);
	}
	m_Ibear->clearDatas(row);
	return iRet;
}

//fetch demo
int selectFetch()
{
	printf("-----selectFetch test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<void*> v_row;
	char a[10] = {"abc0"};
	inParams.insert(pair<string,void*>(string("a"),&a));
	iRet = m_Ibear->queryForList("selectTestAll",inParams);
	if (iRet != 0)
	{
		printf("selectFetch ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	while(1)
	{
		iRet = m_Ibear->fetchRow("selectTestAll", &v_row);
		if (iRet < 0)
		{
			printf("selectFetch fetchRow ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
			iRet = -1;
			return iRet;
		}
		else if (iRet == 1)
		{
			break;
		}
		printf("fetch:[%s],",(char*)v_row[0]);
		printf("[%s],",(char*)v_row[1]);
		printf("[%s],",(char*)v_row[2]);
		printf("[%s]\n",(char*)v_row[3]);
		m_Ibear->clearDatas(v_row);
	}
	
	return iRet;
}

//insert/update/delete 测试函数 void*
int dmlTestAll(char *test1,char *test2,char *test3,char *test4)
{
	printf("-----dmlTestAll test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	
	inParams.insert(pair<string,void*>(string("a"),test1));
	inParams.insert(pair<string,void*>(string("b"),test2));
	inParams.insert(pair<string,void*>(string("c"),test3));
	inParams.insert(pair<string,void*>(string("d"),test4));
	iRet = m_Ibear->DML("dmlTestAll",inParams);
	if (iRet != 0)
	{
		printf("dmlTestAll ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	return iRet;
}

//查询结果测试--多行
int selectTestAlldef()
{
	printf("-----selectTestAlldef test ------\n");
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	char a[10] = {"abc1"};
	inParams.insert(pair<string,string>(string("a"),string(a)));
	iRet = m_Ibear->queryForList("selectTestAll",inParams,&list);
	if (iRet != 0)
	{
		printf("selectTestAll ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<string> >::iterator it=list.begin();it!=list.end();it++)
	{
		vector<string> row = *it;
		printf("def-all:[%s],",row[0].c_str());
		printf("[%s],",row[1].c_str());
		printf("[%s],",row[2].c_str());
		printf("[%s]\n",row[3].c_str());
	}
	
	return iRet;
}

//查询结果测试--单行
int selectTestObjectdef()
{
	printf("-----selectTestObjectdef test ------\n");
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	char a[10] = {"abc2"};
	inParams.insert(pair<string,string>(string("a"),string(a)));
	iRet = m_Ibear->queryForObject("selectTest",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTestObject ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	if (row.size() > 0)
	{
		printf("def-one:[%s],",row[0].c_str());
		printf("[%s],",row[1].c_str());
		printf("[%s],",row[2].c_str());
		printf("[%s]\n",row[3].c_str());
	}
	
	return iRet;
}

//fetch demo
int selectFetchdef()
{
	printf("-----selectFetchdef test ------\n");
	int iRet = 0;
	map<string,string> inParams;
	vector<string> v_row;
	char a[10] = {"abc3"};
	inParams.insert(pair<string,string>(string("a"),string(a)));
	iRet = m_Ibear->queryForList("selectTestAll",inParams);
	if (iRet != 0)
	{
		printf("selectFetchdef ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	while(1)
	{
		iRet = m_Ibear->fetchRow("selectTestAll", &v_row);
		if (iRet < 0)
		{
			printf("selectFetchdef fetchRow ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
			iRet = -1;
			return iRet;
		}
		else if (iRet == 1)
		{
			break;
		}
		printf("def-fetch:[%s],",v_row[0].c_str());
		printf("[%s],",v_row[1].c_str());
		printf("[%s],",v_row[2].c_str());
		printf("[%s]\n",v_row[3].c_str());
	}
	
	return iRet;
}

//insert/update/delete 测试函数
int dmlTestAlldef(char *test1,char *test2,char *test3,char *test4)
{
	printf("-----dmlTestAlldef test ------\n");
	int iRet = 0;
	map<string,string> inParams;
	
	inParams.insert(pair<string,string>(string("a"),string(test1)));
	inParams.insert(pair<string,string>(string("b"),string(test2)));
	inParams.insert(pair<string,string>(string("c"),string(test3)));
	inParams.insert(pair<string,string>(string("d"),string(test4)));
	iRet = m_Ibear->DML("dmlTestAll",inParams);
	if (iRet != 0)
	{
		printf("dmlTestAll ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	return iRet;
}

//delete测试 string
int deleteTEST()
{
	printf("-----deleteTEST test ------\n");
	int iRet = 0;
	map<string,string> inParams;
	iRet = m_Ibear->DML("deleteTEST",inParams);
	if (iRet < 0)
	{
		printf("deleteTEST ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("delete abc");
	return iRet;
}

//拼接字符串在sql中（硬解析方式） 单行void*
int selectTestCount()
{
	printf("-----selectTestCount test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<void*> v_row1;
	char tableName[10] = {"abc"};
	inParams.insert(pair<string,void*>(string("TABLE_NAME"),&tableName));
	iRet = m_Ibear->queryForObject("selectTestCount",inParams,&v_row1);
	if (iRet != 0)
	{
		printf("SEL_TEST_1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	if (v_row1.size() == 0)
	{
		printf("count is 0\n");
	}
	else
	{
		printf("count=%s\n",(char*)v_row1[0]);
	}
	m_Ibear->clearDatas(v_row1);
	return iRet;
}

//sql $L[]的用法
int selInList()
{
	printf("-----selInList test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<vector<void*> > list;
	char c1[2+1] = {"1"};
	char c2[2+1] = {"2"};
	inParams.insert(pair<string,void*>(string("list1"),&c1));
	inParams.insert(pair<string,void*>(string("list2"),&c2));
	iRet = m_Ibear->queryForList("selInList",inParams,&list);
	if (iRet != 0)
	{
		printf("SEL_TEST_IN ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	for (vector<vector<void*> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<void*> v_row = *it;
		printf("$L:[%s],[%s],[%s],[%s]\n",(char*)v_row[0],(char*)v_row[1],(char*)v_row[2],(char*)v_row[3]);
	}
	m_Ibear->clearDatas(list);
	return iRet;
}

//sql写在程序中测试（查询结果单条）
int SEL_TEST_DOUBLE_BYDEF()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> v_row1;
	char sql[1024] = {0};
	strcpy(sql,"select a,b,c from #TABLE_NAME# where b=${b|int} $E[and a=${a|char} ]$E[and d=${d|int}] union all select a,b,c from #TABLE_NAME# where b=${b|int} $E[and a=${a|char} ]and d=${d|int}");
	inParams.insert(pair<string,string>(string("a"),string("a")));
	inParams.insert(pair<string,string>(string("b"),string("1")));
	inParams.insert(pair<string,string>(string("d"),string("1000")));
	inParams.insert(pair<string,string>(string("TABLE_NAME"),string("abc")));
	iRet = m_Ibear->queryForObjectBySQL(0,sql,inParams,&v_row1);
	if (iRet != 0)
	{
		printf("SEL_TEST_1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	printf("%s\n",v_row1[0].c_str());
	return iRet;
}

//sql写在程序中测试（查询结果为列表）
int SEL_TEST_DOUBLE_BYDEF1()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	char sql[1024] = {0};
	strcpy(sql,"select a,b,c from #TABLE_NAME# where $E[d=${d|int} and] b=${b|int} $E[and a=${a|char} ] union all select a,b,c from #TABLE_NAME# where b=${b|int} $E[and a=${a|char} ]and d=${d|int}");
	inParams.insert(pair<string,string>(string("a"),string("a")));
	inParams.insert(pair<string,string>(string("b"),string("1")));
	inParams.insert(pair<string,string>(string("d"),string("1000")));
	inParams.insert(pair<string,string>(string("TABLE_NAME"),string("abc")));
	iRet = m_Ibear->queryForListBySQL(0,sql,inParams,&list);
	if (iRet != 0)
	{
		printf("SEL_TEST_1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<string> v_row = *it;
		printf("%s\n",v_row[0].c_str());
	}
	return iRet;
}

//sql写在程序中测试（insert/update/delete 测试函数）
int dmlTest_BYDEF(char *test1,int test2)
{
	int iRet = 0;
	char ctest2[10] = {0};
	sprintf(ctest2,"%d",test2);
	map<string,string> inParams;
	char sql[1024]  ={0};
	strcpy(sql,"delete abc where a=${a|char} and b=${b|int}");
	inParams.insert(pair<string,string>(string("a"),string(test1)));
	inParams.insert(pair<string,string>(string("b"),string(ctest2)));
	iRet = m_Ibear->DMLBySQL(0,sql,inParams);
	if (iRet < 0)
	{
		printf("dmlTest_BYDEF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	else if (iRet == 1)
	{
		printf("dmlTest_BYDEF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
	}
	return iRet;
}

//自定义解密方式
void getDBSection(char *dbnameKey,stDBSection *dbsection)
{
	printf("%s\n",dbnameKey);
	if (dbsection == NULL)
	{
		dbsection = new stDBSection();
	}
	//自定义解密添加
	strcpy(dbsection->sDBName,"BILLCOLLDEV");
	strcpy(dbsection->sDBUser,"DBACCOPR");
	strcpy(dbsection->sDBPwd,"Lf5k3rcf");
}

//oracle select结果多行测试函数 string
int selectTestOracle()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	inParams.insert(pair<string,string>(string("a"),string("1")));
	inParams.insert(pair<string,string>(string("b"),string("0")));
	iRet = m_Ibear->queryForList("SEL_TEST_LIST",inParams,&list);
	if (iRet != 0)
	{
		printf("selectTest2 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<string> v_row = *it;
		printf("%s\n",v_row[0].c_str());
	}
	
	inParams.insert(pair<string,string>(string("a"),string("1")));
	inParams.insert(pair<string,string>(string("b"),string("0")));
	iRet = m_Ibear->queryForList("SEL_TEST_LIST1",inParams,&list);
	if (iRet != 0)
	{
		printf("selectTest2 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<string> v_row = *it;
		printf("%s\n",v_row[0].c_str());
	}

	iRet = m_Ibear->queryForList("SEL_TEST_LIST2",inParams,&list);
	if (iRet != 0)
	{
		printf("selectTest2 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<string> v_row = *it;
		printf("%s\n",v_row[0].c_str());
	}

	return iRet;
}

//lob delete demo
int deleteBLOB()
{
	printf("-----deleteBLOB test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	iRet = m_Ibear->DML("DELETE_BLOB",inParams);
	if (iRet != 0)
	{
		printf("deleteBLOB fetchRow ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	m_Ibear->Commit();
	printf("----DML test default DB\n");
	return iRet;
}

//lob insert demo
int insertBLOB(LOB_TEST d,int i)
{
	printf("-----insertBLOB test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	
	char a[10] = {0};
	sprintf(a,"%d",i);
	char b[10] = {"-456"};
	char c[14+1] = {"20180910000000"};
	char e[328] = {0};
	sprintf(e, "%s,%s,%s",a,b,c);
	
	inParams.insert(pair<string,void*>(string("A"),&a));
	inParams.insert(pair<string,void*>(string("B"),&b));
	inParams.insert(pair<string,void*>(string("C"),&c));
	inParams.insert(pair<string,void*>(string("D"),&d));
	inParams.insert(pair<string,void*>(string("E"),&e));
	iRet = m_Ibear->DML("INSERT_BLOB",inParams);
	if (iRet != 0)
	{
		printf("insertBLOB fetchRow ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----DML test default DB\n");
	return iRet;
}

//lob select demo
int selectBLOBAll()
{
	printf("-----SELECT_BLOB_ALL test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<vector<void*> > list;
	iRet = m_Ibear->queryForList("SELECT_BLOB_ALL",inParams,&list);
	if (iRet != 0)
	{
		printf("selectBLOBAll ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<void*> >::iterator it = list.begin();it!=list.end();it++){
		vector<void*> row = *it;
		printf("----DMDBAll no0=[%s]\n",(char*)row[0]);
		printf("----DMDBAll no1=[%s]\n",(char*)row[1]);
		printf("----DMDBAll no2=[%s]\n",(char*)row[2]);
		LOB_TEST * test = (LOB_TEST*)row[3];
		printf("----DMDBAll no3=[%d,%s]\n",test->idNo,test->name);
		printf("----DMDBAll no4=[%s]\n",(char*)row[4]);
	}
	m_Ibear->clearDatas(list);
	return iRet;
}

int selectBLOBObject(int i)
{
	printf("-----selectBLOBObject test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<void*> row;
	char a[10] = {0};
	char c[20] = {0};
	sprintf(a, "%d",i);
	strcpy(c,"20180910000000");
	inParams.insert(pair<string,void*>(string("A"),&a));
	inParams.insert(pair<string,void*>(string("C"),&c));
	
	iRet = m_Ibear->queryForObject("SELECT_BLOB",inParams,&row);
	if (iRet != 0)
	{
		printf("selectBLOBObject ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	if (row.size() > 0)
	{
		printf("----DMDB no0=[%s]\n",(char*)row[0]);
		printf("----DMDB no1=[%s]\n",(char*)row[1]);
		printf("----DMDB no2=[%s]\n",(char*)row[2]);
		LOB_TEST * test = (LOB_TEST*)row[3];
		printf("----DMDB no3=[%d,%s]\n",test->idNo,test->name);
		printf("----DMDB no4=[%s]\n",(char*)row[4]);
	}
	m_Ibear->clearDatas(row);
	return iRet;
}

//lob fetch demo
int selectBLOBFetch()
{
	printf("-----selectBLOBFetch test ------\n");
	int iRet = 0;
	map<string,void*> inParams;
	vector<void*> v_row;
	iRet = m_Ibear->queryForList("SELECT_BLOB_FETCH",inParams);
	if (iRet != 0)
	{
		printf("selectBLOB ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	while(1)
	{
		iRet = m_Ibear->fetchRow("SELECT_BLOB_FETCH", &v_row);
		if (iRet < 0)
		{
			printf("selectBLOBFetch fetchRow ERRNO[%s]ERRORCODE[%d]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->error_code,m_Ibear->sql_error);
			iRet = -1;
			return iRet;
		}
		else if (iRet == 1)
		{
			break;
		}
		printf("----DMDB no0=[%s]\n",(char*)v_row[0]);
		printf("----DMDB no1=[%s]\n",(char*)v_row[1]);
		printf("----DMDB no2=[%s]\n",(char*)v_row[2]);
		LOB_TEST * test = (LOB_TEST*)v_row[3];
		printf("----DMDB no3=[%d,%s]\n",test->idNo,test->name);
		printf("----DMDB no4=[%s]\n",(char*)v_row[4]);
		m_Ibear->clearDatas(v_row);
	}
	return iRet;
}

int main (int argc,char *argv[])
{
	int iRet=0;
	//配置文件路径
	char filename[1024] = {0};
	char key[128]={0};
	
	if (argv[1] == NULL)
	{
		printf("input sqlmapconfig.xml plz\n");
		printf("for example:demo sqlMapConfig.xml\n");
		return -1;
	}
	strcpy(filename,argv[1]);
	
	//自定义密码测试
	//ibear_db_pwd *idp = new ibear_db_pwd();
	//stDBSection *dbs = new stDBSection();
	//idp->Custom_decryption(getDBSection,key,dbs);
	
	//sqlmap配置文件初始化
	bear_db * pInfo = new bear_db();
	pInfo ->db_id[0] = 1;//ibear_all/dsci/db.cfg中的db_id
	pInfo ->db_id[1] = 2;//ibear_all/dsci/db.cfg中的db_id
	
	//ibear类初始化
	ibear *br = new ibear(filename,pInfo);
	m_Ibear = br;
	
	//common test
	selectTestCount();
	deleteTEST();
	m_Ibear->Commit();
	
	int i=0;
	char test1[10] = {0};
	char test2[10] = {0};
	char test3[20] = {0};
	char test4[30] = {0};
	while(true)
	{
		if (i>=100)
		{
	 		break;
		}
		sprintf(test1,"abc%d",i);
		sprintf(test2,"%d",i);
		strcpy(test3,"20190725120000");
		sprintf(test4,"def-%d-20190725120000",i);
		i++;
		dmlTestAll(test1,test2,test3,test4);
		sprintf(test1,"cba%d",i);
	 	dmlTestAlldef(test1,test2,test3,test4);
	}
	m_Ibear->Commit();
	
	selectTestCount();
	selInList();
	selectTestObject();
	selectTestObjectdef();
	selectTestAll();
	selectTestAlldef();
	selectFetchdef();
	selectFetch();
	
	//dmdb lob test
	deleteBLOB();
	m_Ibear->Commit();
	int j = 0;
	while (true) {
		if (j>=100){
			break;
		}
		LOB_TEST d;
		d.idNo=j;
		sprintf(d.name,"name-%d",j);
		insertBLOB(d,j);
		j++;
	}
	m_Ibear->Commit();
	selectBLOBAll();
	selectBLOBFetch();
	selectBLOBObject(5);
	
	return 0;

}

