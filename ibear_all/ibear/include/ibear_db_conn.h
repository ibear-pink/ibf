/*
	数据库连接接口
 */
#ifndef _IBEAR_DB_CONN_H__
#define _IBEAR_DB_CONN_H__
#include <stdlib.h>
#include "ibear_sql.h"
#include "ibear_com_type.h"
#include "ibearpub.h"

typedef struct _IBEAR_DbConner
{
	int db_no;
	
	struct IBEAR_sqls db_sqls;
	struct IBEAR_SQL dbSQL;
	DSCI mDSC;
	DSCI *mpDSC;
	DSCI_SQL *p_sql;
}IBEAR_DbConner;

typedef struct
{
	int db_conn_num;
	
#ifdef _DSCI_
	IBEAR_DbConner db_connect;
#else
#endif
}IBEAR_DB_INFO;

#ifdef _DSCI_
#define IBEAR_exec_p(a, b) (DSCI_query_p((a), (b)))
#define IBEAR_exec(a, b, c) (DSCI_query_f((a), (b), (c)))
#define IBEAR_bind_param(a, b, c, d, e) (DSCI_bind_param((a), (b), (c), (d), (e)))
#define IBEAR_bind_blob_param(a, b, c, d, e) (DSCI_bind_blob_param((a), (b), (c), (d), (e)))
#define IBEAR_bind_clob_param(a, b, c, d) (DSCI_bind_clob_param((a), (b), (c), (d)))
#define IBEAR_num_rows(a) (DSCI_num_rows((a)))
#define IBEAR_fetch_row_s(a, b) (DSCI_fetch_row_s((a), (b)))
#define IBEAR_get_presql(a, b) (&((a)->dbSQL.v_presql[(b)]))
#define IBEAR_commit(a) (DSCI_end_trans ((a)->mpDSC, 0))
#define IBEAR_rollback(a) (DSCI_end_trans ((a)->mpDSC, 1))
#define IBEAR_trans(a,b,c) (DSCI_end_trans_sig ((a)->mpDSC, b,c))
#define IBEAR_get_db_instance(a, b) (&((a)->db_connect))
#define IBEAR_error(a) DSCI_error((a))
#define IBEAR_ecode(a) DSCI_ecode((a))
#define IBEAR_lob_get(a,b) (DSCI_lob_get((a),(b)))
#define IBEAR_lob_getp(a) (DSCI_lob_getp((a)))
#define IBEAR_lob_isclob(a) (DSCI_lob_isclob((a)))

#else
#endif
/*断开数据连接*/
extern void ibear_db_quit (IBEAR_DbConner *DB_DSC);
/*数据库初始化*/
extern int ibear_db_init (IBEAR_DbConner *DB_DSC,stSettings *set,char *err_msg);
/*动态变化SQL语句预解析*/
extern IBEAR_sql *db_PrepareSql (IBEAR *mpDB, PRESQL *preSqls, char *sql, int db_no = 0,bool repre = true);

//
extern int ibear_bindParam(map<string,stSQLParam*> bindParams,stSQLParam *para_list[]);
extern int initConf(map<string,stSqlMap*> *m_Sqlconfs,map<string,void*> inParams,char *sql_out,stSqlMap **sqlmap,int routeNo);
#endif
