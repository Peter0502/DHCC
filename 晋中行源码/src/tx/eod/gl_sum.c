#include<stdio.h>
#include<stdlib.h>
#include<string.h>

main(int argc, char *argv[])
{
	int ret=0;
	if(!strcmp(argv[1],"gl_mst"))
	{
			printf("[%s][%d]�����������ļ�����===[%s]\n",__FILE__,__LINE__,argv[1]);
			ret=pub_eod_sum1();
			if( ret ) 
			{
				return 1;
			}	
	}
	if(!strcmp(argv[1],"gl_sub"))
	{
			printf("[%s][%d]����֧���������ļ�����===[%s]\n",__FILE__,__LINE__,argv[1]);
	    ret=pub_eod_sum2();
			if( ret ) 
			{
				return 1;
			}	
	}
	return 0;	
}
