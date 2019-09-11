/*!
 *  @file ini_rw.c 读写INI文件
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2005-12-20
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*替换字符串*/
void _IDSC_StrRep( char *inStr, const char *oldStr, const char *newStr )
{
    unsigned int len, i=0, j=0;
    char tmpStr[2176];

    len = strlen(oldStr);
    while( inStr[i]!='\0' )
    {
        if(strncmp(inStr+i, oldStr, len)==0)
        {
            j = j + (unsigned int)sprintf(tmpStr+j, "%s", newStr);
            i = i + len;
        }
        else
        {
            tmpStr[j++] = inStr[i];
            i++;
        }
    }
    tmpStr[j] = '\0';
    strcpy(inStr, tmpStr);
    return;
}

/*去除字符串两边的指定字符*/
void _IDSC_CutChar( char *inStr, char beCutedChar, char alignMode )
{
    int i, len;

    /*删除右边字符*/
    if((alignMode=='L')||(alignMode=='C'))
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
    /*删除左边字符*/
    if((alignMode=='R')||(alignMode=='C'))
    {
        len = (int)strlen(inStr);
        for(i=0; i<len; i++)
        {
            if(inStr[i] != beCutedChar)
                break;
        }
        if(i>0)
        {
            memmove(inStr, inStr+i, (unsigned int)(len-i));
            inStr[len-i] = '\0';
        }
    }
    return;
}

/*查找KEY*/
int _INIDSC_FindStrFp( FILE  *iniFileStream, const char *section, const char *key, char *outString, int *sectionRow )
{
    char tmpStr[2176], tmpSection[100], *p;
    int flag, i, row;

    sprintf(tmpSection, "[%s]", section);
    /* 下面这个while循环查找[section]所在的位置 */
    flag=0;
    row = 0;
    *sectionRow = 0;
    i = 0;
    rewind(iniFileStream);
    while( !feof( iniFileStream ) )
    {
        memset( tmpStr, '\0', sizeof(tmpStr) );    /* 初始化临时INI数据缓存区 */
        if( fgets( tmpStr, sizeof(tmpStr), iniFileStream ) == NULL )
        {
            if( feof( iniFileStream ) )  /* 判断是否到达文件流的尾 */
                break;
            else
                return -1;
        }

        i++;
        /*清除无用字符*/
        if( ( (p=strchr(tmpStr,'#' ))!=NULL ) ||
            ( (p=strchr(tmpStr,'\r'))!=NULL ) ||
            ( (p=strchr(tmpStr,'\n'))!=NULL ) )
            p[0]=0;

        _IDSC_StrRep( tmpStr, "\t", " " );
        _IDSC_CutChar( tmpStr, ' ', 'C' );
        if(strlen(tmpStr)==0)
            continue;

        if( flag == 0 )
        {
            if( !strcasecmp( tmpSection, tmpStr ) )
            {
                flag=1;                /*找到section了*/
                *sectionRow = i;
            }
        }
        else
        {
            if( strchr( tmpStr, '[' ) != NULL )  /* 没有找到，已经到下一个关键字段 */
                break;
            if( ( p = strstr( tmpStr, "=" ) ) == NULL )
                continue;

            p[0] = 0;
            _IDSC_CutChar( tmpStr, ' ', 'L' );
            if( strcasecmp(tmpStr, key) == 0 )
            {
                row = i;
                _IDSC_CutChar( p+1, ' ', 'R' );
                strcpy( outString, p+1 );
                return row;
            }
        }
    }
    return -1;
}

/*****************************************************************/
/*!
    * @brief 读取指定section及key所代表的字符串
    * @param [out] errStr       出错字符串
    * @param [in]  inFileFp     ini文件指针
    * @param [in]  section      ini文件的section字符串
    * @param [in]  key          ini文件的key字符串
    * @param [out] outString    输出字符串
    * @return >0 成功,key所在行数,  -1 出错
    */
int _INIDSC_Read( char *errStr, FILE *inFileFp, const char *section, const char *key, char *outString )
{
    int   row, sectionRow;
    row = _INIDSC_FindStrFp( inFileFp, section, key, outString, &sectionRow );

    if( row < 0 )
    {
        sprintf(errStr, "Error on read cfg file [%s]--[%s]", section, key);
        return -1;
    }
    else
        return row;
}

/*****************************************************************/
/*!
    * @brief 根据INI文件名读取指定section及key所代表的字符串
    * @param [out] errStr       出错字符串
    * @param [in]  inFileName   ini文件名
    * @param [in]  section      ini文件的section字符串
    * @param [in]  key          ini文件的key字符串
    * @param [out] outString    输出字符串
    * @return >0 成功,key所在行数,  -1 出错
    */
int _INIDSC_ReadFile( char *errStr, const char *inFileName, const char *section, const char *key, char *outString )
{
    FILE *inFileFp;
    int   row, sectionRow;

    if(NULL == (inFileFp = fopen(inFileName, "r")) )
    {
        sprintf(errStr, "Can't open file %s", inFileName);
        return -1;
    }

    row = _INIDSC_FindStrFp( inFileFp, section, key, outString, &sectionRow );

    if( row < 0 )
    {
        sprintf(errStr, "Error on read cfg file [%s]--[%s]", section, key);
        fclose(inFileFp);
        return -1;
    }

    fclose(inFileFp);
    return row;
}

/*****************************************************************/
/*!
    * @brief 读取INI文件中的section
    * @param [in]  inFileFp     ini文件指针
    * @param [out] outSection   输出section字符串
    * @return >0 成功,section所在行数,  -1 出错
    */
int _INIDSC_GetSection(FILE *inFileFp, char *outSection)
{
    char tmpStr[2176], *p;
    int i, len;

    i = 0;
    while( !feof( inFileFp ) )
    {
        memset( tmpStr, '\0', sizeof(tmpStr) );    /* 初始化临时INI数据缓存区 */
        if( fgets( tmpStr, sizeof(tmpStr), inFileFp ) == NULL )
        {
            if( feof( inFileFp ) )  /* 判断是否到达文件流的尾 */
                break;
            else
                return -1;
        }

        i++;
        /*清除无用字符*/
        if( ( (p=strchr(tmpStr,'#' ))!=NULL ) ||
            ( (p=strchr(tmpStr,'\r'))!=NULL ) ||
            ( (p=strchr(tmpStr,'\n'))!=NULL ) )
            p[0]=0;

        _IDSC_StrRep( tmpStr, "\t", " " );
        _IDSC_CutChar( tmpStr, ' ', 'C' );
        if(strlen(tmpStr)==0)
            continue;

        len = (int)strlen(tmpStr);
        if( (tmpStr[0] == '[') && (tmpStr[len-1] == ']') )
        {
            strncpy(outSection, tmpStr+1, (size_t)(len-2));
            outSection[len-2] = 0;
            return i;
        }
    }
    return -1;
}

/*****************************************************************/
/*!
    * @brief 读取INI文件中指定section的所有key
    * @param [in]  inFileFp     ini文件指针
    * @param [in]  inSection    section字符串
    * @param [in]  flag         标志， 0首次查， 1 非首次查
    * @param [out] outKey       输出Key字符串
    * @param [out] outValue     输出Key对应的值
    * @return >0 成功,key所在行数,  -1 出错
    */
int _INIDSC_GetSectionKey(FILE *inFileFp, char *inSection, int *flag, char *outKey, char *outValue)
{
    char tmpStr[2176], *p;
    int i, len;

    i = 0;
    while( !feof( inFileFp ) )
    {
        memset( tmpStr, '\0', sizeof(tmpStr) );    /* 初始化临时INI数据缓存区 */
        if( fgets( tmpStr, sizeof(tmpStr), inFileFp ) == NULL )
        {
            if( feof( inFileFp ) )  /* 判断是否到达文件流的尾 */
                break;
            else
                return -1;
        }

        i++;
        /*清除无用字符*/
        if( ( (p=strchr(tmpStr,'#' ))!=NULL ) ||
            ( (p=strchr(tmpStr,'\r'))!=NULL ) ||
            ( (p=strchr(tmpStr,'\n'))!=NULL ) )
            p[0]=0;

        _IDSC_StrRep( tmpStr, "\t", " " );
        _IDSC_CutChar( tmpStr, ' ', 'C' );
        if(strlen(tmpStr)==0)
            continue;

        len = (int)strlen(tmpStr);
        if( (tmpStr[0] == '[') && (tmpStr[len-1] == ']') )
        {
            if(1 == *flag)
                return -1;
            else if(0 == strncasecmp(tmpStr+1, inSection, len-2))
                *flag = 1;
        }
        else if( (1 == *flag) && (NULL != (p = strstr( tmpStr, "=" ))) )
        {
            p[0] = 0;
            _IDSC_CutChar( tmpStr, ' ', 'L' );
            _IDSC_CutChar( p+1, ' ', 'R' );
            strcpy( outKey, tmpStr );
            strcpy( outValue, p+1 );
            return i;
        }
    }
    return -1;
}

/*****************************************************************/
/*!
    * @brief 写入指定section及key所代表的字符串
    * @param [in]  iniFileName     ini文件名
    * @param [in]  section         ini文件的section字符串
    * @param [in]  key             ini文件的key字符串
    * @param [in]  inString        需写入的字符串
    * @return 0 成功,  -1 出错
    */
int _INIDSC_Write( const char *iniFileName, const char *section, const char *key,  const char *inString )
{
    FILE  *fp, *tmpFp;
    char  buf[2176], tmpStr[2176];
    char  tmpIni[300];
    char  *ptrChar;
    int   i, row, sectionRow, n;

    /*查找key*/
    if(NULL == (fp = fopen(iniFileName, "r")) )
        return -1;
    row = _INIDSC_FindStrFp( fp, section, key, buf, &sectionRow );
    fclose(fp);

    /*section不存在*/
    if( sectionRow == 0 )
    {
        if( ( fp = fopen( iniFileName, "a+" ) ) == NULL )
            return -1;

        n = sprintf(tmpStr, "\n[%s]\n%s = %s\n", section, key, inString);
        fseek(fp, 0, SEEK_END);
        if(n != fprintf(fp, "%s", tmpStr))
            return -1;
        if(0 != fclose(fp))
            return -1;
        return 0;
    }

    /*section存在, Key不存在*/
    if( ( fp = fopen( iniFileName, "r" ) ) == NULL )
         return -1;

    sprintf(tmpIni, "%s.~~temp~~", iniFileName);
    if( ( tmpFp = fopen( tmpIni, "w" ) ) == NULL )
    {
        fclose(fp);
        return -1;
    }

    i=0;
    while( !feof( fp ) )
    {
        memset( buf, '\0', sizeof(buf) );    /* 初始化临时INI数据缓存区 */
        if(NULL == fgets( buf, sizeof(buf), fp))
            break;

        n = 0;
        i++;
        if(row <= 0)  /*key不存在*/
        {
            if(sectionRow == i)
                n = sprintf(tmpStr, "%s%s = %s\n", buf, key, inString);
        }
        else
        {
            if (row ==i)
            {
                if( ( ptrChar = strstr( buf, "#" ) ) != NULL )
                    n = sprintf(tmpStr, "%s = %s  %s", key, inString, ptrChar);
                else
                    n = sprintf(tmpStr, "%s = %s\n", key, inString);
            }
        }

        if(n == 0)
            n = sprintf(tmpStr, "%s", buf);

        if(n != fprintf(tmpFp, "%s", tmpStr))
        {
            fclose(fp);
            fclose(tmpFp);
            remove(tmpIni);
            return -1;
        }
    }
    fclose(fp);

    if(0 != fclose(tmpFp))
    {
        remove(tmpIni);
        return -1;
    }

    remove(iniFileName);
    rename(tmpIni, iniFileName);
    return 0;
}

/*****************************************************************/
/*!
    * @brief 删除INI文件中的section
    * @param [in]  iniFileName     ini文件名
    * @param [in]  section         ini文件的section字符串
    * @return 0 成功,  -1 出错
    */
int _INIDSC_DeleteSection( const char *iniFileName, const char *section )
{
    FILE  *fp;
    char  *fbuf, hStr[5], tmpSec[100], tmpIni[300], *p, *p1, *p2;
    struct stat bufSize;

    if(NULL == (fp = fopen(iniFileName, "rb")) )
        return -1;

    stat(iniFileName, &bufSize);
    if( (bufSize.st_size <= 0) || (NULL == (fbuf = (char *)malloc((size_t)(bufSize.st_size+1)))) )
    {
        fclose(fp);
        return -1;
    }
    fread(fbuf, 1, (size_t)(bufSize.st_size), fp);
    p2 = fbuf + bufSize.st_size;
    p2[0] = 0;
    fclose(fp);

#ifdef WIN32_OS
    strcpy(hStr, "\r\n[");
#else
    strcpy(hStr, "\n[");
#endif

    /*查找section*/
    sprintf(tmpSec, "%s%s]", hStr, section);
    if(NULL == (p = strstr(fbuf, tmpSec)))
    {
        free(fbuf);
        return -1;
    }
    p1 = strstr(p+3, hStr);

    /*删除section*/
    sprintf(tmpIni, "%s.~~temp~~", iniFileName);
    if( ( fp = fopen( tmpIni, "wb" ) ) == NULL )
    {
        free(fbuf);
        return -1;
    }
    if(p-fbuf > 0)
    {
        if((size_t)(p-fbuf) != fwrite(fbuf, 1, (size_t)(p-fbuf), fp))
        {
            free(fbuf);
            fclose(fp);
            remove(tmpIni);
            return -1;
        }
    }
    if(NULL != p1)
    {
        if((size_t)(p2-p1) != fwrite(p1, 1, (size_t)(p2-p1), fp) )
        {
            free(fbuf);
            fclose(fp);
            remove(tmpIni);
            return -1;
        }
    }

    free(fbuf);
    if(0 != fclose(fp))
    {
        remove(tmpIni);
        return -1;
    }
    remove(iniFileName);
    rename(tmpIni, iniFileName);
    return 0;
}

/*****************************************************************/
/*!
    * @brief 重命名INI文件中的section
    * @param [in]  iniFileName     ini文件名
    * @param [in]  oldSec          ini文件的section字符串
    * @param [in]  newSec          新的的section字符串
    * @return 0 成功,  -1 出错
    */
int _INIDSC_RenameSection( const char *iniFileName, const char *oldSec, const char *newSec )
{
    FILE  *fp;
    int k,n;
    char  *fbuf, hStr[5], tmpSec[100], tmpIni[300], *p, *p2;
    struct stat bufSize;

    if(NULL == (fp = fopen(iniFileName, "rb")) )
        return -1;

    stat(iniFileName, &bufSize);
    if( (bufSize.st_size <= 0) || (NULL == (fbuf = (char *)malloc((size_t)(bufSize.st_size+1)))) )
    {
        fclose(fp);
        return -1;
    }
    fread(fbuf, 1, (size_t)(bufSize.st_size), fp);
    p2 = fbuf + bufSize.st_size;
    p2[0] = 0;
    fclose(fp);

#ifdef WIN32_OS
    strcpy(hStr, "\r\n[");
#else
    strcpy(hStr, "\n[");
#endif

    /*查找section*/
    n = sprintf(tmpSec, "%s%s]", hStr, oldSec);
    if(NULL == (p = strstr(fbuf, tmpSec)))
    {
        free(fbuf);
        return -1;
    }

    /*修改section*/
    sprintf(tmpIni, "%s.~~temp~~", iniFileName);
    if( ( fp = fopen( tmpIni, "wb" ) ) == NULL )
    {
        free(fbuf);
        return -1;
    }
    k = sprintf(tmpSec, "%s%s]", hStr, newSec);
    if(p-fbuf > 0)
    {
        if( (size_t)(p-fbuf) != fwrite(fbuf, 1, (size_t)(p-fbuf), fp) )
        {
            free(fbuf);
            fclose(fp);
            remove(tmpIni);
            return -1;
        }
    }

    if( ((size_t)k != fwrite(tmpSec, 1, (size_t)k, fp)) ||
        ((size_t)(p2-p-n) != fwrite(p+n, 1, (size_t)(p2-p-n), fp)) )
    {
        free(fbuf);
        fclose(fp);
        remove(tmpIni);
        return -1;
    }

    free(fbuf);
    if(0 != fclose(fp))
    {
        remove(tmpIni);
        return -1;
    }

    remove(iniFileName);
    rename(tmpIni,iniFileName);
    return 0;
}

/*从文件中取一条记录*/
char *_INIDSC_GetFileStr(FILE *fp, char *outStr, int size)
{
    char *p;
    if(NULL == fgets(outStr, size, fp))
        return NULL;

    if( NULL != (p=strchr(outStr,'\n')) )
        p[0]=0;
    return outStr;
}
