
/*!
 *  @file Database.h 用于MYSQL数据库的连接及对数据库的各种操作
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2002-12-05
 */

#ifndef _MYSQL_H_
#define _MYSQL_H_

#include <mysql.h>

#define MY_MAX_FIELD_LEN        520   /*!< 每个字段的最大长度*/
#define MY_MAX_COL_LEN           64   /*!< 每个表的最多列数*/
#define MY_MAX_LOB_LEN         4096   /*!< BLOB类型长度*/

/*! @brief Mysql连接结构*/
typedef struct _my_session
{
    MYSQL       conn;                            /*!< mysql连接*/
    MYSQL_STMT  *stmt;                           /*!< SQL句柄*/
    MYSQL_RES   *result;                         /*!< 存放select结果*/
    enum enum_field_types ctype[MY_MAX_COL_LEN]; /*!< select结果每个字段的类型*/
    char        *buf;                            /*!< 存放select结果*/
    char        *res_row[MY_MAX_COL_LEN];        /*!< 存放一行select结果的各字段指针*/
    long        ret_rows;                        /*!< 操作记录数*/
    int         field_count;                     /*!< select 字段个数*/
    int         eno;
    char        buf_flag;                        /*!< 0非缓冲  1 缓冲*/
    char        lobselect;
}MY_SESSION;

/*! @brief SQL结构*/
typedef struct _my_sql
{
    MYSQL_STMT  *stmthp;                         /*!< SQL句柄*/
    char        *buf;                            /*!< 存放select结果*/
    char        *timeP[10];                      /*!< 存放日期绑定字段对应的字符串指针*/
    MYSQL_TIME  timeV[10];                       /*!< 存放日期绑定字段实际绑定的指针*/
    enum enum_field_types ctype[MY_MAX_COL_LEN]; /*!< 存放select字段的类型*/
    MYSQL_RES   *result;                         /*!< 存放select查询结果*/
    MYSQL_BIND  bind[MY_MAX_COL_LEN];            /*!< 用于参数绑定*/
    MYSQL_BIND  bindr[MY_MAX_COL_LEN];           /*!< 用于select结果绑定*/
    unsigned long length[MY_MAX_COL_LEN];        /*!< 用于参数绑定*/
    unsigned long lengthr[MY_MAX_COL_LEN];       /*!< 用于select返回值的长度*/
    char        *res_row[MY_MAX_COL_LEN];        /*!< 存放一行select结果的各字段指针*/
    int         field_count;                     /*!< select 字段个数*/
    int         param_num;                       /*!< 参数个数*/
    short       timeN;                           /*!< 存放当前已经绑定到第几个日期字段了*/
    char        type;                            /*!< 0 select, 1 修改*/
    char        lobselect;
}MY_SQL;

/*****************************************************************************************************/

/*登录数据库*/
extern int _MY_Connect(char *errStr, MY_SESSION *DBSession, char *user, char *password, char *server);

/*断开*/
extern void _MY_Disconnect(MY_SESSION *DBSession);

/*直接执行SQL*/
extern int _MY_ExecSql(char *errStr, MY_SESSION *DBSession, const char *inSql);

/*逐条提取数据*/
extern MYSQL_ROW _MY_FetchData(MY_SESSION *DBSession, MY_SQL *pSQL);

/*提交*/
extern int _MY_EndTrans(char *errStr, MY_SESSION *DBSession, int flag);

/*SQL解析*/
extern int _MY_PrepareSql(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, const char *inSql);

/*绑定变量*/
extern int _My_BindParam(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, int param_id, char data_type, void *param);

/*绑定BLOB变量*/
extern int _MY_BindBlobParam(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL, int param_id, void *param, int size);

/*执行已解析的SQL*/
extern int _MY_ExecSql_P(char *errStr, MY_SESSION *DBSession, MY_SQL *pSQL);

/*释放 SQL 内存 */
extern void _MY_FreeSql(MY_SQL *pSQL);

/*设置是否自动提交 0 非自动提交, 1 自动提交*/
extern void _MY_SetAutoCommit(MY_SESSION *DBSession, int flag);

/*设置缓冲模式*/
extern void _MY_SetBuffFlag(MY_SESSION *DBSession, int flag);

#endif
