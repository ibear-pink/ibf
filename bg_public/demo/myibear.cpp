//
//  myibear.cpp
//  myibear
//	�Զ���ibear����
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
	printf("����\n");
	return 0;
}
