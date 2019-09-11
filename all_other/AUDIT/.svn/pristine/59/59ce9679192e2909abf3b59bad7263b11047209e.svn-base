#ifndef _DOODLE_QSTRING_H_
#define _DOODLE_QSTRING_H_

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <malloc.h>
#endif
#include "QList.h"

#ifndef BOOL
#define BOOL	int
#define TRUE	1
#define FALSE	0
#endif


#ifndef NULL
#define NULL 0
#endif

#ifdef __cplusplus
extern "C"{
#endif 

typedef struct tagQString QString;

struct tagQString
{
   void (*Init)(QString * self);
   void (*Release)(QString * self);
   BOOL (*AllocBuffer)(QString * self,int nLen);
   int (*GetLength)(QString * self);
   BOOL (*IsEmpty)(QString * self);
   void (*ReSet)(QString * self,const char *szStr);	
   void (*Append)(QString * self,const char *szStr,int nLen);
   void (*AppendChar)(QString * self,char ch);
   void (*AppendInt)(QString * self,int n);
   void (*AppendString)(QString * self,const char *str);

   char (*GetAt)(QString * self,int index);
   BOOL (*Include)(QString * self,char ch);
   int (*Find)(QString * self,char ch,int offset);
   int (*FindString)(QString * self,const char *str,int offset);

   int (*ToInt)(QString * self);

   int (*CompareString)(QString * self,const char * lpsz);
   BOOL (*Equal)(QString * self,const char * lpsz);

   int (*Replace)(QString * self,char chOld, char chNew);

   char *(*Mid)(QString * self,int nFirst);
   char *(*MidIndex)(QString * self,int nFirst,int nCount);

   char *(*Left)(QString * self,int nCount);
   char *(*Right)(QString * self,int nCount);
	
   void (*Trim)(QString * self);
   void (*TrimRight)(QString * self);		
   void (*TrimLeft)(QString * self);	
	
   char *(*MakeUpper)(QString * self);		
   char *(*MakeLower)(QString * self);	
	
   int (*Delete)(QString * self,int nIndex, int nCount );	
   QList (*Split)(QString * self,char ch );	

   char * m_dataBuff;
   int m_nDataLength;
   int m_nAllocLength;
};

QString QLIB_API NewQString();
QString QLIB_API *MallocQString();

void QStringInit(QString * self);
void QStringRelease(QString * self);
BOOL QStringAllocBuffer(QString * self,int nLen);

int QStringGetLength(QString * self);
BOOL QStringIsEmpty(QString * self);

void QStringReSet(QString * self,const char *szStr);
void QStringAppend(QString * self,const char *szStr,int nLen);
void QStringAppendChar(QString * self,char ch);
void QStringAppendInt(QString * self,int n);
void QStringAppendString(QString * self,const char *str);

char QStringGetAt(QString * self,int index);
BOOL QStringInclude(QString * self,char ch);
int QStringFind(QString * self,char ch,int offset);
int QStringFindString(QString * self,const char *str,int offset);

int QStringToInt(QString * self);

int QStringCompareString(QString * self,const char * lpsz);
BOOL QStringEqual(QString * self,const char * lpsz);

int QStringReplace(QString * self,char chOld, char chNew);

char *QStringMid(QString * self,int nFirst);
char *QStringMidIndex(QString * self,int nFirst,int nCount);

char *QStringLeft(QString * self,int nCount);
char *QStringRight(QString * self,int nCount);

void QStringTrim(QString * self);
void QStringTrimRight(QString * self);		
void QStringTrimLeft(QString * self);	

char *QStringMakeUpper(QString * self);		
char *QStringMakeLower(QString * self);	

int QStringDelete(QString * self,int nIndex, int nCount );
QList QStringSplit(QString * self,char ch );	


typedef struct tagQStringList QStringList;

struct tagQStringList
{
   void (*Release)(QStringList * self);
   QList list;
};

QStringList QLIB_API NewQStringList();
QStringList QLIB_API *MallocQStringList();

void QStringListRelease(QStringList * self);
#ifdef __cplusplus
}
#endif

#endif
