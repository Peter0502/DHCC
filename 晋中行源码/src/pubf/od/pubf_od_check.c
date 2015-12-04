/*************************************************
* 文 件 名: pubf_od_check.c
* 功能描述：   
*              pub_od_check     透支产品属性检查
*
* 作    者:    power
* 完成日期：   2003年12月30日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_infrm_c.h"

/**********************************************************************
* 函 数 名：  pub_od_check
* 函数功能：  透支产品属性检查,检查台账内容是否合乎产品参数定义的要求
* 作    者：  power
* 完成时间：  2003年12月30日
*
* 参    数： 
*     输  入：tx_type   char(1)   交易类型(1-开户 2-现金透支 3-透支 4-销户)    
*      
*     输  出:           char(4)   响应码
*
*     返回值： 0 成功
*            非0 失败
*
* 修改历史：
*                   
********************************************************************/
int pub_od_check(char *tx_type)
{
	struct mo_infrm_c mo_infrm_c;

	int ret = 0;
	long min_day , max_day ;
	double min_rate , max_rate ;
	long infrm_date;

	memset( &g_od_parm,0x00,sizeof(struct od_parm_c) );

    /* 根据产品代码查透支产品参数表，查看是否有该种产品 */
	ret = Od_parm_Sel( g_pub_tx.reply , &g_od_parm, "prdt_no='%s'", 
					   g_od_mst.prdt_no);
	if( ret==100 )
	{
		sprintf( acErrMsg,"透支产品参数表无此记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"T001" );
		return 1;
	} 
	else if( ret !=0 )
	{
    	sprintf( acErrMsg,"取透支产品参数表异常 erro code=[%s]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }

	switch( atoi(tx_type) )
	{
	  case 1:	/* 开户时检查 */

		/* 检查开户日期是否在启用日期和止用日期之间 */	
		if ( (g_od_parm.beg_date > g_pub_tx.tx_date) || \
			(g_pub_tx.tx_date > g_od_parm.end_date))
		{	
			strcpy(acErrMsg,"开户日期不在在启用日期和止用日期之间!");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "A001" );
			goto ErrExit;
		}
		
		/* 检查该产品的透支限额是否超过最低限额和最高限额 */
		if ( g_od_mst.max_bal>g_od_parm.max_amt)
		{
			strcpy(acErrMsg,"输入的透支金额超过最高限额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T004" );
			goto ErrExit;
		}
		
		/* 检查开户限制条件编码 */
			if ( g_td_parm.opn_lmt == 000 )
			{
				goto ErrExit;
			}
			break;
		
	  case 2:          /* 透支时检查 */
		
		/* 检查交易金额是否超过该产品的可用透支金额 */
		if ( g_pub_tx.tx_amt1> ( g_od_mst.max_bal - g_od_mst.bal ) )
		{
			strcpy(acErrMsg,"交易金额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T003" );
			goto ErrExit;
		}
		else if ( g_pub_tx.tx_amt1>g_od_parm.max_amt)
		{
			strcpy(acErrMsg,"透支金额超过最高限额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T004" );
			goto ErrExit;
		}
		break;
		
	case 3:          /* 现金透支时检查 */	
		/* 检查是否超过现金透支金额 */
		if ( g_pub_tx.tx_amt1>g_od_parm.cash_amt )
		{
			strcpy(acErrMsg,"现金透支金额超过最高限额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T005" );
			goto ErrExit;
		}
		break;
		
		
	case 4:          /* 还款时检查 */	
		/* 检查最低还款金额、最低还款比例 */
		if ( g_pub_tx.tx_amt1<g_od_parm.min_pay_amt )
		{
			strcpy(acErrMsg,"透支还款金额低过最低还款金额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T006" );
			goto ErrExit;
		}else if( g_pub_tx.tx_amt1 > g_od_parm.min_pay_ratio * g_od_mst.bal )
		  {
		  	strcpy(acErrMsg,"透支还款金额低过最低还款金额！");
			WRITEMSG
			strcpy ( g_pub_tx.reply , "T006" );
			goto ErrExit;
		  }
		break;
		
	case 5:          /* 销户时检查 */
		/* 检查销户限制条件编码 (调用函数) */
		if ( g_dd_parm.cls_lmt == 000 )
		{
			goto ErrExit;
		}		
		break;
		
	default: /* 交易类型入口参数错误  */
	   
		strcpy( acErrMsg,"交易类型入口参数错误" );
		WRITEMSG
		goto ErrExit;

	}
 return 0;
 
/* 错误出口 */
ErrExit:

		return (-1);

}
