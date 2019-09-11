
#ifndef _QXMLIO_H_
#define _QXMLIO_H_

#if defined(DOODLE_EXPORTS) && defined(WIN32) 
#ifndef QLIB_API
#define QLIB_API __declspec(dllexport)
#endif
#else 
#ifndef QLIB_API
#define QLIB_API 
#endif
#endif

#include <fcntl.h>

#include "QString.h"

#define XML_BUFF_DATA_LENGTH 1024
#ifdef __cplusplus
extern "C"{
#endif 

typedef struct tagQXmlIO QXmlIO;

struct tagQXmlIO
{
   int (*FromFile) ( QXmlIO *self,int fd);
   int (*FromBuff) ( QXmlIO *self,char *buff);
   int (*GetBuff) ( QXmlIO *self , char *buff,int len);
   void (*SkipWhiteSpace)(QXmlIO *self);		
   int (*SkipChar) ( QXmlIO *self,int n);
   BOOL (*IsEmpty) ( QXmlIO *self);
   void (*Release) ( QXmlIO *self);
   int (*GetChar) ( QXmlIO *self);
   
   int m_nFd;
   char *m_pBuff;

   unsigned long   m_nRow;
   unsigned long   m_nCol;
   
   int m_bDataOver;
   QString m_szTempBuff;

};


QXmlIO QLIB_API NewQXmlIO();
QXmlIO QLIB_API *MallocQXmlIO();

int QXmlIOFromFile ( QXmlIO *self,int fd);
int QXmlIOFromBuff ( QXmlIO *self,char *buff);

int QXmlIOGetBuff ( QXmlIO *self , char *buff,int len);
void QXmlIOSkipWhiteSpace (QXmlIO *self);      
int QXmlIOSkipChar ( QXmlIO *self,int n);
BOOL QXmlIOIsEmpty ( QXmlIO *self);

void QXmlIORelease ( QXmlIO *self);
/* :( 有时候为了实现类似于C++的多态也是一件很无聊的事 */

int QXmlIOGetFileChar ( QXmlIO *self);
int QXmlIOGetBuffChar ( QXmlIO *self);

#ifdef __cplusplus
}
#endif

#endif
