/*************************************************
* 文 件 名:  spJ999.c
* 功能描述： 大额来账 登记来帐 子交易
*			       拆报文 登记来帐表
*			 
* 作    者:  ChenMing
* 完成日期： 2006年8月17日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "find_debug.h"

int spJ999()
{
	char chkcmtno[4];
	char txtime[7];	
	
	vtcp_log(" --- 大额来账 登记来帐 开始 --- ");
	pub_base_sysinit();
	
	memset(chkcmtno,'\0',sizeof(chkcmtno));
	memset(txtime,'\0',sizeof(txtime));
	
  get_zd_data("0510",chkcmtno);	
  get_zd_data("0060",txtime);	

	vtcp_log(" --- chkcmtno =[%s] ",chkcmtno);	
	vtcp_log(" --- txtime =[%s] ",txtime);	 
 

	if (pub_hv_recv_reg_main(chkcmtno)) 
	  goto ErrExit; /* 调用 大额来帐登记来帐 主函数 */
	
	/* strcpy( g_pub_tx.reply,"0000" ); */
		
OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

