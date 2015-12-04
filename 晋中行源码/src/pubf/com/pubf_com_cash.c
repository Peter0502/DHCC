#include "find_debug.h" 
/***************************************************************
* 文 件 名:     pubf_com_cash.c
* 功能描述：    柜员现金操作函数组
*              1、
*            
* 作    者:    jack
* 完成日期：    2003年1月12日
*
* 修改记录：    
* 1. 日    期: 	
*    修 改 人: 	
*    修改内容: 	
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

/**********************************************************************
* 函数名：    	pub_com_ChkTelCash
* 函数功能：    	根据机构号、柜员、币种检查该柜员的钱箱（现金帐号）是否存在，
*		状态是否正常
* 作者/时间：    jack//2003年1月12日
* 
* 参数：
*     输入： 
*	    	char 	*br_no	机构号码
*		char	*tel	柜员号码
*		char	*cur_no	币种
*           
*     输出： 
*		g_pub_tx.reply	响应码
* 返 回 值: 0 成功,-1 失败
*           
* 修改历史：
*
********************************************************************/

int pub_com_ChkTelCash( char *br_no, char *tel, char *cur_no )
{    
	struct cash_mst_c sCashMst;
	
	int	ret;
 	
 	MEMSET_DEBUG( &sCashMst, 0x00, sizeof( struct cash_mst_c ) );
 	
 	ret = Cash_mst_Sel( g_pub_tx.reply, &sCashMst, "br_no = '%s' \
 			and tel = '%s' and cur_no = '%s'", br_no, tel, cur_no );
 	if ( ret == 100 )
 	{
 		sprintf( acErrMsg, "不存在柜员钱箱，不能做现金交易" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P154" );
 		return(-1);
 	}else if ( ret )
 	{
 		sprintf( acErrMsg, "查找柜员钱箱错ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);
 	}
 	
 	if ( sCashMst.sts[0] != 'Y' )
 	{
 		sprintf( acErrMsg, "现金账号未启用或状态不正常" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P150" );
 		return(-1);
 	}
 	
	return 0; 
}

/**********************************************************************
* 函数名：    	pub_com_RegCashMst
* 函数功能：    	登记/修改柜员钱箱（现金帐号）,如果修改后帐户余额小于0，则错
*		如果是收入，则加上发生额，如果付出，减去发生额，最后判断帐户
*		余额是否小于0。
* 作者/时间：    jack//2003年1月12日
* 
* 参数：
*     输入： 
*	    	struct cash_mst_c *cash_mst
*			br_no	机构号码
*			tel	柜员号码
*			cur_no	币种	
*		double	amt		 发生额 
*
*     输出： 
*		struct cash_mst_c *cash_mst
*		g_pub_tx.reply	响应码
* 返 回 值: 0 成功,-1 失败
*           
* 修改历史：
*
********************************************************************/

int pub_com_RegCashMst(
		struct cash_mst_c *cash_mst, 
		double amt )
{
	int	ret;
 	
 	ret = Cash_mst_Dec_Upd ( g_pub_tx.reply, "br_no = '%s' and tel = '%s' \
 				and cur_no = '%s'", cash_mst->br_no, \
 				cash_mst->tel, cash_mst->cur_no );
 	if ( ret )
 	{
 		sprintf( acErrMsg, "执行Cash_mst_Dec_Sel错，ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);
 	}

	ret = Cash_mst_Fet_Upd( cash_mst );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "不存在柜员钱箱，不能做现金交易" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P154" );
		return(-1);
	}else if ( ret )
	{
 		sprintf( acErrMsg, "执行Cash_mst_Fet_Sel错，ret=[%d]", ret );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P153" );
 		return(-1);		
	}

 	/* 判断是收还是付，计算帐户余额 */
 	if ( g_pub_tx.add_ind[0] == '1' )
 	{
 		cash_mst->bal += amt;
 	}else if ( g_pub_tx.add_ind[0] == '2' )
 	{
 		cash_mst->bal -= amt;
 	}
 	
 	/* 比较余额是否小于0，如果小，则消失发生额大于帐户余额 */
 	if ( pub_base_CompDblVal( cash_mst->bal , 0.00 ) < 0 )
 	{
 		sprintf( acErrMsg, "柜员现金不足，请检查输入的发生额" );
 		WRITEMSG
 		strcpy( g_pub_tx.reply, "P152" );
 		return(-1);
 	}
	 	
 	/* 修改现金台帐 */
 	ret = Cash_mst_Upd_Upd( *cash_mst, g_pub_tx.reply );
 	if ( ret )
	{
		sprintf( acErrMsg, "钱箱主文件异常，请与中心机房联系" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "P153" );
		return(-1);	
	}
	
	Cash_mst_Clo_Upd();
	
	return 0; 
}

/**********************************************************************
* 函数名：    	pub_com_RegCashHst
* 函数功能：    	登记柜员现金台帐明细
*
* 作者/时间：    jack//2003年1月12日
* 
* 参数：
*     输入： 
*	    	struct cash_mst_c cash_mst
*			br_no	机构号码
*			tel	柜员号码
*			cur_no	币种	
*
*     输出： 
*		g_pub_tx.reply	响应码
* 返 回 值: 0 成功,-1 失败
*           
* 修改历史：
*
********************************************************************/

int pub_com_RegCashHst(	struct cash_mst_c cash_mst )
{
	struct cash_mst_hst_c sCashHst;
	int	ret;
 	
 	MEMSET_DEBUG( &sCashHst, 0x00, sizeof( struct cash_mst_hst_c ) );
 	
 	sCashHst.trace_no = g_pub_tx.trace_no;	/*主机流水号*/
	sCashHst.trace_cnt = g_pub_tx.trace_cnt;	/*交易流水序号*/
	strcpy( sCashHst.br_no , cash_mst.br_no );	/*机构*/
	strcpy( sCashHst.ac_no , cash_mst.ac_no );	/*账号*/
	strcpy( sCashHst.tel, cash_mst.tel );		/*柜员*/
	strcpy( sCashHst.cur_no , cash_mst.cur_no );	/*币种*/
	sprintf( sCashHst.add_ind , g_pub_tx.add_ind ); 	/*收付标志*/
	sCashHst.amt =	g_pub_tx.tx_amt1;		/*发生额*/
	sCashHst.tx_date = g_pub_tx.tx_date;		/*流水日期*/
	sCashHst.tx_time = g_pub_tx.tx_time;		/*交易时间*/
	strcpy( sCashHst.brf ,	g_pub_tx.brf );		/*摘要*/
	strcpy( sCashHst.cash_code , g_pub_tx.cash_code );	/*现金代码*/

 	ret = Cash_mst_hst_Ins( sCashHst, g_pub_tx.reply );
 	if ( ret )
 	{
                sprintf( acErrMsg, "登记钱箱明细出错![%d]", ret );
                WRITEMSG
                strcpy( g_pub_tx.reply, "P155" );
                return(-1);
 	}

	return 0; 
}
