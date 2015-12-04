/*************************************************************
* 文 件 名:  sp9976.c
* 功能描述： 贷款对帐单打印快捷查询
*
* 作    者:  
* 完成日期： 2003年8月14日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "prdt_ac_id_c.h"

int sp9976()
{
	struct prdt_ac_id_c g_prdt_ac_id;

	memset( &g_prdt_ac_id, 0x00, sizeof( struct prdt_ac_id_c ) );

	pub_base_sysinit();

	get_zd_data( "0310", g_pub_tx.ac_no );   		/* 贷款账号 */
	pub_base_old_acno( g_pub_tx.ac_no );           /****对老帐号处理****/

	g_reply_int = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, 
								 "ac_no='%s'", g_pub_tx.ac_no );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "该账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查找出错[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, " ac_id=%ld ", 
							  	  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "该账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查找错误[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_prdt_ac_id.prdt_no[0]!='3' )
	{
		sprintf( acErrMsg, "非贷款账号" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L114");
		goto ErrExit;
	}

	g_reply_int = Ln_mst_Sel( g_pub_tx.reply, &g_ln_mst, " ac_id=%ld ", 
							  g_mdm_ac_rel.ac_id );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "贷款账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if(g_reply_int)
	{
		sprintf( acErrMsg, "查找错误[%d]",g_reply_int );
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}
	/****不是同一机构不允许通过 20070528****/
	vtcp_log("[%s][%d] ln.opn_br_no = [%s]\n",__FILE__,__LINE__,g_ln_mst.opn_br_no);
	if(memcmp(g_ln_mst.opn_br_no,g_pub_tx.tx_br_no,5) != 0)
	{
		sprintf( acErrMsg, "此帐号不是本机构帐号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D124");
		goto ErrExit;
	}
	/****end****/
	sprintf( acErrMsg, "户名[%s]",g_ln_mst.name );
	WRITEMSG
	set_zd_data( "0250", g_ln_mst.name );
	set_zd_double( "0410", g_ln_mst.bal ); /****返回余额****/

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
