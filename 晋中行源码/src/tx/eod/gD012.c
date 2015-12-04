/*************************************************
* 文 件 名: gD012.c
* 功能描述：翻盘后短信自动统计信息(月末余额,定期到期通知,
			贷款到期通知)
*
* 作    者: jack
* 完成日期: 2004年07月08日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"

gD012()
{
	int ret=0;

	char cMessage[141];

	struct mdm_ac_rel_c sMdm_ac_rel;
	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct td_parm_c sTd_parm;
	struct ln_mst_c sLn_mst;
	struct ln_parm_c sLn_parm;
	struct mob_acct_type_c sMob_acct_type;
	struct mob_sendmail_c sMob_sendmail;
	struct com_sys_parm_c sCom_sys_parm;

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

	/*-----------翻盘后统计,取上日日期-------------*/
	ret = pub_base_end_month( sCom_sys_parm.lst_date );
	if ( ret == 0 )
	{
		/*--------------统计活期月末余额-------------*/
		ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, \
				"server_type='1003'" );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Mob_acct_type_Dec_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}
        	
		while (1)
		{
			memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
			ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf( acErrMsg, "执行Mob_acct_type_Fet_Sel错[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}

			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_no = '%s'", sMob_acct_type.ac_no );
			if ( ret )
			{
				sprintf( acErrMsg, "--->读取账户[%s]介质表出错[%d]!!", \
					sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;	/*处理不成功则跳过*/
			}

			memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
			ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
				"ac_id = %ld", sMdm_ac_rel.ac_id );
			if ( ret )
			{
				sprintf( acErrMsg, "--->读取活期[%s]主文件出错[%d]!!", \
					sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;	/*处理不成功则跳过*/
			}

			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );

			sprintf( cMessage, "尊敬的客户:您的账户%s月末活期余额为%.2lf元", \
				sMob_acct_type.ac_no, sDd_mst.bal );

			strcpy( sMob_sendmail.tx_code, "自动" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			sMob_sendmail.ac_id = sMob_acct_type.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
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
			if ( ret ) 
			{
				sprintf( acErrMsg, "写账号[%s]记录mob_sendmail\
					表失败!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*处理不成功则继续*/
			}
				
		}

		Mob_acct_type_Clo_Sel();
	}

	/*---------------------统计定期到期------------------------*/
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, "server_type='1004'" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_acct_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sMob_acct_type.ac_no );
		if ( ret )
		{
			sprintf( acErrMsg, "--->读取账户[%s]介质表出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	/*处理不成功则跳过*/
		}

/*-----多条记录------------------------------------------------
		memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
		ret = Td_mst_Sel( g_pub_tx.reply, &sTd_mst, \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->读取定期[%s]主文件出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
--------------------------------------------------------------*/

		ret = Td_mst_Dec_Sel( g_pub_tx.reply , \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->定义定期[%s]主文件查询游标出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
		
		while (1)
		{
			memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
			ret = Td_mst_Fet_Sel( &sTd_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				continue;	/*处理不成功则跳过该条记录*/
			}
			
			memset( &sTd_parm, 0x0, sizeof(sTd_parm) );
			ret = Td_parm_Sel( g_pub_tx.reply, &sTd_parm,  \
				"prdt_no = '%s'", sTd_mst.prdt_no );
			if ( ret )
			{
				continue;
			}
			pub_base_strpack( sTd_parm.title );

			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );
                	
			sprintf( cMessage, "尊敬的客户:您的%s账号为%s已到期\
				余额为%.2lf元", sTd_parm.title, sMob_acct_type.ac_no, sTd_mst.bal );
                	
			strcpy( sMob_sendmail.tx_code, "自动" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			sMob_sendmail.ac_id = sMob_acct_type.ac_id;
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
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
			if ( ret )
			{
				sprintf( acErrMsg, "写账号[%s]记录mob_sendmail\
					表失败!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*处理不成功则继续*/
			}
		}
		Td_mst_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel();

	/*---------------------统计贷款到期------------------------*/
	ret = Mob_acct_type_Dec_Sel( g_pub_tx.reply, "server_type='1005'" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_acct_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sMob_acct_type, 0x0, sizeof(sMob_acct_type) );
		ret = Mob_acct_type_Fet_Sel( &sMob_acct_type, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Mob_acct_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sMob_acct_type.ac_no );
		if ( ret )
		{
			sprintf( acErrMsg, "--->读取账户[%s]介质表出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	/*处理不成功则跳过*/
		}
/*-----多条记录------------------------------------------------
		memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
		ret = Ln_mst_Sel( g_pub_tx.reply, &sLn_mst, \
			"ac_id = %ld and mtr_date = %ld", \
			sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->读取定期[%s]主文件出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;
		}
-----------------------------------------------------------*/

		ret = Ln_mst_Dec_Sel( g_pub_tx.reply , "ac_id = %ld and \
			mtr_date = %ld", sMdm_ac_rel.ac_id, sCom_sys_parm.lst_date );
		if ( ret )
		{
			sprintf( acErrMsg, "--->定义贷款[%s]主文件查询游标出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			continue;	
		}
		
		while (1)
		{
			memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
			ret = Ln_mst_Fet_Sel( &sLn_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				continue;
			}
			
			memset( &sLn_parm, 0x0, sizeof(sLn_parm) );
			ret = Ln_parm_Sel( g_pub_tx.reply, &sLn_parm, \
				"prdt_no = '%s'", sTd_mst.prdt_no );
			if ( ret )
			{
				continue;
			}
		
			memset( cMessage, 0x0, sizeof(cMessage) );
			memset( &sMob_sendmail, 0x0, sizeof(sMob_sendmail) );
                	
			sprintf( cMessage, "尊敬的客户:您的%s账号%s已到\
				期余额为%.2lf元", sLn_parm.title, sMob_acct_type.ac_no, sLn_mst.bal );
                	
			strcpy( sMob_sendmail.tx_code, "自动" );
			strcpy( sMob_sendmail.ac_no, sMob_acct_type.ac_no );
			strcpy(	sMob_sendmail.tel, g_pub_tx.tel );
			sMob_sendmail.trace_no = g_pub_tx.trace_no;
			sMob_sendmail.tx_date = sCom_sys_parm.sys_date;
			sMob_sendmail.tx_time = g_pub_tx.tx_time;
			strcpy( sMob_sendmail.opt_no, "110" );
			strcpy( sMob_sendmail.mobile, sMob_acct_type.mobile );
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
			if ( ret ) 
			{
				sprintf( acErrMsg, "写账号[%s]记录mob_sendmail\
					表失败!!![%d]", sMob_acct_type.ac_no, ret );
				WRITEMSG
				continue;		/*处理不成功则继续*/
			}
			
		}
		Ln_mst_Clo_Sel();
	}

	Mob_acct_type_Clo_Sel();


OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->统计短信自动发送信息成功!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	sql_rollback();
    sprintf(acErrMsg,"--->统计短信自动发送信息失败[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
