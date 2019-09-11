

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dsci_comm.h"
#include "dsci_inirw.h"
#include "dsci.h"
#include "dsci_db.h"

/*连接库******************************************************/
int _CDSC_ConnIMDB(char *errStr, DSC_DB *pDB, DB_INFO *db)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(db->dbp == NULL)
            {
                if(NULL == (db->dbp = _PDSC_MemMalloc(sizeof(SI_DCI))))
                {
                    sprintf(errStr, "malloc SI_DCI 失败");
                    pDB->eno = _ERR_DB_P;
                    return -1;
                }
                memset(db->dbp, 0, sizeof(SI_DCI));
                if(NULL == si_dci_init((SI_DCI *)(db->dbp)))
                {
                    sprintf(errStr, "dci_init 失败");
                    pDB->eno = _ERR_DCI_INIT;
                    return -1;
                }
            }
            if(NULL == si_dci_real_connect((SI_DCI *)(db->dbp), NULL, db->user, db->pass, db->server, 0))
            {
                sprintf(errStr, "%s", ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            break;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(db->dbp == NULL)
            {
                if(NULL == (db->dbp = _PDSC_MemMalloc(sizeof(DDCI))))
                {
                    sprintf(errStr, "malloc DSC 失败");
                    pDB->eno = _ERR_DB_P;
                    return -1;
                }
                memset(db->dbp, 0, sizeof(DDCI));
                if(0 != ddci_init(errStr, (DDCI *)(db->dbp)))
                {
                    pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                    return -1;
                }
            }
            if(NULL == ddci_real_connect((DDCI *)(db->dbp), NULL, db->user, db->pass, db->server, 0))
            {
                sprintf(errStr, "%s", ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            break;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(db->dbp == NULL)
            {
                if(NULL == (db->dbp = _PDSC_MemMalloc(sizeof(ORA_SESSION))))
                {
                    pDB->eno = _ERR_DB_P;
                    sprintf(errStr, "malloc ORA_SESSION 失败");
                    return -1;
                }
                memset(db->dbp, 0, sizeof(ORA_SESSION));
            }
            if(0 != _ORA_Connect(errStr, (ORA_SESSION *)(db->dbp), db->user, db->pass, db->server))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            break;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(db->dbp == NULL)
            {
                if(NULL == (db->dbp = _PDSC_MemMalloc(sizeof(O_SESSION))))
                {
                    pDB->eno = _ERR_DB_P;
                    sprintf(errStr, "malloc O_SESSION 失败");
                    return -1;
                }
                memset(db->dbp, 0, sizeof(O_SESSION));
            }
            if(0 != _O_Connect(errStr, (O_SESSION *)(db->dbp), db->user, db->pass, db->server))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            break;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(db->dbp == NULL)
            {
                if(NULL == (db->dbp = _PDSC_MemMalloc(sizeof(MY_SESSION))))
                {
                    pDB->eno = _ERR_DB_P;
                    sprintf(errStr, "malloc MY_SESSION 失败");
                    return -1;
                }
                memset(db->dbp, 0, sizeof(MY_SESSION));
            }
            if(0 != _MY_Connect(errStr, (MY_SESSION *)(db->dbp), db->user, db->pass, db->server))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;;
                return -1;
            }
            break;
      #endif
    }

    if(db->acommitFlag == 1)  /*有问题，如果mdb默认是字段提交，就无法设置成非自动提交了*/
        _CDSC_AutoCommit(db, 1);

    if(db->buffFlag == 1)
        _CDSC_SetBuffFlag(db, 1);

    if(strcmp(db->logMode, "on") != 0)
        _CDSC_SetLogMode(db, db->logMode);

    db->cFlag = 1;
    db->uFlag = 0;
    time(&(db->connTime));
    return 0;
}

/*直接查询*/
int _CDSC_Query(DSC_DB *pDB, DB_INFO *db, const char *sqlStr)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_query((SI_DCI *)(db->dbp), sqlStr))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            pDB->ret_rows = si_dci_num_rows((SI_DCI *)(db->dbp));
            pDB->field_count = si_dci_num_fields((SI_DCI *)(db->dbp));
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_query((DDCI *)(db->dbp), sqlStr))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            pDB->ret_rows = ddci_num_rows((DDCI *)(db->dbp));
            pDB->field_count = ddci_num_fields((DDCI *)(db->dbp));
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(0 != _ORA_ExecSql(pDB->error, (ORA_SESSION *)(db->dbp), sqlStr))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }

            pDB->ret_rows = ((ORA_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((ORA_SESSION *)(db->dbp))->tsql.field_count;
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 != _O_ExecSql(pDB->error, (O_SESSION *)(db->dbp), sqlStr))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }

            pDB->ret_rows = ((O_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((O_SESSION *)(db->dbp))->field_count;
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(0 != _MY_ExecSql(pDB->error, (MY_SESSION *)(db->dbp), sqlStr))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
                return -1;
            }

            pDB->ret_rows = ((MY_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((MY_SESSION *)(db->dbp))->field_count;
            return 0;
      #endif
    }
    return 0;
}

/*预解析*/
int _CDSC_Prepare(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, const char *sqlStr)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(NULL == (mSQL->psql = _PDSC_MemMalloc(sizeof(SI_DCI_SQL))))
            {
                sprintf(pDB->error, "malloc MSI_DCI_SQL 失败");
                pDB->eno = _ERR_SQL_P;
                return -1;
            }
            memset(mSQL->psql, 0, sizeof(SI_DCI_SQL));

            if(0 != si_dci_prepare_sql((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql), sqlStr))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(NULL == (mSQL->psql = _PDSC_MemMalloc(sizeof(DDCI_SQL))))
            {
                sprintf(pDB->error, "malloc DSC_SQL 失败");
                pDB->eno = _ERR_SQL_P;
                return -1;
            }
            memset(mSQL->psql, 0, sizeof(DDCI_SQL));

            if(0 != ddci_prepare_sql((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql), sqlStr))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(NULL == (mSQL->psql = (ORA_SQL *)_PDSC_MemMalloc(sizeof(ORA_SQL))))
            {
                sprintf(pDB->error, "malloc ORA_SQL 失败");
                pDB->eno = _ERR_SQL_P;
                return -1;
            }
            memset(mSQL->psql, 0, sizeof(ORA_SQL));

            if(0 != _ORA_PrepareSql(pDB->error, (ORA_SESSION *)(db->dbp), (ORA_SQL *)(mSQL->psql), sqlStr))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(NULL == (mSQL->psql = (O_SQL *)_PDSC_MemMalloc(sizeof(O_SQL))))
            {
                sprintf(pDB->error, "malloc O_SQL 失败");
                pDB->eno = _ERR_SQL_P;
                return -1;
            }
            memset(mSQL->psql, 0, sizeof(O_SQL));

            if(0 != _O_PrepareSql(pDB->error, (O_SESSION *)(db->dbp), (O_SQL *)(mSQL->psql), sqlStr))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(NULL == (mSQL->psql = (MY_SQL *)_PDSC_MemMalloc(sizeof(MY_SQL))))
            {
                sprintf(pDB->error, "malloc MY_SQL 失败");
                pDB->eno = _ERR_SQL_P;
                return -1;
            }
            memset(mSQL->psql, 0, sizeof(MY_SQL));

            if(0 != _MY_PrepareSql(pDB->error, (MY_SESSION *)(db->dbp), (MY_SQL *)(mSQL->psql), sqlStr))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
    }
    return 0;
}

/*绑定参数*/
int _CDSC_Bind(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, char data_type, void *param)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_bind_param((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql), param_id, data_type, param))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_bind_param((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql), param_id, data_type, param))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(0 != _ORA_BindParam(pDB->error, (ORA_SESSION *)(db->dbp), (ORA_SQL *)(mSQL->psql), param_id, data_type, param))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 != _O_BindParam(pDB->error, (O_SESSION *)(db->dbp), (O_SQL *)(mSQL->psql), param_id, data_type, param))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(0 != _My_BindParam(pDB->error, (MY_SESSION *)(db->dbp), (MY_SQL *)(mSQL->psql), param_id, data_type, param))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
    }
    return 0;
}

/*绑定BLOB参数*/
int _CDSC_Bind_Blob(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, void *param, int size)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_bind_blob_param((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql), param_id, param, size))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_bind_blob_param((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql), param_id, param, size))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(0 != _ORA_BindBlobParam(pDB->error, (ORA_SESSION *)(db->dbp), (ORA_SQL *)(mSQL->psql), param_id, param, size))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 != _O_BindBlobParam(pDB->error, (O_SESSION *)(db->dbp), (O_SQL *)(mSQL->psql), param_id, param, size))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(0 != _MY_BindBlobParam(pDB->error, (MY_SESSION *)(db->dbp), (MY_SQL *)(mSQL->psql), param_id, param, size))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
               return -1;
            }
            return 0;
      #endif
    }
    return 0;
}

/*绑定CLOB参数*/
int _CDSC_Bind_Clob(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, void *param)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_bind_clob_param((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql), param_id, param))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_bind_clob_param((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql), param_id, param))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            return 0;
      #endif      
    }
    return 0;
}

/*执行预解析SQL*/
int _CDSC_Query_p(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_query_p((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql)))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            pDB->ret_rows = si_dci_num_rows((SI_DCI *)(db->dbp));
            pDB->field_count = si_dci_num_fields((SI_DCI *)(db->dbp));
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_query_p((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql)))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            pDB->ret_rows = ddci_num_rows((DDCI *)(db->dbp));
            pDB->field_count = ddci_num_fields((DDCI *)(db->dbp));
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(0 != _ORA_ExecSql_P(pDB->error, (ORA_SESSION *)(db->dbp), (ORA_SQL *)(mSQL->psql)))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            pDB->ret_rows = ((ORA_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((ORA_SQL *)(mSQL->psql))->tsql.field_count;
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 != _O_ExecSql_P(pDB->error, (O_SESSION *)(db->dbp), (O_SQL *)(mSQL->psql)))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            pDB->ret_rows = ((O_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((O_SQL *)(mSQL->psql))->field_count;
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(0 != _MY_ExecSql_P(pDB->error, (MY_SESSION *)(db->dbp), (MY_SQL *)(mSQL->psql)))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
                return -1;
            }
            pDB->ret_rows = ((MY_SESSION *)(db->dbp))->ret_rows;
            pDB->field_count = ((MY_SQL *)(mSQL->psql))->field_count;
            return 0;
      #endif
    }
    return 0;
}

/*释放预解析SQL*/
void _CDSC_FreeSql(SQL_INFO *mSQL, DB_INFO *db)
{
    if(NULL != mSQL->psql)
    {
        switch(db->type)
        {
          #ifdef IMDB
            case _T_IMDB:
                if(mSQL->connTime == db->connTime)
                    si_dci_free_sql((SI_DCI_SQL *)(mSQL->psql));
                break;
          #endif
          #ifdef DMDB
            case _T_DMDB:
                if(mSQL->connTime == db->connTime)
                    ddci_free_sql((DDCI_SQL *)(mSQL->psql));
                break;
          #endif
          #ifdef ORACLE
            case _T_ORACLE:
                _ORA_FreeSql((ORA_SQL *)(mSQL->psql));
                break;
          #endif
          #ifdef ODBC
            case _T_ALTIBASE:
                _O_FreeSql((O_SQL *)(mSQL->psql), (mSQL->connTime == db->connTime)?1:0);
                break;
          #endif
          #ifdef MYSQLDB
            case _T_MYSQL:
                _MY_FreeSql((MY_SQL *)(mSQL->psql));
                break;
          #endif
        }
        _PDSC_MemFree(mSQL->psql);
    }
    return;
}

/*提交*/
int _CDSC_EndT(DSC_DB *pDB, DB_INFO *db, int flag)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 != si_dci_end_trans((SI_DCI *)(db->dbp), (unsigned short)flag))
            {
                strcpy(pDB->error, ((SI_DCI *)(db->dbp))->error);
                pDB->eno = ((SI_DCI *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 != ddci_end_trans((DDCI *)(db->dbp), (unsigned short)flag))
            {
                strcpy(pDB->error, ddci_error((DDCI *)(db->dbp)));
                pDB->eno = ddci_ecode((DDCI *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            if(0 != _ORA_EndTrans(pDB->error, (ORA_SESSION *)(db->dbp), flag))
            {
                pDB->eno = _ORA_GetErrCode((ORA_SESSION *)(db->dbp));
                return -1;
            }
            return 0;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 != _O_EndTrans(pDB->error, (O_SESSION *)(db->dbp), flag))
            {
                pDB->eno = ((O_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            if(0 != _MY_EndTrans(pDB->error, (MY_SESSION *)(db->dbp), flag))
            {
                pDB->eno = ((MY_SESSION *)(db->dbp))->eno;
                return -1;
            }
            return 0;
      #endif
    }
    return 0;
}

/*fetch*/
DSCI_ROW _CDSC_Fetch(SQL_INFO *mSQL, int flag)
{
    DB_INFO *db = mSQL->db;
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(0 == flag)
                return si_dci_fetch_row((SI_DCI *)(db->dbp));
            else
                return si_dci_fetch_row_s((SI_DCI *)(db->dbp), (SI_DCI_SQL *)(mSQL->psql));
      #endif
      #ifdef DMDB
        case _T_DMDB:
            if(0 == flag)
                return ddci_fetch_row((DDCI *)(db->dbp));
            else
                return ddci_fetch_row_s((DDCI *)(db->dbp), (DDCI_SQL *)(mSQL->psql));
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            return _ORA_FetchData((ORA_SESSION *)(db->dbp), (ORA_SQL *)(mSQL->psql));
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            if(0 == flag)
                return _O_FetchData((O_SESSION *)(db->dbp), ((O_SESSION *)(db->dbp))->pSQL);
            else
                return _O_FetchData((O_SESSION *)(db->dbp), (O_SQL *)(mSQL->psql));
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            return _MY_FetchData((MY_SESSION *)(db->dbp), (MY_SQL *)(mSQL->psql));
      #endif
    }
    return NULL;
}

/*设置自动提交*/
void _CDSC_AutoCommit(DB_INFO *db, int flag)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            si_dci_set_auto_commit((SI_DCI *)(db->dbp), flag);
            break;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            ddci_set_auto_commit((DDCI *)(db->dbp), flag);
            break;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            _ORA_SetAutoCommit((ORA_SESSION *)(db->dbp), flag);
            break;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            _O_SetAutoCommit((O_SESSION *)(db->dbp), flag);
            break;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            _MY_SetAutoCommit((MY_SESSION *)(db->dbp), flag);
            break;
      #endif
    }
}

/*设置select缓冲模式 1 是， 0 否*/
void _CDSC_SetBuffFlag(DB_INFO *db, int flag)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            if(flag == 1)
                si_dci_set_Buffer_flag((SI_DCI *)(db->dbp));
            else
                si_dci_set_unBuffer_flag((SI_DCI *)(db->dbp));
            break;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            ddci_set_Buffer_flag((DDCI *)(db->dbp), flag);
            break;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            _ORA_SetBuffFlag((ORA_SESSION *)(db->dbp), flag);
            break;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            _O_SetBuffFlag((O_SESSION *)(db->dbp), flag);
            break;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            _MY_SetBuffFlag((MY_SESSION *)(db->dbp), flag);
            break;
      #endif
    }
    return;
}

/*设置日志模式, on 写日志，off 不写日志，sync 实时写磁盘*/
void _CDSC_SetLogMode(DB_INFO *db, const char *inMode)
{
    switch(db->type)
    {
      #ifdef IMDB
        case _T_IMDB:
            si_dci_set_log_mode((SI_DCI *)(db->dbp), inMode);
            break;
      #endif
      #ifdef DMDB
        case _T_DMDB:
            ddci_set_log_mode((DDCI *)(db->dbp), inMode);
            break;
      #endif
      #ifdef ORACLE
        case _T_ORACLE:
            break;
      #endif
      #ifdef ODBC
        case _T_ALTIBASE:
            break;
      #endif
      #ifdef MYSQLDB
        case _T_MYSQL:
            break;
      #endif
    }
    return;
}

/*退出连接，释放内存*/
void _CDSC_FreeDB(DB_INFO *db)
{
    if(db->dbp != NULL)
    {
        if(db->cFlag == 1)
        {
            switch(db->type)
            {
              #ifdef IMDB
                case _T_IMDB:      si_dci_quit((SI_DCI *)(db->dbp));        break;
              #endif
              #ifdef DMDB
                case _T_DMDB:      ddci_quit((DDCI *)(db->dbp));        break;
              #endif
              #ifdef ORACLE
                case _T_ORACLE:    _ORA_Disconnect((ORA_SESSION *)(db->dbp));    break;
              #endif
              #ifdef ODBC
                case _T_ALTIBASE:  _O_Disconnect((O_SESSION *)(db->dbp));      break;
              #endif
              #ifdef MYSQLDB
                case _T_MYSQL:     _MY_Disconnect((MY_SESSION *)(db->dbp));     break;
              #endif
            }
        }
        _PDSC_MemFree(db->dbp);
    }
}
