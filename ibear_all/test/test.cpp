/*
create table abc
(
a VARCHAR2(40),
b NUMBER(14),
c date
);
 
 create table abc_lob
 (
 a VARCHAR(40),
 b NUMBER(14),
 c date,
 d lob(128),
 e lob(200),
 primary key (a)
 ) route type all;
 create index abc_ind on abc(a);
 
 create table abc_lob2
 (
 a VARCHAR(40) PRIMARY KEY,
 b int(14),
 c date,
 d blob,
 e blob
 ) ENGINE=MyISAM  DEFAULT CHARSET=utf8;
 
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
#include <regex.h>

using namespace std;

int g_runflag;//0: run  1：stop
ibear *m_Ibear;

typedef struct
{
	char  name[52];
	int  idNo;
}LOB_TEST;

//跨库测试
int selectTest()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("ID_NO"),string("220100200004600017")));
	iRet = m_Ibear->queryForObject("SELECT_TEST",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTest ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----跨库测试：%s\n",row[0].c_str());
	return iRet;
}

int selectTestDMDB()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("A"),string("1")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_DMDB",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTestDMDB ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----DMDB no=[%s]\n",row[0].c_str());
	return iRet;
}
//缓存测试
int selectTest_mem()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_MEM",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTest_mem ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	if (row.size()>0)
	{
		printf("----缓存测试：%s|%s\n",row[0].c_str(),row[1].c_str());
	}
	return iRet;
}

//缓存测试2
int selectTest_mem1()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_MEM1",inParams,&row);
	if (iRet != 0)
	{
		printf("selectTest_mem1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	if (row.size()>0)
	{
		printf("----缓存测试1：%s|%s\n",row[0].c_str(),row[1].c_str());
	}
	return iRet;
}

//resultNum自动计算测试,默认库测试
int selectAbc()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	//resultnum=0
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_OBJ",inParams,&row);
	if (iRet != 0)
	{
		printf("selectAbc ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----resultNum自动计算测试,默认库测试（resultnum=0）：%s\n",row[1].c_str());
	
	//resultnum=null
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_OBJ1",inParams,&row);
	if (iRet != 0)
	{
		printf("selectAbc ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----resultNum自动计算测试,默认库测试（resultnum=null）：%s\n",row[1].c_str());
	
	//resultnum=4
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForObject("SELECT_ABC_OBJ2",inParams,&row);
	if (iRet != 0)
	{
		printf("selectAbc ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----resultNum自动计算测试,默认库测试（resultnum=4）：%s\n",row[1].c_str());
	return iRet;
}

//list测试
int selectAbclist()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	
	inParams.insert(pair<string,string>(string("A"),string("asdb")));
	iRet = m_Ibear->queryForList("SELECT_ABC_LIST",inParams,&list);
	if (iRet != 0)
	{
		printf("selectAbclist ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> row = *it;
		printf("----list测试：%s|%s|%s|%s\n",row[0].c_str(),row[1].c_str(),row[2].c_str(),row[3].c_str());
	}
	
	return iRet;
}

//fetch测试
int selectTestlistFetch()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("ID_NO"),string("220400200024322050")));
	inParams.insert(pair<string,string>(string("PHONE_NO"),string("20210721354")));
	iRet = m_Ibear->queryForList("SELECT_TEST_LIST",inParams);
	if (iRet != 0)
	{
		printf("selectTestlistFetch ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	while (1)
	{
		iRet = m_Ibear->fetchRow("SELECT_TEST_LIST", &row);
		if (iRet < 0)
		{
			printf("selectTestlistFetch fetchRow ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
			iRet = -1;
			return iRet;
		}
		else if (iRet == 1)
		{
			break;
		}
		
		printf("----fetch测试：%s\n",row[0].c_str());
	}
	return iRet;
}

int insertTest(int i)
{
	int iRet = 0;
	map<string,string> inParams;
	
	char a[10] = {0};
	sprintf(a,"%d",i);
	
	inParams.insert(pair<string,string>(string("A"),string(a)));
	inParams.insert(pair<string,string>(string("B"),string("-456")));
	inParams.insert(pair<string,string>(string("C"),string("20180910000000")));
	iRet = m_Ibear->DML("INSERT_ABC",inParams);
	if (iRet != 0)
	{
		printf("insertTest ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	m_Ibear->Commit();
	printf("----DML测试，默认库\n");
	return iRet;
}

int insertBLOB(LOB_TEST d,int i)
{
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
		printf("insertBLOB ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	m_Ibear->Commit();
	printf("----DML测试，默认库\n");
	return iRet;
}

int selectBLOB(int i)
{
	int iRet = 0;
	map<string,void*> inParams;
	vector<vector<void*> > list;
	char a[10] = {0};
	char c[20] = {0};
	sprintf(a, "%d",i);
	strcpy(c,"20180910000000");
	inParams.insert(pair<string,void*>(string("A"),&a));
	inParams.insert(pair<string,void*>(string("C"),&c));
	
	iRet = m_Ibear->queryForList("SELECT_BLOB",inParams,&list);
	if (iRet != 0)
	{
		printf("selectBLOB ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<void*> >::iterator it = list.begin();it!=list.end();it++){
		vector<void*> row = *it;
		printf("----DMDB no0=[%s]\n",(char*)row[0]);
		printf("----DMDB no1=[%s]\n",(char*)row[1]);
		printf("----DMDB no2=[%s]\n",(char*)row[2]);
		LOB_TEST * test = (LOB_TEST*)row[3];
		printf("----DMDB no3=[%d,%s]\n",test->idNo,test->name);
		printf("----DMDB no4=[%s]\n",(char*)row[4]);
	}
	return iRet;
}

int selectBLOBDe(int i)
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	char a[10] = {0};
	char c[20] = {0};
	sprintf(a, "%d",i);
	strcpy(c,"20180910000000");
	inParams.insert(pair<string,string>(string("A"),string(a)));
	inParams.insert(pair<string,string>(string("C"),string(c)));
	
	iRet = m_Ibear->queryForList("SELECT_BLOB",inParams,&list,3);
	if (iRet != 0)
	{
		printf("selectBLOB ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	return iRet;
}

int selectBLOBFetch()
{
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
			printf("selectBLOBFetch fetchRow ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
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
	}
	return iRet;
}
//自定义单条测试
int SeleteTest_SELF()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("ID_NO"),string("220400200024322050")));
	inParams.insert(pair<string,string>(string("PHONE_NO"),string("20210721354")));
	char sql_in[1024] = {0};
	sprintf(sql_in, "select count(1) from ur_user_info where id_no =${ID_NO|long} $E[and phone_no=${PHONE_NO|char}]");
	iRet = m_Ibear->queryForObjectBySQL(1,1,sql_in,inParams,&row);
	if (iRet != 0)
	{
		printf("SeleteTest_SELF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----自定义单条测试（1库）：%s\n",row[0].c_str());
	row.clear();
	inParams.insert(pair<string,string>(string("ID_NO"),string("220400200024322050")));
	inParams.insert(pair<string,string>(string("PHONE_NO"),string("20210721354")));
	iRet = m_Ibear->queryForObjectBySQL(1,sql_in,inParams,&row);
	if (iRet != 0)
	{
		printf("SeleteTest_SELF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----自定义单条测试（默认库）：%s\n",row[0].c_str());
	
	return iRet;
}

//自定义list测试
int SeleteTestlist_SELF()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	
	char sql_in[1024] = {0};
	sprintf(sql_in, "select phone_no from ur_user_info where rownum < 10");
	iRet = m_Ibear->queryForListBySQL(0,0,sql_in,inParams,&list);
	if (iRet != 0)
	{
		printf("SeleteTestlist_SELF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> row = *it;
		printf("----自定义list测试（0库）：%s\n",row[0].c_str());
	}
	
	list.clear();
	sprintf(sql_in, "select id_no from ur_user_info where rownum < 10");
	iRet = m_Ibear->queryForListBySQL(0,sql_in,inParams,&list);
	if (iRet != 0)
	{
		printf("SeleteTestlist_SELF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> row = *it;
		printf("----自定义list测试（默认库）：%s\n",row[0].c_str());
	}
	return iRet;
}

//自定义DML
int insertTest_DEF()
{
	int iRet = 0;
	map<string,string> inParams;
	
	inParams.insert(pair<string,string>(string("A"),string("bcsa")));
	inParams.insert(pair<string,string>(string("B"),string("3333")));
	inParams.insert(pair<string,string>(string("C"),string("20180910000011")));
	inParams.insert(pair<string,string>(string("D"),string("11111")));
	char sql_in[1024] = {0};
	sprintf(sql_in, "insert into abc values(${A|char},${B|long},to_date(${C|char},'yyyymmddhh24miss'),${D|long})");
	iRet = m_Ibear->DMLBySQL(0,sql_in,inParams);
	if (iRet != 0)
	{
		printf("insertTest_DEF ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	m_Ibear->Commit();
	printf("----自定义DML\n");
	return iRet;
}

int getRoute(char *rules,int *db_no)
{
	*db_no = 1;
	return 0;
}

int testDSCI(char *a,int b,int d)
{
	IBEAR_DB_INFO m_dbDsc;
	memset (&m_dbDsc, 0, sizeof(IBEAR_DB_INFO));
	char sql_error[1024] = {0};
	memset(sql_error,0,sizeof(sql_error));
	stSettings *set = new stSettings();
	set->autoCommit = 1;
	if (0 != ibear_db_init (&(m_dbDsc.db_connect),set,sql_error))
	{
		return -1;
	}
	printf("init ...\n");
	int db_no = 1;
	IBEAR_row v_row;
	IBEAR_row v_row1;
	IBEAR_row v_row2;
	IBEAR_DbConner *pDb = IBEAR_get_db_instance (&m_dbDsc, db_no);
	IBEAR_sql *p_sql = NULL;
	IBEAR *p_db = pDb->mpDSC;
	PRESQL *preSqls = IBEAR_get_presql (pDb, db_no);
	char sql_out[1024] = {0};
	strcpy(sql_out,"select count(1) from abc where b=? and a=? and d=?");
	if (NULL == (p_sql = db_PrepareSql(p_db, preSqls, sql_out, db_no,false)))
	{
		return -1;
	}
	printf("preparesql ...sql_out=%s\n",sql_out);
	
	IBEAR_bind_param (p_db, p_sql, 0, DSCI_TYPE_INT, &b);
	IBEAR_bind_param (p_db, p_sql, 1, DSCI_TYPE_CHAR, a);
	IBEAR_bind_param (p_db, p_sql, 2, DSCI_TYPE_INT, &d);
	
	if (IBEAR_exec_p(p_db, p_sql) != 0)
	{
		return -1;
	}
	printf("exec ...\n");
	
	while (NULL != (v_row = IBEAR_fetch_row_s(NULL, p_sql)))
	{
		printf("fetch ...\n");
		printf("1[%s]\n",string(v_row[0]).c_str());
	}
}

int selectComplexT1()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> row;
	
	inParams.insert(pair<string,string>(string("ID_NO"),string("220101000000008015")));
	iRet = m_Ibear->queryForObject("SELECT_COMPLEX_T1",inParams,&row);
	if (iRet != 0)
	{
		printf("selectComplexT1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = -1;
		return iRet;
	}
	printf("----复杂sql测试1：%s\n",row[0].c_str());
	return iRet;
}

int q_UserRel()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<string> v_row;
	inParams.insert(pair<string,string>(string("SLAVE_ID"),string("1")));
	inParams.insert(pair<string,string>(string("MASTER_ID"),string("1")));
	inParams.insert(pair<string,string>(string("RELATION1"),string("1")));
	inParams.insert(pair<string,string>(string("RELATION2"),string("2")));
	printf("vip=%s\n","q_UserRel");
	iRet = m_Ibear->queryForObject("qUserRel",inParams,&v_row);
	if (iRet < 0)
	{
		printf("qUserRel ERRNO[%s]ERR_MSG[%s] iRet[%d]\n",m_Ibear->no_error,m_Ibear->sql_error,iRet);
		return iRet;
	}
	if (v_row.size()>0)
	{
		printf("%s\n",v_row[0].c_str());
	}
	return v_row.size();
}

//构造1
int test1(char *filename)
{
	//sqlmap配置文件初始化
	map<string,stSqlMap*> sqlconfs;
	stSettings *settings = new stSettings();
	init_XxmlFile(filename,NULL, &sqlconfs,settings);
	bear_db * pInfo = new bear_db();
	pInfo ->db_id[0] = 1;/*ibear_all/dsci/db.cfg中的db_id*/
	pInfo ->db_id[1] = 2;/*ibear_all/dsci/db.cfg中的db_id*/
	ibear *br = new ibear(pInfo,sqlconfs, settings);
	m_Ibear = br;
	int db_no = 0;
	//m_Ibear->queryRoute(getRoute,NULL,&db_no);
	
	selectTestlistFetch();
	selectAbclist();
	SeleteTestlist_SELF();
	insertTest_DEF();
	selectAbc();
	SeleteTest_SELF();
	return 0;
}

void testbug1()
{
	int iRet = 0;
	map<string,string> inParams;
	vector<vector<string> > list;
	iRet = m_Ibear->queryForList("selbillplan",inParams,&list);
	if (iRet != 0)
	{
		printf("testbug1 ERRNO[%s]ERR_MSG[%s]\n",m_Ibear->no_error,m_Ibear->sql_error);
		return;
	}
	for (vector<vector<string> >::iterator it = list.begin();it != list.end();it++)
	{
		vector<string> v_row = *it;
		/*printf("%s|",(static_cast<string *>(v_row[0])).c_str());
		printf("%s|",(static_cast<string *>(v_row[1])).c_str());
		printf("%s|",(static_cast<string *>(v_row[2])).c_str());
		printf("%s|",(static_cast<string *>(v_row[3])).c_str());
		printf("%s\n",(static_cast<string *>(v_row[4])).c_str());*/
		printf("%s|",v_row[0].c_str());
		printf("%s|",v_row[1].c_str());
		printf("%s|",v_row[2].c_str());
		printf("%s|",v_row[3].c_str());
		printf("%s\n",v_row[4].c_str());
	}
}

//构造1
int test2(char *filename)
{
	bear_db * pInfo = new bear_db();
	pInfo ->db_id[0] = 1;/*ibear_all/dsci/db.cfg中的db_id*/
	//pInfo ->db_id[1] = 2;/*ibear_all/dsci/db.cfg中的db_id*/
	ibear *br = new ibear(filename,NULL);
	m_Ibear = br;
	
	int i=1;
	while(1)
	{
		testbug1();
		/*LOB_TEST d;
		d.idNo=i;
		sprintf(d.name,"name-%d",i);
		//i++;
		insertTest(i);
		selectTestDMDB();
		//insertBLOB(d,i);
		//selectBLOB(i);
		//selectBLOBFetch();
		break;
		i++;
		if (i>=1000)
			i = 1;*/
	}
	return 0;
}


int main (int argc,char *argv[])
{
	//配置文件路径
	char filename[1024] = {0};
	
	if (argv[1] == NULL)
	{
		printf("请输入sqlmapconfig.xml路径\n");
		printf("例如:demo sqlMapConfig.xml\n");
		return -1;
	}
	strcpy(filename,argv[1]);
	test2(filename);
	
	/*LOB_TEST lob;
	memset(&lob, 0, sizeof(LOB_TEST));
	lob.idNo=1;
	strcpy(lob.name,"abc");
	void *p = malloc(sizeof(LOB_TEST));
	memcpy(p, &lob, sizeof(LOB_TEST));
	printf("%s\n",p);*/
	return 0;
}

