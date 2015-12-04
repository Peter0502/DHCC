/*************************************************
* 文 件 名:  sp9771.c
* 功能描述： 外汇兑换子交易
*			 根据卖出金额查询买入金额
*			 输入:sale_amt
*			 输出:buy_amt
*
* 作    者:  jane
* 完成日期： 2003年4月06日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "com_exch_rate_c.h"

int sp9771()
{
	double buy_acc,sale_acc,buy_amt,sale_amt;
	char buy_curno[3],sale_curno[3];

	int	ret;

	struct com_exch_rate_c exch_rate;
	
	pub_base_sysinit();

	get_zd_data("0210",buy_curno);		/*买入币种*/
	get_zd_data("0220",sale_curno);		/*卖出币种*/
    get_zd_double("0961",&buy_acc);		/*买入牌价*/
    get_zd_double("0962",&sale_acc);	/*卖出牌价*/
    get_zd_double("0410",&sale_amt);	/*卖出金额*/

	if( pub_base_CompDblVal(sale_amt,0.00)==0)
	{
		strcpy( g_pub_tx.reply, "L071" );
		sprintf( acErrMsg, "买入金额为0");
		WRITEMSG
		goto ErrExit;
	}

	/*计算汇率*/
	memset(&exch_rate,0x00,sizeof(struct com_exch_rate_c));

	if( !strcmp( buy_curno,RMB ) )	/*人民币买入-外币卖出 用钞卖出价 odsale*/
	{
/******JYW
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and date<='%ld' and last_date>='%ld'",sale_curno,buy_curno,g_pub_tx.tx_date,g_pub_tx.tx_date);
********/
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and \"date\"<='%ld' ",sale_curno,buy_curno,g_pub_tx.tx_date);
		if ( ret&&ret!=100 )
		{
			sprintf( acErrMsg, "查询公共汇率表出错!curno1[%s]curno2[%s]",sale_curno,buy_curno );
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==100)
		{
			sprintf( acErrMsg, "查询公共汇率表记录不存在!curno1[%s]curno2[%s]",sale_curno,buy_curno );
			WRITEMSG
			goto ErrExit;
		}
		exch_rate.od_sale /= exch_rate.unit;

		if( sale_acc>0.00005 &&
			( ( sale_acc-exch_rate.od_sale*100.0)>0.00005 ||
			  ( sale_acc-exch_rate.od_sale*100.0)<-0.00005 ) )/*屏幕有输入卖出牌价并且不等于当日汇率*/
		{
			/*需要国际业务部授权,暂时不做*/
			strcpy( g_pub_tx.reply, "W702" );
			sprintf( acErrMsg, "需要国际业务部授权，暂时不做");
			WRITEMSG
			goto ErrExit;
		}
		else if( sale_acc<0.00005 )
		{
			sale_acc=exch_rate.od_sale*100.0;
		}
		set_zd_double("0962",sale_acc);

		buy_amt=sale_amt*sale_acc/100.0;
	}
	else if( !strcmp( sale_curno,RMB ) )	/*外币买入-人民币卖出 用钞买入价 odbuy*/
	{
/**********JYW
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and date<='%ld' and last_date>='%ld'",buy_curno,sale_curno,g_pub_tx.tx_date,g_pub_tx.tx_date);
**************/
		ret=Com_exch_rate_Sel(g_pub_tx.reply,&exch_rate,"cur_no1='%s' and cur_no2='%s' \
			and \"date\"<='%ld' ",buy_curno,sale_curno,g_pub_tx.tx_date);
		if ( ret&&ret!=100 )
		{
			sprintf( acErrMsg, "查询公共汇率表出错!curno1[%s]curno2[%s]",buy_curno,sale_curno );
			WRITEMSG
			goto ErrExit;
		}
		else if(ret==100)
		{
			sprintf( acErrMsg, "查询公共汇率表记录不存在!curno1[%s]curno2[%s]",buy_curno,sale_curno );
			WRITEMSG
			goto ErrExit;
		}
		exch_rate.od_buy /= exch_rate.unit;

		if( buy_acc>0.00005 &&
			( ( buy_acc-exch_rate.od_buy*100.0)>0.00005 ||
			  ( buy_acc-exch_rate.od_buy*100.0)<-0.00005 ) )/*屏幕有输入买入牌价并且不等于当日汇率*/
		{
			/*需要国际业务部授权,暂时不做*/
			strcpy( g_pub_tx.reply, "W702" );
			sprintf( acErrMsg, "需要国际业务部授权，暂时不做");
			WRITEMSG
			goto ErrExit;
		}
		else if( buy_acc<0.00005 )
		{
			buy_acc=exch_rate.od_buy*100.0;
		}
		set_zd_double("0961",buy_acc);

		buy_amt=sale_amt/buy_acc*100.0;
	}
	else
	{
		strcpy( g_pub_tx.reply, "W701" );
		sprintf( acErrMsg, "币种有误，买入币种与卖出币种都是外币");
		WRITEMSG
		goto ErrExit;
	}

	vtcp_log("buy_acc[%lf]sale_acc[%lf]buy_amt[%lf]sale_amt[%lf]",
			  buy_acc,sale_acc,buy_amt,sale_amt);

    set_zd_double("0400",buy_amt);	/*买入金额*/

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
