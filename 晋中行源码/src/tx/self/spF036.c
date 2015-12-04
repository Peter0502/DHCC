/*************************************************
* 文 件 名:  spF036.c
* 功能描述： POS 预授权
*
* 作    者:  rob
* 完成日期： 2003年2月21日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "card_reg_c.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_premise_c.h"
#include "card_reg_c.h"
#include "card.h"
int spF036()
{
	int ret;
	char flag;
	char tmpstr[51];
	long premise_no;
	long old_date;
	char dc_ind[2],ct_ind[2],fee_ind[2];
	struct chnl_marchinfo_c chnl_marchinfo;
	struct chnl_self_trade_c chnl_self_trade;
	struct chnl_premise_c    sChnl_premise;
	struct chnl_premise_c    mChnl_premise;
	memset(tmpstr,0,sizeof(tmpstr));
	memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(dc_ind,0,sizeof(dc_ind));	
	memset(ct_ind,0,sizeof(ct_ind));	
	memset(fee_ind,0,sizeof(fee_ind));	

	if (pub_base_sysinit())
	{
	    sprintf(acErrMsg,"初始化公用结构错误!!");
	    WRITEMSG
	    goto ErrExit;
	}
		
	get_zd_data("0660",dc_ind);
	get_zd_data("0680",ct_ind);
	get_zd_data("0690",fee_ind);
	get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */
	get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
	get_zd_data("0810",tmpstr); /*摘要*/
	memcpy(sChnl_premise.filler,tmpstr,30);
	/*
	get_zd_data("0730",chnl_marchinfo.mrchno);  *商户号 *
	*/
	get_zd_long("0520",&sChnl_premise.pt_trace_no);/* 平台流水号 */
	get_zd_long("0440",&sChnl_premise.pt_date);     /*平台日期*/
	get_zd_long("0860",&sChnl_premise.l_premise_no);/*上次预授权号*/
	get_zd_long("0870",&sChnl_premise.premise_no);  /*预授权号*/ 
	get_zd_long("0870",&premise_no);  /*预授权号*/ 
	vtcp_log("%s,%d,预授权号  premise_no====[%d],l_premise_no[%d]",__FILE__,__LINE__,sChnl_premise.premise_no,sChnl_premise.l_premise_no);
	vtcp_log("%s,%d,消费金额 g_pub_tx.tx_amt1====[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	vtcp_log("%s,%d,pt_trace_no[%d],pt_date[%d],crd_no[%s],tmpstr[%s]",__FILE__,__LINE__,sChnl_premise.pt_trace_no,sChnl_premise.pt_date,g_pub_tx.crd_no,tmpstr);

	/* 检查卡的合法性 */
	if (pub_base_CheckCrdNo())
	{
	    sprintf(acErrMsg,"检查卡的合法性错误!!");
	    WRITEMSG
	    goto ErrExit;
	}
	/* begin add by LiuHuafeng 20070201 */
	/* 开封2.3磁道信息都校验 */
	g_reply_int=pubf_card_chk_msr(3,g_pub_tx.crd_no);
	if(g_reply_int)
	{
	    	sprintf(acErrMsg,"检查卡的2磁道CVN信息错误!!");
	    	WRITEMSG
		if(memcmp(g_pub_tx.reply,"0000",4)==0)
		{
	    		strcpy(g_pub_tx.reply,"CU05");
		}
	    	goto ErrExit;
	}
	/* end by LiuHuafeng 20070201 */

	/* 检查卡的支取方式 ***TJB***
	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
	{
	    sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"CU06");
	    goto ErrExit;
	}
	****支取方式不检查****/

	 /* 检查支取方式的合法性 20090728 start  modify by chenggongxun 由卡系统校验密码* 
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
	                  g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	if (ret != 0)
	{
	    sprintf(acErrMsg,"卡的支取密码错误!!!");
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"CU55");
	    goto ErrExit;
	}
	20090728 end by chenggongxun *****/

	/* 修改帐户的控制金额并对帐户的检查 */
	strcpy(g_pub_tx.cur_no,"01");
	set_zd_data("0210",g_pub_tx.cur_no);
	ret = pub_base_CurToSeqn(g_pub_tx.cur_no,&g_pub_tx.ac_seqn);
	if ( ret != 0)
	{
	    sprintf(acErrMsg,"根据币种生成帐户序号错误!! cur_no=[%s]",
	            g_pub_tx.cur_no);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"CU06");
	    goto ErrExit;
	}

	g_pub_tx.ac_id = 0;
	if(dc_ind[0]=='1')
	{
		g_pub_tx.add_ind[0] = '0'; /* 增加控制金额 ,减客户*/
	}else{
		g_pub_tx.add_ind[0] = '1'; /* 减少控制金额 ,增客户*/
	}
	memcpy(g_pub_tx.brf,tmpstr,20);
	g_pub_tx.svc_ind = 1006;   /*  预授权  */
	g_pub_tx.hst_ind[0] = '0'; /* 不入明细 */
	if (pub_acct_trance())
	{
	    sprintf(acErrMsg,"pos 预授权--错误!!!");
	    WRITEMSG
	    goto ErrExit;
	}
	vtcp_log("l_premise_no[%d]====[%d]", sChnl_premise.premise_no);

	/*给预授权状态表赋值*/
	if(sChnl_premise.l_premise_no==0)
	{
		vtcp_log("yushouquan");
		memcpy(sChnl_premise.ac_no,g_pub_tx.crd_no,strlen(g_pub_tx.crd_no));
		sChnl_premise.sts[0]='0';                   /*正常状态*/ 
		sChnl_premise.premise_amt = g_pub_tx.tx_amt1;
		Chnl_premise_Debug(&sChnl_premise);
		ret = Chnl_premise_Ins(sChnl_premise,g_pub_tx.reply); 	
		if(ret)
		{
	   		sprintf(acErrMsg,"pos 插入登记簿错误!!");
	    		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
	    		goto ErrExit;
		}
	}else{
		vtcp_log("zhuijia");
		ret = Chnl_premise_Dec_Upd(g_pub_tx.reply,"ac_no='%s' and premise_no=%ld",g_pub_tx.crd_no,sChnl_premise.l_premise_no);
		if(ret)
		{
			vtcp_log("%s,%d,查不到 记录 ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU25");
			goto ErrExit;
		}
		ret = Chnl_premise_Fet_Upd(&mChnl_premise,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("%s,%d,查不到 记录 ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU25");
			Chnl_premise_Clo_Upd();
			goto ErrExit;
		}
		memcpy(mChnl_premise.filler,tmpstr,30);
		mChnl_premise.l_premise_no = mChnl_premise.premise_no;
		mChnl_premise.premise_no = premise_no; /*授权号*/
		mChnl_premise.premise_amt = mChnl_premise.premise_amt + g_pub_tx.tx_amt1;
		mChnl_premise.pt_date = sChnl_premise.pt_date;
		mChnl_premise.pt_trace_no = sChnl_premise.pt_trace_no;
		ret = Chnl_premise_Upd_Upd(mChnl_premise,g_pub_tx.reply);
		if(ret)
		{
			vtcp_log("%s,%d,查不到 记录 ",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU25");
			Chnl_premise_Clo_Upd();
			goto ErrExit;
		}
		ret = Chnl_premise_Clo_Upd();
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;


ErrExit:
	if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}	
	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
