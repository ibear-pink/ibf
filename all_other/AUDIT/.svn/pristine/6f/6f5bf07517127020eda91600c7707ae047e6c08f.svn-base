
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsci_comm.h"
#include "dsci.h"

int main(int argc, char * argv[ ])
{
    char str[512];

    if( (argc == 5) && (strcmp(argv[1], "-p") == 0) ) /*生成加密文件*/
    {
        if(0 == _PDSC_SetLoginInfo(argv[4], argv[2], argv[3]))
            printf("%s was created.\n\n", argv[4]);
        else
            printf("can't create file %s.\n\n", argv[4]);
        return 0;
    }
    else if( (argc == 3) && (strcmp(argv[1], "-g") == 0) ) /*根据密码文件获取密码*/
    {
        char user[50], pwd[50];
        if(0 == _PDSC_GetLoginInfo(argv[2], user, pwd))
            printf("%s %s\n",user,pwd);
        else
            printf("can't open file %s.\n\n", argv[2]);
        return 0;
    }
    else if( (argc == 2) && (strcmp(argv[1], "-v") == 0) ) /*查看版本*/
    {
        DSCI_ver(str, str+100);
        printf("\n\tVersion: %s, Release date: %s\n", str, str+100);
        printf("\tCompiled at %s %s\n", __DATE__, __TIME__);
        printf("\tCopyrights hold by Si-tech company\n\n");
        return 0;
    }

    sprintf(str, "dsc_tools");

    printf( "\n  [usage]: %s -p username password loginFile : 生成DB密码文件\n", str );
    printf( "\t   %s -g loginFile : 查看密码文件\n",str);
    printf( "\t   %s -h : to view help\n",str);
    printf( "\t   %s -v : to view version\n\n",str);
    return 0;
}
