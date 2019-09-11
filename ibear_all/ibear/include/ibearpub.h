/**************************
 * ��������:rtStopPub
 * ��������:����ͷ�ļ�
 * ����汾:V1.0
 * ��������:2014/10/20 11:23:53
 * ������Ա:
 **************************/
#ifndef _IBEAR_PUB_FUNC_H_
#define _IBEAR_PUB_FUNC_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __GNUC__
#define GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#else
#define GCC_VERSION_AT_LEAST(x,y) 0
#endif

#if GCC_VERSION_AT_LEAST(3,1)
#define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#define attribute_deprecated __declspec(deprecated)
#else
#define attribute_deprecated
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <map>
#include <iconv.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <strings.h>
#include <regex.h>

#include "ibear_com_type.h"
#include "libxml/libxml.h"
#include "ibear_err_define.h"
#ifdef _PERFORMANCE_
#include "ilog.h"
#endif

using namespace std;

//sql������
extern char * ibear_getSQLParam(stSqlMap *sqlmap,map<string,void*> inParams,map<string,stSQLParam*> &bindParams,char *sql_out);
extern int ibear_SQLResultNum(char *sql);//���sql��������
extern int ibear_SQLResults(char *sql,char *tableName,vector<string> *results,char *result);
extern int ibear_SQLDataNum(char *sql,char *name);
extern int ibear_SQLRules(char *sql,char *rule,vector<string> *rules);//
extern int checkSQL(char *sql,char *newsql);//���sql�Ƿ�Ϸ�
extern int ibear_getRowNum(char *result,char *rowname);//����rowname��result�е�����

//ʱ�䴦����
extern long ibear_getUTime();

//�ַ���������
extern int xmlFileRead(char* filename,string *info);
extern char *xmlTrim(char *S);//ȡ�ո�
extern char *xml_strrep(char *pInput, char *pOutput, char *pSrc, char *pDst);
extern char * xml_deleteEmptyStr(int *dnums,int now_num,char *src,char *new_src,int num);// �滻�ַ����������ַ���Ϊָ���ַ���
extern char * isEmpty_strrep(char *src,char *new_src);//�滻�ַ����������ַ���Ϊָ���ַ���
extern int ibear_conversion(char *src,char *desc);//�ַ���ת��ΪΨһcode
extern char * ibear_stristr (const char * str1,const char * str2,const char * str3,char *result);//��ȡstr1��str2��str3֮����ַ���
extern char * ibear_stristr (const char * str1,const char * str2,char *result);//��ȡstr1��str2����β���ַ���
extern char * stristr (const char * str1,const char * str2);//���Դ�Сд��strstr
extern int ibear_splitVector(char *src ,char splitchar,char *descs[10]);//��src��splitchar��ֳ����飬���ظ���
extern int ibear_dealListType(stSqlMap *sqlmap,map<string,void*> inParams,stSQLParam *sqlParam,map<string,stSQLParam*> &bindParams,char *sql_out);
extern bool ibear_AllisNum(char *str);
extern bool ibear_IsDate(char *bematch);

//xml������
extern int Analysis_ConfXML(char *pSrc,stSqlMapConfig *sql_Conf);
extern int Analysis_ConfXML(char *pSrc,char *host_id,stSqlMapConfig *sql_Conf);
extern int Analysis_SqlXML(stSqlMapConfig *sql_Conf,map<string,stSqlMap*> *sqlconfs);
extern int xmlbuild_sql(char *tpl_content,char *out_content,map<string,stSQLParam*> *bindParams);
extern int init_XxmlFile(char *fileName,char *host_id,map<string,stSqlMap*> *sqlconfs,stSettings *settings);
extern int getSqlContent(map<string,stSqlMap*> *sqlconfs);
extern int xmlConvSpeChar(char *old_src,char *new_src,map<string,string> repedMap);
extern int replaceSql(char *src,map<string,stSQLParam*> bindParams,char *new_src);
extern int xml_SplitString (const char *buff, char *tStr1, char *tStr2);
extern int xml_TtoSp(const char *buff,char *tStr);

//������־��ʼ��
void initLog(char *logpath,char *host_id,bool perforModel,bool testModel);

//��������ڴ�
template<class D> extern void clearXMLList (vector<vector<D*> > &vList);
template<class D> extern void clearXMLList (vector<D*> &vList);
template<class K, class V> extern void clearXMLList (map<K, V*> &mList);
template<class K, class V> extern void clearXMLList (multimap<K, V*> &mList);
template<class K, class V> extern void clearXMLList (map<K, vector<V*> > &mList);

template<class D>
void clearXMLList (vector<vector<D*> > &vList)
{
	if (vList.size() == 0)
	{
		return;
	}
	
	typename vector<vector<D*> >::iterator iter;
	typename vector<D*>::iterator vter;
	for (iter = vList.begin(); iter != vList.end(); iter++)
	{
		vector<D*> nList = *iter;
		for (vter = nList.begin();vter != nList.end();vter++){
			D *pData = *vter;
			delete pData;
			pData = NULL;
		}
		nList.clear();
	}
	
	vList.clear ();
	
	return;
}

template<class D>
void clearXMLList (vector<D*> &vList)
{
	if (vList.size() == 0)
	{
		return;
	}
	
	typename vector<D*>::iterator iter;
	for (iter = vList.begin(); iter != vList.end(); iter++)
	{
		D *pData = *iter;
		delete pData;
		pData = NULL;
	}
	
	vList.clear ();
	
	return;
}

template<typename K, typename V>
void clearXMLList (map<K, V*> &mList)
{
	if (mList.size() == 0)
	{
		return;
	}
	
	typename map<K, V*>::iterator iter;
	for (iter = mList.begin(); iter != mList.end(); iter++)
	{
		V *pData = iter->second;
		delete pData;
		pData = NULL;
	}
	
	mList.clear ();
}

template<typename K, typename V>
void clearXMLList (multimap<K, V*> &mList)
{
	if (mList.size() == 0)
	{
		return;
	}
	
	typename multimap<K, V*>::iterator iter;
	for (iter = mList.begin(); iter != mList.end(); iter++)
	{
		V *pData = iter->second;
		delete pData;
		pData = NULL;
	}
	
	mList.clear ();
}

template<typename K, typename V>
void clearXMLList (map<K, vector<V*> > &mList)
{
	if (mList.size() == 0)
	{
		return;
	}
	
	vector<V*> vList;
	typename vector<V*>::iterator vIter;
	typename map<K, vector<V*> >::iterator iter;
	
	//�ڴ��ͷ�
	for (iter = mList.begin(); iter != mList.end(); iter++)
	{
		vList = iter->second;
		
		for (vIter = vList.begin(); vIter != vList.end(); vIter++)
		{
			V *pNode = *vIter;
			delete pNode;
			pNode = NULL;
		}
		
		vList.clear();
	}
	
	mList.clear();
}

#endif
