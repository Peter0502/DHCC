/*************************************************
* 文 件 名:  spF040.c
* 功能描述： POS 预授权完成(处理卡帐户)(本行卡)
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
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_premise_c.h"
#include "card_reg_c.h"
#include "card.h"
/*add by lifei for nx 2009-8-1 9:13:29 */
#include "march_info_c.h"
#include "march_mst_c.h"

int spF040()
{
	int ret;
	char flag;
	long old_date;
	long old_trace_no;
	long premise_no;
	char ac_no[25], Opn_br_no[6];
	char tmpstr[51];
	char tmp_brf[21],dc_ind[2];
	char fee_ind[2];/*add by lifei for nx 2009-8-2 10:06:18 本行pos*/
	char msr_str[38];
	char old_crd_no[20];    /*补换卡时，平台传旧账户*/
  char crd_no[20];
	struct chnl_premise_c sChnl_premise;
	struct chnl_premise_c mChnl_premise;
	struct chnl_marchinfo_c chnl_marchinfo;
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
	memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
	memset(ac_no,0,sizeof(ac_no));
	memset(tmpstr,0,sizeof(tmpstr));
	memset(tmp_brf,0,sizeof(tmp_brf));
	memset(dc_ind,0,sizeof(dc_ind));
	memset(fee_ind,0,sizeof(fee_ind));/*add by lifei for nx 2009-8-2 10:06:18 本行pos*/
	memset(old_crd_no,0,sizeof(old_crd_no));
  memset(crd_no,0,sizeof(crd_no));

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
			goto ErrExit;
	}
	get_zd_data("0660",dc_ind);
	get_zd_data("0690",fee_ind);
	get_zd_data("0300",g_pub_tx.crd_no); /* 卡号 */
	strcpy(crd_no,g_pub_tx.crd_no);
	get_zd_double("0400",&g_pub_tx.tx_amt1);  /* 消费金额 */

	get_zd_data("0790",g_pub_tx.draw_pwd); /* 支取密码 */
	get_zd_data("0730",chnl_marchinfo.mrchno); /* 商户号 */

	get_zd_long("0520",&sChnl_premise.pt_trace_no); /* 原平台流水号 */
	get_zd_long("0440",&sChnl_premise.pt_date); /* 原平台流水号 */
	get_zd_long("0780",&old_trace_no); /* 原平台流水号 */
	get_zd_long("0450",&old_date); 
	get_zd_long("0870",&premise_no);       /* 预授权号 */
	get_zd_data("0310",ac_no);             /*待清算款项*/
	get_zd_data("0810",tmpstr);
	memcpy(g_pub_tx.ac_no1,ac_no,strlen(ac_no));  

	/* add by lifei  for nx 2009-8-1 9:53:12*/
	get_zd_long("0520",&pt_trace_no);
	get_zd_long("0440",&pt_date);
	get_zd_double("0410",&dFee_amt);
	get_zd_long("0450",&cz_date);
	get_zd_long("0780",&cz_pt_trace_no);
	/* end by lifei  for nx 2009-8-1 9:53:12*/   

	vtcp_log("原平台流水号==g_pub_tx.old_trace_no==[%ld]",g_pub_tx.old_trace_no);
	
	get_zd_data("0330",old_crd_no);
  pub_base_old_acno(old_crd_no);
  vtcp_log("%s%d,旧卡号为[%s]",__FILE__,__LINE__,old_crd_no);

	/* 检查卡的合法性 */
	if (pub_base_CheckCrdNo())
	{
		sprintf(acErrMsg,"检查卡的合法性错误!!");
		WRITEMSG
			goto ErrExit;
	}
	/* 检查卡的支取方式 *******TJB***************
	if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
	{
		sprintf(acErrMsg,"该卡的支取方式错误,必须是凭密码支取!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	***********宁夏不检查密码，有神码检查********/
	/**
	g_reply_int=pubf_card_chk_msr(3,g_pub_tx.crd_no);
	if(g_reply_int)
	{
	sprintf(acErrMsg,"读卡登记簿错误!!");
	WRITEMSG
	strcpy(g_pub_tx.reply, "CU14");
	goto ErrExit;
	}
	预授权完成不验证cvn*/
	/* 检查支取方式的合法性 
	ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
	g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	if (ret != 0)
	{
	sprintf(acErrMsg,"卡的支取密码错误!!!");
	WRITEMSG
	strcpy(g_pub_tx.reply,"CU55");
	goto ErrExit;
	}*/
	
	/*如果做过补换卡，应传旧卡号查询*/
  if(memcmp(crd_no,old_crd_no,19) && strlen(old_crd_no)!=0)
  {
  	strncpy(crd_no,old_crd_no,19);
  }
	/* 根据预授权号查询出预授权的交易日记 */
	ret = Chnl_premise_Sel(g_pub_tx.reply,&mChnl_premise,"ac_no='%s' and premise_no=%ld",crd_no,premise_no);
	if (ret)
	{
		vtcp_log("%s,%d,需要完成的于授权号不存在",__FILE__,__LINE__);	
		strcpy(g_pub_tx.reply,"CU25");
		goto ErrExit;
	}
	if (mChnl_premise.sts[0] != '0')
	{
		vtcp_log("%s,%d,授权号状态不正常",__FILE__,__LINE__);	
		strcpy(g_pub_tx.reply,"CU25");
		goto ErrExit;
	}
	/* 判断金额是否超出预授权金额 *	银联测试是超过15％以上不可以 modified by liuxuan*/
	if (pub_base_CompDblVal(mChnl_premise.premise_amt*1.15,g_pub_tx.tx_amt1) < 0)
	{
		sprintf(acErrMsg,"金额超出了预授权金额!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU61");
		goto ErrExit;
	}
	vtcp_log("%s,%d,完成金额[%lf],控制金额[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,mChnl_premise.premise_amt);
	memcpy(mChnl_premise.filler,tmpstr,30);

	ret = sql_execute("update chnl_premise set sts='2',finish_amt=%lf,filler='%s', pt_date=%ld, pt_trace_no=%ld, ac_no='%s'  where ac_no='%s' and premise_no=%ld ",g_pub_tx.tx_amt1,mChnl_premise.filler,sChnl_premise.pt_date,sChnl_premise.pt_trace_no, g_pub_tx.crd_no,crd_no,premise_no);
	if (ret)
	{
		sprintf(acErrMsg,"更新预授权记录错!!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU25");
		goto ErrExit;
	}
	vtcp_log("%s,%d,完成金额[%lf],控制金额[%lf]",__FILE__,__LINE__,g_pub_tx.tx_amt1,mChnl_premise.premise_amt);

	/* 取款 */
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
	set_zd_double("0420",g_pub_tx.tx_amt1);    
	g_pub_tx.tx_amt2=mChnl_premise.premise_amt; 
	strcpy(g_pub_tx.ct_ind,"2");
	g_pub_tx.ac_id = 0;
	if (dc_ind[0]=='1')
	{
		g_pub_tx.add_ind[0] = '0'; /* 减少控制金额 */
		set_zd_data("0930","1");/*借客户*/
	}
	else if (dc_ind[0] == '2')
	{
		g_pub_tx.add_ind[0] = '1'; /* 减少控制金额 */
		set_zd_data("0930","2");/*贷客户*/
	}
	g_pub_tx.svc_ind = 1007;   /* 预授权完成 */
	g_pub_tx.hst_ind[0] = '1'; /* 日间入明细 */
	memcpy(g_pub_tx.brf,tmpstr,20);
	if (pub_acct_trance())
	{
		sprintf(acErrMsg,"pos 预授权--错误!!!");
		WRITEMSG
			goto ErrExit;
	}
	ret = Ins_trade_self_func();
	if (ret)
	{
		sprintf(acErrMsg,"插入交易日志表出错!");
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
		iselflag = 1; /*现在不存在本行POS */
	vtcp_log("%s,%d,add_ind[%s]",__FILE__,__LINE__,g_pub_tx.add_ind);
	ret = pub_card_reg(g_pub_tx.tx_amt1,"POS",g_pub_tx.add_ind[0],&atm_qy_free_flag,&pos_qy_free_flag,&atm_tx_free_flag,&pos_tx_free_flag,iselflag,dscpt);
	if (ret)
	{
		sprintf(acErrMsg,"插入卡登记簿失败!");
		WRITEMSG
			strcpy(g_pub_tx.reply,"CU06");
		goto ErrExit;
	}
	if (fee_ind[0]=='7')/*add by lifei for nx 2009-8-2 11:30:09 本行pos*/
	{
		set_zd_data("1211",ac_no);     /* 记账分户 */
	}
	else
	{
		/*完成走帐借:客户,贷:贷清算,spF040,spA017*/
		if(strlen(ac_no)<13){/****20090811 修改 小于13为当做参数代码处理******/
			memset(tmpstr,0,sizeof(tmpstr));
			ret = pub_base_GetParm_Str(ac_no,1,tmpstr);
			if (ret)
			{
				vtcp_log("%s,%d,取待清算帐号出错!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU06");
				goto ErrExit;
			}
			memset(ac_no,0,sizeof(ac_no));
			memcpy(ac_no,tmpstr+5,7);
			memcpy(Opn_br_no,tmpstr,5); 
			set_zd_data("1211",ac_no);     /* 记账科目 */
		}else{
			memcpy(Opn_br_no,ac_no,5); 
			set_zd_data("1211",ac_no);     /* 记账科目 */
		}
	}
	set_zd_data("0770",Opn_br_no); /* 开户机构 */        
	set_zd_double("1218",g_pub_tx.tx_amt1);
	/**memcpy(tmp_brf,tmpstr,20);**/
	set_zd_data("121A",g_pub_tx.brf);
	set_zd_data("1215","2"); /*转帐*/
	set_zd_data("1214","01"); /*币种*/
OkExit:
		strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success!!");
	WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	if (strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
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
	char Node[61];
	get_zd_data("0270",Node);
	int ret = 0;
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	memset(Node,0,sizeof(Node));
	memcpy(chnl_self_trade.ttyn,Node,20);      /* 平台发起节点名称 */
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
	strcpy(chnl_self_trade.ac_no1,g_pub_tx.ac_no1);

	strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);

	strcpy(chnl_self_trade.opn_br_no,sMdm_ac_rel.opn_br_no);
	strncpy(chnl_self_trade.tel,g_pub_tx.tel,6);
	strncpy(chnl_self_trade.ttyp,"2",1);
	/**strcpy(chnl_self_trade.ttyn,g_pub_tx.tty); 终端号不对*/
	get_zd_long("0520", &chnl_self_trade.stra);	/*平台流水*/
	get_zd_long("0440", &chnl_self_trade.dz_date);  /*平台日期*/
	/**get_zd_long("0780", &chnl_self_trade.strb);  *原平台流水*/ 
	/* 本交易对帐中心流水号 */
	chnl_self_trade.tx_amt = g_pub_tx.tx_amt1;
	chnl_self_trade.sl_sxamt = 0.00;
	chnl_self_trade.fk_sxamt = 0.00;

	strncpy(chnl_self_trade.dz_flag,"0",1);
	strncpy(chnl_self_trade.ct_ind,g_pub_tx.ct_ind,1);
	memcpy(chnl_self_trade.brf_no,"PSWC",4);
	memcpy(chnl_self_trade.sctp,"PS",2);
	get_zd_data("0810", chnl_self_trade.brf_dscp);
	pub_base_strpack(chnl_self_trade.brf_dscp);
	chnl_self_trade.info_source[0] = '1';
	chnl_self_trade.add_ind[0]=g_pub_tx.add_ind[0];
	strncpy(chnl_self_trade.batch_flag,"0",1); /*费率标志0.代表POS*/
	strncpy(chnl_self_trade.stat,"0",1);
	get_zd_data("0240",chnl_self_trade.bbmk); /*业务类型*/
	/*Chnl_self_trade_Debug(&chnl_self_trade);*/
	/* 登记自助设备交易日记 */
	/*get_zd_long("1261",&chnl_self_trade.tt_no);add by cgx 20090818 nxhhyh */
	g_reply_int = Chnl_self_trade_Ins(chnl_self_trade,g_pub_tx.reply);
	if (g_reply_int != 0)
	{
		sprintf(acErrMsg,"登记自助设备交易日记错误!!");
		WRITEMSG
			strcpy(g_pub_tx.reply, "AT07");
		return(-1);
	}
	return(0);
}
