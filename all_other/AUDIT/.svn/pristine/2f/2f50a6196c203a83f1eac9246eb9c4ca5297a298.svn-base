/*
测试自动提交功能
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsci.h"

void Select(char *tab, DSCI *pD)
{
    int i, n, col;
    char str[256];
    DSCI_ROW row1;
    
    printf("\n查询表 %s 记录\n", tab);
    sprintf(str, "select * from %s", tab);
    if(0 != DSCI_query(pD, str))
    {
        printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        return;
    }
    n = 0;   col=DSCI_num_fields(pD);
    while(NULL != (row1= DSCI_fetch_row(pD)))
    {
        for(i=0; i<col; i++)
            printf("*%s*\t", row1[i]);
        printf("\n");
        n++;
    }
    printf("查询表 %s 记录数 %d\n", tab, n);
}

int main(int argc, char *argv[])
{
    int i, j;
    double bb;
    char errStr[256], str[512];
    DSCI myDSC1=NULL, myDSC2=NULL, *pD1, *pD2;
    DSCI_SQL ss1;

    /*获取DSC接口的版本号*/
    DSCI_ver(errStr, errStr+100);
    printf("DSC接口的版本号:%s;  版本日期:%s.\n\n", errStr, errStr+100);

    /*连接初始化*/
    pD1 = &myDSC1;
    pD2 = &myDSC2;
    DSCI_init(errStr, pD1);
    DSCI_init(errStr, pD2);

    /*设置自动提交*/
    DSCI_set_auto_commit(pD1, argv[1][0] - '0');

    /*清空表*/
    printf("清空表\n\n");
    DSCI_query(pD1, "truncate table bbb1");
    DSCI_query(pD1, "truncate table bbb2");
    DSCI_end_trans(pD1, 0);
    
    memset(&ss1, 0, sizeof(DSCI_SQL));

    /*插入表1记录*/
    if(0 != DSCI_prepare_sql(pD1, &ss1, "insert into bbb1(f1,f2,f3) values(?, ?, ?)"))
    {
        printf("%d, %s\n", DSCI_ecode(pD1), DSCI_error(pD1));
        DSCI_quit(pD1);
        return 0;
    }
    DSCI_bind_param(pD1, &ss1, 0, DSCI_TYPE_CHAR, str);
    DSCI_bind_param(pD1, &ss1, 1, DSCI_TYPE_DOUBLE, &bb);
    DSCI_bind_param(pD1, &ss1, 2, DSCI_TYPE_DATE, str+200);

    for(i=0; i<3; i++)
    {
        sprintf(str, "@%d", i);  bb = 12.34+i;    j = i*200+56;
        if(i==0)
            strcpy(str+200, "20101223020304");
        else if(i==1)
            strcpy(str+200, "20150309145621");
        else
            strcpy(str+200, "19841130235958");
            
        if(0 != DSCI_query_p(pD1, &ss1))
        {
            printf("%d, %s\n", DSCI_ecode(pD1), DSCI_error(pD1));
            DSCI_quit(pD1);
            return 0;
        }
        printf("插入表1记录数 %ld\n", DSCI_num_rows(pD1));
    }

    /*插入表2记录*/
    printf("\n插入表2记录\n\n");
    if( (0 != DSCI_query(pD1, "insert into bbb2 values('aa1', '20131223020304')")) ||
        (0 != DSCI_query(pD1, "insert into bbb2 values('aa2', '20131223020304')"))  )
    {
        printf("%d, %s\n", DSCI_ecode(pD1), DSCI_error(pD1));
        DSCI_quit(pD1);
        return 0;
    }

    Select("bbb1", pD2);
    Select("bbb2", pD2);
    /*提交*/;
    DSCI_end_trans(pD1, 0);
    printf("\n************************************提交*******************************************\n");
    Select("bbb1", pD2);
    Select("bbb2", pD2);
    
 
    DSCI_free_sql(&ss1);

    DSCI_quit(pD1);
    DSCI_quit(pD2);
    return 0;
}
