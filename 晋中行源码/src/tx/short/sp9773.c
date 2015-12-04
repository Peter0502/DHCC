/*************************************************
* 文 件 名:  sp9773.c
* 功能描述： 直接套汇子交易
*			 根据买入金额计算卖出金额
*			 或根据卖出金额计算买入金额
*
* 入口参数：flag_amt	计算方向
*						1：根据买入金额计算卖出金额
*						2：根据卖出金额计算买入金额
*			buy_curno	买入币种
*			sale_curno	卖出币种
*			buy_acc		买入牌价
*			sale_acc	卖出牌价
* 			buy_amt		买入金额
*			或sale_amt	卖出金额
*
* 出口参数：buy_amt		买入金额
*			或sale_amt	卖出金额
*
* 作    者:  jane
* 完成日期： 2003年4月15日
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

int sp9773()
{
	char flag_amt[2];
	double buy_acc,sale_acc,buy_amt,sale_amt,RMB_amt;
	char buy_curno[3],sale_curno[3];

	int	ret;

	struct com_exch_rate_c buy_exch_rate;
	struct com_exch_rate_c sale_exch_rate;

	pub_base_sysinit();

	get_zd_data("0690",flag_amt);	/*计算方向*/
	get_zd_data("0210",buy_curno);	/*买入币种*/
	get_zd_data	("0220",sale_curno);/*卖出币种*/
	get_zd_double("0961",&buy_acc);	/*买入牌价*/
	get_zd_double("0962",&sale_acc);/*卖出牌价*/
	get_zd_double("0400",&buy_amt);	/*买入金额*/
	get_zd_double("0410",&sale_amt);/*卖出金额*/

	if( (pub_base_CompDblVal(buy_amt,0.00)==0 )&&(pub_base_CompDblVal(sale_amt,0.00)==0 ))
	{
		strcpy( g_pub_tx.reply, "W700" );
		sprintf( acErrMsg, "买入卖出金额都为0");
		WRITEMSG
		goto ErrExit;
	}

	if( !strcmp( buy_curno,RMB )||!strcmp( sale_curno,RMB) )
	{
		strcpy( g_pub_tx.reply, "W703" );
		sprintf( acErrMsg, "币种不能是人民币");
		WRITEMSG
		goto ErrExit;
	}

	/*计算汇率*/
	memset(&buy_exch_rate,0x00,sizeof(struct com_exch_rate_c));
	memset(&sale_exch_rate,0x00,sizeof(struct com_exch_rate_c));

	/*外币买入-人民币卖出 用钞买入价 odbuy*/
/**********JYW
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&buy_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and date<='%ld' and last_date>='%ld'",buy_curno,RMB,g_pub_tx.tx_date,g_pub_tx.tx_date);
***********/
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&buy_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and \"date\"<='%ld'",buy_curno,RMB,g_pub_tx.tx_date);
	if ( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "查询公共汇率表出错!curno1[%s]curno2[%s]",buy_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf( acErrMsg, "查询公共汇率表记录不存在!curno1[%s]curno2[%s]",buy_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	buy_exch_rate.od_buy /= buy_exch_rate.unit;

	if( buy_acc>0.00005 &&
		( ( buy_acc-buy_exch_rate.od_buy*100.0)>0.00005 ||
		  ( buy_acc-buy_exch_rate.od_buy*100.0)<-0.00005 ) )/*屏幕有输入买入牌价并且不等于当日汇率*/
	{
		/*需要国际业务部授权,暂时不做*/
		strcpy( g_pub_tx.reply, "W702" );
		sprintf( acErrMsg, "需要国际业务部授权，暂时不做");
		WRITEMSG
		goto ErrExit;
	}
	else if( buy_acc<0.00005 )
	{
		buy_acc=buy_exch_rate.od_buy*100.0;
	}

	/*人民币买入-外币卖出 用钞卖出价 odsale*/
/************JYW
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&sale_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and date<='%ld' and last_date>='%ld'",sale_curno,RMB,g_pub_tx.tx_date,g_pub_tx.tx_date);
*************/
	ret=Com_exch_rate_Sel(g_pub_tx.reply,&sale_exch_rate,"cur_no1='%s' and cur_no2='%s' \
		and \"date\"<='%ld' ",sale_curno,RMB,g_pub_tx.tx_date);
	if ( ret&&ret!=100 )
	{
		sprintf( acErrMsg, "查询公共汇率表出错!curno1[%s]curno2[%s]",sale_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	else if(ret==100)
	{
		sprintf( acErrMsg, "查询公共汇率表记录不存在!curno1[%s]curno2[%s]",sale_curno,RMB );
		WRITEMSG
		goto ErrExit;
	}
	sale_exch_rate.od_sale /= sale_exch_rate.unit;

	if( sale_acc>0.00005 &&
		( ( sale_acc-sale_exch_rate.od_sale*100.0)>0.00005 ||
		  ( sale_acc-sale_exch_rate.od_sale*100.0)<-0.00005 ) )/*屏幕有输入卖出牌价并且不等于当日汇率*/
	{
		/*需要国际业务部授权,暂时不做*/
		strcpy( g_pub_tx.reply, "W702" );
		sprintf( acErrMsg, "需要国际业务部授权，暂时不做");
		WRITEMSG
		goto ErrExit;
	}
	else if( sale_acc<0.00005 )
	{
		sale_acc=sale_exch_rate.od_sale*100.0;
	}

	if(flag_amt[0]=='1')/*根据买入金额计算卖出金额*/
	{
		vtcp_log("一般套汇根据买入金额计算卖出金额");
		RMB_amt=buy_amt*buy_acc/100.0;
		vtcp_log("RMB_amt[%lf]=buy_amt[%lf]*buy_acc[%lf]/100",
				buy_acc,buy_amt,RMB_amt);
		sale_amt=RMB_amt/sale_acc*100.0;
		vtcp_log("sale_amt[%lf]=RMB_amt[%lf]/sale_acc[%lf]*100",
				sale_amt,RMB_amt,sale_acc);
		set_zd_double("0410",sale_amt);	/*卖出金额*/
	}
	else if(flag_amt[0]=='2')/*根据卖出金额计算买入金额*/
	{
		vtcp_log("一般套汇根据卖出金额计算买入金额");
		RMB_amt=sale_amt*sale_acc/100.0;
		vtcp_log("RMB_amt[%lf]=sale_amt[%lf]*sale_acc[%lf]/100",
				RMB_amt,sale_amt,sale_acc);
		buy_amt=RMB_amt/buy_acc*100.0;
		vtcp_log("buy_amt[%lf]=RMB_amt[%lf]/buy_acc[%lf]*100",
				buy_amt,RMB_amt,buy_acc);
		set_zd_double("0400",buy_amt);	/*买入金额*/
	}

	set_zd_double("0961",buy_acc);
	set_zd_double("0962",sale_acc);

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
