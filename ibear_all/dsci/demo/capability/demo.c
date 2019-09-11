#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <pthread.h>
#include "dsci.h"

int g_num, g_rec, g_commit, g_log;

int f_i=1;
int f_s=1;
int f_u=1;
int f_d=1;
char logFlag[32] = "on";

double getUTimeToDouble()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + 0.000001 * tv.tv_usec;
}

void *aaa(void *p)
{
     int i, j, k, row, col, flag, *pid;     
     long aa;
     DSCI_ROW v_row;  
     DSCI_SQL ss1;                      
     DSCI  my_DCI, *p_DCI;
     char str[1000], str1[100], s2[100], testTab[50];
     double t1,t2;
     FILE *fp;    
     
     pid = (int *)p;
     
    /** DCI init **/
    if( 0 != DSCI_init(str,  &my_DCI ))
    {
         printf("%s\n", str);
         exit(1);
    }

    p_DCI = &my_DCI;    
    DSCI_set_log_mode(p_DCI, logFlag);
    memset(&ss1, 0, sizeof(DSCI_SQL));
      
    row = g_rec/g_num;    
    sprintf(testTab, "mytest");
    
if(f_i) /*********************** 预解析 插入 ************************************************/
{    
    /*remove("1.txt");    fp = fopen("1.txt", "w");*/  /*用于测试MDB异常中断*/
    
    /*DSCI_query(p_DCI, "truncate table mytest");*/
    sprintf(str,"insert into %s(msisdn,fav_brand,fav_big_type,fav_type,flag_code,fav_order,fav_period,free_value,start_time,end_time,group_id,product_code) values(?,'aa','bb','cc','dd','e','f',?,?,'20240101000000','ff','gg')", testTab);
    if(0 != DSCI_prepare_sql(p_DCI, &ss1, str))
        {printf(" (%s)\n", DSCI_error(p_DCI));  exit(0);}
    DSCI_bind_param(p_DCI, &ss1, 0, DSCI_TYPE_CHAR, str);
    DSCI_bind_param(p_DCI, &ss1, 1, DSCI_TYPE_LONG, &aa);
    DSCI_bind_param(p_DCI, &ss1, 2, DSCI_TYPE_DATE, str1);
    sprintf(str1, "20070101000001");
        
    t1 = getUTimeToDouble();
    for( i = 0; i < row; i++ )
    {
        sprintf(str, "%011d", (*pid)*row+i);
        aa = 1;
        flag = DSCI_query_p(p_DCI, &ss1);
        if( 0 != flag )
        {
              printf("insert failed (%s)\n", DSCI_error(p_DCI));
              return NULL;
        }
        if(i % g_commit == 0)
            DSCI_end_trans(p_DCI, 0);
    }
    t2 = getUTimeToDouble();
    DSCI_end_trans(p_DCI, 0);
    printf("@@@@@@@ insert %d   time = %lf\n", *pid, t2-t1);    
}

if(f_u) /*********************************  预解析 更新  *****************************************/
{   
    sprintf(str,"update %s set FREE_VALUE = FREE_VALUE+? where msisdn = ?", testTab);
    DSCI_prepare_sql(p_DCI, &ss1, str);  
    DSCI_bind_param(p_DCI, &ss1, 0, DSCI_TYPE_LONG, &aa);
    DSCI_bind_param(p_DCI, &ss1, 1, DSCI_TYPE_CHAR, str);
    
    t1 = getUTimeToDouble();
    for( i = 0; i < row; i++ )
    {
        sprintf(str, "%011d", (*pid)*row+i);
        aa = 1;
        flag = DSCI_query_p(p_DCI, &ss1);
        if( 0 != flag )
        { printf("update pre failed (%s)\n",DSCI_error(p_DCI));   return NULL; } 
        
        if(i % g_commit == 0)
            DSCI_end_trans(p_DCI, 0);
    }    
    DSCI_end_trans(p_DCI, 0);
    t2 = getUTimeToDouble();
    printf("@@@@@@@ update %d   time = %lf\n", *pid, t2-t1);
}  
   
if(f_s) /*********************************  预解析 查询  *****************************************/
{   
    j = (row>100)?0:1;
    sprintf(str,"select a.* from %s a where a.Msisdn=? ", testTab);
    DSCI_prepare_sql(p_DCI, &ss1, str);    
    DSCI_bind_param(p_DCI, &ss1, 0, DSCI_TYPE_CHAR, str);

    t1 = getUTimeToDouble();
    for( i = 0; i < row; i++ )
    {
        sprintf(str, "%011d", (*pid)*row+i);       
        
        flag = DSCI_query_p(p_DCI, &ss1);
        if( 0 != flag )
        { printf("select failed (%s)\n", DSCI_error(p_DCI));   return NULL; } 
        
        col = DSCI_num_fields(p_DCI);
        while(NULL != (v_row = DSCI_fetch_row(p_DCI)))
        {              
            for( k=0; k<col; k++ )
            {
                if(j)
                    printf("%s\t", v_row[k]);
            }
            if(j)  printf("\n");
        }
    }    
    t2 = getUTimeToDouble();
    printf("@@@@@@@ select %d   time = %lf\n", *pid, t2-t1);
}        

if(f_d) /*********************** 预解析 删除 ************************************************/
{   
    sprintf(str,"delete from %s where msisdn = ?", testTab);
    DSCI_prepare_sql(p_DCI, &ss1, str);    
    DSCI_bind_param(p_DCI, &ss1, 0, DSCI_TYPE_CHAR, str);
    
    t1 = getUTimeToDouble();
    for( i = 0; i < row; i++ )
    {
        sprintf(str, "%011d", (*pid)*row+i);
        flag = DSCI_query_p(p_DCI, &ss1);
        if( 0 != flag )
        {
              printf("delete pre failed (%s)\n", DSCI_error(p_DCI));
              return NULL;
        }  
        if(i % g_commit == 0)
            DSCI_end_trans(p_DCI, 0);             
    }    
    DSCI_end_trans(p_DCI, 0);
    t2 = getUTimeToDouble();
    printf("@@@@@@@ delete %d   time = %lf\n", *pid, t2-t1);
}    

    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    DSCI_free_sql(&ss1);
    DSCI_quit(p_DCI );
    return NULL;
}

int main( int argc, char * argv[ ] )
{
    time_t t1, t2;
    int i, j[500];
    char str[100];
    pthread_t ta[500];


    if(argc != 5)
    {    
        printf(" usage: %s thread_num rec_num commit log \n", argv[0]);
        exit(0);
    }
    
    g_num = atoi(argv[1]);
    g_rec = atoi(argv[2]);
    g_commit = atoi(argv[3]);
    g_log = atoi(argv[4]);
    if(0 == g_log)
        strcpy(logFlag, "on");
    else if(1 == g_log)
        strcpy(logFlag, "off");
    else if(2 == g_log)
        strcpy(logFlag, "sync");
        
    printf("thread_num = %d,  total=%d, commit=%d, g_log= %d \n", g_num, g_rec, g_commit, g_log);
    
    if(g_num > 100 || g_num <=0)
    {
        printf("error thread_num[%d]\n", g_num);
        exit(0);
    }
    
    time(&t1);
    
    for(i=0; i<g_num; i++)
    {
        j[i] = i;            
        pthread_create( &(ta[i]), NULL, aaa, &(j[i]));
    }
	for(i=0; i<g_num; i++)
    { 
        pthread_join(ta[i],NULL);
        pthread_detach(ta[i]);
	}
	
	time(&t2);   
	printf("total time = %ld\n", t2-t1);
}
