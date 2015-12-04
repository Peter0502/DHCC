/*************************************************
* 文 件 名: spF321.c
* 功能描述：登记保函登记簿
*
* 作    者: rob
* 完成日期: 2004年08月13日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define EXTERN
#include "public.h"
#include "protect_note_c.h"

spF321()  
{ 	
	struct	protect_note_c pro_note;
	struct	protect_note_c pro_note1;
	int	ret=0;

	memset( &pro_note, 0x00, sizeof(struct protect_note_c));
	memset( &pro_note1, 0x00, sizeof(struct protect_note_c));
sprintf(acErrMsg,"保函登记开始 !" );
WRITEMSG
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
	get_zd_double("0840",&pro_note.over_rate);
	get_zd_data  ("0670", pro_note.protect_typ);
	vtcp_log(" ratio = [%f] ratio=[%f]",pro_note.ratio,g_pub_tx.rate);
	pro_note.protect_sts[0]='0';
	strcpy(g_pub_tx.ac_no,pro_note.pro_ac_no);
	g_pub_tx.cif_no=pro_note.pro_cif_no;
	strcpy(g_pub_tx.beg_note_no,pro_note.pact_no);
	g_pub_tx.tx_amt1=pro_note.sum_amt;
	g_pub_tx.tx_amt2=pro_note.amt;
	g_pub_tx.tx_amt3=pro_note.bail_amt;
	g_pub_tx.tx_amt4=pro_note.amends_amt;
	g_pub_tx.rate=pro_note.ratio;

	strcpy(g_pub_tx.brf, "开具保函");
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"登记交易流水错误!!=[%s][%d]",pro_note.pact_no,ret);
		WRITEMSG
		/**
		strcpy(g_pub_tx.reply,"");
		**/
		goto ErrExit;
	}

	/** 检查保证金帐号，金额是否正确 **/
	ret = Protect_note_Dec_Upd(g_pub_tx.reply,"pact_no='%s' ",pro_note.pact_no); 
	if ( ret )
	{
		sprintf(acErrMsg,"取保函登记薄错误 [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	ret = Protect_note_Fet_Upd(&pro_note1,g_pub_tx.reply);
	if ( ret==100 )	
	{
		sprintf(acErrMsg,"请先用6305交易存入保证金! [%d]",ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B352");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取保函登记薄错误 [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}

	if( pro_note1.protect_sts[0]!='A' )
	{
		sprintf(acErrMsg,"保函登记薄状态非存入保证金状态! [%s]",pro_note1.protect_sts );
		WRITEMSG
		strcpy(g_pub_tx.reply,"B349");
		goto ErrExit;
	}
	pub_base_strpack(pro_note1.bail_ac_no);
	if( strcmp(pro_note1.bail_ac_no,pro_note.bail_ac_no) )
	{
		sprintf( acErrMsg, "保证金帐号错误![%s][%s]",pro_note1.bail_ac_no,pro_note.bail_ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B258");
		goto ErrExit;
	}
	if( pub_base_CompDblVal(pro_note1.bail_amt,pro_note.bail_amt)!=0 )
	{
		sprintf( acErrMsg, "保证金金额错误![%lf][%lf]",pro_note1.bail_amt,pro_note.bail_amt);
		WRITEMSG
		strcpy(g_pub_tx.reply,"B330");
		goto ErrExit;
	}

	/** 计算下一结息日 **/
	ret = pub_base_deadlineM( pro_note.opn_date, 3 ,&pro_note.nex_date );
	if ( ret )
	{
		sprintf(acErrMsg,"net_date error [%d] [%d]",pro_note.opn_date,ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Protect_note_Upd_Upd(pro_note,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"修改保函登记薄错误![%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	Protect_note_Clo_Upd();

	/***
	ret = Protect_note_Ins( pro_note , g_pub_tx.reply );
	if ( ret )
	{
		sprintf(acErrMsg,"insert into protect_note error [%d]",ret );
		WRITEMSG
		goto ErrExit;
	}
	***/
	vtcp_log(" ratio = [%f] ratio=[%f]",pro_note.ratio,g_pub_tx.rate);

	get_zd_double("0430",&g_pub_tx.tx_amt1);			/**手续费**/
	g_pub_tx.tx_amt2 = pro_note.amt*pro_note.ratio/100;/** 担保费 = 保函金额*担保费比例 **/
	set_zd_double("1001",0.00);
	set_zd_data("1011",pro_note.bail_ac_no);
	set_zd_int ("1012",1);
	set_zd_data("1016","贷 担保保证金帐户");
	set_zd_data("101A","01");
	set_zd_data("101B","2");				/*转帐*/
    set_zd_double("1013",pro_note.bail_amt);


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
