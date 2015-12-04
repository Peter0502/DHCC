/*************************************************
* 文 件 名:  spH664.c
* 功能描述： 账户余额查询(缺省序号则查询活期)
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
#include "mob_acct_type_c.h"

int spH664()
{
	int ret = 0;
	int i = 0;
	int iAc_seq = 0;
	char cAc_type[2];

	double dAcbl = 0.00;

	char cOpt[4];
	char cMessage[141];
	char cMobile[16];

	char fldstr[5][128];

	struct mob_sendmail_c sMob_sendmail;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mob_acct_type_c sMob_acct_type;

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
		sprintf( acErrMsg, "[%s]", fldstr[i] );
		WRITEMSG

		i= i + 1;
	}
	
	if ( strlen(fldstr[1]) == 0 )
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Sel( g_pub_tx.reply, &sMob_acct_type, \
			"mobile = '%s'", cMobile );
		
		strcpy( fldstr[1], sMob_acct_type.ac_no );
	}

	ret = sql_count( "mob_acct_type", "mobile = '%s' and \
			ac_no = '%s'", cMobile, fldstr[1] ); 
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
	if ( ret != 0 )
	{
		sprintf( acErrMsg, "查询介质账号对照表失败!!!acno[%s][%d]", fldstr[1], ret );
		WRITEMSG
		goto ErrExit;
	}

	/*----------------------------------------------
	if ( strcmp( sMdm_ac_rel.draw_pwd, fldstr[3] ) != 0 )
	{
		strcpy ( g_pub_tx.reply, "F004" );
		sprintf( acErrMsg, "个人支取密码密码错!!!" );
		WRITEMSG
		goto ErrExit;
	}
	-----------------------------------------------*/

	if ( strlen( fldstr[2] ) == 0 )
	{
		iAc_seq = 1;	/* 没输入序号,则按人民币活期 */
	}
	else
	{
		iAc_seq = atoi( fldstr[2] );
	}

	ret = pub_base_disac_file ( fldstr[1], iAc_seq, cAc_type );
	if ( ret )
	{
		strcpy( cMessage, "尊敬的客户: 你输入的账(卡)号有问题,请核对后再操作!" );
		sprintf( acErrMsg, "查询主文件出错!!!ret[%d]" ,ret );
		WRITEMSG
		goto ErrExit;
	}

	if ( cAc_type[0] == '1' )		/*-- 活期 --*/
	{
		dAcbl = g_dd_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的账(卡)%s当前活期余额为%.2f元.长沙市商业银行",  fldstr[1], dAcbl );
	}
	if ( cAc_type[0] == '2' )		/*-- 定期 --*/
	{
		dAcbl = g_td_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的账(卡)%s当前定期余额为%.2f元.长沙市商业银行",  fldstr[1], dAcbl );
	}
	if ( cAc_type[0] == '3' )		/*-- 贷款 --*/
	{
		dAcbl = g_ln_mst.bal;

		fldstr[1][strlen(fldstr[1])-2] = '*';
		pub_base_strpack( fldstr[1] );

		memset( cMessage, 0x0, sizeof(cMessage) );
		sprintf( cMessage, "尊敬的客户:您的账(卡)%s当前贷款余额为%.2f元.长沙市商业银行",  fldstr[1], dAcbl );
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
	sprintf(acErrMsg,"账户余额查询成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data( "0760", cMessage );
	sprintf(acErrMsg,"账户余额查询失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
