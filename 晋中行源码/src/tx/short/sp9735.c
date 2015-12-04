/*************************************************
* 文 件 名: sp9735.c
* 功能描述：根据密码的正确性返回标志_[支取信息维护交易]
* 作    者: rob
* 完成日期：2003年02月19日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9735()  
{ 		
	int ret=0;
	char ac_no[20],tmp_pass[7];
	
	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	pub_base_sysinit();

	/** 取值、赋值 **/	
	get_zd_data("0300",ac_no);						/*帐号*/
	get_zd_data("0790",tmp_pass);					/*密码*/

	/* 调用根据帐号返回账户序号函数 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel, "ac_no='%s'",ac_no);
	if(ret==100)
	{
		sprintf(acErrMsg,"介质帐号对照表不存在该记录!! [%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	strcpy(g_pub_tx.ac_no,ac_no);
	strcpy(g_pub_tx.draw_pwd,tmp_pass);

	/** 为解决证件支取户下面检查证件的问题，这里修改为非证件支取 **/
	g_mdm_ac_rel.draw_id_yn[0] = 'N';

	if( !strlen(tmp_pass) )
	{
		sprintf( acErrMsg, "原口令输入不正确!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}
	if( pub_base_check_draw("001") )
	{
		sprintf( acErrMsg, "原口令输入不正确!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		/* strcpy(g_pub_tx.reply,"O157"); */
		goto ErrExit;
	}

	pub_base_EnDes(g_pub_tx.tx_date,"",tmp_pass);
    if( strcmp(tmp_pass,g_mdm_ac_rel.draw_pwd) )
	{
		sprintf( acErrMsg, "原口令输入不正确!!tmp_pass=[%s], \
				g_mdm_ac_rel.draw_pwd=[%s]",tmp_pass,g_mdm_ac_rel.draw_pwd );
		WRITEMSG
		strcpy(g_pub_tx.reply,"O157");
		goto ErrExit;
	}

	/* 输出变量 */
	set_zd_data("0710","1");

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
   
