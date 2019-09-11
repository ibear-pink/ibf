
#ifndef _ODBC_H_
#define _ODBC_H_

#ifdef ALTIBASE
    #include <sqlcli.h>
    #define _MYULEN         SQLUINTEGER
    #define _MYLEN          SQLINTEGER
#endif

#ifdef TIMESTEN
    #include <timesten.h>
    #define _MYULEN         SQLULEN
    #define _MYLEN          SQLLEN
#endif

#ifdef SOLID
    #include <solidodbc3.h>
    #define _MYULEN         SQLUINTEGER
    #define _MYLEN          SQLINTEGER
#endif

#define O_BUF_LEN           262144   /*!< 缓冲模式的最大BUF尺寸 256K*/
#define O_MAX_FIELD_LEN        520   /*!< 每个字段的最大长度*/
#define O_MAX_COL_LEN           64   /*!< 每个表的最多列数*/

#define O_MAX_VALUE_LEN        256   /*!< 最大字段长度*/
#define O_MAX_DATE_LEN		    21	 /*!< date长度*/
#define O_MAX_DIGITAL_LEN		30	 /*!< 数字类型长度*/
#define O_MAX_LOB_LEN         4096   /*!< BLOB类型长度*/

#define O_MAX_BUFF_LEN       10240   /*!< 1条记录的最大BUFF长度*/


typedef char **       OSQL_ROW;

typedef struct _o_buf
{
    char buf[O_BUF_LEN];
    int len;
    struct _o_buf *next;
}O_BUF;

/*解析后的SQL*/
typedef struct _o_sql
{
    SQLHSTMT hstmt;
    O_BUF *mem_buf;
    O_BUF *cur_buf;
    char  *res_row[O_MAX_COL_LEN];      /*!< 存放一行select结果的各字段指针*/
    char  *resp_row[O_MAX_COL_LEN];     /*!< 存放一行select结果的各字段指针*/
    char  *buf;                         /*!< */
    int   read_pos;                     /*!< select结果已读字节的偏移*/
    short param_num;                    /*!< 参数个数*/
    int   field_count;                  /*!< select 字段个数*/
    int   param_szie[O_MAX_COL_LEN];    /*!< 绑定参数的长度*/
    short col_type[O_MAX_COL_LEN];      /*!< select的字段类型*/
    _MYULEN col_len[O_MAX_COL_LEN];     /*!< select的字段长度*/
    char open_flag;
    char lobselect;
}O_SQL;

typedef struct _o_session
{
    SQLHDBC hdbc;
    SQLHENV henv;
    O_SQL   *pSQL;
    O_SQL   dSql;
    SQLHSTMT hstmt_commit;
    SQLHSTMT hstmt_rollback;

    int     eno;                             /*!< 错误编号*/
    long    ret_rows;                        /*!< 操作记录数*/
    int     field_count;                     /*!< select 字段个数*/
    char    buf_flag;                        /*!< 0非缓冲  1 缓冲*/
}O_SESSION;

/****************************************************************************/

/*设置是否自动提交，登录成功后调用, 0 非自动提交, 1 自动提交*/
extern void _O_SetAutoCommit(O_SESSION *DBSession, int flag);

/*登陆数据库*/
extern int _O_Connect(char *errStr, O_SESSION *DBSession, char *user, char *password, char *server);

/*断开*/
extern void _O_Disconnect(O_SESSION *DBSession);

/*直接执行SQL*/
extern int _O_ExecSql(char *errStr, O_SESSION *DBSession, const char *inSql);

/*SQL解析*/
extern int _O_PrepareSql(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, const char *inSql);

/*绑定变量*/
extern int _O_BindParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, char data_type, void *param);

/*绑定BLOB变量*/
extern int _O_BindBlobParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, void *param, int size);

/*执行已解析的SQL*/
extern int _O_ExecSql_P(char *errStr, O_SESSION *DBSession, O_SQL *pSQL);

/*提交*/
extern int _O_EndTrans(char *errStr, O_SESSION *DBSession, int flag);

/*逐条提取数据*/
extern OSQL_ROW _O_FetchData(O_SESSION *DBSession, O_SQL *pSQL);

/*设置缓冲模式*/
extern void _O_SetBuffFlag(O_SESSION *DBSession, int flag);

/*释放 SQL 内存 */
extern void _O_FreeSql(O_SQL *pSQL, char flag);

#endif
