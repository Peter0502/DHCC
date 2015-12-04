#include "card.h"
#include <fcntl.h>
#include "card_reg_c.h"
#include "public.h"

int pub_card_reg(double txamt,char * cTradtype,char add_ind,int * atm_qy_free_flag,int * pos_qy_free_flag,int *pos_tx_free_flag,int * atm_tx_free_flag,int iPselflag,int iDscpt)
{
	struct card_reg_c sCard_reg;
	char cTxday[9];
	vtcp_log("%s,%d ����Ĳ�������",__FILE__,__LINE__);
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
	/*  �Լ��ȴ��� */
	if(sCard_reg.l_tx_date/100 != g_pub_tx.tx_date/100 && 
		( memcmp(cTxday+4,"01",2)==0 
		||memcmp(cTxday+4,"04",2)==0 
		||memcmp(cTxday+4,"07",2)==0
		||memcmp(cTxday+4,"10",2)==0)
		)
	{
		sCard_reg.atm_ts_dr_cash_tx_amt   = 0;/*atm�����ֽ�ȡ��׽��*/
		sCard_reg.atm_ts_dr_cash_tx_cnt   = 0;/*atm�����ֽ�ȡ��ױ���*/
		sCard_reg.atm_ts_dr_tran_tx_amt   = 0;/*atm����ת��ȡ��׽��*/
		sCard_reg.atm_ts_dr_tran_tx_cnt   = 0;/*atm����ת��ȡ��ױ���*/

		sCard_reg.atm_ts_cr_cash_tx_amt   = 0;/*atm�����ֽ���׽��*/
		sCard_reg.atm_ts_cr_cash_tx_cnt   = 0;/*atm�����ֽ���ױ���*/
		sCard_reg.atm_ts_cr_tran_tx_amt   = 0;/*atm����ת�ʴ��׽��*/
		sCard_reg.atm_ts_cr_tran_tx_cnt   = 0;/*atm����ת�ʴ��ױ���*/

		sCard_reg.atm_ts_qy_cnt        = 0;/*atm������ѯ����*/
		sCard_reg.pos_ts_qy_cnt        = 0;/* pos������ѯ���� **/
		sCard_reg.pos_ts_tx_amt        = 0;/*pos�������׽�� **/
		sCard_reg.pos_ts_tx_cnt        = 0;/* pos�������ױ��� **/

		sCard_reg.wh_atm_ts_dr_cash_tx_amt= 0;/*��������atm�ֽ�ȡ��׽��*/
		sCard_reg.wh_atm_ts_dr_cash_tx_cnt= 0;/*��������atm�ֽ�ȡ��ױ���*/
		sCard_reg.wh_atm_ts_dr_tran_tx_amt= 0;/*��������atmת��ȡ��׽��*/
		sCard_reg.wh_atm_ts_dr_tran_tx_cnt= 0;/*��������atmת��ȡ��ױ���*/
                                                               
		sCard_reg.wh_atm_ts_cr_cash_tx_amt= 0;/*��������atm�ֽ���׽��*/
		sCard_reg.wh_atm_ts_cr_cash_tx_cnt= 0;/*��������atm�ֽ���ױ���*/
		sCard_reg.wh_atm_ts_cr_tran_tx_amt= 0;/*��������atmת�ʴ��׽��*/
		sCard_reg.wh_atm_ts_cr_tran_tx_cnt= 0;/*��������atmת�ʴ��ױ���*/

		sCard_reg.wh_atm_ts_qy_cnt     = 0;/*��������atm��ѯ����*/

		sCard_reg.wh_pos_ts_qy_cnt     = 0;/*��������pos��ѯ����*/
		sCard_reg.wh_pos_ts_tx_amt     = 0;/*��������pos���׽��*/ 
		sCard_reg.wh_pos_ts_tx_cnt     = 0;/*��������pos���ױ���*/ 
	}
	/* ����ļ�¼�Ĵ��� */
	if(sCard_reg.l_tx_date/10000 != g_pub_tx.tx_date/10000)
	{
		sCard_reg.atm_ty_dr_cash_tx_amt   = 0;/*atm�����ֽ�ȡ��׽��*/
		sCard_reg.atm_ty_dr_cash_tx_cnt   = 0;/*atm�����ֽ�ȡ��ױ���*/
		sCard_reg.atm_ty_dr_tran_tx_amt   = 0;/*atm����ת��ȡ��׽��*/
		sCard_reg.atm_ty_dr_tran_tx_cnt   = 0;/*atm����ת��ȡ��ױ���*/

		sCard_reg.atm_ty_cr_cash_tx_amt   = 0;/*atm�����ֽ���׽��*/
		sCard_reg.atm_ty_cr_cash_tx_cnt   = 0;/*atm�����ֽ���ױ���*/
		sCard_reg.atm_ty_cr_tran_tx_amt   = 0;/*atm����ת�ʴ��׽��*/
		sCard_reg.atm_ty_cr_tran_tx_cnt   = 0;/*atm����ת�ʴ��ױ���*/

		sCard_reg.atm_ty_qy_cnt        = 0;/*atm�����ѯ����*/
		sCard_reg.pos_ty_qy_cnt        = 0;/* pos�����ѯ���� **/
		sCard_reg.pos_ty_tx_amt        = 0;/*pos���꽻�׽�� **/
		sCard_reg.pos_ty_tx_cnt        = 0;/* pos���꽻�ױ��� **/

		sCard_reg.wh_atm_ty_dr_cash_tx_amt= 0;/*��������atm�ֽ�ȡ��׽��*/
		sCard_reg.wh_atm_ty_dr_cash_tx_cnt= 0;/*��������atm�ֽ�ȡ��ױ���*/
		sCard_reg.wh_atm_ty_dr_tran_tx_amt= 0;/*��������atmת��ȡ��׽��*/
		sCard_reg.wh_atm_ty_dr_tran_tx_cnt= 0;/*��������atmת��ȡ��ױ���*/

		sCard_reg.wh_atm_ty_cr_cash_tx_amt= 0;/*��������atm�ֽ���׽��*/
		sCard_reg.wh_atm_ty_cr_cash_tx_cnt= 0;/*��������atm�ֽ���ױ���*/
		sCard_reg.wh_atm_ty_cr_tran_tx_amt= 0;/*��������atmת�ʴ��׽��*/
		sCard_reg.wh_atm_ty_cr_tran_tx_cnt= 0;/*��������atmת�ʴ��ױ���*/

		sCard_reg.wh_atm_ty_qy_cnt     = 0;/*��������atm��ѯ����*/
		sCard_reg.wh_pos_ty_qy_cnt     = 0;/*��������pos��ѯ����*/
		sCard_reg.wh_pos_ty_tx_amt     = 0;/*��������pos���׽��*/
		sCard_reg.wh_pos_ty_tx_cnt     = 0;/*��������pos���ױ���*/
	}
	/* ���µļ�¼���� */
	if(sCard_reg.l_tx_date/100 != g_pub_tx.tx_date/100)
	{
		sCard_reg.atm_tm_dr_cash_tx_amt   = 0;/*atm�����ֽ�ȡ��׽��*/
		sCard_reg.atm_tm_dr_cash_tx_cnt   = 0;/*atm�����ֽ�ȡ��ױ���*/
		sCard_reg.atm_tm_dr_tran_tx_amt   = 0;/*atm����ת��ȡ��׽��*/
		sCard_reg.atm_tm_dr_tran_tx_cnt   = 0;/*atm����ת��ȡ��ױ���*/                                                         
		sCard_reg.atm_tm_cr_cash_tx_amt   = 0;/*atm�����ֽ���׽��*/
		sCard_reg.atm_tm_cr_cash_tx_cnt   = 0;/*atm�����ֽ���ױ���*/
		sCard_reg.atm_tm_cr_tran_tx_amt   = 0;/*atm����ת�ʴ��׽��*/
		sCard_reg.atm_tm_cr_tran_tx_cnt   = 0;/*atm����ת�ʴ��ױ���*/

		sCard_reg.atm_tm_qy_cnt        = 0;/*atm���²�ѯ����*/
		sCard_reg.pos_tm_qy_cnt        = 0;/* pos���²�ѯ���� **/
		sCard_reg.pos_tm_tx_amt        = 0;/*pos���½��׽�� **/
		sCard_reg.pos_tm_tx_cnt        = 0;/* pos���½��ױ��� **/

		sCard_reg.wh_atm_tm_dr_cash_tx_amt= 0;/*��������atm�ֽ�ȡ��׽��*/
		sCard_reg.wh_atm_tm_dr_cash_tx_cnt= 0;/*��������atm�ֽ�ȡ��ױ���*/
		sCard_reg.wh_atm_tm_dr_tran_tx_amt= 0;/*��������atmת��ȡ��׽��*/
		sCard_reg.wh_atm_tm_dr_tran_tx_cnt= 0;/*��������atmת��ȡ��ױ���*/
                                                               
		sCard_reg.wh_atm_tm_cr_cash_tx_amt= 0;/*��������atm�ֽ���׽��*/
		sCard_reg.wh_atm_tm_cr_cash_tx_cnt= 0;/*��������atm�ֽ���ױ���*/
		sCard_reg.wh_atm_tm_cr_tran_tx_amt= 0;/*��������atmת�ʴ��׽��*/
		sCard_reg.wh_atm_tm_cr_tran_tx_cnt= 0;/*��������atmת�ʴ��ױ���*/

		sCard_reg.wh_atm_tm_qy_cnt     = 0;/*��������atm��ѯ����*/
		sCard_reg.wh_pos_tm_qy_cnt     = 0;/*��������pos��ѯ����*/
		sCard_reg.wh_pos_tm_tx_amt     = 0;/*��������pos���׽��*/
		sCard_reg.wh_pos_tm_tx_cnt     = 0;/*��������pos���ױ���*/
	}
	/* ���յļ�¼�Ĵ��� */
	if(sCard_reg.l_tx_date!=g_pub_tx.tx_date)
	{
		sCard_reg.atm_td_dr_cash_tx_amt = 0; /*atm�����ֽ�ȡ��׽��*/
		sCard_reg.atm_td_dr_cash_tx_cnt = 0; /*atm�����ֽ�ȡ��ױ���*/
		sCard_reg.atm_td_dr_tran_tx_amt = 0; /*atm����ת��ȡ��׽��*/
		sCard_reg.atm_td_dr_tran_tx_cnt = 0; /*atm����ת��ȡ��ױ���*/
		sCard_reg.atm_td_cr_cash_tx_amt = 0; /*atm�����ֽ���׽��*/
		sCard_reg.atm_td_cr_cash_tx_cnt = 0; /*atm�����ֽ���ױ���*/
		sCard_reg.atm_td_cr_tran_tx_amt = 0; /*atm����ת�ʴ��׽��*/
		sCard_reg.atm_td_cr_tran_tx_cnt = 0; /*atm����ת�ʴ��ױ���*/

		sCard_reg.atm_td_qy_cnt = 0;   /*atm���ղ�ѯ����*/
		sCard_reg.pos_td_qy_cnt = 0;   /*pos���ղ�ѯ���� **/
		sCard_reg.pos_td_tx_amt = 0;   /*pos���ս��׽�� **/
		sCard_reg.pos_td_tx_cnt = 0;   /*pos���ս��ױ��� **/

		sCard_reg.wh_atm_td_dr_cash_tx_amt = 0;/*��������atm�ֽ�ȡ��׽��*/
		sCard_reg.wh_atm_td_dr_cash_tx_cnt = 0;/*��������atm�ֽ�ȡ��ױ���*/
		sCard_reg.wh_atm_td_dr_tran_tx_amt = 0;/*��������atmת��ȡ��׽��*/
		sCard_reg.wh_atm_td_dr_tran_tx_cnt = 0;/*��������atmת��ȡ��ױ���*/
                                                                
		sCard_reg.wh_atm_td_cr_cash_tx_amt = 0;/*��������atm�ֽ���׽��*/
		sCard_reg.wh_atm_td_cr_cash_tx_cnt = 0;/*��������atm�ֽ���ױ���*/
		sCard_reg.wh_atm_td_cr_tran_tx_amt = 0;/*��������atmת�ʴ��׽��*/
		sCard_reg.wh_atm_td_cr_tran_tx_cnt = 0;/*��������atmת�ʴ��ױ���*/

		sCard_reg.wh_atm_td_qy_cnt      = 0;/*��������atm��ѯ����*/
		sCard_reg.wh_pos_td_qy_cnt = 0;     /*��������pos��ѯ����*/
		sCard_reg.wh_pos_td_tx_amt = 0;     /* ��������pos���׽�� **/
		sCard_reg.wh_pos_td_tx_cnt = 0;     /*��������pos���ױ���*/
	}
	if(memcmp(cTradtype,"ATM",3)==0)
	{
		if(iDscpt==CARD_TRADE_CASH)
		{/* ATM�ֽ��� */
			if(iPselflag==1)
			{
				if(add_ind=='0')
				{
					sCard_reg.wh_atm_td_dr_cash_tx_amt += txamt;  /*��������atm�ֽ�ȡ��׽��*/
					sCard_reg.wh_atm_tm_dr_cash_tx_amt += txamt;  /*��������atm�ֽ�ȡ��׽��*/
					sCard_reg.wh_atm_ts_dr_cash_tx_amt += txamt;  /*��������atm�ֽ�ȡ��׽��*/
					sCard_reg.wh_atm_ty_dr_cash_tx_amt += txamt;  /*��������atm�ֽ�ȡ��׽��*/
					sCard_reg.wh_atm_td_dr_cash_tx_cnt ++;                  /*��������atm�ֽ�ȡ��ױ���*/
					sCard_reg.wh_atm_tm_dr_cash_tx_cnt ++;                  /*��������atm�ֽ�ȡ��ױ���*/
					sCard_reg.wh_atm_ts_dr_cash_tx_cnt ++;                  /*��������atm�ֽ�ȡ��ױ���*/
					sCard_reg.wh_atm_ty_dr_cash_tx_cnt ++;                  /*��������atm�ֽ�ȡ��ױ���*/	
				}
				else if(add_ind=='1')
				{
					sCard_reg.wh_atm_td_cr_cash_tx_amt += txamt;  /*��������atm�ֽ���׽��*/
					sCard_reg.wh_atm_tm_cr_cash_tx_amt += txamt;  /*��������atm�ֽ���׽��*/
					sCard_reg.wh_atm_ts_cr_cash_tx_amt += txamt;  /*��������atm�ֽ���׽��*/
					sCard_reg.wh_atm_ty_cr_cash_tx_amt += txamt;  /*��������atm�ֽ���׽��*/
					sCard_reg.wh_atm_td_cr_cash_tx_cnt ++;                  /*��������atm�ֽ���ױ���*/
					sCard_reg.wh_atm_tm_cr_cash_tx_cnt ++;                  /*��������atm�ֽ���ױ���*/
					sCard_reg.wh_atm_ts_cr_cash_tx_cnt ++;                  /*��������atm�ֽ���ױ���*/
					sCard_reg.wh_atm_ty_cr_cash_tx_cnt ++;                  /*��������atm�ֽ���ױ���*/	
				}
			}
			if(add_ind=='0')
			{
				sCard_reg.atm_td_dr_cash_tx_amt += txamt;  /*atm�����ֽ�ȡ��׽��*/
				sCard_reg.atm_tm_dr_cash_tx_amt += txamt;  /*atm�����ֽ�ȡ��׽��*/
				sCard_reg.atm_ts_dr_cash_tx_amt += txamt;  /*atm�����ֽ�ȡ��׽��*/
				sCard_reg.atm_ty_dr_cash_tx_amt += txamt;  /*atm�����ֽ�ȡ��׽��*/
				sCard_reg.atm_td_dr_cash_tx_cnt ++;                  /*atm�����ֽ�ȡ��ױ���*/
				sCard_reg.atm_tm_dr_cash_tx_cnt ++;                  /*atm�����ֽ�ȡ��ױ���*/
				sCard_reg.atm_ts_dr_cash_tx_cnt ++;                  /*atm�����ֽ�ȡ��ױ���*/
				sCard_reg.atm_ty_dr_cash_tx_cnt ++;                  /*atm�����ֽ�ȡ��ױ���*/
				vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
				if(pub_base_CompDblVal(sCard_reg.atm_td_dr_cash_tx_amt-sCard_reg.atm_max_dr_cash_tx_amt)>0)
				{
					vtcp_log("%s,%d ����atm�ֽ�ȡ��׽�����Լ�����Ľ��׽��",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_amt,sCard_reg.atm_max_dr_cash_tx_amt);
					return 1;
				}
				/***del by hxc 091115 �������п�ȡ������ȡ��������� begin**/
				/**
				if(sCard_reg.atm_td_dr_cash_tx_cnt>sCard_reg.atm_max_dr_cash_tx_cnt)
				{
					vtcp_log("%s,%d ����atm�ֽ�ȡ��ױ�������Լ�����Ľ��ױ���",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_cash_tx_cnt,sCard_reg.atm_max_dr_cash_tx_cnt);
					return 2;
				}**/
				/***del by hxc 091115 �������п�ȡ������ȡ��������� end**/
			}
			else if(add_ind=='1')
			{
				sCard_reg.atm_td_cr_cash_tx_amt += txamt;  /*atm�����ֽ���׽��*/
				sCard_reg.atm_tm_cr_cash_tx_amt += txamt;  /*atm�����ֽ���׽��*/
				sCard_reg.atm_ts_cr_cash_tx_amt += txamt;  /*atm�����ֽ���׽��*/
				sCard_reg.atm_ty_cr_cash_tx_amt += txamt;  /*atm�����ֽ���׽��*/
				sCard_reg.atm_td_cr_cash_tx_cnt ++;                  /*atm�����ֽ���ױ���*/
				sCard_reg.atm_tm_cr_cash_tx_cnt ++;                  /*atm�����ֽ���ױ���*/
				sCard_reg.atm_ts_cr_cash_tx_cnt ++;                  /*atm�����ֽ���ױ���*/
				sCard_reg.atm_ty_cr_cash_tx_cnt ++;                  /*atm�����ֽ���ױ���*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_cr_cash_tx_amt-sCard_reg.atm_max_cr_cash_tx_amt)>0)
				{
					vtcp_log("%s,%d ����atm�ֽ���׽�����Լ�����Ľ��׽��",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_cash_tx_amt,sCard_reg.atm_max_cr_cash_tx_amt);
					return 11;
				}
				/***del by hxc 091115 �������п�ȡ�����մ��������� begin**/
				/**
				if(sCard_reg.atm_td_cr_cash_tx_cnt>sCard_reg.atm_max_cr_cash_tx_cnt)
				{
					vtcp_log("%s,%d ����atm�ֽ���ױ�������Լ�����Ľ��ױ���",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_cash_tx_cnt,sCard_reg.atm_max_cr_cash_tx_cnt);
					return 12;
				}**/
				/***del by hxc 091115 �������п�ȡ�����մ��������� end**/
			}
		}
		else if(iDscpt==CARD_TRADE_TRAN)
		{/* ATMת�ʽ��� */
			if(iPselflag==1)
			{
				if(add_ind=='0')
				{
					sCard_reg.wh_atm_td_dr_tran_tx_amt += txamt;  /*��������atmת��ȡ��׽��*/
					sCard_reg.wh_atm_tm_dr_tran_tx_amt += txamt;  /*��������atmת��ȡ��׽��*/
					sCard_reg.wh_atm_ts_dr_tran_tx_amt += txamt;  /*��������atmת��ȡ��׽��*/
					sCard_reg.wh_atm_ty_dr_tran_tx_amt += txamt;  /*��������atmת��ȡ��׽��*/
					sCard_reg.wh_atm_td_dr_tran_tx_cnt ++;                  /*��������atmת��ȡ��ױ���*/
					sCard_reg.wh_atm_tm_dr_tran_tx_cnt ++;                  /*��������atmת��ȡ��ױ���*/
					sCard_reg.wh_atm_ts_dr_tran_tx_cnt ++;                  /*��������atmת��ȡ��ױ���*/
					sCard_reg.wh_atm_ty_dr_tran_tx_cnt ++;                  /*��������atmת��ȡ��ױ���*/
				}
				else if(add_ind=='1')
				{
					sCard_reg.wh_atm_td_cr_tran_tx_amt += txamt;  /*��������atmת�ʴ��׽��*/
					sCard_reg.wh_atm_tm_cr_tran_tx_amt += txamt;  /*��������atmת�ʴ��׽��*/
					sCard_reg.wh_atm_ts_cr_tran_tx_amt += txamt;  /*��������atmת�ʴ��׽��*/
					sCard_reg.wh_atm_ty_cr_tran_tx_amt += txamt;  /*��������atmת�ʴ��׽��*/
					sCard_reg.wh_atm_td_cr_tran_tx_cnt ++;                  /*��������atmת�ʴ��ױ���*/
					sCard_reg.wh_atm_tm_cr_tran_tx_cnt ++;                  /*��������atmת�ʴ��ױ���*/
					sCard_reg.wh_atm_ts_cr_tran_tx_cnt ++;                  /*��������atmת�ʴ��ױ���*/
					sCard_reg.wh_atm_ty_cr_tran_tx_cnt ++;                  /*��������atmת�ʴ��ױ���*/
				}
			}
			if(add_ind=='0')
			{
				sCard_reg.atm_td_dr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_tm_dr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_ts_dr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_ty_dr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_td_dr_tran_tx_cnt ++;     /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_tm_dr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_ts_dr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_ty_dr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_dr_tran_tx_amt-sCard_reg.atm_max_dr_tran_tx_amt)>0)
				{
					vtcp_log("%s,%d ����atmת��ȡ��׽�����Լ�����Ľ��׽��",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_tran_tx_amt,sCard_reg.atm_max_dr_tran_tx_amt);
					return 1;
				}
				/***del by hxc 091115 �������п�ȡ������ȡ��������� begin**/
				/**
				if(sCard_reg.atm_td_dr_tran_tx_cnt>sCard_reg.atm_max_dr_tran_tx_cnt)
				{
					vtcp_log("%s,%d ����atmת��ȡ��ױ�������Լ�����Ľ��ױ���",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_dr_tran_tx_cnt,sCard_reg.atm_max_dr_tran_tx_cnt);
					return 2;
				}**/
				/***del by hxc 091115 �������п�ȡ������ȡ��������� end**/
			}
			else if(add_ind=='1')
			{
				sCard_reg.atm_td_cr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_tm_cr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_ts_cr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_ty_cr_tran_tx_amt += txamt;  /*atm����ת�ʽ��׽��*/
				sCard_reg.atm_td_cr_tran_tx_cnt ++;     /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_tm_cr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_ts_cr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				sCard_reg.atm_ty_cr_tran_tx_cnt ++;   /*atm����ת�ʽ��ױ���*/
				if(pub_base_CompDblVal(sCard_reg.atm_td_cr_tran_tx_amt-sCard_reg.atm_max_cr_tran_tx_amt)>0)
				{
					vtcp_log("%s,%d ����atmת�ʴ��׽�����Լ�����Ľ��׽��",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%.2f],maxDrCashamt=[%.2f]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_tran_tx_amt,sCard_reg.atm_max_cr_tran_tx_amt);
					return 11;
				}
				/***del by hxc 091115 �������п�ȡ�����մ��������� begin**/
				/**if(sCard_reg.atm_td_cr_tran_tx_cnt>sCard_reg.atm_max_cr_tran_tx_cnt)
				{
					vtcp_log("%s,%d ����atmת�ʴ��ױ�������Լ�����Ľ��ױ���",
						__FILE__,__LINE__);
					vtcp_log("%s,%d atmTdDrCash=[%ld],maxDrCashamt=[%ld]",
						__FILE__,__LINE__,sCard_reg.atm_td_cr_tran_tx_cnt,sCard_reg.atm_max_cr_tran_tx_cnt);
					return 12;
				}**/
				/***del by hxc 091115 �������п�ȡ�����մ��������� end**/
			}
		}
		else
		{/* ��ѯ���� */
			if(iPselflag==1)
			{
				sCard_reg.wh_atm_td_qy_cnt ++; /*��������atm��ѯ����*/
				sCard_reg.wh_atm_tm_qy_cnt ++; /*��������atm��ѯ����*/
				sCard_reg.wh_atm_ts_qy_cnt ++; /*��������atm��ѯ����*/
				sCard_reg.wh_atm_ty_qy_cnt ++; /*��������atm��ѯ����*/
			}
			sCard_reg.atm_td_qy_cnt ++; /*atm���ղ�ѯ����*/
			sCard_reg.atm_tm_qy_cnt ++; /*atm���²�ѯ����*/
			sCard_reg.atm_ts_qy_cnt ++; /*atm������ѯ����*/
			sCard_reg.atm_ty_qy_cnt ++; /*atm�����ѯ����*/
		}
	}
	else if(memcmp(cTradtype,"POS",3)==0)
	{
		if(iDscpt==CARD_TRADE_CASH || iDscpt==CARD_TRADE_TRAN)
		{
			if(iPselflag==1)
			{
				sCard_reg.wh_pos_td_tx_amt += txamt; /* ��������pos���׽�� **/
				sCard_reg.wh_pos_tm_tx_amt += txamt; /*��������pos���׽��*/
				sCard_reg.wh_pos_ts_tx_amt += txamt; /*��������pos���׽��*/
				sCard_reg.wh_pos_ty_tx_amt += txamt; /*��������pos���׽��*/
				sCard_reg.wh_pos_td_tx_cnt ++; /*��������pos���ױ���*/
				sCard_reg.wh_pos_tm_tx_cnt ++; /*��������pos���ױ���*/
				sCard_reg.wh_pos_ts_tx_cnt ++; /*��������pos���ױ���*/
				sCard_reg.wh_pos_ty_tx_cnt ++; /*��������pos���ױ���*/
			}
			sCard_reg.pos_td_tx_amt += txamt; /*pos���ս��׽�� **/
			sCard_reg.pos_tm_tx_amt += txamt; /*pos���½��׽�� **/
			sCard_reg.pos_ts_tx_amt += txamt; /*pos�������׽�� **/
			sCard_reg.pos_ty_tx_amt += txamt; /*pos���꽻�׽�� **/
			sCard_reg.pos_td_tx_cnt ++; /*pos���ս��ױ��� **/
			sCard_reg.pos_tm_tx_cnt ++; /* pos���½��ױ��� **/
			sCard_reg.pos_ts_tx_cnt ++; /* pos�������ױ��� **/
			sCard_reg.pos_ty_tx_cnt ++; /* pos���꽻�ױ��� **/
		}
		else
		{
			if(iPselflag==1)
			{
				sCard_reg.wh_pos_td_qy_cnt ++; /*��������pos��ѯ����*/
				sCard_reg.wh_pos_tm_qy_cnt ++; /*��������pos��ѯ����*/
				sCard_reg.wh_pos_ts_qy_cnt ++; /*��������pos��ѯ����*/
				sCard_reg.wh_pos_ty_qy_cnt ++; /*��������pos��ѯ����*/
			}
			sCard_reg.pos_td_qy_cnt ++; /*pos���ղ�ѯ���� **/
			sCard_reg.pos_tm_qy_cnt ++; /* pos���²�ѯ���� **/
			sCard_reg.pos_ts_qy_cnt ++; /* pos������ѯ���� **/
			sCard_reg.pos_ty_qy_cnt ++; /* pos�����ѯ���� **/
		}
	}
	vtcp_log("%s %d NOTING here before replace l_tx_date,date=%d",__FILE__,__LINE__,sCard_reg.l_tx_date);
	sCard_reg.l_tx_date=g_pub_tx.tx_date;
	vtcp_log("%s %d NOTING here after replace l_tx_date,date=%d",__FILE__,__LINE__,sCard_reg.l_tx_date);
	sCard_reg.tdcnt++;
	/*** ��鵱�ս����Ƿ����޶� **/
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
* �������������Ľ����豸���ص�ǰ�����豸����(pos,atm,trm)
* �������������Ľ�����Դ���ص�ǰ�����豸�����б�־(0-��,1-��)
* �������������Ľ��׷�ʽ 1--�ֽ��� 2--ת�ʽ��� 3--��ѯ����
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
		*iPselflag=0;/* ��������֯�ڽ��� */
	else
		*iPselflag=1;/* �Ǳ�������֯�ڽ���,������ȡ�����ѵĿ��� */
	vtcp_log("%s,%d ������=[%s],̫ԭ=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	vtcp_log("%s,%d ������=[%s],̫ԭ=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	vtcp_log("%s,%d ������=[%s],̫ԭ=[%s]",__FILE__,__LINE__,cAgent,SYS_CODE_AGENT);
	get_zd_data(DC_TX_CODE,cTxcode);
	/* F60.2.5��---> FD126M ������������ϸ���� 
		01	ATM-Automatic Teller Machine	�Զ���Ա��
		02	CDM-Cash Deposit Machine	�Զ�����
		03	POS-Point Of Sale	���۵��ն�
		06	Bank counter and terminal	���й�̨���ն�
		11	Mobile POS	�ƶ�POS
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
	/* F3��---> FD12613 ���״�����
		��1��2λ  ����  ����
		
		00-19     Debits    ���   ���� 01        Cash  �ֽ�
		20-29     Credits   ����
		30-39	    Inquiryservices   ��ѯ����
		40-49	    Transferservices  ת�˷���
		50-99     Reserved   ����
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
	vtcp_log("%s,%d �豸����=[%s]�������־=[%d]���׷�ʽ=[%d]",__FILE__,__LINE__,cPtradetype,*iPselflag,*iTradeflag);
	return 0;
}
