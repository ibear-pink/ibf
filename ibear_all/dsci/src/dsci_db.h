
#ifndef _DSC_DB_H_
#define _DSC_DB_H_

#include <time.h>

#ifdef IMDB
#include "si_dci.h"
#endif

#ifdef DMDB
#include "ddci.h"
#endif

#ifdef ORACLE
#include "DB_Ora.h"
#endif

#ifdef MYSQLDB
#include "DB_Mysql.h"
#endif

#ifdef ALTIBASE
    #define ODBC
#endif

#ifdef TIMESTEN
    #define ODBC
#endif

#ifdef SOLID
    #define ODBC
#endif

#ifdef ODBC
#include "DB_Odbc.h"
#endif

/*! @brief ���ݿ�����*/
#define _T_IMDB          1
#define _T_ORACLE        2
#define _T_ALTIBASE      3
#define _T_MYSQL         4
#define _T_DMDB          5

/*! @brief �ڲ��������*/
#define _ERR_NULL         -99999  /*!< ���ݿ�ָ��Ϊ��*/

#define _ERR_DB_P         -99901  /*!< malloc SI_DCI ʧ��*/
#define _ERR_DCI_INIT     -99902  /*!< dci_init ʧ��*/
#define _ERR_SQL_P        -99903  /*!< malloc SQL ʧ��*/
#define _ERR_DB_ID1       -99904  /*!< ���ܸ��ݱ�����ȡ���ݿ�ID*/
#define _ERR_DB_ID2       -99905  /*!< �Ƿ����ݿ�ID*/
#define _ERR_INVALID_SQL  -99906  /*!< DSCI_SQLû�н���*/
#define _ERR_SQLINFO_P    -99907  /*!< malloc SQL_INFO ʧ��*/

/*! @brief ���ݿ���Ϣ*/
typedef struct _db_info
{
    void *dbp;                                /*!< ��Ӧ�������ݿ������ָ��*/
    char name[8];
    char user[32], pass[32], server[256];
    time_t connTime;                          /*!< ���ӳɹ���ʱ��*/
    char logMode[8];                          /*!< ��־ģʽ on д��־��off ��д��־��sync ʵʱд���� */
    char type;                                /*!< ���ݿ�����*/
    char cFlag;                               /*!< �Ƿ��Ѿ����ӣ� 0 �� 1 ��*/
    char uFlag;                               /*!< �Ƿ��޸Ĺ����ݣ� 0 �� 1 ��*/
    char buffFlag;                            /*!< �Ƿ񻺳�ģʽ�� 0 �� 1 ��*/
    char acommitFlag;                         /*!< �Ƿ��Զ��ύ�� 0 �� 1 ��*/
}DB_INFO;

/*! @brief �������SQL*/
typedef struct _sql_info
{
    void *psql;                               /*!< SQL����ָ��*/
    DB_INFO *db;                              /*!< ��SQL��Ӧ�����ݿ���Ϣ*/
    time_t connTime;                          /*!< ���ӳɹ���ʱ��*/
    char type;                                /*!< 0 select, 1 �޸�*/
}SQL_INFO;

/*! @brief ����Ϣ*/
typedef struct _tab_info
{
    char name[32];                            /*!< ����*/
    signed char dbID;                         /*!< ��Ӧ�ĸ��� 0 ��ʼ*/
}TAB_INFO;

typedef struct _dsc_db
{
    char error[256];                          /*!< ������Ϣ */
    long ret_rows;                            /*!< ������¼�� */
    int field_count;                          /*!< select �ֶθ���*/
    int eno;                                  /*!< ������*/

    DB_INFO *dbInfo;                          /*!< �������ݿ���Ϣ*/
    TAB_INFO *tabInfo;                        /*!< ���б���Ϣ*/
    SQL_INFO *curSql;                         /*!< ��ǰSQL*/
    SQL_INFO dSql;                            /*!< ������ֱ��ʹ�õ�SQL*/

    short dbNum;                              /*!< �����*/
    short tabNum;                             /*!< ������*/
    short defaultDB;                          /*!< Ĭ�����ݿ� */
}DSC_DB;

/*********************************   pub   **************************************/
/*��ȡ���ݿ���Ϣ*/
extern int _DSCI_GetDbInfo(char *errStr, char *dscPath, DSC_DB *pDB);

/*ֱ���������ݿ���Ϣ*/
extern int _DSCI_CreateDbInfo(char *errStr, char *dbStr, DSC_DB *pDB);

/*��ȡ����Ϣ*/
extern int _DSCI_GetTabInfo(char *errStr, char *dscPath, DSC_DB *pDB);

/*���ݱ�����ȡ���ݿ���*/
extern short _DSCI_GetDBNo(const char *sqlStr, DSC_DB *pDB, char *type, int dbNo);

/*����SQL����DB, ������*/
extern int _DSCI_GetSqlDb(DB_INFO **db, DSC_DB *pDB, const char *sqlStr, char *sqlType, int dbNo);

extern void _DSCI_free_sql(DSCI_SQL *pSQL, DB_INFO *db);

/*********************************   case   **************************************/
/*���ӿ�*/
extern int _CDSC_ConnIMDB(char *errStr, DSC_DB *pDB, DB_INFO *db);

/*ֱ�Ӳ�ѯ*/
extern int _CDSC_Query(DSC_DB *pDB, DB_INFO *db, const char *sqlStr);

/*Ԥ����*/
extern int _CDSC_Prepare(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, const char *sqlStr);

/*�󶨲���*/
extern int _CDSC_Bind(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, char data_type, void *param);

/*��BLOB����*/
extern int _CDSC_Bind_Blob(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, void *param, int size);

/*��CLOB����*/
extern int _CDSC_Bind_Clob(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL, int param_id, void *param);

/*ִ��Ԥ����SQL*/
extern int _CDSC_Query_p(DSC_DB *pDB, DB_INFO *db, SQL_INFO *mSQL);

/*ִ��Ԥ����SQL*/
extern void _CDSC_FreeSql(SQL_INFO *mSQL, DB_INFO *db);

/*�ύ*/
extern int _CDSC_EndT(DSC_DB *pDB, DB_INFO *db, int flag);

/*�����Զ��ύ*/
extern void _CDSC_AutoCommit(DB_INFO *db, int flag);

/*����select����ģʽ 1 �ǣ� 0 ��*/
extern void _CDSC_SetBuffFlag(DB_INFO *db, int flag);

/*������־ģʽ, on д��־��off ��д��־��sync ʵʱд����*/
extern void _CDSC_SetLogMode(DB_INFO *db, const char *inMode);

/*�˳����ӣ��ͷ��ڴ�*/
extern void _CDSC_FreeDB(DB_INFO *db);

extern DSCI_ROW _CDSC_Fetch(SQL_INFO *mSQL, int flag);

#endif