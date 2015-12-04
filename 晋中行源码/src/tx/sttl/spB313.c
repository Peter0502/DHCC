/***************************************************************
* 文 件 名: spB313.c
* 功能描述：本票核销子交易(结清)
*
* 作    者: jane
* 完成日期：2003年11月09日
*
* 修改记录：
*    日    期: 
*    修 改 人: 
*    修改内容: 
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_parm_c.h"
#include "mo_cashier_c.h"
#include "mo_city_c.h"

spB313()
{
	struct mo_cashier_c sMoCashier;
	struct mo_city_c sMoCity;
	struct com_parm_c sComParm;

	char deal_flag[2];    /*结清方式*/
	char ct_ind[2];    /*现转标志*/
	double sign_amt,tmp_amt;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );

	pub_base_sysinit();

	get_zd_data( "0690", deal_flag );
	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_double( "0390", &sign_amt );

	/*检查本票、状态、挂失、出票金额*/
	g_reply_int = Mo_cashier_Dec_Upd( g_pub_tx.reply, "cashier_no='%s'", sMoCashier.cashier_no );
	if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "查询本票登记簿出错![%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}
	g_reply_int = Mo_cashier_Fet_Upd( &sMoCashier, g_pub_tx.reply );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "本票号码不存在!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "查询本票登记簿出错![%d]", g_reply_int );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( sMoCashier.cashier_sts[0] == '*' )
	{
		sprintf( acErrMsg, "该本票已核销!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.cashier_sts[0] == '0' )
	{
		sprintf( acErrMsg, "该本票尚未签发!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.cashier_sts[0] != '1' )
	{
		sprintf( acErrMsg, "该本票状态有误,请联系中心机房![%s]", sMoCashier.cashier_sts );
		MBFEWRITEMSG
		goto ErrExit;
	}

	if( sMoCashier.los_rpt_ind[0] == '1' )
	{
		sprintf( acErrMsg, "该本票已挂失!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( sMoCashier.los_rpt_ind[0]!='0' && sMoCashier.los_rpt_ind[0]!='2' )
	{
		sprintf( acErrMsg, "该本票挂失状态有误,请联系中心机房![%s]", sMoCashier.los_rpt_ind );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_CompDblVal( sign_amt, sMoCashier.sign_amt );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "出票金额有误!" );
		MBFEWRITEMSG
		vtcp_log("cli_val:%lf, db_val:%lf", sign_amt, sMoCashier.sign_amt);
		goto ErrExit;
	}

	get_zd_double( "0400", &sMoCashier.cash_amt );
	get_zd_double( "0410", &sMoCashier.rem_amt );
	get_zd_data( "0320", sMoCashier.rem_ac_no );
	sMoCashier.can_date = g_pub_tx.tx_date;
	sMoCashier.can_trace_no = g_pub_tx.trace_no;
	strcpy( sMoCashier.cashier_sts, "*" );

	if(deal_flag[0]=='1')
	{
		strcpy( ct_ind, "2" );
		/*检查并更新同城登记簿*/
		strcpy( sMoCashier.use_ind, "1" );
		get_zd_data( "0300", sMoCashier.pay_ac_no );
		get_zd_double( "0400", &sMoCashier.cash_amt );
		memset( &sMoCity, 0x00, sizeof( struct mo_city_c ) );
		g_reply_int = Mo_city_Dec_Upd( g_pub_tx.reply, "payer_ac_no='%s' and bill_amt='%lf' and sts='C'",
			sMoCashier.pay_ac_no, sMoCashier.cash_amt );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "声明游标错[%d]!", g_reply_int );
			WRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}
		g_reply_int = Mo_city_Fet_Upd( &sMoCity, g_pub_tx.reply );
		if( g_reply_int == 100 )
		{
			sprintf( acErrMsg, "同城登记簿中不存在该笔记录!" );
			MBFEWRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}
		else if( g_reply_int != 0 )
		{
			sprintf( acErrMsg, "取游标错[%d]!", g_reply_int );
			WRITEMSG
			vtcp_log("ac_no[%s], bill_amt[%lf]", sMoCashier.pay_ac_no, sMoCashier.cash_amt );
			goto ErrExit;
		}

		strcpy( sMoCity.sts, "O" );

		g_reply_int = Mo_city_Upd_Upd( sMoCity, g_pub_tx.reply );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "更新表错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		Mo_city_Clo_Upd();

		/*贷记40802赋值*/
		memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
		g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='TCKM' and parm_seqn='3'" );
		if( g_reply_int )
		{
			sprintf( acErrMsg, "查询同城提入科目出错[%d]!", g_reply_int );
			WRITEMSG
			goto ErrExit;
		}
		strcpy( sMoCashier.rec_ac_no, sComParm.val );
		set_zd_data( "1211", sComParm.val );
		set_zd_data( "1214", "01" );
		set_zd_data( "1215", "2" );
		set_zd_data( "1216", "113" );
		set_zd_data( "1217", sMoCashier.cashier_no );
		set_zd_double( "1218", sMoCashier.cash_amt );
		set_zd_data( "121A", "本票结清" );

		strcpy(g_pub_tx.reply,"0001");
	}
	else if(deal_flag[0]=='2')
	{
		strcpy( ct_ind, "2" );
		strcpy( sMoCashier.use_ind, "2" );
		/*记客户帐赋值*/
		get_zd_data( "0310", sMoCashier.rec_ac_no );

		set_zd_data( "0191", "1" );
		set_zd_data( "0192", "0001" );
		set_zd_data( "0193", "1001" );
		set_zd_data( "1011", sMoCashier.rec_ac_no );
		set_zd_double( "1013", sMoCashier.cash_amt );
		set_zd_data( "1016", "本票兑付" );
		set_zd_data( "101A", "01" );
		set_zd_data( "101B", "2" );

		strcpy(g_pub_tx.reply,"0002");
	}
	else if( deal_flag[0] == '3' )
	{
		strcpy( ct_ind, "1" );
		if( sMoCashier.ct_ind[0] != '1' )
		{
			sprintf( acErrMsg, "非现金本票,不能兑付现金" );
			MBFEWRITEMSG
			goto ErrExit;
		}

		g_reply_int = pub_base_CompDblVal( sMoCashier.rem_amt, 0.005 );
		if( g_reply_int > 0 )
		{
			sprintf( acErrMsg, "现金本票需全额兑付" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		
		strcpy( sMoCashier.use_ind, "3" );
		strcpy( sMoCashier.rec_ac_no, "101" );

		set_zd_data( "1191", "01" );
		set_zd_data( "1192", "113" );
		set_zd_data( "1193", sMoCashier.cashier_no );
		set_zd_double( "1194", sMoCashier.cash_amt );
		set_zd_data( "1197", "现金本票兑付" );

		strcpy( g_pub_tx.reply, "0003" );
	}
	else
	{
		sprintf( acErrMsg, "兑付结清方式有误![%s]", deal_flag );
		MBFEWRITEMSG
		goto ErrExit;
	}

	g_reply_int = Mo_cashier_Upd_Upd( sMoCashier, g_pub_tx.reply );
	if( g_reply_int != 0 )
	{
		sprintf( acErrMsg, "更新表错[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	Mo_cashier_Clo_Upd();

	/*给A016借记252传值*/
	memset( &sComParm, 0x00, sizeof( struct com_parm_c ) );
	g_reply_int = Com_parm_Sel( g_pub_tx.reply, &sComParm, "parm_code='BPKM' and parm_seqn='2'" );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "查询本票签发科目出错[%d]!", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}
	strcpy( sMoCashier.rec_ac_no, sComParm.val );
	set_zd_data( "1201", sComParm.val );
	set_zd_data( "1204", "01" );
	set_zd_data( "1205", ct_ind );
	set_zd_data( "1206", "113" );
	set_zd_data( "1207", sMoCashier.cashier_no );
	set_zd_double( "1208", sMoCashier.sign_amt );
	set_zd_data( "120A", "本票结清兑付" );

	/*记交易日志为撤销用*/
	strcpy( g_pub_tx.no_show, "1" );/*隐形*/
	strcpy( g_pub_tx.beg_note_no, sMoCashier.cashier_no );
	strcpy( g_pub_tx.ac_no, sMoCashier.pay_ac_no );
	g_pub_tx.tx_amt1 = sMoCashier.cash_amt;
	strcpy( g_pub_tx.ct_ind, deal_flag );
	strcpy( g_pub_tx.brf, "本票结清兑付" );
	g_reply_int = pub_ins_trace_log();
	if( g_reply_int )
	{
		sprintf( acErrMsg," 记交易日志错误![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg,"更新本票签发登记簿成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"更新本票签发登记簿失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
