
#include <stdlib.h>
#include <string.h>
#include "dscpp_db.h"

/*******************  DSCIResultSet_OP  ******************************************/
DSCIResultSet_OP::DSCIResultSet_OP(DSCI *pdb, DSCI_SQL *pss)
{
    db = pdb;
    ss = pss;
}

DSCIResultSet_OP::~DSCIResultSet_OP()
{
}

bool DSCIResultSet_OP::next()
{
    if(ss != NULL)
        row = DSCI_fetch_row_s(db, ss);
    else
        row = DSCI_fetch_row(db);

    return (row != NULL);
}

char* DSCIResultSet_OP::getString(int field)
{
    return row[field];
}

int DSCIResultSet_OP::getInt(int field)
{
    return atoi(row[field]);
}

long DSCIResultSet_OP::getLong(int field)
{
    return atol(row[field]);
}

double DSCIResultSet_OP::getDouble(int field)
{
    return atof(row[field]);
}

int DSCIResultSet_OP::getBlobSize(int field)
{
    return DSCI_lob_size(row[field]);
}

int DSCIResultSet_OP::getBlob(int field, void *value)
{
    return DSCI_lob_get(value, row[field]);
}

int DSCIResultSet_OP::getFieldCount()
{
    return fieldNum;
}

long DSCIResultSet_OP::getRecCount()
{
    return recCount;
}

/*******************  DSCIStatement_OP  ******************************************/
DSCIStatement_OP::DSCIStatement_OP(DSCI *pdb)
{
    db = pdb;
    rs = NULL;
}

DSCIStatement_OP::~DSCIStatement_OP()
{
    if(rs != NULL)
    {
        delete rs;
        rs = NULL;
    }
}

DSCIResultSet* DSCIStatement_OP::executeQuery(const char* sql, int dbNo)
{
    if(0 == DSCI_query_f(db, sql, dbNo))
    {
        if(rs == NULL)
            rs = new DSCIResultSet_OP(db, NULL);
        rs->fieldNum = DSCI_num_fields(db);
        rs->recCount = DSCI_num_rows(db);
        return rs;
    }
    else
        return NULL;
}

DSCIResultSet* DSCIStatement_OP::executeQuery(const char* sql)
{
    return executeQuery(sql, 0);
}

long DSCIStatement_OP::executeUpdate(const char* sql, int dbNo)
{
    if(0 == DSCI_query_f(db, sql, dbNo))
        return DSCI_num_rows(db);
    else
        return -1;
}

long DSCIStatement_OP::executeUpdate(const char* sql)
{
    return executeUpdate(sql, 0);
}

void DSCIStatement_OP::release()
{
    delete this;
}

/*******************  DSCIPreparedStatement_OP  ******************************************/
DSCIPreparedStatement_OP::DSCIPreparedStatement_OP(DSCI *pdb)
{
    db = pdb;
    memset(&ss, 0, sizeof(DSCI_SQL));
    rs = NULL;
}

DSCIPreparedStatement_OP::~DSCIPreparedStatement_OP()
{
    if(rs != NULL)
    {
        delete rs;
        rs = NULL;
    }
    DSCI_free_sql(&ss);
}

int DSCIPreparedStatement_OP::bind(int paramid, char *value)
{
    return DSCI_bind_param(db, &ss, paramid, DSCI_TYPE_CHAR, value);
}

int DSCIPreparedStatement_OP::bind(int paramid, int *value)
{
    return DSCI_bind_param(db, &ss, paramid, DSCI_TYPE_INT, value);
}

int DSCIPreparedStatement_OP::bind(int paramid, long *value)
{
    return DSCI_bind_param(db, &ss, paramid, DSCI_TYPE_LONG, value);
}

int DSCIPreparedStatement_OP::bind(int paramid, float *value)
{
    return DSCI_bind_param(db, &ss, paramid, DSCI_TYPE_FLOAT, value);
}

int DSCIPreparedStatement_OP::bind(int paramid, double *value)
{
    return DSCI_bind_param(db, &ss, paramid, DSCI_TYPE_DOUBLE, value);
}

int DSCIPreparedStatement_OP::bind(int paramid, void *value, int size)
{
    return DSCI_bind_blob_param(db, &ss, paramid, value, size);
}

DSCIResultSet* DSCIPreparedStatement_OP::executeQuery()
{
    if(0 == DSCI_query_p(db, &ss))
    {
        if(rs == NULL)
            rs = new DSCIResultSet_OP(db, &ss);
        rs->fieldNum = DSCI_num_fields(db);
        rs->recCount = DSCI_num_rows(db);
        return rs;
    }
    else
        return NULL;
}

long DSCIPreparedStatement_OP::executeUpdate()
{
    if(0 == DSCI_query_p(db, &ss))
        return DSCI_num_rows(db);
    else
        return -1;
}

void DSCIPreparedStatement_OP::release()
{
    delete this;
}

/*******************  DSCIConnection_OP  ******************************************/
DSCIConnection_OP::DSCIConnection_OP()
{
    memset(&db, 0, sizeof(DSCI));
}

DSCIConnection_OP::~DSCIConnection_OP()
{
    DSCI_quit(&db);
}

void DSCIConnection_OP::release()
{
    delete this;
}

int DSCIConnection_OP::commit()
{
    return DSCI_end_trans(&db, 0);
}

int DSCIConnection_OP::rollback()
{
    return DSCI_end_trans(&db, 1);
}

DSCIStatement* DSCIConnection_OP::createStatement()
{
    return new DSCIStatement_OP(&db);
}

DSCIPreparedStatement* DSCIConnection_OP::prepareStatement(const char* sql, int dbNo)
{
    DSCIPreparedStatement_OP *pst = new DSCIPreparedStatement_OP(&db);
    if(0 == DSCI_prepare_sql_f(&db, &(pst->ss), sql, dbNo))
        return pst;
    else
    {
        pst->release();
        return NULL;
    }
}

DSCIPreparedStatement* DSCIConnection_OP::prepareStatement(const char* sql)
{
    return prepareStatement(sql, 0);
}

int DSCIConnection_OP::setAutocommit(int flag)
{
    return DSCI_set_auto_commit(&db, flag);
}

void DSCIConnection_OP::setBufferflag(int flag)
{
    DSCI_set_Buffer_flag(&db, flag);
}

void DSCIConnection_OP::setLogmode(const char *inMode)
{
    DSCI_set_log_mode(&db, inMode);
}

int DSCIConnection_OP::getErrCode()
{
    return DSCI_ecode(&db);
}

char* DSCIConnection_OP::getErrMsg()
{
    return DSCI_error(&db);
}

/**********************  连接创建工厂  *******************************************/
DSCIConnection *DSCI_MakeConnection(char *errStr)
{
    DSCIConnection_OP *conn = new DSCIConnection_OP();
    if(0 == DSCI_init(errStr, &conn->db))
        return conn;
    else
    {
        conn->release();
        return NULL;
    }
}

DSCIConnection *DSCI_MakeConnection(char *errStr, char *dbStr)
{
    DSCIConnection_OP *conn = new DSCIConnection_OP();
    if(0 == DSCI_init_direct(errStr, &conn->db, dbStr))
        return conn;
    else
    {
        conn->release();
        return NULL;
    }
}
