/*!
 *  @file ini_rw.h 读写INI文件
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2005-12-20
 */

#ifndef _RW_INIDSC_H_
#define _RW_INIDSC_H_

#if defined(__cplusplus)
extern "C" {
#endif

/*从INI文件中读取指定section及key所代表的字符串*/
extern int _INIDSC_Read( char *errStr, FILE *inFileFp, const char *section, const char *key, char *outString );

/*根据INI文件名读取指定section及key所代表的字符串*/
extern int _INIDSC_ReadFile( char *errStr, const char *inFileName, const char *section, const char *key, char *outString );

/*读取INI文件中的section*/
extern int _INIDSC_GetSection(FILE *inFileFp, char *outSection);

/*读取INI文件中指定section的所有key*/
extern int _INIDSC_GetSectionKey(FILE *inFileFp, char *inSection, int *flag, char *outKey, char *outValue);

/*向INI文件中写入指定sectkon及key所代表的字符串*/
extern int _INIDSC_Write( const char *iniFileName, const char *section, const char *key,  const char *inString );

/*删除INI文件中的section*/
extern int _INIDSC_DeleteSection( const char *iniFileName, const char *section );

/*重命名INI文件中的section*/
extern int _INIDSC_RenameSection( const char *iniFileName, const char *oldSec, const char *newSec );

/*从文件中取一条记录*/
extern char *_INIDSC_GetFileStr(FILE *fp, char *outStr, int size);

#if defined(__cplusplus)
}
#endif

#endif
