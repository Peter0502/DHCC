/*************************************************
* 文 件 名: gDX10.c
* 功能描述：贷款本金到期提醒,贷款本金到期前3天 短信提醒，
*	提醒模版	贵单位合同号#7#的借款于#22#到期,请及时通过网银或到营业网点办理贷款还本,借款逾期后人行征信系统将显示违约记录.#6#
*	
*
* 作    者: zgf
* 完成日期: 2014年06月05日
* 修改日期:
* 修 改 人:
* 修改内容:
* insert into com_eod_exec values(9009,'7779','gDX10','贷款本金到期前3天','1111111111','0','0','00','00','2','Y','Y','N','2','Y');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "mob_acct_type_c.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"

gDX10()
{
	int ret=0;

	char cMessage[141];
	char tmpacno[20];
	char pact_no_tmp[20];
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
	memset(tmpacno,0x00,sizeof(tmpacno));
	memset(pact_no_tmp,0x00,sizeof(pact_no_tmp));
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

	strcpy(g_pub_tx.tx_code,"7779");
	
	/*-----------翻盘后统计,取上日日期-------------*/
	ret = pub_base_end_month( sCom_sys_parm.lst_date );

	/*---------------------统计贷款到期------------------------*/
	/*g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply," ac_type in('3') and mob_sts='1' ");	*/
	g_reply_int=Mob_acct_type_Dec_Sel1(g_pub_tx.reply,"  mob_sts='1' and filler = '1' order by ac_no");  /*mob by lwb 20150408 按还款账号查*/
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
		/***批量处理时，对于同一笔合同只循环一次***/
		else
		{
			if(memcmp(sMob_acct_type.ac_no,tmpacno,strlen(sMob_acct_type.ac_no))==0)
			{				
				continue;
			} 
		}
		memset(tmpacno,0x00,sizeof(tmpacno));
		memcpy(tmpacno,sMob_acct_type.ac_no,strlen(sMob_acct_type.ac_no));
	
		pub_base_deadlineD(sCom_sys_parm.lst_date,3,&note_day);
		
		vtcp_log("[%s][%d]dddddddddddddddddddd==[%ld]",__FILE__,__LINE__,sCom_sys_parm.lst_date);
		vtcp_log("[%s][%d]dddddddddddddddddddd==[%ld]",__FILE__,__LINE__,note_day);
		memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
		ret = Ln_mst_Dec_Sel2( g_pub_tx.reply , "repay_ac_id=%ld and \
			mtr_date = %ld and ac_sts<>'*' and bal<>0", sMob_acct_type.ac_id, note_day );
		if ( ret )
		{
			sprintf( acErrMsg, "--->定义贷款[%s]主文件查询游标出错[%d]!!", \
				sMob_acct_type.ac_no, ret );
			WRITEMSG
			Mob_acct_type_Clo_Sel1();
			goto ErrExit;	
		}
		while (1)
		{
			memset( &sLn_mst, 0x0, sizeof(sLn_mst) );
			ret = Ln_mst_Fet_Sel2( &sLn_mst, g_pub_tx.reply );
			if ( ret == 100 )
			{
				break;
			}
			else if ( ret )
			{
				sprintf(acErrMsg, "[%s][%d]DECLARE FOR FETCH ERROR [%d]",__FILE__,__LINE__, g_reply_int);
				WRITEMSG
				Ln_mst_Clo_Sel2();	
				Mob_acct_type_Clo_Sel1();
				goto ErrExit;
			}
			strncpy(pact_no_tmp,sLn_mst.pact_no,strlen(sLn_mst.pact_no)-3);
			pub_mob_sendmail_ln("***","7779",pact_no_tmp,"1",0.00,0.00,0.00);	
		}
		
		Ln_mst_Clo_Sel2();
	}

	Mob_acct_type_Clo_Sel1();


OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->贷款本金到期前1个月发送信息成功!!!" );
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	sql_rollback();
    sprintf(acErrMsg,"--->贷款本金到期前1个月发送信息失败[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
