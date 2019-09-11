/**************************
* 服务名称:JavaPub
* 功能描述:公共文件
* 程序版本:V1.0
* 编码日期:2011/11/09
* 编码人员:
**************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "JavaPub.h"

#ifndef _DEBUG_
#define _DEBUG_
#endif 
char curSysDatetime[18];


static JavaVM *jvm;
static jobject obj_order;

int createJavaVM()
{
	JNIEnv *env;
	JavaVMOption options[6];
	JavaVMInitArgs vm_args;
	long status;
	jclass cls_order;
	
	char *p_env_abm = NULL;
	DIR *dir = NULL;
	dirent *ptr = NULL;
	char java_path[1024] = {0};
	char javalibs[10240] = {0};
	
	p_env_abm = getenv ("JAVA_LIB");
	if (NULL == p_env_abm)
	{
		printf ("FILE[%s]LINE[%d]ERR_MSG[取得JAVA_LIB环境变量失败\n", __FILE__, __LINE__);
		return -10;
	}
	sprintf (java_path, "%s", p_env_abm);
	//打开入口文件目录
	dir = opendir(java_path);
	sprintf(javalibs ,"-Djava.class.path=%s:%s/XmlSchema-1.4.7.jar:%s/acctMgrWeb.jar:%s/acctmgr_net.jar:%s/activation-1.1.jar",java_path,java_path,java_path,java_path,java_path);
	while ((ptr = readdir(dir)) != NULL)
	{
		sprintf(javalibs,"%s%s/%s:",javalibs,java_path, ptr->d_name);
	}
	closedir(dir);
	printf("%s\n",javalibs);
	options[0].optionString = "-Djava.compiler=NONE";
	/*测试类库*/
	//options[1].optionString = "-Djava.class.path=/acct1pkg/lib/jar/a.jar";
	/*正式类库*/
	options[1].optionString = javalibs;
	options[2].optionString = "-verbose:jni";
	options[3].optionString = "-Xms512M";
	options[4].optionString = "-Xmx512M";
	options[5].optionString = "-Xss512M";
	
	memset(&vm_args, 0, sizeof(vm_args));
	vm_args.version = JNI_VERSION_1_7;
	vm_args.nOptions = 6;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = JNI_TRUE;
	
	/*创建 JVM 处理完所有设置之后，现在就准备创建 JVM 了。
	先从调用方法开始 如果成功，则这个方法返回零，
	否则，如果无法创建 JVM，则返回JNI_ERR。*/
	status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	if (status != JNI_ERR)
	{
		jobject obj;
		/*测试类*/
		cls_order = env->FindClass("java/lang/String");
		/*正式类*/
		//cls_order = env->FindClass("com/sitech/acctmgrint/atom/busi/intface/PubInterCpp");
		if(cls_order != NULL)
		{
			jobject obj = env->AllocObject(cls_order);
			env->GetJavaVM(&jvm); //保存到全局变量中JVM
			obj_order = env->NewGlobalRef(obj);
			printf("test1,cls_order=%ld,env=%ld...\n",cls_order,env);
			printf("test1,jvm=%ld,obj_order=%ld...\n",jvm,obj_order);
			printf ("FILE[%s]LINE[%d]获取成功.\n", __FILE__, __LINE__);
			return 0;
		}
		else
		{
			printf ("FILE[%s]LINE[%d]获取java类PubInterCpp失败.\n", __FILE__, __LINE__);
			return -1;
		}
	}
	else
	{
		printf ("FILE[%s]LINE[%d]JNI_CreateJavaVM ERR.", __FILE__, __LINE__);
		return -1;
	}
}

int dealJavaInter(map<int,string> mResult,const char* serv_flag)
{
	char servName[100] = {0};
	char servIDs[1024] = {0};
	jboolean suc;
	long inIdNo = 0;
	jstring str[10];

	JNIEnv *env;
	jvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls_order = env->GetObjectClass(obj_order);
	
#ifdef _DEBUG_INFO_
	printf("test2,cls_order=%d,env=%d...\n",cls_order,env);
#endif
	//strcpy(servName,"CPP_opUserStatuInter");
	/*正式签名*/
	//strcpy(servIDs,"(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
	
	/*测试*/
	//测试签名
	//strcpy(servIDs,"(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z");
	//220220200051455214,15043271018,1,C,0,,,LOGIN_NO=bossA,CONTACT_ID=100005237590,OP_TIME=20150205164513,OP_CODE=fwkt,DB_A1
	for(map<int ,string>::iterator iter = mResult.begin();iter != mResult.end();iter++)
	{
		string s_value = iter->second;
		char result_value[1024] = {0};
		char result_des[1024] = {0};
		int len = 1024;
		strcpy(result_value , s_value.c_str());
		BytesToUtf8(result_value,result_des,&len);
		
		int num = iter->first;
		str[iter->first] = env->NewStringUTF(result_des);
		if (iter->first == 0)
		{
			inIdNo = atol(result_value);
		}
	}
	jmethodID mid_order = env->GetMethodID(cls_order, servName, servIDs);
#ifdef _DEBUG_INFO_
	printf("test1,mid_order=%d...\n",mid_order);
#endif
	if (mid_order == 0)
	{
		printf ("FILE[%s]LINE[%d]获取方法[%s]失败.", __FILE__, __LINE__,servName);
		return -1;
	}
	else
	{
		if (strcmp(serv_flag ,JAVA_INTERFACE_SERV_ORD) == 0)
		{
			suc = env->CallBooleanMethod(obj_order, mid_order, inIdNo,str[1],str[2],str[3],str[4],str[5],str[6],str[7],str[8]);
		}
		else if (strcmp(serv_flag ,JAVA_INTERFACE_ORDER) == 0)
		{
			suc = env->CallBooleanMethod(obj_order, mid_order,str[0],str[1],str[2],str[3],str[4],str[5],str[6]);
		}
		else if (strcmp(serv_flag ,JAVA_INTERFACE_B_ORDER) == 0)
		{
			suc = env->CallBooleanMethod(obj_order, mid_order, inIdNo,str[1],str[2],str[3],str[4],str[5]);
		}
	}
	
	if (suc == false)
	{
#ifdef _DEBUG_INFO_
	printf("发送java接口报错\n");
#endif
		if(env->ExceptionCheck())
		{
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
		jvm->DetachCurrentThread();
		return STOP_ERROR_NOR_STOP_JAVA_SEND_MSG_IS_ERROR;
	}
	jvm->DetachCurrentThread();
	
	return 0;
}

int BytesToUtf8(char* src, char* dst, int* nout) {
	size_t n_in = strlen(src);
	size_t n_out = *nout;
	iconv_t c = iconv_open("UTF-8", "GB2312");
	if (c == (iconv_t)-1) {
		cerr << strerror(errno) << endl;
		return NULL;
	}
	char* inbuf = new char [n_in + 1];
	if (!inbuf) {
		iconv_close(c);
		return NULL;
	}
	
	strcpy(inbuf, src);
	memset(dst, 0, n_out);
	char* in = inbuf;
	char* out = dst;
	if (iconv(c, &in, &n_in, &out, &n_out) == (size_t)-1) {
		cerr << strerror(errno) << endl;
		out = NULL;
	}
	else {
		n_out = strlen(dst);
		out = dst;
	}
	iconv_close(c);
	*nout = n_out;
	delete[] inbuf;
	return 0;
}

