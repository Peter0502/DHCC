/*************************************************
* 文 件 名:    sp9704.c
* 功能描述：   
*              根据帐号回显账户序号[取款交易]
*
* 作    者:    rob
* 完成日期：   2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9704()  
{ 		
	int ret=0,ac_seqn;
	char ac_no[20];

	/** 取值、赋值 **/	
	get_zd_data("1021",ac_no);						/*帐号*/
	
	/* 调用根据帐号返回账户序号函数 */
	ret=pub_base_actoseqn(ac_no,&ac_seqn);			
	if(ret)
	{
		sprintf( acErrMsg, "调用函数pub_base_actoseqn出错!!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}


	/* 输出变量 */
	set_zd_int("1022",ac_seqn);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
   
