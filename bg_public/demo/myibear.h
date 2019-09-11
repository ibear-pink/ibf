//
//  myibear.h
//  myibear
//	自定义ibear子类
//  Created by wangzhia on 2018/9/12.
//  Copyright  2018/9/12 wangzhia. All rights reserved.
//

#ifndef MY_IBEAR_H
#define MY_IBEAR_H

#include <stdio.h>
#include <sstream>
#include <vector>
#include "DynBase.h"
#include "ibear.h"

typedef struct
{
	char id_type[1+1];
	long begin_id;
	long end_id;
	char part_id[5+1];
}stRouteInfo;

using namespace std;

class myibear : public ibear,public Object
{
	DECLARE_CLASS(myibear)
public:
	myibear();
	~myibear();
	int getRouteDef(void *rules,int *db_no);
		
};

#endif
/* MY_IBEAR_H */

