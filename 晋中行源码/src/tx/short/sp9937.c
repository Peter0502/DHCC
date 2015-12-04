/*************************************************
* 文 件 名:    sp9937.c
* 功能描述：   
*              活期根据显示账号币种得出户名和序号
*
* 作    者:    lance
* 完成日期：   2003年06月28日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2.      :wq

*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9937()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	char cif_type_id[2];
	char name[51];
	char sts[2];
	
	int ac_seqn;
	long cif_no_id;
	long cif_no;
	long save_ac_id;
	
	
	/** 数据初始化 **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/			
	get_zd_data( "0320" , ac_no );	/* 存款帐号 */
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	
	/***
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}
	***/
	
	get_zd_data( "0250" , name );	 /* 贷款户名 */
	get_zd_data( "0670" , sts ); 	 /* 第三人标志 */
			
	get_zd_data( "0210" , cur_no );  /* 币种 */
	
	/* 根据币种生成账户序号 */
	ret = pub_base_CurToSeqn( cur_no , &ac_seqn );
	if ( ret )
		{
			sprintf(acErrMsg,"调用函数根据币种生成账户序号错误!");
			WRITEMSG
			ERR_DEAL;	
		}
	sprintf(acErrMsg,"根据币种生成账户序号 PASS![%s]",cur_no);
	WRITEMSG
	
	/* 根据币种、显示账号取活期主文件*/
	ret = pub_base_CurGetddFile( ac_no , cur_no , &dd_mst_tmp );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据币种、显示账号取活期主文件函数错误");
		WRITEMSG
		goto ErrExit;
	}
	
	pub_base_strpack( ac_no );
	if( strlen( ac_no ) )
	{
		ret = pub_ln_AcCheck( ac_no,ac_seqn,&cif_no,&save_ac_id );
		if( ret )
		{
			sprintf(acErrMsg,"调用函数pub_ln_AcCheck错误!");
			WRITEMSG
			if( strcmp( g_pub_tx.reply , "L016")==0)
			{
				strcpy(g_pub_tx.reply , "L185");
			}
			goto ErrExit;
		}

	}	
	sprintf(acErrMsg,"[%s]",dd_mst_tmp.name);
	WRITEMSG
	pub_base_strpack( name );
	pub_base_strpack( dd_mst_tmp.name );	
	/*if( sts[0]=='N' )
	{
		if( strcmp( name , dd_mst_tmp.name ) )
		{
			sprintf(acErrMsg,"存款账号户名与贷款户名不一致!");
			WRITEMSG
			strcpy(g_pub_tx.reply , "L189");
			goto ErrExit;	
		}	
	}	*/  
	sprintf(acErrMsg,"根据币种、显示账号取活期主文件PASS");
	WRITEMSG
	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	set_zd_data( "0820", dd_mst_tmp.name );		/* 还款账户户名 */	
	set_zd_int( "0360", ac_seqn );			/* 还款账户序号 */
	set_zd_data( "0910", dd_mst_tmp.opn_br_no );	/* 开户机构 */
	set_zd_data("0320",ac_no);	/* 处理旧帐号 */	
		
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
   
