/*************************************************
* 文 件 名:    sp9741.c
* 功能描述：   
*              根据帐号回显帐户序号
*
* 作    者:    lance
* 完成日期：   2003年03月14日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "cif_basic_inf_c.h"
#include "prdt_ac_id_c.h"

sp9741()  
{ 		
	int ret=0,ac_seqn;
	char title[40];
	char ac_no[20];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);							/*帐号*/
	if(pub_base_old_acno( ac_no ))                 /**对旧帐号的处理**/
		goto ErrExit;

	/*根据帐号找到账户序号*/
	pub_base_actoseqn(ac_no,&ac_seqn);
TRACE

    set_zd_int("0340",ac_seqn);                         /*序号*/
    set_zd_data("0300",ac_no);                          /*帐号*/

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

