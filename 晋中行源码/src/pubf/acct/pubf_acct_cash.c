/***************************************************************
* 文 件 名:     pubf_acct_cash.c
* 功能描述：	现金收入、付出
*              1、根据交易机构号和柜员编码取柜员现金账号；
*              2、修改现金台帐；
*              3、登记现金明细；
*
* 作    者:	jack
* 完成日期：    2003年12月27日
*
* 修改记录：
*    日    期:
*    修 改 人:
*    修改内容:
*
**************************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "cash_mst_c.h"
#include "cash_mst_hst_c.h"

pub_acct_cash()
{
	int ret=0;

	struct cash_mst_c	cash_mst_c;
	struct cash_mst_hst_c	cash_mst_hst_c;
	memset(&cash_mst_c,0x00,sizeof(struct cash_mst_c));
	memset(&cash_mst_hst_c,0x00,sizeof(struct cash_mst_hst_c));

	strcpy( g_pub_tx.ct_ind,"1" );
	strcpy( g_pub_tx.opn_br_no,g_pub_tx.tx_br_no );

	/*** 根据交易机构号和柜员编码取现金账号ID ***/
	ret=Cash_mst_Dec_Upd(g_pub_tx.reply,
		" br_no='%s' and cur_no='%s' and tel='%s' ",
		g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	ret=Cash_mst_Fet_Upd(&cash_mst_c,g_pub_tx.reply);
	vtcp_log("brno[%s] cur_no[%s] tel[%s]",g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
	if( ret==100 )
	{
		sprintf(acErrMsg,"此柜员无钱箱[%s][%s][%s]!",
			g_pub_tx.tx_br_no,g_pub_tx.cur_no,g_pub_tx.tel);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P154");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}

	/* 检查币种是否存在，是否启用 */
	ret = pub_base_ChkCurno( g_pub_tx.cur_no );
	if ( ret )
	{
		sprintf( acErrMsg, "检查币种错！" );
		WRITEMSG
		goto ErrExit;
	}

	if( cash_mst_c.sts[0]!='0' )
	{
		sprintf(acErrMsg,"现金账号未启用或状态不正常!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P150");
		goto ErrExit;
	}
	strcpy(g_pub_tx.ac_no,cash_mst_c.ac_no);

	if( g_pub_tx.add_ind[0]=='0' && g_pub_tx.tx_amt1-cash_mst_c.bal>=0.01)
	{
		sprintf(acErrMsg,"现金不足!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P152");
		goto ErrExit;
	}

    if (g_pub_tx.add_ind[0]=='0')
		cash_mst_c.bal=cash_mst_c.bal-g_pub_tx.tx_amt1;
	else if( g_pub_tx.add_ind[0]=='1' )
		cash_mst_c.bal=cash_mst_c.bal+g_pub_tx.tx_amt1;
	else
	{
		sprintf(acErrMsg,"增减标志错误![%s]",g_pub_tx.add_ind);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P106");
		goto ErrExit;
	}

	ret=Cash_mst_Upd_Upd(cash_mst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"钱箱主文件异常，请与中心机房联系!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P153");
		goto ErrExit;
	}
	Cash_mst_Clo_Upd();

	strcpy( g_pub_tx.dc_code,NULL_DC_CODE );

	/*** 登记钱箱明细 ***/
	cash_mst_hst_c.trace_no=g_pub_tx.trace_no;
    vtcp_log("B TRACE [%d]",g_pub_tx.trace_no );
	cash_mst_hst_c.trace_cnt=g_pub_tx.trace_cnt;
	strcpy(cash_mst_hst_c.br_no,g_pub_tx.tx_br_no);
	strcpy(cash_mst_hst_c.ac_no,g_pub_tx.ac_no);
	strcpy(cash_mst_hst_c.tw_ac_no,g_pub_tx.name);
	strcpy(cash_mst_hst_c.tel,g_pub_tx.tel);
	strcpy(cash_mst_hst_c.cur_no,g_pub_tx.cur_no);
	strcpy(cash_mst_hst_c.add_ind,g_pub_tx.add_ind);
	cash_mst_hst_c.amt=g_pub_tx.tx_amt1;
	cash_mst_hst_c.tx_date=g_pub_tx.tx_date;
	cash_mst_hst_c.tx_time=g_pub_tx.tx_time;
	strcpy(cash_mst_hst_c.brf,g_pub_tx.brf);
	strcpy(cash_mst_hst_c.cash_code,g_pub_tx.cash_code);
	ret=Cash_mst_hst_Ins(cash_mst_hst_c,g_pub_tx.reply);
	if( ret )
	{
		sprintf(acErrMsg,"登记钱箱明细出错![%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P155");
		goto ErrExit;
	}

	if( pub_ins_trace_log() )
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data(DC_CODE,g_pub_tx.dc_code);

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"现金处理成功!");
	WRITEMSG
	return 0;
	
ErrExit:
	sprintf(acErrMsg,"现金处理失败![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}
