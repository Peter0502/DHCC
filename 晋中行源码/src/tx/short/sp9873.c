/*************************************************
* 文 件 名:    sp9873.c
* 功能描述：   
*              根据帐号和贷款币种回显相关信息
*
* 作    者:    lance
* 完成日期：   2003年03月25日
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
#include "prdt_ac_id_c.h"

sp9873()  
{ 		
	int ret=0,ac_seqn,ln_ac_seqn;
	char title[40],cur_no[3];
	char ac_no[20],ln_ac_no[20],name[50];
	double bal;

	struct prdt_ac_id_c g_prdt_ac_id;

	memset(&g_prdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));

	/** 取值、赋值 **/	
	get_zd_data("0310",ac_no);							/*帐号*/
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/	
	
	get_zd_data("0370",ln_ac_no);							/*贷款账号*/
	get_zd_int("0340",&ln_ac_seqn);						/*贷款序号*/	

	/* 调用根据贷款显示账号和账号序号取贷款主文件函数*/
	ret = pub_ln_lndis_file(ln_ac_no,ln_ac_seqn,
			&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret )
	{
		sprintf(acErrMsg,"读取贷款户信息error");
		WRITEMSG
		goto ErrExit;
	}
	/* 根据币种生成账户序号 */
	ret = pub_base_CurToSeqn( g_ln_parm.cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"根据币种生成账户序号 PASS![%s]",cur_no);
	WRITEMSG


	
	/* 根据帐号和账户序号取信息 */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",ac_no);
	if( ret==100 )
	{
	    sprintf(acErrMsg,"不存在该帐号!!ac_no=[%s]",ac_no);
	    WRITEMSG
	 	strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
	   	sprintf( acErrMsg, "查询介质帐号表异常!!ret=[%d]", ret );
	   	WRITEMSG
	   	goto ErrExit;
	}

	ret=Prdt_ac_id_Sel(g_pub_tx.reply,&g_prdt_ac_id,
			   			"ac_id=%ld and ac_seqn=%d",
					   	g_mdm_ac_rel.ac_id,ac_seqn);
   	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
		 		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}else if( ret ){
		 sprintf( acErrMsg, "查询产品账户表异常!!ret=[%d]", ret );
		 WRITEMSG
		 goto ErrExit;
 	}
 	sprintf( acErrMsg, "ac_id_type=[%s]", g_prdt_ac_id.ac_id_type );
	WRITEMSG	
	if(g_prdt_ac_id.ac_id_type[0]!='1')
	{
		 sprintf( acErrMsg, "存款账号非活期账号,请重新输入" );
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"L078");
		 goto ErrExit;
	}	

	ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst,
			"ac_id=%ld and ac_seqn=%d",g_mdm_ac_rel.ac_id,ac_seqn);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该账户!!ac_id=[%ld],ac_seqn=[%d]",
				g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}

	ret=Dd_parm_Sel(g_pub_tx.reply,&g_dd_parm,"prdt_no='%s'",g_prdt_ac_id.prdt_no);
	if( ret==100 )
	{
		sprintf(acErrMsg,"不存在该产品编号!!prdt_no=[%ld]",g_prdt_ac_id.prdt_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P145");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查询活期参数表异常!!ret=[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}
	strcpy(name,g_dd_mst.name);

	/* 输出变量 */
	set_zd_data("1166",name);
	set_zd_int("0350" , ac_seqn);
	set_zd_data("0310",ac_no);	/** 处理旧帐号 **/
	
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
 
