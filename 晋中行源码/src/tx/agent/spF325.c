/*************************************************
* 文 件 名: spF325.c
* 功能描述：保函到期保证金帐户处理
*
* 作    者: mike
* 完成日期: 2004年08月13日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

spF325()  
{ 	
	struct	protect_note_c pro_note;
	int	ret=0;

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
	get_zd_long   ("0480",&pro_note.time_limt);
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
	strcpy(g_pub_tx.ac_no,pro_note.pro_ac_no);
	g_pub_tx.cif_no=pro_note.pro_cif_no;
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	g_pub_tx.tx_amt1=pro_note.sum_amt;
	g_pub_tx.tx_amt2=pro_note.amt;
	g_pub_tx.tx_amt3=pro_note.bail_amt;
	g_pub_tx.tx_amt4=pro_note.amends_amt;
	g_pub_tx.rate=pro_note.ratio;

	strcpy(g_pub_tx.brf, "保函到期");
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"");
		**/
		goto ErrExit;
	}
	set_zd_data("1021",pro_note.bail_ac_no);
	set_zd_int ("1022",1);
	set_zd_data("102Y","借 保证金帐户");
	set_zd_data("102J","01");
	set_zd_data("102K","2");					/*转帐*/
	set_zd_data("102O","0");					/*是否打印存折(有折无折标志):1是0否*/
    set_zd_double("102F",pro_note.bail_amt);

	ret = sql_execute("UPDATE protect_note SET protect_sts='1' WHERE pact_no='%s'",pro_note.pact_no);
	if ( ret )
	{
		sprintf(acErrMsg,"UPDATE ERROR pact_no=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H020");
		goto ErrExit;
	}

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"保函到期处理OKExit !!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;

ErrExit:
    sprintf(acErrMsg,"保函到期ErrExit!!! [%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
