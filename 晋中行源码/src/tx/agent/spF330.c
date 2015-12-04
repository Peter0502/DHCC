/*************************************************
* 文 件 名:  spF330.c
* 功能描述： 保证金取款功能
*
* 作    者:  mike
* 完成日期： 2004年11月6日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "protect_note_c.h"
#include "com_parm_c.h"
#include "mo_bail_rate_c.h"

int spF330()
{
	struct	protect_note_c		sProtectNote;	/* 保函协议登记簿 */
	struct	mo_bail_rate_c		sMoBailRate;	/* 保证金利率登记薄 */
	struct	mo_bail_rate_c		sMoBailRate1;	/* 保证金利率登记薄 */
	double	dTempAmt=0.00;						/* 退票时应该从保证金户扣划到结算户的金额 */
	int		ret;
	long     day=0;
	long     HQday=0;
	double	HQ_rate=0.00;
	double	HQdealval=0.00;
	char	bail_ac_no[25];
	char	intst_ac_no[25];
	char	intst_flag[2];

	/* 数据初始化 */
	memset( &sProtectNote, 0x00, sizeof( struct protect_note_c ) );
	memset( &sMoBailRate, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset( &sMoBailRate1, 0x00, sizeof( struct mo_bail_rate_c ) );
	memset ( &g_pub_intst,0x00,sizeof(struct S_pub_intst) );
	pub_base_sysinit();

	/* 从前台屏幕取值 */
	get_zd_data( "0330", sProtectNote.pact_no );		/* 保函协议编号 */
	get_zd_data( "0320", bail_ac_no );					/* 保证金帐号 */
	get_zd_data( "0310", intst_ac_no );					/* 收息帐号 */
	get_zd_double( "0390", &dTempAmt );					/* 取款金额 */
	get_zd_data( "0670" , intst_flag );					/* 计息标志 */
	
	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Protect_note_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", sProtectNote.pact_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Protect_note_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Protect_note_Fet_Upd( &sProtectNote, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "此保函协议编号不存在!" );
       	WRITEMSG
        strcpy( g_pub_tx.reply, "B351" );
        goto ErrExit;
    }
	else if( ret )
    {
    	sprintf( acErrMsg, "执行Protect_note_Fet_Upd错![%d]", ret );
       	WRITEMSG
        goto ErrExit;
    }

/*
	if( sProtectNote.portect_sts[0] != '0' )
	{
		sprintf( acErrMsg, "此承兑协议为录入状态!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B213" );
		goto ErrExit;
	}
*/
	if( pub_base_CompDblVal(dTempAmt,sProtectNote.bail_amt) > 0 )
	{
		sprintf( acErrMsg, "取款金额错误![%.2f][%.2f]",dTempAmt,sProtectNote.bail_amt );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P115" );
		goto ErrExit;
	}
	pub_base_strpack(sProtectNote.bail_ac_no);
	pub_base_strpack(bail_ac_no);
	if( strcmp(sProtectNote.bail_ac_no,bail_ac_no) )
	{
		sprintf( acErrMsg, "协议编号对应的保证金帐号与录入保证金帐号不一致!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B329" );
		goto ErrExit;
	}

	/* 修改保函协议信息登记簿 */
	if( pub_base_CompDblVal(dTempAmt,0.00) > 0 )
		sProtectNote.bail_amt=sProtectNote.bail_amt-dTempAmt;

	ret = Protect_note_Upd_Upd( sProtectNote, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改承兑汇票基本信息登记薄错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Protect_note_Clo_Upd();

	/** 撤销必须要有协议编号 **/
	strcpy(g_pub_tx.ac_no,sProtectNote.pact_no);
	g_pub_tx.tx_amt1 = dTempAmt;
	strcpy( g_pub_tx.beg_note_no, bail_ac_no );
	strcpy( g_pub_tx.brf, "保证金取款" );
	g_pub_tx.svc_ind=0;
	strcpy( g_pub_tx.note_type,intst_flag );	/** 撤销用是否计息了 **/
	strcpy(g_pub_tx.no_show,"1");
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	strcpy(g_pub_tx.no_show,"0");

	if( intst_flag[0]=='Y' )
	{
		ret = Mo_bail_rate_Dec_Upd(g_pub_tx.reply,"bail_type='2' and pact_no='%s' and sts='1' ", sProtectNote.pact_no );
   		if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]",ret,sProtectNote.pact_no);
       		WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
       		goto ErrExit;
   		}
		ret = Mo_bail_rate_Fet_Upd(&sMoBailRate,g_pub_tx.reply);
   		if( ret )
   		{
   			sprintf( acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]",ret,sProtectNote.pact_no);
       		WRITEMSG
			sprintf( g_pub_tx.reply, "B334" );
       		goto ErrExit;
   		}
		memcpy( &sMoBailRate1, &sMoBailRate, sizeof( struct mo_bail_rate_c ) );

		pub_base_strpack(sMoBailRate.ac_no);
		pub_base_strpack(intst_ac_no);
		if( strcmp(sMoBailRate.ac_no,intst_ac_no) )
		{
			sprintf( acErrMsg, "输入的收息帐号与输入的不一致![%s][%s]",sMoBailRate.ac_no,intst_ac_no );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B343" );
			goto ErrExit;
		}

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
		if( pub_base_CompDblVal(dTempAmt,sMoBailRate.amt) > 0 )
		{
       		HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
       		g_pub_intst.dealval = sMoBailRate.ic_rate * sMoBailRate.amt * day / L360 / L100;
			g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
			sMoBailRate1.amt=0.00;
		}
		else
		{
       		HQdealval = HQ_rate * dTempAmt * HQday / L360 / L100;
       		g_pub_intst.dealval = sMoBailRate.ic_rate * dTempAmt * day / L360 / L100;
			g_pub_intst.dealval = g_pub_intst.dealval + HQdealval;
			sMoBailRate1.amt=sMoBailRate.amt-dTempAmt;
			sMoBailRate.amt=dTempAmt;
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
   			sprintf( acErrMsg, "执行Mo_bail_rate_Upd_Upd错![%d][%s]",ret,sProtectNote.pact_no);
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

		dTempAmt=dTempAmt+g_pub_intst.dealval;
		set_zd_data("0153","52101");				/** 保证金利息 **/
		set_zd_double("102P",g_pub_intst.dealval);
	}
	else
	{
		g_pub_intst.dealval=0.00;
		g_pub_intst.tax_intst=0.00;					/** 应税利息 **/
		g_pub_intst.factval=0.00;					/** 实付利息 **/
		g_pub_intst.val=0.00;						/** 利息税 **/
		g_pub_intst.keepval=0.00;					/** 保值利息 **/
		set_zd_data("0153","52101");				/** 保证金利息 **/
		set_zd_double("102P",g_pub_intst.dealval);
	}
	set_zd_double("0410",dTempAmt);

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
