/**************************
 * file_name:ibear
 * function:public
 * program edition:V1.0
 * coding date:2016/11/09
 * programer:wangzhia
 **************************/

#include "ibear.h"

/*get version number and date*/
void IBEAR_ver(char *version, char *r_date)
{
	strcpy(version, IBEAR_VERSION_NUMBER);
	strcpy(r_date, IBEAR_RELEASE_DATE);
	return;
}

ibear::ibear()
{}

void ibear::initbear(char *fileName,bear_db *pInfo,char *host_id)
{
	stSettings *set = new stSettings();
	if (0 != init_XxmlFile(fileName,host_id,&m_Sqlconfs,set))
	{
		printf("init_XxmlFile is error \n");
		exit(0);
	}
	m_Info = pInfo;
	m_Settings = set;
	
	init();
}

ibear::ibear(bear_db *pInfo,map<string,stSqlMap*> sqlconfs,stSettings *set)
{
	m_Info = pInfo;
	m_Sqlconfs = sqlconfs;
	m_Settings = set;
	init();
}

ibear::ibear(char *fileName,bear_db *pInfo)
{
	stSettings *set = new stSettings();
	if (0 != init_XxmlFile(fileName,NULL,&m_Sqlconfs,set))
	{
		printf("init_XxmlFile is error \n");
		exit(0);
	}
	m_Info = pInfo;
	m_Settings = set;
	
	init();
}

ibear::ibear(char *fileName,bear_db *pInfo,char *host_id)
{
	stSettings *set = new stSettings();
	if (0 != init_XxmlFile(fileName,host_id,&m_Sqlconfs,set))
	{
		printf("init_XxmlFile is error \n");
		exit(0);
	}
	m_Info = pInfo;
	m_Settings = set;
	init();
}

void ibear::init()
{
	if(m_Info != NULL)
	{
		defDscType = true;
		dsc_Nums = m_Info->db_id;
		printf("dsc_Nums[0]=%d|dsc_Nums[1]=%d|dsc_Nums[2]=%d\n",dsc_Nums[0],dsc_Nums[1],dsc_Nums[2]);
	}
	else
	{
		defDscType = false;
	}

	int routeNo = 0;
	printf("ibear共加载%ld个sql语句\n",m_Sqlconfs.size());
	memset(sql_error,0,sizeof(sql_error));
	memset (&m_dbDsc, 0, sizeof(IBEAR_DB_INFO));
	if (0 != ibear_db_init (&(m_dbDsc.db_connect),m_Settings,sql_error))
	{
		printf ("FILE[%s]LINE[%d]ERR_MSG[ibear_db_init is failed.]\n", __FILE__, __LINE__);
		strcpy(no_error,DB_CONNECT_ERROR);
		return;
	}
	m_IbearCache = new ibear_cache();
	log_name = "ibear_performance";
	if (strcmp(m_Settings->CrossDBNums,"") != 0)
	{
		char *descs[10];
		int num = ibear_splitVector(m_Settings->CrossDBNums,'|',descs);
		for (int i=0;i <= num;i++)
		{
			CrossDBNums.push_back(atoi(descs[i]));
		}
	}
}

ibear::~ibear()
{
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		App_iLog::Instance()->ilog_Close();
		App_iLog::Close();
	}
#endif
	for (map<string,stSqlMap*>::iterator it = m_Sqlconfs.begin();it!=m_Sqlconfs.end();it++)
	{
		stSqlMap* sm = it->second;
		clearXMLList(sm->inParams);
	}
	clearXMLList(m_Sqlconfs);
	clearXMLList(m_IbearSql);
	Quit();
}

void ibear::getDBP(int db_no,void *dbp)
{
#ifdef _DSCI_DB_
	IBEAR_DbConner *DB_DSC = &(m_dbDsc.db_connect);
	DSC_DB *pDB;
	pDB = (DSC_DB *)(DB_DSC->mDSC);
#ifdef _DEBUG_INFO_
	printf("pDB->dbInfo[%d].name=%s,cflag=%d,type=%d\n",db_no,pDB->dbInfo[db_no].name,pDB->dbInfo[db_no].cFlag,pDB->dbInfo[db_no].type);
#endif
	
	if (pDB->dbInfo[db_no].cFlag == 0)
	{
		if(0 != _CDSC_ConnIMDB(pDB->error, pDB, &pDB->dbInfo[db_no]))
		{
			return;
		}
	}
	
	//默认将标示至为“修改过数据”
	pDB->dbInfo[db_no].uFlag = 1;
	
	switch(pDB->dbInfo[db_no].type)
	{
		case _T_IMDB:
			break;
		case _T_DMDB:
#ifdef DMDB
			memset(dbp ,0,sizeof(DDCI));
			memcpy(dbp,pDB->dbInfo[db_no].dbp,sizeof(DDCI));
#endif
			break;
		case _T_ORACLE:
#ifdef ORACLE
			memset(dbp ,0,sizeof(ORA_SESSION));
			memcpy(dbp,pDB->dbInfo[db_no].dbp,sizeof(ORA_SESSION));
#endif
			break;
		case _T_ALTIBASE:
			break;
		case _T_MYSQL:
			break;
	}
#endif
}

int ibear::Commit()
{
	if (strcmp(m_Settings->autoClearEnabled,"TransationClear") == 0)
	{
		m_IbearCache->ClearMemConf();
		m_IbearCache->ClearMemData();
	}
#ifdef _DSCI_
	return IBEAR_commit (&(m_dbDsc.db_connect));
#else
	return 0;
#endif
}

int ibear::Commit(int db_num)
{
	if (strcmp(m_Settings->autoClearEnabled,"TransationClear") == 0)
	{
		m_IbearCache->ClearMemConf();
		m_IbearCache->ClearMemData();
	}
	int db_no = 0;
	if (defDscType)
	{
		db_no=dsc_Nums[db_num];
	}
	else
	{
		db_no = db_num;
	}

#ifdef _DSCI_
	return IBEAR_trans (&(m_dbDsc.db_connect),0,db_no);
#else
	return 0;
#endif
}

int ibear::Rollback()
{
	if (strcmp(m_Settings->autoClearEnabled,"TransationClear") == 0)
	{
		m_IbearCache->ClearMemConf();
		m_IbearCache->ClearMemData();
	}
#ifdef _DSCI_
	return IBEAR_rollback (&(m_dbDsc.db_connect));
#else
	return 0;
#endif
}

int ibear::Rollback(int db_num)
{
	if (strcmp(m_Settings->autoClearEnabled,"TransationClear") == 0)
	{
		m_IbearCache->ClearMemConf();
		m_IbearCache->ClearMemData();
	}
	int db_no = 0;
	if (defDscType)
	{
		db_no=dsc_Nums[db_num];
	}
	else
	{
		db_no = db_num;
	}
#ifdef _DSCI_
	return IBEAR_trans (&(m_dbDsc.db_connect),1,db_no);
#else
	return 0;
#endif
}

void ibear::ClearAllMemData()
{
	m_IbearCache->ClearMemConf();
	m_IbearCache->ClearMemData();
}

void ibear::ClearTableMemData(string table)
{
	m_IbearCache->ClearMemConf(table);
	m_IbearCache->ClearMemData(table);
}

void ibear::Quit()
{
#ifdef _DSCI_
	ibear_db_quit (&(m_dbDsc.db_connect));
#else
#endif
}

int ibear::queryRoute(int (*getRoute)(void *,int *),void *rules,int *db_no)
{
	int iRet = 0;
	iRet = getRoute(rules,db_no);
	if (iRet != 0)
	{
		sprintf(sql_error,"路由出错，路由字段[%s]",(char*)rules);
		strcpy(no_error,DB_GETROUTE_ERROR);
		return iRet;
	}
	routeNo = *db_no;
	return iRet;
}

int ibear::fetchRow(const char *sql_key,vector<void*> *out_row)
{
	int iRet = 0;
	stSqlMap *sqlmap = NULL;
	int db_id = 0,db_no = 0,res_num = 0;
	
	if ((*out_row).size()!=0)
	{
		(*out_row).clear();
	}
	
	map<string,stSQLParam*> bindParams;
	//use key found sql and other info
	map<string,stSqlMap*>::iterator dataIter = m_Sqlconfs.find(string(sql_key));
	if (dataIter != m_Sqlconfs.end())
	{
		sqlmap = dataIter->second;
	}
	else
	{
		printf("sql_key %s 不存在\n",sql_key);
		sprintf(sql_error,"sql_key %s 不存在",sql_key);
		strcpy(no_error,DB_SQLKEYNOTFOUND_ERROR);
		return -1;
	}
	db_id = atoi(sqlmap->db_no);
	res_num = atoi(sqlmap->res_num);
	map<int,stDataParam*> map_data = sqlmap->outParams;
	//DMDB or PDB
	
	if (defDscType)
	{
		db_no=dsc_Nums[db_id];
	}
	else
	{
		db_no = db_id;
	}
	
	IBEAR_DbConner *pDb = IBEAR_get_db_instance (&m_dbDsc, db_no);
	IBEAR_sql *p_sql = NULL;
	IBEAR *p_db = pDb->mpDSC;
	IBEAR_row v_row;
	map<string,IBEAR_sql*>::iterator it = m_IbearSql.find(string(sql_key));
	if (it!=m_IbearSql.end())
	{
		p_sql = it->second;
		if (NULL != (v_row = IBEAR_fetch_row_s(p_db, p_sql)))
		{
			for (int j = 0; j < res_num; j++)
			{
				int lobnum = 0;
				void* si_param;
				char param_type[10] = {0};
				int param_len = 1024;
				map<int,stDataParam*>::iterator data = map_data.find(j+1);
				if (data != map_data.end())
				{
					stDataParam *dataparam = data->second;
					lobnum = dataparam->dataNum;
					strcpy(param_type,dataparam->type);
					param_len = dataparam->len;
				}
				
				if ((lobnum - 1) == j)
				{
					char *ch_param;
					if(strcmp(param_type,"clob") == 0 && 1 == IBEAR_lob_isclob(v_row[j]))   /*CLOB*/
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					else if (strcmp(param_type,"blob") == 0)
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					lobnum = 0;
				}
				else
				{
					si_param = v_row[j];
				}
				void *vparam = malloc(param_len);
				memcpy(vparam, si_param, param_len);
				(*out_row).push_back(vparam);
			}
		}
		else
		{
			return 1;
		}
	}
	else
	{
		printf("sql_key %s 不存在\n",sql_key);
		sprintf(sql_error,"sql_key %s 不存在",sql_key);
		strcpy(no_error,DB_SQLKEYNOTFOUND_ERROR);
		return -1;
	}
	
	return 0;
}

int ibear::fetchRow(const char *sql_key,vector<string> *out_row)
{
	if ((*out_row).size()!=0)
	{
		(*out_row).clear();
	}
	vector<void*> v_row;
	int ret = fetchRow(sql_key,&v_row);
	if (ret == 0)
	{
		for (vector<void*>::iterator vit = v_row.begin();vit!=v_row.end();vit++ )
		{
			char *cvalue =(char*)*vit;
			(*out_row).push_back(string(cvalue));
		}
		clearDatas(v_row);
	}
	else
	{
		return ret;
	}
}

int ibear::queryByDef(int queryType,stSqlMap **sqlmap,map<string,string> &inParams,vector<vector<string> > *list,int lobnum)
{
	map<string,void*> vParams;
	for (map<string,string>::iterator it = inParams.begin();it!=inParams.end();it++)
	{
		void *pvalue = const_cast<char*>((it->second).c_str());
		vParams.insert(pair<string,void*>(it->first,pvalue));
	}
	
	vector<vector<void*> > vlist;
	queryByDef(queryType,sqlmap,vParams,&vlist,lobnum);
	for (vector<vector<void*> >::iterator it= vlist.begin();it!=vlist.end();it++)
	{
		vector<void*> vrow = *it;
		vector<string> row;
		for (vector<void*>::iterator vit = vrow.begin();vit!=vrow.end();vit++ )
		{
			char *cvalue =(char*)*vit;
			row.push_back(string(cvalue));
		}
		(*list).push_back(row);
	}
	clearDatas(vlist);
	return 0;
}

int ibear::queryForList(const char *sql_key,map<string,string> &inParams)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<string> > list;
	iRet = queryByDef(3,&sqlmap,inParams,&list,0);
	list.clear();
	return iRet;
}

int ibear::queryForList(const char *sql_key,map<string,void*> &inParams)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<void*> > list;
	iRet = queryByDef(3,&sqlmap,inParams,&list,0);
	list.clear();
	return iRet;
}

int ibear::queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	iRet = queryByDef(1,&sqlmap,inParams,list,0);
	return iRet;
}

int ibear::queryForList(const char *sql_key,map<string,void*> &inParams,vector<vector<void*> > *list)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	iRet = queryByDef(1,&sqlmap,inParams,list,0);
	return iRet;
}

int ibear::queryForList(const char *sql_key,map<string,string> &inParams,vector<vector<string> > *list,int num)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	iRet = queryByDef(1,&sqlmap,inParams,list,num);
	return iRet;
}

int ibear::queryForObject(const char *sql_key,map<string,string> &inParams,vector<string> *object)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<string> > list;
	iRet = queryByDef(0,&sqlmap,inParams,&list,0);
	if (iRet == 0 && list.size() != 0)
	{
		(*object) = list[0];
	}
	return iRet;
}

int ibear::queryForObject(const char *sql_key,map<string,void*> &inParams,vector<void*> *object)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<void*> > list;
	iRet = queryByDef(0,&sqlmap,inParams,&list,0);
	if (iRet == 0 && list.size() != 0)
	{
		(*object) = list[0];
	}
	return iRet;
}

int ibear::DML (const char *sql_key,map<string,string> &inParams)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<string> > list;
	iRet = queryByDef(2,&sqlmap,inParams,&list,0);
	list.clear();
	return iRet;
}

int ibear::DML (const char *sql_key,map<string,string> &inParams,int *rownum)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<string> > list;
	iRet = queryByDef(2,&sqlmap,inParams,&list,0);
	if (iRet >= 0)
	{
		vector<string> row = list[0];
		string snum = row[0];
		*rownum = atoi(snum.c_str());
	}
	list.clear();
	return iRet;
}

int ibear::DML (const char *sql_key,map<string,void*> &inParams)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<void*> > list;
	iRet = queryByDef(2,&sqlmap,inParams,&list,0);
	clearDatas(list);
	return iRet;
}

int ibear::DML (const char *sql_key,map<string,void*> &inParams,int *rownum)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->id,sql_key);
	vector<vector<void*> > list;
	iRet = queryByDef(2,&sqlmap,inParams,&list,0);
	if (iRet >= 0)
	{
		vector<void*> row = list[0];
		char* snum = (char*)row[0];
		*rownum = atoi(snum);
	}
	clearDatas(list);
	return iRet;
}

int ibear::queryForListBySQL(int db_no_in,int res_num,char *sql_in,map<string,string> &inParams,vector<vector<string> > *list)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->sql, sql_in);
	sprintf(sqlmap->res_num,"%d",res_num);
	sprintf(sqlmap->db_no,"%d",db_no_in);
	iRet = queryByDef(1,&sqlmap,inParams,list,0);
	return iRet;
}

int ibear::queryForObjectBySQL(int db_no_in,char *sql_in,map<string,string> &inParams,vector<string> *object)
{
	return queryForObjectBySQL(db_no_in,0,sql_in,inParams,object);
}

int ibear::queryForListBySQL(int db_no_in,char *sql_in,map<string,string> &inParams,vector<vector<string> > *list)
{
	return queryForListBySQL(db_no_in,0,sql_in,inParams,list);
}

int ibear::queryForObjectBySQL(int db_no_in,int res_num,char *sql_in,map<string,string> &inParams,vector<string> *object)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->sql, sql_in);
	sprintf(sqlmap->res_num,"%d",res_num);
	sprintf(sqlmap->db_no,"%d",db_no_in);
	vector<vector<string> > list;
	iRet = queryByDef(0,&sqlmap,inParams,&list,0);
	if (iRet == 0 && list.size() != 0)
	{
		(*object) = list[0];
	}
	return iRet;
}

int ibear::DMLBySQL (int db_no_in,char *sql_in,map<string,string> &inParams)
{
	int iRet = 0;
	stSqlMap *sqlmap = new stSqlMap();
	strcpy(sqlmap->sql, sql_in);
	sprintf(sqlmap->db_no,"%d",db_no_in);
	vector<vector<string> > list;
	iRet = queryByDef(2,&sqlmap,inParams,&list,0);
	list.clear();
	return iRet;
}

char* ibear::getDBNum(const char *sql_key)
{
	map<string,stSqlMap*>::iterator dataIter = m_Sqlconfs.find(string(sql_key));
	if (dataIter != m_Sqlconfs.end())
	{
		stSqlMap *sm = dataIter->second;
		return sm->db_no;
	}
	else
	{
		return NULL;
	}
}

int ibear::getDBNum_real(const char *sql_key)
{
	map<string,stSqlMap*>::iterator dataIter = m_Sqlconfs.find(string(sql_key));
	if (dataIter != m_Sqlconfs.end())
	{
		stSqlMap *sm = dataIter->second;
		if (sm->db_no == NULL || strcmp(sm->db_no,"") == 0)
		{
			if (ibear_AllisNum(m_Settings->defaultDBNum))
			{
				return atoi(m_Settings->defaultDBNum);
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if (ibear_AllisNum(sm->db_no))
			{
				return atoi(sm->db_no);
			}
			else
			{
				return -1;
			}
		}
	}
	return -1;
}

void ibear::clearPrepareSql(int db_no)
{
	IBEAR_DbConner *pDb = IBEAR_get_db_instance (&m_dbDsc, db_no);
	IBEAR_row v_row;
	IBEAR_sql *p_sql = NULL;
	IBEAR *p_db = pDb->mpDSC;
	PRESQL *preSqls = IBEAR_get_presql (pDb, db_no);
	for (int i= 0;i<preSqls->_next_seq;i++)
	{
		memset(preSqls->msql[i],0,sizeof(preSqls->msql[i]));
	}
	preSqls->_next_seq = 0;
}

int ibear::queryByDef(int queryType,stSqlMap **sqlmap,map<string,void*> &inParams,vector<vector<void*> > *list,int lobnum)
{
	int iRet = 0;
	char sql_out[1024] = {0};
	int defdb_no = 0;
	map<string,void*>::iterator it = inParams.find(string("ROUTE_KEY"));
	if (it != inParams.end())
	{
		char *rules = (char*)it->second;
		iRet = getRouteDef(rules,&defdb_no);
		if (iRet != 0)
		{
			printf ("FILE[%s]LINE[%d]ERR_MSG[getRouteDef is ERROR]\n", __FILE__, __LINE__);
			sprintf(sql_error,"ERR_MSG[getRouteDef is ERROR]");
			strcpy(no_error,DB_GETROUTE_ERROR);
			return iRet;
		}
		routeNo = defdb_no;
	}
	else
	{
		if (strcmp((*sqlmap)->db_no_real,"?") == 0)
		{
			printf ("FILE[%s]LINE[%d]ERR_MSG[ROUTE_KEY is NULL]\n", __FILE__, __LINE__);
			sprintf(sql_error,"ERR_MSG[ROUTE_KEY is NULL]");
			strcpy(no_error,DB_GETROUTE_ERROR);
			return -1;
		}
	}
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_Begin(log_name);
	}
#endif
	//初始化ibear参数
	iRet  = initConf(&m_Sqlconfs, inParams, sql_out, sqlmap, routeNo);
	if (iRet != 0)
	{
		//clearXMLList(inParams);
		inParams.clear();
#ifdef _PERFORMANCE_
		if(m_Settings->performanceAnalyzeEnabled == 1)
		{
			ilog_End(log_name,string("ERR|")+string(sql_out));
			if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
			{
				WriteLog(log_name,LOG_APP_PERFORMANCE,"ERR|%s",sql_out);
			}
		}
#endif
		return iRet;
	}
	int db_no = 0;
	int num  = atoi((*sqlmap)->db_no);
	if (defDscType)
	{
		db_no=dsc_Nums[num];
#ifdef _DEBUG_INFO_
		printf("db[%d]=%d,resnum=[%s],sql_out=%s\n",num,db_no,(*sqlmap)->res_num,sql_out);
#endif
	}
	else
	{
		db_no = num;
#ifdef _DEBUG_INFO_
		printf("db=%d,resnum=[%s],sql_out=%s\n",db_no,(*sqlmap)->res_num,sql_out);
#endif
	}
	
	if (queryType == 1 || queryType == 0)
	{
		iRet = m_IbearCache->loadDesc((*sqlmap)->memType, sql_out, db_no, atoi((*sqlmap)->res_num), inParams, m_Settings->maxOverflaw);
		inParams.clear();
		//clearXMLList(inParams);
		if (iRet == IBEAR_MEMCACHE_GETDATA)
		{
			*list = m_IbearCache->getRows();
#ifdef _PERFORMANCE_
			if(m_Settings->performanceAnalyzeEnabled == 1)
			{
				ilog_End(log_name,string("MEM|")+string(sql_out));
				if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
				{
					WriteLog(log_name,LOG_APP_PERFORMANCE,"MEM|%s",sql_out);
				}
			}
#endif
			return 0;
		}
	}
	
	//绑定变量
	stSQLParam *para_list[MAX_PARAM_NUM];
	int paraNum = ibear_bindParam((*sqlmap)->inParams,para_list);
	string sqlvalue = "";
	for (int i = 0; i < paraNum; i++)
	{
		stSQLParam *param = para_list[i];
		char *pvalue = NULL;
		if (strcmp(param->type,"blob")==0)
		{
			char cvalue[10] = "blob";
			pvalue = cvalue;
		}
		else
		{
			pvalue = (char*)param->values;
			if ((*sqlmap)->testModel)
			{
				char si[10] = {0};
				sprintf(si, "%d",i);
				sqlvalue.append("[").append(si).append("]").append(pvalue);
			}
		}
	}
	if (queryType == 2)
	{
		if ((*sqlmap)->testModel)
		{
#ifdef _PERFORMANCE_
			WriteLog(log_name,LOG_SYS_NORMAL,"[%d]%s|%s",db_no,sql_out,sqlvalue.c_str());
#endif
			vector<void*> params;
			void *prow = malloc(10);
			char crow[10] = {0};
			sprintf(crow, "1");
			//prow = crow;
			memcpy(prow, crow, sizeof(prow));
			params.push_back(prow);
			(*list).push_back(params);
			return 0;
		}
	}
	
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_Mark(log_name,"init");
	}
#endif
	IBEAR_DbConner *pDb = IBEAR_get_db_instance (&m_dbDsc, db_no);
	IBEAR_row v_row;
	IBEAR_sql *p_sql = NULL;
	IBEAR *p_db = pDb->mpDSC;
	PRESQL *preSqls = IBEAR_get_presql (pDb, db_no);
	
	bool repre = true;
	char db_type[10] = {0};
#ifdef _DSCI_DB_
	DSC_DB *pDscBb;
	pDscBb = (DSC_DB *)(pDb->mDSC);
	switch(pDscBb->dbInfo[db_no-1].type)
	{
		case _T_IMDB:
			break;
		case _T_DMDB:
			repre = false;
			strcpy(db_type,"dmdb");
#ifdef _DEBUG_INFO_
			printf("pDscBb->dbInfo[%d].type=DMDB\n",db_no-1);
#endif
			break;
		case _T_ORACLE:
			repre = true;
			strcpy(db_type,"oracle");
#ifdef _DEBUG_INFO_
			printf("pDscBb->dbInfo[%d].type=ORACLE\n",db_no-1);
#endif
			break;
		case _T_ALTIBASE:
			break;
		case _T_MYSQL:
#ifdef _DEBUG_INFO_
			printf("pDscBb->dbInfo[%d].type=MYSQL\n",db_no-1);
#endif
			strcpy(db_type,"mysql");
			break;
	}
#endif
	bool reConnect = false;
	if(m_Settings->reConnect == 1 && !repre)
	{
		//DMDB reConnect oracle not reconnect
		reConnect = true;
	}
	//预解析
	for (int reConnectNum=0;reConnectNum<=2;reConnectNum++)
	{
		if (NULL == (p_sql = db_PrepareSql(p_db, preSqls, sql_out, db_no, repre)))
		{
			printf ("FILE[%s]LINE[%d]ERR_MSG[%s][%s][%d]\n", __FILE__, __LINE__, IBEAR_error(p_db),sql_out,db_no);
			sprintf(sql_error,"ERR_MSG[%s][%s][%d]",IBEAR_error(p_db),sql_out,db_no);
			strcpy(no_error,DB_PREPARESQL_ERROR);
			error_code = IBEAR_ecode(p_db);
			if (error_code > 0)
			{
				if (reConnect)
				{
					clearPrepareSql(db_no);
					printf("[%d]ReConnect error_code=%d\n",reConnectNum,error_code);
					continue;
				}
				return -9999;
			}
			else if (error_code < 0)
			{
				return -9999;
			}
	#ifdef _PERFORMANCE_
			if(m_Settings->performanceAnalyzeEnabled == 1)
			{
				ilog_End(log_name,string("ERR|")+string(sql_out));
				if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
				{
					WriteLog(log_name,LOG_APP_PERFORMANCE,"ERR|%s",sql_out);
				}
			}
	#endif
			return -1;
		}
		break;
	}
	
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_Mark(log_name,"presql");
	}
#endif
	long lvalues[MAX_PARAM_NUM];
	int ivalues[MAX_PARAM_NUM];
	char param_values[1024] = {0};
	//参数赋值
	for (int i = 0; i < paraNum; i++)
	{
		stSQLParam *param = para_list[i];
		char *pvalue = NULL;
		if (strcmp(param->type,"blob")==0)
		{
			char cvalue[10] = "blob";
			pvalue = cvalue;
		}
		else
		{
			pvalue = (char*)param->values;
		}
#ifdef _DEBUG_INFO_
		printf("\t%d-[%s][%s]\n",i,param->type,pvalue);
#endif
		if (strcasecmp(param->type,"int") == 0)
		{
			if (!ibear_AllisNum(pvalue))
			{
				printf ("FILE[%s]LINE[%d]ERR_MSG[valuetype is not number]\n", __FILE__, __LINE__);
				sprintf(sql_error,"ERR_MSG[valuetype is not int]");
				strcpy(no_error,DB_DATATYPE_ERROR);
				return -4;
			}
			ivalues[i] = atoi(pvalue);
			IBEAR_bind_param (p_db, p_sql, i, DSCI_TYPE_INT, &(ivalues[i]));
		}
		else if (strcasecmp(param->type,"long") == 0)
		{
			if (!ibear_AllisNum(pvalue))
			{
				printf ("FILE[%s]LINE[%d]ERR_MSG[valuetype is not number]\n", __FILE__, __LINE__);
				sprintf(sql_error,"ERR_MSG[valuetype is not long]");
				strcpy(no_error,DB_DATATYPE_ERROR);
				return -4;
			}
			lvalues[i] = atol(pvalue);
			IBEAR_bind_param (p_db, p_sql, i, DSCI_TYPE_LONG, &(lvalues[i]));
		}
		else if (strcasecmp(param->type,"char") == 0)
		{
			IBEAR_bind_param (p_db, p_sql, i, DSCI_TYPE_CHAR, pvalue);
		}
		else if (strcasecmp(param->type,"date") == 0)
		{
			if (!ibear_IsDate(pvalue))
			{
				printf ("FILE[%s]LINE[%d]ERR_MSG[valuetype is not date]\n", __FILE__, __LINE__);
				sprintf(sql_error,"ERR_MSG[valuetype is not date]");
				strcpy(no_error,DB_DATATYPE_ERROR);
				return -4;
			}
			IBEAR_bind_param (p_db, p_sql, i, DSCI_TYPE_DATE, pvalue);
		}
		else if (strcasecmp(param->type,"blob") == 0)
		{
			if (param->len == 0)
			{
				printf ("FILE[%s]LINE[%d]ERR_MSG[valuetype is blob ,len must greater than 0]\n", __FILE__, __LINE__);
				sprintf(sql_error,"ERR_MSG[valuetype is blob ,len must greater than 0]");
				strcpy(no_error,DB_DATATYPE_ERROR);
				return -4;
			}
			IBEAR_bind_blob_param (p_db, p_sql, i, param->values,param->len);
		}
		else if (strcasecmp(param->type,"clob") == 0)
		{
			IBEAR_bind_clob_param (p_db, p_sql, i, pvalue);
		}
		else if (strcasecmp(param->type,"lob") == 0)
		{
			IBEAR_bind_clob_param (p_db, p_sql, i, pvalue);
		}
		
		sprintf(param_values, "%s|%s", param_values,pvalue);
	}
	
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_Mark(log_name,"bind");
	}
#endif
	for (int reConnectNum=0;reConnectNum<=2;reConnectNum++)
	{
		if (IBEAR_exec_p(p_db, p_sql) != 0)
		{
			printf ("FILE[%s]LINE[%d]ERR_MSG[%s]\n", __FILE__, __LINE__, IBEAR_error(p_db));
			sprintf(sql_error,"ERR_MSG[%s]",IBEAR_error(p_db));
			strcpy(no_error,DB_EXECSQL_ERROR);
			error_code = IBEAR_ecode(p_db);
			if (error_code > 0)
			{
				if (reConnect)
				{
					clearPrepareSql(db_no);
					printf("[%d]ReConnect error_code=%d\n",reConnectNum,error_code);
					continue;
				}
				return -9999;
			}
			else if (error_code < 0)
			{
				return -9999;
			}
	#ifdef _PERFORMANCE_
			if(m_Settings->performanceAnalyzeEnabled == 1)
			{
				ilog_End(log_name,string("ERR|")+string(sql_out));
				if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
				{
					WriteLog(log_name,LOG_APP_PERFORMANCE,"ERR|%s",sql_out);
				}
			}
	#endif
			return -1;
		}
		break;
	}
	
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_Mark(log_name,"exec");
	}
#endif
	if (queryType == 0)
	{
		int cur_num = 0;
		map<int,stDataParam*> map_data = (*sqlmap)->outParams;
		while (NULL != (v_row = IBEAR_fetch_row_s(p_db, p_sql)))
		{
			if (cur_num > 0)
			{
				continue;
			}
			vector<void*> params;
			for (int j = 0; j < atoi((*sqlmap)->res_num); j++)
			{
				void* si_param;
				char param_type[10] = {0};
				int param_len = 1024;
				if (lobnum == 0)
				{
					map<int,stDataParam*>::iterator data = map_data.find(j+1);
					if (data != map_data.end())
					{
						stDataParam *dataparam = data->second;
						lobnum = dataparam->dataNum;
						strcpy(param_type,dataparam->type);
						param_len = dataparam->len;
					}
				}
				
				if ((lobnum - 1) == j)
				{
					char *ch_param;
					if(strcmp(param_type,"clob") == 0 && 1 == IBEAR_lob_isclob(v_row[j]))   /*CLOB*/
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					else if (strcmp(param_type,"blob") == 0)
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					lobnum = 0;
				}
				else
				{
					si_param = v_row[j];
				}
				void *vparam = malloc(param_len);
				memcpy(vparam, si_param, param_len);
				params.push_back(vparam);
				cur_num++;
			}
			(*list).push_back(params);
		}
		m_IbearCache->addCacheValue(list);
	}
	else if (queryType == 1)
	{
		map<int,stDataParam*> map_data = (*sqlmap)->outParams;
		while (NULL != (v_row = IBEAR_fetch_row_s(p_db, p_sql)))
		{
			vector<void*> params;
			for (int j = 0; j < atoi((*sqlmap)->res_num); j++)
			{
				void* si_param;
				char param_type[10] = {0};
				int param_len = 1024;
				if (lobnum == 0)
				{
					map<int,stDataParam*>::iterator data = map_data.find(j+1);
					if (data != map_data.end())
					{
						stDataParam *dataparam = data->second;
						lobnum = dataparam->dataNum;
						strcpy(param_type,dataparam->type);
						param_len = dataparam->len;
					}
				}
				
				if ((lobnum - 1) == j)
				{
					char *ch_param;
					if(strcmp(param_type,"clob") == 0 && 1 == IBEAR_lob_isclob(v_row[j]))   /*CLOB*/
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					else if (strcmp(param_type,"blob") == 0)
					{
						ch_param = IBEAR_lob_getp (v_row[j]);
						si_param = ch_param;
					}
					lobnum = 0;
				}
				else
				{
					si_param = v_row[j];
				}
				//printf("---%s\n",si_param);
				void *vparam = malloc(param_len);
				memcpy(vparam, si_param, param_len);
				params.push_back(vparam);
			}
			(*list).push_back(params);
		}
		/*int vnum=0;
		for (vector<vector<void*> >::iterator pt=(*list).begin();pt!=(*list).end();pt++)
		{
			vector<void*> cc=*pt;
			printf("----[%d]%x,%s,%s,%s,%s,%s\n",vnum++,cc[0],(char*)cc[0],(char*)cc[1],(char*)cc[2],(char*)cc[3],(char*)cc[4]);
		}*/
		m_IbearCache->addCacheValue(list);
	}
	else if (queryType == 2)
	{
		vector<void*> params;
		int rownum = IBEAR_num_rows(p_db);
		void *prow = malloc(10);
		char crow[10] = {0};
		sprintf(crow, "%d", rownum);
		//prow = crow;
		memcpy(prow, crow, sizeof(prow));
		params.push_back(prow);
		(*list).push_back(params);
		if (rownum <= 0)
		{
			strcpy(sql_error,"exe num is NULL");
#ifdef _PERFORMANCE_
			if(m_Settings->performanceAnalyzeEnabled == 1)
			{
				ilog_End(log_name,string("DB|")+string(sql_out));
				if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
				{
					WriteLog(log_name,LOG_APP_PERFORMANCE,"DB|%s%s",sql_out,param_values);
				}
			}
#endif
			iRet = 1;
			return iRet;
		}
	}
	else if (queryType == 3)
	{
		m_IbearSql.insert(pair<string, IBEAR_sql*>(string((*sqlmap)->id),p_sql));
	}
#ifdef _PERFORMANCE_
	if(m_Settings->performanceAnalyzeEnabled == 1)
	{
		ilog_End(log_name,string("DB|")+string(sql_out));
		if (0 == JudgePerformance(log_name,m_Settings->performanceThreshold))
		{
			WriteLog(log_name,LOG_APP_PERFORMANCE,"DB|%s%s",sql_out,param_values);
		}
	}
#endif
	return 0;
}

void ibear::clearDatas (vector<vector<void*> > &mList)
{
	if (mList.size() == 0)
	{
		return;
	}
	
	vector<void*> vList;
	vector<void*>::iterator vIter;
	vector<vector<void*> >::iterator iter;
	
	for (iter = mList.begin(); iter != mList.end(); iter++)
	{
		vList = *iter;
		for (vIter = vList.begin(); vIter != vList.end(); vIter++)
		{
			void *pNode = *vIter;
			free(pNode);
			pNode = NULL;
		}
		
		vList.clear();
	}
	
	mList.clear();
	
	return;
}

void ibear::clearDatas (vector<void*> &mList)
{
	if (mList.size() == 0)
	{
		return;
	}
	
	vector<void*>::iterator vIter;
	for (vIter = mList.begin(); vIter != mList.end(); vIter++)
	{
		void *pNode = *vIter;
		free(pNode);
		pNode = NULL;
	}
	
	mList.clear();
	
	return;
}

