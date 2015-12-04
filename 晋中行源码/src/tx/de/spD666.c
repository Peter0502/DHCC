/***************************************************************
* 文 件 名:     spD666.c
* 功能描述：	测试新加交易
* 作    者:     JYWe
* 完成日期：    2006年7月17日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD666()
{
	char testch[21],str1[21];
	int i,j;
	char tmpname[100];
	FILE *fp;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	
	memset(testch,0,sizeof(testch));
	memset(str1,0,sizeof(str1));

	get_zd_data("0250",testch);

	j=strlen(testch);
	for(i=0;i<j;i++){
		str1[j-i-1]=testch[i];	
	}	
	set_zd_data("0260",str1);
	
/***
    pub_base_AllDwnFilName( tmpname );
	fp=fopen(tmpname,"a");
	if( fp==NULL )
	{
		sprintf(acErrMsg,"打开文件[%s]错",tmpname);
		WRITEMSG
		strcpy( g_pub_tx.reply,"P157" );
		goto ErrExit;
	}
	fprintf(fp,"TEST123|456|789|abc|defg|\n");

	fclose(fp);

	set_zd_data(DC_FILE_SND,"1");
****/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"测试新加交易成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"测试新加交易失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
