/*************************************************
* 文 件 名: sp9990.c
* 功能描述：根据帐号回显帐户支取信息_[账户余额查询]
* 作    者: rob
* 完成日期：2003年08月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9990()  
{ 		
	int ret=0;
	char ac_no[20];
	long cif_no;
	char name[51];
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);						/** 帐号 **/
	pub_base_old_acno( ac_no );  					/** 新旧转换 **/
	set_zd_data("0300",ac_no);						/** 处理旧帐号 **/
	
	/* 调用根据帐号返回账户序号函数 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no );
	if( ret==100 )
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找失败! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*** 找客户名 ***/
	if( pub_base_acgetname( g_mdm_ac_rel.ac_no, &cif_no, name ) )
	{
		sprintf(acErrMsg,"查找客户号、户名失败!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D249");
		goto ErrExit;
	}

	set_zd_data("0250",name);						/*户    名*/
	set_zd_data("0670",g_mdm_ac_rel.draw_pwd_yn);	/*密码支取*/
	set_zd_data("0680",g_mdm_ac_rel.draw_id_yn);	/*证件支取*/
	set_zd_data("0690",g_mdm_ac_rel.id_type);		/*证件类型*/

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
   
