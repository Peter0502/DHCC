/*************************************************
* 文 件 名:  spH661.c
* 功能描述： 短信服务和服务品种介绍
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

int spH661()
{
	int ret = 0;

	char cOpt[4];
	char cMessage[141];

	char cMobile[16];

	struct mob_sendmail_c sMob_sendmail;

	memset( cOpt, 0x0, sizeof(cOpt) );
	memset( cMessage, 0x0, sizeof(cMessage) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0760", cMessage );
	get_zd_data( "0610", cMobile );

	ret = sql_count( "mob_acct_type", "mobile = '%s'", cMobile ); 
	if ( ret <= 0 )
	{
		strcpy( cMessage,"该手机没有开通短信银行业务!" );
		sprintf( acErrMsg, "该手机没有开通短信银行业务!" );
		WRITEMSG
		goto ErrExit;
	}

	if ( strncmp(cMessage, "0001", 4) == 0 )
	{
		memset( cMessage, 0x0, sizeof(cMessage) );

		strcpy( cMessage,"长沙商行短信为您服务.请到我行柜台申请服务.移动用户发0002到13874183399,联通用户发0002到13054110000品种介绍");
	}
	if ( strncmp(cMessage, "0002", 4) == 0 )
	{
		memset( cMessage, 0x0, sizeof(cMessage) );

		strcpy( cMessage,"0001介绍(dxjs),0003查询余额(cxye),ktgs卡折口头挂失,0005查当日累计(cxlj),zxfw注消服务");
	}

	strcpy( sMob_sendmail.tx_code, g_pub_tx.tx_code );
	strcpy( sMob_sendmail.ac_no, "账号" );
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
	set_zd_data( "0760", cMessage );
	sprintf(acErrMsg,"短信服务介绍失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
