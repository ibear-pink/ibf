
#ifndef _DSCI_CPP_DB_H_
#define _DSCI_CPP_DB_H_

#include "dsci.h"
#include "dscpp.h"

class DSCIResultSet_OP:public DSCIResultSet
{
public:
    DSCIResultSet_OP(DSCI *pdb, DSCI_SQL *pss);
    virtual ~DSCIResultSet_OP();

    bool next();
    char* getString(int field);
    int getInt(int field);
    long getLong(int field);
    double getDouble(int field);
    int getBlobSize(int field);
    int getBlob(int field, void *value);
    int getFieldCount();
    long getRecCount();

    long recCount;
    int fieldNum;

private:
    DSCI *db;
    DSCI_SQL *ss;
    DSCI_ROW row;
};

class DSCIStatement_OP:public DSCIStatement
{
public:
    DSCIStatement_OP(DSCI *pdb);
    virtual ~DSCIStatement_OP();

    DSCIResultSet* executeQuery(const char* sql);
    DSCIResultSet* executeQuery(const char* sql, int dbNo);
    long executeUpdate(const char* sql);
    long executeUpdate(const char* sql, int dbNo);
    void release();

private:
    DSCI *db;
    DSCIResultSet_OP *rs;
};

class DSCIPreparedStatement_OP:public DSCIPreparedStatement
{
public:
    DSCIPreparedStatement_OP(DSCI *pdb);
    virtual ~DSCIPreparedStatement_OP();

    int bind(int paramid, char *value);
    int bind(int paramid, int *value);
    int bind(int paramid, long *value);
    int bind(int paramid, float *value);
    int bind(int paramid, double *value);
    int bind(int paramid, void *value, int size);
    DSCIResultSet* executeQuery();
    long executeUpdate();
    void release();

    DSCI_SQL ss;

private:
    DSCI *db;
    DSCIResultSet_OP *rs;
};

class DSCIConnection_OP:public DSCIConnection
{
public:
    DSCIConnection_OP();
    virtual ~DSCIConnection_OP();

    DSCIStatement* createStatement();
    DSCIPreparedStatement* prepareStatement(const char* sql);
    DSCIPreparedStatement* prepareStatement(const char* sql, int dbNo);
    int setAutocommit(int flag);
    void setBufferflag(int flag);
    void setLogmode(const char *inMode);
    int getErrCode();
    char* getErrMsg();
    int commit();
    int rollback();
    void release();

    DSCI db;

private:

};

#endif
