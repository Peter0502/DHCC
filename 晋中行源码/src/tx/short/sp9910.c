/*************************************************
* 文 件 名:    sp9910.c
* 功能描述：   
*              活期根据显示账号币种得出户名和序号和余额
*
* 作    者:    jane
* 完成日期：   2003年05月25日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9910()  
{ 		
	int ret=0;
	char ac_no[20];
	char cur_no[3];
	int ac_seqn;
	
	/** 数据初始化 **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0320" , ac_no );	/* 保证金帐号 */
	if ( strlen(ac_no)==0 )
	{
	    goto OkExit;
	}

	pub_base_old_acno( ac_no );   	/* 对旧帐号的处理 */

	get_zd_data( "0210" , cur_no ); /* 币种 */
	
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
	
	sprintf(acErrMsg,"[%s]",dd_mst_tmp.name);
	WRITEMSG		  
	sprintf(acErrMsg,"根据币种、显示账号取活期主文件PASS");
	WRITEMSG

/**
	if( strcmp( dd_mst_tmp.prdt_no, "131" ) !=0 )
	{
		sprintf( acErrMsg, "所输账号非保证金账号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B202" );
		goto ErrExit;
	}
**/

	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	set_zd_data( "0820", dd_mst_tmp.name );	/* 保证金账户全称 */
	set_zd_double( "1004", dd_mst_tmp.bal );/* 保证金账户余额 */
	set_zd_data( "0230", dd_mst_tmp.prdt_no );/* 产品编号 */
	set_zd_int( "0340", ac_seqn );		/* 还款账户序号 */
	set_zd_data("0320",ac_no);		/* 处理旧帐号 */
		
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
   
