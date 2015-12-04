/*************************************************
* 文 件 名:    sp9739.c
* 功能描述：   
*              根据介质种类返回支取方式
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
#include "mdm_attr_c.h"

sp9739()  
{ 		
	int ret=0;
	char mdm_code[5];

	struct mdm_attr_c g_mdm_attr;

	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/** 取值、赋值 **/	
	get_zd_data("1161",mdm_code);					/*介质种类*/
	
	/* 根据介质种类取相关参数*/
	ret=Mdm_attr_Sel(g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'",mdm_code);
  	if( ret==100 )
	{
	  	sprintf(acErrMsg,"不存在该凭证种类!!mdm_code=[%s]",mdm_code);
	  	WRITEMSG
		strcpy(g_pub_tx.reply,"H009");
		goto ErrExit;
	}

	/* 输出变量 */
	if(g_mdm_attr.draw_pwd_yn[0]=='O')
		strcpy(g_mdm_attr.draw_pwd_yn,"N");

	if(g_mdm_attr.draw_id_yn[0]=='O')
		strcpy(g_mdm_attr.draw_id_yn,"N");

	if(g_mdm_attr.pwd_mach_yn[0]=='O')
		strcpy(g_mdm_attr.pwd_mach_yn,"N");

	set_zd_data("1155", g_mdm_attr.draw_id_yn);
	set_zd_data("1159", g_mdm_attr.qry_pwd_yn);
	set_zd_data("1152", g_mdm_attr.draw_pwd_yn);
	set_zd_data("115A", g_mdm_attr.pwd_mach_yn);

	sprintf(acErrMsg,"AZZ=================g_mdm_attr.no_ind[%s]", \
			g_mdm_attr.no_ind);
	WRITEMSG
	set_zd_data("1169", g_mdm_attr.no_ind);			/*介质种类*/

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
