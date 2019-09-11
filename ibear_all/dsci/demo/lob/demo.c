/*
drop table t_lob;
CREATE TABLE T_LOB(
AA      NUMERIC(9),
BB      LOB(229076),
CC      LOB(1024),
EE      char(9),
DD      LOB(229076),
PRIMARY KEY(AA)
)TABLESPACE MEM_1000 ROUTE TYPE PART aa;
CREATE INDEX IND1 ON T_LOB(AA);
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsci.h"

typedef struct
{
    char  name[52];
    int  idNo;             /*用户id*/
}LOB_TEST;

void printLob(char *buf)
{
    int len;
    LOB_TEST tmpLob;

    if(1 == DSCI_lob_isclob(buf))   /*CLOB 普通字符串*/
    {
        len = DSCI_lob_size(buf);
        printf("CLOB= **%s**, len=%d\n", DSCI_lob_getp(buf), len);
    }                                  /*CLOB 二进制*/
    else
    {
        len = DSCI_lob_get(&tmpLob, buf);
        if(len==0)
            memset(&tmpLob, 0, sizeof(LOB_TEST));
        printf("BLOB= %d, %s**\n", tmpLob.idNo, tmpLob.name);
    }
}

int main(int argc, char *argv[])
{
    int g_i=1, g_s=1;

    int i, j, k, n, col, col1, rowNum, len;
    char errStr[256], str[512], *str1, *str2;
    DSCI myDSC=NULL, *p_DCI;
    DSCI_ROW v_row;
    DSCI_SQL ss1;
    LOB_TEST tmpLob;

    /*连接初始化*/
    p_DCI = &myDSC;
    if(0 != DSCI_init(errStr, p_DCI))
    {
        printf("%s\n", errStr);
        return 0;
    }
    DSCI_set_Buffer_flag(p_DCI, (argc==2)?atoi(argv[1]):0);  /*设为缓冲模式*/
    memset(&ss1, 0, sizeof(ss1));
    str1 = malloc(1024*1024);
    str2 = malloc(1024*1024);

/******************************  插入  ********************************/
    if(1==g_i)
    {
        if(0 != DSCI_query(p_DCI, "truncate table t_lob"))
        {
            printf("insert failed (%s)\n", DSCI_error(p_DCI));
            return 0;
        }
        if(0 != DSCI_prepare_sql(p_DCI, &ss1, "insert into t_lob(aa,bb,cc,ee,dd) values(?, ?, ?, ?, ?)"))
        {
            printf("insert failed (%s)\n", DSCI_error(p_DCI));
            return 0;
        }

        DSCI_bind_param(     p_DCI, &ss1, 0, DSCI_TYPE_INT, &i);
        //DSCI_bind_clob_param(p_DCI, &ss1, 1, str1);
        DSCI_bind_blob_param(p_DCI, &ss1, 1, &tmpLob, sizeof(LOB_TEST));
        DSCI_bind_blob_param(p_DCI, &ss1, 2, &tmpLob, sizeof(LOB_TEST));
        DSCI_bind_param(     p_DCI, &ss1, 3, DSCI_TYPE_CHAR, str);
        //DSCI_bind_clob_param(p_DCI, &ss1, 4, str2);
        DSCI_bind_blob_param(p_DCI, &ss1, 4, &tmpLob, sizeof(LOB_TEST));
        
        for( i = 0; i < 5; i++ )
        {
            sprintf( str, "9!!!%d", i);
            sprintf(tmpLob.name, "^^^^^^^%d", i);
            tmpLob.idNo = i+10;

            sprintf( str1, "@@@8%d@@@", i);
            sprintf( str2, "###77%d###", i);
            DSCI_bind_blob_param(p_DCI, &ss1, 2, &tmpLob, sizeof(LOB_TEST));

            if(i==1)  /*LOB全空*/
            {
                str1[0] = 0;
                str2[0] = 0;
                DSCI_bind_blob_param(p_DCI, &ss1, 2, &tmpLob, 0);
            }
            else if(i==3) /*第一个为空*/
            {
                str1[0] = 0;
            }
            else if(i==2)  /*超长*/
            {
                sprintf( str2, "###77%0210000d###", i);
                sprintf( str2, "###77%0300d###", i);
            }

            if( 0 != DSCI_query_p(p_DCI, &ss1) )
            {
                printf("insert failed (%s)\n", DSCI_error(p_DCI));
                break;
            }
            printf("%ld rows inserted\n", DSCI_num_rows(p_DCI));
        }
        DSCI_end_trans(p_DCI, 0);
        printf("**********************************************************\n\n");
    }

/*****************查询************************************************************************/
    if(g_s == 1)
    {
        DSCI_query(p_DCI, "select * from t_lob");
        printf("%ld rows selected\n", DSCI_num_rows(p_DCI));
        col = DSCI_num_fields(p_DCI);
        rowNum = 0;
        while(NULL != (v_row = DSCI_fetch_row(p_DCI)) )
        {
            printf("aa=*%s*, bb=clob(%d), cc=blob(%d), ee=*%s*, dd=clob(%d)\n", v_row[0], DSCI_lob_size(v_row[1]), DSCI_lob_size(v_row[2]),
                    v_row[3], DSCI_lob_size(v_row[4]));

            printLob(v_row[1]);
            printLob(v_row[2]);
            printLob(v_row[4]);

            rowNum++;
        }
        printf("%d rows selected\n", rowNum);
    }


    /*退出*/
    printf("\n退出数据库\n");
    DSCI_free_sql(&ss1);

    DSCI_quit(p_DCI);
    return 0;
}
