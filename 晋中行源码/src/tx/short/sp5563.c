/*************************************************
* 文 件 名: sp5563.c 
* 功能描述：根据流水号查询记账交易摘要。
*
* 作    者: 侯毅
* 完成日期：20090820
insert into tx_def values('5563','由流水查记账交易摘要','10000000000000000000000000000000000000000001000000000000000000000000000000000000001000000000000000000000000000000000000000000000','0','4','1','3',0);
insert into tx_flow_def values('5563',0,'5563','#$');
insert into tx_sub_def  values('5563','由流水查记账交易摘要','sp5563','0','0');
* 修  改 者:林冬燕
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "trace_log_c.h"

sp5563()
{
	int  iRet=0;
	long int  iTtno=0;/**柜员流水号**/
	struct trace_log_c tmp_trace_log;
	
	memset(&tmp_trace_log,'\0',sizeof(tmp_trace_log));
	pub_base_sysinit();
	get_zd_long("0440",&iTtno);
 
	iRet=Trace_log_Sel(g_pub_tx.reply,&tmp_trace_log,"tx_date=%ld and tel='%s' and trace_no=%ld ",g_pub_tx.tx_date,g_pub_tx.tel,iTtno);
	if(iRet==100)
	{		
			vtcp_log("[%s][%d]业务流水中无此流水号\n",__FILE__,__LINE__);
			goto ErrExit;
	}
	else if(iRet)
	{
			vtcp_log("[%s][%d]取业务流水异常出错[%d]\n",__FILE__,__LINE__,iRet);
			strcpy(g_pub_tx.reply,"P298");
			goto ErrExit;
	}
	set_zd_data("0830",tmp_trace_log.brf);/**获得摘要**/
	
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	set_zd_data("0120",g_pub_tx.reply);
	return iRet;
}
