/*************************************************
* 文 件 名: sp9900.c
* 功能描述：支付业务处理状态选项明细快查
* 作    者: jane
* 完成日期：2003年7月5日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/

#define EXTERN
#include "public.h"

sp9900()  
{
	char filename[100],qrychoi[2];
	FILE *fp;

	get_zd_data("0670", qrychoi);
	
	pub_base_AllDwnFilName( filename );
	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/***选项列表***/
	if (!strcmp(qrychoi,"1"))
	{
		fprintf( fp,"0680|已清算|0|\n" );
		fprintf( fp,"0680|未回执|1|\n" );
		fprintf( fp,"0680|排队|2|\n" );
		fprintf( fp,"0680|滞留|3|\n" );
		fprintf( fp,"0680|已撤销|*|\n" );
	}
	else if (!strcmp(qrychoi,"2"))
	{
		fprintf( fp,"0680|已清算|0|\n" );
		fprintf( fp,"0680|挂帐|5|\n" );
		fprintf( fp,"0680|已退回|6|\n" );
		fprintf( fp,"0680|已提入|7|\n" );
	}

	fclose(fp);
	set_zd_data( DC_FILE_SND,"2" );
	
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
