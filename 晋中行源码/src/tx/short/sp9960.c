/*************************************************************
* 文 件 名:  sp9960.c
* 功能描述： 2108收费交易_客户账号回显户名2
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

int sp9960()
{
	int ret=0;

	struct prdt_ac_id_c prdt_ac_id_c;

	memset(&g_mdm_ac_rel, 0x00, sizeof(struct mdm_ac_rel_c));

	pub_base_sysinit();

	get_zd_data("122D",g_pub_tx.ac_no);
	sprintf(acErrMsg, "老账号为:[%s]", g_pub_tx.ac_no);
	WRITEMSG
	pub_base_old_acno( g_pub_tx.ac_no );               /**对旧帐号的处理**/
	pub_base_strpack( g_pub_tx.ac_no );
	if( strlen(g_pub_tx.ac_no)==0 ) goto OkExit;

	sprintf(acErrMsg, "新账号为:[%s]", g_pub_tx.ac_no);
	WRITEMSG

	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &g_mdm_ac_rel, "ac_no='%s'", \
						  g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg, "账号不存在![%s]",g_pub_tx.ac_no );
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel错误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* jane于2003年7月27日添加 begin */
	/*根据账户id取得账户序号*/
	memset( &prdt_ac_id_c, 0x00, sizeof(struct prdt_ac_id_c) );
	
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c,
			   "ac_id=%ld", g_mdm_ac_rel.ac_id );
	if ( ret==100 )
	{
		sprintf(acErrMsg,"产品帐号对照表中不存在该记录[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W012");
		return 1;
	}else if( ret )
	{
	    sprintf(acErrMsg,"查询产品帐号对照表异常[%d]",ret);
		WRITEMSG
		strcpy (g_pub_tx.reply,"W013");
		return 1;
	}

	/* 查询客户名称*/
	ret = pub_base_disac_file(g_pub_tx.ac_no,prdt_ac_id_c.ac_seqn,
								g_pub_tx.ac_id_type);
	if( ret )
	{
		sprintf(acErrMsg,"调用由显示帐号和账号序号返回账户类型和主文件出错!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W010" );
		goto	ErrExit;
	}

	switch( g_pub_tx.ac_id_type[0] )
	{
		case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
		case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
		case '3':
				strcpy(g_pub_tx.name,g_ln_mst.name);
				break;		
		case '9':
				strcpy(g_pub_tx.name,g_in_mst.name);
				break;							
		default	:
				sprintf(acErrMsg,"账号对应产品类型错误,请与中心机房联系!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
	}
	/* jane于2003年7月27日添加 end */

	/*	jane于2003年7月27日封
	*** 卡的情况 ***
	if( !strncmp(g_mdm_ac_rel.mdm_code,"0020",4) )
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, \
							"ac_id=%ld and ac_seqn=1", g_mdm_ac_rel.ac_id);
		if(ret)
		{
	 		sprintf(acErrMsg,"该卡内不存在活期户,不允许收费!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O226");
			goto ErrExit;
		}
	}
	else
	{
		ret = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_c, "ac_id=%ld", \
							  g_mdm_ac_rel.ac_id);
		if(ret==100)
		{
	 		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
			goto ErrExit;
		}
	}

	if( prdt_ac_id_c.prdt_no[0]=='1' )
	{
		ret = Dd_mst_Sel(g_pub_tx.reply, &g_dd_mst, "ac_id=%ld", \
						 prdt_ac_id_c.ac_id);
		if(ret==100)
		{
	 		sprintf(acErrMsg,"产品帐号对照表不存在该记录!! [%d]",ret);
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
		sprintf(acErrMsg,"该账号不能作收费交易!! [%d]",ret);
 		WRITEMSG
  		strcpy(g_pub_tx.reply,"O192");
   		goto ErrExit;
	}*/

	set_zd_data("122E", g_pub_tx.name);
	set_zd_data("122D", g_pub_tx.ac_no);

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
