#ifndef _QXMLPARSER_H_
#define _QXMLPARSER_H_
#include "QXml.h"
#include "QXmlIO.h"
#include "QField.h"

#ifdef __cplusplus
extern "C"{
#endif 

typedef struct tagQXmlParser QXmlParser;

struct tagQXmlParser
{
   int (*Parse) (QXmlParser *self,QXml *xml);
   int (*Worker) (QXmlParser *self);
   int (*FromFile) (QXmlParser *self,int fd);
   int (*FromBuff) (QXmlParser *self,char *buff);
   void (*ParseDeclaration) (QXmlParser *self);
   void (*ParseCommentText) (QXmlParser *self);
   void (*ParseNode) (QXmlParser *self);
   QField *(*ParseAttrib) (QXmlParser *self);
   int (*ReadName) (QXmlParser *self , QString *Name);

   int (*ReadValue) (QXmlParser *sel , QString *Value, 
                     char *endTag, /*以什么结束*/ 
                     BOOL conv   /*是否进行 '&' 的转换*/);

   int (*ReadText) (QXmlParser *self , QString *Text);
   char (*GetEntity) ( QXmlParser *self ,char *data);
   void (*Release) ( QXmlParser *self);
   QXml *xml;
   QXmlIO m_ioXML;
   QXmlNode *m_nodeCurr;
};

QXmlParser QLIB_API NewQXmlParser();
QXmlParser QLIB_API *MallocQXmlParser();

int QXmlParserParse (QXmlParser *self,QXml *xml);
int QXmlParserWorker (QXmlParser *self);
int QXmlParserFromFile (QXmlParser *self,int fd);
int QXmlParserFromBuff (QXmlParser *self,char *buff);

void QXmlParserDeclaration (QXmlParser *self);
void QXmlParserCommentText (QXmlParser *self);
void QXmlParserNode (QXmlParser *self);

QField *QXmlParserAttrib (QXmlParser *self);
int QXmlParserReadName (QXmlParser *self , QString *Name);
int QXmlParserReadValue (QXmlParser *self ,                   
                         QString *Value,
                         char *endTag, /*以什么结束*/
                         BOOL conv   /*是否进行 '&' 的转换*/);
int  QXmlParserReadText (QXmlParser *self , QString *Text);

char QXmlParserGetEntity (QXmlParser *self , char *data);
void  QXmlParserRelease ( QXmlParser *self);

#ifdef __cplusplus
}
#endif

#endif
