/*************************************************
* 文 件 名:  spF072.c
* 功能描述： 更新记帐交易流水表
*
* 作	 者:  rob
* 完成日期： 2003年2月17日
*
* 修改记录： 
*	日	期:
*	修 改 人:
*	修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_tradeadd_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_tradelimit_c.h"
#include "card_reg_c.h"
#include "card_consum_lmt_c.h"
#include "card.h" /* add by LiuHuafeng 20070127 */
/*add by lifei for nx 2009-8-1 9:13:29 */
#include "march_info_c.h"
int spF072()
{
	int ret=0;
	char crd_no[25],crd2_no[25],cAc_no[25];
	char sAc_no[25];
	char brf10[21],tmp_brf[51];
	char dc_ind[2], ct_ind[2], cz_ind[2],fee_ind[2],ysq_ind[2];
	char nbank_flag[5];
	char s_ct_ind[2];
	double tx_amt=0.0;
	double tx_amt1=0.0;
	int iselflag=0,dscpt=0;
	long cx_ind;	
	char c_add_ind;
	char trade_type[4]; /*交易类型*/
	int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
	int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
	int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
	int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
	struct card_reg_c sCard_reg;
	struct chnl_self_trade_c sChnl_self_trade;
	struct card_consum_lmt_c  sCard_consum_lmt;
	struct mdm_ac_rel_c      cMdm_ac_rel;

	memset(&sCard_consum_lmt,0,sizeof(struct card_consum_lmt_c));
	memset(&cMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(s_ct_ind,0,sizeof(s_ct_ind));
	memset(&sCard_reg,0,sizeof(struct card_reg_c)); 
	memset(&sChnl_self_trade,0,sizeof(struct chnl_self_trade_c)); 
	memset(trade_type,0,sizeof(trade_type));
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(crd2_no, 0x0, sizeof(crd2_no));
	memset(cAc_no, 0x0, sizeof(cAc_no));
	memset(sAc_no, 0x0, sizeof(sAc_no));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(ysq_ind, 0x0, sizeof(ysq_ind));
	memset(tmp_brf, 0x0, sizeof(tmp_brf));
	memset(brf10, 0x0, sizeof(brf10));
	memset(nbank_flag, 0x0, sizeof(nbank_flag));
	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}
	get_zd_data("0660",dc_ind); /*借贷标志*/ 
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	get_zd_data("0680",ct_ind); /*现转标志*/ 
	get_zd_data("0690",fee_ind); /*收费模式*/
	get_zd_data("0700",ysq_ind); /*预授权标志 */
	get_zd_data("0720",s_ct_ind); /*提现标志*/

	get_zd_data("0300", crd_no); /*借贷标志相对方*/
	get_zd_data("0300", g_pub_tx.crd_no);
	get_zd_data("0270", nbank_flag);
	/* begin add by LiuHuafeng  2008-12-18 */
	pub_base_strpack(g_pub_tx.crd_no);
	if (strlen(g_pub_tx.crd_no) <=12)
	{
		ret=pub_base_GetParm_Str(g_pub_tx.crd_no,1,g_pub_tx.crd_no);
		if (ret)
		{
			vtcp_log("%s,%d,取记账科目错误!",__FILE__,__LINE__);
			if (strlen(g_pub_tx.reply)==0)
			{
				strcpy(g_pub_tx.reply,"O004");
			}
			goto ErrExit;
		}
		/** 20090405 开封 wyw **/
		if (strlen(g_pub_tx.crd_no)<13)
		{
			strcpy(g_pub_tx.crd_no,g_pub_tx.crd_no+5);
			vtcp_log("%s,%d,KAIFENG——CESHI，g_pub_tx.crd_no[%s]",__FILE__,__LINE__,g_pub_tx.crd_no);
		}
	}
	/* end by LiuHuafeng 2008-12-18 */

	get_zd_double("0400",&tx_amt);/*交易金额*/
	get_zd_data("0810",tmp_brf);  /*摘要*/
	get_zd_long("0500",&cx_ind);  /*撤销标志*/
	/*2010-2-26 zxp*/
	memcpy(brf10,tmp_brf,20);

	strcpy(g_pub_tx.cur_no, "01");
	vtcp_log("%s,%d,借贷[%s],现转[%s],收费模式[%s],卡号[%s],金额[%.2f]",__FILE__,__LINE__,dc_ind,ct_ind,fee_ind,crd_no,tx_amt);
	vtcp_log("%s,%d,卡号[%s],卡号2[%s]摘要[%s]",__FILE__,__LINE__,crd_no,crd2_no,tmp_brf);
	sprintf(acErrMsg,"交易类型= ysq_ind[%s] ,cz_ind[%s],撤销标志[%d]提现标志[%s]",ysq_ind,cz_ind,cx_ind,s_ct_ind);
	WRITEMSG
	if (cz_ind[0] == '1')
	{
		tx_amt = -1 * tx_amt;
	}
	g_pub_tx.tx_amt1= tx_amt;
	pub_base_old_acno(g_pub_tx.crd_no);
	/* begin add by LiuHuafeng 2008-12-18 */
	if (memcmp(g_pub_tx.reply,"0000",4))
	{
		memcpy(g_pub_tx.reply,"0000",4);
	}
	/* end add by LiuHuafeng 2008-12-18 */

	if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6) == 0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6) == 0) /*本行卡登记card_reg*/
	{
		if (dc_ind[0] == '1')
		{
			c_add_ind='0';  /*增减标志*/
		}
		else if(dc_ind[0]=='2')
		{
			c_add_ind='1';
		}
		if (fee_ind[0] =='1'||fee_ind[0]=='2')
		{
			iselflag=0; /*内外行标志*/	
			memcpy(trade_type,"ATM",3);
		}
		else if(fee_ind[0]=='3'||fee_ind[0]=='4')
		{
			memcpy(trade_type,"ATM",3);  /*交易类型标志*/
			iselflag=1;
		}
		else if(fee_ind[0]=='0')
		{
			memcpy(trade_type,"POS",3);
			dscpt=CARD_TRADE_TRAN; /*POS交易是转帐*/
			iselflag=1; /*现在还没有本行POS因此本外行POS置1*/
			/*POS交易撤销是反记得为统计POS消费金额,将金额置为负*/
			if ( cz_ind[0]=='0' && cx_ind==1)
			{
				tx_amt = tx_amt * -1;
			}
			else if(cz_ind[0] =='1' && cx_ind==1)
			{
				/*撤销冲正的时候记正数*/
				tx_amt = -1 *tx_amt;
			}
		}
		else if(fee_ind[0]=='5')
		{
			iselflag=0; /*内外行标志*/	
			memcpy(trade_type,"GMY",3);
		}
		else if(fee_ind[0]=='7')/* add by lifei for nx 2009-8-3 14:44:48 本行pos*/
		{
			memcpy(trade_type,"POS",3);
			dscpt=CARD_TRADE_TRAN; /*POS交易是转帐*/
			iselflag=0; /*有本行POS因此本外行POS置0*/
			/*POS交易撤销是反记得为统计POS消费金额,将金额置为负*/
			if ( cz_ind[0]=='0' && cx_ind==1)
			{
				tx_amt = tx_amt * -1;
			}
			else if(cz_ind[0] =='1' && cx_ind==1)
			{
				/*撤销冲正的时候记正数*/
				tx_amt = -1 *tx_amt;
			}
		}
		else if(fee_ind[0]=='a')/* add by hzh for 柜面通取款*/
		{
			memcpy(trade_type,"GMT",3);  /*交易类型标志*/
			iselflag=1;
		}
		else if(fee_ind[0]=='b')/* add by hzh for 跨行存款，包括IC，磁条卡的atm他代本以及柜面通的他代本存款*/
		{
			memcpy(trade_type,"KHC",3);  /*交易类型标志*/
			iselflag=1;
		}
		
		if (s_ct_ind[0]=='1')
		{
			dscpt=CARD_TRADE_CASH;  /*现转标志*/
		}
		else if(s_ct_ind[0]=='2')
		{
			dscpt=CARD_TRADE_TRAN;
		}
		vtcp_log("%s,%d,进入卡登记簿!tx_amt1[%lf],trade_type[%s],c_add_ind[%c],self[%d],dscpt[%d]",__FILE__,__LINE__,g_pub_tx.tx_amt1,trade_type,c_add_ind,iselflag,dscpt);
		ret = pub_card_reg(tx_amt,trade_type,c_add_ind,&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
		if (ret)
		{
			vtcp_log("%s,%d,__插入卡登记簿失败",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU27");
			goto ErrExit;
		}
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&cMdm_ac_rel,"ac_no='%s'",g_pub_tx.crd_no);	
		if (ret)
		{
			vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
		/** add by wyw 柜面返回账户账号 **/
		pub_base_acgetname_mst( cMdm_ac_rel.ac_id,1,g_pub_tx.name );
		set_zd_data("0250",g_pub_tx.name);
		/** add by wyw 20090517 */
		if (cMdm_ac_rel.main_ind[0]=='2')
		{
			vtcp_log("%s,%d,该卡为副卡,则更新卡限额登记簿!",__FILE__,__LINE__);
			ret = Card_consum_lmt_Dec_Upd(g_pub_tx.reply,"ac_no='%s'",g_pub_tx.crd_no);
			if (ret)
			{
				vtcp_log("%s,%d,副卡帐号没进行维护!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU62");
				goto ErrExit;
			}
			ret = Card_consum_lmt_Fet_Upd(&sCard_consum_lmt,g_pub_tx.reply);
			if (ret)
			{
				vtcp_log("%s,%d,副卡帐号没进行维护!",__FILE__,__LINE__);
				Card_consum_lmt_Clo_Upd();
				strcpy(g_pub_tx.reply,"CU62");
				goto ErrExit;
			}
			if (s_ct_ind[0]=='1')
			{
				sCard_consum_lmt.da_all_cash_lmt += g_pub_tx.tx_amt1; /*每日取现累计*/
				sCard_consum_lmt.mo_all_cash_lmt += g_pub_tx.tx_amt1; /*每月取现累计*/
			}
			else if(s_ct_ind[0]=='2' && !memcpy(trade_type,"ATM",3))
			{
				sCard_consum_lmt.da_all_amt_lmt +=  g_pub_tx.tx_amt1;/*每日转帐累计*/
				sCard_consum_lmt.mo_all_cash_lmt +=  g_pub_tx.tx_amt1;/*每月转帐累计*/
			}
			else if(!memcmp(trade_type,"POS",3))
			{
				sCard_consum_lmt.da_all_con_lmt +=  g_pub_tx.tx_amt1;/*每日消费累计*/
				sCard_consum_lmt.mo_all_con_lmt +=  g_pub_tx.tx_amt1;/*每月消费累计*/
			}
			ret = Card_consum_lmt_Upd_Upd(sCard_consum_lmt,g_pub_tx.reply);
			if (ret)
			{
				vtcp_log("%s,%d,更新卡限额登记簿!",__FILE__,__LINE__);
				Card_consum_lmt_Clo_Upd();
				strcpy(g_pub_tx.reply,"CU62");
				goto ErrExit;
			}
			Card_consum_lmt_Clo_Upd();
			vtcp_log("%s,%d,更新卡限额登记簿成功!",__FILE__,__LINE__);
		}
	}
	if (cz_ind[0]=='0')
	{
		vtcp_log("%s,%d,插入自助设备登记簿",__FILE__,__LINE__);
		ret = Ins_trade_self_func();
		if (ret)
		{
			vtcp_log("%s,%d,插入登记簿错误!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU06");
			goto ErrExit;
		}
		/*if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0)*/
		/**对于网银交易存折介质也要返回余额 20130619 hzh**/
		if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0 || memcmp(nbank_flag,"nbank",5)==0)
		{
			/*vtcp_log("%s,%d,若是本行卡则要返回账号余额!",__FILE__,__LINE__);*/
			vtcp_log("%s,%d,若是本行卡或者存折的网银交易则要返回账号余额!",__FILE__,__LINE__);
			ret = Return_bal(dc_ind,ct_ind);
			if (ret)
			{
				vtcp_log("%s,%d,返回帐户余额!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU06");
				goto ErrExit;
			}
		}
	}
	else if(cz_ind[0]=='1')
	{
		get_zd_long("0780",&sChnl_self_trade.stra);
		get_zd_long("0450",&sChnl_self_trade.dz_date);
		vtcp_log("%s,%d,平台流水[%ld],平台日期[%ld]",__FILE__,__LINE__,sChnl_self_trade.stra,sChnl_self_trade.dz_date);
		get_zd_double("0400",&sChnl_self_trade.tx_amt);
		ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"dz_date=%ld and stra=%ld and tx_amt=%lf and ac_no='%s'",sChnl_self_trade.dz_date,sChnl_self_trade.stra ,sChnl_self_trade.tx_amt,g_pub_tx.crd_no);
		/****Modified by SSH,20090405,记录不存在返25****/
		if (ret)
		{
			vtcp_log("%s,%d,要冲正的纪录不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU25");
			goto ErrExit;
		}
		ret = Chnl_self_trade_Fet_Upd(&sChnl_self_trade,g_pub_tx.reply);
		if (ret)
		{
			vtcp_log("%s,%d,要冲正的纪录不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU25");
			Chnl_self_trade_Clo_Upd();
			goto ErrExit;
		}
		/* begin add by LiuHuafeng 2008-9-30 21:38:57 判断如果已经冲正就不再继续处理 */
		if (sChnl_self_trade.stat[0]=='1')
		{
			vtcp_log("%s,%d,该笔交易已经冲正，原冲正的对账中心流水是%ld!",__FILE__,__LINE__,sChnl_self_trade.strb);
			strcpy(g_pub_tx.reply,"CU94");
			Chnl_self_trade_Clo_Upd();
			goto ErrExit;
		}
		/* end by LiuHuafeng 2008-9-30 21:42:08 */
		sChnl_self_trade.stat[0]='1'; /*冲正状态*/
		get_zd_long("0520",&sChnl_self_trade.strb);
		get_zd_data("0810",sChnl_self_trade.brf_dscp);
		/*  begin add by Wangyongwei 2008-12-18**/
		sChnl_self_trade.premise_date=g_pub_tx.trace_no;/* 借用授权日期放入冲正核心流水号 */
		/*  end add by Wangyongwei 2008-12-18**/
		ret = Chnl_self_trade_Upd_Upd(sChnl_self_trade,g_pub_tx.reply);
		if (ret)
		{
			vtcp_log("%s,%d,要冲正的纪录不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			Chnl_self_trade_Clo_Upd();
			goto ErrExit;
		}
		Chnl_self_trade_Clo_Upd();
	}
	else
	{
		vtcp_log("%s,%d,重正标志错!",__FILE__,__LINE__);
		goto ErrExit;
	}
OkExit:
	/*strcpy( g_pub_tx.reply, "0002" );*/
		sprintf(acErrMsg,"本行卡登记登记簿成功!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
		sprintf(acErrMsg,"登记卡登记簿失败! ");
	WRITEMSG
	if (strlen(g_pub_tx.reply)==0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
int Ins_trade_self_func()
{
	struct chnl_self_trade_c chnl_self_trade;
	struct mdm_ac_rel_c  sMdm_ac_rel;
	char sNode[61];
	int ret = 0;

	/*add by lifei 2009-8-1 16:56:35*/
	struct march_info_c sMarch_info;
	char cQs[BRNO_LEN+1];
	memset(cQs, 0 , sizeof(cQs));

	memset(&sMarch_info,0x00,sizeof(sMarch_info));

	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	char fee_ind[2],dc_ind[2],ct_ind[2],cz_ind[2],s_ct_ind[2];
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(s_ct_ind, 0x0, sizeof(s_ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(sNode,0,sizeof(sNode));
	get_zd_data("0660",dc_ind); /*借贷标志*/ 
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	get_zd_data("0680",ct_ind); /*现转标志*/ 
	get_zd_data("0690",fee_ind); /*收费模式*/
	get_zd_data("0720",s_ct_ind);/*实际取现标志*/
	get_zd_data("0270",sNode);
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);
	/* 给自助设备交易日记结构赋值 */
	if (strlen(g_pub_tx.crd_no)>12)
	{
		Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.crd_no);
		strcpy(chnl_self_trade.opn_br_no,sMdm_ac_rel.opn_br_no);
	}else
	{
		/* rem by LiuHuafeng 2008-12-18  上边已经找过了
		pub_base_GetParm_Str(g_pub_tx.crd_no,1,g_pub_tx.crd_no);	
		*****************************************/
		strncpy(chnl_self_trade.opn_br_no,cQs,BRNO_LEN);/*清算*/

	}
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strncpy(chnl_self_trade.cur_no,g_pub_tx.cur_no,2);
	strncpy(chnl_self_trade.ac_no,g_pub_tx.crd_no,strlen(g_pub_tx.crd_no));
	strncpy(chnl_self_trade.tx_code,g_pub_tx.tx_code,sizeof(g_pub_tx.tx_code)-1);
	strncpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1);
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);

	strncpy(chnl_self_trade.tel,g_pub_tx.tel,6);
	if (fee_ind[0]=='1'||fee_ind[0]=='2'||fee_ind[0]=='3'||fee_ind[0]=='4')
	{
		strncpy(chnl_self_trade.ttyp,"1",1);
		if (s_ct_ind[0]=='1')
		{
			if (cz_ind[0]=='0')
			{
				memcpy(chnl_self_trade.brf_no,"ATMQ",4);
			}else if(cz_ind[0]=='1')
			{
				memcpy(chnl_self_trade.brf_no,"ATMC",4);
			}
		}else if(s_ct_ind[0]=='2')
		{
			memcpy(chnl_self_trade.brf_no,"ATMZ",4);
		}
	}else if(fee_ind[0]=='0' || fee_ind[0]=='7')/*edit by lifei  2009-8-1 15:45:40 fee_ind[0]=='7' 本行pos*/
	{
		vtcp_log("%s %d 本行pos",__FILE__,__LINE__);
		strncpy(chnl_self_trade.ttyp,"2",1);
		memcpy(chnl_self_trade.brf_no,"PSXF",4);
	}else
	{
		strncpy(chnl_self_trade.ttyp,"5",1);
	}
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);
	get_zd_long("0520", &chnl_self_trade.stra);	/*平台流水*/
	get_zd_long("0440", &chnl_self_trade.dz_date); /*平台日期*/
	/* 本交易对帐中心流水号 */
	/*
	if(cz_ind[0]=='1')
	{
	get_zd_long("0780", &chnl_self_trade.strb);  
	get_zd_long("0450", &chnl_self_trade.dz_date);
	}
	*/
	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	strncpy(chnl_self_trade.dz_flag,"0",1);
	strncpy(chnl_self_trade.ct_ind,ct_ind,1);
	/****Add by SSH,20090405,增加记录第二帐号****/
	{
		char sTmp[128];
		memset(sTmp,'\0',sizeof(sTmp));
		get_zd_data("0310",sTmp); 
		strncpy(chnl_self_trade.ac_no1,sTmp,sizeof(chnl_self_trade.ac_no1)-1);
	}
	get_zd_data("0810", chnl_self_trade.brf_dscp);
	if (fee_ind[0]=='1'||fee_ind[0]=='2')
	{
		if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0)
		{
			chnl_self_trade.info_source[0] = '0';
		}else
		{
			chnl_self_trade.info_source[0] = '2';
		}
	}else
	{
		chnl_self_trade.info_source[0]= '1';
	}
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);
	if (dc_ind[0]=='1')
	{
		chnl_self_trade.add_ind[0]='0';
	}else
	{
		chnl_self_trade.add_ind[0]='1';
	}
	strncpy(chnl_self_trade.batch_flag,fee_ind,1);/* 用来存放费率编码,统计手续费时用*/
	strncpy(chnl_self_trade.stat,"0",1);
	get_zd_data("0240",chnl_self_trade.bbmk); /*用来存放业务类型*/
	memcpy(chnl_self_trade.ttyn,sNode,20);    /* 用来存放平台发起节点 */
	vtcp_log("%s,%d,开始插入自助设备登记簿!",__FILE__,__LINE__);
	/*Chnl_self_trade_Debug(&chnl_self_trade);*/
	/* 登记自助设备交易日记 */
	/*g_reply_int = pub_base_Insertselftrade(chnl_self_trade);*/
	Chnl_self_trade_Debug(&chnl_self_trade);
	get_zd_long("1261",&chnl_self_trade.tt_no);/*add by cgx 20090818 nxhhyh */
	g_reply_int = Chnl_self_trade_Ins(chnl_self_trade,g_pub_tx.reply);
	if (g_reply_int != 0)
	{
		vtcp_log("%s,%d,插入自助设备登记簿[%s]![%d]",__FILE__,__LINE__,g_pub_tx.reply,g_reply_int);
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "AT07");
		return(-1);
	}
	return(0);
}
int Return_bal(char *dc_ind,char *ct_ind)
{
	int ret=0;
	double today_amt=0.0;
	double account_amt=0.0,limit_amt=0.0;
	double ky_bal=0.0;
	char tmpstr[41];
	char sFee_ind[2];
	memset(tmpstr,0,sizeof(tmpstr));
	memset(sFee_ind,0,sizeof(sFee_ind));
	strcpy(g_pub_tx.cur_no,"01");
	struct dd_mst_c sDd_mst;
	struct card_reg_c sCard_reg;
	memset(&sDd_mst,0,sizeof(sDd_mst));
	memset(&sCard_reg,0,sizeof(sCard_reg));
	ret = pub_base_CurGetddFile(g_pub_tx.crd_no,g_pub_tx.cur_no,&sDd_mst);
	account_amt = sDd_mst.bal; /* 账面余额*/
	vtcp_log("%s,%d,sDd_mst_bal[%f]",__FILE__,__LINE__,sDd_mst.bal);
	get_zd_data("0690",sFee_ind);	
	/**if((memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0) && dc_ind[0]=='1') **/
	if (sFee_ind[0]>='1' && sFee_ind[0]<='5')
	{
		vtcp_log("%s,%d,若是本行卡走本分支!",__FILE__,__LINE__);
		ret = Card_reg_Sel(g_pub_tx.reply,&sCard_reg,"ac_no='%s' ",g_pub_tx.crd_no);
		if (ret)
		{
			vtcp_log("%s,%d,卡号不存在!",__FILE__,__LINE__);	
			strcpy(g_pub_tx.reply,"CU14");
			return(-1);
		}
		ky_bal = sCard_reg.atm_max_dr_cash_tx_amt - sCard_reg.atm_td_dr_cash_tx_amt;
		limit_amt = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
		if (pub_base_CompDblVal(ky_bal,limit_amt)>0)
		{
			ky_bal = limit_amt;
			vtcp_log("%s,%d,可用金额[%lf]",__FILE__,__LINE__,ky_bal);
		}
		vtcp_log("%s,%d,可用金额[%lf]",__FILE__,__LINE__,ky_bal);
	}else
	{
		ky_bal = sDd_mst.bal - sDd_mst.ctl_amt - sDd_mst.hold_amt;
	}
	if (pub_base_CompDblVal(sDd_mst.bal,ky_bal)<0)
	{
		ky_bal = sDd_mst.bal;
	}
	vtcp_log("%s,%d,可用金额[%lf],余额[%lf]",__FILE__,__LINE__,ky_bal,account_amt);
	memcpy(tmpstr,"1001156C",8);
	sprintf(tmpstr+8,"%012.0f",account_amt*100);
	memcpy(tmpstr+20,"1001156C",8);
	sprintf(tmpstr+28,"%012.0f",ky_bal*100);
	vtcp_log("%s,%d,tmpstr[%s]",__FILE__,__LINE__,tmpstr);
	set_zd_data("0820",tmpstr);
	return(0);
}
