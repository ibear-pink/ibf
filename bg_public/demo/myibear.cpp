//
//  myibear.cpp
//  myibear
//	自定义ibear子类
//  Created by wangzhia on 2018/9/12.
//  Copyright  2018/9/12 wangzhia. All rights reserved.
//
#include "myibear.h"

IMPLEMENT_CLASS(myibear)

myibear::myibear()
{
}

myibear::~myibear()
{
}

int myibear::getRouteDef(void *rules,int *db_no)
{
	*db_no = 1;
	printf("子类\n");
	return 0;
}
