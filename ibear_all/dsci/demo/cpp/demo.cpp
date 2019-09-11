
/*
CREATE TABLE T_CPP(
AA      NUMERIC(9),
BB      LOB(50),
CC      VARCHAR(10),
DD      NUMERIC(9,2),
EE      date,
PRIMARY KEY(AA)
);
CREATE INDEX IND1 ON T_CPP(AA);
*/

#include <iostream>
#include <stdio.h>
#include <string.h>
#include "dscpp.h"

using namespace std;

typedef struct
{
    long  idNo;             /*用户id*/
    char  name[40];
}LOB_TEST;

/*打印select信息*/
void printRS(DSCIResultSet *rs)
{
    int i=0, len;
    LOB_TEST bv;

    printf("\nRecCount = %ld, FieldCount = %d\n", rs->getRecCount(), rs->getFieldCount());
    while(rs->next())
    {
        printf("%ld, %s, %f, %s\t", rs->getLong(0), rs->getString(2), rs->getDouble(3), rs->getString(4));
        len = rs->getBlob(1, &bv);
        printf("len=%d, %ld, %s, %d\n", len, bv.idNo, bv.name, rs->getBlobSize(1));
        i++;
    }
    printf("检索到 %d 条记录\n", i);
}

int main(int argc, char *argv[])
{
    char errStr[256], str[256];
    DSCIConnection *db;
    DSCIStatement *st;
    DSCIPreparedStatement *pst1, *pst2;
    DSCIResultSet *rs;
    long lv, aa;
    int iv, i;
    double dv;
    float fv;
    LOB_TEST bv;

    if((argc > 1) && (argv[1][0] == '1'))
    {
        strcpy(str, "mmdb/mmdb@oracle:billdb");  
        db = DSCI_MakeConnection(errStr, str);
    }
    else
        db = DSCI_MakeConnection(errStr);
    
    if(db == NULL)
    {
        printf("%s\n", errStr);
        return 0;
    }

    db->setAutocommit(0);
    db->setLogmode("on");

    /*直接执行SQL*/
    st = db->createStatement();
    aa = st->executeUpdate("delete from t_cpp");
    if(aa == -1)
        printf("%d, %s\n", db->getErrCode(), db->getErrMsg());
    else
        printf("删除 %ld 条记录\n", aa);


    /*预解析SQL*/
    pst1 = db->prepareStatement("insert into t_cpp(aa,bb,cc,dd,ee) values(?,?,?,?,?)");
    if(NULL == pst1)
    {
        printf("@@    %d, %s\n", db->getErrCode(), db->getErrMsg());
        return 0;
    }
    pst1->bind(0, &lv);
    pst1->bind(1, &bv, sizeof(LOB_TEST));
    pst1->bind(2, str);
    pst1->bind(3, &fv);
    pst1->bind(4, str+128);

    for(i=0; i<3; i++)
    {
        sprintf(str, "@@@_%d", i);
        sprintf(str+128, "201102030000%02d", i);
        iv = 12+i;
        lv = 100*i + 100;
        fv = 123.56 + i;
        dv = 345.21 + i;
        bv.idNo = 100 + i;
        sprintf(bv.name, "^^^ %d  ^^^", i);

        aa = pst1->executeUpdate();
        if(aa == -1)
            printf("%d, %s\n", db->getErrCode(), db->getErrMsg());
        else
            printf("操作 %ld 记录\n", aa);
    }
    db->commit();

    /*设置缓冲模式*/
    db->setBufferflag(0);

    /*普通查询*/
    if(NULL == (rs = st->executeQuery("select * from t_cpp where aa >= 100")))
    {
        printf("%d, %s\n", db->getErrCode(), db->getErrMsg());
        return 0;
    }
    printRS(rs);

    if(NULL == (rs = st->executeQuery("select a.*,'aa' from t_cpp a where aa >= 200")))
    {
        printf("%d, %s\n", db->getErrCode(), db->getErrMsg());
        return 0;
    }
    printRS(rs);

    /*预解析查询*/
    if(NULL == (pst2 = db->prepareStatement("select * from t_cpp where aa >= ?")))
    {
        printf("%d, %s\n", db->getErrCode(), db->getErrMsg());
        return 0;
    }
    pst2->bind(0, &lv);
    lv = 0;
    rs = pst2->executeQuery();
    printRS(rs);

    lv = 300;
    pst2->executeQuery();
    printRS(rs);


    pst1->release();
    pst2->release();
    st->release();
    db->release();

    return 0;
}
