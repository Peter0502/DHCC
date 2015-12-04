/*************************************************
* 文 件 名: sp9130.c
* 功能描述：修改密码发送短信
* 作    者: 
* 完成日期：
* 修改记录：   
* 1. 日    期:2011-01-26
*    修 改 人:王俊鹏
*    修改内容:完成上行的短信银行注销功能
* 2. 
*交易配置：
insert into tx_def values('9130','修改密码发送短信','10000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000','1','6','0');
insert into tx_flow_def values('9130','0','9130','#$');
insert into tx_sub_def values('9130','修改密码发送短信','sp9130','0','0');
*************************************************/
#include <string.h>

#define EXTERN
#include "public.h"
#include "mob_sendmail_c.h"
#include "com_sys_parm_c.h"
#include "mob_acct_type_c.h"
#include "mdm_ac_rel_c.h"

int sp9130()
{ 
	char cSh_no[100]={0};
  int ret=0;
  char cMessage[257]={0}; 
  long date;

  
  struct mob_sendmail_c sMob_sendmail;
  struct com_sys_parm_c sCom_sys_parm;
  struct mob_acct_type_c sMob_acct_type;
  struct mdm_ac_rel_c sMdm_ac_rel;
  

	ret = sql_begin(); 
	if (ret)
	{
		sprintf(acErrMsg, "打开事务失败!!!");
		WRITEMSG
		goto ErrExit;
	}
	
	memset(&sMob_sendmail,0x0,sizeof(sMob_sendmail));
	memset(&sCom_sys_parm,0x0,sizeof(sCom_sys_parm));
	memset(&sMob_acct_type,0x00,sizeof(sMob_acct_type));
	memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
	
	ret= Com_sys_parm_Sel(g_pub_tx.reply,&sCom_sys_parm,"1=1");
	if(ret)
	{
		vtcp_log("[%s][%d]得到系统日期错![%d]\n",__FILE__,__LINE__,ret);
		goto ErrExit;
	}
	set_zd_long("0050",sCom_sys_parm.sys_date);
	if ( pub_base_sysinit() )
	{
		sprintf(acErrMsg,"初始化公用结构错误!!!");
		WRITEMSG
		goto ErrExit;
	}	

	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG
	get_zd_data("0300",cSh_no);   
	get_zd_data("1250",cMessage);
	
	vtcp_log("%s,%d dddd 账号=[%s]",__FILE__,__LINE__,cSh_no);
	ret= Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",cSh_no);
	if(ret)
	{
		vtcp_log("[%s][%d]查询账户ac_id出错![%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"D104");
		goto ErrExit;
	}
	sMob_sendmail.ac_id = sMdm_ac_rel.ac_id;
	strcpy( sMob_sendmail.tx_code,g_pub_tx.tx_code);
	strcpy( sMob_sendmail.ac_no,cSh_no);
	strcpy( sMob_sendmail.tel, g_pub_tx.tel );
	sMob_sendmail.trace_no = g_pub_tx.trace_no;
	sMob_sendmail.tx_date = g_pub_tx.tx_date;
	sMob_sendmail.tx_time = g_pub_tx.tx_time;
	strcpy( sMob_sendmail.opt_no, "110" );
	/*
	strcpy(sMob_sendmail.mobile,cSh_no);
	*/
	strcpy( sMob_sendmail.message, cMessage );
	strcpy( sMob_sendmail.timeflag, "0" );
	sMob_sendmail.t_date = 0;
	sMob_sendmail.t_time = 0;
	strcpy( sMob_sendmail.send_level, "0" );
	sMob_sendmail.sene_times = 1;
	sMob_sendmail.send_date = 0;
	sMob_sendmail.send_time = 0;
	sMob_sendmail.sxf_amt = 0.00;
	strcpy(sMob_sendmail.proc_flag, "0" );
	sMob_sendmail.mobseq = 0;
	strcpy( sMob_sendmail.tx_type, "2" );  
	ret = Mob_acct_type_Dec_Sel(g_pub_tx.reply,"ac_no = '%s'",cSh_no);
	if(ret != 0 )
	{
		sprintf(acErrMsg,"Mob_acct_type表准备语句错误【%d】",ret);
		strcpy(g_pub_tx.reply,"D102");
		WRITEMSG
		goto ErrExit;
	}

	ret = Mob_acct_type_Fet_Sel(&sMob_acct_type,g_pub_tx.reply);
	if(ret)
	{
			vtcp_log("[%s][%d]查询mob_acct_type表出错![%d]\n",__FILE__,__LINE__,ret);
			strcpy(g_pub_tx.reply,"D102");
			goto ErrExit;
	}
	strcpy(sMob_sendmail.mobile,sMob_acct_type.mobile);
	Mob_sendmail_Debug(&sMob_sendmail);
	ret = Mob_sendmail_Ins( sMob_sendmail, g_pub_tx.reply );
	if (ret)
	{
		sprintf( acErrMsg, "写mob_sendmail表失败!!![%d]", ret );
		WRITEMSG
		strcpy(g_pub_tx.reply,"MT09");
		goto ErrExit;
	}
	
OkExit:
	sql_commit();
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"短信银行信息维护成功！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sql_rollback();
	sprintf(acErrMsg,"短信银行信息维护失败！[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
}

