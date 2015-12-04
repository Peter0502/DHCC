/*************************************************************
* 文 件 名:     spD698.c
* 功能描述：    帐户余额查询 
* 作    者:     
* 完成日期：    2003年8月25日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
	}
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "prdt_ac_id_c.h"

spD698()
{
	int ret=0;
	char name[50];
	long cif_no;
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));
	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/
	get_zd_data("0300",g_pub_tx.ac_no);
    pub_base_old_acno(g_pub_tx.ac_no);
	get_zd_data("0670",g_pub_tx.draw_pwd_yn);		
	get_zd_data("0800",g_pub_tx.draw_pwd);			/*** 支取密码 ***/
	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);			/*** 证件类型 ***/
	get_zd_data("0310",g_pub_tx.id_no);				/*** 证件号码 ***/
/*vtcp_log("密文是:[%s]",g_pub_tx.draw_pwd);*/
	/* 取帐户信息 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
							g_pub_tx.ac_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"账号不存在[%d]");
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找错[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}

	/*** 检验账户状态 ***/
	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"该介质已挂失!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='2')
	{
		sprintf(acErrMsg,"该介质已挂失换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if(g_mdm_ac_rel.note_sts[0]=='3')
	{
		sprintf(acErrMsg,"该介质已正常换证!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"该介质已销户!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"该介质已做部提，请输入新帐号!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"O217");
		goto ErrExit;
	}

	if( g_pub_tx.draw_pwd_yn[0]=='Y' )
	{
		pub_base_EnDes( g_pub_tx.tx_date,"",g_pub_tx.draw_pwd );
		pub_base_strpack( g_mdm_ac_rel.draw_pwd );
		if( strcmp(g_mdm_ac_rel.draw_pwd, g_pub_tx.draw_pwd) )
		{
			sprintf(acErrMsg, "密码错误!!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "O123");
			goto ErrExit;
		}
	}

	if( g_pub_tx.draw_id_yn[0]=='Y' )
	{
		if( g_pub_tx.id_type[0]!=g_mdm_ac_rel.id_type[0] )
		{
		 	sprintf(acErrMsg, "证件类型不符!! [%s]", g_pub_tx.id_type);
		   	WRITEMSG
			strcpy(g_pub_tx.reply, "P444");
			goto ErrExit;
		}

		pub_base_strpack( g_mdm_ac_rel.id_no );
		pub_base_strpack( g_pub_tx.id_no );
		if( strcmp(g_pub_tx.id_no,g_mdm_ac_rel.id_no) )
		{
		 	sprintf(acErrMsg, "证件号码不正确!! [%s]", g_pub_tx.id_type);
			WRITEMSG
			strcpy(g_pub_tx.reply, "P445");
			goto ErrExit;
		}
	}

	/*** 取账户余额 ***/
	ret = Prdt_ac_id_Sel( g_pub_tx.reply, &g_prdt_ac_id, "ac_id=%ld", \
							g_mdm_ac_rel.ac_id );
	if( ret==100 )
	{
		sprintf(acErrMsg,"账号不存在!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"M003" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"取账号错误![%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply,"D103" );
		goto ErrExit;
	}

	if( g_prdt_ac_id.ac_id_type[0]=='1' )
	{
		ret = Dd_mst_Sel( g_pub_tx.reply,&g_dd_mst,"ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
		ERR_DEAL

		if( g_dd_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"账户已销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P245" );
			goto ErrExit;
		}	
		bal = g_dd_mst.bal ;
	}
	else if( g_prdt_ac_id.ac_id_type[0]=='2' )
	{
		ret = Td_mst_Sel( g_pub_tx.reply,&g_td_mst,"ac_id=%ld", \
						  g_mdm_ac_rel.ac_id);
		ERR_DEAL
		if( g_td_mst.ac_sts[0]=='*' )
		{
			sprintf(acErrMsg,"账户已销!");
			WRITEMSG
			strcpy( g_pub_tx.reply,"P245" );
			goto ErrExit;
		}	
		bal = g_td_mst.bal ;
	}
	else 
	{
		sprintf(acErrMsg,"账户类型错误!");
		WRITEMSG
		strcpy( g_pub_tx.reply,"O166" );
		goto ErrExit;
	}

	set_zd_double( "0420" , bal );
/*
	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水错误!");
		WRITEMSG
		goto ErrExit;
	}
*/
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"查询打印成功![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"查询打印失败![%d]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;

}
