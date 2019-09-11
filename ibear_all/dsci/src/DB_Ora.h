
/*!
 *  @file Database.h ����ORACLE���ݿ�����Ӽ������ݿ�ĸ��ֲ���
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2002-12-05
 */

#ifndef _ORA_H_
#define _ORA_H_

#include <oci.h>

#define ORA_SUCCESS                0   /*!< �ɹ���־*/
#define ORA_FAILURE               -1   /*!< ʧ�ܱ�־*/
#define ORA_NO_DATA_FOUND       -100   /*!< û������*/

#define ORA_MAX_FIELD_LEN        520   /*!< ÿ���ֶε���󳤶�*/
#define ORA_MAX_COL_LEN           64   /*!< ÿ������������*/

typedef char **       ORA_ROW;
#define ORA_BUF_LEN           262144   /*!< ����ģʽ�����BUF�ߴ� 256K*/
#define ORA_MAX_BUFF_LEN       10240   /*!< 1����¼�����BUFF����*/
#define ORA_MAX_LOB_LEN         4096   /*!< BLOB���ͳ���*/

/*! @brief ����ģʽ��̬�ڴ�*/
typedef struct _ora_buf
{
    char buf[ORA_BUF_LEN];
    int len;
    struct _ora_buf *next;
}ORA_BUF;

/*! @brief ����ģʽʹ��*/
typedef struct ora_query_buf
{
    ORA_BUF *mem_buf;
    ORA_BUF *cur_buf;
    char  *resp_row[ORA_MAX_COL_LEN];   /*!< ���һ��select����ĸ��ֶ�ָ��*/
    int   read_pos;                     /*!< select����Ѷ��ֽڵ�ƫ��*/
}ORA_QUERY_BUF;

/*! @brief SQL�ṹ���ֹ��ó�Ա*/
typedef struct _ora_tsql
{
    char          *buf;                           /*!< ���select���*/
    char          *res_row[ORA_MAX_COL_LEN];      /*!< ���һ��select����ĸ��ֶ�ָ��*/
    OCILobLocator *tlob[ORA_MAX_COL_LEN];
    ub2           col_type[ORA_MAX_COL_LEN];      /*!< select���ֶ�����*/
    ORA_QUERY_BUF *qbuf;
    OCIStmt       *stmthp;
    int           field_count;                    /*!< select �ֶθ���*/
    char          lobselect;
}ORA_TSQL;

/*! @brief ORACLE���ӽṹ*/
typedef struct _ora_session
{
    OCIEnv      *envhp;
    OCIServer   *srvhp;
    OCIError    *errhp;
    OCISvcCtx   *svchp;
    OCISession  *authp;
    ORA_TSQL    tsql;
    long        ret_rows;                       /*!< ������¼��*/
    ub4         flag;                           /*!< �Ƿ��Զ��ύ*/
    char        buf_flag;                       /*!< 0�ǻ���  1 ����*/
}ORA_SESSION;

/*! @brief SQL�ṹ*/
typedef struct _ora_sql
{
    ORA_TSQL    tsql;
    OCIBind     *bindHandle[ORA_MAX_COL_LEN];   /*!< �����󶨵�ָ��*/
    void        *bind_p[ORA_MAX_COL_LEN];
    int         bind_size[ORA_MAX_COL_LEN];
    char        bind_type[ORA_MAX_COL_LEN];
    ORA_SESSION *db;
    int         param_num;                      /*!< ��������*/
    char        type;                           /*!< 0 select, 1 �޸�*/
    char        lobupdate;
}ORA_SQL;


/*��ȡ���ݿ�������*/
extern int _ORA_GetErrCode(ORA_SESSION *DBSession);

/*��¼���ݿ�*/
extern int _ORA_Connect(char *errStr, ORA_SESSION *DBSession, char *user, char *password, char *server);

/*�Ͽ����ݿ�*/
extern void _ORA_Disconnect(ORA_SESSION *DBSession);

/*SQL����*/
extern int _ORA_PrepareSql(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, const char *inSql);

/*�󶨱���*/
extern int _ORA_BindParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, char data_type, void *param);

/*��BLOB����*/
extern int _ORA_BindBlobParam(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL, int param_id, void *param, int size);

/*ִ���ѽ�����SQL*/
extern int _ORA_ExecSql_P(char *errStr, ORA_SESSION *DBSession, ORA_SQL *pSQL);

/*�ͷ� SQL �ڴ� */
extern void _ORA_FreeSql(ORA_SQL *pSQL);

/*�����Ƿ��Զ��ύ 0 ���Զ��ύ, 1 �Զ��ύ*/
extern void _ORA_SetAutoCommit(ORA_SESSION *DBSession, int flag);

/*���û���ģʽ*/
extern void _ORA_SetBuffFlag(ORA_SESSION *DBSession, int flag);

/*����һ������  0 �ύ�� 1 �ع�*/
extern int _ORA_EndTrans(char *errStr, ORA_SESSION *DBSession, int flag);

/*������ѯ��¼*/
extern ORA_ROW _ORA_FetchData(ORA_SESSION *DBSession, ORA_SQL *pSQL);

/*ִ��oracle���*/
extern int _ORA_ExecSql(char *errStr, ORA_SESSION *DBSession, const char *inSql);


#endif
