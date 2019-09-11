/*!
 *  @file ini_rw.h ��дINI�ļ�
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

/*��INI�ļ��ж�ȡָ��section��key��������ַ���*/
extern int _INIDSC_Read( char *errStr, FILE *inFileFp, const char *section, const char *key, char *outString );

/*����INI�ļ�����ȡָ��section��key��������ַ���*/
extern int _INIDSC_ReadFile( char *errStr, const char *inFileName, const char *section, const char *key, char *outString );

/*��ȡINI�ļ��е�section*/
extern int _INIDSC_GetSection(FILE *inFileFp, char *outSection);

/*��ȡINI�ļ���ָ��section������key*/
extern int _INIDSC_GetSectionKey(FILE *inFileFp, char *inSection, int *flag, char *outKey, char *outValue);

/*��INI�ļ���д��ָ��sectkon��key��������ַ���*/
extern int _INIDSC_Write( const char *iniFileName, const char *section, const char *key,  const char *inString );

/*ɾ��INI�ļ��е�section*/
extern int _INIDSC_DeleteSection( const char *iniFileName, const char *section );

/*������INI�ļ��е�section*/
extern int _INIDSC_RenameSection( const char *iniFileName, const char *oldSec, const char *newSec );

/*���ļ���ȡһ����¼*/
extern char *_INIDSC_GetFileStr(FILE *fp, char *outStr, int size);

#if defined(__cplusplus)
}
#endif

#endif
