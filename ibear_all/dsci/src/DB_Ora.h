
/*!
 *  @file Database.h 用于ORACLE数据库的连接及对数据库的各种操作
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2002-12-05
 */

#ifndef _ORA_H_
#define _ORA_H_

#include <oci.h>

#define ORA_SUCCESS                0   /*!< 成功标志*/
#define ORA_FAILURE               -1   /*!< 失败标志*/
#define ORA_NO_DATA_FOUND       -100   /*!< 没有数据*/

#define ORA_MAX_FIELD_LEN        520   /*!< 每个字段的最大长度*/
#define ORA_MAX_COL_LEN           64   /*!< 每个表的最多列数*/

typedef char **       ORA_ROW;
#define ORA_BUF_LEN           262144   /*!< 缓冲模式的最大BUF尺寸 256K*/
#define ORA_MAX_BUFF_LEN       10240   /*!< 1条记录的最大BUFF长度*/
#define ORA_MAX_LOB_LEN         4096   /*!< BLOB类型长度*/

/*! @brief 缓冲模式动态内存*/
typedef struct _ora_buf
{
    char buf[ORA_BUF_LEN];
    int len;
    struct _ora_buf *next;
}ORA_BUF;

/*! @brief 缓冲模式使用*/
typedef struct ora_query_buf
{
    ORA_BUF *mem_buf;
    ORA_BUF *cur_buf;
    char  *resp_row[ORA_MAX_COL_LEN];   /*!< 存放一行select结果的各字段指针*/
    int   read_pos;                     /*!< select结果已读字节的偏移*/
}ORA_QUERY_BUF;

/*! @brief SQL结构部分公用成员*/
typedef struct _ora_tsql
{
    char          *buf;                           /*!< 存放select结果*/
    char          *res_row[ORA_MAX_COL_LEN];      /*!< 存放一行select结果的各字段指针*/
    OCILobLocator *tlob[ORA_MAX_COL_LEN];
    ub2           col_type[ORA_MAX_COL_LEN];      /*!< select的字段类型*/
    ORA_QUERY_BUF *qbuf;
    OCIStmt       *stmthp;
    int           field_count;                    /*!< select 字段个数*/
    char          lobselect;
}ORA_TSQL;

/*! @brief ORACLE连接结构*/
typedef struct _ora_session
{
    OCIEnv      *envhp;
    OCIServer   *srvhp;
    OCIError    *errhp;
    OCISvcCtx   *svchp;
    OCISession  *authp;
    ORA_TSQL    tsql;
    long        ret_rows;                       /*!< 操作记录数*/
    ub4         flag;                           /*!< 是否自动提交*/
    char        buf_flag;                       /*!< 0非缓冲  1 缓冲*/
}ORA_SESSION;

/*! @brief SQL结构*/
typedef struct _ora_sql
{
    ORA_TSQL    tsql;
    OCIBind     *bindHandle[ORA_MAX_COL_LEN];   /*!< 参数绑定的指针*/
    void        *bind_p[ORA_MAX_COL_LEN];
    int         bind_size[ORA_MAX_COL_LEN];
    char        bind_type[ORA_MAX_COL_LEN];
    ORA_SESSION *db;
    int         param_num;                      /*!< 参数个数*/
    char        type;                           /*!< 0 select, 1 修改*/
    char        lobupdate;
}ORA_SQL;


/*获取数据库错误代码*/
extern int _ORA_GetErrCode(ORA_SESSION *DBSession);

/*登录数据库*/
extern int _ORA_Connect(char *errStr, ORA_SESSION *DBSession, char *user, char *password, char *server);

/*断开数据库*/
extern void _ORA_Disconnect(ORA_SESSION *DBSession);

/*SQL解析*/
extern int _ORA_PrepareSql(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, const char *inSql);

/*绑定变量*/
extern int _ORA_BindParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, char data_type, void *param);

/*绑定BLOB变量*/
extern int _ORA_BindBlobParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, void *param, int size);

/*执行已解析的SQL*/
extern int _ORA_ExecSql_P(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL);

/*释放 SQL 内存 */
extern void _ORA_FreeSql(ORA_SQL *pSQL);

/*设置是否自动提交 0 非自动提交, 1 自动提交*/
extern void _ORA_SetAutoCommit(ORA_SESSION *DBSession, int flag);

/*设置缓冲模式*/
extern void _ORA_SetBuffFlag(ORA_SESSION *DBSession, int flag);

/*结束一个事务  0 提交， 1 回滚*/
extern int _ORA_EndTrans(char *errStr, ORA_SESSION *DBSession, int flag);

/*逐条查询记录*/
extern ORA_ROW _ORA_FetchData(ORA_SESSION *DBSession, ORA_SQL *pSQL);

/*执行oracle语句*/
extern int _ORA_ExecSql(char *errStr, ORA_SESSION *DBSession, const char *inSql);


#endif
