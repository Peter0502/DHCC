/*************************************************
* 文 件 名:  spH667.c
* 功能描述： 查询账户当天累计发生额
*
* 作    者:  mike
* 完成日期： 2004年04月25日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"

int spH667()
{
	int ret = 0;
	int i = 0;
	int iAc_seq = 0;
	char cAc_type[2];

	double dJfs = 0.00;
	double dDfs = 0.00;

	char cOpt[4];
	char cMessage[141];
	char cMobile[16];

	char fldstr[5][128];

	struct mob_sendmail_c sMob_sendmail;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset( cAc_type, 0x0, sizeof(cAc_type) );

	memset( cOpt, 0x0, sizeof(cOpt) );
	memset( cMessage, 0x0, sizeof(cMessage) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( fldstr, 0x0, sizeof(fldstr) );
	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0760", cMessage );
	get_zd_data( "0610", cMobile );

	for ( ; ; )
	{
		ret = pub_base_cut_str( cMessage, fldstr[i], "#", i+1 );
		if ( ret == 100 )
		{
			break;
		}

		pub_base_strpack( fldstr[i] );

		i= i + 1;
	}

	ret = sql_count( "mob_acct_type", "mobile = '%s' and \
			ac_no = '%s'", cMobile, fldstr[i] ); 
	if ( ret <= 0 )
	{
		strcpy( cMessage,"该账号对应该手机没有开通短信银行业务!" );
		sprintf( acErrMsg, "该账号对应该手机没有开通短信银行业务!" );
		WRITEMSG
		goto ErrExit;
	}

	/*--- fldstr[1]账号 fldstr[2]账号序号 fldstr[3]密码 ---*/

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
		"ac_no = '%s'", fldstr[1] );
	if ( ret == 100 )
	{
		sprintf( cMessage, "尊敬的客户:您的账户%s不存在!", fldstr[1] );
		sprintf( acErrMsg, "查询介质账号对照表失败!!!acno[%s][%d]", fldstr[1], ret );
		WRITEMSG
		goto ErrExit;
	}
	if ( ret != 0 )
	{
		sprintf( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
		sprintf( acErrMsg, "查询介质账号对照表失败!!!acno[%s][%d]", fldstr[1], ret );
		WRITEMSG
		goto ErrExit;
	}

	/*---------------------------------------------------
	if ( strcmp( sMdm_ac_rel.draw_pwd, fldstr[3] ) != 0 )
	{
		strcpy ( g_pub_tx.reply, "F004" );
		sprintf( acErrMsg, "个人支取密码密码错!!!" );
		WRITEMSG
		goto ErrExit;
	}
	---------------------------------------------------*/

	if ( strlen( fldstr[2] ) == 0 )
	{
		iAc_seq = 1;	/* 没输入序号,则按人民币活期 */
	}
	else
	{
		iAc_seq = atoi( fldstr[2] );
	}

	ret = pub_base_disac_file ( fldstr[1], iAc_seq, cAc_type );
	{
		sprintf( cMessage, "尊敬的客户:您的账户%s有问题,请核对后再试!", fldstr[1] );
		sprintf( acErrMsg, "查询主文件出错!!!ret[%d]" ,ret );
		WRITEMSG
		goto ErrExit;
	}

	if ( cAc_type[0] == '1' )		/*-- 活期 --*/
	{
		ret = sql_sum_double ( "dd_mst_hst" , "tx_amt" , &dJfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '0'", \
			g_dd_mst.ac_id, g_dd_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计活期账号借方发生额出错!!!ret[%d]" ,ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double ( "dd_mst_hst" , "tx_amt" , &dDfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '1'", \
			g_dd_mst.ac_id, g_dd_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计活期账号贷方发生额出错!!!ret[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的活期账户%s当天累计取款%.2f元存款%.2f元.长沙市商业银行", dJfs, dDfs );
	}
	if ( cAc_type[0] == '2' )		/*-- 定期 --*/
	{
		ret = sql_sum_double ( "td_mst_hst" , "tx_amt" , &dJfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '0'", \
			g_td_mst.ac_id, g_td_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计定期账号借方发生额出错!!!ret[%d]" ,ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double ( "td_mst_hst" , "tx_amt" , &dDfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '1'", \
			g_td_mst.ac_id, g_td_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计定期账号贷方发生额出错!!!ret[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的定期账户%s当天累计取款%.2f元存款%.2f元.长沙市商业银行", dJfs, dDfs );
	}
	if ( cAc_type[0] == '3' )		/*-- 贷款 --*/
	{
		ret = sql_sum_double ( "ln_mst_hst" , "tx_amt" , &dJfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '0'", \
			g_ln_mst.ac_id, g_ln_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计贷款账号借方发生额出错!!!ret[%d]" ,ret );
			WRITEMSG
			goto ErrExit;
		}

		ret = sql_sum_double ( "dd_mst_hst" , "tx_amt" , &dDfs , \
			"ac_id = %ld and ac_seqn = %d and tx_date = %ld \
			and add_ind = '1'", \
			g_dd_mst.ac_id, g_dd_mst.ac_seqn, g_pub_tx.tx_date );
		if ( ret != 0 )
		{
			strcpy( cMessage, "尊敬的客户:系统有问题,请稍后再试!" );
			sprintf( acErrMsg, "统计贷款账号贷方发生额出错!!!ret[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的贷款账户%s当天累计取款%.2f元存款%.2f元.长沙市商业银行", dJfs, dDfs );
	}

	strcpy( sMob_sendmail.tx_code, g_pub_tx.tx_code );
	strcpy( sMob_sendmail.ac_no, fldstr[1] );
	strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
	sMob_sendmail.trace_no = g_pub_tx.trace_no;
	sMob_sendmail.tx_date = g_pub_tx.tx_date;
	sMob_sendmail.tx_time = g_pub_tx.tx_time;
	get_zd_data( "0240", sMob_sendmail.opt_no );
	get_zd_data( "0610", sMob_sendmail.mobile );
	strcpy( sMob_sendmail.message, cMessage );
	strcpy(	sMob_sendmail.timeflag, "0" );
	sMob_sendmail.t_date = 0;
	sMob_sendmail.t_time = 0;
	strcpy(	sMob_sendmail.send_level, "1" );
	sMob_sendmail.sene_times = 1;
	sMob_sendmail.send_date = 0;
	sMob_sendmail.send_time = 0;
	sMob_sendmail.sxf_amt = 0.00;
	strcpy(	sMob_sendmail.proc_flag, "1" );

	ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
	if ( ret != 0 ) 
	{
		sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0760", cMessage );

OkExit:
	sprintf(acErrMsg,"短信服务介绍成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"短信服务介绍失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
