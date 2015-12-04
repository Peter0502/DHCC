/*************************************************
* 文 件 名:    sp3468.c
* 功能描述：   
*          支付宝交易返回渠道交易的核心日期及流水
* 作    者:    wjp
* 完成日期：   2013年01月31日

insert into tx_def values('3468','支付宝返回流水','00000000000000000000000000001000000000000001100000000000000000000000000000000000000000000000000000000000000000000000000000000100','1','4','0','','');
insert into tx_flow_def values('3468','0','3468','#$');
insert into tx_sub_def values('3468','支付宝返回流水','sp3468','0','0');
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#include "string.h"
#define EXTERN
#include "public.h"
#include "in_mst_hst_c.h"

int sp3468()
{
	int  ret = 0;
	long max_trace=0;
	long max_date=0;
	struct in_mst_hst_c in_mst_hst;

	/** 初始化全局变量 **/
	pub_base_sysinit();

	/** 取值、赋值 **/
	memset(&in_mst_hst,0x00,sizeof(in_mst_hst));
	ret=sql_max_long_xier("in_mst_hst","tx_date",&max_date,"ac_id=900004617");
  if(ret)
  {
    sprintf(acErrMsg,"取最大日期错误iRet=[%d]",ret);
    WRITEMSG
    goto ErrExit;
  }
	
sprintf(acErrMsg,"xx取最大日期[%ld]",max_date);
    WRITEMSG
	
	ret=sql_max_long_xier("in_mst_hst","trace_no",&max_trace,"ac_id=900004617 and tx_date=%ld",max_date);
  if(ret)
  {
    sprintf(acErrMsg,"取最大流水号错误iRet=[%d]",ret);
    WRITEMSG
    goto ErrExit;
  }
	set_zd_long("0440",max_date);
	set_zd_long("0450",max_trace);
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
