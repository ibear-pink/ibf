/*!
 *  @file MyPublic.c ������������
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2007-12-28
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include "dsci_comm.h"

#ifdef WIN32_OS
  #include <windows.h>
  #include <io.h>
#endif

/*uint4korr����CLOB�ֶ�*/
int _PDSC_uint4korr(const char *buf)
{
    unsigned char str[4];
    str[0] = buf[0];
    str[1] = buf[1];
    str[2] = buf[2];
    str[3] = buf[3];

    if(str[3] >= 128)
        str[3] -= 128;
    return uint4korr(str);
}

/*****************************************************************/
/*!
    * @brief ��ȡϵͳ΢����
    * @return ΢��
    */
double _PDSC_GetMicSec(void)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec + 0.000001 * tv.tv_usec;
}

/*****************************************************************/
/*!
    * @brief �����ڴ�
    * @param [in] size      ���ݴ�С
    * @return �ڴ���ָ��
    */
void *_PDSC_MemMalloc(size_t size)
{
    /*printf("++++ %ld\n", size);*/
    return malloc(size);
}

/*****************************************************************/
/*!
    * @brief �ͷ��ڴ�
    * @param [in] p      �ڴ���ָ��
    * @return �ڴ���ָ��
    */
void _PDSC_MemFree(void *p)
{
    /*printf("---\n");*/
    free(p);
    return;
}

/*****************************************************************/
/*!
    * @brief �滻�ַ���
    * @param [out] inStr       ���滻���ַ���
    * @param [in]  oldStr      ���Ӵ�
    * @param [in]  newStr      ���Ӵ�
    * @return ��
    */
void _PDSC_StrRep( char *inStr, const char *oldStr, const char *newStr )
{
    char *p1;

    if(NULL == (p1 = strstr(inStr, oldStr))) /*�����滻*/
        return;
    else
    {
        int i, j = 0;
        unsigned int len;
        char tmpStr[2048], *p;

        len = strlen(oldStr);
        p = inStr;
        do
        {   i = p1 - p;
            strncpy(tmpStr+j, p, (size_t)i); /*ȡǰ�벿��*/
            j += i;
            j += sprintf(tmpStr+j, "%s", newStr);   /*�滻Ϊ���Ӵ�*/
            p = p1 + len;
            p1 = strstr(p, oldStr);
        }while( NULL != p1 );

        j += sprintf(tmpStr+j, "%s", p);  /*ȡ��벿��*/
        strncpy(inStr, tmpStr, (size_t)(j+1));
        return;
    }
}

/*****************************************************************/
/*!
    * @brief �ָ��ַ���,��������ķָ����Ϊ1��
    * @param [in]  inStr        �����ַ���
    * @param [out] outStr       ����ַ���
    * @param [in]  delimiter    �ָ��
    * @param [out] outLen       ���鳤��
    * @param [in]  maxLen       ����������
    * @param [out] iArr         ���ÿ���ַ�����ƫ��λ��
    * @return ��
    */
void _PDSC_SplitStrD(const char *inStr, char *outStr, char delimiter, int *outLen, int maxLen, int iArr[])
{
    int i, len, flag=0;

    *outLen = 0;
    len = (int)strlen(inStr);
    for(i=0; i<len; i++)
    {
        if(delimiter != inStr[i])
        {
            if((inStr[i]=='\r')||(inStr[i]=='\n'))
                outStr[i] = 0;
            else
                outStr[i] = inStr[i];
            if(0 == flag) /*�ϸ��ַ���cc������ַ�����cc*/
            {
                flag = 1;
                iArr[*outLen] = i;
                (*outLen)++;
            }
        }
        else
        {
            outStr[i] = 0;
            flag = 0;
            if(*outLen >= maxLen)
                return;
        }
    }
    outStr[len] = 0;
    return;
}

/*****************************************************************/
/*!
    * @brief �ָ��ַ���,��������ķָ����Ϊ���
    * @param [in]  inStr        �����ַ���
    * @param [out] outStr       ����ַ���
    * @param [in]  cc           �ָ��
    * @param [out] outLen       ���鳤��
    * @param [in]  maxLen       ����������
    * @param [out] iArr         ���ÿ���ַ�����ƫ��λ��
    * @return ��
    */
void _PDSC_SplitStrM(const char *inStr, char *outStr, char cc, int *outLen, int maxLen, int iArr[])
{
    int i, len;

    len = (int)strlen(inStr);
    if(0 == len)
    {
        *outLen = 0;
        return;
    }

    *outLen = 1;
    iArr[0] = 0;
    for(i=0; i<len; i++)
    {
        if(cc != inStr[i])
        {
            if((inStr[i]=='\r')||(inStr[i]=='\n'))
                outStr[i] = 0;
            else
                outStr[i] = inStr[i];
        }
        else
        {
            outStr[i] = 0;
            if(*outLen >= maxLen)
                return;
            iArr[*outLen] = i+1;
            (*outLen)++;
        }
    }
    outStr[len] = 0;
    return;
}

/*��������*/
void _PDSC_SplitStrfM(char *inStr, char cc, int *outLen, int maxLen, int iArr[])
{
    int j;
    char *p, *p1;

    iArr[0] = 0;
    j = 1;
    p1 = inStr;
    while(NULL != (p = strchr(p1, cc)) )
    {
        p1 = p + 1;
        p[0] = 0;

        if(j >= maxLen)
            break;
        iArr[j] = p1 - inStr;
        j++;
    }
    *outLen = j;
    return;
}

/*****************************************************************/
/*!
    * @brief �ָ��ַ���, �ָ�������Ƕ���ַ�, ��������ķָ����Ϊ���
    * @param [in]  inStr        �����ַ���
    * @param [in]  cc           �ָ��
    * @param [out] outLen       ���鳤��
    * @param [in]  maxLen       ����������
    * @param [out] iArr         ���ÿ���ַ�����ƫ��λ��
    * @return ��
    */
void _PDSC_SplitStrSM(const char *inStr, const char *cc, int *outLen, int maxLen, int iArr[])
{
    int j;
    char *p, *p1;

    iArr[0] = 0;
    j = 1;

    p1 = (char *)inStr;
    if(0 == cc[1])
    {
        while(NULL != (p = strchr(p1, cc[0])) )
        {
            p1 = p + 1;
            p[0] = 0;

            if(j >= maxLen)
                break;
            iArr[j] = p1 - inStr;
            j++;
        }
        *outLen = j;
        return;
    }
    else
    {
        unsigned int len = strlen(cc);
        while(NULL != (p = strstr(p1, cc)) )
        {
            p1 = p + len;
            p[0] = 0;

            if(j >= maxLen)
                break;
            iArr[j] = p1 - inStr;
            j++;
        }
        *outLen = j;
        return;
    }
}

/*****************************************************************/
/*!
    * @brief �滻��������
    * @param [in]  inDir       �����ַ���
    * @return ��
    */
void _PDSC_RepEnv(char *inDir)
{
    char tmpStr[1024]={0};
    char envStr[256], envStr1[128], *envValue;
    char *p, *p1;

    strcpy(tmpStr, inDir);
    /*���һ�������*/
    while( (p=strstr(tmpStr, "${"))!=NULL )
    {
        if( (p1=strstr(p, "}"))==NULL)
        {
            return;
        }
        memset(envStr, '\0', sizeof(envStr));
        strncpy(envStr, p+2, strlen(p)-strlen(p1)-2);
        envValue = getenv(envStr);
        sprintf(envStr1, "${%s}", envStr);
        _PDSC_StrRep(tmpStr, envStr1, (NULL==envValue)?"null":envValue);
    }
    if('.' == tmpStr[0])
    {
        getcwd(envStr, 256);
        sprintf(inDir, "%s/%s", envStr, tmpStr);
    }
    else
        strcpy(inDir, tmpStr);
    return;
}

/*****************************************************************/
/*!
    * @brief ����Login�����ļ�
    * @param [in]  inPassFile     �����ļ�
    * @param [in]  inUser         �����û���
    * @param [in]  inPass         ��������
    * @return 0 �ɹ���-1 �������ļ�����
    */
short _PDSC_SetLoginInfo(const char *inPassFile, const char *inUser, const char *inPass)
{
    FILE *fp;
    char tmpStr[512];
    int  i, k, len, pointer=0;
    char key[6] = "DFJLT";

    len = sprintf(tmpStr, "user\n");
    pointer=0;
    i = (int)strlen(inUser);
    for (k=0; k<i; k++)
    {
        tmpStr[len+k] = inUser[k]^key[pointer];
        pointer++;
        if (pointer == 5)
            pointer = 0;
    }
    tmpStr[len+i] = '\n';
    len += i + 1;

    len += sprintf(tmpStr+len, "pwd\n");
    pointer=0;
    i = (int)strlen(inPass);
    for (k=0; k<i; k++)
    {
        tmpStr[len+k] = inPass[k]^key[pointer];
        pointer++;
        if (pointer == 5)
            pointer = 0;
    }
    tmpStr[len+i] = '\n';
    len += i + 1;

    if((fp = fopen(inPassFile,"wb")) == NULL)
        return -1;

    fwrite(tmpStr, sizeof(char), len, fp);
    fclose(fp);
    return 0;
}

/*****************************************************************/
/*!
    * @brief ����Login�ļ�
    * @param [in]  inPassFile     �����ļ�
    * @param [out] outUser        ����û���
    * @param [out] outPass        �������
    * @return 0 �ɹ���-1 �������ļ�����
    */
short _PDSC_GetLoginInfo(const char *inPassFile, char *outUser, char *outPass)
{
    char tmpStr[512], *p, *p1;
    int  fd, k, len, pointer = 0;
    char key[6] = "DFJLT";
    struct stat bufSize;

    fd = open (inPassFile, O_RDONLY);
    if(fd == -1)
        return -1;

    stat(inPassFile, &bufSize);
    read(fd, tmpStr, bufSize.st_size);
    close (fd);

    p = NULL;
    for(k=6; k<bufSize.st_size-4; k++)
    {
        if(0 == strncmp(tmpStr+k, "pwd\n", 4))
        {
            p = tmpStr + k;
            break;
        }
    }
    if(p == NULL)
        return -1;

    p1 = tmpStr + 5;
    len = p - p1 - 1;
    pointer = 0;
    for (k = 0; k < len; k++)
    {
        outUser[k] = p1[k] ^ key[pointer];
        pointer++;
        if (pointer == 5)
            pointer = 0;
    }
    outUser[len] = 0;

    p1 = p + 4;
    len = tmpStr + bufSize.st_size - p1 -1;
    pointer = 0;
    for (k = 0; k < len; k++)
    {
        outPass[k] = p1[k] ^ key[pointer];
        pointer++;
        if (pointer == 5)
            pointer = 0;
    }
    outPass[len] = 0;
    return 0;
}

/*****************************************************************/
/*!
    * @brief ɾ���ַ��������˵�����ָ���ַ�
    * @param [in]  inStr            �����ַ���
    * @param [in]  beCutedChar      Ҫɾ�����ַ�
    * @param [in]  alignMode        Lɾ����ߣ�Rɾ���ұߣ�Cɾ������
    * @return ��
    */
void _PDSC_CutChar( char *inStr, char beCutedChar, char alignMode )
{
    int i, len;

    /*ɾ���ұ��ַ�*/
    if((alignMode=='R')||(alignMode=='C'))
    {
        len = (int)strlen(inStr);
        for(i=len-1; i>=0; i--)
        {
            if(inStr[i] != beCutedChar)
                break;
        }
        if(i < len-1)
            inStr[i+1] = '\0';
    }

    /*ɾ������ַ�*/
    if((alignMode=='L')||(alignMode=='C'))
    {
        len = (int)strlen(inStr);
        for(i=0; i<len; i++)
        {
            if(inStr[i] != beCutedChar)
                break;
        }
        if(i>0)
        {
            memmove(inStr, inStr+i, (size_t)(len-i));
            inStr[len-i] = '\0';
        }
    }
    return;
}
