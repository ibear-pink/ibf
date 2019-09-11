
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
    pD = &myDSC;
  
    memset(&ss1, 0, sizeof(&ss1));
    /**************** 测试重复连接 是否内存泄露 ***************************/
   
    while(1)
    {
         DSCI_init(errStr, pD);
        
        if(0 != DSCI_query(pD, "update bbb1 set f2=f2+1"))
            printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        else
            printf("修改表1记录数 %ld\n", DSCI_num_rows(pD));
        DSCI_end_trans(pD, 0);
                
        if(0!=DSCI_prepare_sql(pD, &ss1, "select * from bbb1 where f1=?"))
            printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
        else            
            DSCI_bind_param(pD, &ss1, 0, DSCI_TYPE_CHAR, str);
        for(i=0; i<3; i++)
        {
            sprintf(str, "@%d", i);
            if(0 != DSCI_query_p(pD, &ss1))
                printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
            else
            {
                col=DSCI_num_fields(pD);
                while(NULL != (row1= DSCI_fetch_row(pD)))
                {
                    for(j=0; j<col; j++)
                        printf("*%s*\t", row1[j]);
                    printf("\n");
                }
            }
        }
        DSCI_free_sql(&ss1);
    
        sleep(1);
        DSCI_quit(pD);
    }
    
    /**************** 测试连接异常 ***************************/
    DSCI_init(errStr, pD);
    DSCI_prepare_sql(pD, &ss1, "select * from bbb1 where f1=?");
    DSCI_bind_param(pD, &ss1, 0, DSCI_TYPE_CHAR, str);
    while(1)
    { 
        if(0 != DSCI_query(pD, "update bbb1 set f2=f2+1"))
            printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
      getchar();      
        DSCI_end_trans(pD, 0);
        printf("修改表1记录数 %ld\n", DSCI_num_rows(pD));

        for(i=0; i<3; i++)
        {
            sprintf(str, "@%d", i);
            if(0 != DSCI_query_p(pD, &ss1))
            {
                printf("%d, %s\n", DSCI_ecode(pD), DSCI_error(pD));
                if(DSCI_ecode(pD) > 0)
                {
                    printf("重新连接 \n");
                    getchar();   
                    DSCI_prepare_sql(pD, &ss1, "select * from bbb1 where f1=?");
                    DSCI_bind_param(pD, &ss1, 0, DSCI_TYPE_CHAR, str);
                }
            }
            else
            {
                col=DSCI_num_fields(pD);
                while(NULL != (row1= DSCI_fetch_row(pD)))
                {
                    for(j=0; j<col; j++)
                        printf("*%s*\t", row1[j]);
                    printf("\n");
                }
            }
        }
    }


    return 0;
}
