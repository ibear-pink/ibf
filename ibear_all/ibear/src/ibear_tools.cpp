//
//  ibear_tools.cpp
//  ibf
//
//  Created by 王智 on 2019/7/24.
//  Copyright © 2019 王智. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ibear.h"

int main(int argc, char * argv[ ])
{
	char version_num[20] = {0};
	char version_date[20] = {0};
	if( (argc == 2) && (strcmp(argv[1], "-v") == 0) ) /*查看版本*/
	{
		IBEAR_ver(version_num, version_date);
		printf("\n\tVersion: %s, Release date: %s\n", version_num, version_date);
		printf("\tCompiled at %s %s\n", __DATE__, __TIME__);
		return 0;
	}
	
	printf( "\t   ibear_tools -v : to view version\n\n");
	return 0;
}
