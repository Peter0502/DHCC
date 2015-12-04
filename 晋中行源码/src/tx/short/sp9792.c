/*************************************************************
* 文 件 名:  sp9792.c
* 功能描述： 收费交易_客户账号回显户名
* 作    者:  rob
* 完成日期： 2003年6月9日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

int sp9792()
{
	int ret=0;

	struct prdt_ac_id_c prdt_ac_id_c;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("1221",g_pub_tx.ac_no);
	pub_base_old_acno( g_pub_tx.ac_no );               /**对旧帐号的处理**/
	set_zd_data("1221", g_pub_tx.ac_no);
		
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if(ret==100)
	{
		sprintf( acErrMsg, "账号不存在!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	/*** 卡的情况 ***/
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, \
							"ac_id=%ld and ac_seqn=1", g_mdm_ac_rel.ac_id);
		if( ret==100 )
		{
	 		sprintf(acErrMsg,"该卡内不存在活期户,不允许收费!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O226");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"查找错误! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}
	else
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld", \
							  g_mdm_ac_rel.ac_id);
		if(ret)
		{
	 		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
		else if( ret )
		{
	 		sprintf(acErrMsg,"查找错误! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
			goto ErrExit;
		}
	}

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld and ac_seqn=1", \
						 prdt_ac_id_c.ac_id);
		if( ret )
		{
	 		sprintf(acErrMsg,"产品帐号对照表不存在该记录!![%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}

		if(g_dd_mst.ac_sts[0]=='*')	
		{
			sprintf(acErrMsg,"该账号已销户!! [%d]",ret);
	 		WRITEMSG
	  		strcpy(g_pub_tx.reply,"P245");
	   		goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"该账号不能作收费交易!! [%s]",prdt_ac_id_c.prdt_no);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"O192");
   		goto ErrExit;
	}
	vtcp_log("ssssssssssssssss=[%s]",g_dd_mst.name);
	set_zd_data("122C", g_dd_mst.name);
	set_zd_data("1228", g_mdm_ac_rel.draw_pwd_yn);
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
