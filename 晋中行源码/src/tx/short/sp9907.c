/*************************************************
* 文 件 名:    sp9907.c
* 功能描述：   
*              定期存单根据显示账号得出户名和余额
*
* 作    者:    jane
* 完成日期：   2003年05月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9907()  
{ 		
	struct	mdm_ac_rel_c 	sMdmAcRel;	/* 介质账号对照表 */
	struct	td_mst_c 	sTdMst;		/* 定期主文件 */
	int ret=0;
	char ac_no[20];
	
	/** 数据初始化 **/
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sTdMst, 0x00, sizeof( struct td_mst_c ) );
	
	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	
	get_zd_data( "0320" , ac_no );	/* 质押帐号 */

	/* 根据质押账号查询介质账号对照表及定期存款主文件 */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
						ac_no);
	if( ret == 100 )
	{
		sprintf( acErrMsg, "质押账号不存在![%s]", ac_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B183" );
		goto ErrExit;
	}

	else if( ret )
	{
		sprintf( acErrMsg, "执行Mdm_ac_rel_Sel出错![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Td_mst_Sel( g_pub_tx.reply, &sTdMst, "ac_id=%ld and ac_seqn=%d",\
					sMdmAcRel.ac_id, 0 );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "质押账号不是定期存单号!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B184" );
		goto ErrExit;
	}

	else if( ret )
	{
		sprintf( acErrMsg, "执行Td_mst_Sel错误![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( sTdMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "此质押账号状态不正常!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B185" );
		goto ErrExit;
	}

	/* 输出变量 */
	set_zd_data( "0820", sTdMst.name );	/* 客户名称 */	
	set_zd_double( "1004", sTdMst.bal );	/* 质押存单金额 */
	
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
   
