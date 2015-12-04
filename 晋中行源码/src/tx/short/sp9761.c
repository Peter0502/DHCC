/*************************************************
* 文 件 名:  sp9761.c
* 功能描述： 大额支付快捷查询
*				根据行号快捷查询行名
*
* 作    者:  jane
* 完成日期： 2003年5月06日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#define EXTERN
#include "public.h"
#include "mbfe_pub.h"

int sp9761()
{
	char same_sabk_flg[2];	/*是否检查与本行是同一清算行的标志*/
	struct mbfenetbank_c mbfenetbank_v;
	struct mbfeeisbank_c mbfeeisbank_v;

	memset( &mbfenetbank_v, 0x00, sizeof( struct mbfenetbank_c ) );
	memset( &mbfeeisbank_v, 0x00, sizeof( struct mbfeeisbank_c ) );

	strcpy( same_sabk_flg, "0" );	/*是否检查与本行是同一清算行的标志*/

	get_zd_data( "0580", mbfenetbank_v.nbkcode );	/*接收行号*/
	if( strlen( mbfenetbank_v.nbkcode ) == 0 )
	{
		get_zd_data( "0590", mbfenetbank_v.nbkcode );	/*签发行号或现金汇票兑付行号*/
		strcpy( same_sabk_flg, "1" );	/*是否检查与本行是同一清算行的标志*/
	}

	if( strlen( mbfenetbank_v.nbkcode ) == 12 )
	{
		g_reply_int = Mbfenetbank_Sel( g_pub_tx.reply, &mbfenetbank_v, 
			"nbkcode='%s'", mbfenetbank_v.nbkcode );
		if ( g_reply_int==100 )
		{
			sprintf( acErrMsg, "该行号不存在" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "查询参与者行号表数据库操作出错[%d]",g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfenetbank_v.nbkstate[0] != '1' )
		{
			sprintf( acErrMsg, "该行号已经被注销" );
			MBFEWRITEMSG
			vtcp_log("行号＝【%s", mbfenetbank_v.nbkcode);
			goto ErrExit;
		}

		if( same_sabk_flg[0] == '0' )
		{
			if( get_sys_par_val( "01", mbfetran.isdficode ) ) return -1;
			if( !strcmp( mbfetran.isdficode, mbfenetbank_v.sabkcode ) )
			{
				sprintf( acErrMsg, "不可与本行为同一清算行范围内的机构" );
				MBFEWRITEMSG
				goto ErrExit;
			}
		}

		set_zd_data( "0963", mbfenetbank_v.nbkname );
	}
	else if( strlen( mbfenetbank_v.nbkcode ) == 6 )
	{
		g_reply_int = Mbfeeisbank_Sel( g_pub_tx.reply, &mbfeeisbank_v, 
			"bnkno='%s'", mbfenetbank_v.nbkcode );
		if ( g_reply_int == 100 )
		{
			sprintf( acErrMsg, "电子联行行号不存在" );
			MBFEWRITEMSG
			goto ErrExit;
		}
		else if( g_reply_int )
		{
			sprintf( acErrMsg, "查询电子联行行号表数据库操作出错[%d]", g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfeeisbank_v.bnksstat[0] != '1' )
		{
			sprintf( acErrMsg, "该行号已经被注销" );
			MBFEWRITEMSG
			goto ErrExit;
		}

		if( mbfeeisbank_v.cnapsflg[0] == 'N' )
		{
			sprintf( acErrMsg, "该行已参与支付系统请转走汇兑业务[%s]", mbfeeisbank_v.cnapsbnkno );
			MBFEWRITEMSG
			goto ErrExit;
		}

		set_zd_data( "0963", mbfeeisbank_v.bnklname );
	}
	else
	{
		sprintf( acErrMsg, "行号位数不正确" );
		MBFEWRITEMSG
		goto ErrExit;
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
