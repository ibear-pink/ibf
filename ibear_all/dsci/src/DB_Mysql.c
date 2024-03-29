
/*!
 *  @file Database.c 用于Mysql数据库的连接及对数据库的各种操作
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
#include "DB_Mysql.h"


/*获取数据库错误代码*/
int _MY_GetErrCode(MY_SESSION *DBSession)
{
    unsigned int ecode = mysql_errno(&(DBSession->conn));

    switch(ecode)
    {
        case 2013:         /*Lost connection to MySQL server during query*/
        case 2006:         /*MySQL server has gone away*/
        case 2003:         /*Can't connect to MySQL server*/
            return ecode;

        case 1062:
            return -2100;

        case 2100:
            return -2101;

        default:
            return 0 - ecode;
    }

}

/*登录数据库*/
int _MY_Connect(char *errStr, MY_SESSION *DBSession, char *user, char *password, char *server)
{
    char host[30], dbServer[30], tmpStr[100];
    int i, port, iArr[5];
    _PDSC_SplitStrM(server, tmpStr, ':', &i, 5, iArr);

    strcpy(dbServer, (tmpStr+iArr[0]));
    strcpy(host, (tmpStr+iArr[1]));
    port = atoi(tmpStr+iArr[2]);

    if(mysql_init(&(DBSession->conn)) == 0)
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "Failed to init MYSQL: Error: %s", mysql_error(&(DBSession->conn)));
        return -1;
    }

    if (mysql_real_connect(&(DBSession->conn), host, user, password, dbServer, port, NULL, 0) == 0)
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "Failed to connect to database: Error: %s", mysql_error(&(DBSession->conn)));
        return -1;
    }

    if(mysql_autocommit(&(DBSession->conn), 0) != 0)
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "Failed to forbid autocommt: Error: %s", mysql_error(&(DBSession->conn)));
        return -1;
    }

    DBSession->buf_flag = 0;
    return 0;
}

/*断开*/
void _MY_Disconnect(MY_SESSION *DBSession)
{
    if(DBSession == NULL)
        return;

    if(DBSession->result != NULL)
        mysql_free_result(DBSession->result);

    if(DBSession->buf != NULL)
    {
        _PDSC_MemFree(DBSession->buf);
        DBSession->buf = NULL;
    }

    mysql_close(&(DBSession->conn));
    return;
}

/*直接执行SQL*/
int _MY_ExecSql(char *errStr, MY_SESSION *DBSession, const char *inSql)
{
    if(0 != mysql_query(&(DBSession->conn), inSql))
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "Error: %s", mysql_error(&(DBSession->conn)));
        return -1;
    }

    if(DBSession->result != NULL)
        mysql_free_result(DBSession->result);

    if(DBSession->buf_flag == 0)
        DBSession->result = mysql_use_result(&(DBSession->conn));
    else
        DBSession->result = mysql_store_result(&(DBSession->conn));

    DBSession->lobselect = 0;
    if (DBSession->result)  /*查询操作*/
    {
        int i;
        MYSQL_FIELD *field;
        DBSession->field_count = mysql_num_fields(DBSession->result);
        DBSession->ret_rows = mysql_affected_rows(&(DBSession->conn));
        for(i=0; i<DBSession->field_count; i++)
        {
            field = mysql_fetch_field(DBSession->result);
            DBSession->ctype[i] = field->type;
            if(DBSession->ctype[i] == MYSQL_TYPE_BLOB)
            {
                DBSession->lobselect = 1;
                if(DBSession->buf == NULL)
                    DBSession->buf = (char *)_PDSC_MemMalloc(MY_MAX_LOB_LEN * 8);
            }
        }
    }
    else if (mysql_field_count(&(DBSession->conn)) == 0)
        DBSession->ret_rows = mysql_affected_rows(&(DBSession->conn));

    return 0;
}

/*转换select结果集中的日期字段值*/
void _MY_ConvertDate(MYSQL_ROW row, enum enum_field_types *ctype, int colNum)
{
    int i;
    if(row != NULL)
    {
        for(i=0; i<colNum; i++)
        {
            if(ctype[i] == MYSQL_TYPE_DATETIME)
            {
                strncpy(row[i] + 4,  row[i] + 5,  2);
                strncpy(row[i] + 6,  row[i] + 8,  2);
                strncpy(row[i] + 8,  row[i] + 11, 2);
                strncpy(row[i] + 10, row[i] + 14, 2);
                strncpy(row[i] + 12, row[i] + 17, 2);
                row[i][14] = 0;
            }
        }
    }
}

/*逐条提取数据*/
MYSQL_ROW _MY_FetchData(MY_SESSION *DBSession, MY_SQL *pSQL)
{
    if(pSQL == NULL)
    {
        MYSQL_ROW row = mysql_fetch_row(DBSession->result);
        if(row != NULL)
        {
            _MY_ConvertDate(row, DBSession->ctype, DBSession->field_count);
            if(1 == DBSession->lobselect)
            {
                int i, pos=0;
                unsigned long *len = mysql_fetch_lengths(DBSession->result);

                for(i=0; i<DBSession->field_count; i++)
                {
                    if(DBSession->ctype[i] != MYSQL_TYPE_BLOB)
                        DBSession->res_row[i] = row[i];
                    else
                    {
                        memcpy(DBSession->buf+pos+4, row[i], len[i]);
                        DBSession->res_row[i] = DBSession->buf+pos;
                        int4store(DBSession->res_row[i], len[i]);
                        pos += len[i] + 5;
                    }
                }
                return DBSession->res_row;
            }
            else
                return row;
        }
        else
        {
            if(DBSession->result != NULL)
            {
                mysql_free_result(DBSession->result);
                DBSession->result = NULL;
            }
            return NULL;
        }
    }
    else
    {
        if(0 == mysql_stmt_fetch(pSQL->stmthp))
        {
            int i;
            _MY_ConvertDate(pSQL->res_row, pSQL->ctype, pSQL->field_count);
            if(1 == pSQL->lobselect)
            {
                for(i=0; i<pSQL->field_count; i++)
                {
                    if(pSQL->ctype[i] == MYSQL_TYPE_BLOB)
                        int4store(pSQL->res_row[i], pSQL->lengthr[i]);
                }
            }
            return pSQL->res_row;
        }
        else
            return NULL;
    }
}

/*提交*/
int _MY_EndTrans(char *errStr, MY_SESSION *DBSession, int flag)
{
    int rc;
    if(0 == flag)
        rc = mysql_commit(&(DBSession->conn));
    else
        rc = mysql_rollback(&(DBSession->conn));

    if(rc == 0)
        return 0;

    DBSession->eno = _MY_GetErrCode(DBSession);
    sprintf(errStr, "Error: %s", mysql_error(&(DBSession->conn)));
    return -1;
}

/*SQL解析*/
int _MY_PrepareSql(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, const char *inSql)
{
    if(NULL != pSQL->stmthp)
        mysql_stmt_close(pSQL->stmthp);

    if(NULL == (pSQL->stmthp = mysql_stmt_init(&(DBSession->conn))))
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "mysql_stmt_init error");
        return -1;
    }

    if (mysql_stmt_prepare(pSQL->stmthp, inSql, strlen(inSql)))
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr,  "mysql_stmt_prepare error: %s", mysql_stmt_error(pSQL->stmthp));
        return -1;
    }
    pSQL->timeN = 0;
    pSQL->lobselect = 0;

    pSQL->param_num = mysql_stmt_param_count(pSQL->stmthp);
    pSQL->result = mysql_stmt_result_metadata(pSQL->stmthp);
    if(pSQL->result)   /*select 操作*/
    {
        int i;
        char *p;
        MYSQL_FIELD *field;
        pSQL->field_count = mysql_num_fields(pSQL->result);
        for(i=0; i<pSQL->field_count; i++)
        {
            field = mysql_fetch_field(pSQL->result);
            pSQL->ctype[i] = field->type;
        }
        mysql_free_result(pSQL->result);
        pSQL->result = NULL;
        pSQL->type = 0;

        /*绑定seletc结果*/
        if(pSQL->buf == NULL)
        {
            pSQL->buf = (char *)_PDSC_MemMalloc(MY_MAX_COL_LEN * MY_MAX_FIELD_LEN);
        }

        memset(pSQL->bindr, 0, pSQL->field_count * sizeof( MYSQL_BIND));
        p = pSQL->buf;
        for(i=0; i<pSQL->field_count; i++)
        {
            pSQL->res_row[i] = p;
            pSQL->bindr[i].is_null= 0;
            pSQL->bindr[i].length= &(pSQL->lengthr[i]);

            if(pSQL->ctype[i] != MYSQL_TYPE_BLOB)
            {
                pSQL->bindr[i].buffer= p;
                p += MY_MAX_FIELD_LEN;
                pSQL->bindr[i].buffer_type= MYSQL_TYPE_STRING;
                pSQL->bindr[i].buffer_length= MY_MAX_FIELD_LEN;
            }
            else
            {
                pSQL->lobselect = 1;
                pSQL->bindr[i].buffer= p + 2;
                p += MY_MAX_LOB_LEN;
                pSQL->bindr[i].buffer_type= MYSQL_TYPE_BLOB;
                pSQL->bindr[i].buffer_length= MY_MAX_LOB_LEN;
            }
        }
        mysql_stmt_bind_result(pSQL->stmthp, pSQL->bindr);
    }
    else
    {
        pSQL->type = 1;  /*非查询*/
    }

    return 0;
}

/*绑定变量*/
int _My_BindParam(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, int param_id, char data_type, void *param)
{
    int len;
    void *p;
    enum enum_field_types ptype;

    p = param;
    switch(data_type)
    {
        case 0:    ptype = MYSQL_TYPE_STRING;    len = MY_MAX_FIELD_LEN; break;
        case 1:    ptype = MYSQL_TYPE_LONG;      len = sizeof(int);      break;
        case 2:    ptype = MYSQL_TYPE_DATETIME;  len =0;
                   p = &(pSQL->timeV[pSQL->timeN]);   pSQL->timeP[pSQL->timeN++] = (char *)param;   break;
        case 3:    ptype = MYSQL_TYPE_FLOAT;     len = sizeof(float);    break;
        case 4:    ptype = MYSQL_TYPE_DOUBLE;    len = sizeof(double);   break;
        case 5:    ptype = MYSQL_TYPE_LONG;      len = sizeof(long);     break;
    }

    pSQL->length[param_id] = len;
    pSQL->bind[param_id].buffer_type = ptype;
    pSQL->bind[param_id].buffer = p;
    pSQL->bind[param_id].buffer_length = len;
    pSQL->bind[param_id].is_null= 0;
    pSQL->bind[param_id].length= &(pSQL->length[param_id]);

    if(param_id + 1 == pSQL->param_num)
    {
        if(mysql_stmt_bind_param(pSQL->stmthp, pSQL->bind))
        {
            DBSession->eno = _MY_GetErrCode(DBSession);
            sprintf(errStr,  "mysql_stmt_bind_param error: %s", mysql_stmt_error(pSQL->stmthp));
            return -1;
        }
    }
    return 0;
}

/*绑定BLOB变量*/
int _MY_BindBlobParam(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, int param_id, void *param, int size)
{
    pSQL->length[param_id] = size;
    pSQL->bind[param_id].buffer_type = MYSQL_TYPE_BLOB;
    pSQL->bind[param_id].buffer = param;
    pSQL->bind[param_id].buffer_length = size;
    pSQL->bind[param_id].is_null= 0;
    pSQL->bind[param_id].length= &(pSQL->length[param_id]);

    if(param_id + 1 == pSQL->param_num)
    {
        if(mysql_stmt_bind_param(pSQL->stmthp, pSQL->bind))
        {
            DBSession->eno = _MY_GetErrCode(DBSession);
            sprintf(errStr,  "mysql_stmt_bind_param error: %s", mysql_stmt_error(pSQL->stmthp));
            return -1;
        }
    }
    return 0;
}

/*执行已解析的SQL*/
int _MY_ExecSql_P(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL)
{
    int i, j=0;
    char *p;
    for(i=0; i<pSQL->param_num; i++)
    {
        if(MYSQL_TYPE_STRING == pSQL->bind[i].buffer_type)
        {
            pSQL->length[i] = strlen((char *)(pSQL->bind[i].buffer));
        }
        else if(MYSQL_TYPE_DATETIME == pSQL->bind[i].buffer_type)
        {
            p = pSQL->timeP[j];
            pSQL->timeV[j].year   = p[0] * 1000 + p[1] * 100 + p[2] * 10 + p[3] - 53328;
            pSQL->timeV[j].month  = p[4]  * 10 + p[5] - 528;
            pSQL->timeV[j].day    = p[6]  * 10 + p[7] - 528;
            pSQL->timeV[j].hour   = p[8]  * 10 + p[9] - 528;
            pSQL->timeV[j].minute = p[10] * 10 + p[11] - 528;
            pSQL->timeV[j].second = p[12] * 10 + p[13] - 528;
            j++;
        }
    }

    if (mysql_stmt_execute(pSQL->stmthp))
    {
        DBSession->eno = _MY_GetErrCode(DBSession);
        sprintf(errStr, "mysql_stmt_execute error: %s", mysql_stmt_error(pSQL->stmthp));
        return -1;
    }

    /*获取操作记录数*/
    if(1 == pSQL->type)
    {
        DBSession->ret_rows = mysql_affected_rows(&(DBSession->conn));
    }
    else
    {
        if(DBSession->buf_flag == 1)
        {
            mysql_stmt_store_result(pSQL->stmthp);
            DBSession->ret_rows = mysql_affected_rows(&(DBSession->conn));
        }
        else
            DBSession->ret_rows = -1;
    }
    return 0;
}

/*释放 SQL 内存 */
void _MY_FreeSql(MY_SQL *pSQL)
{
    if(pSQL->buf != NULL)
    {
        _PDSC_MemFree(pSQL->buf);
        pSQL->buf = NULL;
    }

    if(pSQL->result != NULL)
    {
        mysql_free_result(pSQL->result);
        pSQL->result = NULL;
    }

    if (pSQL->stmthp != NULL)
    {
        mysql_stmt_close(pSQL->stmthp);
        pSQL->stmthp = NULL;
    }
    return;
}

/*设置是否自动提交 0 非自动提交, 1 自动提交*/
void _MY_SetAutoCommit(MY_SESSION *DBSession, int flag)
{
    mysql_autocommit(&(DBSession->conn), flag);
    return;
}

/*设置缓冲模式*/
void _MY_SetBuffFlag(MY_SESSION *DBSession, int flag)
{
    DBSession->buf_flag = flag;
}
