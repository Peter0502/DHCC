/*************************************************
* 文 件 名:    sp9714.c
* 功能描述：   
*              根据柜员和币种回显相关信息[现金调剂交易]
*
* 作    者:    mike
* 完成日期：   2003年03月15日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "com_tel_connect_c.h"

sp9714()  
{ 		
	int i=0,ret=0,ac_seqn;
	char tmp_ind[2], ac_no[20],teller[6];

	struct  com_tel_connect_c   g_com_tel_connect;

	memset(&g_com_tel_connect,0x00,sizeof(struct com_tel_connect_c));

	/*系统初始化（初始化g_pub_tx)数据初始化*/
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("0670",tmp_ind);							/*调剂方式*/
	get_zd_data("0210",g_pub_tx.cur_no);					/*币种*/
	get_zd_data("0920",teller);								/*对方柜员号*/

	if(tmp_ind[0]=='2')
	{
 		ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
								"tel='%s' and tw_tel='%s' and cur_no='%s' \
								and ind='%s'", teller,g_pub_tx.tel, \
								g_pub_tx.cur_no,"A");
 		if( ret==100 )
 		{
	 		sprintf(acErrMsg,"对方柜员没有该笔付出!!tel='%s' and tw_tel='%s' and cur_no='%s' and ind='%s'", teller,g_pub_tx.tel, g_pub_tx.cur_no,"0");
	 		WRITEMSG
	 		strcpy(g_pub_tx.reply,"O054");
	 		goto ErrExit;
 		}

		set_zd_double("0410",g_com_tel_connect.amt);
	}
	else if(tmp_ind[0]=='3')
	{
		ret=Com_tel_connect_Sel(g_pub_tx.reply,&g_com_tel_connect, \
		  						"tel='%s' and tw_tel='%s' and cur_no='%s' \
		   						and ind='%s'", g_pub_tx.tel,teller, \
								g_pub_tx.cur_no,"A");
		if( ret==100 )
		{
		  	sprintf(acErrMsg,"没有付出过该笔现金!!tel='%s' and tw_tel='%s' \
		and cur_no='%s' and ind='%s'", g_pub_tx.tel,teller,g_pub_tx.cur_no,"0");
		   WRITEMSG
		   strcpy(g_pub_tx.reply,"O044");			/*改!!*/
		   goto ErrExit;
		}

	   set_zd_double("0410",g_com_tel_connect.amt);
	}
	else 
		set_zd_double("0410",0.00);

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
   
