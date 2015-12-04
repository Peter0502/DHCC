/*************************************************************
* 文 件 名: gDX04.c
* 功能描述: 电子对账提醒，每月1号。检索签约电子对账表记录 netbank_yqdz，若存在再检索短信
* 签约表记录mob_acct_type表，满足条件发送短信通知。
*
* 作    者: zgf
* 完成日期: 2014年6月20日
*
* 修改记录:
* 日    期:
* 修 改 人:
* 修改内容:
* insert into com_eod_exec values(9010,'7774','gDX04','1号电子对账提醒','1111111111','4','1','01','01','2','Y','Y','N','2','Y');
**************************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"
#include "netbank_yqdz_c.h"

gDX04()
{
	int ret=0;

	char cMessage[141];
	char tmpacno[20];
	long note_day;		/* 通知日期 */
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct ln_mst_c sLn_mst;
	struct ln_parm_c sLn_parm;
	struct mob_acct_type_c sMob_acct_type;
	struct mob_sendmail_c sMob_sendmail;
	struct com_sys_parm_c sCom_sys_parm;
	struct netbank_yqdz_c sNetbankYqdz;
	memset(tmpacno,0x00,sizeof(tmpacno));
	ret = sql_begin();
	if ( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** 初始化全局变量 **/
	pub_base_sysinit();

	memset( &sCom_sys_parm, 0x0, sizeof(sCom_sys_parm) );
	ret = Com_sys_parm_Sel( g_pub_tx.reply, &sCom_sys_parm, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "查询系统日期出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.tx_code,"7774");
	
	/*-----------翻盘后统计,取上日日期-------------*/
	ret = pub_base_end_month( sCom_sys_parm.lst_date );

	/*---------------------统计电子对账签约信息------------------------*/
	g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply," mob_sts='1' ");	
	if(g_reply_int)
	{
		sprintf(acErrMsg, "DECLARE FOR SELECT ERROR [%d]", g_reply_int);
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel1( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			Mob_acct_type_Clo_Sel1();
			goto ErrExit;
		}
		memset(tmpacno,0x00,sizeof(tmpacno));
		memcpy(tmpacno,sMob_acct_type.ac_no,strlen(sMob_acct_type.ac_no));
	
		memset( &sNetbankYqdz, 0x0, sizeof(sNetbankYqdz) );
		ret = Netbank_yqdz_Dec_Sel( g_pub_tx.reply , "ac_id = %ld ", sMob_acct_type.ac_id );
		if ( ret )
		{
			sprintf( acErrMsg, "--->定义Netbank_yqdz[%s]主文件查询游标出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			Netbank_yqdz_Clo_Sel();
			goto ErrExit;	
		}
		while (1)
		{
			memset( &sNetbankYqdz, 0x0, sizeof(sNetbankYqdz) );
			ret = Netbank_yqdz_Fet_Sel( &sNetbankYqdz, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
				WRITEMSG
				Netbank_yqdz_Clo_Sel();
				Mob_acct_type_Clo_Sel1();
				goto ErrExit;
			}
			pub_mob_sendmail_ln("***","7774",sMob_acct_type.ac_no,"1",0.00,0.00,0.00);	
		}
		
		Netbank_yqdz_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel1();


OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->1号电子对账提醒发送信息成功!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	sql_rollback();
    sprintf(acErrMsg,"--->1号电子对账提醒发送信息失败[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
