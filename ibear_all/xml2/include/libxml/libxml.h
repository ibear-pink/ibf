
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

/******************************XPathʹ�ú���************************************************/
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


/*xml��Կո����ã��˺���Ϊ�պ����ɲ�ʵ�֣�Ϊͳһxml�ӿ�ʹ��*/
int xmlKeepBlanksDefault(int val);

/*�ͷ�xml����*/
void xmlFreeDoc(xmlDocPtr cur);

/*����xml�ļ��������ڴ�xml����*/
xmlDocPtr xmlParseFile(const char *filename);

/*�õ�xml���ڵ�*/
xmlNodePtr xmlDocGetRootElement(xmlDocPtr doc);

/*�Ƿ��ǿսڵ㣬�˺���Ϊ�պ����ɲ�ʵ�֣�Ϊͳһxml�ӿ�ʹ��*/
int xmlIsBlankNode(xmlNodePtr node);

/*�õ��ڵ��Ӧ������ֵ*/
xmlChar* xmlGetProp(xmlNodePtr node, const xmlChar *name);

/*���ýڵ��Ӧ������ֵ*/
xmlAttrPtr xmlSetProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value);

/*���ڴ�xml����������xml�ļ�*/
int xmlSaveFile(const char *filename, xmlDocPtr cur);

/*�����ͷţ��˺���Ϊ�պ����ɲ�ʵ�֣�Ϊͳһxml�ӿ�ʹ��*/
void xmlFree(void *ptr);


/*���ڴ�xml����������xml��buffer�У�����<0��ʾ�����ڵ���0��ʾ����xml��buff����*/
int xmlSaveToBuff (xmlDocPtr cur,char *buff);

/*����buffer�е�xml�������ڴ�xml����*/
xmlDocPtr xmlParseMemory(const char *buffer, int size);

/*�ͷ�ȫ��xml�������˺���Ϊ�պ����ɲ�ʵ�֣�Ϊͳһxml�ӿ�ʹ��*/
void xmlCleanupParser(void);

/*����xml����*/
xmlDocPtr xmlNewDoc(const xmlChar *version);

/*����xml�ڵ�*/
xmlNodePtr xmlNewNode(xmlNsPtr ns,const xmlChar *name);

/*����xml���ڵ�*/
xmlNodePtr xmlDocSetRootElement(xmlDocPtr doc, xmlNodePtr root);

/*xml�е�ǰ�ڵ����Ӻ��ӽڵ�*/
xmlNodePtr xmlAddChild(xmlNodePtr parent,xmlNodePtr cur);

/*�����µĽڵ����Զ���*/
xmlAttrPtr xmlNewProp(xmlNodePtr node,const xmlChar *name, const xmlChar *value);

/*�ڵ㴴��text�ı���Ϣ*/
xmlNodePtr xmlNewText(xmlNodePtr node,const xmlChar *content);

/*�õ�xml�ڵ�ֵ*/
xmlChar* xmlGetText(xmlNodePtr node);

/***************************************�ַ�����������**************************************/
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

