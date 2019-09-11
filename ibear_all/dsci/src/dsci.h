

#ifndef _DSCI_H_
#define _DSCI_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define     DSCI_VERSION_NUMBER   "2.0.0.1"
#define     DSCI_RELEASE_DATE     "Aug 15 2018"

/*
2012-05-01  wangjs  1.0.0.3   ����DSCI_init_direct����
2012-05-14  wangjs  1.0.0.4   ��initʱ���ж��Ƿ�֧�ִ������ݿ�
2012-05-16  wangjs  1.0.0.5   DSCI_query_p����δ����curSql������oracle��mysql��DSCI_fetch_row������
2012-08-28  wangjs  1.0.0.6   �޸�ODBC��_O_EndTrans�������ĳ�ִ��commit����rollback
2012-09-20  wangjs  1.0.0.7   �޸�_CDSCI_Query_p��iMDB��LOB�󶨷�ʽ������imdb�ĸ����汾
2012-12-21  wangjs  1.0.0.8   �޸�_ORA_FetchData_P���� ����ֶο�ֵ������
2013-06-06  wangjs  1.0.0.9   char dbID �ĳ� signed char dbID����ΪAIXϵͳ��charĬ�����޷��ŵġ�
2014-07-21  wangjs  1.0.1.0   ���Ӷ�DMDB��֧��

2016-12-19  wangjs  2.0.0.0   �޸Ķ��°汾DMDB��LOB�ֶε�֧��
2018-08-15  wangjs  2.0.0.1   �޸��ڲ��������������DMDB���ڲ���������
*/

typedef void *        DSCI;
typedef void *        DSCI_SQL;
typedef char **       DSCI_ROW;

#define DSCI_TYPE_CHAR         0     /*!< char���������Ͷ��� */
#define DSCI_TYPE_INT          1     /*!< int���������Ͷ��� */
#define DSCI_TYPE_DATE         2     /*!< time_t���������Ͷ��� */
#define DSCI_TYPE_FLOAT        3     /*!< fload���������Ͷ��� */
#define DSCI_TYPE_DOUBLE       4     /*!< double���������Ͷ��� */
#define DSCI_TYPE_LONG         5     /*!< long���������Ͷ��� */

/*****************************************************************************/
/*��ȡDSC�ӿڵİ汾��*/
extern void DSCI_ver(char *version, char *r_date);

/*��ȡ�������*/
extern int DSCI_ecode(DSCI *pDSC);

/*��ȡDSC������Ϣ*/
extern char *DSCI_error(DSCI *pDSC);

/*���ӳ�ʼ��*/
extern int DSCI_init(char *errStr, DSCI *pDSC);

/*���ӳ�ʼ����ֱ���������ݿ���Ϣ*/
extern int DSCI_init_direct(char *errStr, DSCI *pDSC, char *dbStr);

/*ֱ��ִ��SQL����*/
extern int DSCI_query(DSCI *pDSC, const char *sqlStr);

/*Ԥ����SQL����*/
extern int DSCI_prepare_sql(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr);

/*ֱ��ִ��SQL����(ָ�������)*/
extern int DSCI_query_f(DSCI *pDSC, const char *sqlStr, int dbNo);

/*Ԥ����SQL����(ָ�������)*/
extern int DSCI_prepare_sql_f(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr, int dbNo);

/*�󶨲���*/
extern int DSCI_bind_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, char data_type, void *param);

/*��LOB�ֶ�*/
extern int DSCI_bind_blob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param, int size);

/*��CLOB�ֶ�*/
extern int DSCI_bind_clob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param);

/*��ȡLOB�ֶε��ֽڳ���*/
extern int DSCI_lob_size(char *pData);

/*��ȡLOB�ֶ�ʵ�����ݵ�ָ��*/
extern char *DSCI_lob_getp(char *pData);

/*��ȡLOB�ֶ��Ƿ�ΪCLOB*/
extern int DSCI_lob_isclob(char *p_DATA);

/*��ȡLOB�ֶΣ�����LOB�ֶγ���*/
extern int DSCI_lob_get(void *outP, char *pData);

/*ִ�н������SQL����*/
extern int DSCI_query_p(DSCI *pDSC, DSCI_SQL *pSQL);

/*�ͷ�Ԥ������SQL*/
extern void DSCI_free_sql(DSCI_SQL *pSQL);

/*0 �ύ,  1 �ع�*/
extern int DSCI_end_trans(DSCI *pDSC, int flag);

/*�Զ��������  0 �ύ,  1 �ع�*/
extern int DSCI_end_trans_sig(DSCI *pDSC, int flag, int dbNum);

/*��ȡһ������*/
extern DSCI_ROW DSCI_fetch_row(DSCI *pDSC);

/*��ȡһ�����ݣ� ����ָ����Ԥ����SQL*/
extern DSCI_ROW DSCI_fetch_row_s(DSCI *pDSC, DSCI_SQL *pSQL);

/*��ȡselect�ֶθ���*/
extern int DSCI_num_fields(DSCI *pDSC);

/*�����Ƿ��Զ��ύ*/
extern int DSCI_set_auto_commit(DSCI *pDSC, int flag);

/*����select����ģʽ 1 �ǣ� 0 ��*/
extern void DSCI_set_Buffer_flag(DSCI *pDSC, int flag);

/*������־ģʽ, on д��־��off ��д��־��sync ʵʱд����*/
extern void DSCI_set_log_mode(DSCI *pDSC, const char *inMode);

/*������¼��*/
extern long DSCI_num_rows(DSCI *pDSC);

/*�˳�, �ͷ��ڴ�*/
extern void DSCI_quit(DSCI *pDSC);

#if defined(__cplusplus)
}
#endif

#endif
