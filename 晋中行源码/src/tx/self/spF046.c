/*************************************************
* 文 件 名:  spF046.c
* 功能描述： POS 预授权完成撤销冲正
*
* 作    者:  rob
* 完成日期： 2003年2月22日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_premise_c.h"
#include "card.h"
/*add by lifei for nx 2009-8-1 9:13:29 */
#include "march_info_c.h"
#include "march_mst_c.h"

int spF046()
{
	int ret;
	char flag;
	long old_date;
	long old_trace_no,premise_no;
	char tmpstr[51],tmp_brf[21],ac_no[20];
	char dc_ind[2],fee_ind[2];/*add by lifei for nx 2009-8-2 17:06:15 本行pos*/
	struct chnl_marchinfo_c chnl_marchinfo;
	struct chnl_self_trade_c chnl_self_trade;
	struct chnl_premise_c  sChnl_premise;
	struct chnl_premise_c  mChnl_premise;
	/*add by lifei for nx 2009-8-1 9:13:29 */
	double dFee_amt=0.00;
	long pt_date=0;
	long pt_trace_no=0;
	long cz_date=0;
	long cz_pt_trace_no=0;
	char ls_ac_no[25];
	struct march_info_c sMarch_info;
	struct march_mst_c sMarch_mst;
	memset(&sMarch_mst,0x00,sizeof(sMarch_mst));
	memset(&sMarch_info,0x00,sizeof(sMarch_info));
	memset(ls_ac_no,0x00,sizeof(ls_ac_no));
	/*end by lifei for nx 2009-8-1 9:13:29 */    
	memset(dc_ind,0x00,sizeof(dc_ind));
	memset(fee_ind,0x00,sizeof(fee_ind));/*add by lifei for nx 2009-8-2 17:06:15 本行pos*/
	memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(ac_no,0,sizeof(ac_no));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(tmp_brf,0,sizeof(tmp_brf));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0660",dc_ind);
	get_zd_data("0690",fee_ind);/*add by lifei for nx 2009-8-2 17:06:15 本行pos*/
	get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
	get_zd_data("0310",ac_no);
	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
	get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */
	get_zd_data("0810",tmpstr); /* 摘要*/
	memcpy(tmp_brf,tmpstr,20); 
	get_zd_long("0780",&old_trace_no);  /*原终端流水号 */
	get_zd_long("0450",&old_date);  
	get_zd_long("0440",&sChnl_premise.pt_date);
	get_zd_long("0780",&sChnl_premise.pt_trace_no);
	get_zd_long("0870",&premise_no);

	get_zd_char("0670",&flag); /* 信息来源 */
	/* add by lifei  for nx 2009-8-1 9:53:12*/
	get_zd_long("0520",&pt_trace_no);
	get_zd_long("0440",&pt_date);
	get_zd_double("0410",&dFee_amt);
	get_zd_long("0450",&cz_date);
	get_zd_long("0780",&cz_pt_trace_no);
	/* end by lifei  for nx 2009-8-1 9:53:12*/       

	strcpy(chnl_self_trade.ac_no1,chnl_self_trade.ac_no);

	/* 检查卡的合法性 */
	if (pub_base_CheckCrdNo())
	{
		sprintf(acErrMsg,"检查卡的合法性错误!!");
		WRITEMSG
			goto ErrExit;
	}
	/* 查询预授权完成撤销的交易流水日记 */

	ret = Chnl_premise_Sel("g_pub_tx.reply",&mChnl_premise,"pt_date=%ld and pt_trace_no=%ld",old_date,old_trace_no);
	if (ret)
	{
		sprintf(acErrMsg,"要冲正的记录不存在!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	if (mChnl_premise.sts[0] != '0')
	{
		sprintf(acErrMsg,"记录状态不正常!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	if (memcmp(mChnl_premise.ac_no,g_pub_tx.crd_no,19)!=0)
	{
		sprintf(acErrMsg,"冲正卡号不一致!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	if (pub_base_CompDblVal(mChnl_premise.finish_amt, g_pub_tx.tx_amt1)!=0)
	{
		sprintf(acErrMsg,"冲正金额于完成金额不一直!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	memcpy(mChnl_premise.filler,tmpstr,30);
	ret = sql_execute("update chnl_premise set sts='2', pt_date=%ld, pt_trace_no=%ld, filler ='%s' where ac_no='%s' and pt_date=%ld and pt_trace_no=%ld",pt_date, pt_trace_no, mChnl_premise.filler,g_pub_tx.crd_no,old_date,old_trace_no);
	if (ret)
	{
		sprintf(acErrMsg,"冲正金额于完成金额不一直!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;

	}

	/* 冲正前的初始化 */
	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);

	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.tx_amt1 =0 - mChnl_premise.finish_amt;
	g_pub_tx.tx_amt2 =0 - mChnl_premise.premise_amt;
	g_pub_tx.ac_id = 0;
	g_pub_tx.add_ind[0] = '1'; /* 减少控制金额 */
	g_pub_tx.svc_ind = 1007;   /*  预授权完成  */
	g_pub_tx.hst_ind[0] = '1'; /* 入明细 */
	memcpy(g_pub_tx.brf,tmpstr,20);
	if (pub_acct_trance())
	{
		sprintf(acErrMsg,"pos 预授权--错误!!!");
		WRITEMSG
			goto ErrExit;
	}
	set_zd_double("0420",g_pub_tx.tx_amt1); /*交易金额*/
	set_zd_data("0930","2");
	if (fee_ind[0]=='7')/*add by lifei for nx 2009-8-2 11:30:09 本行pos*/
	{
		set_zd_data("1201",ac_no);     /* 记账分户 */
	}
	else
	{      
		if(strlen(ac_no)<13){/****20090811 修改 小于13为当做参数代码处理******/
			ret = pub_base_GetParm_Str(ac_no,1,ac_no); 
			if (ret)
			{
				sprintf(acErrMsg,"pos 取待清算行出错!!");
				WRITEMSG
					strcpy(g_pub_tx.reply,"CU06");
				goto ErrExit;
			}
			memset(tmpstr,0,sizeof(tmpstr));
			memcpy(tmpstr,ac_no,5);
			set_zd_data("0770",tmpstr);
			set_zd_data("1201",ac_no+5);
		}else{
			memset(tmpstr,0,sizeof(tmpstr));
			memcpy(tmpstr,ac_no,5);
			set_zd_data("0770",tmpstr);
            set_zd_data("1201",ac_no); /* 过渡科目 */
		}
	}
	set_zd_data("120A",tmp_brf);
	set_zd_data("1205","2");
	set_zd_data("1204","01");
	set_zd_double("1208",g_pub_tx.tx_amt1);
	/*借贷清算，贷：待客户,spF041,spA016*/

	/* 修改预授权完成自助设备交易日记*/
	/*ret = Ins_trade_self_func();*/
	ret = Upd_self_trade_func(old_date,old_trace_no);
	if (ret)
	{
		sprintf(acErrMsg,"pos 更新自助设备登记簿失败!");
		strcpy(g_pub_tx.reply,"CU06");
		WRITEMSG
			goto ErrExit;
	}
	int atm_qy_free_flag=0;/* atm查询免费标志 0免费 1收费 */
	int pos_qy_free_flag=0;/* pos 查询免费标志 0免费 1收费 */
	int pos_tx_free_flag=0;/* pos 交易免费标志 0免费 1收费 */
	int atm_tx_free_flag=0;/* atm 交易免费标志 0免费 1收费 */
	int dscpt = 0, iselflag=0;
	dscpt = CARD_TRADE_TRAN; 
	if (fee_ind[0]=='7')
		iselflag = 0;/*add by lifei for nx 2009-8-2 10:31:00 有本行pos*/
	else   
		iselflag = 1; 
	/*完成撤销冲正交易，借方记正数*/
	ret = pub_card_reg(mChnl_premise.finish_amt,"POS",'0',&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
	if (ret)
	{
		sprintf(acErrMsg,"插入卡登记簿失败!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

	ErrExit:
	if (memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

int Ins_trade_self_func()
{
	struct chnl_self_trade_c chnl_self_trade;
	struct mdm_ac_rel_c  sMdm_ac_rel;
	int ret = 0;
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	/* 给自助设备交易日记结构赋值 */
	if (memcmp(g_pub_tx.crd_no,CARDHEADCODE,6)==0 || memcmp(g_pub_tx.crd_no,ICCARDHEADCODE,6)==0)
	{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.crd_no);
		if (ret)
		{
			vtcp_log("%s,%d,__",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			return(-1);
		}
	}
	chnl_self_trade.tx_date = g_pub_tx.tx_date;
	chnl_self_trade.tx_time = g_pub_tx.tx_time;
	chnl_self_trade.trace_no = g_pub_tx.trace_no;
	strcpy(chnl_self_trade.cur_no,g_pub_tx.cur_no);
	strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);

	strcpy(chnl_self_trade.opn_br_no,sMdm_ac_rel.opn_br_no);
	strncpy(chnl_self_trade.tel,g_pub_tx.tel,6);
	strncpy(chnl_self_trade.ttyp,"1",1);
	strncpy(chnl_self_trade.ttyp,"2",1);
	strcpy(chnl_self_trade.ttyn,g_pub_tx.tty);
	get_zd_long("0520", &chnl_self_trade.stra);	/*平台流水*/
	get_zd_long("0440", &chnl_self_trade.dz_date);  /*平台日期*/
	get_zd_long("0780", &chnl_self_trade.strb);     /*原平台流水*/ 
	/* 本交易对帐中心流水号 */
	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	strncpy(chnl_self_trade.dz_flag,"0",1);
	strncpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind,1);
	memcpy(chnl_self_trade.brf_no,"PSWC",4);
	memcpy(chnl_self_trade.sctp,"PS",2);
	get_zd_data("0810", chnl_self_trade.brf_dscp);
	chnl_self_trade.info_source[0] = '1';
	chnl_self_trade.add_ind[0]=g_pub_tx.add_ind[0];
	strncpy(chnl_self_trade.batch_flag,"0",1);
	strncpy(chnl_self_trade.stat,"0",1);

	get_zd_data("0240",chnl_self_trade.bbmk); /*业务类型*/
	/**Chnl_self_trade_Debug(&chnl_self_trade);*/
	/* 登记自助设备交易日记 */
	/*g_reply_int = pub_base_Insertselftrade(chnl_self_trade);*/
	g_reply_int = pub_base_Insertselftrade(chnl_self_trade);
	if (g_reply_int != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "AT07");
		return(-1);
	}
	return(0);
}

int Upd_self_trade_func(int old_date,int old_trace_no)
{
	int ret=0;
	double tx_amt=0.0;
	struct chnl_self_trade_c sChnl_self_trade;
	memset(&sChnl_self_trade,0x00,sizeof(struct chnl_self_trade_c));
	get_zd_double("0400",&tx_amt);
	ret = Chnl_self_trade_Dec_Upd(g_pub_tx.reply,"stra=%ld and dz_date=%ld and tx_amt=%lf", old_trace_no,old_date,tx_amt);
	if (ret)
	{
		vtcp_log("%s,%d,查询原记录错误old_trace_no[%ld],old_date[%ld]",__FILE__,__LINE__,old_trace_no,old_date);
		strcpy(g_pub_tx.reply,"CU06");
		return(-1);
	}
	ret = Chnl_self_trade_Fet_Upd(&sChnl_self_trade,g_pub_tx.reply);
	if (ret)
	{
		vtcp_log("%s,%d,查询原纪录错误!old_trace_no[%ld],old_date[%ld]",__FILE__,__LINE__,old_trace_no,old_date);
		strcpy(g_pub_tx.reply,"CU06");
		Chnl_self_trade_Clo_Upd();
		return(-1);
	}
	sChnl_self_trade.stat[0]='1'; /*冲正交易更新原状态*/
	get_zd_data("0810",sChnl_self_trade.brf_dscp);	
	get_zd_long("0520",&sChnl_self_trade.strb);	
	ret = Chnl_self_trade_Upd_Upd(sChnl_self_trade,g_pub_tx.reply);
	if (ret)
	{
		vtcp_log("%s,%d,更新原纪录错误!old_trace_no[%ld],old_date[%ld]",__FILE__,__LINE__,old_trace_no,old_date);
		strcpy(g_pub_tx.reply,"CU06");
		Chnl_self_trade_Clo_Upd();
		return(-1);
	}
	Chnl_self_trade_Clo_Upd();
	return(0);
}
