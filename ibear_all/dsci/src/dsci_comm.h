/*!
 *  @file MyPublic.h ������������
 *  <br> copyright (C), 1995-2005, Si-Tech Information Technology Ltd
 *  @author  wangjs
 *  @version 1.0
 *  @date    2007-12-28
 */

#ifndef _PUB_FUNC_H_
#define _PUB_FUNC_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define int4store(T,A)    {  *((T))=(unsigned char) ((A));\
                            *((T)+1)=(unsigned char) (((A) >> 8));\
                            *((T)+2)=(unsigned char) (((A) >> 16));\
                            *((T)+3)=(unsigned char) (((A) >> 24)); }

#define uint4korr(A)    (long) (((int) ((unsigned char) (A)[0])) +\
                            (((int) ((unsigned char) (A)[1])) << 8) +\
                            (((int) ((unsigned char) (A)[2])) << 16) +\
                            (((long) ((unsigned char) (A)[3])) << 24))

extern int _PDSC_uint4korr(const char *buf);

/*��ȡϵͳ΢����*/
extern double _PDSC_GetMicSec(void);

/*malloc*/
extern void *_PDSC_MemMalloc(size_t size);

/*free*/
extern void _PDSC_MemFree(void *p);

/*�滻�ַ���*/
extern void _PDSC_StrRep( char *inStr, const char *oldStr, const char *newStr );

/*�ָ��ַ���,��������ķָ����Ϊ1��*/
extern void _PDSC_SplitStrD(const char *inStr, char *outStr, char delimiter, int *outLen, int maxLen, int iArr[]);

/*�ָ��ַ���,��������ķָ����Ϊ���*/
extern void _PDSC_SplitStrM(const char *inStr, char *outStr, char delimiter, int *outLen, int maxLen, int iArr[]);

/*�ָ��ַ���,��������ķָ����Ϊ���(��������)*/
extern void _PDSC_SplitStrfM(char *inStr, char cc, int *outLen, int maxLen, int iArr[]);

/*�ָ��ַ���, �ָ�������Ƕ���ַ�, ��������ķָ����Ϊ���*/
extern void _PDSC_SplitStrSM(const char *inStr, const char *cc, int *outLen, int maxLen, int iArr[]);

/*����Login�����ļ�*/
extern short _PDSC_SetLoginInfo(const char *inPassFile, const char *inUser, const char *inPass);

/*���������ļ��õ���½�û��Ϳ���*/
extern short _PDSC_GetLoginInfo(const char *inPassFile, char *outUser, char *outPass);

/*ɾ���ַ��������˵�����ָ���ַ�*/
extern void _PDSC_CutChar( char *inStr, char beCutedChar, char alignMode );

/*�滻��������*/
extern void _PDSC_RepEnv(char *inDir);


#if defined(__cplusplus)
}
#endif

#endif