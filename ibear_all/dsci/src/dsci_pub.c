
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsci_comm.h"
#include "dsci_inirw.h"
#include "dsci.h"
#include "dsci_db.h"

/*读取数据库类型*/
char _DSCI_GetDbType(char *errStr, char *typeStr)
{
    char type;

    if(0 == strcasecmp(typeStr, "imdb"))
    {
        type = _T_IMDB;
        #ifndef IMDB
            type = 0;
        #endif
    }
    else if(0 == strcasecmp(typeStr, "dmdb"))
    {
        type = _T_DMDB;
        #ifndef DMDB
            type = 0;
        #endif
    }
    else if(0 == strcasecmp(typeStr, "oracle"))
    {
        type = _T_ORACLE;
        #ifndef ORACLE
            type = 0;
        #endif
    }
    else if(0 == strcasecmp(typeStr, "altibase") )
    {
        type = _T_ALTIBASE;
        #ifndef ALTIBASE
            type = 0;
        #endif
    }
    else if(0 == strcasecmp(typeStr, "timesten") )
    {
        type = _T_ALTIBASE;
        #ifndef TIMESTEN
            type = 0;
        #endif
    }
    else if(0 == strcasecmp(typeStr, "mysql"))
    {
        type = _T_MYSQL;
        #ifndef MYSQLDB
            type = 0;
        #endif
    }
    else
    {
        sprintf(errStr, "invalid type[%s]", typeStr);
        return 0;
    }

    if(type == 0)
    {
        sprintf(errStr, "DSCI library not support %s, please check environment variable and re-compile.", typeStr);
    }
    return type;
}

/*直接生成数据库信息 user/pass@dbtype:dbserver*/
int _DSCI_CreateDbInfo(char *errStr, char *dbStr, DSC_DB *pDB)
{
    char *p1, *p2, *p3, tmpStr[512];

    strcpy(tmpStr, dbStr);
    if( (NULL == (p1 = strchr(tmpStr, '/'))) ||
        (NULL == (p2 = strchr(p1+1, '@'))) ||
        (NULL == (p3 = strchr(p2+1, ':'))) )
    {
        sprintf(errStr, "invalid dbStr, please use [user/pass@dbtype:dbserver]");
        return -1;
    }
    p1[0] = 0;   p2[0] = 0;  p3[0] = 0;

    pDB->dbNum = 1;
    pDB->dbInfo = (DB_INFO *)_PDSC_MemMalloc(sizeof(DB_INFO));
    memset(pDB->dbInfo, 0, sizeof(DB_INFO));
    pDB->defaultDB = 0;

    strcpy(pDB->dbInfo[0].name, "DB01");
    strcpy(pDB->dbInfo[0].user, tmpStr);
    strcpy(pDB->dbInfo[0].pass, p1+1);
    strcpy(pDB->dbInfo[0].server, p3+1);

    if(0 == (pDB->dbInfo[0].type = _DSCI_GetDbType(errStr, p2+1)))
        return -1;

    pDB->dbInfo[0].acommitFlag = 0;
    pDB->dbInfo[0].buffFlag = 0;
    strcpy(pDB->dbInfo[0].logMode, "on");

    pDB->tabNum = 0;
    return 0;
}

/*读取数据库信息*/
int _DSCI_GetDbInfo(char *errStr, char *dscPath, DSC_DB *pDB)
{
    int i;
    char tmpFile[256], tmpStr[512], dbStr[100][5], defStr[16];
    FILE *fp;

    sprintf(tmpFile, "%s/cfg/db.cfg", dscPath);
    if(NULL == (fp  = fopen(tmpFile, "r")))
    {
        sprintf(errStr, "打开文件 %s 失败", tmpFile);
        return -1;
    }
    if(0 >= _INIDSC_Read( errStr, fp, "SYS", "DEFAULT_DB", defStr ))
    {
        fclose(fp);
        return -1;
    }

    i = 0;
    while(0 < _INIDSC_GetSection(fp, tmpStr))
    {
        if( (0 != strncasecmp(tmpStr, "DB", 2)) || (strlen(tmpStr) != 4) )
            continue;

        strcpy(dbStr[i++], tmpStr);
    }
    pDB->dbNum = i;
    if(pDB->dbNum == 0)
    {
        sprintf(errStr, "数据库个数不能为0");
        fclose(fp);
        return -1;
    }
    pDB->dbInfo = (DB_INFO *)_PDSC_MemMalloc(pDB->dbNum * sizeof(DB_INFO));
    memset(pDB->dbInfo, 0, pDB->dbNum * sizeof(DB_INFO));
    pDB->defaultDB = -1;
    for(i=0; i<pDB->dbNum; i++)
    {
        strcpy(pDB->dbInfo[i].name, dbStr[i]);
        if( (0 >= _INIDSC_Read( errStr, fp, dbStr[i], "login", tmpFile )) ||
            (0 >= _INIDSC_Read( errStr, fp, dbStr[i], "Server", pDB->dbInfo[i].server ) ) ||
            (0 >= _INIDSC_Read( errStr, fp, dbStr[i], "Type", tmpStr ) ) )
        {
            fclose(fp);
            return -1;
        }

        if(0 == (pDB->dbInfo[i].type = _DSCI_GetDbType(errStr, tmpStr)))
        {
            fclose(fp);
            return -1;
        }

        _PDSC_RepEnv(tmpFile);
        if(0 != _PDSC_GetLoginInfo(tmpFile, pDB->dbInfo[i].user, pDB->dbInfo[i].pass))
        {
            sprintf(errStr, "读取密码文件[%s]失败", tmpFile);
            fclose(fp);
            return -1;
        }

        pDB->dbInfo[i].acommitFlag = 0;
        pDB->dbInfo[i].buffFlag = 0;
        strcpy(pDB->dbInfo[i].logMode, "on");

        if(0 == strcasecmp(defStr, dbStr[i]))
            pDB->defaultDB = i;
    }
    fclose(fp);

    if(pDB->defaultDB == -1)
    {
        sprintf(errStr, "请指定默认数据库");
        return -1;
    }
    return 0;
}

/*读取表信息*/
int _DSCI_GetTabInfo(char *errStr, char *dscPath, DSC_DB *pDB)
{
    int i, j, flag;
    char tmpFile[256], tmpStr[512];
    FILE *fp;

    sprintf(tmpFile, "%s/cfg/table.cfg", dscPath);
    if(NULL == (fp  = fopen(tmpFile, "r")))
    {
        sprintf(errStr, "打开文件 %s 失败", tmpFile);
        return -1;
    }

    i = 0;   flag = 0;
    while(0 < _INIDSC_GetSectionKey(fp, "TAB_DB", &flag, tmpStr, tmpStr+200))
        i++;
    pDB->tabNum = i;
    if(pDB->tabNum == 0)
    {
        fclose(fp);
        return 0;
    }

    pDB->tabInfo = (TAB_INFO *)_PDSC_MemMalloc(pDB->tabNum * sizeof(TAB_INFO));
    memset(pDB->tabInfo, 0, pDB->tabNum * sizeof(TAB_INFO));
    i = 0;   flag = 0;
    fseek(fp, 0L, SEEK_SET);
    while(0 < _INIDSC_GetSectionKey(fp, "TAB_DB", &flag, pDB->tabInfo[i].name, tmpStr))
    {
        pDB->tabInfo[i].dbID = -1;
        for(j=0; j<pDB->dbNum; j++)
        {
            if(0 == strcasecmp(pDB->dbInfo[j].name, tmpStr))
            {
                pDB->tabInfo[i].dbID = j;
                break;
            }
        }
        if(pDB->tabInfo[i].dbID == -1)
        {
            sprintf(errStr, "指定的数据库不存在(%s)", tmpStr);
            fclose(fp);
            return -1;
        }
        i++;
    }
    fclose(fp);

    return 0;
}

/*从字符串中取出表名*/
void _DSCI_GetTabName(char *inStr, char *tabName, char seg)
{
    int i;
    while(*inStr == ' ')
        inStr++;
    for(i=0; i<40; i++)
    {
        if( (inStr[i] != ' ') && (inStr[i] != seg) &&(inStr[i] != 0) )
            tabName[i] = inStr[i];
        else
        {
            tabName[i] = 0;
            break;
        }
    }
    tabName[i] = 0;
    return;
}

/*根据表名获取数据库编号
select sd f from ssss,  逗号或者空格
update aas set
delete from sdsad, 逗号或者空格
insert into sadsa (   空格或者左括号
*/
short _DSCI_GetDBNo(const char *sqlStr, DSC_DB *pDB, char *type, int dbNo)
{
    int i, len;
    char *pSql, tabName[40], flag;

    pSql = (char *)sqlStr;
    while(*pSql == ' ')
        pSql++;

    tabName[0] = 0;
    *type = 1;

    /*获取表名*/
    if(0 == strncasecmp(pSql, "select ", 7))
    {
        pSql += 7;
        len = strlen(pSql) - 6;
        flag = 0;
        for(i=0; i<len; i++)
        {
            if(pSql[i] == '\'')
                flag = 1 - flag;

            if((flag == 0) && (0 == strncasecmp(pSql+i, " from ", 6)) )
            {
                pSql = pSql + i + 6;
                break;
            }
        }
        if(i == len)
            return -1;
        _DSCI_GetTabName(pSql, tabName, ',');

        /*检查是否为for update*/
        pSql = (char *)sqlStr + (strlen(sqlStr) - 1);
        while(*pSql == ' ')
            pSql--;
        if(0 != strncasecmp(pSql-6, " update", 7))
            *type = 0;
    }
    else if(0 == strncasecmp(pSql, "update ", 7))
    {
        _DSCI_GetTabName(pSql+7, tabName, ',');
    }
    else if(0 == strncasecmp(pSql, "insert ", 7))
    {
        pSql += 7;
        while(*pSql == ' ')
            pSql++;
        if(0 != strncasecmp(pSql, "into ", 5))
            return -1;
        _DSCI_GetTabName(pSql+5, tabName, '(');
    }
    else if(0 == strncasecmp(pSql, "delete ", 7))
    {
        pSql += 7;
        while(*pSql == ' ')
            pSql++;
        if(0 == strncasecmp(pSql, "from ", 5))
            _DSCI_GetTabName(pSql+5, tabName, ',');
        else
            _DSCI_GetTabName(pSql, tabName, ',');
    }
    else if(0 == strncasecmp(pSql, "truncate ", 9))
    {
        pSql += 9;
        while(*pSql == ' ')
            pSql++;
        if(0 == strncasecmp(pSql, "table ", 6))
            _DSCI_GetTabName(pSql+6, tabName, ',');
        else
            return -1;
    }
    else
    {
        return -1;
    }

    if(dbNo <= 0)
    {
        /*根据表名求DB*/
        for(i=0; i<pDB->tabNum; i++)
        {
            if(0 == strcasecmp(pDB->tabInfo[i].name, tabName))
                return pDB->tabInfo[i].dbID;
        }
        return pDB->defaultDB;
    }
    else
        return dbNo - 1;
}

/*根据SQL查找DB, 并连接*/
int _DSCI_GetSqlDb(DB_INFO **db, DSC_DB *pDB, const char *sqlStr, char *sqlType, int dbNo)
{
    if(dbNo <= pDB->dbNum)
    {
        int i;
        if(-1 == (i = _DSCI_GetDBNo(sqlStr, pDB, sqlType, dbNo)))
        {
            sprintf(pDB->error, "不能根据表名获取数据库ID");
            pDB->eno = _ERR_DB_ID1;
            return -1;
        }
        *db = pDB->dbInfo + i;
    }
    else
    {
        sprintf(pDB->error, "数据库编号[%d]非法", dbNo);
        pDB->eno = _ERR_DB_ID2;
        return -1;
    }

    /*判断是否已经连接*/
    if((*db)->cFlag == 0)
    {
        if(0 != _CDSC_ConnIMDB(pDB->error, pDB, *db))
            return -1;
    }

    return 0;
}

/*释放预解析的SQL*/
void _DSCI_free_sql(DSCI_SQL *pSQL, DB_INFO *db)
{
    SQL_INFO *mSQL = (SQL_INFO *)(*pSQL);

    if(*pSQL != NULL)
    {
        _CDSC_FreeSql(mSQL, db);
        _PDSC_MemFree(*pSQL);
        *pSQL = NULL;
    }
    return;
}
