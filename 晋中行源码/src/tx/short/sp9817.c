/*************************************************
* 文 件 名:    sp9817.c
* 功能描述：   
*              根据贷款账号,账户序号，取得账户信息(贷款还本)
*
* 作    者:    lance
* 完成日期：   2003年01月19日
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
	
sp9817()  
{ 		
	int ret=0;
	int flag=0, redflag=0;
	struct ln_mst_c		ln_mst_tmp;
	struct ln_mst_c		v;
	struct mdm_ac_rel_c		w;
	double vbjlx,vbnlx,vbylx,vfllx;
	
	/** 数据初始化 **/
	memset(&ln_mst_tmp , 0x00 , sizeof(struct ln_mst_c) );

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "1071" , w.ac_no );	/* 贷款帐号 */
	pub_base_old_acno(  w.ac_no );  	/** 对旧帐号的处理 **/	
	get_zd_long( "1072" , &v.ac_seqn );	/* 帐号序号 */


	set_zd_data("1071",w.ac_no);	/* 处理旧帐号 */
	
	sprintf( acErrMsg," senf acno [%s] [%ld]",w.ac_no,v.ac_seqn );
	WRITEMSG
	
	/* 根据贷款帐号查询账号介质关系表取出账户ID */
	ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel," ac_no='%s' ",w.ac_no);
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
	
	if( g_mdm_ac_rel.ac_seqn!=ALL_SEQN && g_mdm_ac_rel.ac_seqn!=v.ac_seqn)
	{
    	sprintf( acErrMsg,"[%d]!=[%d]",g_mdm_ac_rel.ac_seqn,v.ac_seqn);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	ERR_DEAL
	}
	else
	{
		ret = Ln_mst_Sel(g_pub_tx.reply,&g_ln_mst," ac_id=%ld and ac_seqn=%d ",
			g_mdm_ac_rel.ac_id,v.ac_seqn );
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"L002" );
			ERR_DEAL
		} 

		else if( ret ) { ERR_DEAL }	
		Ln_mst_Clo_Sel ();
		ret = Ln_parm_Sel(g_pub_tx.reply,&g_ln_parm," prdt_no='%s' ",
			g_ln_mst.prdt_no);
		if( ret==100 )
		{
			sprintf( acErrMsg,"贷款主文件表中无此记录 erro code=[%s]",ret);
			WRITEMSG
			strcpy( g_pub_tx.reply ,"W030" );
			ERR_DEAL
		} 
		else if( ret ) { ERR_DEAL }	
		vbjlx=vbnlx=vbylx=vfllx=0.00;
		TRACE
		ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"0",&vbjlx,1 );
		if( ret ) { ERR_DEAL }	
		TRACE
		ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"1",&vbnlx,1 );
		if( ret ) { ERR_DEAL }	
		ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"2",&vbylx,1 );
		if( ret ) { ERR_DEAL }	
		ret = pub_ln_CalIntst( g_ln_mst , g_ln_parm ,"3",&vfllx,1 );
		if( ret ) { ERR_DEAL }	
		TRACE
vtcp_log("name is [%s]",g_ln_mst.name);
vtcp_log("bal is [%.2lf]",g_ln_mst.bal);
		set_zd_data("1073",g_ln_mst.name);
		set_zd_double("1074",g_ln_mst.bal);
		set_zd_double("1075",g_ln_mst.in_lo_intst);
		set_zd_double("1076",g_ln_mst.out_lo_intst);
		set_zd_double("1077",g_ln_mst.cmpd_lo_intst);
		set_zd_double("1078",vbjlx);
		set_zd_double("1079",vbnlx+vbylx+vfllx);
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
   
