
#ifndef _LIBXML_H_
#define _LIBXML_H_

#ifdef __cplusplus
extern "C"{
#endif 
#include "libxml/QXml.h"
#define LIBXML_TEST_VERSION
#define LIBXML_SAX1_ENABLED

#define LIBXML_LEVEL_DEFAULT  128
typedef unsigned char xmlChar;


#define XML_ELEMENT_NODE NODE
typedef  QXml* xmlDocPtr;
typedef QXml xmlDoc;
typedef QXmlNode xmlNode;
typedef QXmlNode* xmlNodePtr;
typedef int xmlAttrPtr; 
typedef char*  xmlNsPtr; 

/*
 * A node-set (an unordered collection of nodes without duplicates).
 */
typedef struct _xmlNodeSet xmlNodeSet;
typedef xmlNodeSet *xmlNodeSetPtr;
struct _xmlNodeSet {
    int nodeNr;			/* number of nodes in the set */
    int nodeMax;		/* size of the array as allocated */
    xmlNodePtr *nodeTab;	/* array of nodes in no particular order */
    /* @@ with_ns to check wether namespace nodes should be looked at @@ */
};


typedef struct _xmlXPathObject xmlXPathObject;
typedef xmlXPathObject *xmlXPathObjectPtr;
struct _xmlXPathObject {
    xmlNodeSetPtr nodesetval;/*result nodes*/
};


/**
 * xmlXPathContext:
 *
 * Expression evaluation occurs with respect to a context.
 * he context consists of:
 *    - a node (the context node) 
 *    - a node list (the context node list) 
 *    - a set of variable bindings 
 *    - a function library 
 *    - the set of namespace declarations in scope for the expression 
 * Following the switch to hash tables, this need to be trimmed up at
 * the next binary incompatible release.
 */

struct _xmlXPathContext {
    xmlDocPtr doc;			/* The current document */
    xmlNodePtr node;			/* The current node */
};
typedef struct _xmlXPathContext xmlXPathContext;
typedef xmlXPathContext *xmlXPathContextPtr;

/******************************XPath使用函数************************************************/
/*
xmlChar* xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine);*/
/**
 * xmlXPathNodeSetIsEmpty:
 * @ns: a node-set
 *
 * Checks whether @ns is empty or not.
 *
 * Returns %TRUE if @ns is an empty node-set.
 */
#define xmlXPathNodeSetIsEmpty(ns) (((ns) == NULL) || ((ns)->nodeNr == 0) || ((ns)->nodeTab == NULL))

xmlXPathContextPtr xmlXPathNewContext(xmlDocPtr doc);
void xmlXPathFreeContext(xmlXPathContextPtr ctxt);
xmlXPathObjectPtr xmlXPathEvalExpression(const xmlChar *str, xmlXPathContextPtr ctxt);
void xmlXPathFreeObject(xmlXPathObjectPtr obj);

/*****************************************************************************************/


/*xml针对空格设置，此函数为空函数可不实现，为统一xml接口使用*/
int xmlKeepBlanksDefault(int val);

/*释放xml变量*/
void xmlFreeDoc(xmlDocPtr cur);

/*解析xml文件，生成内存xml变量*/
xmlDocPtr xmlParseFile(const char *filename);

/*得到xml根节点*/
xmlNodePtr xmlDocGetRootElement(xmlDocPtr doc);

/*是否是空节点，此函数为空函数可不实现，为统一xml接口使用*/
int xmlIsBlankNode(xmlNodePtr node);

/*得到节点对应的属性值*/
xmlChar* xmlGetProp(xmlNodePtr node, const xmlChar *name);

/*设置节点对应的属性值*/
xmlAttrPtr xmlSetProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value);

/*将内存xml变量，生成xml文件*/
int xmlSaveFile(const char *filename, xmlDocPtr cur);

/*变量释放，此函数为空函数可不实现，为统一xml接口使用*/
void xmlFree(void *ptr);


/*将内存xml变量，生成xml到buffer中，返回<0表示错，大于等于0表示生成xml的buff长度*/
int xmlSaveToBuff (xmlDocPtr cur,char *buff);

/*解析buffer中的xml，生成内存xml变量*/
xmlDocPtr xmlParseMemory(const char *buffer, int size);

/*释放全局xml变量，此函数为空函数可不实现，为统一xml接口使用*/
void xmlCleanupParser(void);

/*生成xml变量*/
xmlDocPtr xmlNewDoc(const xmlChar *version);

/*生成xml节点*/
xmlNodePtr xmlNewNode(xmlNsPtr ns,const xmlChar *name);

/*设置xml根节点*/
xmlNodePtr xmlDocSetRootElement(xmlDocPtr doc, xmlNodePtr root);

/*xml中当前节点增加孩子节点*/
xmlNodePtr xmlAddChild(xmlNodePtr parent,xmlNodePtr cur);

/*创建新的节点属性对象*/
xmlAttrPtr xmlNewProp(xmlNodePtr node,const xmlChar *name, const xmlChar *value);

/*节点创建text文本信息*/
xmlNodePtr xmlNewText(xmlNodePtr node,const xmlChar *content);

/*得到xml节点值*/
xmlChar* xmlGetText(xmlNodePtr node);

/***************************************字符串操作函数**************************************/
int xmlStrlen(const xmlChar *str);

const xmlChar* xmlStrstr(const xmlChar *str, const xmlChar *val);

const xmlChar* xmlStrchr(const xmlChar *str, xmlChar val);

int xmlStrncasecmp(const xmlChar *str1, const xmlChar *str2, int len);

int xmlStrcasecmp(const xmlChar *str1, const xmlChar *str2);

int xmlStrncmp(const xmlChar *str1, const xmlChar *str2, int len);

int xmlStrcmp(const xmlChar *str1, const xmlChar *str2);




#ifdef __cplusplus
}
#endif


#endif

