/*************************************************************
* 文 件 名:  sp9970.c
* 功能描述： 大额支付快捷查询
*			 汇票本行汇票挂失
*
* 作    者:  jane
* 完成日期： 2003年8月1日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "com_branch_c.h"

int sp9970()
{
	char oprt_flag[2];
	struct mbfebillopnreg_c mbfebillopnreg_v;

	memset( &mbfebillopnreg_v, 0x00, sizeof( struct mbfebillopnreg_c ) );

	pub_base_sysinit();

	get_zd_data( "0700", oprt_flag );
	get_zd_data( "0860", mbfebillopnreg_v.billcode );
	sprintf( mbfebillopnreg_v.billcode, "%08ld", atol( mbfebillopnreg_v.billcode ) );

	g_reply_int = Mbfebillopnreg_Sel( g_pub_tx.reply, &mbfebillopnreg_v,
		"billcode='%s'", mbfebillopnreg_v.billcode );
	if( g_reply_int == 100 )
	{
		sprintf( acErrMsg, "没有签发该张汇票!" );
		MBFEWRITEMSG
		goto ErrExit;
	}
	else if( g_reply_int )
	{
		sprintf( acErrMsg, "查询汇票登记簿数据库操作出错[%d]", g_reply_int );
		WRITEMSG
		goto ErrExit;
	}

	if( oprt_flag[0] == '0' )    /*本行签发汇票挂失*/
	{
		/*汇票状态为移存，挂失状态为正常或解挂*/
		if( mbfebillopnreg_v.billsts[0] =='2' && 
			( mbfebillopnreg_v.los_rpt_ind[0] == '0' || mbfebillopnreg_v.los_rpt_ind[0] == '2' ) )
		;
		else if( mbfebillopnreg_v.billsts[0] != '2' )
		{
			vtcp_log( "该张汇票状态不允许挂失!汇票号码[%s]", mbfebillopnreg_v.billcode);
			sprintf( acErrMsg, "该张汇票状态不是已经移存,不允许挂失!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( mbfebillopnreg_v.billsts[0] == '2' && mbfebillopnreg_v.los_rpt_ind[0] == '1' )
		{
			sprintf( acErrMsg, "该张汇票已经挂失!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
	}
	else if( oprt_flag[0] == '1' )    /*本行签发汇票解挂*/
	{
		/*汇票状态为移存，挂失状态为挂失*/
		if( mbfebillopnreg_v.billsts[0] =='2' && mbfebillopnreg_v.los_rpt_ind[0] == '1' )
		;
		else
		{
			vtcp_log( "该张汇票状态不允许解挂!汇票号码[%s]", mbfebillopnreg_v.billcode);
			sprintf( acErrMsg, "该张汇票状态不是已经移存并挂失,不能解挂!" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		set_zd_data( "0300", mbfebillopnreg_v.los_rpt_bookno );    /*挂失申请书号*/
	}

	set_zd_double( "0390", mbfebillopnreg_v.amount );    /*出票金额*/
	set_zd_long( "0450", mbfebillopnreg_v.signdate );    /*出票日期*/
	set_zd_data( "0600", mbfebillopnreg_v.payeropenaccbkcode );    /*签发行号*/
	set_zd_data( "0590", mbfebillopnreg_v.cashbillbkno );    /*现金兑付行号*/

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
