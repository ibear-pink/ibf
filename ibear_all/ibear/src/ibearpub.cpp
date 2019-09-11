/**************************
 * 服务名称:ibearpub
 * 功能描述:公共文件
 * 程序版本:V1.0
 * 编码日期:2016/11/09
 * 编码人员:wangzhia
 **************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "ibearpub.h"

char *xmlTrim(char *S)
{
	int I = 0, L = 0;
	if (S == NULL)
	{
		return S;
	}
	L = strlen(S) - 1;
	I = 0;
	while ( (I <= L) && (S[I] <= ' ') && (S[I] > 0) )
	{
		I ++;
	}
	char C[1024] = {0};
	if (I > L)
	{
		S[0] = '\0';
	}
	else
	{
		while ( (S[L] <= ' ')  && (S[L] > 0 ) )
		{
			L --;
		}
		strncpy(C, S + I, L - I + 1);
	}
	strcpy(S,C);
	return S;
}

// 替换字符串中特征字符串为指定字符串
char * xml_strrep(char *pInput,char *pOutput,char *pSrc,char *pDst)
{
	char *p;//*p1;
	char p1[1024] = {0};
	int len;
	
	if (pInput == NULL || pSrc == NULL || pDst == NULL)
		return NULL;
	
	p = strstr(pInput, pSrc);       //返回字符串第一次出现的地址,否则返回NULL
	if (p == NULL)  /*not found*/
		return NULL;
	//printf("p=%s\n",p);
	len = strlen(pInput) + strlen(pDst) - strlen(pSrc);
	/*p1 = (char*)malloc(len);
	 if (p1 == NULL)
	 {
		return NULL;
	 }
	 bzero(p1, len);*/
	strncpy(p1, pInput, p-pInput);
	strcat(p1, pDst);
	//printf("p1=%s\n",p1);
	p += strlen(pSrc);
	strcat(p1, p);
	strcpy(pOutput,p1);
	
	return pOutput;
}

/*
 *读文件，返回文件内容
 *入参：文件全路径
 *出参：文件内容
 */
int xmlFileRead(char* filename,string *info)
{
	if(filename == NULL || strcmp(xmlTrim(filename),"") == 0)
	{
		return -1;
	}
	string temp;
	FILE *pf = fopen(filename,"r");
	if (pf == NULL)
	{
		printf("%s未找到\n",filename);
		return -2;
	}
	fseek(pf,0,SEEK_END);
	long lSize = ftell(pf);
	// 用完后需要将内存free掉
	char* text=(char*)malloc(lSize+1);
	if (text == NULL)
	{
		return -3;
	}
	rewind(pf);
	fread(text,sizeof(char),lSize,pf);
	text[lSize] = '\0';
	fclose(pf);
	pf = NULL;
	temp = string(text);
	*info = temp;
	free(text);
	text = NULL;
	return 0;
}

/*
 * 解析sqlMapConfig.xml 将数据计入内存
 * pSrc:sqlMapConfig.xml的路径
 */
int Analysis_ConfXML(char *pSrc,char *host_id,stSqlMapConfig *sql_Conf)
{
	int iRet = 0;
	char pro_MapValue[1024] = {0};
	char sz_Value[1024] = {0};
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	
	printf("xml路径为：%s\n",pSrc);
	string xml_src;
	iRet = xmlFileRead(pSrc,&xml_src);
	if (iRet != 0)
	{
		return iRet;
	}
	if (strcmp(xml_src.c_str(),"") == 0)
	{
		printf("xml_src为空！\n");
		return -1;
	}
	//printf("xml：%s\n",xml_src);
	/*解析入参*/
	doc = xmlParseMemory(xml_src.c_str(),sizeof(xml_src.c_str()));
	if(doc==NULL)
	{
		printf("xml：%s\n",xml_src.c_str());
		printf("入参为空！\n");
		return -1;
	}
	
	cur = xmlDocGetRootElement(doc); //确定文档根元素
	if (cur==NULL)/*检查确认当前文档中包含内容*/
	{
		printf("xmlDocGetRootElement wrong!\n");
		xmlFreeDoc(doc);
		return -1;
	}
	/*确认文档是正确的类型,"sqlMapConfig"是使用xml的根类型。*/
	if(xmlStrcasecmp(cur->name,(const xmlChar *)"sqlMapConfig")!=0)
	{
		printf("document of the wrong type, root node =%s  != sqlMapConfig!!!\n",cur->name);
		xmlFreeDoc(doc);
		return -1;
	}
	
	cur=cur->xmlChildrenNode;
	int i = 0;
	while (cur != NULL)
	{
		memset(sz_Value,0,sizeof(sz_Value));
		if(xmlStrcasecmp(cur->name,(const xmlChar *)"settings")==0)
		{
			/*------------------------------------cacheModelsEnabled----------------------------------*/
			sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"cacheModelsEnabled"));
			if(sz_Value==NULL)
			{
				cur=cur->next;
				printf("xmlGetProp %s is wrong!\n",cur->name);
				return -1;
			}
			strcpy((sql_Conf->set).cacheModelsEnabled,sz_Value);
			printf("cacheModelsEnabled:%s\n",(sql_Conf->set).cacheModelsEnabled);
			
			/*------------------------------------testModel----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"testModel") == NULL)
			{
				(sql_Conf->set).testModel = false;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"testModel"));
				if(sz_Value==NULL)
				{
					(sql_Conf->set).testModel = false;
				}
				else if (strcmp(sz_Value,"true")==0)
				{
					(sql_Conf->set).testModel = true;
				}
				else
				{
					(sql_Conf->set).testModel = false;
				}
			}
			printf("testModel:%d\n",(sql_Conf->set).testModel);

			/*------------------------------------autoCommit----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"autoCommit") == NULL)
			{
				(sql_Conf->set).autoCommit = 0;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"autoCommit"));
				if(sz_Value==NULL)
				{
					(sql_Conf->set).autoCommit = 0;
				}
				else
				{
					(sql_Conf->set).autoCommit = atoi(sz_Value);
				}
			}
			printf("autoCommit:%d\n",(sql_Conf->set).autoCommit);
			
			/*------------------------------------defaultDBNum----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"defaultDBNum") == NULL)
			{
				strcpy((sql_Conf->set).defaultDBNum,"");
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"defaultDBNum"));
				if(sz_Value==NULL)
				{
					strcpy((sql_Conf->set).defaultDBNum,"");
				}
				else
				{
					strcpy((sql_Conf->set).defaultDBNum,sz_Value);
				}
			}
			printf("defaultDBNum:%s\n",(sql_Conf->set).defaultDBNum);
			
			/*------------------------------------CrossDBNums----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"CrossDBNums") == NULL)
			{
				strcpy((sql_Conf->set).CrossDBNums,"");
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"CrossDBNums"));
				if(sz_Value == NULL)
				{
					strcpy((sql_Conf->set).CrossDBNums,"");
				}
				else
				{
					strcpy((sql_Conf->set).CrossDBNums,sz_Value);
				}
			}
			printf("CrossDBNums:%s\n",(sql_Conf->set).CrossDBNums);
			
			/*------------------------------------autoClearEnabled----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"autoClearEnabled") == NULL)
			{
				printf("xmlGetProp %s is wrong!\n",cur->name);
				return -1;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"autoClearEnabled"));
				if(sz_Value==NULL)
				{
					cur=cur->next;
					printf("xmlGetProp %s is wrong!\n",cur->name);
					return -1;
				}
				strcpy((sql_Conf->set).autoClearEnabled ,sz_Value);
				printf("autoClearEnabled:%s\n",(sql_Conf->set).autoClearEnabled);
			}
			
			/*------------------------------------maxOverflaw----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"maxOverflaw") == NULL)
			{
				(sql_Conf->set).maxOverflaw = 128;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"maxOverflaw"));
				if(sz_Value==NULL)
				{
					(sql_Conf->set).maxOverflaw = 128;
				}
				else
				{
					(sql_Conf->set).maxOverflaw = atoi(sz_Value);
				}
				printf("maxOverflaw:%d\n",(sql_Conf->set).maxOverflaw);
			}
			
			/*------------------------------------performanceAnalyzeEnabled----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"performanceAnalyzeEnabled") == NULL)
			{
				(sql_Conf->set).performanceAnalyzeEnabled = 0;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"performanceAnalyzeEnabled"));
				if(sz_Value==NULL || strcmp(sz_Value,"false") == 0)
				{
					(sql_Conf->set).performanceAnalyzeEnabled = 0;
				}
				else if (strcmp(sz_Value,"true") == 0)
				{
					(sql_Conf->set).performanceAnalyzeEnabled = 1;
				}
				printf("performanceAnalyzeEnabled:%d\n",(sql_Conf->set).performanceAnalyzeEnabled);
			}
			
			/*------------------------------------performanceLogPath----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"performanceLogPath") == NULL)
			{
				strcpy((sql_Conf->set).performanceLogPath ,"./");
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"performanceLogPath"));
				if(sz_Value==NULL)
				{
					strcpy((sql_Conf->set).performanceLogPath ,"./");
				}
				else
				{
					strcpy((sql_Conf->set).performanceLogPath ,sz_Value);
				}
				printf("performanceLogPath:%s\n",(sql_Conf->set).performanceLogPath);
			}
			
			/*------------------------------------performanceThreshold----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"performanceThreshold") == NULL)
			{
				(sql_Conf->set).performanceThreshold = 0;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"performanceThreshold"));
				if(sz_Value==NULL)
				{
					(sql_Conf->set).performanceThreshold = 0;
				}
				else
				{
					(sql_Conf->set).performanceThreshold = atoi(sz_Value);
				}
				printf("performanceThreshold:%d\n",(sql_Conf->set).performanceThreshold);
			}
			
			/*------------------------------------reConnect----------------------------------*/
			if (xmlGetProp(cur,(const xmlChar *)"reConnect") == NULL)
			{
				(sql_Conf->set).reConnect = 0;
			}
			else
			{
				sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"reConnect"));
				if(sz_Value==NULL || strcmp(sz_Value,"false") == 0)
				{
					(sql_Conf->set).reConnect = 0;
				}
				else if (strcmp(sz_Value,"true") == 0)
				{
					(sql_Conf->set).reConnect = 1;
				}
				printf("reConnect:%d\n",(sql_Conf->set).reConnect);
			}
		}
		
		if(xmlStrcasecmp(cur->name,(const xmlChar *)"sqlMap")==0)
		{
			strcpy(sz_Value,(char *)xmlGetProp(cur,(const xmlChar *)"resource"));
			if(sz_Value==NULL)
			{
				cur=cur->next;
				printf("xmlGetProp %s is wrong!\n",cur->name);
				return -1;
			}
			if (host_id != NULL)
			{
				//替换resource中的host_id
				char pSrc[10+1] = {0};
				strcpy(pSrc,"{host_id}");
				if (NULL == xml_strrep(sz_Value,sz_Value,pSrc,host_id))
				{
					return -1;
				}
			}
			strcpy(sql_Conf->sqlsrc_map[i],sz_Value);
			printf("[%d]resource:%s\n",i,sz_Value);
			i++;
			sql_Conf->map_num = i;
		}
		cur=cur->next;
	}
	
	xmlFreeDoc(doc);
	//xmlCleanupParser();
	
	return 0;
}

/*
 * 解析sql的xml，将数据计入内存
 * pSrc:解析sql的xml的路径
 */
int Analysis_SqlXML(stSqlMapConfig *sql_Conf,map<string,stSqlMap*> *sqlconfs,stSettings *settings)
{
	int iRet = 0;
	map<string,string> repedMap;
	repedMap.insert(pair<string,string>(string("@lt"),string("<")));
	repedMap.insert(pair<string,string>(string("@gt"),string(">")));
	
	for(int i = 0; i < sql_Conf->map_num; i++)
	{
		/*解析入参*/
		xmlDocPtr doc = NULL;
		xmlNodePtr cur = NULL;
		xmlNodePtr sel = NULL;
		char pro_MapValue[1024] = {0};
		char sz_Value[1024] = {0};
		char pSrc[1024] = {0};
		
		strcpy(pSrc,sql_Conf->sqlsrc_map[i]);
		
		string xml_src;
		iRet = xmlFileRead(pSrc,&xml_src);
		if (iRet != 0)
		{
			return iRet;
		}
		
		if (strcmp(xml_src.c_str(),"") == 0)
		{
			printf("xml_src为空！\n");
			return -1;
		}
		//printf("xml：%s\n",xml_src.c_str());
		/*解析入参*/
		doc = xmlParseMemory(xml_src.c_str(),sizeof(xml_src.c_str()));
		if(doc==NULL)
		{
			printf("%s\n",xml_src.c_str());
			printf("入参为空！或格式错误，请检查xml文件%s\n",pSrc);
			return -1;
		}
		/*输入参数赋值 begin*/
		cur = xmlDocGetRootElement(doc);
		if (cur==NULL)
		{
			printf("xmlDocGetRootElement wrong!\n");
			return -1;
		}
		
		if(xmlStrcasecmp(cur->name,(const xmlChar *)"sqlMap")==0)
		{
			strcpy(pro_MapValue,(char *)xmlGetProp(cur,(const xmlChar *)"namespace"));
			if(pro_MapValue==NULL)
			{
				printf("namespace %s is wrong!\n",cur->name);
				return -1;
			}
			//printf("namespace:%s\n",pro_MapValue);
		}
		
		cur=cur->xmlChildrenNode;
		if (cur==NULL)
		{
			printf("xmlChildrenNode next wrong !\n");
			return -1;
		}
		while (cur != NULL)
		{
			stSqlMap *sqlmap = new stSqlMap();
			char MapValue[1024] = {0};
			//memset(pro_MapValue,0,sizeof(pro_MapValue));
			if(xmlStrcasecmp(cur->name,(const xmlChar *)"insert")==0 || xmlStrcasecmp(cur->name,(const xmlChar *)"update")==0 || xmlStrcasecmp(cur->name,(const xmlChar *)"delete")==0)
			{
				strcpy(sqlmap->id,(char *)xmlGetProp(cur,(const xmlChar *)"id"));
				if(sqlmap->id==NULL)
				{
					printf("sqlmap->id %s is wrong!\n",cur->name);
					return -1;
				}
				
				strcpy(sqlmap->param,(char *)xmlGetProp(cur,(const xmlChar *)"parameterClass"));
				if(sqlmap->param==NULL)
				{
					printf("sqlmap->param %s is wrong!\n",cur->name);
					return -1;
				}
				
				if (xmlGetProp(cur,(const xmlChar *)"db_no") != NULL)
				{
					strcpy(sqlmap->db_no,(char *)xmlGetProp(cur,(const xmlChar *)"db_no"));
					strcpy(sqlmap->db_no_real,(char *)xmlGetProp(cur,(const xmlChar *)"db_no"));
				}
				else
				{
					strcpy(sqlmap->db_no_real,(sql_Conf->set).defaultDBNum);
				}
				
				strcpy(MapValue,(char *)xmlGetText(cur));
				if(MapValue==NULL)
				{
					printf("sqlmap->sql %s is wrong!\n",cur->name);
					return -1;
				}
				
				/*------------------------------------testModel----------------------------------*/
				if ((sql_Conf->set).testModel)
				{
					if (xmlGetProp(cur,(const xmlChar *)"testModel") == NULL)
					{
						sqlmap->testModel = true;
					}
					else
					{
						sprintf(sz_Value,"%s",xmlGetProp(cur,(const xmlChar *)"testModel"));
						if (sz_Value==NULL)
						{
							sqlmap->testModel = true;
						}
						else if (strcmp(sz_Value,"true")==0)
						{
							sqlmap->testModel = true;
						}
						else
						{
							sqlmap->testModel = false;
						}
					}
				}
				else
				{
					sqlmap->testModel = false;
				}

				xmlConvSpeChar(MapValue,sqlmap->sql,repedMap);
			}
			else if(xmlStrcasecmp(cur->name,(const xmlChar *)"select")==0)
			{
				strcpy(sqlmap->id,(char *)xmlGetProp(cur,(const xmlChar *)"id"));
				if(sqlmap->id==NULL)
				{
					printf("sqlmap->id %s is wrong!\n",cur->name);
					return -1;
				}
				
				if (xmlGetProp(cur,(const xmlChar *)"parameterClass") != NULL)
				{
					strcpy(sqlmap->param,(char *)xmlGetProp(cur,(const xmlChar *)"parameterClass"));
				}
				else
				{
					printf("sqlmap->param %s is wrong!\n",cur->name);
					return -1;
				}
				
				if (xmlGetProp(cur,(const xmlChar *)"resultNum") != NULL)
				{
					strcpy(sqlmap->res_num,(char *)xmlGetProp(cur,(const xmlChar *)"resultNum"));
				}
				else
				{
					strcpy(sqlmap->res_num,"0");
				}
				
				if (xmlGetProp(cur,(const xmlChar *)"db_no") != NULL)
				{
					strcpy(sqlmap->db_no,(char *)xmlGetProp(cur,(const xmlChar *)"db_no"));
					strcpy(sqlmap->db_no_real,(char *)xmlGetProp(cur,(const xmlChar *)"db_no"));
				}
				else
				{
					strcpy(sqlmap->db_no_real,(sql_Conf->set).defaultDBNum);
				}
				
				if (xmlGetProp(cur,(const xmlChar *)"memType") != NULL)
				{
					strcpy(sqlmap->memType,(char *)xmlGetProp(cur,(const xmlChar *)"memType"));
				}
				else
				{
					strcpy(sqlmap->memType,(sql_Conf->set).cacheModelsEnabled);
				}
				
				strcpy(MapValue,(char *)xmlGetText(cur));
				if(MapValue==NULL)
				{
					printf("sqlmap->sql %s is wrong!\n",cur->name);
					return -1;
				}
				xmlConvSpeChar(MapValue,sqlmap->sql,repedMap);
				//printf("cur:%s,%s,%s,%s\n",cur->name,sqlmap->id,sqlmap->res_num,sqlmap->db_no_real);
				
				//取字段
				sel=cur->xmlChildrenNode;
				while (sel != NULL)
				{
					if(xmlStrcasecmp(sel->name,(const xmlChar *)"param")==0)
					{
						stDataParam *param = new stDataParam();
						strcpy(param->name,(char *)xmlGetProp(sel,(const xmlChar *)"name"));
						strcpy(param->type,(char *)xmlGetProp(sel,(const xmlChar *)"type"));
						param->dataNum = ibear_SQLDataNum(sqlmap->sql,param->name);
						if (xmlGetProp(sel,(const xmlChar *)"len") != NULL)
						{
							param->len = atoi((char *)xmlGetProp(sel,(const xmlChar *)"len"));
						}
						else
						{
							param->len = 1024;
						}
						sqlmap->outParams.insert(pair<int,stDataParam*>(param->dataNum,param));
					}
					sel=sel->next;
				}
			}
			xml_TtoSp(sqlmap->sql,sqlmap->sql);
			(*sqlconfs).insert(pair<string,stSqlMap*>(sqlmap->id,sqlmap));
			cur=cur->next;
		}
		if (doc != NULL)
		{
			xmlFreeDoc(doc);
			//xmlCleanupParser();
		}
	}
	
	memcpy (settings, &(sql_Conf->set), sizeof(stSettings));
	return 0;
}

int init_XxmlFile(char *fileName,char *host_id,map<string,stSqlMap*> *sqlconfs,stSettings *settings)
{
	int iRet = 0;
	printf("初始化XML文件！\n");
	stSqlMapConfig *sql_Conf = new stSqlMapConfig();
	iRet = Analysis_ConfXML(fileName,host_id,sql_Conf);
	if (0 != iRet)
	{
		printf("Analysis_ConfXML ERROR\n");
		delete sql_Conf;
		sql_Conf = NULL;
		return iRet;
	}
	
	iRet = Analysis_SqlXML(sql_Conf,sqlconfs,settings);
	if (0 != iRet)
	{
		printf("Analysis_SqlXML ERROR\n");
		delete sql_Conf;
		sql_Conf = NULL;
		return iRet;
	}
	
	delete sql_Conf;
	sql_Conf = NULL;
	
	getSqlContent(sqlconfs);
	//解析后的sql;
	//链接的数据库编号db_no;
	//key值
	//属性map
	printf("初始化XML文件成功！\n");
	if(settings->performanceAnalyzeEnabled == 1)
	{
		initLog(settings->performanceLogPath,host_id,true,settings->testModel);
	}
	else if(settings->performanceAnalyzeEnabled != 1 && settings->testModel)
	{
		initLog(settings->performanceLogPath,host_id,false,settings->testModel);
	}
	return 0;
}

void initLog(char *logpath,char *host_id,bool perforModel,bool testModel)
{
	printf("logpath:%s\n",logpath);
#ifdef _PERFORMANCE_
	ilog_cfgserver *log1 = new ilog_cfgserver();
	strcpy(log1->log_path,logpath);
	strcpy(log1->log_name,"ibear_performance");
	log1->log_file_num = 3;
	log1->file_type = 1;
	log1->log_thread_num = 1;
	log1->logfile_infos[0].log_type = 1004;
	log1->logfile_infos[0].log_invalid = !perforModel;
	log1->logfile_infos[0].log_procmode = 2;
	log1->logfile_infos[0].log_backup_type = 3;
	log1->logfile_infos[0].log_interval = 10;
	
	log1->logfile_infos[1].log_type = 1009;
	log1->logfile_infos[1].log_invalid = !perforModel;
	log1->logfile_infos[1].log_procmode = 2;
	log1->logfile_infos[1].log_backup_type = 3;
	log1->logfile_infos[1].log_interval = 10;
	
	log1->logfile_infos[2].log_type = 1000;
	log1->logfile_infos[2].log_invalid = !testModel;
	log1->logfile_infos[2].log_procmode = 2;
	log1->logfile_infos[2].log_backup_type = 3;
	log1->logfile_infos[2].log_interval = 10;
	if (host_id == NULL)
	{
		if (!App_iLog::Instance()->ilog_Init (log1))
		{
			printf ("App_LogManager Init Is Failed.\n");
			return;
		}
	}
	else
	{
		if (!App_iLog::Instance()->ilog_Init (log1,host_id))
		{
			printf ("App_LogManager Init Is Failed.\n");
			return;
		}
	}
	//日志管理中心启动
	App_iLog::Instance()->ilog_Run ();
	printf("初始化性能日志成功！\n");
#endif
}

/*
 解析SQL
 入参：
	tpl_content,需要解析SQL
 出参：
	out_content,解析后拼接正常的串,参数value值，防止硬解析
	bindParams,参数key及类型
 */
int xmlbuild_sql(char *tpl_content,char *out_content,map<string,stSQLParam*> *bindParams)
{
	int iRet = 0;
	char param[128] = {0};
	char tmpBuf[1024] = {0};
	char out_content_tmp[1024] = {0};
	char key[32] = {0};//参数名称
	char type[10] = {0};//参数类型
	char str_len[10] = {0};
	char repedStr[64] = {0};
	char flag[1+1] = {0};//替换后的值
	char *p=NULL;
	int i = 0, end = 0, begin = 0, mid = 0, num = 0, o_begin = 0, o_end = 0;
	int len1 = 0 , q = 0;
	
	map<string,stSQLParam*> tmpBindParams;
	strcpy(flag,"?");
	//sprintf (tmpBuf, "%s", tpl_content);
	strcpy(tmpBuf,tpl_content);
	int len = strlen(tmpBuf);
	//tmpBuf[len-1] = '\0';
	//sprintf (out_content_tmp, "%s", tmpBuf);
	strcpy(out_content_tmp,tpl_content);
	int is_empty = 0;
	int is_in = 0,in_begin = 0,in_second = 0,in_end = 0;
	int param_num = 0,type_num = 0;
	
	while (tmpBuf[i] != '\0' && i <= len)
	{
		if (tmpBuf[i] == '$' && tmpBuf[i+1] == '{')
		{
			begin = i+2;
			i++;
			param_num++;
			continue;
		}
		//first '|'
		else if (tmpBuf[i] == '|' && tmpBuf[i+1] != '|' && tmpBuf[i-1] != '|' && type_num == 0)
		{
			mid = i;
			memcpy (key, tmpBuf+begin, mid - begin);
			key[mid-begin] = '\0';
			type_num = 1;
		}
		else if (tmpBuf[i] == '}' && type_num == 1)
		{
			type_num = 0;
			stSQLParam *sql_param = new stSQLParam();
			end = i;
			memcpy (type, tmpBuf+begin, end - begin);
			type[end-begin] = '\0';
			strcpy(sql_param->key,key);
			strcpy(sql_param->type,type);
			sql_param->num = num;
			sql_param->isEmpty = is_empty;
			num++;
			char ckey[32] = {0};
			for (int z = 0; z < num; z++)
			{
				sprintf(ckey,"%s:%d",key,z);
				map<string,stSQLParam*>::iterator iz = tmpBindParams.find(string(ckey));
				if (iz == tmpBindParams.end())
				{
					break;
				}
			}
			tmpBindParams.insert(pair<string,stSQLParam*>(string(ckey),sql_param));
		}
		//second '|'
		else if (tmpBuf[i] == '|' && tmpBuf[i+1] != '|' && tmpBuf[i-1] != '|' && type_num == 1)
		{
			in_second = i;
			memcpy (type, tmpBuf + begin, in_second - begin);
			type[in_second - begin] = '\0';
			type_num = 2;
		}
		else if (tmpBuf[i] == '}' && type_num == 2)
		{
			type_num = 0;
			stSQLParam *sql_param = new stSQLParam();
			end = i;
			memcpy (str_len, tmpBuf + begin, end - begin);
			str_len[end-begin] = '\0';
			strcpy(sql_param->key,key);
			strcpy(sql_param->type,type);
			sql_param->len = atoi(str_len);
			sql_param->num = num;
			sql_param->isEmpty = is_empty;
			num++;
			char ckey[32] = {0};
			for (int z = 0; z < num; z++)
			{
				sprintf(ckey,"%s:%d",key,z);
				map<string,stSQLParam*>::iterator iz = tmpBindParams.find(string(ckey));
				if (iz == tmpBindParams.end())
				{
					break;
				}
			}
			tmpBindParams.insert(pair<string,stSQLParam*>(string(ckey),sql_param));
		}
		else if (tmpBuf[i] == '#')
		{
			if (o_begin == 0)
			{
				o_begin = i+1;
				o_end = 0;
			}
			else if (o_end == 0)
			{
				o_end = i;
				memcpy (key, tmpBuf+o_begin, o_end-o_begin);
				key[o_end - o_begin] = '\0';
				stSQLParam *sql_param = new stSQLParam();
				strcpy(sql_param->key,key);
				strcpy(sql_param->type,"char");
				sql_param->name_type = 1;
				sql_param->isEmpty = is_empty;
				char ckey[32] = {0};
				for (int z = 0; z < 10; z++)
				{
					sprintf(ckey,"%s:%d",key,z);
					map<string,stSQLParam*>::iterator iz = tmpBindParams.find(string(ckey));
					if (iz == tmpBindParams.end())
					{
						break;
					}
				}
				tmpBindParams.insert(pair<string,stSQLParam*>(string(ckey),sql_param));
				o_begin = 0;
			}
		}
		else if (tmpBuf[i] == '$' && tmpBuf[i+1] == 'E' && tmpBuf[i+2] == '[')
		{
			is_empty = 1;
		}
		else if (tmpBuf[i] == ']')
		{
			is_empty = 0;
			if (is_in == 1)
			{
				in_end = i;
				memcpy (key, tmpBuf+in_begin, in_end-in_begin);
				key[in_end - in_begin] = '\0';
				stSQLParam *sql_param = new stSQLParam();
				strcpy(sql_param->key,key);
				strcpy(sql_param->type,"char");
				sql_param->name_type = 4;
				sql_param->isEmpty = 0;
				sql_param->num = num;
				num++;
				char ckey[32] = {0};
				sprintf(ckey,"%s:%d",key,num);
				tmpBindParams.insert(pair<string,stSQLParam*>(string(ckey),sql_param));
				in_begin = 0;
				is_in = 0;
			}
		}
		else if (tmpBuf[i] == '$' && tmpBuf[i+1] == 'L' && tmpBuf[i+2] == '[')
		{
			is_in = 1;
			in_begin = i+3;
			in_end = 0;
		}
		else
		{
			i++;
			continue;
		}
		i++;
		begin = i;
	}

	for (map<string,stSQLParam*>::iterator iter = tmpBindParams.begin();iter != tmpBindParams.end();iter++)
	{
		stSQLParam *param = iter->second;
		memset(repedStr,0,sizeof(repedStr));
		sprintf(repedStr,"${%s|%s}",param->key,param->type);
		p = strstr(out_content_tmp, repedStr);
		if (p == NULL)
		{
			memset(repedStr,0,sizeof(repedStr));
			sprintf(repedStr,"${%s|%s|%d}",param->key,param->type,param->len);
			p = strstr(out_content_tmp, repedStr);
			if (p == NULL)
			{
				continue;
			}
		}
		
		if( NULL==xml_strrep((char *)out_content_tmp,(char *)out_content_tmp,repedStr,flag))
		{
			if ( 0 != tmpBindParams.size())
			{
				clearXMLList(tmpBindParams);
			}
			return -1;
		}
	}
	strcpy(out_content,out_content_tmp);
	
	*bindParams = tmpBindParams;
	/*if ( 0 != tmpBindParams.size())
	{
		clearXMLList(tmpBindParams);
	}
	printf("%ld\n",(*bindParams).size());*/
	return 0;
}

int getSqlContent(map<string,stSqlMap*> *sqlconfs)
{
	stSqlMap *sqlmap = NULL;
	char sql[1024] = {0};
	char key[1024] = {0};
	char tmp_out[1024] = {0};
	
	for (map<string,stSqlMap*>::iterator dataIter = (*sqlconfs).begin(); dataIter != (*sqlconfs).end(); dataIter++)
	{
		sqlmap = dataIter->second;
		if (0 != xmlbuild_sql(sqlmap->sql,tmp_out,&(sqlmap->inParams)))
		{
			return -1;
		}
		strcpy(sqlmap->new_sql,tmp_out);
	}
	
	return 0;
}

int xmlConvSpeChar(char *old_src,char *new_src,map<string,string> repedMap)
{
	char temp_src[1024] = {0};
	strcpy(temp_src,old_src);
	strcpy(new_src,old_src);
	//增加替换项
	for (map<string,string>::iterator it=repedMap.begin();it!=repedMap.end();it++)
	{
		char repedStr[5]={0};
		char flag[5]={0};
		strcpy(repedStr,(it->first).c_str());
		strcpy(flag,(it->second).c_str());
		while (strstr(temp_src, repedStr)!=NULL)
		{
			if( NULL==xml_strrep((char *)temp_src,(char *)new_src,repedStr,flag))
			{
				return -1;
			}
			strcpy(temp_src,new_src);
		}
	}
	
	return 0;
}

// 替换字符串中特征字符串为指定字符串
char * xml_deleteEmptyStr(int *dnums,int now_num,char *src,char *new_src,int num)
{
	if (src == NULL)
		return NULL;
	for (int n=0; n<=now_num; n++)
	{
		if (dnums[n] < num)
		{
			num--;
		}
	}
	char *p_be,*p_en,*p_key;
	char left[1024] = {0};
	char right[1024] = {0};
	char begin[1024] = {0};
	char end[1024] = {0};
	int j = 0;
	char pSrc[3+1] = {0};
	char pDsc[1+1] = {0};
	char key_tmp[1024] = {0};
	strcpy(pSrc,"$E[");
	strcpy(pDsc,"]");
	strcpy(new_src,src);
	for (int n=0; n<strlen(new_src); n++)
	{
		if(new_src[n] == '?')
		{
			j++;
		}
		if (j == num+1)
		{
			strncpy(begin,new_src,n);
			strncpy(end,new_src+n,strlen(src)-n);
			break;
		}
	}
	p_en = strstr(end, pDsc);
	if (p_en == NULL)
		return new_src;
	p_be = strstr(begin, pSrc);
	if (p_be == NULL)
		return new_src;
	while (p_be != NULL) {
		strncpy(right, p_be+strlen(pSrc), strlen(p_be));
		p_be = strstr(right, pSrc);
		if (p_be == NULL)
			break;
	}
	strncpy(left,begin,strlen(begin)-3-strlen(right));
	sprintf(new_src,"%s%s",left,p_en+1);
	
	return new_src;
}

int replaceSql(char *src,map<string,stSQLParam*> bindParams,char *new_src)
{
	char temp_src[1024] = {0};
	char temp_dsrc[1024] = {0};
	strcpy(temp_src,src);
	strcpy(new_src,src);
	xmlTrim(new_src);
	int dnums[MAX_EMPTY_NUM] = {0};
	int i = 0;
	bool L_flag = false;
	for(map<string,stSQLParam*>::iterator ii = bindParams.begin(); ii != bindParams.end(); ii++)
	{
		stSQLParam *param = ii->second;
		char *p=NULL;
		char repedStr[32] = {0};
		memset(temp_dsrc,0,sizeof(temp_dsrc));
		//解析并替换$E[xxx]部分
		if (param->name_type == 2)
			param->name_type = 0;
#if _DEBUG_INFO_
		//printf("key[%s]isEmpty=%d,type=%d\n",param->key,param->isEmpty,param->name_type);
#endif
		if (param->name_type == 0 && param->isEmpty != 2)
		{
			continue;
		}
		else if(param->isEmpty == 2)
		{
			dnums[i] = param->num;
			param->name_type = 2;
			if (NULL == xml_deleteEmptyStr(dnums,i,temp_src,temp_dsrc,param->num))
			{
				printf("xml_deleteEmptyStr-2 is err[%s][%s]\n",temp_src,temp_dsrc);
				return -1;
			}
			i++;
		}
		else
		{
			strcpy(temp_dsrc,temp_src);
		}
		
		//解析并替换#xxx#部分
		sprintf(repedStr,"#%s#",param->key);
		p = strstr(temp_dsrc, repedStr);
		if (p != NULL)
		{
			char *pvalue = (char*)param->values;
			if( NULL==xml_strrep((char *)temp_dsrc,(char *)new_src,repedStr,pvalue))
			{
				printf("xml_strrep is err[%s][%s][%s]\n",temp_dsrc,repedStr,pvalue);
				return -1;
			}
			strcpy(temp_src,new_src);
		}
		else
		{
			strcpy(new_src,temp_dsrc);
			strcpy(temp_src,new_src);
		}
	}
	if (NULL == isEmpty_strrep(new_src,new_src))
	{
		printf("isEmpty_strrep is err\n");
		return -1;
	}
	strcpy(temp_src,new_src);
	return 0;
}

// 替换字符串中特征字符串为指定字符串
char * isEmpty_strrep(char *src,char *dsrc)
{
	char *p,*p1,*result;
	char begin[1024] = {0};
	char model[1024] = {0};
	char new_src[1024] = {0};
	char end[1024] = {0};
	int len,right_len;
	char pSrc[3+1] = {0};
	char pDsc[1+1] = {0};
	strcpy(pSrc,"$E[");
	strcpy(pDsc,"]");
	strcpy(new_src,src);
	
	if (src == NULL)
		return NULL;
	int j = 0,i = 0;
	while(new_src[i] != '\0')
	{
		if (new_src[i] == ']')
		{
			i++;
		}
		else if (new_src[i] == '$' && new_src[i+1] == 'E' && new_src[i+2] == '[')
		{
			i = i+3;
		}
		else
		{
			new_src[j++] = new_src[i++];
		}
	}
	new_src[j] = '\0';
	strcpy(dsrc,new_src);
	return dsrc;
}

long ibear_getUTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((unsigned long)tv.tv_sec) * 1000*1000 + ((unsigned long)tv.tv_usec));
}

int xml_SplitString (const char *buff, char *tStr1, char *tStr2)
{
	int size = 0, len = 0;
	int i = 0, end = 0, begin = 0;
	char tmpBuf[1024] = {0};
	strcpy (tmpBuf, buff);
	len = strlen(tmpBuf);
	tmpBuf[len] = ':';
	tmpBuf[len+1] = '\0';
	
	while (tmpBuf[i] != '\0' && i <= len)
	{
		if (tmpBuf[i] == ':')
		{
			end = i;
			i++;
		}
		else
		{
			i++;
			continue;
		}
		size++;
		
		if (size == 1)
		{
			memcpy (tStr1, tmpBuf+begin, end - begin);
			tStr1[end-begin] = '\0';
		}
		
		if (size == 2)
		{
			memcpy (tStr2, tmpBuf+begin, end - begin);
			tStr2[end-begin] = '\0';
		}
		begin = i;
	}
	return 0;
}

int xml_TtoSp(const char *buff,char *tStr)
{
	int len = 0,i = 0;
	char tmpBuf[1024] = {0};
	strcpy(tmpBuf,buff);
	len = strlen(tmpBuf);
	tmpBuf[len] = '\0';
	while (tmpBuf[i] != '\0' && i <= len)
	{
		if (tmpBuf[i] == '\t')
		{
			tmpBuf[i] = ' ';
			i++;
		}
		else
		{
			i++;
			continue;
		}
	}
	strcpy(tStr,tmpBuf);
	return 0;
}

int ibear_conversion(char *src,char *desc)
{
	int len = strlen(src);
	int idesc = 0;
	for (int i = 0;i<len ;i++)
	{
		idesc += src[i];
	}
	sprintf(desc, "%d", idesc);
	return idesc;
}

char * ibear_stristr (const char * str1,const char * str2,char *result)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	if ( !*str2 )
		return((char *)str1);
	
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		while ( *s1 && *s2 && (!(*s1-*s2) || !(*s1-*s2-32) || !(*s1-*s2+32)) ){
			s1++, s2++;
			
			if (!*s2)
			{
				strcpy(result,cp + strlen(str2));
				return result;
			}
		}
		cp++;
	}
	
	return(NULL);
}

char * ibear_stristr (const char * str1,const char * str2,const char * str3,char *result)
{
	char *cp = (char *) str1;
	char *s1,*s11, *s2, *s3;
	if ( !*str2 )
		return((char *)str1);
	
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		while ( *s1 && *s2 && (!(*s1-*s2) || !(*s1-*s2-32) || !(*s1-*s2+32)) ){
			s1++, s2++;
		}
		int num = 1;
		if (!*s2)
		{
			char *cpp = s1;
			while (*s1)
			{
				s11 = s1;
				s3 = (char *) str3;
				while ( *s11 && *s3 && (!(*s11-*s3) || !(*s11-*s3-32) || !(*s11-*s3+32)) ){
					s11++, s3++;
					num++;
				}
				if (!*s3)
				{
					strncpy(result,cpp,num-strlen(str3)-1);
					return result;
				}
				s1++;
				num++;
			}
		}
		
		cp++;
	}
	
	return(NULL);
	
}

char * stristr (const char * str1,const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	
	if ( !*str2 )
		return((char *)str1);
	
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		
		while ( *s1 && *s2 && (!(*s1-*s2) || !(*s1-*s2-32) || !(*s1-*s2+32)) ){
			s1++, s2++;
		}
		
		if (!*s2)
			return(cp);
		
		cp++;
	}
	
	return(NULL);
	
}

int ibear_SQLDataNum(char *sql,char *name)
{
	int res_num = 0;
	int begin_lens = 0;
	int end_lens = 0;
	int infunc = 0;
	char result[1024] = {0};
	char sql_tmp[1024] = {0};
	if (checkSQL(sql,sql_tmp) != 0)
	{
		return -1;
	}
	
	ibear_stristr(sql_tmp,"select "," from ",result);
	if (result == NULL || strcmp(result,"") == 0)
	{
		return -2;
	}
	int len=strlen(result);
	result[len]=',';
	for (int i = 0;i < strlen(result); i++) {
		char data[64] = {0};
		if (result[i] == '(')
		{
			infunc++;
		}
		else if (result[i] == ')' && infunc > 0)
		{
			infunc --;
		}
		else if (result[i] == ',' && infunc == 0)
		{
			end_lens = i;
			res_num++;
			strncpy(data, result + begin_lens, end_lens - begin_lens);
			begin_lens = i + 1;
			if (strcmp(name,data) == 0)
			{
				return res_num;
			}
		}
	}
	return -3;
}

int ibear_SQLResultNum(char *sql)
{
	int res_num = 1;
	int infunc = 0;
	char result[1024] = {0};
	char sql_tmp[1024] = {0};
	if (checkSQL(sql,sql_tmp) != 0)
	{
		return -1;
	}
	
	ibear_stristr(sql_tmp,"select "," from ",result);
	if (result == NULL || strcmp(result,"") == 0)
	{
		return 0;
	}
	for (int i = 0;i < strlen(result); i++) {
		if (result[i] == '(')
		{
			infunc++;
		}
		else if (result[i] == ')' && infunc > 0)
		{
			infunc --;
		}
		else if (result[i] == ',' && infunc == 0)
		{
			res_num++;
		}
	}
	
	return res_num;
}

int checkSQL(char *sql,char *newsql)
{
	strcpy(newsql,sql);
	for (int i = 0;i<strlen(newsql);i++)
	{
		if (newsql[i] == '\t' || newsql[i] == '\n')
		{
			newsql[i] = ' ';
		}
	}
	if (stristr(sql," where ") == NULL)
	{
		sprintf(newsql,"%s where NULL ",newsql);
	}
	if (stristr(sql,"select ") == NULL)
	{
		return -1;
	}
	if (stristr(sql," from ") == NULL)
	{
		return -1;
	}
	sprintf(newsql,"%s",newsql);
	return 0;
}

int ibear_SQLResults(char *sql,char *tableName,vector<string> *results,char *result)
{
	int res_num = 1;
	int infunc = 0;
	int last = 0;
	char sql_tmp[1024] = {0};
	char result_tmp[1024] = {0};
	if (checkSQL(sql,sql_tmp) != 0)
	{
		return -1;
	}
	
	ibear_stristr(sql_tmp,"select "," from ",result_tmp);
	sprintf(result_tmp, "%s|",result_tmp);
	ibear_stristr(sql_tmp," from "," where ",tableName);
	for (int i = 0;i < strlen(result_tmp); i++) {
		if (result_tmp[i] == '(')
		{
			infunc++;
		}
		else if (result_tmp[i] == ')' && infunc > 0)
		{
			infunc --;
		}
		else if (result_tmp[i] == ',' && infunc == 0)
		{
			res_num++;
			char tmp[1024] = {0};
			if (last == 0)
			{
				strncpy(tmp,result_tmp+last,i-last);
				sprintf(result,"%s",xmlTrim(tmp));
			}
			else
			{
				strncpy(tmp,result_tmp+last+1,i-last-1);
				sprintf(result,"%s,%s",result,xmlTrim(tmp));
			}
			(*results).push_back(string(xmlTrim(tmp)));
			last = i;
		}
		else if (i == strlen(result_tmp)-1)
		{
			char tmp[1024] = {0};
			if (res_num == 1)
			{
				strncpy(tmp,result_tmp+last,i-last);
				sprintf(result,"%s,",xmlTrim(tmp));
			}
			else
			{
				strncpy(tmp,result_tmp+last+1,i-last-1);
				sprintf(result,"%s,%s,",result,xmlTrim(tmp));
			}
			(*results).push_back(string(xmlTrim(tmp)));
			last = i;
		}
	}
	
	return res_num;
}

int ibear_SQLRules(char *sql,char *rule,vector<string> *rules)
{
	char sql_tmp[1024] = {0};
	char rule_tmp[1024] = {0};
	if (checkSQL(sql,sql_tmp) != 0)
	{
		return -1;
	}
	
	ibear_stristr(sql_tmp,"where ",rule);
	sprintf(rule_tmp, " and %s and ", rule);
	char sign_rule[1024] = {0};
	while(ibear_stristr(rule_tmp," and "," and ",sign_rule) != NULL)
	{
		long i = strlen(sign_rule) + 4;
		long j = strlen(rule_tmp);
		(*rules).push_back(string(sign_rule));
		memset(sign_rule,0,sizeof(sign_rule));
		if (j <= i)
		{
			break;
		}
		char temp[1024] = {0};
		strcpy(temp,rule_tmp + i);
		strcpy(rule_tmp,temp);
	}
	
	return 0;
}

char * ibear_getSQLParam(stSqlMap *sqlmap,map<string,void*> inParams,map<string,stSQLParam*> &bindParams,char *sql_out)
{
	char *no_error = (char*)malloc(32*sizeof(char));
	char sql_error[1024] = {0};
	strcpy(no_error,"0");
	strcpy(sql_out,sqlmap->new_sql);

	//给sql入参进行赋值
	for (map<string,stSQLParam*>::iterator iter = bindParams.begin(); iter != bindParams.end(); iter++)
	{
		stSQLParam *pmap = iter->second;
		string key = iter->first;
		char ckey[128] = {0};
		char cnum[2+1] = {0};
		if (0 != xml_SplitString(key.c_str(),ckey,cnum))
		{
			printf("xml_SplitString is ERROR[%s]\n",sqlmap->sql);
			sprintf(sql_error,"xml_SplitString is ERROR[%s]",sqlmap->sql);
			strcpy(no_error,"-1");
			throw sql_error;
			return no_error;
		}
		
		map<string,void*>::iterator it = inParams.find(string(ckey));
		if (it != inParams.end())
		{
			if (pmap->isEmpty != 0)
			{
				pmap->isEmpty = 1;
			}
			//strcpy(pmap->value,(it->second).c_str());
			pmap->values = it->second;
		}
		else
		{
			if (pmap->name_type == 4)
			{
				ibear_dealListType(sqlmap,inParams,pmap,bindParams,sql_out);
			}
			else
			{
				if (pmap->isEmpty == 0)
				{
					printf("[%s]未找到相应key[%s]\n",ckey,pmap->key);
					sprintf(sql_error,"[%s]未找到相应key[%s]",ckey,pmap->key);
					strcpy(no_error,DB_SQLKEYNOTFOUND_ERROR);
					throw sql_error;
					return no_error;
				}
				else
				{
					//strcpy(pmap->value,"");
					char cvalue[10] = "";
					void *pvalue = cvalue;
					pmap->values = pvalue;
					pmap->isEmpty = 2;
				}
			}
		}
	}
	
	if (0 != replaceSql(sql_out,bindParams,sql_out))
	{
		printf("[%s]replaceSql is ERR[bindParams.size=%ld]\n",sqlmap->new_sql,bindParams.size());
		sprintf(sql_error,"[%s]replaceSql is ERR[bindParams.size=%ld]",sqlmap->new_sql,bindParams.size());
		strcpy(no_error,DB_GETOUTSQL_ERROR);
		throw sql_error;
		return no_error;
	}
	return no_error;
}

//$L[xxxx]参数判断
int ibear_dealListType(stSqlMap *sqlmap,map<string,void*> inParams,stSQLParam *sqlParam,map<string,stSQLParam*> &bindParams,char *sql_out)
{
	char flag_begin[3+1] = {"$L["};
	char flag_end[1+1] = {"]"};
	char name[123] = {0};
	char result[32] = {0};
	int f_num = 0;
	
	sprintf(name, "%s%s%s",flag_begin,sqlParam->key,flag_end);
	long len = strlen(sqlParam->key);
	for (map<string,void*>::iterator it = inParams.begin();it!=inParams.end();it++)
	{
		if (strncmp(sqlParam->key,(it->first).c_str(),len) == 0)
		{
			if (strcmp(result,"")==0)
			{
				strcpy(result,"?");
			}
			else
			{
				sprintf(result,"%s,?" ,result);
			}
			stSQLParam *param = new stSQLParam();
			strcpy(param->key,(it->first).c_str());
			param->name_type = 3;
			f_num ++;
			param->num = sqlParam->num + f_num -1;
			//strcpy(param->value,(it->second).c_str());
			param->values = it->second;
			param->isEmpty = 0;
			strcpy(param->type,"char");
			char ckey[32] = {0};
			sprintf(ckey,"%s:%d",param->key,param->num);
			bindParams.insert(pair<string,stSQLParam*>(string(ckey),param));

		}
	}
	xml_strrep(sqlmap->new_sql,sql_out,name,result);
	
	for (map<string,stSQLParam*>::iterator it =bindParams.begin();it!=bindParams.end();it++)
	{
		stSQLParam *param = it->second;
		if (param->num > sqlParam->num && param->name_type != 3 && param->name_type != 4)
		{
			param->num += (f_num - 1);
		}
	}
	
	return 0;
}

int ibear_getRowNum(char *result,char *rowname)
{
	char result_tmp[1024] = {0};
	int infunc = 0, res_num = 0, last = 0;
	strcpy(result_tmp,result);
	for (int i = 0;i < strlen(result_tmp); i++) {
		if (result_tmp[i] == '(')
		{
			infunc++;
		}
		else if (result_tmp[i] == ')' && infunc > 0)
		{
			infunc --;
		}
		else if (result_tmp[i] == ',' && infunc == 0)
		{
			char tmp[1024] = {0};
			if (last == 0)
			{
				strncpy(tmp,result_tmp+last,i-last);
			}
			else
			{
				strncpy(tmp,result_tmp+last+1,i-last-1);
			}
			last = i;
			if (strcasecmp(xmlTrim(tmp),rowname) == 0)
			{
				return res_num;
			}
			res_num++;
		}
		else if (i == strlen(result_tmp)-1)
		{
			char tmp[1024] = {0};
			if (res_num == 1)
			{
				strncpy(tmp,result_tmp+last,i-last);
			}
			else
			{
				strncpy(tmp,result_tmp+last+1,i-last-1);
			}
			last = i;
			if (strcasecmp(xmlTrim(tmp),rowname) == 0)
			{
				return res_num;
			}
		}
	}
	return -1;
}

int ibear_splitVector(char *src ,char splitchar,char *descs[10])
{
	int src_size = strlen(src);
	int num = 0;
	int last = 0;
	char *desc;
	for (int i = 0; i < src_size; i++)
	{
		if (src[i] == splitchar)
		{
			desc = (char *)malloc(10);
			strncpy(desc,src+last,i-last);
			descs[num] = desc;
			num++;
			last = i+1;
		}
		else if(i == src_size-1)
		{
			desc = (char *)malloc(10);
			strncpy(desc,src+last,i-last+1);
			descs[num] = desc;
		}
	}
	return num;
}

bool ibear_AllisNum(char *str)
{
	long len = strlen(str);
	if (len == 0)
	{
		return false;
	}
	for (int i = 0; i < len; i++)
	{
		if (str[i] == '-')
		{
			continue;
		}
		int tmp = (int)str[i];
		if (tmp >= 48 && tmp <= 57)
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool ibear_IsDate(char *bematch)
{
	char pattern[1024] = {0};
	strcpy(pattern,"^[0-9]{4}[-]?[0-9]{2}[-]?[0-9]{2}[ ]?[0-9]{2}[:]?[0-9]{2}[:]?[0-9]{2}|[0-9]{4}[-]?[0-9]{2}[-]?[0-9]{2}$");
	char errbuf[1024] = {0};
	char match[100] = {0};
	regex_t reg;
	int err,nm = 10;
	regmatch_t pmatch[nm];
	
	if(regcomp(&reg,pattern,REG_EXTENDED) < 0){
		regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
	}
	err = regexec(&reg,bematch,nm,pmatch,0);
	if(err == REG_NOMATCH)
	{
		printf("no match\n");
		regfree(&reg);
		return false;
	}
	else if(err)
	{
		regerror(err,&reg,errbuf,sizeof(errbuf));
		printf("err:%s\n",errbuf);
		regfree(&reg);
		return false;
	}
	regfree(&reg);
	return true;
}
