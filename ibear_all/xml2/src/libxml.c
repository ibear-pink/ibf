#include "libxml/libxml.h"
#include <string.h>


static void xmlFreeNodestVal(xmlNodeSetPtr nodesetval);
static void xmlFreeNodeSet(xmlNodeSetPtr nodesetval);
static int xmlGetXpathMulName(xmlChar *str,xmlNodePtr cur,xmlXPathObjectPtr res);
static const xmlChar casemap[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
0x78,0x79,0x7A,0x7B,0x5C,0x5D,0x5E,0x5F,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};


int xmlKeepBlanksDefault(int val) 
{    
  return(val);
}

void xmlFreeDoc(xmlDocPtr cur) 
{
	cur->Release(cur);
	free(cur);
}

xmlDocPtr xmlParseFile(const char *filename) 
{
	int rc;
	xmlDocPtr	xml=MallocQXml();
	if (xml==NULL){return NULL;}
	rc = xml->ParseFile(xml,(char *)filename);
	if (rc!=XML_OK)
	{
		printf("Parser Error in [%d]!\n",rc);
		xmlFreeDoc(xml);
		return NULL;
	}
  return xml;
}

xmlNodePtr xmlDocGetRootElement(xmlDocPtr doc) 
{
	if (doc == NULL) return(NULL);
	return doc->ToRoot(doc);
}


int xmlStrcmp(const xmlChar *str1, const xmlChar *str2) 
{
    register int tmp;

    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
        tmp = *str1++ - *str2;
        if (tmp != 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}
int xmlStrncmp(const xmlChar *str1, const xmlChar *str2, int len) 
{
    register int tmp;

    if (len <= 0) return(0);
    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
#ifdef __GNUC__
    tmp = strncmp((const char *)str1, (const char *)str2, len);
    return tmp;
#else
    do {
        tmp = *str1++ - *str2;
        if (tmp != 0 || --len == 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
#endif
}

int xmlStrcasecmp(const xmlChar *str1, const xmlChar *str2) 
{
    register int tmp;

    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
        tmp = casemap[*str1++] - casemap[*str2];
        if (tmp != 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

int xmlStrncasecmp(const xmlChar *str1, const xmlChar *str2, int len) 
{
    register int tmp;

    if (len <= 0) return(0);
    if (str1 == str2) return(0);
    if (str1 == NULL) return(-1);
    if (str2 == NULL) return(1);
    do {
        tmp = casemap[*str1++] - casemap[*str2];
        if (tmp != 0 || --len == 0) return(tmp);
    } while (*str2++ != 0);
    return 0;
}

const xmlChar * xmlStrchr(const xmlChar *str, xmlChar val) 
{
    if (str == NULL) return(NULL);
    while (*str != 0) { /* non input consuming */
        if (*str == val) return((xmlChar *) str);
        str++;
    }
    return(NULL);
}

/**
 * xmlStrstr:
 * @str:  the xmlChar * array (haystack)
 * @val:  the xmlChar to search (needle)
 *
 * a strstr for xmlChar's
 *
 * Returns the xmlChar * for the first occurrence or NULL.
 */

const xmlChar* xmlStrstr(const xmlChar *str, const xmlChar *val) 
{
    int n;
    
    if (str == NULL) return(NULL);
    if (val == NULL) return(NULL);
    n = xmlStrlen(val);

    if (n == 0) return(str);
    while (*str != 0) { /* non input consuming */
        if (*str == *val) {
            if (!xmlStrncmp(str, val, n)) return((const xmlChar *) str);
        }
        str++;
    }
    return(NULL);
}

int xmlStrlen(const xmlChar *str) 
{
    int len = 0;

    if (str == NULL) return(0);
    while (*str != 0) { /* non input consuming */
        str++;
        len++;
    }
    return(len);
}

int xmlIsBlankNode(xmlNodePtr node) 
{
    return(0);
}

xmlChar* xmlGetProp(xmlNodePtr node, const xmlChar *name) 
{
	if(node==NULL){return NULL;}
	return (xmlChar *)node->GetAttrib(node,(char *)name);
}

xmlAttrPtr xmlSetProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value)
{
	/*if(node==NULL){return NULL;} */
	if(node==NULL){return FALSE;}
	return node->SetAttrib(node,(char *)name,(char *)value);
}

/*xmlChar *xmlNodeListGetString(xmlDocPtr doc, xmlNodePtr list, int inLine)
{
	if(list==NULL){return NULL;}
	return (xmlChar *)list->m_szValue.m_dataBuff;
}*/

void xmlCleanupParser(void) 
{
	return;
}

int xmlSaveFile(const char *filename, xmlDocPtr cur)
{
	return ( (cur->SaveToFile(cur,(char *)filename)!=XML_OK)?-1:0);
}

void xmlFree(void *ptr)
{
	return ;
}


/**
 * xmlXPathNewContext:
 * @doc:  the XML document
 *
 * Create a new xmlXPathContext
 *
 * Returns the xmlXPathContext just allocated. The caller will need to free it.
 */
xmlXPathContextPtr xmlXPathNewContext(xmlDocPtr doc) 
{
    xmlXPathContextPtr ret;

    ret = (xmlXPathContextPtr) malloc(sizeof(xmlXPathContext));
    if (ret == NULL) 
    {
      printf("creating context\n");
			return(NULL);
    }
    memset(ret, 0, (size_t)sizeof(xmlXPathContext));
    ret->doc = doc;
    ret->node = NULL;
    return(ret);
}


/**
 * xmlXPathFreeContext:
 * @ctxt:  the context to free
 *
 * Free up an xmlXPathContext
 */
void xmlXPathFreeContext(xmlXPathContextPtr ctxt) 
{
    if (ctxt == NULL) return;
    free(ctxt);
    ctxt=NULL;
}


/**
 * xmlXPathEvalExpression:
 * @str:  the XPath expression
 * @ctxt:  the XPath context
 *
 * Evaluate the XPath expression in the given context.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 *
xmlXPathObjectPtr xmlXPathEvalExpression(const xmlChar *str, xmlXPathContextPtr ctxt) 
{
    xmlXPathObjectPtr res=NULL;
    char *p=NULL;
    char *buff=NULL;
	char *tmp=NULL;
    xmlNodePtr cur=NULL;
    cur = xmlDocGetRootElement(ctxt->doc);
    buff=(char *)malloc(strlen((char *)str)+1);
    memset(buff,0,strlen((char *)str)+1);
    memcpy(buff,(char *)str,strlen((char *)str));
#ifdef WIN32
	p=strtok_s(buff,"/",&tmp);
#else
  	p=strtok_r(buff,"/",&tmp);
#endif
    while(cur!=NULL)
    {
      if (xmlStrncasecmp(cur->name,(const xmlChar *)p,xmlStrlen(cur->name))==0)
      {
					#ifdef WIN32
						p=strtok_s(NULL,"/",&tmp);
					#else
  						p=strtok_r(NULL,"/",&tmp);
					#endif				
					if (p==NULL)
					{
						break;
					}
					cur=cur->xmlChildrenNode;
					continue;
      }
      cur=cur->next;
    }
		if (cur!=NULL)
		{
			res=(xmlXPathObjectPtr)malloc(sizeof(xmlXPathObject));
			memset(res,0,sizeof(xmlXPathObject));
			res->nodesetval=(xmlNodeSetPtr)malloc(sizeof(xmlNodeSet));
			memset(res->nodesetval,0,sizeof(xmlNodeSet));
			res->nodesetval->nodeMax=LIBXML_LEVEL_DEFAULT;
			res->nodesetval->nodeNr=0;
			res->nodesetval->nodeTab=(xmlNodePtr *)malloc(sizeof(xmlNodePtr)*LIBXML_LEVEL_DEFAULT);
			res->nodesetval->nodeTab[0]=cur;
			++res->nodesetval->nodeNr;
		}
		free(buff);
    return(res);
}
*/
static int xmlGetXpathMulName(xmlChar *str,xmlNodePtr cur,xmlXPathObjectPtr res)
{
	char *start=(char *)str;
    char *end=NULL;
    char nodeValue[128+1];
    xmlNodePtr cur1=cur;
    int flag=0;
	end=strstr(start,"/");
	if(end==NULL)
	{
		flag=1;
		end=(char *)str+strlen((char *)str);
	}
	memset(nodeValue,0,sizeof(nodeValue));
	memcpy(nodeValue,start,end-start);
	while(cur1!=NULL)
	{	    	
		if (xmlStrncasecmp(cur1->name,(const xmlChar *)nodeValue,xmlStrlen(cur1->name))==0)
		{
			if(flag==1)
			{
				if(res->nodesetval->nodeNr==LIBXML_LEVEL_DEFAULT)
				{
					printf("the get nodes gt %d.\n",LIBXML_LEVEL_DEFAULT);
					return -1;
				}
				res->nodesetval->nodeTab[res->nodesetval->nodeNr]=cur1;
				++res->nodesetval->nodeNr;
			}
			else
			{
				if(xmlGetXpathMulName((xmlChar *)end+1,cur1->xmlChildrenNode,res)!=0)
				{
					return -1;
				}
			}
		}
		cur1=cur1->next;
		if(flag!=1)
		{
			start=end+1;
		}
	}
    return 0;
}
/**
 * xmlXPathEvalExpression:
 * @str:  the XPath expression
 * @ctxt:  the XPath context
 *
 * Evaluate the XPath expression in the given context.
 *
 * Returns the xmlXPathObjectPtr resulting from the evaluation or NULL.
 *         the caller has to free the object.
 */
xmlXPathObjectPtr xmlXPathEvalExpression(const xmlChar *str, xmlXPathContextPtr ctxt) 
{
    xmlXPathObjectPtr res=NULL;
    xmlNodePtr cur=NULL;
    xmlChar *p=(xmlChar *)str;
    cur = xmlDocGetRootElement(ctxt->doc);
	if(cur!=NULL)
	{
		res=(xmlXPathObjectPtr)malloc(sizeof(xmlXPathObject));
		if(res==NULL)return NULL;
		memset(res,0,sizeof(xmlXPathObject));
		res->nodesetval=(xmlNodeSetPtr)malloc(sizeof(xmlNodeSet));
		memset(res->nodesetval,0,sizeof(xmlNodeSet));
		res->nodesetval->nodeMax=LIBXML_LEVEL_DEFAULT;
		res->nodesetval->nodeNr=0;
		res->nodesetval->nodeTab=(xmlNodePtr *)malloc(sizeof(xmlNodePtr)*LIBXML_LEVEL_DEFAULT);
	}
	if(str[0]=='/')
	{
		p=(xmlChar *)str+1;
	}
    if(xmlGetXpathMulName(p,cur,res)!=0)
    {
	    ;
    }
    return res;
}

static void xmlFreeNodeSet(xmlNodeSetPtr nodesetval)
{
	free(nodesetval->nodeTab);
	nodesetval->nodeTab=NULL;
	nodesetval->nodeMax=0;
	nodesetval->nodeNr=0;
}
static void xmlFreeNodestVal(xmlNodeSetPtr nodesetval)
{
	xmlFreeNodeSet(nodesetval);
}
/**
 * xmlXPathFreeObject:
 * @obj:  the object to free
 *
 * Free up an xmlXPathObjectPtr object.
 */
void xmlXPathFreeObject(xmlXPathObjectPtr obj) 
{
	if(obj == NULL)
	{
		return;
	}

	if(obj->nodesetval!=NULL)
	{
		xmlFreeNodestVal(obj->nodesetval);
		free(obj->nodesetval);
		obj->nodesetval=NULL;
	}

	free(obj);
	obj=NULL;
}


/**
 * xmlNewDoc:
 * @version:  xmlChar string giving the version of XML "1.0"
 *
 * Creates a new XML document
 *
 * Returns a new document
 */
xmlDocPtr xmlNewDoc(const xmlChar *version) 
{
	char szHead[128];
	int rc;
	xmlDocPtr	xml=MallocQXml();
	if (xml==NULL){return NULL;}
	memset(szHead,0,sizeof(szHead));
	#ifdef WIN32
		sprintf_s(szHead,128,"<?xml version=\"%s\" encoding=\"utf-8\" ?><root></root>",version);
	#else
  		sprintf(szHead,"<?xml version=\"%s\" encoding=\"utf-8\" ?><root></root>",version);
	#endif
	rc = xml->ParseBuff(xml,(char *)szHead);
	if (rc!=XML_OK)
	{
		printf("Parser Error in [%d]!\n",rc);
		xmlFreeDoc(xml);
		return NULL;
	}
 	return xml;		
}

/**
 * xmlNewNode:
 * @ns:  namespace if any
 * @name:  the node name
 *
 * Creation of a new node element. @ns is optional (NULL).
 *
 * Returns a pointer to the new node object. Uses xmlStrdup() to make
 * copy of @name.
 */
xmlNodePtr xmlNewNode(xmlNsPtr ns, const xmlChar *name) 
{
	xmlNodePtr node= MallocQXmlNode();
	if(node==NULL)return NULL;
	node->SetNodeName(node,(char *)name);
	return node;
}

/**
 * xmlDocSetRootElement:
 * @doc:  the document
 * @root:  the new document root element
 *
 * Set the root element of the document (doc->children is a list
 * containing possibly comments, PIs, etc ...).
 *
 * Returns the old root element if any was found
 */
xmlNodePtr xmlDocSetRootElement(xmlDocPtr doc, xmlNodePtr root) 
{
	doc->m_nodeRoot->Release(doc->m_nodeRoot);
	free(doc->m_nodeRoot);
	doc->m_nodeRoot=root;
	return doc->m_nodeRoot;
}

/**
 * xmlAddChild:
 * @parent:  the parent node
 * @cur:  the child node
 *
 * Add a new node to @parent, at the end of the child (or property) list
 * merging adjacent TEXT nodes (in which case @cur is freed)
 * If the new node is ATTRIBUTE, it is added into properties instead of children.
 * If there is an attribute with equal name, it is first destroyed. 
 *
 * Returns the child or NULL in case of error.
 */
xmlNodePtr xmlAddChild(xmlNodePtr parent, xmlNodePtr cur) 
{	
	return ((parent->AddNodeToChild(parent,cur))?cur:NULL);
}

/**
 * xmlNewProp:
 * @node:  the holding node
 * @name:  the name of the attribute
 * @value:  the value of the attribute
 *
 * Create a new property carried by a node.
 * Returns a pointer to the attribute
 */
xmlAttrPtr xmlNewProp(xmlNodePtr node, const xmlChar *name, const xmlChar *value) 
{
	return node->SetAttrib(node,(char *)name,(char *)value);
}

/**
 * xmlNewText:
 * @content:  the text content
 *
 * Creation of a new text node.
 * Returns a pointer to the new node object.
 */
xmlNodePtr xmlNewText(xmlNodePtr node, const xmlChar *content) 
{
	node->SetNodeValue(node,(char *)content);
	return node;
}


/*返回<0表示错，大于等于0表示生成xml的buff长度*/
int xmlSaveToBuff (xmlDocPtr cur,char *buff)
{
   QString str = NewQString();
   QXmlNode *node = NULL;
   char *ret=NULL;
   int length = 0;
   node = cur->ToRoot(cur);
   
   cur->GetDeclareData(cur,&str);
   while(node)
   {   	  
      node->GetNodeData(node,&str,0);
      node = node->ToNextNode(node,NULL);
   }
   
   ret = str.m_dataBuff;

   while(*ret)
   {
      (*buff)=(*ret);
      ret++;
      buff++;
      length++;
   }
   (*buff)='\0';
   str.Release(&str);
   if(cur->m_nErrorCode!=XML_OK)
   {
		return -1;
   }
   return length;
}


xmlDocPtr xmlParseMemory(const char *buffer, int size)
{
	int rc;
	xmlDocPtr	xml=MallocQXml();
	if (xml==NULL){return NULL;}
	rc = xml->ParseBuff(xml,(char *)buffer);
	if (rc!=XML_OK)
	{
		printf("Parser Error in [%d]!\n",rc);
		xmlFreeDoc(xml);
		return NULL;
	}
	return xml;
}

xmlChar* xmlGetText(xmlNodePtr node)
{
	if(node==NULL){return NULL;}
	return (xmlChar *)node->value;
}


