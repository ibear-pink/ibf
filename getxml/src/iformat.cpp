/**************************
 * 服务名称:iformat
 * 功能描述:公共文件
 * 程序版本:V1.0
 * 编码日期:2017/08/10
 * 编码人员:wangzhia
 **************************/

#include "iformat.h"

map<int,string> xml_files;

int iformat_init(char *path)
{
	DIR* dir;
	dirent* ptr;
	char fullFileName[1024] = {0};
	struct stat stStatBuf;
	dir = opendir(path);
	while ((ptr = readdir(dir)) != NULL)
	{
		char event_id[5+1] = {0};
		string xml;
		memset (fullFileName, 0, sizeof(fullFileName));
		//指定入口文件目录
		sprintf(fullFileName, "%s/%s", path, ptr->d_name);
		if (stat(fullFileName, &stStatBuf) == -1)
		{
			continue;
		}
		if(S_ISDIR(stStatBuf.st_mode))
		{
			if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			{
				continue;
			}
		}
		FILE *pf = fopen(fullFileName, "r");
		if (pf == NULL)
		{
			printf("%s未找到\n",fullFileName);
			return -2;
		}
		fseek(pf,0,SEEK_END);
		long lSize = ftell(pf);
		// 用完后需要将内存free掉
		char* text=(char*)malloc(lSize+1);
		if (text == NULL)
		{
			return -3;
		}
		rewind(pf);
		fread(text,sizeof(char),lSize,pf);
		text[lSize] = '\0';
		fclose(pf);
		pf = NULL;
		xml = string(text);
		free(text);
		text = NULL;
		strncpy(event_id, ptr->d_name, 5);
		xml_files.insert(pair<int,string>(atoi(event_id),xml));
	}
	closedir(dir);
	return 0;
}

int iformat_getxml(int event_id,map<string,string> &values,char *xml)
{
	string xml_temp;
	map<int,string>::iterator it = xml_files.find(event_id);
	if (it!=xml_files.end())
	{
		xml_temp = it->second;
		strcpy(xml,xml_temp.c_str());
		printf("find====[%d]%s\n",it->first,xml);
	}
	values.clear();
	return 0;
}

xmlXPathObjectPtr get_nodeset(xmlDocPtr doc, const xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	
	context = xmlXPathNewContext(doc);
	if (context == NULL)
	{
		printf("context is NULL\n");
		return NULL;
	}
	printf("xpath=%s\n",xpath);
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL)
	{
		printf("xmlXPathEvalExpression return NULL\n");
		return NULL;
	}
	
	if (xmlXPathNodeSetIsEmpty(result->nodesetval))
	{
		xmlXPathFreeObject(result);
		printf("nodeset is empty\n");
		return NULL;
	}
	
	return result;
}

int iformat_analysis(char *buff,vector<stXMLMode*> &values)
{
	int iRet = 0;
	/*生成xml变量*/
	xmlDocPtr xml = xmlNewDoc((const xmlChar *)"1.0");
	/*生成xml节点*/
	xmlNodePtr ptr_root = xmlNewNode((xmlNsPtr)"root",(const xmlChar *)"root");
	/*设置xml根节点*/
	xmlNodePtr root = xmlDocSetRootElement(xml, ptr_root);
	
	for(vector<stXMLMode*>::iterator it = values.begin();it!=values.end();it++)
	{
		stXMLMode *mode = *it;
		xmlNodePtr ptr_child = xmlNewNode((xmlNsPtr)(mode->key),(const xmlChar *)(mode->key));
		/*节点创建text文本信息*/
		xmlNewText(ptr_child,(const xmlChar *)(mode->value));
		/*xml中当前节点增加孩子节点*/
		if (strcasecmp(mode->parent_key,"root") == 0)
		{
			xmlAddChild(root,ptr_child);
		}
		else
		{
			char xpath[128+1] = {0};
			sprintf(xpath,"%s",mode->parent_key);
			get_nodeset(xml,(const xmlChar *)xpath);
		}
	}
	
	/*将内存xml变量，生成xml到buffer中，返回<0表示错，大于等于0表示生成xml的buff长度*/
	iRet = xmlSaveToBuff (xml,buff);

	return 0;
}




