
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "dsci_comm.h"
#include "DB_Odbc.h"

int __O_errcode(SQLINTEGER errcode)
{
    int ecode=-2000;

    #ifdef TIMESTEN
    switch(errcode)
    {
        case 994:   ecode = 2008;     break;
        case 907:   ecode = -2100;    break;
        default:    ecode = -2000;
    }
    #endif

    #ifdef ALTIBASE
    switch(errcode)
    {
        case 331830:
        case 331843:
        case 327730:
            ecode = 2008;
            break;

        case 69720:  ecode = -2100;    break;
        default:     ecode = -2000;
    }
    #endif

    return ecode;
}

/*获取SQL错误信息*/
int _O_SqlError(SQLHDBC hdbc, SQLHSTMT hstmt, RETCODE rc, char *errStr, int *errCode)
{
    SQLCHAR   szErrorMsg[256];
    SWORD     length;
    SQLINTEGER ecode;

    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA_FOUND)
        return 0;

    SQLError(SQL_NULL_HENV, hdbc, hstmt, NULL, &ecode, szErrorMsg, sizeof(szErrorMsg), &length);
    szErrorMsg[230] = 0;
    *errCode = __O_errcode(ecode);
    sprintf(errStr, "Error %d: %s", ecode, szErrorMsg);
    return -1;
}

/*设置是否自动提交，登录成功后调用, 0 非自动提交, 1 自动提交*/
void _O_SetAutoCommit(O_SESSION *DBSession, int flag)
{
    SQLSetConnectOption(DBSession->hdbc, SQL_AUTOCOMMIT, (long)flag);
    return;
}

/*登陆数据库*/
int _O_Connect(char *errStr, O_SESSION *DBSession, char *user, char *password, char *server)
{
    char tmpStr[256];
    RETCODE rc;
    int tcpMode=0;
    #ifdef ALTIBASE
      int i, iArr[4];
      char str[256], cMode=0;
    #endif

    DBSession->eno = -2000;
    rc = SQLAllocEnv(&(DBSession->henv));
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    {
        sprintf(errStr, "Error: failed to allocate environment!");
        return -1;
    }

    rc = SQLAllocConnect(DBSession->henv, &(DBSession->hdbc));
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    {
        sprintf(errStr, "Error: failed to allocate connection!");
        return -1;
    }

#ifdef ALTIBASE
    memset(iArr, 0, sizeof(iArr));
    _PDSC_SplitStrM(server, str, ':', &i, 5, iArr);
    cMode = str[iArr[2]];
    if('2' == cMode) /*UNXI domain*/
        cMode = '2';
    else if('1' == cMode)  /*IPC*/
        cMode = '3';
    else
    {
        cMode = '1'; /*TCP模式*/
        tcpMode = 1;/*使用TCP模式*/
    }

    sprintf(tmpStr, "DSN=%s;UID=%s;PWD=%s;CONNTYPE=%c;NLS_USE=US7ASCII;PORT_NO=%s",
            str, user, password, cMode, str+iArr[1]);

    /*设置ALTIBASE_HOME*/
    if( (('3' == cMode)||('2' == cMode) ) && (i >= 3) ) /*使用IPC方式*/
    {
        char envStr[512];
        sprintf(envStr, "ALTIBASE_HOME=%s", str+iArr[3]);
        putenv(envStr);
    }
#else
    sprintf(tmpStr, "DSN=%s;UID=%s;PWD=%s", server, user, password);
#endif

    rc = SQLDriverConnect (DBSession->hdbc, NULL, (SQLCHAR*)tmpStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (-1 == _O_SqlError(DBSession->hdbc, SQL_NULL_HSTMT, rc, errStr, &(DBSession->eno)))
    {
		/*连接不成功用TCP模式*/
		if (tcpMode == 1)/*TCP模式*/
			return -1;
      #ifdef ALTIBASE
  		/*非TCP模式连接不成功,用TCP模式再连接一次*/
  		sprintf(tmpStr, "DSN=%s;UID=%s;PWD=%s;CONNTYPE=1;NLS_USE=US7ASCII;PORT_NO=%s", str, user, password, str+iArr[1]);
		rc = SQLDriverConnect (DBSession->hdbc, NULL, (SQLCHAR*)tmpStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
		if (-1 == _O_SqlError(DBSession->hdbc, SQL_NULL_HSTMT, rc, errStr, &(DBSession->eno)))
			return -1;
      #endif
    }

    /*预解析commit*/
    rc = SQLAllocStmt(DBSession->hdbc, &(DBSession->hstmt_commit));
    if (-1 == _O_SqlError(DBSession->hdbc, SQL_NULL_HSTMT, rc, errStr, &(DBSession->eno)))
         return -1;
    rc = SQLPrepare(DBSession->hstmt_commit, (SQLCHAR *)"commit", SQL_NTS);
    if (-1 == _O_SqlError(DBSession->hdbc, DBSession->hstmt_commit, rc, errStr, &(DBSession->eno)))
        return -1;

    /*预解析rollback*/
    rc = SQLAllocStmt(DBSession->hdbc, &(DBSession->hstmt_rollback));
    if (-1 == _O_SqlError(DBSession->hdbc, SQL_NULL_HSTMT, rc, errStr, &(DBSession->eno)))
         return -1;
    rc = SQLPrepare(DBSession->hstmt_rollback, (SQLCHAR *)"rollback", SQL_NTS);
    if (-1 == _O_SqlError(DBSession->hdbc, DBSession->hstmt_rollback, rc, errStr, &(DBSession->eno)))
        return -1;

    _O_SetAutoCommit(DBSession, 0);
    DBSession->buf_flag = 0;

    return 0;
}

/*断开*/
void _O_Disconnect(O_SESSION *DBSession)
{
    _O_FreeSql(&(DBSession->dSql), 1);

    if (DBSession->hstmt_commit != NULL)
    {
        SQLFreeStmt(DBSession->hstmt_commit, SQL_DROP);
        DBSession->hstmt_commit = NULL;
    }
    if (DBSession->hstmt_rollback != NULL)
    {
        SQLFreeStmt(DBSession->hstmt_rollback, SQL_DROP);
        DBSession->hstmt_rollback = NULL;
    }

    if(NULL != DBSession->hdbc)
    {
        SQLDisconnect(DBSession->hdbc);
        SQLFreeConnect(DBSession->hdbc);
    }

    if(NULL != DBSession->henv)
        SQLFreeEnv(DBSession->henv);

    memset(&(DBSession->dSql), 0, sizeof(O_SQL));
    DBSession->hdbc  = NULL;
    DBSession->henv  = NULL;
    return;
}

/*SQL解析*/
int _O_PrepareSql(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, const char *inSql)
{
    RETCODE rc;
    SQLSMALLINT paramNum;

    if(NULL != pSQL->hstmt)
        SQLFreeStmt(pSQL->hstmt, SQL_DROP);
    rc = SQLAllocStmt(DBSession->hdbc, &(pSQL->hstmt));
    if (-1 == _O_SqlError(DBSession->hdbc, SQL_NULL_HSTMT, rc, errStr, &(DBSession->eno)))
         return -1;

    pSQL->open_flag = 0;
    rc = SQLPrepare(pSQL->hstmt, (SQLCHAR *)inSql, SQL_NTS);
    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
        return -1;

    rc = SQLNumResultCols(pSQL->hstmt, &paramNum);
    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
         return -1;
    pSQL->field_count = paramNum;

    /*select操作*/
    if(pSQL->field_count > 0)
    {
        unsigned short i,j;
        char tmpStr[1024];
        SQLSMALLINT blen=128, nlen, scale, nullable, iType;
        _MYULEN colsize;
        int valueLen=0, pos;

        if(pSQL->buf == NULL)
        {
            pSQL->buf = (char *)_PDSC_MemMalloc(O_MAX_COL_LEN * O_MAX_FIELD_LEN);
        }

        j=0;
        pSQL->lobselect = 0;
        for(i=0; i<pSQL->field_count; i++)
        {
            rc = SQLDescribeCol (pSQL->hstmt, (unsigned short)(i+1), (SQLCHAR *)tmpStr, blen, &nlen,
                     (short *)&(pSQL->col_type[i]), &colsize, &scale, &nullable);
            if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
                return -1;

            pSQL->res_row[i] = pSQL->buf+j;
            pSQL->res_row[i][0] = 0;
            valueLen=0;
            pos = 0;
            iType = SQL_C_CHAR;
            switch(pSQL->col_type[i])
            {
                case SQL_BIGINT:
                case SQL_SMALLINT:
                case SQL_INTEGER:
                case SQL_DECIMAL:
                case SQL_NUMERIC:
                case SQL_REAL:
                case SQL_FLOAT:
                case SQL_DOUBLE:
                    valueLen = O_MAX_DIGITAL_LEN;
                    break;
                case SQL_DATE:
                    valueLen = O_MAX_DATE_LEN;
                    break;
                case SQL_BINARY:
                    valueLen = O_MAX_LOB_LEN;
                    iType = SQL_C_BINARY;
                    pSQL->lobselect = 1;
                    pos = 4;
                    break;
                default:
                    valueLen = colsize+10;
            }
            j += valueLen + 1;
            rc = SQLBindCol(pSQL->hstmt, (unsigned short)(i+1), iType, (void *)(pSQL->res_row[i]+pos), valueLen, &(pSQL->col_len[i]));
            if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
            return -1;
        }
    }

    /*参数个数*/
    rc = SQLNumParams(pSQL->hstmt, &(pSQL->param_num));
    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
        return -1;

    DBSession->pSQL = pSQL;
    return 0;
}

/*逐行取数据*/
int _O_Fetch(O_SESSION *DBSession, O_SQL *pSQL)
{
    RETCODE rc;
    unsigned short i;
    char *p=NULL;

    rc = SQLFetch(pSQL->hstmt);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    {
        SQLFreeStmt(pSQL->hstmt, SQL_CLOSE);
        pSQL->open_flag = 0;
        return 0;
    }

    if(pSQL->lobselect == 1)
    {
        for(i=0; i<pSQL->field_count; i++)
        {
            if(SQL_BINARY == pSQL->col_type[i])  /*LOB*/
                int4store(pSQL->res_row[i], pSQL->col_len[i]);
        }
    }

    #ifdef TIMESTEN
    for(i=0; i<pSQL->field_count; i++)
    {
        if(11 == pSQL->col_type[i])  /*日期转换*/
        {
            p = pSQL->res_row[i];
            memcpy(p+4, p+5, 2);
            memcpy(p+6, p+8, 2);
            memcpy(p+8, p+11, 2);
            memcpy(p+10, p+14, 2);
            memcpy(p+12, p+17, 2);
            p[14] = 0;
        }
    }
    #endif

    DBSession->ret_rows++;
    return 1;
}

/*执行已解析的SQL*/
int _O_ExecSql_P(char *errStr, O_SESSION *DBSession, O_SQL *pSQL)
{
    RETCODE rc;
    _MYULEN crow;

    if(1 == pSQL->open_flag)
    {
        SQLFreeStmt(pSQL->hstmt, SQL_CLOSE);
        pSQL->open_flag = 0;
    }

    rc = SQLExecute(pSQL->hstmt);
    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
        return -1;
    DBSession->pSQL = pSQL;

    if(0 == pSQL->field_count)   /*普通操作*/
    {
        SQLRowCount(pSQL->hstmt, &crow);
        DBSession->ret_rows = (long)crow;
    }
    else              /*select */
    {
        DBSession->field_count = pSQL->field_count;
        DBSession->ret_rows = 0;
        pSQL->open_flag = 1;

        if(1 == DBSession->buf_flag) /*缓冲模式*/
        {
            int i, k;
            O_BUF *tmpBuf;

            if(NULL == pSQL->mem_buf)
            {
                pSQL->mem_buf = (O_BUF *)_PDSC_MemMalloc(sizeof(O_BUF));
                pSQL->mem_buf->next = NULL;
                pSQL->mem_buf->len = 0;
            }
            else
            {
                while(NULL != pSQL->mem_buf->next)
                {
                    tmpBuf = pSQL->mem_buf->next;
                    pSQL->mem_buf->next = pSQL->mem_buf->next->next;
                    _PDSC_MemFree(tmpBuf);
                }
                pSQL->mem_buf->len = 0;
            }

            tmpBuf = pSQL->mem_buf;
            while(1 == _O_Fetch(DBSession, pSQL))
            {
                if(tmpBuf->len > O_BUF_LEN - O_MAX_BUFF_LEN)
                {
                    tmpBuf->next = (O_BUF *)_PDSC_MemMalloc(sizeof(O_BUF));
                    tmpBuf->next->next = NULL;
                    tmpBuf->next->len = 0;
                    tmpBuf = tmpBuf->next;
                }

                for( i=0; i<pSQL->field_count; i++ )
                {
                    if(SQL_BINARY != pSQL->col_type[i])    /*普通字段*/
                    {
                        k = sprintf(tmpBuf->buf + tmpBuf->len + 4, "%s", pSQL->res_row[i]);
                        int4store(tmpBuf->buf + tmpBuf->len, k);
                        tmpBuf->len += k + 5;
                    }
                    else /*LOB*/
                    {
                        k = pSQL->col_len[i];
                        memcpy(tmpBuf->buf + tmpBuf->len + 4, pSQL->res_row[i], k+4);
                        int4store(tmpBuf->buf + tmpBuf->len, k+4);
                        tmpBuf->len += k + 9;
                    }
                }
            }
            pSQL->cur_buf = pSQL->mem_buf;
            pSQL->read_pos = 0;
        }
    }
    return 0;
}

/*直接执行SQL*/
int _O_ExecSql(char *errStr, O_SESSION *DBSession, const char *inSql)
{
    if(0 != _O_PrepareSql(errStr, DBSession, &(DBSession->dSql), inSql))
        return -1;

    return _O_ExecSql_P(errStr, DBSession, &(DBSession->dSql));
}

/*绑定变量*/
int _O_BindParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, char data_type, void *param)
{
    RETCODE rc;

    switch(data_type)
    {
        case 0:
            rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, O_MAX_VALUE_LEN, 0, (char *)param, 0, NULL);
            break;
        case 1:
            rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (int *)param, 0, NULL);
            break;
        case 5:
        	rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(long), 0, (long *)param, 0, NULL);
			break;
        case 3:
            rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, sizeof(float), 0, (float *)param, 0, NULL);
            break;
        case 4:
            rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, sizeof(double), 0, (double *)param, 0, NULL);
            break;
        case 2:
            rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, O_MAX_DATE_LEN, 0, (char *)param, 0, NULL);
            break;
    }

    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
        return -1;

    return 0;
}

/*绑定BLOB变量*/
int _O_BindBlobParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, void *param, int size)
{
    RETCODE rc;

    pSQL->param_szie[param_id] = size;
    rc = SQLBindParameter(pSQL->hstmt, param_id+1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, O_MAX_LOB_LEN,
                     0, param, O_MAX_LOB_LEN+1, (_MYLEN *)(&(pSQL->param_szie[param_id])));

    if (-1 == _O_SqlError(DBSession->hdbc, pSQL->hstmt, rc, errStr, &(DBSession->eno)))
        return -1;

    return 0;
}

/*提交*/
int _O_EndTrans(char *errStr, O_SESSION *DBSession, int flag)
{
    RETCODE rc;
    if(0 == flag)
    {
        rc = SQLExecute(DBSession->hstmt_commit);
        if (-1 == _O_SqlError(DBSession->hdbc, DBSession->hstmt_commit, rc, errStr, &(DBSession->eno)))
            return -1;
    }
    else
    {
        rc = SQLExecute(DBSession->hstmt_rollback);
        if (-1 == _O_SqlError(DBSession->hdbc, DBSession->hstmt_rollback, rc, errStr, &(DBSession->eno)))
            return -1;
    }
    return 0;
}

/*逐条提取数据*/
OSQL_ROW _O_FetchData(O_SESSION *DBSession, O_SQL *pSQL)
{
    if(0 == DBSession->buf_flag) /*非缓冲模式*/
    {
        if(1 == _O_Fetch(DBSession, pSQL))
            return pSQL->res_row;
        else
            return NULL;
    }
    else
    {
        int i, k;
        k = pSQL->read_pos;
        if(k >= pSQL->cur_buf->len)
        {
            if(NULL == pSQL->cur_buf->next)
            {
                O_BUF *tmpBuf;
                while(NULL != pSQL->mem_buf->next)
                {
                    tmpBuf = pSQL->mem_buf->next;
                    pSQL->mem_buf->next = pSQL->mem_buf->next->next;
                    _PDSC_MemFree(tmpBuf);
                }
                return NULL;/*预解析commit*/
            }
            else
            {
                pSQL->cur_buf = pSQL->cur_buf->next;
                k = 0;
            }
        }
        for(i=0; i<pSQL->field_count; i++)
        {
            pSQL->resp_row[i] = pSQL->cur_buf->buf + k + 4;
            k += uint4korr(pSQL->cur_buf->buf + k) + 5;
        }
        pSQL->read_pos = k;
        return pSQL->resp_row;
    }
}

/*设置缓冲模式*/
void _O_SetBuffFlag(O_SESSION *DBSession, int flag)
{
    DBSession->buf_flag = flag;
    return;
}

/*释放 SQL 内存 */
void _O_FreeSql(O_SQL *pSQL, char flag)
{
    O_BUF *tmpBuf;

    if(pSQL->buf != NULL)
    {
        _PDSC_MemFree(pSQL->buf);
        pSQL->buf = NULL;
    }

    while(NULL != pSQL->mem_buf)
    {
        tmpBuf = pSQL->mem_buf;
        pSQL->mem_buf = pSQL->mem_buf->next;
        _PDSC_MemFree(tmpBuf);
    }

    if (pSQL->hstmt != NULL)
    {
        if(flag == 1)  /*altibase在重新连接之后，不能在free之前的hstmt*/
            SQLFreeStmt(pSQL->hstmt, SQL_DROP);
        pSQL->hstmt = NULL;
    }
    return;
}
