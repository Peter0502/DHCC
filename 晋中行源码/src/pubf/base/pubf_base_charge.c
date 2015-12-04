#include "find_debug.h" 
/*************************************************
* 文 件 名: pubf_base_charge.c
* 功能描述：   
*              pub_base_GetCharge   根据收费编号取费用金额
*              pub_base_GetRate     根据费率编号获得费率
*
* 作    者:    lance
* 完成日期：   2003年12月27日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*     2. 
*************************************************/
#include <stdio.h>  
#define EXTERN
#include "public.h"
#include "mo_def_rate_c.h"
#include "mo_def_agio_c.h"
#include "com_chrg_rate_c.h"

struct com_chrg_rate_c Gl_com_chrg_rate_c;

/**********************************************************************
* 函 数 名：  pub_base_GetCharge
* 函数功能：  根据收费编号取费用金额
* 作    者：  lance
* 完成时间：  2003年12月27日
*
* 参    数：
*     输  入：cif_lvl      客户级别
*             chrg_code    费用编号
*             amt          金额
*      
*     输  出: charge       费用金额
*
*     返回值： 0 成功
*              1 不存在
*             -1 错误
*
* 修改历史：
*                   
********************************************************************/
int pub_base_GetCharge(char *cif_lvl,char *chrg_code,double amt,double *charge){

	struct mo_def_rate_c	mo_def_rate_c;
	struct com_chrg_rate_c  com_chrg_rate_c;
	struct mo_def_agio_c	mo_def_agio_c;
	struct pub_tx pub_tx; 

	int    ret;
	double cif_lvl_rate;
	char   chrg_rate_no[4];
	int    beg_date;
	int    end_date;
	char   type[2];
    double val;
    double min_amt;
	double max_amt;
    int    date;
	char   chrg_rate_code[4];
	
    /* 根据分段费率表取分段费率结构 */
    ret = Mo_def_rate_Sel(g_pub_tx.reply, &mo_def_rate_c , \
			  "chrg_code='%s' \
			  and min_amt<=%lf and max_amt>=%lf" , chrg_code , amt ) ;

    if( ret==100 )
	{
		sprintf( acErrMsg,"分段费率表无记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取分段费率表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    /* 根据客户级别和交易编码取优惠利率 */
    ret = Mo_def_agio_Sel(g_pub_tx.reply, &mo_def_agio_c , "cif_lvl='%s' \
			  and tx_code='%s'" , cif_lvl , g_pub_tx.tx_code );
    if( ret==100 )
	{
		sprintf( acErrMsg,"优惠定义表无记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取优惠定义表记录异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    	
    	    
    /* 提取费率编号和客户优惠比率 */
	cif_lvl_rate = mo_def_agio_c.cif_lvl_rate;
    strcpy( chrg_rate_no , mo_def_rate_c.chrg_rate_no );
    
	/* 根据费率编号查询公共费率表, 取出费率类型(金额或比率)、费率值以及收费上限
	   金额和收费下限金额 */

    ret = Com_chrg_rate_Sel(g_pub_tx.reply, &com_chrg_rate_c ,
			 " chrg_rate_code='%s' " , chrg_rate_code ) ;

    if( ret==100 )
	{
		sprintf( acErrMsg,"公共费率表无记录 erro code=[%s]",ret);
		WRITEMSG
		strcpy( pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
		{
    		sprintf( acErrMsg,"取公共费率表异常 erro code=[%s]",ret);
			WRITEMSG
			strcpy( pub_tx.reply ,"D103" );
    		return -1;
    	}
    
	/* 提取费率类型(金额或比率)、费率值以及收费上限金额和收费下限金额和日期 */

        strcpy( type , com_chrg_rate_c.type );
        beg_date = com_chrg_rate_c.beg_date;
		end_date = com_chrg_rate_c.end_date;
		val = com_chrg_rate_c.val;
    	min_amt = com_chrg_rate_c.min_amt;
        max_amt = com_chrg_rate_c.max_amt;

    /* 与传入的值进行比较 */

    /* 日期检查 */  
    /* 调用取系统日期函数取得系统日期 */
	if ( date>end_date )
	{ 
		sprintf( acErrMsg,"日期超过定义的日期上限 end_date=[%d]",end_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W001");
		return -1;
    }else if ( date<beg_date )
		{
    		sprintf( acErrMsg,"日期低于定义的日期下限 beg_date=[%d]",beg_date);
			WRITEMSG
			strcpy( pub_tx.reply ,"W002");
			return -1;
		}

    /* 费率类型判断 */

    if ( type[0]==1 )  /* 金额 */
	{   
		*charge=val*cif_lvl_rate;
	}else
	  {
		*charge=amt*val*cif_lvl_rate;
	  }
    
	/* 费用金额检查，检查是否超过费用金额的上下限 */

    if ( *charge > max_amt )
	{
		*charge=max_amt;
	}else if ( *charge < min_amt )
	  {
		*charge=min_amt;
	  }

 	return 0; 
}

/**********************************************************************
* 函 数 名：  pub_base_GetRate
* 函数功能：  根据费率编号获得费率
* 作    者：  lance
* 完成时间：  2003年12月27日
*
* 参    数：
*     输  入: chrg_rate_code 费率编号
*
*     输  出: val            费率
*
*     返回值：0 成功
*             1 不存在
*            -1 错误
*
* 修改历史：
*
********************************************************************/
int pub_base_GetRate(char *chrg_rate_code,long tx_date,double *val){
	
	struct com_chrg_rate_c	com_chrg_rate_c;
	int    ret;	

	if( !strcmp(Gl_com_chrg_rate_c.chrg_rate_code,chrg_rate_code) &&
		Gl_com_chrg_rate_c.beg_date<=tx_date &&
		Gl_com_chrg_rate_c.end_date>tx_date )
	{
		MEMCPY_DEBUG(&com_chrg_rate_c,&Gl_com_chrg_rate_c,sizeof(Gl_com_chrg_rate_c));
		ret=0;
	}
	else
    ret = Com_chrg_rate_Sel(g_pub_tx.reply, &com_chrg_rate_c ,
			 " chrg_rate_code='%s' and beg_date <= %ld and end_date > %ld " ,
			  chrg_rate_code, tx_date, tx_date);

    if( ret==100 )
	{
		if(! strcmp(SLBH, chrg_rate_code) && tx_date < 19991101){  /*定期19991101之前无利息税, gujy*/
			Gl_com_chrg_rate_c.val = 0;
			*val = 0;
			return 0;
		}
		sprintf( acErrMsg,"公共费率表无记录 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D104" );
		return 1;
	}else if( ret !=0 )
	{
    	sprintf( acErrMsg,"取公共费率表异常 erro code=[%d]",ret);
		WRITEMSG
		strcpy( g_pub_tx.reply ,"D103" );
    	return -1;
    }
		MEMCPY_DEBUG( &Gl_com_chrg_rate_c,&com_chrg_rate_c,sizeof(Gl_com_chrg_rate_c));
    
	/* 提取费率类型(金额或比率)、费率值以及收费上限金额和收费下限金额和日期 */


		*val = com_chrg_rate_c.val;

    /* 日期检查 *  
    * 调用取系统日期函数取得系统日期 *
	if ( g_pub_tx.tx_date>end_date )
	{ 
		sprintf( acErrMsg,"日期超过定义的日期上限 end_date=[%d]",end_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W001");
		return -1;
    }
	else if ( g_pub_tx.tx_date<beg_date )
	{
    	sprintf( acErrMsg,"日期低于定义的日期下限 beg_date=[%d]",beg_date);
		WRITEMSG
		strcpy( pub_tx.reply ,"W002");
		return -1;
	} */
    
	return 0;
}

