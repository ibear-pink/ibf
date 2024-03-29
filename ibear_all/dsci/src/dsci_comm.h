/*!
 *  @file MyPublic.h 公共操作函数
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

/*获取系统微秒数*/
extern double _PDSC_GetMicSec(void);

/*malloc*/
extern void *_PDSC_MemMalloc(size_t size);

/*free*/
extern void _PDSC_MemFree(void *p);

/*替换字符串*/
extern void _PDSC_StrRep( char *inStr, const char *oldStr, const char *newStr );

/*分割字符串,多个连续的分割符视为1个*/
extern void _PDSC_SplitStrD(const char *inStr, char *outStr, char delimiter, int *outLen, int maxLen, int iArr[]);

/*分割字符串,多个连续的分割符视为多个*/
extern void _PDSC_SplitStrM(const char *inStr, char *outStr, char delimiter, int *outLen, int maxLen, int iArr[]);

/*分割字符串,多个连续的分割符视为多个(覆盖输入)*/
extern void _PDSC_SplitStrfM(char *inStr, char cc, int *outLen, int maxLen, int iArr[]);

/*分割字符串, 分割符可以是多个字符, 多个连续的分割符视为多个*/
extern void _PDSC_SplitStrSM(const char *inStr, const char *cc, int *outLen, int maxLen, int iArr[]);

/*生成Login密码文件*/
extern short _PDSC_SetLoginInfo(const char *inPassFile, const char *inUser, const char *inPass);

/*根据密码文件得到登陆用户和口令*/
extern short _PDSC_GetLoginInfo(const char *inPassFile, char *outUser, char *outPass);

/*删除字符串中两端的所有指定字符*/
extern void _PDSC_CutChar( char *inStr, char beCutedChar, char alignMode );

/*替换环境变量*/
extern void _PDSC_RepEnv(char *inDir);


#if defined(__cplusplus)
}
#endif

#endif
