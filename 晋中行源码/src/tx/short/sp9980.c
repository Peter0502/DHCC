/*************************************************
* 文 件 名:    sp9980.c
* 功能描述：   检查合同号是否存在
*
* 作    者:    mike
* 完成日期：   2003年08月14日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

sp9980()
{ 		
	int		ret;
	struct	protect_note_c pro_note;

	/** 数据初始化 **/
	ret = 0;
	memset( &pro_note, 0x00, sizeof( struct protect_note_c ));

	get_zd_data("0330",g_pub_tx.ac_no);

	ret = Protect_note_Sel(g_pub_tx.reply, &pro_note,"pact_no='%s' ",g_pub_tx.ac_no);
	if ( ret )
	{
		sprintf(acErrMsg,"ac_no to name erro [%s] ",g_pub_tx.ac_no,g_pub_tx.cif_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B177");
		goto ErrExit;
	}
	if ( pro_note.protect_sts[0]=='1' )
	{
		sprintf(acErrMsg,"帐户已销户pact_no=[%s] sts=[%s] ",g_pub_tx.ac_no,pro_note.protect_sts);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P245");
		goto ErrExit;
	}
	set_zd_data("0300",pro_note.pro_ac_no);
	set_zd_long("0280",pro_note.pro_cif_no);
	set_zd_data("0961",pro_note.pro_name);
	set_zd_data("0962",pro_note.pro_addr);
	set_zd_data("0310",pro_note.ben_ac_no);
	set_zd_long("0290",pro_note.ben_cif_no);
	set_zd_data("0963",pro_note.ben_name);
	set_zd_data("0964",pro_note.ben_addr);
	set_zd_data("0320",pro_note.bail_ac_no);
	set_zd_long("0440",pro_note.opn_date);
	set_zd_long("0450",pro_note.end_date);
	set_zd_data("0670",pro_note.protect_typ);
	set_zd_data("0220",pro_note.mort_typ);
	set_zd_long ("0480",pro_note.time_limt);
	set_zd_double("0390",pro_note.sum_amt);
	set_zd_double("0400",pro_note.amt);
	vtcp_log(" CHKKK OVER_RATE=[%f] [%f]",pro_note.over_rate,pro_note.ratio);
	set_zd_double("0840",pro_note.over_rate);
	set_zd_double("0410",pro_note.bail_amt);
	set_zd_double("0850",pro_note.ratio);
	set_zd_data("0370",pro_note.mort_ac_no);

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
