/*************************************************
* 文 件 名:  sp9914.c
* 功能描述： 股权出售加权计算成本 
*
* 作    者:  
* 完成日期： 2003年6月09日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"

int sp9914()
{
	struct	mo_righ_invest_c	sMo_righ_invest;	/* 权益性投资登记簿 */
	int	ret=0,sal_num=0;
	double amt=0.00;
	double sj1,sj2;

	memset( &sMo_righ_invest, 0x00, sizeof( struct mo_righ_invest_c ) );

	get_zd_data( "0580", sMo_righ_invest.pact_no );	/* 权益投资协议编号 */
	get_zd_int( "0530", &sal_num );					/* 出售股数 */

	/* 根据权益性投资协议编号查找权益性投资登记薄 */
	ret = Mo_righ_invest_Sel( g_pub_tx.reply, &sMo_righ_invest, "pact_no='%s' ",sMo_righ_invest.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "无此权益投资协议编号!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B082" );
		goto ErrExit;
    }
	else if( ret )
   	{
   		sprintf( acErrMsg, "执行Mo_righ_invset_Sel错![%d]", ret );
        WRITEMSG
        goto ErrExit;
   	}

	if( sal_num>sMo_righ_invest.num )
   	{
   		sprintf( acErrMsg, "出售股数大于持有股数[%d]", sMo_righ_invest.num);
        WRITEMSG
		sprintf( g_pub_tx.reply, "B401" );
        goto ErrExit;
   	}
	else if( sal_num==sMo_righ_invest.num )
   	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_righ_invest.ac_id);
		if( ret )
   		{
   			sprintf( acErrMsg, "执行In_mst_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   		}
		set_zd_double( "A029", g_in_mst.bal );		/* 出售成本 */
	}
	else
	{
		ret=In_mst_Sel(g_pub_tx.reply,&g_in_mst,"ac_id=%d",sMo_righ_invest.ac_id);
		if( ret )
   		{
   			sprintf( acErrMsg, "执行In_mst_Sel错![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
   		}
		vtcp_log("sal_num=[%d],num=[%d]",sal_num,sMo_righ_invest.num);
		sj1=sal_num;
		sj2=sMo_righ_invest.num;
		amt=pub_base_PubDround_1( (sj1/sj2) * g_in_mst.bal);
		vtcp_log("amt=[%lf]",amt);
		set_zd_double( "0400", amt );		/* 出售成本 */
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
