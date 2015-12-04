/*************************************************
* 文 件 名: sp9710.c
* 功能描述: 根据帐号回显相关信息___更换介质专用
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
#include "cif_basic_inf_c.h"
#include "cif_id_code_rel_c.h"
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9710()  
{ 		
	int ret=0;
	long cif_no;
	char tmp_note_no[20],ac_no[20],note_no[20];
	char name[51];
	char tag[2];

	struct cif_basic_inf_c g_cif_basic_inf;
	struct cif_id_code_rel_c g_cif_id_code_rel;
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	memset(&g_cif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));
	memset(&g_cif_id_code_rel,0x00,sizeof(struct cif_id_code_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(&g_mdm_attr,0x00,sizeof(struct mdm_attr_c));

	/** 取值、赋值 **/	
	get_zd_data("0670",tag);					/*** 换证类型 ***/
	get_zd_data("0300",ac_no);					/*** 帐号 ***/
	pub_base_old_acno( ac_no );                 /*** 对旧帐号的处理 ***/
	
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该帐号!!ac_no=[%ld]",ac_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已作销户处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"该介质已作挂失换证处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"该介质已作挂失处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已作部提换证处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='5' )
	{
		sprintf(acErrMsg,"该介质已作撤销处理!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M012");
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
  set_zd_data( "0250",name );
  vtcp_log("[%s][%d] 客户name==[%s]",__FILE__,__LINE__,name);
	strcpy(note_no,g_mdm_ac_rel.note_no);
	
	/*** 读取介质属性 ***/
	ret = Mdm_attr_Sel(&g_pub_tx.reply,&g_mdm_attr," mdm_code='%s' ", \
						g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"该介质属性不存在![%d][%s]",
		 		g_reply_int,g_pub_tx.mdm_code);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"P132");
		 goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找错误");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	 /*如果是卡类：需要输入卡号并且回显凭证号*/
	if( g_mdm_attr.no_ind[0]=='Y' )
	{
		ret = pub_card_CardToNote(ac_no,note_no);
	}
	else if( tag[0]!='1' )
	{;
	}
	else
	{
		set_zd_data("0310",ac_no);
	}

	/* 输出变量 */
	set_zd_data("1142",g_mdm_ac_rel.draw_pwd_yn);
	set_zd_data("1145",g_mdm_ac_rel.draw_id_yn);
	set_zd_data("1148",g_mdm_ac_rel.id_type);
	/**返回证件号码-wangwenkui-20110113**/
	set_zd_data("0370",g_mdm_ac_rel.id_no);
	/**end**/
	set_zd_data("1168",note_no);
	set_zd_data("1161",g_mdm_ac_rel.mdm_code);
	
	/*set_zd_data( "0250",name );*/
	 vtcp_log("[%s][%d] 客户name==[%s]",__FILE__,__LINE__,name);
	set_zd_data( "0300",ac_no );

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
 
