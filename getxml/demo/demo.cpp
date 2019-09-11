
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "iformat.h"

using namespace std;
map<int,string> xml_files;

int main (int argc,char *argv[])
{
	int iRet=0;
	//≈‰÷√Œƒº˛¬∑æ∂
	char path[1024] = {0};
	char xml[1024]={0};
	vector<stXMLMode*> values;
	stXMLMode *mode = new stXMLMode();
	strcpy(mode->key,"PHONE_NO");
	strcpy(mode->value,"138xxxxxxxx");
	strcpy(mode->parent_key,"root");
	stXMLMode *mode1 = new stXMLMode();
	strcpy(mode->key,"LIST");
	strcpy(mode->value,"");
	strcpy(mode->parent_key,"root");
	stXMLMode *mode2 = new stXMLMode();
	strcpy(mode->key,"CUR_VALUE");
	strcpy(mode->value,"128");
	strcpy(mode->parent_key,"LIST");
	values.push_back(mode);
	values.push_back(mode1);
	values.push_back(mode2);
	/*strcpy(path,"/ocsapp/work/wangzhia/ibf/getxml/cfg");
	iformat_init(path);
	iformat_getxml(10001, values, xml);*/
	iformat_analysis(xml,values);
	printf("xml===\n%s\n",xml);
	//xml_files.clear();
	return 0;
}

