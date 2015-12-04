/*************************************************
* 文 件 名:  sp9810.c
* 功能描述： 由内部产品编号查询内部产品参数表,
             并将查出的科目控制字转换成科目送前台显示	
*
* 作    者:  jack
* 完成日期： 2003年1月17日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "com_item_c.h"

int sp9810()
{
	struct in_parm_c sInParm;	/* 内部产品表结构 */
	struct com_item_c sComItem;	/* 公共科目表结构 */
	int ret;
	double llz = 0.00;
	double fxllz = 0.00;

	pub_base_sysinit();
	get_zd_data( "1121", g_pub_tx.prdt_code ); /* 产品编号 */

	ret=In_parm_Sel( g_pub_tx.reply , &sInParm ,
			 		"prdt_no='%s'" , g_pub_tx.prdt_code );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "查询内部帐产品参数表出错!\
			prdt_no=[%s]", g_pub_tx.prdt_code );
		WRITEMSG
		strcpy( g_pub_tx.reply, "W058" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行In_parm_Sel错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret=Com_item_Sel( g_pub_tx.reply , &sComItem ,
			 		 "acc_hrt='%s'" , sInParm.acc_hrt );	
	if( ret==100 )
	{
		sprintf( acErrMsg, "查询公共科目表出错!\
			acc_hrt=[%s]", sInParm.acc_hrt );
		WRITEMSG
		strcpy( g_pub_tx.reply, "S033" );
		goto	ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Com_item_Sel错误!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret = pub_base_getllz(sInParm.rate_no,sInParm.cur_no,g_pub_tx.tx_date,&llz);
	if( ret != 0 )
	{
		sprintf( acErrMsg, "取利率值失败!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

	ret = pub_base_getllz(sInParm.fine_rate_no,sInParm.cur_no,
							g_pub_tx.tx_date,&fxllz);
	if( ret != 0 )
	{
		sprintf( acErrMsg, "取罚息利率值失败!ret=[%d]", ret );
		WRITEMSG
		goto	ErrExit;
	}

		sprintf( acErrMsg, "取罚息利率值失败!sInParm.stop_date=[%ld]",
					sInParm.stop_date);
		WRITEMSG
	set_zd_data( "1128", sComItem.acc_no );     /*科目号*/
	set_zd_data( "1126", sInParm.prdt_mo );     /*产品名称*/
	set_zd_data( "1127", sInParm.cur_no );      /*币种*/
	set_zd_long( "1122", sInParm.stop_date );	/*到期日期*/
	set_zd_double( "1123", llz );				/*利率值*/
	set_zd_double( "1124", fxllz );				/*罚息利率*/
	set_zd_data( "1125", sInParm.intst_knd );   /*计息类型*/

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
