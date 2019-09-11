/**************************
 * 服务名称:ibear_db_pwd
 * 功能描述:公共文件
 * 程序版本:V1.0
 * 编码日期:2016/11/12
 * 编码人员:wangzhia
 **************************/

#include "ibear_db_pwd.h"

ibear_db_pwd::ibear_db_pwd()
{
	char dbcfg[1024] = {0};
	sprintf(dbcfg,"%s/cfg/db.cfg",getenv("DSCI_PATH"));
	pFile = fopen(dbcfg, "r");
	if (pFile == NULL)
	{
		printf("FILE打开失败!!!!\n");
		exit(1);
	}
	struct stat buf;
	stat(dbcfg, &buf);
	m_Len = buf.st_size;
	this->ibear_OpenFile();
}

ibear_db_pwd::~ibear_db_pwd()
{
	if(pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
}

int ibear_db_pwd::ibear_OpenFile()
{
	int	offset = 0;
	char	line[100*2];		//配置文件一行最大长度：MaxLineLen*2
	
	m_Content = (char *)malloc(sizeof(char)*m_Len+1);
	while(fgets(line, 100, pFile)!=NULL)
	{
		char	*pos = strchr(line, '#');
		if(pos == line)
			continue;
		else if(pos > line)
		{
			pos[0] = '\n';
			pos[1] = 0;
		}
		
		strcpy(m_Content+offset,line);
		offset += strlen(line);
	}
	
	return 0;
}

int ibear_db_pwd::ibear_SearchString(const char *Str,const int start,const int end)
{
	char *tmpptr;
	
	tmpptr=strstr(m_Content+start, Str);
	
	if(tmpptr==NULL||tmpptr > m_Content+end) return -1;
	
	return tmpptr-m_Content;
}

int ibear_db_pwd::ibear_SearchChar(char C, int start, int end)
{
	char *tmpptr;
	
	tmpptr=strchr(m_Content+start, C);
	if(tmpptr==NULL||tmpptr > m_Content+end) return -1;
	return tmpptr-m_Content;
}

int ibear_db_pwd::ibear_getConfigRecord(const char *Section, const char *KeyName,char *KeyVal)
{
	int pos,pos1;
	char	tmpSection[128];
	int	StrLen = strlen(Section);
	
	tmpSection[0] = '[';
	strcpy(tmpSection+1, Section);
	tmpSection[StrLen+1] = ']';
	tmpSection[StrLen+2] = 0;
	
	pos = ibear_SearchString(tmpSection,0,m_Len);
	if(pos != -1)
	{
		pos1 = ibear_SearchChar('[', pos+StrLen+2, m_Len);
		if(pos1 == -1)
			pos1 = m_Len;
		
		pos = ibear_SearchString(KeyName, pos, pos1);
		if(pos == -1)
			return -1;
		
		StrLen = strlen(KeyName);
		pos = ibear_SearchChar('=', pos+StrLen, pos1);
		if(pos == -1)
			return -1;
		pos++;
		pos1 = ibear_SearchChar('\n', pos, pos1);
		if(pos1 == -1)
			pos1 = m_Len;
		strncpy(KeyVal, m_Content+pos, pos1-pos);
		KeyVal[pos1-pos]=0;
		ibear_trim(KeyVal);
	}
	else
	{
		return -1;
	}
	return 0;
}

/*add by huanghl 去空格*/
char *ibear_db_pwd::ibear_trim(char *S)
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

int ibear_db_pwd::ibear_getDBNameKey(char *fileName,char *dbnameKey)
{
	char fileName_temp[1024] = {0};
	char dbnameKey_temp[1024] = {0};
	char *p,*p1;
	strcpy(fileName_temp,fileName);
	while (true)
	{
		p = strstr(fileName_temp,"/");
		if (p == NULL)
		{
			strcpy(dbnameKey_temp,fileName_temp);
			break;
		}
		memcpy(fileName_temp,p+1,strlen(fileName_temp));
	}
	p1 = strstr(dbnameKey_temp,".");
	memcpy(dbnameKey,dbnameKey_temp,strlen(fileName_temp)-strlen(p1));
	dbnameKey[strlen(fileName_temp)-strlen(p1)] = '\0';
	return 0;
}

int ibear_db_pwd::Custom_decryption(void (*getDBSection)(char *,stDBSection *),char *dbnameKey,stDBSection *dbsection)
{
	int ret = 0;
	char opstr[1024] = {0};
	char db_num[4+1] = {0};
	char logincfg[1024] = {0};
	char type[10] = {0};
	FILE *stream;
	
	for (int i= 1 ;i<100;i++)
	{
		if (i < 10)
		{
			sprintf(db_num,"DB0%d",i);
		}
		else
		{
			sprintf(db_num,"DB%d",i);
		}
		ret = this->ibear_getConfigRecord(db_num, "login", logincfg);
		ret |= this->ibear_getConfigRecord(db_num, "Type", type);
		if(ret != 0)
		{
			break;
		}
		printf("%s,%s\n",logincfg,type);
		if (strcmp(type,"dmdb") == 0)
		{
			//内存库暂不解析
			continue;
		}
		else
		{
			//Oracle解析
			ibear_getDBNameKey(logincfg,dbnameKey);
			getDBSection(dbnameKey,dbsection);
		}
		if (strcmp(dbsection->sDBName,"") == 0 || strcmp(dbsection->sDBUser,"") == 0 || strcmp(dbsection->sDBPwd,"") == 0)
		{
			printf("dbsection解析失败[%s][%s][%s]\n",dbsection->sDBName,dbsection->sDBUser,dbsection->sDBPwd);
			return -1;
		}
		char user_pwd[128] = {0};
		char ng_user_pwd[128] = {0};
		sprintf(ng_user_pwd,"%s %s",dbsection->sDBUser,dbsection->sDBPwd);
		sprintf(opstr,"${DSCI_PATH}/lib/dsci_tools -g ${DSCI_PATH}/cfg/%s.login",dbnameKey);
		stream=popen(opstr,"r");
		fread(user_pwd, sizeof(char), sizeof(user_pwd), stream);
		pclose(stream);
		if (strncmp(user_pwd,ng_user_pwd,strlen(ng_user_pwd)) == 0 && strlen(ng_user_pwd)+1 == strlen(user_pwd))
		{
			printf("一致[%s]\n",logincfg);
		}
		else
		{
			printf("%s|%s\n",ng_user_pwd,user_pwd);
			printf("不一致！！！\n");
			sprintf(opstr,"${DSCI_PATH}/lib/dsci_tools -p %s %s ${DSCI_PATH}/cfg/%s.login",dbsection->sDBUser,dbsection->sDBPwd,dbnameKey);
			system(opstr);
		}
	}
	return 0;
}

