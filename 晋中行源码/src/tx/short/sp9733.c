/*************************************************
* 文 件 名: sp9733.c
* 功能描述：根据帐号回显信息___[销介质交易]
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
#include "prdt_ac_id_c.h"
#include "mdm_attr_c.h"

sp9733( )  
{ 		
	int ret=0;
	double bal;
	char ac_no[20],note_no[20],name[51];
	
	struct prdt_ac_id_c g_prdt_ac_id;
	struct mdm_attr_c g_mdm_attr;

	/** 取值、赋值 **/	
	get_zd_data("1051",ac_no);					/*帐号*/
	pub_base_old_acno( ac_no );                 /**对旧帐号的处理**/

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"产品帐号对照表不存在该记录!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"查找出错!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='*')
	{
		 sprintf(acErrMsg,"该介质已销!");
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M012");
		 goto ErrExit;
	}

	ret = Mdm_attr_Sel( g_pub_tx.reply,&g_mdm_attr,"mdm_code='%s'", \
					 	g_mdm_ac_rel.mdm_code);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"介质帐号对照表不存在该记录!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"查找出错!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	ret = Prdt_ac_id_Sel( g_pub_tx.reply,&g_prdt_ac_id,"ac_id=%ld", \
						   g_mdm_ac_rel.ac_id);
	if( ret==100 )
	{
		 sprintf(acErrMsg,"产品帐号对照表不存在该记录!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"M003");
		 goto ErrExit;
	}
	else if( ret )
	{
		 sprintf(acErrMsg,"查找出错!![%d]",ret);
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	}

	switch(g_prdt_ac_id.prdt_no[0])
	{
		case '1': 
				ret = Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
								 g_mdm_ac_rel.ac_id);
				if( ret==100 )
				{
		 			sprintf(acErrMsg," 账号不存在![%ld]",g_mdm_ac_rel.ac_id );
		 			WRITEMSG
		 			strcpy(g_pub_tx.reply,"M003");
		 			goto ErrExit;
				}
				else if( ret )
				{
		 			sprintf(acErrMsg,"查找出错!![%d]",ret);
					WRITEMSG
		 			strcpy(g_pub_tx.reply,"D103");
		 			goto ErrExit;
				}
				strcpy(name,g_dd_mst.name);
				break;
		case '2':
				ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
								 g_mdm_ac_rel.ac_id);
				if( ret==100 )
				{
		 			sprintf(acErrMsg,"账号不存在!![%ld]",g_mdm_ac_rel.ac_id );
		 			WRITEMSG
		 			strcpy(g_pub_tx.reply,"M003");
		 			goto ErrExit;
				}
				else if( ret )
				{
		 			sprintf(acErrMsg,"查找出错!![%d]",ret);
					WRITEMSG
		 			strcpy(g_pub_tx.reply,"D103");
		 			goto ErrExit;
				}
				strcpy(name,g_td_mst.name);
				break;
		default :
		 		sprintf(acErrMsg,"非存取款类帐户!![%s][%s]", \
						g_prdt_ac_id.prdt_no, ac_no);
		 		WRITEMSG
		 		strcpy(g_pub_tx.reply,"O166");
		 		goto ErrExit;
				break;
	}

	set_zd_data("105B",name);						/*户名*/
	set_zd_data("1051",ac_no);						/*账号*/
	set_zd_data("1023",g_mdm_attr.note_type);		/*凭证种类*/
	set_zd_data("1024",g_mdm_ac_rel.note_no);		/*凭证号码*/

	set_zd_data("1054",g_mdm_ac_rel.draw_pwd_yn);	/*是否密码支取*/
	set_zd_data("1057",g_mdm_ac_rel.draw_id_yn);	/*是否证件支取*/

	if(g_mdm_ac_rel.draw_id_yn[0]=='Y')
	{
		set_zd_data("105A",g_mdm_ac_rel.id_type);  /*证件类型*/
		set_zd_data("1058",g_mdm_ac_rel.id_no);    /*证件号码*/
	}

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
   
