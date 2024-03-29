

#ifndef _DSCI_H_
#define _DSCI_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define     DSCI_VERSION_NUMBER   "2.0.0.1"
#define     DSCI_RELEASE_DATE     "Aug 15 2018"

/*
2012-05-01  wangjs  1.0.0.3   增加DSCI_init_direct函数
2012-05-14  wangjs  1.0.0.4   在init时，判断是否支持此类数据库
2012-05-16  wangjs  1.0.0.5   DSCI_query_p函数未设置curSql，导致oracle和mysql的DSCI_fetch_row有问题
2012-08-28  wangjs  1.0.0.6   修改ODBC的_O_EndTrans函数，改成执行commit或者rollback
2012-09-20  wangjs  1.0.0.7   修改_CDSCI_Query_p的iMDB的LOB绑定方式，兼容imdb的各个版本
2012-12-21  wangjs  1.0.0.8   修改_ORA_FetchData_P函数 解决字段空值的问题
2013-06-06  wangjs  1.0.0.9   char dbID 改成 signed char dbID，因为AIX系统，char默认是无符号的。
2014-07-21  wangjs  1.0.1.0   增加对DMDB的支持

2016-12-19  wangjs  2.0.0.0   修改对新版本DMDB的LOB字段的支持
2018-08-15  wangjs  2.0.0.1   修改内部函数名，避免和DMDB的内部函数重名
*/

typedef void *        DSCI;
typedef void *        DSCI_SQL;
typedef char **       DSCI_ROW;

#define DSCI_TYPE_CHAR         0     /*!< char型数据类型定义 */
#define DSCI_TYPE_INT          1     /*!< int型数据类型定义 */
#define DSCI_TYPE_DATE         2     /*!< time_t型数据类型定义 */
#define DSCI_TYPE_FLOAT        3     /*!< fload型数据类型定义 */
#define DSCI_TYPE_DOUBLE       4     /*!< double型数据类型定义 */
#define DSCI_TYPE_LONG         5     /*!< long型数据类型定义 */

/*****************************************************************************/
/*获取DSC接口的版本号*/
extern void DSCI_ver(char *version, char *r_date);

/*读取错误代码*/
extern int DSCI_ecode(DSCI *pDSC);

/*读取DSC错误信息*/
extern char *DSCI_error(DSCI *pDSC);

/*连接初始化*/
extern int DSCI_init(char *errStr, DSCI *pDSC);

/*连接初始化，直接生成数据库信息*/
extern int DSCI_init_direct(char *errStr, DSCI *pDSC, char *dbStr);

/*直接执行SQL命令*/
extern int DSCI_query(DSCI *pDSC, const char *sqlStr);

/*预解析SQL命令*/
extern int DSCI_prepare_sql(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr);

/*直接执行SQL命令(指定库序号)*/
extern int DSCI_query_f(DSCI *pDSC, const char *sqlStr, int dbNo);

/*预解析SQL命令(指定库序号)*/
extern int DSCI_prepare_sql_f(DSCI *pDSC, DSCI_SQL *pSQL, const char *sqlStr, int dbNo);

/*绑定参数*/
extern int DSCI_bind_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, char data_type, void *param);

/*绑定LOB字段*/
extern int DSCI_bind_blob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param, int size);

/*绑定CLOB字段*/
extern int DSCI_bind_clob_param(DSCI *pDSC, DSCI_SQL *pSQL, int param_id, void *param);

/*获取LOB字段的字节长度*/
extern int DSCI_lob_size(char *pData);

/*获取LOB字段实际内容的指针*/
extern char *DSCI_lob_getp(char *pData);

/*获取LOB字段是否为CLOB*/
extern int DSCI_lob_isclob(char *p_DATA);

/*获取LOB字段，返回LOB字段长度*/
extern int DSCI_lob_get(void *outP, char *pData);

/*执行解析后的SQL命令*/
extern int DSCI_query_p(DSCI *pDSC, DSCI_SQL *pSQL);

/*释放预解析的SQL*/
extern void DSCI_free_sql(DSCI_SQL *pSQL);

/*0 提交,  1 回滚*/
extern int DSCI_end_trans(DSCI *pDSC, int flag);

/*自定义事务库  0 提交,  1 回滚*/
extern int DSCI_end_trans_sig(DSCI *pDSC, int flag, int dbNum);

/*提取一行数据*/
extern DSCI_ROW DSCI_fetch_row(DSCI *pDSC);

/*提取一行数据， 根据指定的预解析SQL*/
extern DSCI_ROW DSCI_fetch_row_s(DSCI *pDSC, DSCI_SQL *pSQL);

/*获取select字段个数*/
extern int DSCI_num_fields(DSCI *pDSC);

/*设置是否自动提交*/
extern int DSCI_set_auto_commit(DSCI *pDSC, int flag);

/*设置select缓冲模式 1 是， 0 否*/
extern void DSCI_set_Buffer_flag(DSCI *pDSC, int flag);

/*设置日志模式, on 写日志，off 不写日志，sync 实时写磁盘*/
extern void DSCI_set_log_mode(DSCI *pDSC, const char *inMode);

/*操作记录数*/
extern long DSCI_num_rows(DSCI *pDSC);

/*退出, 释放内存*/
extern void DSCI_quit(DSCI *pDSC);

#if defined(__cplusplus)
}
#endif

#endif
