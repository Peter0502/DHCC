/**************************************************************
* 文 件 名:  spH632.c
* 功能描述： 个人代理编号信息查询
*
* 作    者:  ligl
* 完成日期： 2004年5月26日
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "pay_agent_bk_c.h"
int spH632()
{
		struct pay_agent_bk_c sPay_agent_bk;	/*单位信息表*/
		int ret = 0;
		long lTx_date=0;
  	char cUnit_no[5];
  	char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		double amt=0.0;
		FILE *fp;
		char filename[100];
	memset( cUnit_no,0x0,sizeof(cUnit_no));
	memset( &sPay_agent_bk,0x0,sizeof(struct pay_agent_bk_c));
	pub_base_sysinit();
	get_zd_data( "0920", cUnit_no);
	get_zd_long("0440",&lTx_date);
	ret=Pay_agent_bk_Dec_Sel(g_pub_tx.reply,"unit_no='%s' and tx_date=%ld order by to_number(item_no)",cUnit_no,lTx_date);
	ERR_DEAL
  ttlnum=0;
  while(1)
	{
		memset( &sPay_agent_bk,0x0,sizeof(struct pay_agent_bk_c));
		ret=Pay_agent_bk_Fet_Sel(&sPay_agent_bk, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL
		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			
		}
    vtcp_log("aaaaaaaaaaaaaa[%s]\n",sPay_agent_bk.cnt);
		fprintf( fp, "%s|%s|%.2lf|%s|%s|%s|%s|%s|\n",sPay_agent_bk.item_no,sPay_agent_bk.name,sPay_agent_bk.tram,sPay_agent_bk.ac_no,sPay_agent_bk.unit_no,sPay_agent_bk.id_type,sPay_agent_bk.id_no,sPay_agent_bk.cnt);
		ttlnum++;
	}
	ret=Ag_peritemmap_Clo_Sel();
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}
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
