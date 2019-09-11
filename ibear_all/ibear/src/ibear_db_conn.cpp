/**************************
 * 服务名称:ibear_db_conn
 * 功能描述:公共文件
 * 程序版本:V1.0
 * 编码日期:2016/11/09
 * 编码人员:wangzhia
 **************************/
#include "ibear_db_conn.h"

//预解析
IBEAR_sql *db_PrepareSql (IBEAR *mpDSC, PRESQL *preSqls, char *sql, int db_no, bool repre)
{
	int flag = 0;
	IBEAR_sql *p_SQL;
	/*//直接使用预解析函数即可
	printf("preSqls->_next_seq[%d]\n",preSqls->_next_seq);
	p_SQL = &(preSqls->_sql[preSqls->_next_seq]);
#ifdef _DSCI_
	if (DSCI_prepare_sql_f (mpDSC, p_SQL, sql, db_no) < 0)
	{
		return NULL;
	}
	preSqls->_next_seq++;
#else
#endif*/
	
	/*无需使用这种方式，会导致内存泄漏（直接使用预解析函数即可）*/
	//printf("sql=%s\n",sql);
	//printf("preSqls->_next_seq=%d\n",preSqls->_next_seq);
	for (int i= 0;i<preSqls->_next_seq;i++)
	{
		p_SQL = &(preSqls->_sql[i]);
		if (strcmp(preSqls->msql[i],sql) == 0)
		{
			if (repre)
			{
				#ifdef _DSCI_
				if (DSCI_prepare_sql_f (mpDSC, p_SQL, sql, db_no) < 0)
				{
					return NULL;
				}
				#else
				#endif
			}
			return p_SQL;
		}
	}
	
	p_SQL = &(preSqls->_sql[preSqls->_next_seq]);
	#ifdef _DSCI_
	if (DSCI_prepare_sql_f (mpDSC, p_SQL, sql, db_no) < 0)
	{
		return NULL;
	}
	#else
	#endif
	strcpy(preSqls->msql[preSqls->_next_seq],sql);

	preSqls->_next_seq++;
	
	return p_SQL;
}

//数据库初始化
int ibear_db_init (IBEAR_DbConner *DB_DSC,stSettings *set,char *err_msg)
{
	char msg[512] = {0};
#ifdef _DSCI_
	if (0 != DSCI_init (msg, &(DB_DSC->mDSC)))
	{
		printf("\n[%s][%d]err_msg[%s]\n",__FILE__,__LINE__, msg);
		strcpy(err_msg,msg);
		return DB_FAIL;
	}
#else
#endif
	
	DB_DSC->mpDSC = &(DB_DSC->mDSC);
	memset(&(DB_DSC->db_sqls), 0, sizeof(DB_DSC->db_sqls));
	memset(&(DB_DSC->p_sql), 0, sizeof(DB_DSC->p_sql));

	//设为缓冲模式
#ifdef _DSCI_
	DSCI_set_Buffer_flag (DB_DSC->mpDSC, 0);
	
	DSCI_set_auto_commit (DB_DSC->mpDSC, set->autoCommit);
#else
#endif
	
#ifdef _DSCI_
	int looper = 0;
	
#else
	
#endif
	
	return 0;
}

//数据库断开
void ibear_db_quit (IBEAR_DbConner *DB_DSC)
{
	if (DB_DSC->mpDSC != NULL)
	{
#ifdef _DSCI_
		DSCI_quit (DB_DSC->mpDSC);
#else
#endif
	}
	
	DB_DSC->mpDSC = NULL;
}

//绑定变量
int ibear_bindParam(map<string,stSQLParam*> bindParams,stSQLParam *para_list[])
{
	//对入参进行排序
	stSQLParam *para_list_tmp[MAX_PARAM_NUM];
	int userd_num = 0;
	for(map<string,stSQLParam*>::iterator ii = bindParams.begin(); ii != bindParams.end(); ii++)
	{
		stSQLParam *param = ii->second;
		if (param->name_type == 1 || param->name_type == 4)
			continue;
		para_list_tmp[param->num] = param;
		userd_num++;
	}
	int j = 0;
	for (int i = 0; i < userd_num; i++)
	{
		stSQLParam *param = para_list_tmp[i];
		if (param->name_type == 2)// || param->value == NULL || strcmp(param->value,"") == 0)
			continue;
		para_list[j] = param;
		j++;
	}
	
	return j;
}

//参数初始化
int initConf(map<string,stSqlMap*> *m_Sqlconfs,map<string,void*> inParams,char *sql_out,stSqlMap **sqlmap,int routeNo)
{
	char sql_key[1024] = {0};
	if (strcmp((*sqlmap)->sql,"") != 0)
	{
		ibear_conversion((*sqlmap)->sql,sql_key);
	}
	else
	{
		strcpy(sql_key,(*sqlmap)->id);
	}
	//根据key找到相应的sql及相关信息
	map<string,stSqlMap*>::iterator dataIter = (*m_Sqlconfs).find(string(sql_key));
	if (dataIter != (*m_Sqlconfs).end())
	{
		delete *sqlmap;
		*sqlmap = NULL;
		*sqlmap = dataIter->second;
	}
	else
	{
		if (strcmp((*sqlmap)->sql,"") != 0)
		{
	#ifdef _DEBUG_INFO_
			printf("首次\n");
	#endif
			char new_sql[1024] = {0};
			if (0 != xmlbuild_sql((*sqlmap)->sql,new_sql,&((*sqlmap)->inParams)))
			{
				printf("xmlbuild_sql is ERROR[%s]\n",(*sqlmap)->sql);
				return -1;
			}
			strcpy((*sqlmap)->new_sql,new_sql);
			(*m_Sqlconfs).insert(pair<string,stSqlMap*>(sql_key,(*sqlmap)));
		}
		else
		{
			delete *sqlmap;
			*sqlmap = NULL;
			printf("sql_key %s 不存在\n",sql_key);
			return -2;
		}
	}
	char *ret_code = NULL;
	//给sql入参进行赋值
	try{
		ret_code = ibear_getSQLParam(*sqlmap,inParams,(*sqlmap)->inParams,sql_out);
	} catch(char * e){
		inParams.clear();
		printf("[%s]error:%s\n",ret_code,e);
		free(ret_code);
		return -3;
	}
	inParams.clear();
	free(ret_code);
	
	//自动检测sql中列数
	if (strcmp((*sqlmap)->res_num,"0") == 0 || strcmp((*sqlmap)->res_num,"") == 0)
	{
		sprintf((*sqlmap)->res_num ,"%d", ibear_SQLResultNum(sql_out));
	}
	//自动路由判断
	if (strcmp((*sqlmap)->db_no_real,"?") == 0)
	{
		sprintf((*sqlmap)->db_no,"%d",routeNo);
	}
	//多库查询判断
	if (strcmp((*sqlmap)->db_no_real,"ALL") == 0)
	{
		//sprintf((*sqlmap)->db_no,"%d",routeNo);
	}
	return 0;
}
