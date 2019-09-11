/**
 *SQL语句宏定义
 */
#ifndef _IBEAR_SQL_H_
#define _IBEAR_SQL_H_

#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>

#ifdef _DSCI_
#include "dsci.h"
#ifdef _DSCI_DB_
extern "C" {
#include "dsci_db.h"
	
#ifdef ORACLE
#include "DB_Ora.h"
#endif
	
#ifdef DMDB
#include "ddci.h"
#endif
	
}
#endif
#else
#endif

using namespace std;

#define DB_SUCCESS				0
#define DB_FAIL					-1

#define MAX_PREPARE_SQL			1024
#define MAX_DB_SQL				1024
#define MAX_IBEAR_DB_NUM		30

#ifdef _DSCI_
typedef DSCI_SQL IBEAR_sql;
typedef DSCI IBEAR;
typedef DSCI_ROW IBEAR_row;
#else
#endif
typedef vector<void*> IBEAR_MemData;
typedef vector<string> IBEAR_MemIndex;

typedef struct _pDb_sql
{
	char szSQL[1024];
	int pFlag;		/*DB_SQL变量是否解析过,初始为0，解析后设置为1*/
	IBEAR_sql _s;
}_PDB_SQL;

typedef struct
{
	int _next_seq;
	char msql[MAX_PREPARE_SQL][1024];
	
	IBEAR_sql _sql[MAX_PREPARE_SQL];
}PRESQL;

#ifdef _DSCI_
struct IBEAR_sqls
{
	_PDB_SQL v_db_sql[MAX_IBEAR_DB_NUM][MAX_DB_SQL];
};
struct IBEAR_SQL
{
	PRESQL v_presql[MAX_IBEAR_DB_NUM];
};
#else
#endif

#endif
