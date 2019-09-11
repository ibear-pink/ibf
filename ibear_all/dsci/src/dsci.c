
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsci_comm.h"
#include "dsci_inirw.h"
#include "dsci.h"
#include "dsci_db.h"

/*��ȡDSC�ӿڵİ汾��*/
void DSCI_ver(char *version, char *r_date)
{
    strcpy(version, DSCI_VERSION_NUMBER);
    strcpy(r_date, DSCI_RELEASE_DATE);
    return;
}

/*��ȡ�������*/
int DSCI_ecode(DSCI *pDSC)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    if(pDB != NULL)
        return pDB->eno;
    else
        return _ERR_NULL;
}

/*��ȡDSC������Ϣ*/
char *DSCI_error(DSCI *pDSC)
{
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    if(pDB != NULL)
        return pDB->error;
    else
        return "���ݿ�ָ��Ϊ��";
}

/*���ӳ�ʼ��*/
int DSCI_init(char *errStr, DSCI *pDSC)
{
    char dscPath[256];
    DSC_DB *pDB;

    *pDSC = NULL;
    if(NULL == (pDB = (DSC_DB *)_PDSC_MemMalloc(sizeof(DSC_DB))))
    {
        sprintf(errStr, "malloc ʧ��");
        return -1;
    }
    memset(pDB, 0, sizeof(DSC_DB));

    strcpy(dscPath, "${DSCI_PATH}");
    _PDSC_RepEnv(dscPath);

    /*��ȡ���ݿ���Ϣ*/
    if(0 != _DSCI_GetDbInfo(errStr, dscPath, pDB))
    {
        if(pDB->dbInfo != NULL)
            _PDSC_MemFree(pDB->dbInfo);
        _PDSC_MemFree(pDB);
        return -1;
    }

    /*��ȡ����Ϣ*/
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

/*���ӳ�ʼ����ֱ���������ݿ���Ϣ*/
int DSCI_init_direct(char *errStr, DSCI *pDSC, char *dbStr)
{
    DSC_DB *pDB;

    *pDSC = NULL;
    if(NULL == (pDB = (DSC_DB *)_PDSC_MemMalloc(sizeof(DSC_DB))))
    {
        sprintf(errStr, "malloc ʧ��");
        return -1;
    }
    memset(pDB, 0, sizeof(DSC_DB));

    /*ֱ���������ݿ���Ϣ*/
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

/*ֱ��ִ��SQL����(ָ�������)*/
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
        db->uFlag = 1;  /*�����������޸�*/

    return 0;
}

/*ֱ��ִ��SQL����*/
int DSCI_query(DSCI *pDSC, const char *sqlStr)
{
    return DSCI_query_f(pDSC, sqlStr, 0);
}

/*Ԥ����SQL����(ָ�������)*/
int DSCI_prepare_sql_f(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr, int dbNo)
{
    char sqlType;
    DB_INFO *db;
    SQL_INFO *mSQL;
    DSC_DB *pDB;

    /*���ҿ�*/
    pDB = (DSC_DB *)(*pDSC);
    if(pDB == NULL)
        return -1;
    if(0 != _DSCI_GetSqlDb(&db, pDB, sqlStr, &sqlType, dbNo))
        return -1;

    if(*pSQL != NULL)
        _DSCI_free_sql(pSQL, db);

    if(NULL == (mSQL = (SQL_INFO *)_PDSC_MemMalloc(sizeof(SQL_INFO))))
    {
        sprintf(pDB->error, "malloc SQL_INFO ʧ��");
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
/*Ԥ����SQL����*/
int DSCI_prepare_sql(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr)
{
    return DSCI_prepare_sql_f(pDSC, pSQL, sqlStr, 0);
}

/*�󶨲���*/
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

/*��BLOB�ֶ�*/
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

/*��CLOB�ֶ�*/
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

/*��ȡLOB�ֶε��ֽڳ���*/
int DSCI_lob_size(char *pData)
{
    return _PDSC_uint4korr(pData);
}

/*��ȡLOB�ֶ�ʵ�����ݵ�ָ��*/
char *DSCI_lob_getp(char *pData)
{
    return pData+4;
}

/*��ȡLOB�ֶ��Ƿ�ΪCLOB*/
int DSCI_lob_isclob(char *p_DATA)
{
    if( (unsigned char)(p_DATA[3]) >= 128 )
        return 1;
    else
        return 0;
}

/*��ȡLOB�ֶΣ�����LOB�ֶγ���*/
int DSCI_lob_get(void *outP, char *pData)
{
    int len = DSCI_lob_size(pData);
    memcpy(outP, DSCI_lob_getp(pData), (size_t)len);
    return len;
}

/*ִ�н������SQL����*/
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
        mSQL->db->uFlag = 1;  /*�����������޸�*/
    return 0;
}

/*�ͷ�Ԥ������SQL*/
void DSCI_free_sql(DSCI_SQL *pSQL)
{
    if(NULL == *pSQL)
        return;
    _DSCI_free_sql(pSQL, ((SQL_INFO *)(*pSQL))->db);
    return;
}

/*0 �ύ,  1 �ع�*/
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

/*0 �ύ,  1 �ع�*/
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

/*��ȡһ������*/
DSCI_ROW DSCI_fetch_row(DSCI *pDSC)
{
    return _CDSC_Fetch(((DSC_DB *)(*pDSC))->curSql, 0);
}

/*��ȡһ�����ݣ� ����ָ����Ԥ����SQL*/
DSCI_ROW DSCI_fetch_row_s(DSCI *pDSC, DSCI_SQL *pSQL)
{
    return _CDSC_Fetch((SQL_INFO *)(*pSQL), 1);
}

/*�����Ƿ��Զ��ύ  0 ���Զ��ύ, 1 �Զ��ύ*/
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

/*����select����ģʽ 1 �ǣ� 0 ��*/
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

/*������־ģʽ, on д��־��off ��д��־��sync ʵʱд����*/
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

/*������¼��*/
long DSCI_num_rows(DSCI *pDSC)
{
    return ((DSC_DB *)(*pDSC))->ret_rows;
}

/*��ȡselect�ֶθ���*/
int DSCI_num_fields(DSCI *pDSC)
{
    return  ((DSC_DB *)(*pDSC))->field_count;
}

/*�˳�, �ͷ��ڴ�*/
void DSCI_quit(DSCI *pDSC)
{
    int i;
    DSC_DB *pDB = (DSC_DB *)(*pDSC);
    DB_INFO *db;

    if(pDB == NULL)
        return;

    DSCI_end_trans(pDSC, 1);   /*�ع�*/

    /*�ͷ�DB*/
    if(pDB->dbInfo != NULL)
    {
        /*�ͷ�����ָ��*/
        for(i=0; i<pDB->dbNum; i++)
        {
            db = pDB->dbInfo + i;
            _CDSC_FreeDB(db);
        }
        _PDSC_MemFree(pDB->dbInfo);
    }

    /*�ͷű���Ϣ*/
    if(pDB->tabInfo != NULL)
        _PDSC_MemFree(pDB->tabInfo);

    /*�ͷ�DSC*/
    _PDSC_MemFree(*pDSC);
    *pDSC = NULL;
    return;
}
