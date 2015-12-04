/*************************************************
* 文 件 名: spF323.c
* 功能描述：登记保函登记簿
*
* 作    者: jick
* 完成日期: 2004年08月13日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"
#define ERR_DEAL if(ret){sprintf(acErrMsg,"ERROR");WRITEMSG goto ErrExit;}

spF323()  
{ 	
	struct	protect_note_c pro_note;
	int	ret=0;
	char	ac_type[2];

	memset( &pro_note, 0x00, sizeof(struct protect_note_c));

	pub_base_sysinit();

	pro_note.trace_no=g_pub_tx.trace_no;
	pro_note.tx_date=g_pub_tx.tx_date;
	strcpy(pro_note.tx_br_no,g_pub_tx.tx_br_no);
	strcpy(pro_note.tel,g_pub_tx.tel);

	get_zd_data  ("0300", pro_note.pro_ac_no);
	get_zd_long  ("0280",&pro_note.pro_cif_no);
	get_zd_data  ("0310", pro_note.ben_ac_no);
	get_zd_long  ("0290",&pro_note.ben_cif_no);
	get_zd_data  ("0320", pro_note.bail_ac_no);
	get_zd_data  ("0330", pro_note.pact_no);
	get_zd_long  ("0440",&pro_note.opn_date);
	get_zd_long  ("0480",&pro_note.time_limt);
	get_zd_long  ("0450",&pro_note.end_date);
	get_zd_double("0390",&pro_note.sum_amt);
	get_zd_double("0400",&pro_note.amt);
	get_zd_double("0410",&pro_note.bail_amt);
	get_zd_double("0850",&pro_note.ratio);
	get_zd_data  ("0220", pro_note.mort_typ);
	get_zd_data  ("0370", pro_note.mort_ac_no);
	get_zd_data  ("0961", pro_note.pro_name);
	get_zd_data  ("0962", pro_note.pro_addr);
	get_zd_data  ("0963", pro_note.ben_name);
	get_zd_data  ("0964", pro_note.ben_addr);
	get_zd_double("0420",&pro_note.amends_amt);
vtcp_log("RATIO ratio = [%f] ratio=[%f][%s->%d]",pro_note.ratio,g_pub_tx.rate,__FILE__,__LINE__);

	/**  ***/
	if ( pro_note.amends_amt > pro_note.amt )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",pro_note.pro_ac_no);
		if( ret==100 )
	{
		sprintf( acErrMsg,"介质账号对照表中不存在此记录!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}
	else if( ret )
	{
		sprintf( acErrMsg,"查询介质帐号对照表异常!!erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}

	if(g_mdm_ac_rel.ac_seqn==ALL_SEQN)g_mdm_ac_rel.ac_seqn=1;

	/* 根据显示帐号和账号序号查询主文件 */
	ret=pub_base_disac_file(pro_note.pro_ac_no, g_mdm_ac_rel.ac_seqn, ac_type);
	if( ret )
	{
		sprintf( acErrMsg,"根据显示帐号和账号序号查询主文件erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
   	}
	}

/**
	tmp_bal=g_dd_mst.bal;
**/

	set_zd_double("102F",pro_note.bail_amt);
	set_zd_data("1021",pro_note.pro_ac_no);
	set_zd_int ("1022",1);
	set_zd_data("102Y","借 申请人帐户");
	set_zd_data("102J","01");
	set_zd_data("102K","2");					/*转帐*/
	set_zd_data("102O","0");					/*是否打印存折(有折无折标志):1是0否*/

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"登记保函处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"登记保函ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
