/*************************************************
* 文 件 名:  spH651.c
* 功能描述： 客户短信服务品种维护
*
* 作    者:  jack
* 完成日期： 2004年04月24日
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
#include "mob_custinfo_c.h"

int spH651()
{
	int ret = 0;
	int i = 0;
	int iLen = 0;
	int iAc_count = 0;	/*---区别是否要向客户发送开通业务交易---*/

	char cFlag[2];
	char cMobile[14];
	char cAcno[20];
	char cPasswd[7];
	char cSerind[51];

	long lCif_no = 0;

	char fldstr[11][5];
	char cMessage[141];

	struct mob_sendmail_c sMob_sendmail;
	struct mob_acct_type_c sMob_acct_type;
	struct mob_custinfo_c sMob_custinfo;
	struct mdm_ac_rel_c sMdm_ac_rel;

	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cPasswd, 0x0, sizeof(cPasswd) );
	memset( cSerind, 0x0, sizeof(cSerind) );
	memset( fldstr, 0x0, sizeof(fldstr) );

	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0700", cFlag );
	get_zd_data( "0610", cMobile );
	get_zd_data( "0310", cAcno );
	pub_base_old_acno( cAcno );
	
	get_zd_data( "0790", cPasswd );
	get_zd_data( "0810", cSerind );

	pub_base_strpack( cSerind );

	if ( cFlag[0] == '0' )
	{
		memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply , &sMdm_ac_rel, \
			"ac_no = '%s'", cAcno );
		if ( ret != 0 )
		{
			sprintf( acErrMsg, "查询介质账号对照表失败!!!acno[%s][%d]", fldstr[1], ret );
			WRITEMSG
			goto ErrExit;
		}

		/*-----del by dgf 20050829----------------------*
		pub_base_EnDes( g_pub_tx.tx_date, "", cPasswd );
		if ( strcmp( sMdm_ac_rel.draw_pwd, cPasswd ) != 0 )
		{
			strcpy ( g_pub_tx.reply, "F004" );
			sprintf( acErrMsg, "个人支取密码密码错!!!" );
			WRITEMSG
			goto ErrExit;
		}
		*-----------------------------------------------*/

		ret = sql_count( "mob_acct_type", "ac_id = %ld", \
			sMdm_ac_rel.ac_id );
		if ( ret < 0 )
		{
			strcpy( g_pub_tx.reply, "F031" );
			sprintf(acErrMsg,"统计短信客户信息出错[%d]!!!", ret );
			WRITEMSG
			goto ErrExit;
		}
		if ( ret == 0 )
		{
			memset( cMessage, 0x0, sizeof(cMessage) );
			sprintf( cMessage, "尊敬的客户,恭喜您开通了账号%s的短信服务,回0002可看服务品种介绍", cAcno );
			
			memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
			get_zd_data( "0160", sMob_sendmail.tx_code );
			strcpy( sMob_sendmail.ac_no, cAcno );
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = g_pub_tx.tx_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, cMobile );
			strcpy( sMob_sendmail.message, cMessage );
			strcpy(	sMob_sendmail.timeflag, "0" );
			sMob_sendmail.t_date = 0;
			sMob_sendmail.t_time = 0;
			strcpy(	sMob_sendmail.send_level, "1" );
			sMob_sendmail.sene_times = 1;
			sMob_sendmail.send_date = 0;
			sMob_sendmail.send_time = 0;
			sMob_sendmail.sxf_amt = 0.00;
			strcpy(	sMob_sendmail.proc_flag, "0" );
        		
			ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
			if ( ret != 0 ) 
			{
				sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		if ( ret > 0 ) /*该账号已经存在记录,需要判断手机号码是否同一个*/
		{
			ret = 0;
			ret = sql_count( "mob_acct_type", "ac_id = %ld and \
				mobile = '%s'", sMdm_ac_rel.ac_id, cMobile );
			if ( ret < 0 )
			{
				strcpy( g_pub_tx.reply, "F031" );
				sprintf( acErrMsg, "统计短信客户信息出错[%d]!!!", ret );
				WRITEMSG
				goto ErrExit;
			}
			if ( ret == 0 )
			{
				strcpy( g_pub_tx.reply, "F032" );
				sprintf(acErrMsg,"一个账号不能对应多个手机号码!!" );
				WRITEMSG
				goto ErrExit;
			}
		}
	}

	for ( ; ; )
	{
		ret = pub_base_cut_str( cSerind,fldstr[i],";",i+1 );
		if ( ret == 100 )
		{
			break;
		}

		if ( cFlag[0] == '0' )	/*---  增加  ---*/
		{
			memset( &sMob_acct_type, 0x00, sizeof(sMob_acct_type) );
			memset( &sMob_custinfo, 0x00, sizeof(sMob_custinfo) );

			strcpy( sMob_acct_type.mobile, cMobile );
			strcpy( sMob_acct_type.ac_no, cAcno );
			sMob_acct_type.ac_id = sMdm_ac_rel.ac_id;
			strcpy( sMob_acct_type.server_type, fldstr[i] );
			sMob_acct_type.tx_date = g_pub_tx.tx_date;
			strcpy( sMob_acct_type.tel, g_pub_tx.tel );
			strcpy( sMob_acct_type.sfbz, "0" );

			ret = Mob_acct_type_Ins( sMob_acct_type, g_pub_tx.reply );
			if ( ret != 0 )
			{
				if ( ret == -268 )
				{
					strcpy( g_pub_tx.reply, "D001" );
					sprintf(acErrMsg,"服务类型[%s]插入数据库重复",fldstr[i]);
					set_zd_data( "0130", acErrMsg );
				}

				sprintf(acErrMsg,"执行Mob_acct_type_Ins出错[%d]!!!", g_pub_tx.reply);
				WRITEMSG
				goto ErrExit;
			}

			ret = sql_count( "mob_custinfo", "mobile = '%s'", cMobile );
			if ( ret < 0 )
			{
				strcpy( g_pub_tx.reply, "F031" );
				sprintf(acErrMsg,"统计短信客户信息出错[%d]!!!", ret );
				WRITEMSG
				goto ErrExit;
			}
			if ( ret == 0 )		/*--- 该客户没登记过 ---*/
			{
				ret = pub_base_acgetname( cAcno, &lCif_no, \
					sMob_custinfo.cust_name);
				if ( ret != 0 )
				{
					sprintf(acErrMsg,"该账号对应的客户号和户名不存在!!![%s][%d]", cAcno, ret );
					WRITEMSG
					goto ErrExit;
				}

				sprintf( sMob_custinfo.cust_no , "%ld", lCif_no );
				strcpy( sMob_custinfo.mobile, cMobile );
				sMob_custinfo.group = 8888;
				strcpy( sMob_custinfo.passwd, "111111" );

				ret = Mob_custinfo_Ins( sMob_custinfo, g_pub_tx.reply );
				if ( ret != 0 )
				{
					sprintf(acErrMsg,"执行Mob_custinfo_Ins出错[%d]!!!", g_pub_tx.reply);
					WRITEMSG
					goto ErrExit;
				}
				
							
				memset( cMessage, 0x0, sizeof(cMessage) );
				sprintf( cMessage, "%11ld%20s%15s%40s%6d00000", lCif_no, "",  \
					cMobile, sMob_custinfo.cust_name, 0 );
				
				memset( &sMob_sendmail, 0x00, sizeof(sMob_sendmail) );
				get_zd_data( "0160", sMob_sendmail.tx_code );
				strcpy( sMob_sendmail.ac_no, cAcno );
				strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
				sMob_sendmail.trace_no = g_pub_tx.trace_no;
				sMob_sendmail.tx_date = g_pub_tx.tx_date;
				sMob_sendmail.tx_time = g_pub_tx.tx_time;
				strcpy( sMob_sendmail.opt_no, "120" );
				strcpy( sMob_sendmail.mobile, cMobile );
				strcpy( sMob_sendmail.message, cMessage );
				strcpy(	sMob_sendmail.timeflag, "0" );
				sMob_sendmail.t_date = 0;
				sMob_sendmail.t_time = 0;
				strcpy(	sMob_sendmail.send_level, "1" );
				sMob_sendmail.sene_times = 1;
				sMob_sendmail.send_date = 0;
				sMob_sendmail.send_time = 0;
				sMob_sendmail.sxf_amt = 0.00;
				strcpy(	sMob_sendmail.proc_flag, "0" );
        			
				ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
				if ( ret != 0 ) 
				{
					sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
					WRITEMSG
					goto ErrExit;
				}
			}
		}

		if ( cFlag[0] == '2' )	/*---  删除  ---*/
		{
			ret = sql_execute( "delete from mob_acct_type where \
				mobile = '%s' and ac_no = '%s' and server_type = '%s'",\
				cMobile, cAcno, fldstr[i] );
			if ( ret )
			{
				strcpy( g_pub_tx.reply, "F033" );
				sprintf(acErrMsg,"删除客户账号服务品种出错!!![%s]", fldstr[i] );
				WRITEMSG
				goto ErrExit;
			}
		}

		i = i + 1;
	}

	if ( cFlag[0] == '0' )	/*判断该清除的记录*/
	{
		/*----------------------1002包括1007--------------------------*/
		ret = 0;
		ret = sql_count( "mob_acct_type", "mobile = '%s' and \
			ac_id = %ld and server_type = '1002'", \
			cMobile, sMdm_ac_rel.ac_id ); 
		if ( ret < 0 )
		{
			strcpy( g_pub_tx.reply, "F031" );
			sprintf(acErrMsg,"统计客户1002服务品种出错!!![%s][%s]", cMobile, cAcno );
			WRITEMSG
			goto ErrExit;
		}
		
		sprintf(acErrMsg, "--------------->取出的记录数为[%d]", ret );
		WRITEMSG
		
		if ( ret == 1 )
		{
			ret = sql_execute( "delete from mob_acct_type where mobile = '%s' and ac_no = '%s' \
				and server_type = '1007'", cMobile, cAcno );
			if ( ret )
			{
				strcpy( g_pub_tx.reply, "F033" );
				sprintf(acErrMsg,"删除客户账号服务品种出错!!![%s]", fldstr[i] );
				WRITEMSG
				goto ErrExit;
			}
		}
		
		/*----------------------1001包括1002,1006,1007---------------------*/
		ret = 0;
		ret = sql_count( "mob_acct_type", "mobile = '%s' \
			and ac_id = %ld and server_type = '1001'", \
			cMobile, sMdm_ac_rel.ac_id ); 
		if ( ret < 0 )
		{
			strcpy( g_pub_tx.reply, "F031" );
			sprintf(acErrMsg,"统计客户1002服务品种出错!!![%s][%s]", cMobile, cAcno );
			WRITEMSG
			goto ErrExit;
		}
		
		if ( ret == 1 )
		{
			ret = sql_execute( "delete from mob_acct_type where \
				mobile = '%s' and ac_id = %ld and \
				server_type in( '1002', '1006', '1007' ) ", \
				cMobile, sMdm_ac_rel.ac_id );
			if ( ret )
			{
				strcpy( g_pub_tx.reply, "F033" );
				sprintf(acErrMsg,"删除客户账号服务品种出错!!![%s]", fldstr[i] );
				WRITEMSG
				goto ErrExit;
			}
		}
	}

OkExit:
	sprintf(acErrMsg,"客户短信服务品种维护成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"客户短信服务品种维护失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
