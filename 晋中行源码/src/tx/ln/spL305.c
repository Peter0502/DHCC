/*************************************************
* 文 件 名: spL305.c
* 功能描述：   
*              贷款五级认定
*
* 作    者:    lance
* 完成日期：   2003年01月17日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "stdio.h" 
#define EXTERN
#include "public.h"
#include "trace_log_c.h"
	
	static char sts[2];
		
spL305()  
{ 		
	int ret=0;

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	sprintf(acErrMsg,"初始化全局变量 PASS!");
	WRITEMSG	
	
	/** 数据初始化 **/
	if ( data_init_struct() )	ERR_DEAL;
	
	sprintf(acErrMsg,"数据初始化 PASS!");
	WRITEMSG
		
	/** 取值、赋值 **/
	if ( data_set_struct() )        ERR_DEAL;
	
	sprintf(acErrMsg,"取值、赋值 PASS!");
	WRITEMSG
			
	/** 处理流程 **/
		
	/* 检查贷款状态 
	if ( strcmp( g_ln_mst.ac_sts,"1") )
	{
		sprintf( acErrMsg,"该贷款账户状态非正常");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L018" );
		ERR_DEAL
	}
	sprintf(acErrMsg,"检查贷款状态 PASS!");
	WRITEMSG */	
					
	/* 更新贷款主文件 */
    ret = Ln_mst_Dec_Upd( g_pub_tx.reply , "ac_id=%ld and ac_seqn=%d" ,g_ln_mst.ac_id , g_ln_mst.ac_seqn );
    if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Dec_Upd错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    
    ret = Ln_mst_Fet_Upd( &g_ln_mst,g_pub_tx.reply );   
    if ( ret == 100 )
    {
        sprintf( acErrMsg, "贷款主文件中无此记录" );
        WRITEMSG
        ERR_DEAL
    }else if ( ret )
    {
        sprintf( acErrMsg, "执行Ln_mst_Fet_Upd出错!ret=[%d]",ret );
        WRITEMSG
        ERR_DEAL
    }
    	/* 修改 */
	strcpy ( g_ln_mst.five_sts , sts );		/* 五级认定标志 */
	
	
	ret = Ln_mst_Upd_Upd( g_ln_mst, g_pub_tx.reply );
	if ( ret == -239 )
	{
		sprintf( acErrMsg, "修改贷款主文件错,存在重复记录!" );
		WRITEMSG
		strcpy(g_pub_tx.reply,"L031");
		ERR_DEAL
	}else if( ret )
		{
			sprintf( acErrMsg,"执行Ln_mst_Upd_Upd错!ret=[%d]",ret );
			WRITEMSG
			ERR_DEAL
		}
	Ln_mst_Clo_Upd();
	sprintf(acErrMsg,"更新贷款主文件 PASS!");
	WRITEMSG	
		
	/* Pub_tx结构赋值 */

	strcpy ( g_pub_tx.add_ind , "1" );		/* 增减:0减1加2销户 */	
	strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
	strcpy ( g_pub_tx.ct_ind , "2" );		/* 现转:1现2转 */	
	/* 是否打印存折(有折无折标志):1是0否 */
	strcpy ( g_pub_tx.prt_ind , "0" );	
	/* 入明细帐标志:1日间入2日终单笔入3日终汇总入 */
	strcpy ( g_pub_tx.hst_ind , "1" );
	g_pub_tx.svc_ind = 3060;			/* 贷款五级认定 */
	strcpy ( g_pub_tx.brf,"贷款五级评定" );	
	sprintf(acErrMsg,"Pub_tx结构赋值 PASS!");
	WRITEMSG
	
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	/*** 交易记帐处理 ***/
	if( pub_acct_trance() )
	{
		sprintf(acErrMsg,"交易记帐处理错误!");
		WRITEMSG
		ERR_DEAL
	}
	sprintf(acErrMsg,"交易记帐处理 PASS!");
	WRITEMSG
			

OkExit:
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"贷款五级认定成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
    sprintf(acErrMsg,"贷款五级认定失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
    
int data_init_struct()
{	
	return 0;	
}

int data_set_struct()
{  
	int ret=0;  
	sprintf(acErrMsg,"进入赋值函数");
	WRITEMSG
			
	get_zd_data("0370",g_pub_tx.ac_no);		/* 贷款帐号 */
	sprintf(acErrMsg,"贷款帐号[%s]",g_pub_tx.ac_no);
	WRITEMSG
	
	get_zd_int( "0340" , &g_pub_tx.ac_seqn);	/* 贷款账户序号 */
	sprintf(acErrMsg,"贷款账户序号[%d]",g_pub_tx.ac_seqn);
	WRITEMSG	
	
	get_zd_data("0700",sts);			/* 五级认定标志 */
	sprintf(acErrMsg,"五级认定标志[%s]",sts);
	WRITEMSG
	ret = pub_ln_lndis_file(g_pub_tx.ac_no,g_pub_tx.ac_seqn,
				&g_mdm_ac_rel,&g_ln_mst,&g_ln_parm ); /*读取贷款户信息*/
	if( ret ) {return ret;}
						
	return 0;
}

