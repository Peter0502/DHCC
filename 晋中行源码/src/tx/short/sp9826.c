/*************************************************
* 文 件 名:    sp9826.c
* 功能描述：   
*              根据贷款显示账号和账号序号取贷款主文件
*
* 作    者:    lance
* 完成日期：   2003年01月21日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9826()  
{ 		
	int ret=0;
	char ac_no[20];
	int ac_seqn;
	
	/** 数据初始化 **/
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0370" , ac_no );	/* 贷款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	set_zd_data("0370",ac_no);	/* 处理旧帐号 */	
	
	get_zd_int( "0340" , &ac_seqn );	/* 贷款账户序号s */
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",ac_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"凭证(介质)与账户关系表无此记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"O001" );
		ERR_DEAL
	} 
	else if( ret )
	{
    	sprintf( acErrMsg,"取凭证(介质)与账户关系表异常erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
    }

	sprintf(acErrMsg,"取出的账户ID为[%ld]seqn[%d]",
		g_mdm_ac_rel.ac_id,g_mdm_ac_rel.ac_seqn);
	WRITEMSG	
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	

		Ln_mst_Clo_Sel ();
	}
				
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量[%s]",g_ln_mst.name);
	WRITEMSG
	set_zd_data( "0250", g_ln_mst.name );		/* 户名 */	
	set_zd_double( "0420", g_ln_mst.bal );		/* 余额 */	
	set_zd_data( "0700", g_ln_mst.ac_sts );		/* 账户状态 */			
			
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

