/*************************************************
*2108的收费编号下载
* 文 件 名:    sp9769.c
* 功能描述：   
* 作    者:    jack
* 完成日期：   2003年01月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_chrg_rate_c.h"
	

sp9769()  
{ 		
	/** 数据初始化 **/
	struct com_chrg_rate_c       g_com_chrg_rate; 
	int ret=0;
	char filename[100],tmpcode[5];
	FILE *fp;
	int i=0;

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/***费率编号列表***/
	ret=Com_chrg_rate_Dec_Sel( g_pub_tx.reply,"type='9' order by chrg_rate_code");
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Com_chrg_rate_Fet_Sel( &g_com_chrg_rate , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( i==0 )
		{
			set_zd_data("1228",g_com_chrg_rate.chrg_rate_code);
		}
		else if(!strncmp(tmpcode,g_com_chrg_rate.chrg_rate_code,4))
			continue;

		pub_base_strpack(g_com_chrg_rate.chrg_rate_code);
		pub_base_strpack(g_com_chrg_rate.name);

		fprintf( fp,"1228|%-20s|%s|\n",g_com_chrg_rate.name, \
									g_com_chrg_rate.chrg_rate_code );
		i++;
		strcpy(tmpcode,g_com_chrg_rate.chrg_rate_code);
	}
	Com_chrg_rate_Clo_Sel();

	if( i )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
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
