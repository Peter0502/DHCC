/*************************************************
* 文 件 名:    sp9806.c
* 功能描述：   
*              活期根据显示账号币种得出户名和序号
*
* 作    者:    lance
* 完成日期：   2003年01月13日
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
	
sp9806()  
{ 		
	int ret=0;
	char ac_no[21];
	char cur_no[3];
	int ac_seqn;
	
	/** 数据初始化 **/
	struct dd_mst_c dd_mst_tmp;
	memset (&dd_mst_tmp, 0x00, sizeof(struct dd_mst_c));
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0380" , ac_no );	/* 还款帐号 */
	if ( strlen(ac_no)==0 )
	{	
	    goto OkExit;
	}
	pub_base_old_acno( ac_no );  	/** 对旧帐号的处理 **/
	
	get_zd_data( "0210" , cur_no ); /* 币种 */
vtcp_log( "zy1111ac_no[%s]cur_no[%s]",ac_no,cur_no );	
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
	
vtcp_log( "zy2222ac_no[%s]cur_no[%s]",ac_no,cur_no );	
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
	sprintf(acErrMsg,"根据币种、显示账号取活期主文件PASS   name[%s]", dd_mst_tmp.name);
	WRITEMSG

	
	/* 输出变量 */
	sprintf(acErrMsg,"进入输出变量");
	WRITEMSG
	set_zd_data( "0260", dd_mst_tmp.name );		/* 还款账户户名 */	
	set_zd_int( "0340", ac_seqn );			/* 还款账户序号 */
	set_zd_data( "0910", dd_mst_tmp.opn_br_no );	/* 开户机构 */
	set_zd_data("0380",ac_no);	/* 处理旧帐号 */
		
vtcp_log( "xxxx__name[%s]br_no[%s]",dd_mst_tmp.name,dd_mst_tmp.opn_br_no );	
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
   
