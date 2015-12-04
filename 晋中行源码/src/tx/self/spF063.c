/***************************************************************
* 文 件 名: spF063.c
* 功能描述：自助终端修改支取密码
* 作    者: jack
* 完成日期：2003年6月9日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spF063()
{   
	int ret; 
	char cAcno[20];
	char cNpass[7];
	char cOpass[7];

	struct mdm_ac_rel_c sMdm_ac_rel;

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( cNpass, 0x0, sizeof(cNpass) );
	memset( cOpass, 0x0, sizeof(cOpass) );
	memset( &sMdm_ac_rel, 0x00, sizeof(sMdm_ac_rel) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0370", cAcno );
	pub_base_old_acno( cAcno );
	get_zd_data( "0790", cOpass );
	get_zd_data( "0800", cNpass );

	/*根据帐户查找*/
	ret=Mdm_ac_rel_Dec_Upd( g_pub_tx.reply, "ac_no='%s'", cAcno );
	if ( ret )
	{
	 	sprintf( acErrMsg,"执行Mdm_ac_rel_Dec_Upd出错!!!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	ret=Mdm_ac_rel_Fet_Upd( &sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
	 	sprintf( acErrMsg,"查找介质账号对应记录失败!!!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	if( sMdm_ac_rel.draw_pwd_yn[0] != 'Y' )
	{
	 	sprintf( acErrMsg,"该介质不允许密码支取!!!" );
	  	WRITEMSG
		strcpy( g_pub_tx.reply, "P137" );
	    goto ErrExit;
	}

	get_zd_data("1027",g_pub_tx.draw_pwd); 			/* 支取密码 */

	pub_base_EnDes( g_pub_tx.tx_date, "", cOpass );
	pub_base_EnDes( g_pub_tx.tx_date, "", cNpass );

	if ( strcmp( cOpass, sMdm_ac_rel.draw_pwd ) )
	{
	 	sprintf( acErrMsg,"原密码输入不正确!!!" );
	  	WRITEMSG
		strcpy( g_pub_tx.reply, "M005" );
	    goto ErrExit;
	}

	memset( sMdm_ac_rel.draw_pwd, 0x0, sizeof(sMdm_ac_rel.draw_pwd) );
	strcpy( sMdm_ac_rel.draw_pwd, cNpass );

	ret=Mdm_ac_rel_Upd_Upd( sMdm_ac_rel, g_pub_tx.reply );
	if ( ret )
	{
	 	sprintf( acErrMsg, "执行Mdm_ac_rel_Upd_Upd错[%d]", ret );
	  	WRITEMSG
	    goto ErrExit;
	}

	Mdm_ac_rel_Clo_Upd();

    /*** 登记交易流水 ***/
	if( pub_ins_trace_log() )
	{
	 	sprintf( acErrMsg,"登记交易流水错误!" );
	  	WRITEMSG
	    goto ErrExit;
	}

	set_zd_data( "0370", cAcno );

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"修改密码成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"修改密码失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
