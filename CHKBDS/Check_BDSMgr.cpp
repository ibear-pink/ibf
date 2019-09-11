
/*
 * 功能描述:一致性比对、修复
 * 创建时间:
 * 编码人员:自动生成
*/

#include "Check_BDSMgr.h"
#include "CDealData.h"

IMPLEMENT_CLASS(Check_BDSMgr)

Check_BDSMgr::Check_BDSMgr()
{
	memset(m_BusinessType, 0, sizeof(m_BusinessType));
}

Check_BDSMgr::~Check_BDSMgr()
{
	
}

int Check_BDSMgr::init(ibear *br, char *logName,int thread_num,map<string,icfg_Map*> user_defined)
{
	int iRet = 0;
	printf("Check_BDSMgr->init(%s|%d)\n",logName,thread_num);
	m_LogName = string(logName);
	m_ThreadNum = thread_num;
	m_Ibear = br;
	map<string,icfg_Map*>::iterator it = user_defined.find(string("business_type"));
	if (it!=user_defined.end())
	{
		strcpy(m_BusinessType,it->second->chValue);
	}
	
	//读取配置表
	if (0 != getCheckCfg())
	{
		return RET_FAIL;
	}
	
	if (0 != getCheckTableMap())
	{
		return RET_FAIL;
	}
	
	if (0 != getControlInfo())
	{
		return RET_FAIL;
	}
	
	return iRet;
}

int Check_BDSMgr::SplitStringBySP (const char *buff, char split_char, string key[])
{
	int size = 0,j = 0;
	
	char tmpBuf[1024];
	char tStr[1024];
	
	memset (tmpBuf, 0, sizeof(tmpBuf));
	
	strcpy (tmpBuf, buff);
	
	int len = strlen(tmpBuf);
	if (tmpBuf[len-1] != split_char && tmpBuf[len-1] != '\n')
	{
		tmpBuf[len] = split_char;
		len = len+1;
	}
	int i = 0, end = 0, begin = 0;

	while (tmpBuf[i] != '\0' && i <= len)
	{
		if (tmpBuf[i] == split_char)
		{
			end = i;
			i++;
		}
		else
		{
			i++;
			continue;
		}
		
		memset (tStr, 0, sizeof(tStr));
		size++;
		
		memcpy (tStr, tmpBuf+begin, end - begin);
		tStr[end-begin] = '\0';
		key[j] = string(tStr);
		j ++;
		begin = i;
	}
	
	return j;
}

int Check_BDSMgr::getRepairCount(char *check_table,char *count)
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<string> v_row;
	inParams.insert(pair<string,string>("CHECK_TABLE",check_table));
	iRet = m_Ibear->queryForObject("SEL_COUNT",inParams,&v_row);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_COUNT is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	
	strcpy(count,v_row[0].c_str());
  return iRet;
}

int Check_BDSMgr::getRepairCountHis(char *check_table,char *ym,char *count)
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<string> v_row;
	inParams.insert(pair<string,string>("CHECK_TABLE",check_table));
	inParams.insert(pair<string,string>("YM",ym));
	iRet = m_Ibear->queryForObject("SEL_COUNT_HIS",inParams,&v_row);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_COUNT_HIS is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	
	strcpy(count,v_row[0].c_str());
  return iRet;
}

int Check_BDSMgr::getCheckCfg()
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<vector<string> > list;
	iRet = m_Ibear->queryForList("SEL_REL_CFG",inParams,&list);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_REL_CFG is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> v_row = *it;
		stKeyRelas *rela = new stKeyRelas();
		strcpy(rela->table_name,v_row[0].c_str());
		strcpy(rela->key_id,v_row[1].c_str());
		strcpy(rela->dbno_key,v_row[2].c_str());
		strcpy(rela->prkey_id,v_row[3].c_str());
		strcpy(rela->dbno_prkeys,v_row[4].c_str());
		m_KeyTableRelas.insert(pair<string,stKeyRelas*>(v_row[0],rela));
	}
  return iRet;
}

int Check_BDSMgr::getCheckTableMap()
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<vector<string> > list;
	iRet = m_Ibear->queryForList("SEL_TAB_CFG",inParams,&list);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_TAB_CFG is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> v_row = *it;
		string bds_tableName;//BDS 表名
		string bds_sqlname;//BDS sql
		string add_sqlname;//add_sql
		string bds_keys;//BDS key
		char check_sqlname[30] = {0};//物理库sql
		char check_key[64] = {0};//物理库keys
		bds_tableName = v_row[0];
		bds_sqlname = v_row[1];
		bds_keys = v_row[2];
		strcpy(check_sqlname,v_row[3].c_str());
		strcpy(check_key,v_row[4].c_str());
		add_sqlname = v_row[5];

		//获取内存库对应的物理库表（可能涉及多个物理库表）
		map<string,stCheckMap*>::iterator tmp = m_CheckTableMaps.find(bds_tableName);
		if (tmp != m_CheckTableMaps.end())
		{
			stCheckMap *checkmap = tmp->second;
			int flag = 0;
			for (vector<stTableKey*>::iterator it=(checkmap->checks).begin();it!=(checkmap->checks).end();it++)
			{
				stTableKey *tk = *it;
				string name = tk->check_sqlname;
				if (strcmp(name.c_str() ,check_sqlname) == 0)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 0)
			{
				stTableKey *tk = new stTableKey();
				strcpy(tk->check_sqlname,check_sqlname);
				strcpy(tk->check_keys,check_key);
				(checkmap->checks).push_back(tk);
			}
		}
		else 
		{
			stCheckMap *checkmap = new stCheckMap();
			vector<stTableKey*> tablelist;
			stTableKey *tk = new stTableKey();
			strcpy(tk->check_sqlname,check_sqlname);
			strcpy(tk->check_keys,check_key);
			tablelist.push_back(tk);
			strcpy(checkmap->bds_sqlname,bds_sqlname.c_str());
			strcpy(checkmap->add_sqlname,add_sqlname.c_str());
			strcpy(checkmap->bds_keys,bds_keys.c_str());
			checkmap->checks = tablelist;
			m_CheckTableMaps.insert(pair<string,stCheckMap*>(bds_tableName,checkmap));
		}
	}

	return iRet;
}

int Check_BDSMgr::getControlInfo()
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<string> v_row;
	iRet = m_Ibear->queryForObject("SEL_CON_INFO",inParams,&v_row);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_CON_INFO is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	
	m_Controls = new stControls();

	if (v_row.size() > 0)
	{
		m_Controls->qrydb_nums = atoi(v_row[0].c_str());
		m_Controls->check_nums = atoi(v_row[1].c_str());
		m_Controls->repair_max = atoi(v_row[2].c_str());
		strcpy(m_Controls->ctrl_flag,v_row[3].c_str());
	}
	else 
	{
		m_Controls->qrydb_nums = 0;
		m_Controls->check_nums = 3;
		m_Controls->repair_max = 0;
		strcpy(m_Controls->ctrl_flag,"0");
	}
  return iRet;
}

int Check_BDSMgr::getInterfaceDatas()
{
	int iRet = RET_OK;
	map<string,string> inParams;
	vector<vector<string> > list;
	iRet = m_Ibear->queryForList("SEL_INTER",inParams,&list);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_INTER is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		vector<string> v_row = *it;
		stCheckData *checkdata = new stCheckData();
		strcpy(checkdata->table_name,v_row[0].c_str());
		strcpy(checkdata->key_id,v_row[1].c_str());
		strcpy(checkdata->data_type,v_row[2].c_str());
		m_CheckDatas.push_back(checkdata);
	}
  return iRet;
}

/*根据用户获得用户级数据（可能多条）
 *入参:
 *table_name:内存库表名
 *sqlname:sql_key_id(xml中的id)
 *bds_keys:主键在select中的位置,从0开始,多个用$分开,如:1$3
 *key_value:唯一主键值
 *db_type:1->mdb,2->db
*/
int Check_BDSMgr::getDataBySigKeyValue(char *table_name,char *sqlname,char *sqlkeys,char *key_value,map<string,string> *db_datas,const int db_type)
{
	int iRet = RET_OK;
	long num = 0,knum = 0;
	char dbnum_type[1+1] = {0};
	int db_num = 0;
	char c_id1[1+1] = {0};
	char c_id2[2+1] = {0};
	string keys[20];
	map<string,string> datas_temp;
	map<string,string> inParams;
	vector<vector<string> > list;
	stKeyRelas *rela;
	map<string,stKeyRelas*>::iterator it = m_KeyTableRelas.find(string(table_name));
	if (it != m_KeyTableRelas.end())
	{
		rela = it->second;
	}
	else 
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"m_KeyTableRelas is end :%s",table_name);
		return RET_FAIL;
	}
	strncpy(c_id1,key_value + strlen(key_value)-1,1);
	strncpy(c_id2,key_value + strlen(key_value)-2,2);
	
	inParams.insert(pair<string,string>("ID1",string(c_id1)));
	inParams.insert(pair<string,string>("ID2",string(c_id2)));
	inParams.insert(pair<string,string>(rela->key_id,string(key_value)));
	
	iRet = m_Ibear->queryForList(sqlname,inParams,&list);
	if (iRet != 0)
	{
		if (db_type == 1 && m_Ibear->error_code > 0)
		{
			exit(0);
		}
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SQLNAME:%s[%s] is ERROR,ERRMSG:%s",m_Ibear->no_error,sqlname,key_value,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	
	knum = SplitStringBySP(sqlkeys,'$',keys);
	for (vector<vector<string> >::iterator it = list.begin();it!=list.end();it++)
	{
		char key[1024] = {0};
		char note[1024] = {0};
		vector<string> v_row = *it;
		num = v_row.size();
		for(int i=0;i<num;i++)
		{
			char note_temp[1024] = {0};
			strcpy(note_temp,v_row[i].c_str());
			sprintf(note,"%s%s,",note,Trim(note_temp));
			for (int j = 0; j < knum; j++)
			{
				if(i == atoi(keys[j].c_str()))
				{
					if (strcmp(key,"") == 0)
					{
						strcpy(key,v_row[i].c_str());
					}
					else 
					{
						sprintf(key,"%s$%s",key,v_row[i].c_str());
					}
					break;
				}
			}
		}
		datas_temp.insert(pair<string,string>(string(key),string(note)));
	}
	*db_datas = datas_temp;
	return iRet;
}

/*根据用户获得用户级数据（可能多条）
 *入参:
 *sqlname:sql_key_id(xml中的id)
 *kvs:存sql中${KEY|char}以及该key的值
 *db_datas:主键在select中的位置,从0开始,多个用$分开,如:1$3
 *db_type:1->mdb,2->db
 *dbno_key:判断AB库的标示
*/
int Check_BDSMgr::getDataByAllKeyValue(char *sqlname,vector<stKeyValues*> kvs,map<string,string> *db_datas,const int db_type,char *dbno_key)
{
	int iRet = RET_OK;
	char dbnum_type[1+1] = {0};
	int db_num = 0;
	char key_value[256] = {0};
	char note[1024] = {0};
	char key[1024] = {0};
	map<string,string> datas_temp;
	map<string,string> inParams;
	vector<string> v_row;
	for (vector<stKeyValues*>::iterator it=kvs.begin();it!=kvs.end();it++)
	{
		stKeyValues* kv = *it;
		if (strcasecmp(kv->key,dbno_key) == 0)
		{
			strcpy(key_value,kv->value);
		}
		if (strcasecmp(kv->key,"ID_NO") == 0)
		{
			char c_id1[1+1] = {0};
			char c_id2[2+1] = {0};
			strncpy(c_id1,key_value + strlen(key_value)-1,1);
			strncpy(c_id2,kv->value+strlen(kv->value)-2,2);
			inParams.insert(pair<string,string>("ID1",string(c_id1)));
			inParams.insert(pair<string,string>("ID2",string(c_id2)));
		}
#ifdef _DEBUG_INFO_
	printf("kv-key=%s,kv->value=%s\n",kv->key,kv->value);
#endif
		inParams.insert(pair<string,string>(kv->key,kv->value));
		if (it==kvs.end()-1)
		{
			sprintf(key,"%s%s",key,kv->value);
		}
		else 
		{
			sprintf(key,"%s%s$",key,kv->value);
		}
	}
	
	iRet = m_Ibear->queryForObject(sqlname,inParams,&v_row);
	if (iRet != 0)
	{
		if (db_type == 1 && m_Ibear->error_code > 0)
		{
			exit(0);
		}
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SQLNAME:%s is ERROR,ERRMSG:%s",m_Ibear->no_error,sqlname,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	

	char note_temp[1024] = {0};
	for(int i = 0; i < v_row.size(); i++)
	{
		strcpy(note_temp,v_row[i].c_str());
		sprintf(note,"%s%s,", note, Trim(note_temp));
	}
	
	if (v_row.size() > 0)
	{
		datas_temp.insert(pair<string,string>(string(key),string(note)));
		
		*db_datas = datas_temp;
	}
	
	return iRet;
}

int Check_BDSMgr::countCompareLog(char *key_value,char *table_name,int *count)
{
	int iRet = 0;
	int temp_count = 0;
	vector<string> v_row;
	map<string,string> inParams;
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	iRet = m_Ibear->queryForObject("SEL_COM_COUNT",inParams,&v_row);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_COM_COUNT is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	for (vector<string>::iterator it=v_row.begin();it!=v_row.end();it++)
	{
		temp_count = atoi((*it).c_str());
	}
	*count = temp_count;
	return iRet;
}

int Check_BDSMgr::deleteInterface(char *key_value,char *table_name)
{
	int iRet = 0;
	map<string,string> inParams;
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	iRet = m_Ibear->DML("DEL_INTER",inParams);
	if (iRet != 0 && iRet != 1)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|DEL_INTER is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	return 0;
}

int Check_BDSMgr::deletedateCompareLog(char *key_value,char *table_name)
{
	int iRet = 0;
	map<string,string> inParams;
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	iRet = m_Ibear->DML("DEL_COM",inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|DEL_COM is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	return iRet;
}

int Check_BDSMgr::insertCompareLog(char *key_value,char *table_name,char *check_type,char *data_type)
{
	int iRet = 0;
	char check_sn[20+1] = {0};
	sprintf(check_sn,"%s",CurrSysDate(16));
	map<string,string> inParams;
	inParams.insert(pair<string,string>("CHECK_SN",check_sn));
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	inParams.insert(pair<string,string>("CHECK_TYPE",check_type));
	inParams.insert(pair<string,string>("DATA_TYPE",data_type));
	inParams.insert(pair<string,string>("FILE_TYPE",check_type));
	iRet = m_Ibear->DML("INS_COM",inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|INS_COM is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
	}
	return iRet;
}

int Check_BDSMgr::updateCompareLog(char *key_value,char *table_name,char *check_type)
{
	int iRet = 0;
	map<string,string> inParams;
	inParams.insert(pair<string,string>("CHECK_TYPE",check_type));
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	iRet = m_Ibear->DML("UPD_COM",inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|UPD_COM is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	return iRet;
}

int Check_BDSMgr::selectCompareLog()
{
	int iRet = 0;
	int temp_count = 0;
	char c_num[2] = {0};
	vector<vector<string> > list;
	map<string,string> inParams;
	iRet = m_Ibear->queryForList("SEL_COM",inParams,&list);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|SEL_COM is ERROR,ERRMSG:%s",m_Ibear->no_error,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	
	for (vector<vector<string> >::iterator it=list.begin();it!=list.end();it++)
	{
		vector<string> v_row = *it;
		stCheckData *checkdata = new stCheckData();
		strcpy(checkdata->check_sn,v_row[0].c_str());
		strcpy(checkdata->table_name,v_row[1].c_str());
		strcpy(checkdata->key_id,v_row[2].c_str());
		checkdata->times = atoi(v_row[3].c_str());
		strcpy(checkdata->check_type,v_row[4].c_str());
		strcpy(checkdata->data_type,v_row[5].c_str());
		strcpy(checkdata->file_type,v_row[6].c_str());
		
		m_CheckDatas.push_back(checkdata);
	}
	
	return iRet;
}

int Check_BDSMgr::insertAddLog(char *table_name,int num,string values[],char *dml_type,vector<stKeyValues*> kvs,char *dbno_key)
{
	int iRet = 0;
	char key_value[256] = {0};
	char region_code[2+1] = {0};
	char dbnum_type[1+1] = {0};
	int db_num = 3;
	map<string,string> inParams;
	for (int i=0;i<num;i++)
	{
		char key[10+1] = {0};
		sprintf(key,"VALUE%d",i);
		inParams.insert(pair<string,string>(key,values[i]));
	}
	inParams.insert(pair<string,string>("DML_TYPE",dml_type));
	
	iRet = m_Ibear->DML(table_name,inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|DML_TYPE[%s] is ERROR,ERRMSG:%s ",m_Ibear->no_error,table_name,m_Ibear->sql_error);
		iRet = RET_FAIL;
		return iRet;
	}
	
	return iRet;
}

int Check_BDSMgr::insertRepairLog(char *check_sn,char *table_name,char *key_value,char *check_type,char *check_note,char *is_deal,char *data_type,char *file_type)
{
	int iRet = 0;
	char check_result[1+1] = {0};
	if (strcmp(check_type,"4") == 0)
	{
		strcpy(check_result,"N");
		strcpy(check_note,"0");
	}
	else
	{
		strcpy(check_result,"Y");
	}
	if (strcmp(file_type,"") == 0)
	{
		strcpy(file_type,check_type);
	}
	map<string,string> inParams;
	inParams.insert(pair<string,string>("CHECK_SN",check_sn));
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	inParams.insert(pair<string,string>("CHECK_TYPE",check_type));
	inParams.insert(pair<string,string>("CHECK_RESULT",check_result));
	inParams.insert(pair<string,string>("CHECK_NOTE",check_note));
	inParams.insert(pair<string,string>("IS_DEAL",is_deal));
	inParams.insert(pair<string,string>("DATA_TYPE",data_type));
	inParams.insert(pair<string,string>("FILE_TYPE",file_type));
	iRet = m_Ibear->DML("INS_REP",inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|INS_REP is ERROR[%s|%s|%s|%s|%s|%s|%s|%s] ERRMSG:%s ",m_Ibear->no_error,check_sn,key_value,table_name,check_type,check_result,check_note,is_deal,data_type,m_Ibear->sql_error);
    iRet = RET_FAIL;
		return iRet;
	}
	return iRet;
}

int Check_BDSMgr::insertCompareLogErr(char *key_value,char *table_name,char *ErrMsg)
{
	int iRet = 0;
	map<string,string> inParams;
	inParams.insert(pair<string,string>("KEY_VALUE",key_value));
	inParams.insert(pair<string,string>("CHECK_TABLE",table_name));
	inParams.insert(pair<string,string>("ERR_MSG",ErrMsg));
	iRet = m_Ibear->DML("INS_ERR_COM",inParams);
	if (iRet != 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"%s|INS_ERR_COM is ERROR,ERRMSG:%s ",m_Ibear->no_error,m_Ibear->sql_error);
    iRet = RET_FAIL;
	}
	return iRet;
}

//根据keyid获取比对数据集(用户级->记录级)
int Check_BDSMgr::getCheckDataUser(char *table_name,char *key_value,map<string,string> *bds_datas,map<string,string> *db_datas)
{
	int iRet = 0;
	char sql_name[32+1] = {0};
	map<string,string> bds_datas_temp;
	map<string,string> db_datas_real;
	
	stCheckMap *checkmap = NULL;
	map<string,stCheckMap*>::iterator itable = m_CheckTableMaps.find(string(table_name));
	if (itable == m_CheckTableMaps.end())
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"无对应sqlname[%s]",table_name);
		printf("无对应sqlname[%s]\n",table_name);
		return RET_FAIL;
	}
	checkmap = itable->second;
	iRet = getDataBySigKeyValue(table_name,checkmap->bds_sqlname,checkmap->bds_keys,key_value,&bds_datas_temp,1);
	if (iRet != 0)
	{
		return iRet;
	}
	
	for (vector<stTableKey*>::iterator itch = (checkmap->checks).begin();itch!= (checkmap->checks).end();itch++)
	{
		stTableKey *tk = *itch;
		map<string,string> db_datas_temp;
		iRet = getDataBySigKeyValue(table_name,tk->check_sqlname,tk->check_keys,key_value,&db_datas_temp,2);
		if (iRet != 0)
		{
			return iRet;
		}
		if ((checkmap->checks).size() == 1)
		{
			*db_datas = db_datas_temp;
			break;
		}
		for (map<string,string>::iterator mp = db_datas_temp.begin();mp!= db_datas_temp.end();mp++)
		{
			db_datas_real.insert(pair<string,string>(mp->first,mp->second));
		}
		db_datas_temp.clear();
	}
	
	*bds_datas = bds_datas_temp;
	if ((checkmap->checks).size() > 1)
	{
		*db_datas = db_datas_real;
	}
	return iRet;
}

//根据keyid获取比对数据集（记录级->记录级）
int Check_BDSMgr::getCheckDataRecord(char *table_name,vector<stKeyValues*> kvs,map<string,string> *bds_datas,map<string,string> *db_datas)
{
	int iRet = 0;
	char sql_name[32+1] = {0};
	stKeyRelas *rela;
	map<string,string> bds_datas_temp;
	map<string,string> db_datas_real;
	
	map<string,stKeyRelas*>::iterator it = m_KeyTableRelas.find(string(table_name));
	if (it!=m_KeyTableRelas.end())
	{
		rela = it->second;
	}
	
	stCheckMap *checkmap = NULL;
	map<string,stCheckMap*>::iterator itable = m_CheckTableMaps.find(string(table_name));
	if (itable == m_CheckTableMaps.end())
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"无对应sqlname[%s]",table_name);
		printf("无对应sqlname[%s]\n",table_name);
		return RET_FAIL;
	}
	checkmap = itable->second;
	iRet = getDataByAllKeyValue(checkmap->bds_sqlname,kvs,&bds_datas_temp,1,rela->dbno_prkeys);
	if (iRet != 0)
	{
		return iRet;
	}
	
	for (vector<stTableKey*>::iterator itch = (checkmap->checks).begin();itch!= (checkmap->checks).end();itch++)
	{
		stTableKey *tk = *itch;
		map<string,string> db_datas_temp;
		iRet = getDataByAllKeyValue(tk->check_sqlname,kvs,&db_datas_temp,2,rela->dbno_prkeys);
		if (iRet != 0)
		{
			return iRet;
		}
		
		if ((checkmap->checks).size() == 1)
		{
			*db_datas = db_datas_temp;
			break;
		}
		for (map<string,string>::iterator mp = db_datas_temp.begin();mp!= db_datas_temp.end();mp++)
		{
			db_datas_real.insert(pair<string,string>(mp->first,mp->second));
		}
		db_datas_temp.clear();
	}
	
	*bds_datas = bds_datas_temp;
	if ((checkmap->checks).size() > 1)
	{
		*db_datas = db_datas_real;
	}
	else 
	{
		db_datas_real.clear();
	}
	return iRet;
}

//数据比对及记录操作
int Check_BDSMgr::CheckDataByList(stCheckData *checkdata,map<string,string> bds_datas,map<string,string> db_datas,char *check_type)
{
	int iRet = 0;
	char check_note[255+1] = {0};
	strcpy(check_note,"0");
	
	//物理库、内存库均无数据；
	if (bds_datas.size() == 0 && db_datas.size() == 0)
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"物理库、内存库均无数据");
		strcpy(check_type,"0");
		
		iRet = ProDealData(checkdata->key_id,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,check_type,2,checkdata->file_type);
		if(iRet != 0)
		{
			return RET_FAIL;
		}
		return iRet;
	}
	
	//比较内容（以bds为参考）
	for(map<string,string>::iterator it = bds_datas.begin();it!=bds_datas.end();it++)
	{
		string key = it->first;
		string note = it->second;
		map<string,string>::iterator ct=db_datas.find(key);
		char c_key[128+1] = {0};
		strcpy(c_key,key.c_str());
		//数据存在
		if (ct != db_datas.end())
		{
			string dnote = ct->second;
			//相同
		#ifdef _DEBUG_INFO_
			printf("[%s]\n[%s]\n",note.c_str(),dnote.c_str());
		#endif
			if (note == dnote)
			{
				strcpy(check_type,"4");
				iRet = ProDealData(c_key,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,check_type,2,checkdata->file_type);
				if(iRet != 0)
				{
					return RET_FAIL;
				}
			}
			//不同
			else 
			{
				strcpy(check_type,"3");
				iRet = ProDealData(c_key,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,check_type,1,checkdata->file_type);
				if(iRet != 0)
				{
					return RET_FAIL;
				}
			}
		}
		//不存在,bds有,物理库无
		else 
		{
			strcpy(check_type,"2");
			iRet = ProDealData(c_key,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,check_type,1,checkdata->file_type);
			if(iRet != 0)
			{
				return RET_FAIL;
			}
		}
	}

#ifdef _DEBUG_INFO_
	printf("%ld\n",db_datas.size());
#endif
	//比较内容（以物理库为参考）
	for(map<string,string>::iterator it=db_datas.begin();it!=db_datas.end();it++)
	{
		string key = it->first;
		string note = it->second;
		char c_key[128+1] = {0};
		strcpy(c_key,key.c_str());
#ifdef _DEBUG_INFO_
	printf("%ld,%s\n",bds_datas.size(),c_key);
#endif
		map<string,string>::iterator ct = bds_datas.find(key);
		//不存在,物理库有,bds无
		if (ct == bds_datas.end())
		{
			strcpy(check_type,"1");
			iRet = ProDealData(c_key,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,check_type,1,checkdata->file_type);
			if(iRet != 0)
			{
				return RET_FAIL;
			}
		}
		else 
		{
			string dnote = ct->first;
		#ifdef _DEBUG_INFO_
			printf("key[%s]物理库存在[%s]!\n",checkdata->key_id,dnote.c_str());
		#endif
		}
	}
	return iRet;
}

/*
* ProDealData
* 入参：deal_type:
* 1:数据不相同处理方式
* 2:数据相同处理方式
* 3:数据修复处理方式
* 4:数据未修复处理方式(超出修复次数)
*/
int Check_BDSMgr::ProDealData(char *key_id,char *table_name,char *data_type,char *check_sn,char *check_note,char *check_type,int deal_type,char *file_type)
{
	int iRet = 0;
	char is_deal[1+1] = {0};
#ifdef _DEBUG_INFO_
	printf("deal_type::%d\n",deal_type);
#endif
	WriteLog(m_LogName,LOG_SYS_RUNNING,"[%d](key|%s,table_name|%s,data_type|%s),note|%s,check_type|%s",deal_type,key_id,table_name,data_type,check_note,check_type);
	if (deal_type == 1)
	{
		int count = 0;
		iRet = countCompareLog(key_id,table_name,&count);
		if(iRet != 0)
		{
			return iRet;
		}
		if (count == 0)
		{
			iRet = insertCompareLog(key_id,table_name,check_type,data_type);
			if(iRet != 0)
			{
				return iRet;
			}
		}
		else 
		{
			iRet = updateCompareLog(key_id,table_name,check_type);
			if(iRet != 0)
			{
				return iRet;
			}
		}
		return RET_OK;
	}
	else if(deal_type == 2)
	{
		strcpy(is_deal,"0");
	}
	else if(deal_type == 3)
	{
		strcpy(is_deal,"1");
	}
	else if(deal_type == 4)
	{
		strcpy(is_deal,"0");
	}
	int count = 0;
	iRet = countCompareLog(key_id,table_name,&count);
	if(iRet != 0)
	{
		return iRet;
	}
	if (count > 0)
	{
		iRet = deletedateCompareLog(key_id,table_name);
		if(iRet != 0)
		{
			return iRet;
		}
	}
	if (strcmp(check_sn,"") == 0)
		strcpy(check_sn,"0");
	iRet = insertRepairLog(check_sn,table_name,key_id,check_type,check_note,is_deal,data_type,file_type);
	if(iRet != 0)
	{
		return iRet;
	}
	
	return iRet;
}
//数据修复
int Check_BDSMgr::ProRepairData(stCheckData *checkdata)
{
	int iRet = 0;
	char add_sqlname[32+1] = {0};
	char bds_sqlname[32+1] = {0};
	char dml_type[1+1] = {0};
	char check_note[1024] = {0};
	
	//获得key-value对应
	map<string,stKeyRelas*>::iterator it = m_KeyTableRelas.find(checkdata->table_name);
	if (it==m_KeyTableRelas.end())
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"m_KeyTableRelas is end");
		return RET_FAIL;
	}
	stKeyRelas *kr = it->second;
	vector<stKeyValues*> kvs;
	string keys[20];
	string values[20];
	int keynum = SplitStringBySP(kr->prkey_id,'$',keys);
	int valuenum = SplitStringBySP(checkdata->key_id,'$',values);
	for (int i = 0;i < valuenum;i++)
	{
		stKeyValues *kv = new stKeyValues();
		strcpy(kv->key,keys[i].c_str());
		strcpy(kv->value,values[i].c_str());
		kvs.push_back(kv);
	}
	
	//获取add表sqlid,bds查询sqlid
	map<string,string> db_datas;
	map<string,string> bds_datas;
	map<string,stCheckMap*>::iterator tmp = m_CheckTableMaps.find(string(checkdata->table_name));
	if (tmp != m_CheckTableMaps.end())
	{
		strcpy(add_sqlname,(tmp->second)->add_sqlname);
		iRet = getDataByAllKeyValue((tmp->second)->bds_sqlname,kvs,&bds_datas,1,kr->dbno_prkeys);
		if(iRet != 0)
		{
			if (kvs.size()>0)
				clearDataList(kvs);
			if (db_datas.size()>0)
				db_datas.clear();
			if (bds_datas.size()>0)
				bds_datas.clear();
			return iRet;
		}
		
		for (vector<stTableKey*>::iterator it = ((tmp->second)->checks).begin();it!=((tmp->second)->checks).end();it++)
		{
			stTableKey *tk = *it;
			iRet = getDataByAllKeyValue(tk->check_sqlname,kvs,&db_datas,2,kr->dbno_prkeys);
			if(iRet != 0)
			{
				if (kvs.size()>0)
					clearDataList(kvs);
				if (db_datas.size()>0)
					db_datas.clear();
				if (bds_datas.size()>0)
					bds_datas.clear();
				return iRet;
			}
		}
	}
	
	int insert_num = 0;
	string insert_values[30];
	//1物理库有，内存库没有
	if (strcmp(checkdata->check_type,"1") == 0)
	{
		strcpy(dml_type,"I");
		for(map<string,string>::iterator it = db_datas.begin();it!=db_datas.end();it++)
		{
			strcpy(check_note,(it->second).c_str());
		}
	}
	//2内存库有，物理库没有
	else if (strcmp(checkdata->check_type,"2") == 0)
	{
		strcpy(dml_type,"D");
		for(map<string,string>::iterator it = bds_datas.begin();it!=bds_datas.end();it++)
		{
			strcpy(check_note,(it->second).c_str());
		}
	}
	//3数据不一致
	else if (strcmp(checkdata->check_type,"3") == 0)
	{
		strcpy(dml_type,"U");
		for(map<string,string>::iterator it = db_datas.begin();it!=db_datas.end();it++)
		{
			strcpy(check_note,(it->second).c_str());
		}
	}
	insert_num = SplitStringBySP(check_note,',',insert_values);
#ifdef _DEBUG_INFO_
	printf("check_note=%s\n",check_note);
	printf("insert_num=%d|add_sqlname=%s\n",insert_num,add_sqlname);
#endif
	int deal_type = 0;
	
	//区分update，delete，insert操作
	iRet = insertAddLog(add_sqlname,insert_num,insert_values,dml_type,kvs,kr->dbno_prkeys);
	if(iRet != 0)
	{
		if (kvs.size()>0)
			clearDataList(kvs);
		if (db_datas.size()>0)
			db_datas.clear();
		if (bds_datas.size()>0)
			bds_datas.clear();
		return RET_FAIL;
	}
	deal_type = 3;
	
	iRet = ProDealData(checkdata->key_id,checkdata->table_name,checkdata->data_type,checkdata->check_sn,check_note,checkdata->check_type,deal_type,checkdata->file_type);
	if(iRet != 0)
	{
		iRet = RET_FAIL;
	}
	if (kvs.size()>0)
		clearDataList(kvs);
	if (db_datas.size()>0)
		db_datas.clear();
	if (bds_datas.size()>0)
		bds_datas.clear();
	return iRet;
}

//针对差异数据比对(用户级->记录级)
int Check_BDSMgr::ProCheckDataUserToRecord(stCheckData *checkdata)
{
	int iRet = 0;
	//进行数据解析，根据表名查询物理库和内存库数据
#ifdef _DEBUG_INFO_
	printf("用户级针对差异数据比对->table_name[%s]\n",checkdata->table_name);
#endif
	map<string,string> bds_datas;
	map<string,string> db_datas;
	iRet = getCheckDataUser(checkdata->table_name,checkdata->key_id,&bds_datas,&db_datas);
	if(iRet != 0)
	{
		return iRet;
	}
#ifdef _DEBUG_INFO_
	printf("数据比对,并记录CompairLog[%s]\n",checkdata->table_name);
#endif
	//数据比对,并记录CompairLog
	char check_type[1+1] = {0};
	iRet = CheckDataByList(checkdata,bds_datas,db_datas,check_type);
	if(iRet != 0)
	{
		return iRet;
	}
	if (bds_datas.size()>0)
		bds_datas.clear();
	if (db_datas.size()>0)
		db_datas.clear();
	return iRet;
}

//针对差异数据比对(记录级->记录级)
int Check_BDSMgr::ProCheckDataRecord(stCheckData *checkdata,char *check_type)
{
	int iRet = 0;
	//进行数据解析，根据表名查询物理库和内存库数据
#ifdef _DEBUG_INFO_
	long a = 0,b=0,c=0,d=0;
	a = getUTime();
	printf("记录级针对差异数据比对->table_name[%s]\n",checkdata->table_name);
#endif
	map<string,string> bds_datas;
	map<string,string> db_datas;
	vector<stKeyValues*> kvs;
	
	map<string,stKeyRelas*>::iterator it = m_KeyTableRelas.find(checkdata->table_name);
	if (it==m_KeyTableRelas.end())
	{
		WriteLog(m_LogName,LOG_SYS_ERROR,"m_KeyTableRelas is end");
		return RET_FAIL;
	}
	stKeyRelas *kr = it->second;
	string keys[20];
	string values[20];
	int keynum = SplitStringBySP(kr->prkey_id,'$',keys);
	int valuenum = SplitStringBySP(checkdata->key_id,'$',values);
	for (int i = 0;i < valuenum;i++)
	{
		stKeyValues *kv = new stKeyValues();
		strcpy(kv->key,keys[i].c_str());
		strcpy(kv->value,values[i].c_str());
#ifdef _DEBUG_INFO_
	printf("kv[%s][%s]\n",kv->key,kv->value);
#endif
		kvs.push_back(kv);
	}
#ifdef _DEBUG_INFO_
	b=getUTime();
	printf("_ProCheckDataRecord-getCheckDataRecord\n");
#endif
	iRet = getCheckDataRecord(checkdata->table_name,kvs,&bds_datas,&db_datas);
	if(iRet != 0)
	{
		if (kvs.size()>0)
			clearDataList(kvs);
		if (db_datas.size()>0)
			db_datas.clear();
		if (bds_datas.size()>0)
			bds_datas.clear();
		return iRet;
	}

#ifdef _DEBUG_INFO_
	c=getUTime();
	printf("数据比对,并记录CompairLog\n");
#endif
	//数据比对,并记录CompairLog
	iRet = CheckDataByList(checkdata,bds_datas,db_datas,check_type);
	if(iRet != 0)
	{
		if (kvs.size()>0)
			clearDataList(kvs);
		if (db_datas.size()>0)
			db_datas.clear();
		if (bds_datas.size()>0)
			bds_datas.clear();
		return iRet;
	}
	if (kvs.size()>0)
		clearDataList(kvs);
	if (db_datas.size()>0)
		db_datas.clear();
	if (bds_datas.size()>0)
		bds_datas.clear();
#ifdef _DEBUG_INFO_
	d=getUTime();
	printf("b:%ld|c:%ld|d:%ld|\n",b-a,c-b,d-c);
#endif
	
	return iRet;
}

int Check_BDSMgr::process()
{
	int iRet = 0,cRet = 0;
	
#ifdef _DEBUG_INFO_
	printf("----------begin--------\n");
#endif

	//读取接口表
	if (0 != getInterfaceDatas())
	{
		exit(0);
	}
	//首次比对(data_interface--->compare_log)
	for(vector<stCheckData*>::iterator it = m_CheckDatas.begin();it != m_CheckDatas.end();it++)
	{
		stCheckData *checkdata = *it;
		iRet = ProCheckDataUserToRecord(checkdata);
		if (iRet == 0)
		{
			m_Ibear->Commit();
		}
		else 
		{
			m_Ibear->Rollback();
		}
		cRet = deleteInterface(checkdata->key_id,checkdata->table_name);
		if (cRet == 0)
		{
			m_Ibear->Commit();
		}
		else 
		{
			m_Ibear->Rollback();
			exit(0);
		}
	}
	if (m_CheckDatas.size()>0)
		clearDataList(m_CheckDatas);
	
	//后续比对,针对不平的数据进行比对(compare_log--->repair_log)
	iRet = selectCompareLog();
	if (iRet != 0)
	{
		exit(0);
	}
	if (m_CheckDatas.size() == 0)
	{
		sleep(1);
		return RET_OK;
	}
	long t1 = 0,t2 = 0;
	for(vector<stCheckData*>::iterator it = m_CheckDatas.begin();it != m_CheckDatas.end();it++)
	{
		stCheckData *checkdata = *it;
		for (; checkdata->times <= m_Controls->check_nums; checkdata->times++)
		{
			t1 = getUTime();
			memset(m_ErrNo,0,sizeof(m_ErrNo));
			memset(m_ErrMsg,0,sizeof(m_ErrMsg));
#ifdef _DEBUG_INFO_
	printf("check_sn[%s];key_id[%s];table_name[%s];check_type[%s];data_type[%s];times[%d]\n",checkdata->check_sn,checkdata->key_id,checkdata->table_name,checkdata->check_type,checkdata->data_type,checkdata->times);
	long second_beg_time = 0,second_end_time = 0,pair_time=0,check_time=0;
	second_beg_time = getUTime();
#endif
			WriteLog(m_LogName,LOG_SYS_RUNNING,"check_sn[%s];key_id[%s];table_name[%s];check_type[%s];data_type[%s];times[%d]",checkdata->check_sn,checkdata->key_id,checkdata->table_name,checkdata->check_type,checkdata->data_type,checkdata->times);
			
			//数据相同，不再进行多次比对
			if (checkdata->times != 0 && (strcmp(checkdata->check_type,"4") == 0 || strcmp(checkdata->check_type,"0") == 0))
			{
				WriteLog(m_LogName,LOG_SYS_RUNNING,"数据相同,不再进行多次[%d]比对!!!",checkdata->times);
				break;
			}
			if (checkdata->times >= m_Controls->check_nums-1)
			{
				//已到次数的进行数据修复
#ifdef _DEBUG_INFO_
	printf("已到次数[%d]的进行数据修复\n",checkdata->times);
#endif
				WriteLog(m_LogName,LOG_SYS_RUNNING,"已到次数[%d]的进行数据修复",checkdata->times);
				iRet = ProRepairData(checkdata);
				if (iRet == 0)
				{
					m_Ibear->Commit();
				}
				else 
				{
					m_Ibear->Rollback();
				}
#ifdef _DEBUG_INFO_
	pair_time = getUTime();
#endif
			}
			else 
			{
#ifdef _DEBUG_INFO_
	printf("未到次数[%d]的再次比对\n",checkdata->times);
#endif
				WriteLog(m_LogName,LOG_SYS_RUNNING,"未到次数[%d]的再次比对",checkdata->times);
				//未到次数的再次比对
				iRet = ProCheckDataRecord(checkdata,checkdata->check_type);
				if (iRet == 0)
				{
					m_Ibear->Commit();
				}
				else 
				{
					m_Ibear->Rollback();
				}
#ifdef _DEBUG_INFO_
	check_time = getUTime();
#endif
			}
			if (iRet != 0)
			{
				char sql_error[256+1] = {0};
				if (strcmp(m_ErrNo,"") == 0)
				{
					sprintf(sql_error,"%s|%s",m_Ibear->no_error,m_Ibear->sql_error);
				}
				else 
				{
					sprintf(sql_error,"%s|%s",m_ErrNo,m_ErrMsg);
				}
				iRet = insertCompareLogErr(checkdata->key_id,checkdata->table_name,sql_error);
				if(iRet != 0)
				{
					m_Ibear->Rollback();
					exit(0);
				}
				iRet = deletedateCompareLog(checkdata->key_id,checkdata->table_name);
				if(iRet != 0)
				{
					m_Ibear->Rollback();
					exit(0);
				}
				m_Ibear->Commit();
				memset(m_ErrNo,0,sizeof(m_ErrNo));
				memset(m_ErrMsg,0,sizeof(m_ErrMsg));
				break;
			}
#ifdef _DEBUG_INFO_
	second_end_time = getUTime();
	printf("second:%ld|pair_time:%ld|check_time:%ld|\n",second_end_time-second_beg_time,pair_time-second_beg_time,check_time-second_beg_time);
#endif
			if (checkdata->times >= m_Controls->check_nums-1)
			{
				break;
			}
		}
	}
	if (m_CheckDatas.size()>0)
		clearDataList(m_CheckDatas);

#ifdef _DEBUG_INFO_
	printf("----------end--------\n");
#endif
	return iRet;
}


int Check_BDSMgr::ProTimeList(char *sql_key)
{
	int iRet = 0;
	return iRet;
}

int Check_BDSMgr::setTask(vector<string> v_row,stDealTask *pTask)
{
	return 0;
}

int Check_BDSMgr::ProInterface(vector<stDealTask*> *values)
{
	int iRet = 0;
	
	return iRet;
}

int Check_BDSMgr::UpdPoorStatus(stDealTask *pTask)
{
	int iRet = 0;
	return iRet;
}


