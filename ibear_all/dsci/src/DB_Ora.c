
/*!
 *  @file Database.c ����ORACLE���ݿ�����Ӽ������ݿ�ĸ��ֲ���
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2002-12-05
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dsci_comm.h"
#include "DB_Ora.h"

/***********************************************************
 * Function: ��ȡ���ݿ�������.
 * Parameter:
 *      [I]errhp               :������
 * Returns:
 *      ������� >0 ��ʾOracle����
 **********************************************************/
int _ORA_GetErrCode(ORA_SESSION *DBSession)
{
    char errBuf[512];
    sb4 errcode = 0;

    (void) OCIErrorGet(DBSession->errhp, (ub4) 1, (text *) NULL, &errcode,
               (text *)errBuf, (ub4) sizeof(errBuf), OCI_HTYPE_ERROR);

    switch(errcode)
    {
        case 28:    /*�Ự����ֹ*/
        case 1012:  /*û�е�¼*/
        case 1041:  /*internal error. HOSTDEF extension does not exist*/
        case 3113:  /*end-of-file on communication channel */
        case 3114:  /*not connected to Oracle*/
        case 12571: /*TNS:packet writer failure*/
        case 12560: /*TNS:Э�����*/
            return errcode;

        case 1:
            return -2100;

        case 2100:
            return -2101;

        default:
            return 0 - abs(errcode);
    }
}

/*****************************************************************/
/*!
    * @brief �ж����ݿ����
    * @param [out] errStr        ���������ַ���
    * @param [in]  errhp         ������
    * @param [in]  status        ����״̬
    * @return 0���޴���-1 ����
    */
int _ORA_CheckErr( char *errStr, OCIError *errhp, sword status )
{
    if(status == OCI_SUCCESS)
        return ORA_SUCCESS;

    switch (status)
    {
        case OCI_SUCCESS_WITH_INFO:  /*1*/
            /*strcpy(errStr, "OCI_SUCCESS_WITH_INFO");
            break;*/
            return ORA_SUCCESS;
        case OCI_NEED_DATA:   /*99*/
            if(errStr != NULL)
                strcpy(errStr, "OCI_NEED_DATA");
            break;
        case OCI_NO_DATA:   /*100*/
            if(errStr != NULL)
                strcpy(errStr, "OCI_NO_DATA");
            return ORA_NO_DATA_FOUND;

        case OCI_ERROR:   /*-1*/
            {
                char errBuf[512], *p;
                sb4 errcode = 0;
                (void) OCIErrorGet((dvoid *)errhp, (ub4) 1, (text *) NULL, &errcode,
                                    (text *)errBuf, (ub4) sizeof(errBuf), OCI_HTYPE_ERROR);
                if(1405 == errcode)
                    return ORA_SUCCESS;   /*��Ϊ��ֵʱ���Ǵ���*/

                errBuf[100]='\0';
                if(NULL != (p = strchr(errBuf, '\n')))
                    p[0] = 0;
                if(errStr != NULL)
                    strcpy(errStr, errBuf);
                break;
            }
        case OCI_INVALID_HANDLE:   /*-2*/
            if(errStr != NULL)
                strcpy(errStr, "OCI_INVALID_HANDLE");
            break;
        case OCI_STILL_EXECUTING:   /*-3123*/
            if(errStr != NULL)
                strcpy(errStr, "OCI_STILL_EXECUTING");
            break;
        case OCI_CONTINUE:   /*-24200*/
            if(errStr != NULL)
                strcpy(errStr, "OCI_CONTINUE");
            break;
        default:
            if(errStr != NULL)
                strcpy(errStr, "unknown error");
    }
    return ORA_FAILURE;
}

/*****************************************************************/
/*!
    * @brief ��¼���ݿ�
    * @param [out] errStr        ������Ϣ����
    * @param [in]  DBSession     ORACLE����ָ��
    * @param [in]  user          �û���
    * @param [in]  password      ����
    * @param [in]  server        ���ݿ����Ӵ�
    * @return 0 �ɹ� -1 ʧ��
    */
int _ORA_Connect(char *errStr, ORA_SESSION *DBSession,
                  char *user, char *password, char *server)
{
    memset( DBSession, 0, sizeof(ORA_SESSION) );
         /*����OCI����*/
    if( OCIInitialize((ub4) OCI_THREADED|OCI_OBJECT, (dvoid *)0,
                    (dvoid * (*)(dvoid *, size_t)) 0,
                    (dvoid * (*)(dvoid *, dvoid *, size_t))0,
                    (void (*)(dvoid *, dvoid *)) 0 ))
    {
        strcpy(errStr, "FAILED:OCIInitialize");
        return OCI_ERROR;
    }

    if( OCIEnvInit( (OCIEnv **) &(DBSession->envhp), OCI_DEFAULT, (size_t) 0, (dvoid **) 0 ))
    {
        strcpy(errStr, "FAILED:OCIEnvInit");
        return OCI_ERROR;
    }
         /*���������*/
    if( OCIHandleAlloc( (dvoid *) (DBSession->envhp), (dvoid **) &(DBSession->errhp),
              OCI_HTYPE_ERROR,     (size_t) 0, (dvoid **) 0))
    {
        strcpy(errStr, "FAILED:OCIHandleAlloc On Error Handle");
        return OCI_ERROR;
    }
         /*������������*/
    if( OCIHandleAlloc( (dvoid *) (DBSession->envhp), (dvoid **) &(DBSession->srvhp),
              OCI_HTYPE_SERVER,    (size_t) 0, (dvoid **) 0))
    {
        strcpy(errStr, "FAILED:OCIHandleAlloc On Srv Handle");
        return OCI_ERROR;
    }
         /*������񻷾����*/
    if( OCIHandleAlloc( (dvoid *) (DBSession->envhp), (dvoid **) &(DBSession->svchp),
              OCI_HTYPE_SVCCTX,    (size_t) 0, (dvoid **) 0))
    {
        strcpy(errStr, "FAILED:OCIHandleAlloc On Service Context Handle");
        return OCI_ERROR;
    }
        /*����server���ݿ�*/
    if( OCIServerAttach( DBSession->srvhp, DBSession->errhp, (text *)server, strlen(server), 0))
    {
        sprintf(errStr, "FAILED:OCIServerAttach (Can't connect to %s)", server );
        return OCI_ERROR;
    }
        /*���÷��񻷾��ķ���������*/
    if( OCIAttrSet( (dvoid *) (DBSession->svchp), OCI_HTYPE_SVCCTX, (dvoid *)(DBSession->srvhp),
               (ub4) 0,    OCI_ATTR_SERVER, (OCIError *) (DBSession->errhp)))
    {
        strcpy(errStr, "FAILED:OCIAttrSet" );
        return OCI_ERROR;
    }
        /*����Ự���*/
    if( OCIHandleAlloc((dvoid *) (DBSession->envhp), (dvoid **)&(DBSession->authp),
               (ub4) OCI_HTYPE_SESSION,    (size_t) 0, (dvoid **) 0))
    {
        strcpy(errStr, "FAILED:OCIHandleAlloc On Authentication Handle" );
        return OCI_ERROR;
    }
        /*���ûỰ��ʹ�õ��û��ʻ�*/
    if( OCIAttrSet((dvoid *) (DBSession->authp), (ub4) OCI_HTYPE_SESSION,
                    (dvoid *) user, (ub4) strlen(user),
                    (ub4) OCI_ATTR_USERNAME, DBSession->errhp))
    {
        strcpy(errStr, "FAILED:OCIAttrSet on USERID" );
        return OCI_ERROR;
    }
        /*���ûỰ��ʹ�õ��û�����*/
    if( OCIAttrSet((dvoid *) (DBSession->authp), (ub4) OCI_HTYPE_SESSION,
                    (dvoid *)password, (ub4) strlen(password),
                    (ub4) OCI_ATTR_PASSWORD, DBSession->errhp))
    {
        strcpy(errStr, "FAILED:OCIAttrSet on PASSWD");
        return OCI_ERROR;
    }
       /*��֤�û��������Ự*/
    if( _ORA_CheckErr(errStr, DBSession->errhp, OCISessionBegin ( DBSession->svchp,
               DBSession->errhp, DBSession->authp, OCI_CRED_RDBMS, (ub4) OCI_DEFAULT))<0)
    {
        return OCI_ERROR;
    }
       /*���ûỰ���񻷾�*/
    if( OCIAttrSet((dvoid *) (DBSession->svchp), (ub4) OCI_HTYPE_SVCCTX,
                    (dvoid *) (DBSession->authp), (ub4) 0,
                    (ub4) OCI_ATTR_SESSION, (DBSession->errhp)))
    {
        strcpy(errStr, "FAILED:OCIAttrSet on session" );
        return OCI_ERROR;
    }
       /*���������*/
    if( _ORA_CheckErr(errStr, (DBSession->errhp), OCIHandleAlloc( (dvoid *) (DBSession->envhp),
               (dvoid **) &(DBSession->tsql.stmthp),    OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0))<0)
    {
        return OCI_ERROR;
    }

    DBSession->flag = OCI_DEFAULT;
    DBSession->buf_flag = 0;
    _ORA_ExecSql(errStr, DBSession, "ALTER SESSION SET NLS_DATE_FORMAT = 'YYYYMMDDHH24MISS'");

    return ORA_SUCCESS;
}

/*�ͷŻ���ģʽ���ڴ�*/
void _ORA_FreeSqlBuf(ORA_TSQL *tsql, ORA_SESSION *db)
{
    int i;
    for(i=0; i<ORA_MAX_COL_LEN; i++)
    {
        if(tsql->tlob[i] != NULL)
        {
            OCILobFreeTemporary(db->svchp, db->errhp, tsql->tlob[i]);
            OCIDescriptorFree((dvoid *) tsql->tlob[i], (ub4) OCI_DTYPE_LOB);
            tsql->tlob[i] = NULL;
        }
    }

    if(tsql->buf != NULL)
    {
        _PDSC_MemFree(tsql->buf);
        tsql->buf = NULL;
    }

    if(tsql->qbuf != NULL)
    {
        ORA_QUERY_BUF *qbuf = tsql->qbuf;
        ORA_BUF *tmpBuf;

        while(NULL !=qbuf->mem_buf)
        {
            tmpBuf = qbuf->mem_buf;
            qbuf->mem_buf = qbuf->mem_buf->next;
            _PDSC_MemFree(tmpBuf);
        }
        _PDSC_MemFree(qbuf);
        tsql->qbuf = NULL;
    }

    if (tsql->stmthp != NULL)
    {
        OCIHandleFree((dvoid *)(tsql->stmthp), OCI_HTYPE_STMT);
        tsql->stmthp = NULL;
    }

    return;
}

/*****************************************************************/
/*!
    * @brief �Ͽ����ݿ�
    * @param [in]  DBSession     ORACLE����ָ��
    * @return ��
    */
void _ORA_Disconnect(ORA_SESSION *DBSession)
{
    if(NULL==DBSession)
        return;

    _ORA_FreeSqlBuf(&(DBSession->tsql), DBSession);

    /* Delete the user session */
    if (DBSession->authp != NULL)
    {
        OCISessionEnd(DBSession->svchp, DBSession->errhp, DBSession->authp, (ub4)OCI_DEFAULT);
    }
    /* Delete access to the data source */
    if (DBSession->errhp)
    {
        OCIServerDetach(DBSession->srvhp, DBSession->errhp, OCI_DEFAULT);
    }
    /* Explicitly deallocate all handles */
    if (DBSession->srvhp != NULL)
    {
        OCIHandleFree((dvoid *)(DBSession->srvhp), OCI_HTYPE_SERVER);
        DBSession->srvhp = NULL;
    }
    if (DBSession->svchp != NULL)
    {
        OCIHandleFree((dvoid *)(DBSession->svchp), OCI_HTYPE_SVCCTX);
        DBSession->svchp = NULL;
    }
    if (DBSession->errhp != NULL)
    {
        OCIHandleFree((dvoid *)(DBSession->errhp), OCI_HTYPE_ERROR);
        DBSession->errhp = NULL;
    }

    if (DBSession->authp != NULL)
    {
        OCIHandleFree((dvoid *)(DBSession->authp), OCI_HTYPE_SESSION);
        DBSession->authp = NULL;
    }
    if (DBSession->envhp != NULL)
    {
        OCIHandleFree((dvoid *)(DBSession->envhp), OCI_HTYPE_ENV);
        DBSession->envhp = NULL;
    }

    return;
}

/*�ͷž��*/
void _ORA_FreeStmt(OCIStmt **stmthp)
{
    OCIHandleFree((dvoid *)(*stmthp), OCI_HTYPE_STMT);
    *stmthp = NULL;
    return;
}

/*�任SQL���*/
int _ORA_CovertSql(const char *inSql, char *outSql)
{
    int i=0, num=0, flag=0;
    char *p;

    p = (char *)inSql;
    while(*p == ' ')
        p++;
    while(*p != 0)
    {
        if(*p == '\'')
            flag = 1 - flag;

        if( (*p == '?') && (flag == 0) )
        {
            num++;
            i += sprintf(outSql+i, ":v%d", num);
        }
        else
            outSql[i++] = *p;

        p++;
    }
    outSql[i] = 0;

    return num;
}

/*��Select���*/
int _ORA_BindSelect(char *errStr, ORA_SESSION *DBSession, ORA_TSQL *tsql)
{
    int i, j;
    OCIDefine *defnp[ORA_MAX_COL_LEN];
    sword ret;
    OCIParam *parmdp;

    /*ִ��SQL*/
    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIStmtExecute( DBSession->svchp, tsql->stmthp, DBSession->errhp,
            (ub4)0, (ub4) 0, (CONST OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_DEFAULT) ) <0)
        return ORA_FAILURE;

    /*ȡ�ֶθ���*/
    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIAttrGet(tsql->stmthp,
            OCI_HTYPE_STMT, &(tsql->field_count), 0, OCI_ATTR_PARAM_COUNT, DBSession->errhp) )<0)
        return ORA_FAILURE;

    if( (tsql->buf == NULL) && (NULL == (tsql->buf = (char *)_PDSC_MemMalloc(ORA_MAX_COL_LEN * ORA_MAX_FIELD_LEN))) )
    {
        strcpy(errStr, "malloc ORA_MAX_COL_LEN * ORA_MAX_FIELD_LEN ʧ��");
        return ORA_FAILURE;
    }

    /*������ݰ�*/
    j = 0;
    tsql->lobselect = 0;
    for(i=0; i<tsql->field_count; i++)
    {
        defnp[i] = (OCIDefine *) 0;
        tsql->res_row[i] = tsql->buf + j;

        OCIParamGet(tsql->stmthp, OCI_HTYPE_STMT, DBSession->errhp, (dvoid **)&parmdp, (ub4)i+1);
        OCIAttrGet((dvoid*)parmdp, OCI_DTYPE_PARAM, (dvoid *)&(tsql->col_type[i]), (ub4 *)0, OCI_ATTR_DATA_TYPE, DBSession->errhp);

        if(SQLT_BLOB != tsql->col_type[i]) /*��ͨ�ֶ�*/
        {
            j += ORA_MAX_FIELD_LEN;
            ret = OCIDefineByPos( tsql->stmthp, &defnp[i], DBSession->errhp,  i+1, (dvoid *) (tsql->res_row[i]),
                  ORA_MAX_FIELD_LEN, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
        }
        else
        {
            j += ORA_MAX_LOB_LEN;
            tsql->lobselect = 1;
            if(tsql->tlob[i] == NULL)
                OCIDescriptorAlloc((dvoid *)DBSession->envhp, (dvoid **)&(tsql->tlob[i]), (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid**)0);

            ret = OCIDefineByPos( tsql->stmthp, &defnp[i], DBSession->errhp,  i+1, (dvoid *) &(tsql->tlob[i]),
                  -1, SQLT_BLOB, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT);
        }

        if( _ORA_CheckErr( errStr, DBSession->errhp, ret ) < 0 )
        {
            _PDSC_MemFree(tsql->buf);
            tsql->buf = NULL;
            return ORA_FAILURE;
        }
    }
    return ORA_SUCCESS;
}

/*SQL����*/
int _ORA_PrepareSql(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, const char *inSql)
{
    char tmpSql[1024];

    pSQL->param_num = _ORA_CovertSql(inSql, tmpSql);
    if(NULL == pSQL->tsql.stmthp)
    {
        if( _ORA_CheckErr(errStr, (DBSession->errhp), OCIHandleAlloc( (dvoid *) (DBSession->envhp),
               (dvoid **) &(pSQL->tsql.stmthp),  OCI_HTYPE_STMT, (size_t) 0, (dvoid **) 0))<0)
        {
            return ORA_FAILURE;
        }
    }

    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIStmtPrepare( pSQL->tsql.stmthp,
                  DBSession->errhp, (text *)tmpSql, (ub4)strlen( (char *)tmpSql ),
                  (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT)) < 0 )
    {
        _ORA_FreeStmt(&(pSQL->tsql.stmthp));
        pSQL->tsql.stmthp = NULL;
        return ORA_FAILURE;
    }

    if(0 == strncasecmp(tmpSql, "select ", 7))
    {
        if(pSQL->param_num == 0) /*���û�в�����������ֱ�Ӱ�select���*/
        {
            if(ORA_SUCCESS != _ORA_BindSelect(errStr, DBSession, &(pSQL->tsql)) )
            {
                _ORA_FreeStmt(&(pSQL->tsql.stmthp));
                pSQL->tsql.stmthp = NULL;
                return ORA_FAILURE;
            }
        }
        pSQL->type = 0;
    }
    else
        pSQL->type = 1;  /*�ǲ�ѯ*/

    pSQL->db = DBSession;
    pSQL->lobupdate = 0;
    return ORA_SUCCESS;
}

/*�󶨱���*/
int _ORA_BindParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, char data_type, void *param)
{
    ub2 ptype;
    sb4 len;

    switch(data_type)
    {
        case 0:    ptype = SQLT_STR;    len = 1024;             break;
        case 1:    ptype = SQLT_INT;    len = sizeof(int);      break;
        case 2:    ptype = SQLT_STR;    len = 20;               break;
        case 3:    ptype = SQLT_FLT;    len = sizeof(float);    break;
        case 4:    ptype = SQLT_FLT;    len = sizeof(double);   break;
        case 5:    ptype = SQLT_INT;    len = sizeof(long);     break;
    }
    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIBindByPos(pSQL->tsql.stmthp, &(pSQL->bindHandle[param_id]), DBSession->errhp, (ub4) (param_id+1),
        (dvoid *) param, len, ptype, (dvoid *) 0, (ub2 *)0, (ub2 *)0, (ub4) 0, (ub4 *) 0, (ub4) OCI_DEFAULT)) < 0 )
        return ORA_FAILURE;

    /*��select���*/
    if( (pSQL->type == 0) && (param_id + 1 == pSQL->param_num) )
    {
        if(ORA_SUCCESS != _ORA_BindSelect(errStr, DBSession, &(pSQL->tsql)) )
        {
            _ORA_FreeStmt(&(pSQL->tsql.stmthp));
            pSQL->tsql.stmthp = NULL;
            return ORA_FAILURE;
        }
    }
    pSQL->bind_type[param_id] = 0;
    return ORA_SUCCESS;
}

/*��BLOB����*/
int _ORA_BindBlobParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, void *param, int size)
{
    ORA_TSQL *tsql = &(pSQL->tsql);

    if(tsql->tlob[param_id] == NULL)
    {
        OCIDescriptorAlloc((dvoid *)DBSession->envhp, (dvoid **)&(tsql->tlob[param_id]), (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid**)0);
        OCILobCreateTemporary(DBSession->svchp, DBSession->errhp, tsql->tlob[param_id], (ub2)0, SQLCS_IMPLICIT,
                           OCI_TEMP_BLOB, OCI_ATTR_NOCACHE, OCI_DURATION_SESSION);
    }

    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIBindByPos(tsql->stmthp, &(pSQL->bindHandle[param_id]), DBSession->errhp, (ub4) (param_id+1),
          (dvoid *) &(tsql->tlob[param_id]), (sword) -1, SQLT_BLOB, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT)) < 0 )
        return ORA_FAILURE;

    pSQL->bind_p[param_id] = param;
    pSQL->bind_size[param_id] = size;
    pSQL->bind_type[param_id] = 1;
    pSQL->lobupdate = 1;
    return 0;
}

/*��ȡһ����¼*/
ORA_ROW _ORA_FetchData_P(ORA_SESSION *DBSession, ORA_TSQL *tsql)
{
    sword ret = OCIStmtFetch( tsql->stmthp, DBSession->errhp, (ub4)1, (ub4)OCI_FETCH_NEXT, (ub4) OCI_DEFAULT);
    if( (ret == 0) || (_ORA_CheckErr( NULL, DBSession->errhp, ret) >= 0 ) )
    {
        if(1 == tsql->lobselect)
        {
            int i;
            ub4 amt;
            for(i=0; i<tsql->field_count; i++)
            {
                if(SQLT_BLOB == tsql->col_type[i])
                {
                    amt = 0;
                    OCILobRead(DBSession->svchp, DBSession->errhp, tsql->tlob[i], &amt, 1, (dvoid *)(tsql->res_row[i]+4), (ub4) ORA_MAX_LOB_LEN,
                             (dvoid *)0, (sb4 (*) (dvoid *, CONST dvoid *, ub4, ub1)) 0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
                    int4store(tsql->res_row[i], amt);
                }
            }
        }
        return tsql->res_row;
    }
    else
        return NULL;
}

/*��������ģʽ*/
void _ORA_QueryBuf(ORA_SESSION *DBSession, ORA_TSQL *tsql)
{
    int i, k;
    ORA_ROW row;
    ORA_BUF *tmpBuf;
    ORA_QUERY_BUF *qbuf;

    if(tsql->qbuf == NULL)
    {
        tsql->qbuf = (ORA_QUERY_BUF *)_PDSC_MemMalloc(sizeof(ORA_QUERY_BUF));
        memset(tsql->qbuf, 0, sizeof(ORA_QUERY_BUF));
    }
    qbuf = tsql->qbuf;

    if(NULL == qbuf->mem_buf)
    {
        qbuf->mem_buf = (ORA_BUF *)_PDSC_MemMalloc(sizeof(ORA_BUF));
        qbuf->mem_buf->next = NULL;
        qbuf->mem_buf->len = 0;
    }
    else
    {
        while(NULL != qbuf->mem_buf->next)
        {
            tmpBuf = qbuf->mem_buf->next;
            qbuf->mem_buf->next = qbuf->mem_buf->next->next;
            _PDSC_MemFree(tmpBuf);
        }
        qbuf->mem_buf->len = 0;
    }

    tmpBuf = qbuf->mem_buf;
    DBSession->ret_rows = 0;
    while(NULL != (row = _ORA_FetchData_P(DBSession, tsql)))
    {
        if(tmpBuf->len > ORA_BUF_LEN - ORA_MAX_BUFF_LEN)
        {
            tmpBuf->next = (ORA_BUF *)_PDSC_MemMalloc(sizeof(ORA_BUF));
            tmpBuf->next->next = NULL;
            tmpBuf->next->len = 0;
            tmpBuf = tmpBuf->next;
        }

        for( i=0; i<tsql->field_count; i++ )
        {
            if(SQLT_BLOB != tsql->col_type[i])   /*��ͨ�ֶ�*/
            {
                k = sprintf(tmpBuf->buf + tmpBuf->len + 4, "%s", row[i]);
                int4store(tmpBuf->buf + tmpBuf->len, k);
                tmpBuf->len += k + 5;
            }
            else
            {
                k = uint4korr(row[i]);
                memcpy(tmpBuf->buf + tmpBuf->len + 4, row[i], k+4);
                int4store(tmpBuf->buf + tmpBuf->len, k+4);
                tmpBuf->len += k + 9;
            }
        }
        DBSession->ret_rows++;
    }
    qbuf->cur_buf = qbuf->mem_buf;
    qbuf->read_pos = 0;
    return;
}

/*ִ���ѽ�����SQL*/
int _ORA_ExecSql_P(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL)
{
    if(1 == pSQL->lobupdate)
    {
        int i;
        ub4 amt;
        for(i=0; i<pSQL->param_num; i++)
        {
            if(1 == pSQL->bind_type[i])
            {
                amt = pSQL->bind_size[i];
                OCILobWrite(DBSession->svchp, DBSession->errhp, pSQL->tsql.tlob[i], &amt, 1, (dvoid *) pSQL->bind_p[i],
                    ORA_MAX_LOB_LEN, OCI_ONE_PIECE, (dvoid *)0, (sb4 (*)(dvoid *, dvoid *, ub4 *, ub1 *)) 0, (ub2) 0, (ub1) SQLCS_IMPLICIT);
            }
        }
    }

    if (_ORA_CheckErr(errStr, DBSession->errhp, OCIStmtExecute(DBSession->svchp,
                 pSQL->tsql.stmthp, DBSession->errhp, pSQL->type, (ub4) 0,
                 (OCISnapshot *) NULL, (OCISnapshot *) NULL, DBSession->flag))<0 )
        return ORA_FAILURE;

    /*��ȡ������¼��*/
    if(1 == pSQL->type)
    {
        int rows;
        OCIAttrGet(pSQL->tsql.stmthp, OCI_HTYPE_STMT, &rows, 0, OCI_ATTR_ROW_COUNT, DBSession->errhp);
        DBSession->ret_rows = rows;
    }
    else   /*select����*/
    {
        DBSession->ret_rows = 0;
        if(1 == DBSession->buf_flag) /*����ģʽ*/
            _ORA_QueryBuf(DBSession, &(pSQL->tsql));
    }

    return ORA_SUCCESS;
}

/*�ͷ� SQL �ڴ� */
void _ORA_FreeSql(ORA_SQL *pSQL)
{
    _ORA_FreeSqlBuf(&(pSQL->tsql), pSQL->db);
    return;
}

/*�����Ƿ��Զ��ύ 0 ���Զ��ύ, 1 �Զ��ύ*/
void _ORA_SetAutoCommit(ORA_SESSION *DBSession, int flag)
{
    DBSession->flag = (flag == 1) ? OCI_COMMIT_ON_SUCCESS : OCI_DEFAULT;
    return;
}

/*���û���ģʽ*/
void _ORA_SetBuffFlag(ORA_SESSION *DBSession, int flag)
{
    DBSession->buf_flag = flag;
    return;
}

/*****************************************************************/
/*!
    * @brief ����һ������
    * @param [in]  DBSession     ORACLE����ָ��
    * @param [in]  flag          0 �ύ�� 1 �ع�
    * @return 0 �ɹ���-1 ʧ��
    */
int _ORA_EndTrans(char *errStr, ORA_SESSION *DBSession, int flag)
{
    sword ret;
    if(0 == flag)
        ret = OCITransCommit(DBSession->svchp, DBSession->errhp, (ub4) 0);
    else
        ret = OCITransRollback(DBSession->svchp, DBSession->errhp, (ub4) 0);

    if (_ORA_CheckErr(errStr, DBSession->errhp, ret)<0 )
        return -1;

    return 0;
}

/*������ѯ��¼*/
ORA_ROW _ORA_FetchData(ORA_SESSION *DBSession, ORA_SQL *pSQL)
{
    ORA_TSQL *tsql;
    if(pSQL != NULL)
        tsql = &(pSQL->tsql);
    else
        tsql = &(DBSession->tsql);

    if(0 == DBSession->buf_flag) /*�ǻ���ģʽ*/
    {
        return _ORA_FetchData_P(DBSession, tsql);
    }
    else     /*����ģʽ*/
    {
        int i, k;
        ORA_QUERY_BUF *qbuf = tsql->qbuf;

        k = qbuf->read_pos;
        if(k >= qbuf->cur_buf->len)
        {
            if(NULL == qbuf->cur_buf->next)
            {
                ORA_BUF *tmpBuf;
                while(NULL != qbuf->mem_buf->next)
                {
                    tmpBuf = qbuf->mem_buf->next;
                    qbuf->mem_buf->next = qbuf->mem_buf->next->next;
                    _PDSC_MemFree(tmpBuf);
                }
                return NULL;
            }
            else
            {
                qbuf->cur_buf = qbuf->cur_buf->next;
                k = 0;
            }
        }
        for(i=0; i<tsql->field_count; i++)
        {
            qbuf->resp_row[i] = qbuf->cur_buf->buf + k + 4;
            k += uint4korr(qbuf->cur_buf->buf + k) + 5;
        }
        qbuf->read_pos = k;
        return qbuf->resp_row;
    }
}

/*****************************************************************/
/*!
    * @brief ִ��oracle���
    * @param [out] errStr        �����ַ���
    * @param [in]  DBSession     ORACLE����ָ��
    * @param [in]  inSql         SQL���
    * @return 0���ɹ���-1��ʧ��
    */
int _ORA_ExecSql(char *errStr, ORA_SESSION *DBSession, const char *inSql)
{
    int i;
    char *p;

    if( _ORA_CheckErr( errStr, DBSession->errhp, OCIStmtPrepare( DBSession->tsql.stmthp,
                  DBSession->errhp, (text *)inSql, (ub4)strlen( (char *)inSql ),
                  (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT)) < 0 )
        return ORA_FAILURE;

    p = (char *)inSql;
    while(*p == ' ')
        p++;
    /*������ݰ�*/
    if(0 == strncasecmp(p, "select ", 7))
    {
        if(ORA_SUCCESS != _ORA_BindSelect(errStr, DBSession, &(DBSession->tsql)))
            return ORA_FAILURE;

        DBSession->ret_rows = 0;
        if(1 == DBSession->buf_flag) /*����ģʽ*/
            _ORA_QueryBuf(DBSession, &(DBSession->tsql));
    }
    else  /*�޸Ĳ���*/
    {
        if (_ORA_CheckErr(errStr, DBSession->errhp, OCIStmtExecute(DBSession->svchp,
                 DBSession->tsql.stmthp, DBSession->errhp, (ub4) 1, (ub4) 0,
                 (OCISnapshot *) NULL, (OCISnapshot *) NULL, DBSession->flag))<0 )
            return ORA_FAILURE;

        /*������¼��*/
        OCIAttrGet(DBSession->tsql.stmthp, OCI_HTYPE_STMT, &i, 0, OCI_ATTR_ROW_COUNT, DBSession->errhp);
        DBSession->ret_rows = i;
    }

    return ORA_SUCCESS;   /* 0 */
}