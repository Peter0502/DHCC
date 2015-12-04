#include "card.h"
#include <fcntl.h>
#include "card_reg_c.h"
#include "public.h"

int pub_card_reg(double txamt,char * cTradtype,char add_ind,int * atm_qy_free_flag,int * pos_qy_free_flag,int *pos_tx_free_flag,int * atm_tx_free_flag,int iPselflag,int iDscpt)
{
	struct card_reg_c sCard_reg;
	char cTxday[9];
	vtcp_log("%s,%d 传入的参数入下",__FILE__,__LINE__);
	vtcp_log("%s,%d txamt=[%f],tradtype=[%s]",__FILE__,__LINE__,txamt,cTradtype);
	vtcp_log("%s,%d add_ind=[%c],atm_qy_free_flag=[%d]",__FILE__,__LINE__,add_ind,atm_qy_free_flag);
	vtcp_log("%s,%d iPselflag=[%d],iDscpt=[%d]",__FILE__,__LINE__,iPselflag,iDscpt);
	memset(&sCard_reg, 0  , sizeof(sCard_reg));
	g_reply_int=Card_reg_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.crd_no);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,declare cursor error ret=[%d]",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	g_reply_int=Card_reg_Fet_Upd(&sCard_reg,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,open card_reg curor error ret=[%d]",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
	memset(cTxday, 0 , sizeof(cTxday));
	sprintf(cTxday,"%08d",g_pub_tx.tx_date);
	/*  对季度处理 */
	if(sCard_reg.l_tx_date/100 != g_pub_tx.tx_date/100 && 
		( memcmp(cTxday+4,"01",2)==0 
		||memcmp(cTxday+4,"04",2)==0 
		||memcmp(cTxday+4,"07",2)==0
		||memcmp(cTxday+4,"10",2)==0)
		)
	{
		sCard_reg.atm_ts_dr_cash_tx_amt   = 0;/*atm本季现金取款交易金额*/
		sCard_reg.atm_ts_dr_cash_tx_cnt   = 0;/*atm本季现金取款交易笔数*/
		sCard_reg.atm_ts_dr_tran_tx_amt   = 0;/*atm本季转帐取款交易金额*/
		sCard_reg.atm_ts_dr_tran_tx_cnt   = 0;/*atm本季转帐取款交易笔数*/

		sCard_reg.atm_ts_cr_cash_tx_amt   = 0;/*atm本季现金存款交易金额*/
		sCard_reg.atm_ts_cr_cash_tx_cnt   = 0;/*atm本季现金存款交易笔数*/
		sCard_reg.atm_ts_cr_tran_tx_amt   = 0;/*atm本季转帐存款交易金额*/
		sCard_reg.atm_ts_cr_tran_tx_cnt   = 0;/*atm本季转帐存款交易笔数*/

		sCard_reg.atm_ts_qy_cnt        = 0;/*atm本季查询笔数*/
		sCard_reg.pos_ts_qy_cnt        = 0;/* pos本季查询笔数 **/
		sCard_reg.pos_ts_tx_amt        = 0;/*pos本季交易金额 **/
		sCard_reg.pos_ts_tx_cnt        = 0;/* pos本季交易笔数 **/

		sCard_reg.wh_atm_ts_dr_cash_tx_amt= 0;/*当季外行atm现金取款交易金额*/
		sCard_reg.wh_atm_ts_dr_cash_tx_cnt= 0;/*当季外行atm现金取款交易笔数*/
		sCard_reg.wh_atm_ts_dr_tran_tx_amt= 0;/*当季外行atm转帐取款交易金额*/
		sCard_reg.wh_atm_ts_dr_tran_tx_cnt= 0;/*当季外行atm转帐取款交易笔数*/
                                                               
		sCard_reg.wh_atm_ts_cr_cash_tx_amt= 0;/*当季外行atm现金存款交易金额*/
		sCard_reg.wh_atm_ts_cr_cash_tx_cnt= 0;/*当季外行atm现金存款交易笔数*/
		sCard_reg.wh_atm_ts_cr_tran_tx_amt= 0;/*当季外行atm转帐存款交易金额*/
		sCard_reg.wh_atm_ts_cr_tran_tx_cnt= 0;/*当季外行atm转帐存款交易笔数*/

		sCard_reg.wh_atm_ts_qy_cnt     = 0;/*当季外行atm查询笔数*/

		sCard_reg.wh_pos_ts_qy_cnt     = 0;/*当季外行pos查询笔数*/
		sCard_reg.wh_pos_ts_tx_amt     = 0;/*当季外行pos交易金额*/ 
		sCard_reg.wh_pos_ts_tx_cnt     = 0;/*当季外行pos交易笔数*/ 
	}
	/* 对年的纪录的处理 */
	if(sCard_reg.l_tx_date/10000 != g_pub_tx.tx_date/10000)
	{
		sCard_reg.atm_ty_dr_cash_tx_amt   = 0;/*atm本年现金取款交易金额*/
		sCard_reg.atm_ty_dr_cash_tx_cnt   = 0;/*atm本年现金取款交易笔数*/
		sCard_reg.atm_ty_dr_tran_tx_amt   = 0;/*atm本年转帐取款交易金额*/
		sCard_reg.atm_ty_dr_tran_tx_cnt   = 0;/*atm本年转帐取款交易笔数*/

		sCard_reg.atm_ty_cr_cash_tx_amt   = 0;/*atm本年现金存款交易金额*/
		sCard_reg.atm_ty_cr_cash_tx_cnt   = 0;/*atm本年现金存款交易笔数*/
		sCard_reg.atm_ty_cr_tran_tx_amt   = 0;/*atm本年转帐存款交易金额*/
		sCard_reg.atm_ty_cr_tran_tx_cnt   = 0;/*atm本年转帐存款交易笔数*/

		sCard_reg.atm_ty_qy_cnt        = 0;/*atm本年查询笔数*/
		sCard_reg.pos_ty_qy_cnt        = 0;/* pos本年查询笔数 **/
		sCard_reg.pos_ty_tx_amt        = 0;/*pos本年交易金额 **/
		sCard_reg.pos_ty_tx_cnt        = 0;/* pos本年交易笔数 **/

		sCard_reg.wh_atm_ty_dr_cash_tx_amt= 0;/*当年外行atm现金取款交易金额*/
		sCard_reg.wh_atm_ty_dr_cash_tx_cnt= 0;/*当年外行atm现金取款交易笔数*/
		sCard_reg.wh_atm_ty_dr_tran_tx_amt= 0;/*当年外行atm转帐取款交易金额*/
		sCard_reg.wh_atm_ty_dr_tran_tx_cnt= 0;/*当年外行atm转帐取款交易笔数*/

		sCard_reg.wh_atm_ty_cr_cash_tx_amt= 0;/*当年外行atm现金存款交易金额*/
		sCard_reg.wh_atm_ty_cr_cash_tx_cnt= 0;/*当年外行atm现金存款交易笔数*/
		sCard_reg.wh_atm_ty_cr_tran_tx_amt= 0;/*当年外行atm转帐存款交易金额*/
		sCard_reg.wh_atm_ty_cr_tran_tx_cnt= 0;/*当年外行atm转帐存款交易笔数*/

		sCard_reg.wh_atm_ty_qy_cnt     = 0;/*当年外行atm查询笔数*/
		sCard_reg.wh_pos_ty_qy_cnt     = 0;/*当年外行pos查询笔数*/
		sCard_reg.wh_pos_ty_tx_amt     = 0;/*当年外行pos交易金额*/
		sCard_reg.wh_pos_ty_tx_cnt     = 0;/*当年外行pos交易笔数*/
	}
	/* 对月的记录处理 */
	if(sCard_reg.l_tx_date/100 != g_pub_tx.tx_date/100)
	{
		sCard_reg.atm_tm_dr_cash_tx_amt   = 0;/*atm本月现金取款交易金额*/
		sCard_reg.atm_tm_dr_cash_tx_cnt   = 0;/*atm本月现金取款交易笔数*/
		sCard_reg.atm_tm_dr_tran_tx_amt   = 0;/*atm本月转帐取款交易金额*/
		sCard_reg.atm_tm_dr_tran_tx_cnt   = 0;/*atm本月转帐取款交易笔数*/                                                         
		sCard_reg.atm_tm_cr_cash_tx_amt   = 0;/*atm本月现金存款交易金额*/
		sCard_reg.atm_tm_cr_cash_tx_cnt   = 0;/*atm本月现金存款交易笔数*/
		sCard_reg.atm_tm_cr_tran_tx_amt   = 0;/*atm本月转帐存款交易金额*/
		sCard_reg.atm_tm_cr_tran_tx_cnt   = 0;/*atm本月转帐存款交易笔数*/

		sCard_reg.atm_tm_qy_cnt        = 0;/*atm本月查询笔数*/
		sCard_reg.pos_tm_qy_cnt        = 0;/* pos本月查询笔数 **/
		sCard_reg.pos_tm_tx_amt        = 0;/*pos本月交易金额 **/
		sCard_reg.pos_tm_tx_cnt        = 0;/* pos本月交易笔数 **/

		sCard_reg.wh_atm_tm_dr_cash_tx_amt= 0;/*当月外行atm现金取款交易金额*/
		sCard_reg.wh_atm_tm_dr_cash_tx_cnt= 0;/*当月外行atm现金取款交易笔数*/
		sCard_reg.wh_atm_tm_dr_tran_tx_amt= 0;/*当月外行atm转帐取款交易金额*/
		sCard_reg.wh_atm_tm_dr_tran_tx_cnt= 0;/*当月外行atm转帐取款交易笔数*/
                                                               
		sCard_reg.wh_atm_tm_cr_cash_tx_amt= 0;/*当月外行atm现金存款交易金额*/
		sCard_reg.wh_atm_tm_cr_cash_tx_cnt= 0;/*当月外行atm现金存款交易笔数*/
		sCard_reg.wh_atm_tm_cr_tran_tx_amt= 0;/*当月外行atm转帐存款交易金额*/
		sCard_reg.wh_atm_tm_cr_tran_tx_cnt= 0;/*当月外行atm转帐存款交易笔数*/

		sCard_reg.wh_atm_tm_qy_cnt     = 0;/*当月外行atm查询笔数*/
		sCard_reg.wh_pos_tm_qy_cnt     = 0;/*当月外行pos查询笔数*/
		sCard_reg.wh_pos_tm_tx_amt     = 0;/*当月外行pos交易金额*/
		sCard_reg.wh_pos_tm_tx_cnt     = 0;/*当月外行pos交易笔数*/
	}
	/* 对日的纪录的处理 */
	if(sCard_reg.l_tx_date!=g_pub_tx.tx_date)
	{
		sCard_reg.atm_td_dr_cash_tx_amt = 0; /*atm本日现金取款交易金额*/
		sCard_reg.atm_td_dr_cash_tx_cnt = 0; /*atm本日现金取款交易笔数*/
		sCard_reg.atm_td_dr_tran_tx_amt = 0; /*atm本日转帐取款交易金额*/
		sCard_reg.atm_td_dr_tran_tx_cnt = 0; /*atm本日转帐取款交易笔数*/
		sCard_reg.atm_td_cr_cash_tx_amt = 0; /*atm本日现金存款交易金额*/
		sCard_reg.atm_td_cr_cash_tx_cnt = 0; /*atm本日现金存款交易笔数*/
		sCard_reg.atm_td_cr_tran_tx_amt = 0; /*atm本日转帐存款交易金额*/
		sCard_reg.atm_td_cr_tran_tx_cnt = 0; /*atm本日转帐存款交易笔数*/

		sCard_reg.atm_td_qy_cnt = 0;   /*atm本日查询笔数*/
		sCard_reg.pos_td_qy_cnt = 0;   /*pos本日查询笔数 **/
		sCard_reg.pos_td_tx_amt = 0;   /*pos本日交易金额 **/
		sCard_reg.pos_td_tx_cnt = 0;   /*pos本日交易笔数 **/

		sCard_reg.wh_atm_td_dr_cash_tx_amt = 0;/*当日外行atm现金取款交易金额*/
		sCard_reg.wh_atm_td_dr_cash_tx_cnt = 0;/*当日外行atm现金取款交易笔数*/
		sCard_reg.wh_atm_td_dr_tran_tx_amt = 0;/*当日外行atm转帐取款交易金额*/
		sCard_reg.wh_atm_td_dr_tran_tx_cnt = 0;/*当日外行atm转帐取款交易笔数*/
                                                                
		sCard_reg.wh_atm_td_cr_cash_tx_amt = 0;/*当日外行atm现金存款交易金额*/
		sCard_reg.wh_atm_td_cr_cash_tx_cnt = 0;/*当日外行atm现金存款交易笔数*/
		sCard_reg.wh_atm_td_cr_tran_tx_amt = 0;/*当日外行atm转帐存款交易金额*/
		sCard_reg.wh_atm_td_cr_tran_tx_cnt = 0;/*当日外行atm转帐存款交易笔数*/

		sCard_reg.wh_atm_td_qy_cnt      = 0;/*当日外行atm查询笔数*/
		sCard_reg.wh_pos_td_qy_cnt = 0;     /*当日外行pos查询笔数*/
		sCard_reg.wh_pos_td_tx_amt = 0;     /* 当日外行pos交易金额 **/
		sCard_reg.wh_pos_td_tx_cnt = 0;     /*当日外行pos交易笔数*/
	}
	if(memcmp(cTradtype,"ATM",3)==0)
	{
		if(iDscpt==CARD_TRADE_CASH)
		{/* ATM现金交易 */
			if(iPselflag==1)
			{
				if(add_ind=='0')
				{
					sCard_reg.wh_atm_td_dr_cash_tx_amt += txamt;  /*当日外行atm现金取款交易金额*/
					sCard_reg.wh_atm_tm_dr_cash_tx_amt += txamt;  /*当月外行atm现金取款交易金额*/
					sCard_reg.wh_atm_ts_dr_cash_tx_amt += txamt;  /*当季外行atm现金取款交易金额*/
					sCard_reg.wh_atm_ty_dr_cash_tx_amt += txamt;  /*当年外行atm现金取款交易金额*/
					sCard_reg.wh_atm_td_dr_cash_tx_cnt ++;                  /*当日外行atm现金取款交易笔数*/
					sCard_reg.wh_atm_tm_dr_cash_tx_cnt ++;                  /*当月外行atm现金取款交易笔数*/
					sCard_reg.wh_atm_ts_dr_cash_tx_cnt ++;                  /*当季外行atm现金取款交易笔数*/
					sCard_reg.wh_atm_ty_dr_cash_tx_cnt ++;                  /*当年外行atm现金取款交易笔数*/	
				}
				else if(add_ind=='1')
				{
					sCard_reg.wh_atm_td_cr_cash_tx_amt += txamt;  /*当日外行atm现金存款交易金额*/
					sCard_reg.wh_atm_tm_cr_cash_tx_amt += txamt;  /*当月外行atm现金存款交易金额*/
					sCard_reg.wh_atm_ts_cr_cash_tx_amt += txamt;  /*当季外行atm现金存款交易金额*/
					sCard_reg.wh_atm_ty_cr_cash_tx_amt += txamt;  /*当年外行atm现金存款交易金额*/
					sCard_reg.wh_atm_td_cr_cash_tx_cnt ++;                  /*当日外行atm现金存款交易笔数*/
					sCard_reg.wh_atm_tm_cr_cash_tx_cnt ++;                  /*当月外行atm现金存款交易笔数*/
					sCard_reg.wh_atm_ts_cr_cash_tx_cnt ++;                  /*当季外行atm现金存款交易笔数*/
					sCard_reg.wh_atm_ty_cr_cash_tx_cnt ++;                  /*当年外行atm现金存款交易笔数*/	
				}
			}
			if(add_ind=='0')
			{
				sCard_reg.atm_td_dr_cash_tx_amt += txamt;  /*atm本日现金取款交易金额*/
				sCard_reg.atm_tm_dr_cash_tx_amt += txamt;  /*atm本月现金取款交易金额*/
				sCard_reg.atm_ts_dr_cash_tx_amt += txamt;  /*atm本季现金取款交易金额*/
				sCard_reg.atm_ty_dr_cash_tx_amt += txamt;  /*atm本年现金取款交易金额*/
				sCard_reg.atm_td_dr_cash_tx_cnt ++;                  /*atm本日现金取款交易笔数*/
				sCard_reg.atm_tm_dr_cash_tx_cnt ++;                  /*atm本月现金取款交易笔数*/
				sCard_reg.atm_ts_dr_cash_tx_cnt ++;                  /*atm本季现金取款交易笔数*/
				sCard_reg.atm_ty_dr_cash_tx_cnt ++;                  /*atm本年现金取款交易笔数*/
				vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
				if(pub_base_CompDblVal(sCard_reg.atm_td_dr_cash_tx_amt-sCard_reg.atm_max_dr_cash_tx_amt)>0)
				{
					vtcp_log("%s,%d 当日atm现金取款交易金额大于约定最大的交易金额",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
					return 1;
				}
				/***del by hxc 091115 晋中商行卡取消单日取款笔数限制 begin**/
				/**
				if(sCard_reg.atm_td_dr_cash_tx_cnt>sCard_reg.atm_max_dr_cash_tx_cnt)
				{
					vtcp_log("%s,%d 当日atm现金取款交易笔数大于约定最大的交易笔数",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_cnt,sCard_reg.atm_max_dr_cash_tx_cnt);
					return 2;
				}**/
				/***del by hxc 091115 晋中商行卡取消单日取款笔数限制 end**/
			}
			else if(add_ind=='1')
			{
				sCard_reg.atm_td_cr_cash_tx_amt += txamt;  /*atm本日现金存款交易金额*/
				sCard_reg.atm_tm_cr_cash_tx_amt += txamt;  /*atm本月现金存款交易金额*/
				sCard_reg.atm_ts_cr_cash_tx_amt += txamt;  /*atm本季现金存款交易金额*/
				sCard_reg.atm_ty_cr_cash_tx_amt += txamt;  /*atm本年现金存款交易金额*/
				sCard_reg.atm_td_cr_cash_tx_cnt ++;                  /*atm本日现金存款交易笔数*/
				sCard_reg.atm_tm_cr_cash_tx_cnt ++;                  /*atm本月现金存款交易笔数*/
				sCard_reg.atm_ts_cr_cash_tx_cnt ++;                  /*atm本季现金存款交易笔数*/
				sCard_reg.atm_ty_cr_cash_tx_cnt ++;                  /*atm本年现金存款交易笔数*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_cr_cash_tx_amt-sCard_reg.atm_max_cr_cash_tx_amt)>0)
				{
					vtcp_log("%s,%d 当日atm现金存款交易金额大于约定最大的交易金额",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_cash_tx_amt,sCard_reg.atm_max_cr_cash_tx_amt);
					return 11;
				}
				/***del by hxc 091115 晋中商行卡取消单日存款笔数限制 begin**/
				/**
				if(sCard_reg.atm_td_cr_cash_tx_cnt>sCard_reg.atm_max_cr_cash_tx_cnt)
				{
					vtcp_log("%s,%d 当日atm现金存款交易笔数大于约定最大的交易笔数",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_cash_tx_cnt,sCard_reg.atm_max_cr_cash_tx_cnt);
					return 12;
				}**/
				/***del by hxc 091115 晋中商行卡取消单日存款笔数限制 end**/
			}
		}
		else if(iDscpt==CARD_TRADE_TRAN)
		{/* ATM转帐交易 */
			if(iPselflag==1)
			{
				if(add_ind=='0')
				{
					sCard_reg.wh_atm_td_dr_tran_tx_amt += txamt;  /*当日外行atm转帐取款交易金额*/
					sCard_reg.wh_atm_tm_dr_tran_tx_amt += txamt;  /*当月外行atm转帐取款交易金额*/
					sCard_reg.wh_atm_ts_dr_tran_tx_amt += txamt;  /*当季外行atm转帐取款交易金额*/
					sCard_reg.wh_atm_ty_dr_tran_tx_amt += txamt;  /*当年外行atm转帐取款交易金额*/
					sCard_reg.wh_atm_td_dr_tran_tx_cnt ++;                  /*当日外行atm转帐取款交易笔数*/
					sCard_reg.wh_atm_tm_dr_tran_tx_cnt ++;                  /*当月外行atm转帐取款交易笔数*/
					sCard_reg.wh_atm_ts_dr_tran_tx_cnt ++;                  /*当季外行atm转帐取款交易笔数*/
					sCard_reg.wh_atm_ty_dr_tran_tx_cnt ++;                  /*当年外行atm转帐取款交易笔数*/
				}
				else if(add_ind=='1')
				{
					sCard_reg.wh_atm_td_cr_tran_tx_amt += txamt;  /*当日外行atm转帐存款交易金额*/
					sCard_reg.wh_atm_tm_cr_tran_tx_amt += txamt;  /*当月外行atm转帐存款交易金额*/
					sCard_reg.wh_atm_ts_cr_tran_tx_amt += txamt;  /*当季外行atm转帐存款交易金额*/
					sCard_reg.wh_atm_ty_cr_tran_tx_amt += txamt;  /*当年外行atm转帐存款交易金额*/
					sCard_reg.wh_atm_td_cr_tran_tx_cnt ++;                  /*当日外行atm转帐存款交易笔数*/
					sCard_reg.wh_atm_tm_cr_tran_tx_cnt ++;                  /*当月外行atm转帐存款交易笔数*/
					sCard_reg.wh_atm_ts_cr_tran_tx_cnt ++;                  /*当季外行atm转帐存款交易笔数*/
					sCard_reg.wh_atm_ty_cr_tran_tx_cnt ++;                  /*当年外行atm转帐存款交易笔数*/
				}
			}
			if(add_ind=='0')
			{
				sCard_reg.atm_td_dr_tran_tx_amt += txamt;  /*atm本日转帐交易金额*/
				sCard_reg.atm_tm_dr_tran_tx_amt += txamt;  /*atm本月转帐交易金额*/
				sCard_reg.atm_ts_dr_tran_tx_amt += txamt;  /*atm本季转帐交易金额*/
				sCard_reg.atm_ty_dr_tran_tx_amt += txamt;  /*atm本年转帐交易金额*/
				sCard_reg.atm_td_dr_tran_tx_cnt ++;     /*atm本日转帐交易笔数*/
				sCard_reg.atm_tm_dr_tran_tx_cnt ++;   /*atm本月转帐交易笔数*/
				sCard_reg.atm_ts_dr_tran_tx_cnt ++;   /*atm本季转帐交易笔数*/
				sCard_reg.atm_ty_dr_tran_tx_cnt ++;   /*atm本年转帐交易笔数*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_dr_tran_tx_amt-sCard_reg.atm_max_dr_tran_tx_amt)>0)
				{
					vtcp_log("%s,%d 当日atm转帐取款交易金额大于约定最大的交易金额",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_tran_tx_amt,sCard_reg.atm_max_dr_tran_tx_amt);
					return 1;
				}
				/***del by hxc 091115 晋中商行卡取消单日取款笔数限制 begin**/
				/**
				if(sCard_reg.atm_td_dr_tran_tx_cnt>sCard_reg.atm_max_dr_tran_tx_cnt)
				{
					vtcp_log("%s,%d 当日atm转帐取款交易笔数大于约定最大的交易笔数",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_tran_tx_cnt,sCard_reg.atm_max_dr_tran_tx_cnt);
					return 2;
				}**/
				/***del by hxc 091115 晋中商行卡取消单日取款笔数限制 end**/
			}
			else if(add_ind=='1')
			{
				sCard_reg.atm_td_cr_tran_tx_amt += txamt;  /*atm本日转帐交易金额*/
				sCard_reg.atm_tm_cr_tran_tx_amt += txamt;  /*atm本月转帐交易金额*/
				sCard_reg.atm_ts_cr_tran_tx_amt += txamt;  /*atm本季转帐交易金额*/
				sCard_reg.atm_ty_cr_tran_tx_amt += txamt;  /*atm本年转帐交易金额*/
				sCard_reg.atm_td_cr_tran_tx_cnt ++;     /*atm本日转帐交易笔数*/
				sCard_reg.atm_tm_cr_tran_tx_cnt ++;   /*atm本月转帐交易笔数*/
				sCard_reg.atm_ts_cr_tran_tx_cnt ++;   /*atm本季转帐交易笔数*/
				sCard_reg.atm_ty_cr_tran_tx_cnt ++;   /*atm本年转帐交易笔数*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_cr_tran_tx_amt-sCard_reg.atm_max_cr_tran_tx_amt)>0)
				{
					vtcp_log("%s,%d 当日atm转帐存款交易金额大于约定最大的交易金额",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_tran_tx_amt,sCard_reg.atm_max_cr_tran_tx_amt);
					return 11;
				}
				/***del by hxc 091115 晋中商行卡取消单日存款笔数限制 begin**/
				/**if(sCard_reg.atm_td_cr_tran_tx_cnt>sCard_reg.atm_max_cr_tran_tx_cnt)
				{
					vtcp_log("%s,%d 当日atm转帐存款交易笔数大于约定最大的交易笔数",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_tran_tx_cnt,sCard_reg.atm_max_cr_tran_tx_cnt);
					return 12;
				}**/
				/***del by hxc 091115 晋中商行卡取消单日存款笔数限制 end**/
			}
		}
		else
		{/* 查询交易 */
			if(iPselflag==1)
			{
				sCard_reg.wh_atm_td_qy_cnt ++; /*当日外行atm查询笔数*/
				sCard_reg.wh_atm_tm_qy_cnt ++; /*当月外行atm查询笔数*/
				sCard_reg.wh_atm_ts_qy_cnt ++; /*当季外行atm查询笔数*/
				sCard_reg.wh_atm_ty_qy_cnt ++; /*当年外行atm查询笔数*/
			}
			sCard_reg.atm_td_qy_cnt ++; /*atm本日查询笔数*/
			sCard_reg.atm_tm_qy_cnt ++; /*atm本月查询笔数*/
			sCard_reg.atm_ts_qy_cnt ++; /*atm本季查询笔数*/
			sCard_reg.atm_ty_qy_cnt ++; /*atm本年查询笔数*/
		}
	}
	else if(memcmp(cTradtype,"POS",3)==0)
	{
		if(iDscpt==CARD_TRADE_CASH || iDscpt==CARD_TRADE_TRAN)
		{
			if(iPselflag==1)
			{
				sCard_reg.wh_pos_td_tx_amt += txamt; /* 当日外行pos交易金额 **/
				sCard_reg.wh_pos_tm_tx_amt += txamt; /*当月外行pos交易金额*/
				sCard_reg.wh_pos_ts_tx_amt += txamt; /*当季外行pos交易金额*/
				sCard_reg.wh_pos_ty_tx_amt += txamt; /*当年外行pos交易金额*/
				sCard_reg.wh_pos_td_tx_cnt ++; /*当日外行pos交易笔数*/
				sCard_reg.wh_pos_tm_tx_cnt ++; /*当月外行pos交易笔数*/
				sCard_reg.wh_pos_ts_tx_cnt ++; /*当月外行pos交易笔数*/
				sCard_reg.wh_pos_ty_tx_cnt ++; /*当年外行pos交易笔数*/
			}
			sCard_reg.pos_td_tx_amt += txamt; /*pos本日交易金额 **/
			sCard_reg.pos_tm_tx_amt += txamt; /*pos本月交易金额 **/
			sCard_reg.pos_ts_tx_amt += txamt; /*pos本季交易金额 **/
			sCard_reg.pos_ty_tx_amt += txamt; /*pos本年交易金额 **/
			sCard_reg.pos_td_tx_cnt ++; /*pos本日交易笔数 **/
			sCard_reg.pos_tm_tx_cnt ++; /* pos本月交易笔数 **/
			sCard_reg.pos_ts_tx_cnt ++; /* pos本季交易笔数 **/
			sCard_reg.pos_ty_tx_cnt ++; /* pos本年交易笔数 **/
		}
		else
		{
			if(iPselflag==1)
			{
				sCard_reg.wh_pos_td_qy_cnt ++; /*当日外行pos查询笔数*/
				sCard_reg.wh_pos_tm_qy_cnt ++; /*当月外行pos查询笔数*/
				sCard_reg.wh_pos_ts_qy_cnt ++; /*当季外行pos查询笔数*/
				sCard_reg.wh_pos_ty_qy_cnt ++; /*当年外行pos查询笔数*/
			}
			sCard_reg.pos_td_qy_cnt ++; /*pos本日查询笔数 **/
			sCard_reg.pos_tm_qy_cnt ++; /* pos本月查询笔数 **/
			sCard_reg.pos_ts_qy_cnt ++; /* pos本季查询笔数 **/
			sCard_reg.pos_ty_qy_cnt ++; /* pos本年查询笔数 **/
		}
	}
	vtcp_log("%s %d NOTING here before replace l_tx_date,date=%d",__FILE__,__LINE__,sCard_reg.l_tx_date);
	sCard_reg.l_tx_date=g_pub_tx.tx_date;
	vtcp_log("%s %d NOTING here after replace l_tx_date,date=%d",__FILE__,__LINE__,sCard_reg.l_tx_date);
	sCard_reg.tdcnt++;
	/*** 检查当日交易是否查过限额 **/
	if(sCard_reg.wh_atm_tm_qy_cnt>sCard_reg.wh_atm_qy_free_cnt)
	{
		*atm_qy_free_flag=1;
	}
	else
	{
		*atm_qy_free_flag=0;
	}
	if(sCard_reg.wh_atm_tm_dr_cash_tx_cnt+sCard_reg.wh_atm_tm_dr_tran_tx_cnt>sCard_reg.wh_atm_tx_free_cnt)
	{
		*atm_tx_free_flag=1;
	}
	else
	{
		*atm_tx_free_flag=0;
	}
	if(sCard_reg.wh_pos_tm_qy_cnt>sCard_reg.wh_pos_qy_free_cnt)
	{
		*pos_qy_free_flag=1;
	}
	else
	{
		*pos_qy_free_flag=0;
	}
	if(sCard_reg.wh_pos_tm_tx_cnt>sCard_reg.wh_pos_tx_free_cnt)
	{
		*pos_tx_free_flag=1;
	}
	else
	{
		*pos_tx_free_flag=0;
	}
	g_reply_int=Card_reg_Upd_Upd(sCard_reg,g_pub_tx.reply);
	if(g_reply_int)
	{
		vtcp_log("%s,%d,find card_reg error ret=[%d]",__FILE__,__LINE__,g_reply_int);
		return g_reply_int;
	}
}

/********************
* 根据渠道传来的交易设备返回当前交易设备类型(pos,atm,trm)
* 跟据渠道传来的交易来源返回当前交易设备内外行标志(0-内,1-外)
* 根据渠道传来的交易方式 1--现金交易 2--转帐交易 3--查询交易
*******************/
int iGetTradeType(char * cPtradetype,int * iPselflag,int * iTradeflag)
{
	char cAgent[20];
	char cTxcode[5];
	char cTmpbuf[1024];
	char cId[3];
	int  iId=0;
	memset(cId, 0 , sizeof(cId));
	memset(cTmpbuf , 0 , sizeof(cTmpbuf));
	memset(cTxcode, 0 , sizeof(cTxcode));
	memset(cAgent, 0 , sizeof(cAgent));
	get_zd_data("1268",cAgent);

	if(memcmp(cAgent,SYS_CODE_SEND,strlen(cAgent))==0)
		*iPselflag=0;/* 本清算组织内交易 */
	else
		*iPselflag=1;/* 非本清算组织内交易,存在收取手续费的可能 */
	vtcp_log("%s,%d 代理行=[%s],太原=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	vtcp_log("%s,%d 代理行=[%s],太原=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	vtcp_log("%s,%d 代理行=[%s],太原=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	get_zd_data(DC_TX_CODE,cTxcode);
	/* F60.2.5：---> FD126M 交易渠道的详细定义 
		01	ATM-Automatic Teller Machine	自动柜员机
		02	CDM-Cash Deposit Machine	自动存款机
		03	POS-Point Of Sale	销售点终端
		06	Bank counter and terminal	银行柜台和终端
		11	Mobile POS	移动POS
	 */
	get_zd_data("126M",cTmpbuf);
	vtcp_log("%s,%d 126M --->tmpbuf[%s],tmpbuf+8===[%.2s]",__FILE__,__LINE__,cTmpbuf,cTmpbuf+8);
	if(memcmp(cTmpbuf+8,"01",2)==0 ||memcmp(cTmpbuf+8,"02",2)==0)
		memcpy(cPtradetype,"ATM",3);
	else if(memcmp(cTmpbuf+8,"03",2)==0 || memcmp(cTmpbuf+8,"11",2)==0)
		memcpy(cPtradetype,"POS",3);
	else if(memcmp(cTmpbuf+8,"06",2)==0)
		memcpy(cPtradetype,"TRM",3);
	else
		memcpy(cPtradetype,"UNK",3);
	/* F3：---> FD12613 交易处理码
		第1、2位  描述  含义
		
		00-19     Debits    借记   其中 01        Cash  现金
		20-29     Credits   贷记
		30-39	    Inquiryservices   查询服务
		40-49	    Transferservices  转账服务
		50-99     Reserved   保留
	 */
	memset(cTmpbuf, 0 , sizeof(cTmpbuf));
	get_zd_data("1261",cTmpbuf);
	vtcp_log("%s,%d 1261 --->tmpbuf===[%s]",__FILE__,__LINE__,cTmpbuf);
	memcpy(cId,cTmpbuf,2);
	iId=atoi(cId);
	vtcp_log("%s,%d CARD_TRADE_CASH=[%d]CARD_TRADE_TRAN=[%d]CARD_TRADE_QURY=[%d]",__FILE__,__LINE__,CARD_TRADE_CASH,CARD_TRADE_TRAN,CARD_TRADE_QURY);
	vtcp_log("%s,%d cId==[%s],iId=[%d]",__FILE__,__LINE__,cId,iId);
	if(iId==1)
		*iTradeflag=CARD_TRADE_CASH;
	else if(iId==0 || (iId>1 && iId<=29) || (iId>=40 && iId<=49))
		*iTradeflag=CARD_TRADE_TRAN;
	else if(iId >= 30 && iId <= 39)
		*iTradeflag=CARD_TRADE_QURY;
	vtcp_log("%s,%d 设备类型=[%s]行内外标志=[%d]交易方式=[%d]",__FILE__,__LINE__,cPtradetype,*iPselflag,*iTradeflag);
	return 0;
}
