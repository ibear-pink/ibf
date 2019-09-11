
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

#define O_BUF_LEN           262144   /*!< ����ģʽ�����BUF�ߴ� 256K*/
#define O_MAX_FIELD_LEN        520   /*!< ÿ���ֶε���󳤶�*/
#define O_MAX_COL_LEN           64   /*!< ÿ������������*/

#define O_MAX_VALUE_LEN        256   /*!< ����ֶγ���*/
#define O_MAX_DATE_LEN		    21	 /*!< date����*/
#define O_MAX_DIGITAL_LEN		30	 /*!< �������ͳ���*/
#define O_MAX_LOB_LEN         4096   /*!< BLOB���ͳ���*/

#define O_MAX_BUFF_LEN       10240   /*!< 1����¼�����BUFF����*/


typedef char **       OSQL_ROW;

typedef struct _o_buf
{
    char buf[O_BUF_LEN];
    int len;
    struct _o_buf *next;
}O_BUF;

/*�������SQL*/
typedef struct _o_sql
{
    SQLHSTMT hstmt;
    O_BUF *mem_buf;
    O_BUF *cur_buf;
    char  *res_row[O_MAX_COL_LEN];      /*!< ���һ��select����ĸ��ֶ�ָ��*/
    char  *resp_row[O_MAX_COL_LEN];     /*!< ���һ��select����ĸ��ֶ�ָ��*/
    char  *buf;                         /*!< */
    int   read_pos;                     /*!< select����Ѷ��ֽڵ�ƫ��*/
    short param_num;                    /*!< ��������*/
    int   field_count;                  /*!< select �ֶθ���*/
    int   param_szie[O_MAX_COL_LEN];    /*!< �󶨲����ĳ���*/
    short col_type[O_MAX_COL_LEN];      /*!< select���ֶ�����*/
    _MYULEN col_len[O_MAX_COL_LEN];     /*!< select���ֶγ���*/
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

    int     eno;                             /*!< ������*/
    long    ret_rows;                        /*!< ������¼��*/
    int     field_count;                     /*!< select �ֶθ���*/
    char    buf_flag;                        /*!< 0�ǻ���  1 ����*/
}O_SESSION;

/****************************************************************************/

/*�����Ƿ��Զ��ύ����¼�ɹ������, 0 ���Զ��ύ, 1 �Զ��ύ*/
extern void _O_SetAutoCommit(O_SESSION *DBSession, int flag);

/*��½���ݿ�*/
extern int _O_Connect(char *errStr, O_SESSION *DBSession, char *user, char *password, char *server);

/*�Ͽ�*/
extern void _O_Disconnect(O_SESSION *DBSession);

/*ֱ��ִ��SQL*/
extern int _O_ExecSql(char *errStr, O_SESSION *DBSession, const char *inSql);

/*SQL����*/
extern int _O_PrepareSql(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, const char *inSql);

/*�󶨱���*/
extern int _O_BindParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, char data_type, void *param);

/*��BLOB����*/
extern int _O_BindBlobParam(char *errStr, O_SESSION *DBSession, O_SQL *pSQL, int param_id, void *param, int size);

/*ִ���ѽ�����SQL*/
extern int _O_ExecSql_P(char *errStr, O_SESSION *DBSession, O_SQL *pSQL);

/*�ύ*/
extern int _O_EndTrans(char *errStr, O_SESSION *DBSession, int flag);

/*������ȡ����*/
extern OSQL_ROW _O_FetchData(O_SESSION *DBSession, O_SQL *pSQL);

/*���û���ģʽ*/
extern void _O_SetBuffFlag(O_SESSION *DBSession, int flag);

/*�ͷ� SQL �ڴ� */
extern void _O_FreeSql(O_SQL *pSQL, char flag);

#endif
