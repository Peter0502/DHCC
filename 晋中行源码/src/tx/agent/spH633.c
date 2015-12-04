/*************************************************
* 文 件 名:  spH633.c
* 功能描述： 导入代收付记录查询信息
*
* 作    者:  ligl
* 完成日期： 2007-1-19 9:24
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
#include "pay_agent_c.h"

int spH633()
{
		struct pay_agent_c sPay_agent;	/*代收付记录信息处理表*/
		int ret = 0;
		char cBr_no[6];
  	char cUnit_no[5];
  	long lTx_date=0;
		int ttlnum=0;			/**读取的总条数**/
		FILE *fp;
		char filename[100];
		char f_id_type[21];
		char resultname[7];
		memset(f_id_type,0x00,sizeof(f_id_type));
		memset(resultname,0x00,sizeof(resultname));
		memset( cUnit_no,0x0,sizeof(cUnit_no));
		memset(&sPay_agent,0x00,sizeof(struct pay_agent_c));
		memset( cBr_no,0x0,sizeof(cBr_no));
		pub_base_sysinit();
		get_zd_data( "0920",cUnit_no);
  /**组成查询条件**/
	ret=Pay_agent_Dec_Sel(g_pub_tx.reply,"unit_no='%s' and tx_date=%ld",cUnit_no,g_pub_tx.tx_date);
	ERR_DEAL
  ttlnum=0;
  while(1)
	{
		memset(&sPay_agent,0x00,sizeof(struct pay_agent_c));
		ret=Pay_agent_Fet_Sel(&sPay_agent, g_pub_tx.reply );
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

			/**显示列标题**/
			fprintf( fp,"~单位编号|个人编号|客户姓名|代理账号|交易金额|证件类型|证件号码|处理状态|\n" );
		}
		memset(f_id_type,0x00,sizeof(f_id_type));
		memset(resultname,0x00,sizeof(resultname));
    pub_base_dic_show_str(f_id_type,"id_type",sPay_agent.id_type);
    if(sPay_agent.result[0]=='0')
    {
      memcpy(resultname,"已处理",6);
    }
    else
    {
      memcpy(resultname,"未处理",6);
    }
		fprintf( fp, "%s|%s|%s|%s|%.2lf|%s|%s|%s|\n",sPay_agent.unit_no,sPay_agent.item_no,sPay_agent.name,sPay_agent.ac_no,sPay_agent.tram,f_id_type,sPay_agent.id_no,resultname);
		ttlnum++;
	}
	ret=Pay_agent_Clo_Sel();
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
