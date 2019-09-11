#ifndef _QXML_H_
#define _QXML_H_

#if defined(DOODLE_EXPORTS) && defined(WIN32) 
#ifndef QLIB_API
#define QLIB_API __declspec(dllexport)
#endif
#else 
#ifndef QLIB_API
#define QLIB_API 
#endif
#endif

#include "QString.h"
#include "QField.h"


#ifdef __cplusplus
extern "C"{
#endif 

typedef enum
{
   DECLARE, /*宣告*/
   DISPOSE, /*处理 :) */
   NODE, /*节点*/
   COMMENT, /*注释*/
   HOLD, /*原样*/
   UNKNOW /*未知*/
}NODETYPE;

typedef struct tagQXmlEntity
{
   char *str;
   char chr;
}QXmlEntity;

extern char *xmlHeader;
extern char *commentHeader;
extern char *commentEnd;
extern char *cdataHeader;
extern char *cdataEnd;
extern QXmlEntity entity[];


enum ERROR_CODE
{
   XML_OK = 1, /*正常*/
   XML_OUT_MEMORY, /*内存不足*/
   XML_DATA_EMPTY, /*数据为空*/
   XML_IO_OPEN_FILE_ERROR, /*文件打开出错*/
   XML_ATTRIB_ERROR, /*错误的属性格式*/
   XML_NO_QUOTA, /*缺少引号*/
   XML_DECLARE_ERROR, /*宣告声明错误*/
   XML_COMMENT_ERROR, /*注释字段错误*/
   XML_NODE_START_ERROR, /*字段的起始格式错误*/
   XML_NODE_END_ERROR, /*字段的结束格式错误*/
   XML_READ_NODE_VALUE, /*读取节点的值出错*/
   XML_NO_CDATA_END, /*没有]]>的结束字段*/
   XML_CONV_TEXT, /*转义符错误*/
   XML_UNKNOW_FORMAT, /*未知格式*/
   XML_TEST_MSG
};

typedef struct tagQXml QXml;
typedef struct tagQXmlNode QXmlNode;

struct tagQXml
{	
   QXmlNode *(*ToRoot) (QXml *self);
   int (*ParseFile) (QXml *self,char *fileName);
   int (*ParseBuff) (QXml *self,char *buff);
   int (*SaveToFile) (QXml *self,char *fileName);
   char *(*GetErrText) (QXml *self);
   int (*GetDeclareData) (QXml *self,QString *data);
   void (*ReSet) (QXml *self);
   void (*Release) (QXml *self);

   QFieldList m_listDeclare; /* 宣告的链表,存储QString :) */

   QXmlNode *m_nodeRoot;
   int m_nErrorCode;
   unsigned long m_nRow;
   unsigned long m_nCol;
};

QXml QLIB_API NewQXml();
QXml QLIB_API *MallocQXml();


QXmlNode *QXmlNodeToRoot (QXml *self);
int QXmlParseFile (QXml *self,char *fileName);
int QXmlParseBuff (QXml *self,char *buff);
int QXmlSaveToFile (QXml *self,char *fileName);
char *QXmlGetErrText (QXml *self);
int QXmlGetDeclareData (QXml *self,QString *data);
void QXmlReSet (QXml *self);
void QXmlRelease (QXml *self);


struct tagQXmlNode
{
   QXmlNode *(*ToParent) (QXmlNode *self);
   QXmlNode *(*ToChild) (QXmlNode *self,char *name);
   QXmlNode *(*ToNextNode) (QXmlNode *self,char *name);
   QXmlNode *(*ToPrevNode) (QXmlNode *self,char *name);
   QXmlNode *(*ToFirstChild) (QXmlNode *self);
   QXmlNode *(*ToLastChild) (QXmlNode *self);

   BOOL (*AddChild) (QXmlNode *self,char *name ,char *value);
   BOOL (*AddNodeToChild) (QXmlNode *self,QXmlNode *node);

   BOOL (*SetNodeName) (QXmlNode *self , char *name);
   BOOL (*SetNodeValue) (QXmlNode *self , char *value);

   char *(*GetNodeName) (QXmlNode *self);
   char *(*GetNodeValue) (QXmlNode *self);
   int (*GetNodeType) (QXmlNode *self);
   int (*GetNodeData) (QXmlNode *self,QString *data ,int depth);

   BOOL (*SetAttrib) (QXmlNode *self , char *name , char * value);
   char *(*GetAttrib) (QXmlNode *self , char *name);
   BOOL (*RemoveAttrib) (QXmlNode *self , char *name);
   void (*Destory) (QXmlNode *self);
   void (*Release) (QXmlNode *self);
	
   QString m_szName;
   QString m_szValue;
   unsigned char *name;/*by zhuoch 20080821*/
   unsigned char *value;/*by zhuoch 20080821*/
   int type;

   QFieldList m_listAttrib;

   QXmlNode *xmlChildrenNode;
   QXmlNode *next;
   QXmlNode *prev;
   QXmlNode *parent;

   
};

QXmlNode QLIB_API *MallocQXmlNode();

QXmlNode *QXmlNodeToParent (QXmlNode *self);
QXmlNode *QXmlNodeToChild (QXmlNode *self,char *name);
QXmlNode *QXmlNodeToNextNode (QXmlNode *self,char *name);
QXmlNode *QXmlNodeToPrevNode (QXmlNode *self,char *name);
QXmlNode *QXmlNodeToFirstChild (QXmlNode *self);
QXmlNode *QXmlNodeToLastChild (QXmlNode *self);

BOOL QXmlNodeAddChild (QXmlNode *self,char *name ,char *value);
BOOL QXmlNodeAddNodeToChild (QXmlNode *self,QXmlNode *node);

BOOL QXmlNodeSetNodeName (QXmlNode *self , char *name);
BOOL QXmlNodeSetNodeValue (QXmlNode *self , char *value);

char *QXmlNodeGetNodeName (QXmlNode *self);
char *QXmlNodeGetNodeValue (QXmlNode *self);
int QXmlNodeGetNodeType (QXmlNode *self);
int QXmlNodeGetNodeData (QXmlNode *self,QString *data ,int depth);

BOOL QXmlNodeSetAttrib (QXmlNode *self , char *name , char * value);
char *QXmlNodeGetAttrib (QXmlNode *self , char *name);
BOOL QXmlNodeRemoveAttrib (QXmlNode *self , char *name);

void QXmlNodeDestory (QXmlNode *self);
void QXmlNodeRelease (QXmlNode *self);

int QXmlDeGetEntity (QString *str);

#ifdef __cplusplus
}
#endif

#endif
