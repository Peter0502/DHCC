/*************************************************
* 文 件 名:    sp9738.c
* 功能描述：   
*              根据卡号返回凭证号码
*
* 作    者:    
* 完成日期：   2003年01月13日
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
#include "cif_id_code_rel_c.h"

sp9738()  
{ 		
	int ret=0;
	char tmp_str1[20],tmp_str[20];
	char card_no[20];

	memset(tmp_str,0,sizeof(tmp_str));
	memset(tmp_str1,0,sizeof(tmp_str1));

	/** 取值、赋值 **/	
	get_zd_data("1162",card_no);					/*卡号*/
	
	/* 根据证件类型和证件号码取客户号 */
	pub_card_CardToNote(card_no,tmp_str1);

	/* 输出变量 */
	set_zd_data("1168", tmp_str1);

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
   
