/*************************************************
* 文 件 名: spF326.c
* 功能描述：保函到期申请人存款帐户处理
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
#include "mo_bail_rate_c.h"

spF326()  
{ 	
	struct	protect_note_c 		pro_note;
	struct	mo_bail_rate_c		sMoBailRate;	/* 保证金利率登记薄 */
	struct	mo_bail_rate_c		sMoBailRate1;	/* 保证金利率登记薄 */
	int		ret=0;
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	HQdealval=0.00;
	char	intst_flag[2];

	memset( &pro_note, 0x00, sizeof(struct protect_note_c));
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );

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
	get_zd_data  ("0670", pro_note.protect_typ);

	/**---- hao ---- 20051113 ----- 增加担保保证金计息 ---**/
	strcpy(intst_flag,"0");
	if( pub_base_CompDblVal(pro_note.bail_amt,0.00)>0 )
	{
	   ret=Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1'",pro_note.pact_no);
   		if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
		if( ret==100 )	
		{
			g_pub_intst.dealval=0.00;
			g_pub_intst.tax_intst=0.00;					/** 应税利息 **/
			g_pub_intst.factval=0.00;					/** 实付利息 **/
			g_pub_intst.val=0.00;						/** 利息税 **/
			g_pub_intst.keepval=0.00;					/** 保值利息 **/
			set_zd_data("0153","52101");                /** 保证金利息 **/
			set_zd_double("102P",g_pub_intst.dealval);
		}
   		else if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,pro_note.pact_no);
        	WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
        	goto ErrExit;
   		}
		else
		{
			strcpy(intst_flag,"1");
			memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );
			ret = pub_base_getllz(HQLLBH,"01",g_pub_tx.tx_date,&HQ_rate);
			if( ret )
			{
				sprintf( acErrMsg, "取利率错误!" );
				WRITEMSG
				sprintf( g_pub_tx.reply, "O195" );
				goto ErrExit;	
			}

			/** 判断是否到期 **/
			if( g_pub_tx.tx_date<sMoBailRate.end_date ) 	/** 提前支取 **/
			{
				/** 计算天数 **/
				if( pub_base_CalDays(sMoBailRate.beg_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
				day=0;
			}
			else
			{
				/** 计算天数 **/
				if( pub_base_CalDays(sMoBailRate.beg_date,sMoBailRate.end_date,"0",&day) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
				if( pub_base_CalDays(sMoBailRate.end_date,g_pub_tx.tx_date,"0",&HQday) )
   				{
   					sprintf( acErrMsg, "计算天数错误!");
       				WRITEMSG
       				goto ErrExit;
   				}
   			}

			sprintf(acErrMsg,"HAO --- look --- HQday=[%d],day=[%d]",HQday,day);
			/** 计算利息 **/
			if( pub_base_CompDblVal(pro_note.bail_amt,sMoBailRate.amt) > 0 )
			{
       			HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=0.00;
			}
			else
			{
       			HQdealval = HQ_rate * pro_note.bail_amt * HQday / L360 / L100;
       			g_pub_intst.dealval = sMoBailRate.ic_rate * pro_note.bail_amt * day / L360 / L100;
				g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
				sMoBailRate1.amt=sMoBailRate.amt-pro_note.bail_amt;
				sMoBailRate.amt=pro_note.bail_amt;
			}
			g_pub_intst.dealval = pub_base_PubDround_1(g_pub_intst.dealval);
			if ( pub_base_CompDblVal(g_pub_intst.dealval)<0 )
			{
				sprintf(acErrMsg,"计算利息错误错误!");
				WRITEMSG
				strcpy(g_pub_tx.reply,"D158");
				goto ErrExit;
			}

			/** 更新保证金计息金额 **/
			sMoBailRate.intst_date=g_pub_tx.tx_date;
			sMoBailRate.end_trace_no=g_pub_tx.trace_no;
			sMoBailRate.intst_amt=g_pub_intst.dealval;
			strcpy(sMoBailRate.sts,"*");
			ret = Mo_bail_rate_Upd_Upd(sMoBailRate,g_pub_tx.reply);
			if( ret )
   			{
   				sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,pro_note.pact_no);
       		 	WRITEMSG
				sprintf( g_pub_tx.reply, "B334" );
       		 	goto ErrExit;
   			}
			Mo_bail_rate_Clo_Upd();

			/** 插入新的保证金利息登记薄 **/
			if( pub_base_CompDblVal(sMoBailRate1.amt,0.00) > 0 )
			{
				ret = Mo_bail_rate_Ins(sMoBailRate1,g_pub_tx.reply);
				if( ret )
   				{
   					sprintf( acErrMsg, "执行Mo_bail_rate_Ins错![%d]",ret);
        			WRITEMSG
					sprintf( g_pub_tx.reply, "B334" );
        			goto ErrExit;
   				}
			}
			g_pub_intst.tax_intst=g_pub_intst.dealval;	/** 应税利息 **/
			g_pub_intst.factval=g_pub_intst.dealval;	/** 实付利息 **/
			g_pub_intst.val=0.00;						/** 利息税 **/
			g_pub_intst.keepval=0.00;					/** 保值利息 **/

			pro_note.bail_amt=pro_note.bail_amt+g_pub_intst.dealval;
			set_zd_data("0153","52101");                /** 保证金利息 **/
			set_zd_double("102P",g_pub_intst.dealval);
		}
	}
	/**---------- hao ---------------- end ------------**/

vtcp_log("RATIO ratio = [%f] ratio=[%f][%s->%d]",pro_note.ratio,g_pub_tx.rate,__FILE__,__LINE__);
	set_zd_data("1011",pro_note.pro_ac_no);
	set_zd_int ("1012",1);
	/**
	set_zd_data("1014",g_pub_tx.note_type);
	set_zd_data("1015",g_pub_tx.beg_note_no);
	**/
	set_zd_data("1016","贷 担保申请人帐户");
	set_zd_data("101A","01");
	set_zd_data("101B","2");				/*转帐*/
    set_zd_double("1013",pro_note.bail_amt);

	/**----- hao ---- @20051113@ ---- 撤销必须要有协议编号 ----**/
	strcpy(g_pub_tx.ac_no,pro_note.pact_no);
	g_pub_tx.svc_ind=0;
	strcpy(g_pub_tx.no_show,"1");
	if( intst_flag[0]=='1' )		/*** 保证金计息了 ***/
	{
		strcpy(g_pub_tx.note_type,"Y");
	}
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");

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
