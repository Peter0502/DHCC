/*************************************************
* 文 件 名:  spJ098.c
* 功能描述： 小额来账 挂帐检查 子交易
*			       
*			 
* 作    者:  ChenMing
* 完成日期： 2006年8月30日
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
#include "lv_pub.h"
#include "lv_define.h"

int spJ098()
{
	long iSubpackIndex=0;
    char cTx_br_no[BRNO_LEN+1];
	pub_base_sysinit();
    
    memset(cTx_br_no,0,sizeof(cTx_br_no));
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("spJ098() 小额来账 挂帐检查 子交易 started! [%ld]",iSubpackIndex);
	vLvBrnoDebug(__FILE__,__LINE__);
	g_reply_int=pubf_lv_recv_check_main();
	if(g_reply_int==9999)
	{
			vtcp_log("%s,%d 这里判断是否可以接受来帐"__FILE__,__LINE__);
			strncpy(g_pub_tx.reply,"P046",4);
			WRITEMSG
			goto ErrExit; 
	}
	else if (g_reply_int) /* 调用 小额来帐挂帐检查 主函数 */
	{
		/*挂帐检查失败*/ 
		set_zd_data("0500","1");       /* 报文处理完毕 */ 
		strcpy( g_pub_tx.reply,"0060" );
		vLvBrnoDebug(__FILE__,__LINE__);
		goto ErrExit; 
	}
    get_zd_data("0030",cTx_br_no);
  iGetReceTel(cTx_br_no);    /*  柜员号 ==> 0070 */
	vLvBrnoDebug(__FILE__,__LINE__);
OkExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ098() 小额来账 检查帐务 子交易结束 end! [%ld]",__FL__,iSubpackIndex);
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	get_zd_long("0520",&iSubpackIndex);
	vtcp_log("%s,%d spJ098() 小额来账 检查帐务 子交易结束 end! [%ld]",__FL__,iSubpackIndex);
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

