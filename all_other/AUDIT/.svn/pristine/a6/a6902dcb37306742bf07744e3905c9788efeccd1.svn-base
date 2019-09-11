/*
create table bbb1(
f1 	varchar(15),
f2 	numeric(9,2),
f3 	date,
primary key(f1,f2)
);
create index in1 on bbb1(f1);

create table bbb2(
aa 	varchar(15),
bb 	date,
primary key(aa)
);
create index in2 on bbb2(aa);

truncate table bbb1;
truncate table bbb2;
select * from bbb1;
select * from bbb2;

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsci.h"

int main()
{
    int i, j, k, n, col, col1, col2;
    double bb=0;
    float ff=213.34;
    long ll=123;
    char errStr[256], str[512];
    DSCI myDSC=NULL, *pD;
    DSCI_ROW row1, row2;
    DSCI_SQL ss1, ss2, ss3;

    /*获取DSC接口的版本号*/
    DSCI_ver(errStr, errStr+100);
    printf("DSC接口的版本号:%s;  版本日期:%s.\n\n", errStr, errStr+100);

    /*连接初始化*/
    pD = &myDSC;
    if(0 != DSCI_init(errStr, pD))
    {
        printf("%s\n", errStr);
        return 0;
    }   

    /*设置非自动提交*/
    /*DSCI_set_auto_commit(pD, 1);*/

    /*清空表*/
    printf("清空表\n\n");
    if( (0 != DSCI_query(pD, "truncate table bbb1")) ||
        (0 != DSCI_query(pD, "truncate table bbb2")) )
    {
        printf("t %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        DSCI_quit(pD);
        return 0;
    }
    DSCI_end_trans(pD, 0);

    memset(&ss1, 0, sizeof(DSCI_SQL));

    /*插入表1记录*/
    //if(0 != DSCI_prepare_sql(pD, &ss1, "insert into bbb1(f1,f2,f3) values(?, 12, '20101223020304')"))
    if(0 != DSCI_prepare_sql(pD, &ss1, "insert into bbb1(f1,f2,f3) values(?, ?, ?)"))
    {
        printf("i %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        DSCI_quit(pD);
        return 0;
    }
    if(0 != DSCI_bind_param(pD, &ss1, 0, DSCI_TYPE_CHAR, str))
        printf("0 %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
    if(0 != DSCI_bind_param(pD, &ss1, 1, DSCI_TYPE_DOUBLE, &bb))
        printf("0 %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
    if(0 != DSCI_bind_param(pD, &ss1, 2, DSCI_TYPE_DATE, str+200))
        printf("0 %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));

    for(i=0; i<3; i++)
    {
        sprintf(str, "@%d", i);  bb = 12.34+i;    j = i*200+56;
        if(i==0)
            strcpy(str+200, "20101223020304");
        else if(i==1)
            strcpy(str+200, "20150309145621");
        else
            strcpy(str+200, "19841130235958");
            
        if(0 != DSCI_query_p(pD, &ss1))
        {
            printf("p %d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
            DSCI_quit(pD);
            return 0;
        }
        printf("插入表1记录数 %ld\n", DSCI_num_rows(pD));
    }

    /*插入表2记录*/
    printf("\n插入表2记录\n\n");
    if( (0 != DSCI_query(pD, "insert into bbb2 values('aa1', '20131223020304')")) ||
        (0 != DSCI_query(pD, "insert into bbb2 values('aa2', '20131223020304')"))  )
    {
        printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        DSCI_quit(pD);
        return 0;
    }

    /*修改操作*/
    DSCI_query(pD, "update bbb1 set f2=f2+1");
    printf("修改表1记录数 %ld\n", DSCI_num_rows(pD));

    DSCI_query(pD, "update bbb2 set bb='20120101235958'");
    printf("修改表2记录数 %ld\n", DSCI_num_rows(pD));

    /*提交*/
    DSCI_end_trans(pD, 0);

    /*查询表1*/
    printf("\n查询表1记录\n");
    if(0 != DSCI_query(pD, "select * from bbb1"))
    {
        printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        return 0;
    }
    n = 0;   col=DSCI_num_fields(pD);
    while(NULL != (row1= DSCI_fetch_row(pD)))
    {
        for(i=0; i<col; i++)
            printf("*%s*\t", row1[i]);
        printf("\n");
        n++;
    }
    printf("查询表1记录数 %d\n", n);

    /*查询表2*/
    printf("\n查询表2记录\n");
    if(0 != DSCI_prepare_sql(pD, &ss1, "select * from bbb2 where aa=?"))
    {
        printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        DSCI_quit(pD);
        return 0;
    }
    
    /*memset(&ss2, 0, sizeof(DSCI_SQL));
    DSCI_prepare_sql(pD, &ss2, "select * from bbb1");
    DSCI_prepare_sql(pD, &ss2, "delete from bbb1");*/
    
    DSCI_bind_param(pD, &ss1, 0, DSCI_TYPE_CHAR, str);
    for(i=0; i<2; i++)
    {
        sprintf(str, "aa%d", i+1);
        printf("%s\n", str);
        if(0 != DSCI_query_p(pD, &ss1))
        {
            printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
            DSCI_quit(pD);
            return 0;
        }
        col=DSCI_num_fields(pD);
        while(NULL != (row1= DSCI_fetch_row(pD)))
        {
            for(j=0; j<col; j++)
                printf("*%s*\t", row1[j]);
            printf("\n");
        }
    }
    DSCI_free_sql(&ss1);

    printf("\n**************************** select 嵌套测试 ***********************\n");
    DSCI_set_Buffer_flag(pD, 1);  /*设为缓冲模式*/

    memset(&ss1, 0, sizeof(DSCI_SQL));
    memset(&ss2, 0, sizeof(DSCI_SQL));
    memset(&ss3, 0, sizeof(DSCI_SQL));
    DSCI_prepare_sql(pD, &ss1, "select f1,f2,f3 from bbb");
    DSCI_prepare_sql(pD, &ss2, "select '@@@@', f3,f1, f2, '@@@@' from bbb where f1=?");
    DSCI_prepare_sql(pD, &ss3, "update bbb set f2=f2+5 where f1=?");

    DSCI_query_p(pD, &ss1);
    col1 = DSCI_num_fields(pD);
    printf("%ld rows selected\n\n", DSCI_num_rows(pD));
    while( (row1 = DSCI_fetch_row_s(pD, &ss1)) )
    {
        for( k=0; k<col1; k++ )
            printf("%s\t", row1[k]);
        printf("\n");

        DSCI_bind_param(pD, &ss3, 0, DSCI_TYPE_CHAR, row1[0]);
        if(0 != DSCI_query_p(pD, &ss3))
            printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));

        DSCI_bind_param(pD, &ss2, 0, DSCI_TYPE_CHAR, row1[0]);
        DSCI_query_p(pD, &ss2);
        col2 = DSCI_num_fields(pD);
        while( (row2 = DSCI_fetch_row_s(pD, &ss2)) )
        {
             for( k=0; k<col2; k++ )
                printf("   %s\t", row2[k]);
            printf("\n");
        }
    }
    DSCI_end_trans(pD, 0);
    /******************************************************************/


    /*退出*/
    printf("\n退出数据库\n");
    DSCI_free_sql(&ss1);
    DSCI_free_sql(&ss2);
    DSCI_free_sql(&ss3);

    DSCI_quit(pD);
    return 0;
}
