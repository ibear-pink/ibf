
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsci_comm.h"
#include "dsci_inirw.h"
#include "dsci.h"
#include "dsci_db.h"

/*获取DSC接口的版本号*/
void DSCI_ver(char *version, char *r_date)
{
    strcpy(version, DSCI_VERSION_NUMBER);
    strcpy(r_date, DSCI_RELEASE_DATE);
    return;
}

/*读取错误代码*/
int DSCI_ecode(DSCI *pDSC)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    if(pDB != NULL)
        return pDB->eno;
    else
        return _ERR_NULL;
}

/*读取DSC错误信息*/
char *DSCI_error(DSCI *pDSC)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    if(pDB != NULL)
        return pDB->error;
    else
        return "数据库指针为空";
}

/*连接初始化*/
int DSCI_init(char *errStr, DSCI *pDSC)
{
    char dscPath[256];
    DSC_DB *pDB;

    *pDSC = NULL;
    if(NULL == (pDB = (DSC_DB *)_PDSC_MemMalloc(sizeof(DSC_DB))))
    {
        sprintf(errStr, "malloc 失败");
        return -1;
    }
    memset(pDB, 0, sizeof(DSC_DB));

    strcpy(dscPath, "${DSCI_PATH}");
    _PDSC_RepEnv(dscPath);

    /*读取数据库信息*/
    if(0 != _DSCI_GetDbInfo(errStr, dscPath, pDB))
    {
        if(pDB->dbInfo != NULL)
            _PDSC_MemFree(pDB->dbInfo);
        _PDSC_MemFree(pDB);
        return -1;
    }

    /*读取表信息*/
    if(0 != _DSCI_GetTabInfo(errStr, dscPath, pDB))
    {
        _PDSC_MemFree(pDB->dbInfo);
        if(pDB->tabInfo != NULL)
            _PDSC_MemFree(pDB->tabInfo);
        _PDSC_MemFree(pDB);
        return -1;
    }

    *pDSC = pDB;
    return 0;

}

/*连接初始化，直接生成数据库信息*/
int DSCI_init_direct(char *errStr, DSCI *pDSC, char *dbStr)
{
    DSC_DB *pDB;

    *pDSC = NULL;
    if(NULL == (pDB = (DSC_DB *)_PDSC_MemMalloc(sizeof(DSC_DB))))
    {
        sprintf(errStr, "malloc 失败");
        return -1;
    }
    memset(pDB, 0, sizeof(DSC_DB));

    /*直接生成数据库信息*/
    if(0 != _DSCI_CreateDbInfo(errStr, dbStr, pDB))
    {
        if(pDB->dbInfo != NULL)
            _PDSC_MemFree(pDB->dbInfo);
        _PDSC_MemFree(pDB);
        return -1;
    }

    *pDSC = pDB;

    if(0 != _CDSC_ConnIMDB(pDB->error, pDB, pDB->dbInfo))
    {
        strcpy(errStr, pDB->error);
        return -1;
    }

    return 0;

}

/*直接执行SQL命令(指定库序号)*/
int DSCI_query_f(DSCI *pDSC, const char *sqlStr, int dbNo)
{
    char sqlType;
    DB_INFO *db;
    DSC_DB *pDB;

    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return -1;
    if(0 != _DSCI_GetSqlDb(&db, pDB, sqlStr, &sqlType, dbNo))
        return -1;
    pDB->curSql = &(pDB->dSql);
    pDB->curSql->db = db;
    pDB->curSql->psql = NULL;
    pDB->curSql->type = sqlType;

    if(0 != _CDSC_Query(pDB, db, sqlStr))
    {
        if(pDB->eno > 0)
            db->cFlag = 0;
        return -1;
    }

    if(1 == sqlType)
        db->uFlag = 1;  /*此连接做过修改*/

    return 0;
}

/*直接执行SQL命令*/
int DSCI_query(DSCI *pDSC, const char *sqlStr)
{
    return DSCI_query_f(pDSC, sqlStr, 0);
}

/*预解析SQL命令(指定库序号)*/
int DSCI_prepare_sql_f(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr, int dbNo)
{
    char sqlType;
    DB_INFO *db;
    SQL_INFO *mSQL;
    DSC_DB *pDB;

    /*查找库*/
    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return -1;
    if(0 != _DSCI_GetSqlDb(&db, pDB, sqlStr, &sqlType, dbNo))
        return -1;

    if(*pSQL != NULL)
        _DSCI_free_sql(pSQL, db);

    if(NULL == (mSQL = (SQL_INFO *)_PDSC_MemMalloc(sizeof(SQL_INFO))))
    {
        sprintf(pDB->error, "malloc SQL_INFO 失败");
        pDB->eno = _ERR_SQLINFO_P;
        return -1;
    }
    memset(mSQL, 0, sizeof(SQL_INFO));
    *pSQL = mSQL;
    pDB->curSql = mSQL;
    pDB->curSql->type = sqlType;

    mSQL->db = db;
    if(0 != _CDSC_Prepare(pDB, db, mSQL, sqlStr))
    {
        _DSCI_free_sql(pSQL, db);
        if(pDB->eno > 0)
            db->cFlag = 0;
        return -1;
    }
    mSQL->connTime = db->connTime;

    return 0;
}
/*预解析SQL命令*/
int DSCI_prepare_sql(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr)
{
    return DSCI_prepare_sql_f(pDSC, pSQL, sqlStr, 0);
}

/*绑定参数*/
int DSCI_bind_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, char data_type, void *param)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    SQL_INFO *mSQL = (SQL_INFO *)(*pSQL);

    if(mSQL == NULL)
    {
        pDB->eno = _ERR_INVALID_SQL;
        sprintf(pDB->error, "invalid DSCI_SQL");
        return -1;
    }

    if(0 != _CDSC_Bind(pDB, mSQL->db, mSQL, param_id, data_type, param))
    {
        if(pDB->eno > 0)
            mSQL->db->cFlag = 0;
        return -1;
    }
    return 0;
}

/*绑定BLOB字段*/
int DSCI_bind_blob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param, int size)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    SQL_INFO *mSQL = (SQL_INFO *)(*pSQL);

    if(mSQL == NULL)
    {
        pDB->eno = _ERR_INVALID_SQL;
        sprintf(pDB->error, "invalid DSCI_SQL");
        return -1;
    }

    if(0 != _CDSC_Bind_Blob(pDB, mSQL->db, mSQL, param_id, param, size))
    {
        if(pDB->eno > 0)
            mSQL->db->cFlag = 0;
        return -1;
    }
    return 0;
}

/*绑定CLOB字段*/
int DSCI_bind_clob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    SQL_INFO *mSQL = (SQL_INFO *)(*pSQL);

    if(mSQL == NULL)
    {
        pDB->eno = _ERR_INVALID_SQL;
        sprintf(pDB->error, "invalid DSCI_SQL");
        return -1;
    }

    if(0 != _CDSC_Bind_Clob(pDB, mSQL->db, mSQL, param_id, param))
    {
        if(pDB->eno > 0)
            mSQL->db->cFlag = 0;
        return -1;
    }
    return 0;
}

/*获取LOB字段的字节长度*/
int DSCI_lob_size(char *pData)
{
    return _PDSC_uint4korr(pData);
}

/*获取LOB字段实际内容的指针*/
char *DSCI_lob_getp(char *pData)
{
    return pData+4;
}

/*获取LOB字段是否为CLOB*/
int DSCI_lob_isclob(char *p_DATA)
{
    if( (unsigned char)(p_DATA[3]) >= 128 )
        return 1;
    else
        return 0;
}

/*获取LOB字段，返回LOB字段长度*/
int DSCI_lob_get(void *outP, char *pData)
{
    int len = DSCI_lob_size(pData);
    memcpy(outP, DSCI_lob_getp(pData), (size_t)len);
    return len;
}

/*执行解析后的SQL命令*/
int DSCI_query_p(DSCI *pDSC, DSCI_SQL *pSQL)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    SQL_INFO *mSQL = (SQL_INFO *)(*pSQL);

    if(pDB == NULL)
        return -1;
    if(mSQL == NULL)
    {
        pDB->eno = _ERR_INVALID_SQL;
        sprintf(pDB->error, "invalid DSCI_SQL");
        return -1;
    }

    if(0 != _CDSC_Query_p(pDB, mSQL->db, mSQL))
    {
        if(pDB->eno > 0)
            mSQL->db->cFlag = 0;
        return -1;
    }
    pDB->curSql = mSQL;

    if(1 == mSQL->type)
        mSQL->db->uFlag = 1;  /*此连接做过修改*/
    return 0;
}

/*释放预解析的SQL*/
void DSCI_free_sql(DSCI_SQL *pSQL)
{
    if(NULL == *pSQL)
        return;
    _DSCI_free_sql(pSQL, ((SQL_INFO *)(*pSQL))->db);
    return;
}

/*0 提交,  1 回滚*/
int DSCI_end_trans(DSCI *pDSC, int flag)
{
    int i, j=0, ret;
    DSC_DB *pDB;
    DB_INFO *db;

    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return -1;
    for(i=0; i<pDB->dbNum; i++)
    {
        db = pDB->dbInfo+i;
        if(db->uFlag == 1)
        {
            ret = _CDSC_EndT(pDB, db, flag);
            if( (ret != 0) && (j == 0) )
                return -1;
            db->uFlag  = 0;
            j++;
        }
    }

    return 0;
}

/*0 提交,  1 回滚*/
int DSCI_end_trans_sig(DSCI *pDSC, int flag, int dbNum)
{
	int ret=0;
	DSC_DB *pDB;
	DB_INFO *db;
	
	pDB = (DSC_DB *)(*pDSC);
	if(pDB == NULL)
		return -1;
	db = pDB->dbInfo+dbNum-1;
	if(db->uFlag == 1)
	{
		ret = _CDSC_EndT(pDB, db, flag);
		if(ret != 0)
			return -1;
		db->uFlag  = 0;
	}
	
	return 0;
}

/*提取一行数据*/
DSCI_ROW DSCI_fetch_row(DSCI *pDSC)
{
    return _CDSC_Fetch(((DSC_DB *)(*pDSC))->curSql, 0);
}

/*提取一行数据， 根据指定的预解析SQL*/
DSCI_ROW DSCI_fetch_row_s(DSCI *pDSC, DSCI_SQL *pSQL)
{
    return _CDSC_Fetch((SQL_INFO *)(*pSQL), 1);
}

/*设置是否自动提交  0 非自动提交, 1 自动提交*/
int DSCI_set_auto_commit(DSCI *pDSC, int flag)
{
    int i;
    DSC_DB *pDB;
    DB_INFO *db;

    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return -1;

    for(i=0; i<pDB->dbNum; i++)
    {
        db = pDB->dbInfo+i;
        db->acommitFlag = flag;
        if(db->cFlag == 1)
            _CDSC_AutoCommit(db, flag);
    }

    return 0;
}

/*设置select缓冲模式 1 是， 0 否*/
void DSCI_set_Buffer_flag(DSCI *pDSC, int flag)
{
    int i;
    DSC_DB *pDB;
    DB_INFO *db;

    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return;

    for(i=0; i<pDB->dbNum; i++)
    {
        db = pDB->dbInfo+i;
        db->buffFlag = flag;
        if(db->cFlag == 1)
            _CDSC_SetBuffFlag(db, flag);
    }
    return;
}

/*设置日志模式, on 写日志，off 不写日志，sync 实时写磁盘*/
void DSCI_set_log_mode(DSCI *pDSC, const char *inMode)
{
    int i;
    DSC_DB *pDB;
    DB_INFO *db;

    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return;

    for(i=0; i<pDB->dbNum; i++)
    {
        db = pDB->dbInfo+i;
        strcpy(db->logMode, inMode);
        if(db->cFlag == 1)
            _CDSC_SetLogMode(db, inMode);
    }
    return;
}

/*操作记录数*/
long DSCI_num_rows(DSCI *pDSC)
{
    return ((DSC_DB *)(*pDSC))->ret_rows;
}

/*获取select字段个数*/
int DSCI_num_fields(DSCI *pDSC)
{
    return  ((DSC_DB *)(*pDSC))->field_count;
}

/*退出, 释放内存*/
void DSCI_quit(DSCI *pDSC)
{
    int i;
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    DB_INFO *db;

    if(pDB == NULL)
        return;

    DSCI_end_trans(pDSC, 1);   /*回滚*/

    /*释放DB*/
    if(pDB->dbInfo != NULL)
    {
        /*释放连接指针*/
        for(i=0; i<pDB->dbNum; i++)
        {
            db = pDB->dbInfo + i;
            _CDSC_FreeDB(db);
        }
        _PDSC_MemFree(pDB->dbInfo);
    }

    /*释放表信息*/
    if(pDB->tabInfo != NULL)
        _PDSC_MemFree(pDB->tabInfo);

    /*释放DSC*/
    _PDSC_MemFree(*pDSC);
    *pDSC = NULL;
    return;
}
