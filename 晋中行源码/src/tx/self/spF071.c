/*************************************************
* 文 件 名:  spF071.c
* 功能描述： ATM和POS交易手续费处理
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
#include "chnl_tradelimit_c.h"
#include "card.h" /* add by LiuHuafeng 20070127 */
#include "com_parm_c.h"

#define YL_DQS_KM     "41404"     /*银联待清算科目*/
#define BH_DFFEE_KM   "52704"     /*本行垫付手续费科目*/
#define BH_SQATMFEE_KM   "5111401"   /*本行收取ATM手续费科目  Mod by hxc 101218 修改记账科目5111203->5111401*/
#define BH_SQPOSFEE_KM   "5111701"   /*本行收取POS手续费科目  Mod by hxc 101218 修改记账科目5111204->5111701*/
#define YL_FEE         3.60       /*银联手续费标准*/
#define TCQX_KH_FEE    2.00       /*同城他行ATM取现收取客户费用*/
#define TCZZ_BZ1       10000.00   /*同城转帐10000.00的限额*/
#define TCZZ_KH_FEE1   3.00       /*同城转帐10000.00的限额,收客户费用3.00*/
#define TCZZ_BZ2       50000.00   /*同城转帐10000.00--50000.00的限额*/
#define TCZZ_KH_FEE2   5.00       /*同城转帐10000.00--50000.00的限额,收客户费用5.00*/
#define TCZZ_BZ3       100000.00    /*同城转帐50000.00--100000.00的限额*/
#define TCZZ_KH_FEE3   8.00         /*同城转帐50000.00--100000.00的限额,收客户费用8.00*/
#define TCZZ_KH_FEE4   10.00        /*同城转帐大于100000.00的,收客户费用10.00*/
#define YDQX_KH_FEE    2.00         /*异地他行ATM取现,收取客户2元+交易金额的 
						(一定比率)YDQX_AMT_RATE*/ 
#define YDQX_AMT_RATE  0.01         /*异地他行ATM取现,收取交易金额的一定比率*/
#define YDQX_RATE_ZDFEE  1.00       /*异地他行ATM取现,按金额收取最低1.00元*/
#define YDQX_RATE_ZGFEE  50.00      /*异地他行ATM取现,按金额收取最高50.00元*/
#define YDZZ_ZDKH_FEE  5.00         /*异地他行ATM转帐,收取客户最低费用*/
#define YDZZ_ZGKH_FEE  50.00        /*异地他行ATM转帐,收取客户最高费用*/
#define YDZZ_AMT_RATE  0.01         /*异地他行ATM转帐,收取交易金额的比率*/
#define JWQX_KH_FEE    12.00        /*境外ATM取现,收取客户费用12.00+金额比率*/
#define JWQX_AMT_RATE  0.01         /*境外ATM取现，收取交易金额的比率*/ 
/** 开封现在只有两种收费模式,都是按笔收取 1.境内他行ATM(2.00)，2.境外他行ATM(15.00), 20090424 以上宏定义不在使用**/
/**Add by hxc 110513 农民工卡收费模式 按金额的0.8%收取,最低1元,最高20元**/
#define NMG_QX_RATE   0.005         /*农民工卡取现,收取交易金额的一定比率*/
#define NMG_QX_ZDFEE  1.00          /*农民工卡取现,按金额收取最低1.00元*/
#define NMG_QX_ZGFEE  20.00         /*农民工卡取现,按金额收取最高20.00元*/
#define NMG_FEE_RATE  0.70          /*农民工卡取现,手续费中的70%返给银联过渡科目,另外30%归行内收入*/
/**End by hxc 110513 农民工卡收费模式 按金额的0.8%收取,最低1元,最高20元**/
/**Add by hxc 121217 助农取款收费模式 收单方按金额的0.48%收取,银联按0.08%收取网络服务费，最低0.7元,最高3.5元**/
#define ZNQK_QX_SN_RATE   0.0040         /*助农取款取现,收单方收取交易金额的一定比率*/
#define ZNQK_QX_YL_RATE  0.0005        /*助农取款取现,银联网络服务费收取交易金额的一定比率*/
#define ZNQK_QX_ZDFEE  0.90          /*助农取款取现,按金额收取最低0.70元*/
#define ZNQK_QX_ZGFEE  9.00         /*助农取款取现,按金额收取最高3.50元*/
#define ZNQK_FEE_RATE  1          /*助农取款取现,手续费中全部返给银联过渡科目*/
/**End by hxc 121217 助农取款收费模式 按金额的0.48%收取,最低0.6元,最高3.5元**/
/**Add by hzh 130324 他行柜面取款收费模式 按金额的0.1%收取,最低3元,最高50元**/
#define GMTQK_QX_RATE   0.001         /*他行柜面取款取现,收取交易金额的一定比率*/
#define GMTQK_QX_ZDFEE  3.00          /*他行柜面取现,按金额收取最低3.00元*/
#define GMTQK_QX_ZGFEE  50.00         /*他行柜面取现,按金额收取最高50.00元*/
#define GMTQK_FEE_RATE  1          /*他行柜面取现,手续费中全部返给银联过渡科目*/
/**End by hzh 130324 他行柜面取款收费模式 按金额的0.1%收取,最低3元,最高50元**/
/**Add by hzh 130324 跨行存款收费模式 按金额的0.1%收取,最低3元,最高50元**/
#define KHCK_RATE   0.001         /*跨行存款,收取交易金额的一定比率*/
#define KHCK_ZDFEE  3.00          /*跨行存款,按金额收取最低3.00元*/
#define KHCK_ZGFEE  50.00         /*跨行存款,按金额收取最高50.00元*/
#define KHCK_FEE_RATE  1          /*跨行存款,手续费中全部返给银联过渡科目*/
/**End by hzh 130324 跨行存款收费模式 按金额的0.1%收取,最低3元,最高50元**/
int spF071()
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mdm_ac_rel_c v_mdm_ac_rel;
	struct com_parm_c   sCom_parm;
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&v_mdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	memset(&sCom_parm,0,sizeof(struct com_parm_c));
	int ret=0;
	char crd_no[25],crd_no2[25],cAc_no[25];
	char sAc_no[25],zAc_no[25];
	char sAc_Fee[25];
	char tmpstr[25];
	char dc_ind[2], ct_ind[2], cz_ind[2],fee_ind[2],ysq_ind[2];
	char s_ct_ind[2];
	char auth_no[8];
	char cAcc_no[8];
	char cAcno[25];
	double tx_amt=0.0,tx_amt1=0.0, tx_amt2=0.0,yl_fee=0.0,yl_jwfee=0.0,bank_dfee=0.0;
	double tckh_fee=0.0;
	double IC_YL_FEE=0.00;  /*IC卡电子现金银联手续费标准(他代本指定圈存收0.5元，其他都0.9元)*/ 
	
	/**Add by hxc 100125 **/
	double dFee_amt=0.00;
	long pt_date=0;
	long pt_trace_no=0;
	long cx_ind;	
	/**End by hxc 100125 **/
	
	memset(crd_no, 0x0, sizeof(crd_no));
	memset(crd_no2, 0x0, sizeof(crd_no2));
	memset(cAc_no, 0x0, sizeof(cAc_no));
	memset(sAc_no, 0x0, sizeof(sAc_no));
	memset(zAc_no, 0x0, sizeof(zAc_no));
	memset(sAc_Fee,0,sizeof(sAc_Fee));
	memset(tmpstr, 0x0, sizeof(tmpstr));
	memset(dc_ind, 0x0, sizeof(dc_ind));
	memset(ct_ind, 0x0, sizeof(ct_ind));
	memset(cz_ind, 0x0, sizeof(cz_ind));
	memset(fee_ind, 0x0, sizeof(fee_ind));
	memset(ysq_ind, 0x0, sizeof(ysq_ind));
	memset(auth_no, 0x0, sizeof(auth_no));
	memset(s_ct_ind,0,sizeof(s_ct_ind));
	memset(cAcc_no, 0x0, sizeof(cAcc_no));
	memset(cAcno, 0x0, sizeof(cAcno));
	pub_base_sysinit();
	
	/**Add by hxc 100125 **/
	get_zd_long("0520",&pt_trace_no);
	get_zd_long("0440",&pt_date);
	get_zd_double("0410",&dFee_amt);
	/**End by hxc 100125 **/

	get_zd_data("0660",dc_ind); /*借贷标志*/
	get_zd_data("0670",cz_ind); /*冲正标志*/ 
	get_zd_data("0680",ct_ind); /*帐务现转标志*/ 
	get_zd_data("0690",fee_ind); /*收费模式*/
	get_zd_data("0700",ysq_ind); /*预授权标志 */
	get_zd_data("0720",s_ct_ind); /*实际现转标志*/
	get_zd_data("0300", crd_no); /*主帐号*/
	get_zd_data("0300",g_pub_tx.crd_no); /*主帐号*/
	get_zd_data("0310", crd_no2); /*转入方*/
	get_zd_data("0790",g_pub_tx.draw_pwd);	/* 密码 */
	get_zd_double("0400",&tx_amt);/*交易金额*/
	get_zd_long("0500",&cx_ind);  /*撤销标志*/
	strcpy(g_pub_tx.cur_no, "01");
	vtcp_log("%s,%d,帐号为[%s]",__FILE__,__LINE__,crd_no);
	pub_base_old_acno(crd_no);
	pub_base_old_acno(g_pub_tx.crd_no);
	
	get_zd_double("0430",&IC_YL_FEE);
	vtcp_log("%s,%d,IC卡电子现金银联手续费为[%lf]",__FILE__,__LINE__,IC_YL_FEE);
	get_zd_data("0320",cAcno);/*若fee_ind为i,则32域需要传IC卡电子现金主账号*/
	vtcp_log("%s,%d,帐号为[%s]",__FILE__,__LINE__,cAcno);
	pub_base_old_acno(cAcno);
	if(strlen(g_pub_tx.crd_no)>12)
	{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",g_pub_tx.crd_no);
		if(ret)
		{
			vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"CU14");
			goto ErrExit;
		}
	}
	if(fee_ind[0]=='i')
	{
		if(memcmp(cAcno,ICCARDHEADCODE,6) == 0)
		{
			ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&v_mdm_ac_rel,"ac_no='%s'",cAcno);
			if(ret)
			{
				vtcp_log("%s,%d,帐号不存在!",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU14");
				goto ErrExit;
			}
			ret = Com_parm_Sel(g_pub_tx.reply,&sCom_parm,"parm_code='%s' and parm_seqn=11",v_mdm_ac_rel.opn_br_no);
			if(ret)
			{
				vtcp_log("%s,%d,内部帐号不存在!",__FILE__,__LINE__);
				goto ErrExit;
			}
		}
		ret=pub_base_GetParm_Str( "ICACC",1, cAcc_no );
		if(ret)
		{
			vtcp_log("%s,%d,科目号不存在!",__FILE__,__LINE__);
			goto ErrExit;
		}
		vtcp_log("%s,%d,电子现金科目号[%s]",__FILE__,__LINE__,cAcc_no);

		/*set_zd_data("1201",sCom_parm.val);*//*从IC卡电子账户中扣手续费*/
		/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh begin**/
		set_zd_data("1201",cAcno);
		/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh end**/
		set_zd_data("1205","2");
		set_zd_data("1204","01");
		vtcp_log("%s,%d,cAcc_no[%s]",__FILE__,__LINE__,cAcc_no);
		vtcp_log("%s,%d,sCom_parm.val[%s]",__FILE__,__LINE__,sCom_parm.val);
	}
	
	/**开封计算手续费金额 20090504 wyw
	ret = pub_base_GetParm_double("YLSS",1,&yl_fee);
	if(ret)
	{
		vtcp_log("%s,%d,取银联上报手续费!",__FILE__,__LINE__);
		yl_fee = YL_FEE; 
		vtcp_log("%s,%d,取银联上报手续费失败,取默认值[%lf]!",__FILE__,__LINE__,yl_fee);
	}
	********/
	
	sprintf(acErrMsg,"交易类型= ysq_ind[%s] ,cz_ind[%s]",ysq_ind,cz_ind);
	WRITEMSG
	vtcp_log("%s,%d,fee_ind[%s],ct_ind[%s],cz_ind[%s],s_ct_ind[%s],tx_amt[%.2f],dc_ind[%s],cx_ind[%d]",__FILE__,__LINE__,fee_ind,ct_ind,cz_ind,s_ct_ind,tx_amt,dc_ind,cx_ind);
	
	set_zd_data("0910",g_pub_tx.tx_br_no);	
	set_zd_data("1021",crd_no);
	/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh begin**/
	if(fee_ind[0]=='i')
	{
	set_zd_data("1021",cAcno);
	}
	/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh end**/
	vtcp_log("%s,%d,crd_no[%s]",__FILE__,__LINE__,crd_no);
	set_zd_data("1022","1");/*活期序号*/
	set_zd_data("1028",g_pub_tx.draw_pwd);
	set_zd_data("102K","2");/*现转标志*/
	set_zd_data("102J","01");
	/** 开封关于卡的公共参数都是刘伟峰定parm_code=KFCAR  **
	ret = pub_base_GetParm_Str("CUPS",1,cAc_no);
	if(ret)
	{
		memset(cAc_no, 0 , sizeof(cAc_no));
		strncpy(cAc_no,YL_DQS_KM,sizeof(cAc_no)-1);
		vtcp_log("%s,%d 没有找到待清算放在银联款项，所以启用默认帐户=%s",__FILE__,__LINE__,cAc_no);
		ret=0;
	}
	strcpy(cAc_no,cAc_no+5);
	********/
	/** ATM手续费收入账号 **/
	ret = pub_base_GetParm_Str("YLCSR",1,sAc_no);
	if(ret)
	{
		memset(sAc_no, 0 , sizeof(sAc_no));
		strncpy(sAc_no,BH_SQATMFEE_KM,sizeof(sAc_no)-1);
		vtcp_log("%s,%d 没有找到收费科目所以启用默认帐户=%s",__FILE__,__LINE__,sAc_no);
		goto ErrExit;
		ret=0;
	}
	/** 手续费垫付账号 **/
	ret = pub_base_GetParm_Str("YLCDF",1,zAc_no);
	if(ret)
	{
		memset(zAc_no, 0 , sizeof(zAc_no));
		strncpy(zAc_no,BH_DFFEE_KM,sizeof(zAc_no)-1);
		vtcp_log("%s,%d 没有找到收费科目所以启用默认帐户=%s",__FILE__,__LINE__,zAc_no);
		goto ErrExit;
		ret=0;
	}
	/** 开封手续费和清算金额是分开的，放到了262上面 20090422**/
	/** 开封手续费不上交到银联，直接受到其他应付款账户，com_code=KFCAR,parm_seqn=22 取出的账号 **/
	/**ret = pub_base_GetParm_Str("KFCAR",22,sAc_Fee);     //41404  hxc 
	if(ret)
	{
		vtcp_log("%s,%d 没有找到收费科目所以启用默认帐户=%s",__FILE__,__LINE__,zAc_no);
		goto ErrExit;
	}
	ret=Cal_Fee(fee_ind,&yl_fee,&bank_dfee);    //bank_dfee =3.6  hxc
	if(ret)
	{
		vtcp_log("%s,%d,计算手续费失败!",__FILE__,__LINE__);
		goto ErrExit;
	}**/
	/**Add by hxc 100125**/
	ret = pub_base_GetParm_Str("CUPS",1,sAc_Fee);
	if(ret)
	{
		memset(sAc_Fee, 0 , sizeof(sAc_Fee));
		strncpy(sAc_Fee,YL_DQS_KM,sizeof(sAc_Fee)-1);
		vtcp_log("%s,%d 没有找到收费科目所以启用默认帐户=%s",__FILE__,__LINE__,sAc_Fee);
		goto ErrExit;
	}
	bank_dfee=YL_FEE;
	yl_fee = YL_FEE;
	yl_jwfee = JWQX_KH_FEE;
	if(fee_ind[0] == '3' && s_ct_ind[0] =='2' && dc_ind[0]=='1')  /**晋中卡同城跨行转账手续费计算**/
	{
		yl_fee=0.00;
		if(pub_base_CompDblVal(tx_amt,TCZZ_BZ1)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ1)==0)
		{
			yl_fee = TCZZ_KH_FEE1;
		}
		else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ2)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ2)==0)
		{
			yl_fee = TCZZ_KH_FEE2;
		}
		else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ3)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ3)==0)
		{
			yl_fee = TCZZ_KH_FEE3;
		}
		else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ3)==1)
		{
			yl_fee = TCZZ_KH_FEE4;
		}
			vtcp_log("%s,%d,同城跨行转账银联手续费[%f]",__FILE__,__LINE__,yl_fee);
	}
	else if(fee_ind[0] == '4' && s_ct_ind[0] =='2' && dc_ind[0]=='1')  /**晋中卡异地跨行转账手续费计算**/
	{
		yl_fee=0.00;
		yl_fee = tx_amt * YDZZ_AMT_RATE; 
		vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,yl_fee);
		if(pub_base_CompDblVal(yl_fee,YDZZ_ZDKH_FEE) <0)
		{
			yl_fee = YDZZ_ZDKH_FEE;
		}
		else if(pub_base_CompDblVal(yl_fee,YDZZ_ZGKH_FEE)>0)
		{
			yl_fee = YDZZ_ZGKH_FEE;
		}
		else
		{
			yl_fee = tx_amt * YDZZ_AMT_RATE;
		}
		vtcp_log("%s,%d,异地跨行转账银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
	}
	else if(fee_ind[0]=='5')  /**境外收费标志，此时银联手续费为境外费用**/
	{
		yl_fee=0.00;
		yl_fee=yl_jwfee;
	}
	else if(fee_ind[0]=='i')
	{
		yl_fee=0.00;
		yl_fee=IC_YL_FEE;
	}
	else if(fee_ind[0] == '9' && s_ct_ind[0] =='1' && dc_ind[0]=='1')  /**Add by hxc 110513 晋中农民工卡取现手续费计算**/
	{
		yl_fee=0.00;
		yl_fee = tx_amt * NMG_QX_RATE; 
		vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,yl_fee);
		if(pub_base_CompDblVal(yl_fee,NMG_QX_ZDFEE) <0)
		{
			yl_fee = NMG_QX_ZDFEE;
		}
		else if(pub_base_CompDblVal(yl_fee,NMG_QX_ZGFEE)>0)
		{
			yl_fee = NMG_QX_ZGFEE;
		}
		else
		{
			yl_fee = tx_amt * NMG_QX_RATE;
		}
		vtcp_log("%s,%d,农民工卡取现原银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
		yl_fee = yl_fee * NMG_FEE_RATE;	/**Mod by hxc 110705 实际返给银联手续费应该是计算出的银联手续费的70%,另外30%直接计入行内收入科目**/
		vtcp_log("%s,%d,农民工卡取现实际返给银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
	}
	else if(fee_ind[0] == '8' && s_ct_ind[0] =='1' && dc_ind[0]=='1')  /**Add by hxc 121217 晋中助农取款取现手续费计算**/
	{
		yl_fee=pub_base_PubDround_1(tx_amt * ZNQK_QX_SN_RATE)+pub_base_PubDround_1(tx_amt * ZNQK_QX_YL_RATE);
		vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,yl_fee);
		if(pub_base_CompDblVal(yl_fee,ZNQK_QX_ZDFEE) <0)
		{
			yl_fee = ZNQK_QX_ZDFEE;
		}
		else if(pub_base_CompDblVal(yl_fee,ZNQK_QX_ZGFEE)>0)
		{
			yl_fee = ZNQK_QX_ZGFEE;
		}
		else
		{
		yl_fee=pub_base_PubDround_1(tx_amt * ZNQK_QX_SN_RATE)+pub_base_PubDround_1(tx_amt * ZNQK_QX_YL_RATE);
		}
		vtcp_log("%s,%d,助农取款取现原银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
		yl_fee = yl_fee * ZNQK_FEE_RATE;	/**Mod by hxc 110705 实际返给银联手续费应该是计算出的银联手续费的70%,另外30%直接计入行内收入科目**/
		vtcp_log("%s,%d,农民工卡取现实际返给银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
	}
	else if(fee_ind[0] == 'a' && s_ct_ind[0] =='1' && dc_ind[0]=='1')  /**Add by hzh 130324 晋中他行柜面取款取现手续费计算**/
	{
		yl_fee=0.00;
		yl_fee = tx_amt * GMTQK_QX_RATE; 
		vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,yl_fee);
		if(pub_base_CompDblVal(yl_fee,GMTQK_QX_ZDFEE) <0)
		{
			yl_fee = GMTQK_QX_ZDFEE;
		}
		else if(pub_base_CompDblVal(yl_fee,GMTQK_QX_ZGFEE)>0)
		{
			yl_fee = GMTQK_QX_ZGFEE;
		}
		else
		{
			yl_fee = tx_amt * GMTQK_QX_RATE;
		}
		vtcp_log("%s,%d,他行柜面取款取现原银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
		yl_fee = yl_fee * GMTQK_FEE_RATE;
		vtcp_log("%s,%d,他行柜面取现实际返给银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
	}
	else if(fee_ind[0] == 'b' && s_ct_ind[0] =='2')  /**Add by hzh 130324 晋中跨行存款手续费计算**/
	{
		yl_fee=0.00;
		yl_fee = tx_amt * KHCK_RATE; 
		vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,yl_fee);
		if(pub_base_CompDblVal(yl_fee,KHCK_ZDFEE) <0)
		{
			yl_fee = KHCK_ZDFEE;
		}
		else if(pub_base_CompDblVal(yl_fee,KHCK_ZGFEE)>0)
		{
			yl_fee = KHCK_ZGFEE;
		}
		else
		{
			yl_fee = tx_amt * KHCK_RATE;
		}
		vtcp_log("%s,%d,跨行存款原银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
		yl_fee = yl_fee * KHCK_FEE_RATE;
		vtcp_log("%s,%d,跨行存款实际返给银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
	}
	vtcp_log("%s,%d 交易模式fee_ind=[%s]，银联手续费yl_fee=[%.2f],yl_jwfee=[%.2f]",__FILE__,__LINE__,fee_ind,yl_fee,yl_jwfee);
	/**End by hxc 100125**/
	
	vtcp_log("%s,%d,genzong--bank_dfee[%.2f] yl_fee[%.2f]",__FILE__,__LINE__,bank_dfee,yl_fee);
	set_zd_data("1211",sAc_Fee);  /* 银联手续费收入科目 */
	set_zd_data("1215","2");    /*转帐*/
	set_zd_data("1214",g_pub_tx.cur_no);
	if(cz_ind[0] == '1')
	{
		tx_amt2 = -1 * yl_fee;
		set_zd_data("121A","银联手续费冲正");
		set_zd_data("120A","ATM手续费冲正");
		set_zd_data("102Y","ATM手续费冲正");
		set_zd_data("1026","N"); /*冲正交易不检查密码*/
	}
	else if(cz_ind[0]=='0')
	{
		tx_amt2 = yl_fee;
		set_zd_data("121A","银联手续费");
		set_zd_data("120A","手续费");
		set_zd_data("102Y","手续费");
	}
	/** 银联柜面通他代本存款相关交易特殊处理 20130326 hzh **/
	if(fee_ind[0] == 'b' &&　cz_ind[0]=='0' && cx_ind == 1)
	{
		tx_amt2 = -1 * yl_fee;
		set_zd_data("121A","银联手续费冲正");
		set_zd_data("120A","柜面通手续费冲正");
		set_zd_data("102Y","柜面通手续费冲正");
		set_zd_data("1026","N"); /*冲正交易不检查密码*/
	}
	 if(fee_ind[0] == 'b' && cz_ind[0]=='1' && cx_ind == 1)
	{
		tx_amt2 = yl_fee;
		set_zd_data("121A","银联手续费");
		set_zd_data("120A","柜面通手续费");
		set_zd_data("102Y","柜面通手续费");
	}
	/** 银联柜面通他代本存款相关交易特殊处理 20130326 hzh **/
	/** 计算开封:ATM划到其他应付款的账号 **/
	/**Del by hxc 2010-1-28 0:01**/
	/**
	if(s_ct_ind[0]=='1')取现向银联转手续费
	{
		set_zd_double("1218",tx_amt2); 银联手续费金额
	}
	else
	{
		set_zd_double("1218",0.00);转帐时不向银联转手续费3.60
	}
	**/
	/**Del by hxc 2010-1-28 0:01**/
	set_zd_double("1218",tx_amt2);/**Add by hxc 2010-1-28 0:01**/
	
	vtcp_log("%s,%d,取科目完毕sAc_Fee[%s],sAc_no[%s],zAc_no[%s]",__FILE__,__LINE__,sAc_Fee,sAc_no,zAc_no);

	if(fee_ind[0]=='1' || fee_ind[0] =='2' || fee_ind[0]=='0'||fee_ind[0]=='6')
	{
		vtcp_log("%s,%d本行ATM和POS交易不收费",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"0000");
		goto OkExit;
	}
	/*当本行卡在他行ATM取款，他行柜面取款，和境外取款时,收费*/
	if((fee_ind[0] == '3' || fee_ind[0] == '4' || fee_ind[0]=='5' || fee_ind[0]=='8'|| fee_ind[0]=='9' || fee_ind[0]=='i' || fee_ind[0]=='a') && (memcmp(crd_no,CARDHEADCODE,6) == 0 || memcmp(crd_no,ICCARDHEADCODE,6) == 0 || strlen(cAcc_no)!=0 ) && dc_ind[0]=='1' )
	{
		vtcp_log("%s,%d收费分支",__FILE__,__LINE__);
		/*同城他行ATM取现,借客户和垫付,贷:贷清算款项, spD003,spF071,spA017*/
		if((fee_ind[0] == '3') && s_ct_ind[0] =='1')  /*同城他行ATM取现*/
		{
			/*同城他行ATM取款客户收取费用*/
			/*同城AT取现收取客户费用,可以重头上宏定义修改*/
			/**Del by hxc 100125**/
			/**ret = pub_base_GetParm_double("KFCAR",3,&tckh_fee);
			if(ret)
			{
				vtcp_log("%s,%d取客户应付费用失败!",__FILE__,__LINE__);
				tckh_fee=YDQX_KH_FEE;
				vtcp_log("%s,%d取客户应付费用失败!取默认值[%lf]",__FILE__,__LINE__,tckh_fee);
				goto ErrExit;
			}
			**/
			/**End by hxc 100125**/
			tckh_fee=dFee_amt; /**收取客户金额 add by hxc 100125**/
			if(cz_ind[0] == '1')
			{
				tx_amt2= 0 - tckh_fee;
				set_zd_double("102F",tx_amt2); /*收取客户金额*/
			
			}
			else
			{
				set_zd_double("102F",tckh_fee);
			}	
			if(pub_base_CompDblVal(tckh_fee,yl_fee)<0)
			{
				g_pub_tx.tx_amt1 = yl_fee - tckh_fee;
				if(cz_ind[0]=='1')
				{
					g_pub_tx.tx_amt1 = 0 - g_pub_tx.tx_amt1;
				}
		     		set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0930",DR_FLAG);/*借贷标志*/
				set_zd_data("0152",zAc_no);
		     		memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.ac_no,zAc_no);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,同城他行atm取现垫付分支",__FILE__,__LINE__);
			}
			else
			{ /*手续费收入,占时走不到此分支*/
				g_pub_tx.tx_amt1 =tckh_fee - yl_fee;
				if(cz_ind[0]=='1')
				{
					g_pub_tx.tx_amt1 = 0 - g_pub_tx.tx_amt1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
				vtcp_log("%s,%d,同城他行atm取现,收入",__FILE__,__LINE__);
			}
		}
		else if((fee_ind[0] == '3') && s_ct_ind[0] =='2') /*同城他行ATM转帐*/
		{
			/*** 开封用不到这些比例，按笔收费跟金额没什么关系 20090421
			if(pub_base_CompDblVal(tx_amt,TCZZ_BZ1)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ1)==0)
			{
				vtcp_log("%s,%d取转装手续费失败取默认值!",__FILE__,__LINE__);
				tx_amt1 = TCZZ_KH_FEE1;
			}
			else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ2)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ2)==0)
			{
				tx_amt1 = TCZZ_KH_FEE2;
			}
			else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ3)<0 || pub_base_CompDblVal(tx_amt,TCZZ_BZ3)==0)
			{
				tx_amt1 = TCZZ_KH_FEE3;
			}
			else if(pub_base_CompDblVal(tx_amt,TCZZ_BZ3)==1)
			{
				tx_amt1 = TCZZ_KH_FEE4;
			}
			tx_amt1=YDQX_KH_FEE;  **异地ATM取款每笔2元 *
			***/
			/** 开封每笔两块钱 20090422**/
			/**Add by hxc 100125**/
			/**ret = pub_base_GetParm_double("KFCAR",3,&tx_amt1);
			if(ret)
			{
				vtcp_log("%s,%d取客户应付费用失败!",__FILE__,__LINE__);
				goto ErrExit;
			}**/
			/**End by hxc 100125**/
			tx_amt1=dFee_amt; /**取客户应付费用金额 add by hxc 100125**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2= -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			/*转帐的时候不给银联反手续费,因此提取客户手续费,都记入银行的手续费收入,modified by wyw 20080411*/
			/*
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				*垫付*
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1=g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1);*手续费垫付*
				set_zd_data("0930",DR_FLAG); *借贷标志*
				set_zd_data("0152",zAc_no);
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.add_ind,"0");*借方标志*
				vtcp_log("%s,%d,同城他行atm转帐,垫付",__FILE__,__LINE__);
			}
			else
			{
			}
			*/
			
			/**Del by hxc 100429**/
			/*收入*/
			/*g_pub_tx.tx_amt1 = tx_amt1;
			if(cz_ind[0] == '1')
			{
				g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
			}
	     		set_zd_double("0420",g_pub_tx.tx_amt1); 手续费收入*/
			/*set_zd_data("0930",CR_FLAG); 借贷标志
			set_zd_data("0152",sAc_no);
			strcpy(g_pub_tx.ac_no,sAc_no);
	     		memcpy(g_pub_tx.brf,"手续费收入",10);
			strcpy(g_pub_tx.add_ind,"1");贷方标志*/
			/**Del by hxc 100429**/
			
			/**Add by hxc 100429**/
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额 add by hxc 100125**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,同城他行atm转帐,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
				/*垫付*/
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"垫付费用",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,同城他行atm转帐,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,同城他行atm转帐,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*垫付金额*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,同城他行atm转帐,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
			/**End by hxc 100429**/
		}
		else if((fee_ind[0] == '4') && s_ct_ind[0] =='1') /*异地他行ATM取现*/
		{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			/** 开封是按笔收费的跟交易金额没关系 20090421
			tx_amt2 = tx_amt * YDQX_AMT_RATE;
			if(pub_base_CompDblVal(tx_amt2,YDQX_RATE_ZDFEE) <0)
			{
				tx_amt1 = YDQX_RATE_ZDFEE;
			}else if(pub_base_CompDblVal(tx_amt2,YDQX_RATE_ZGFEE)==1)
			{
				vtcp_log("%s,%d,收取多少[%.2f]",__FILE__,__LINE__,tx_amt2);
				tx_amt1 = YDQX_RATE_ZGFEE;
			}else{
				tx_amt1 = tx_amt * YDQX_AMT_RATE;
			}
			tx_amt1 = tx_amt1 + YDQX_KH_FEE;
			*******/
			/** 开封每笔两块钱 20090422**/
			/**Del by hxc 100125**/
			/**
			ret = pub_base_GetParm_double("KFCAR",6,&tx_amt1);
			if(ret)
			{
				vtcp_log("%s,%d取客户应付费用失败!",__FILE__,__LINE__);
				goto ErrExit;
			}**/
			/**End by hxc 100125**/
			tx_amt1=dFee_amt; /**取客户应付费用金额 add by hxc 100125**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
				/*垫付*/
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"垫付费用",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,同城他行atm取现,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,异地他行atm取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*垫付金额*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,异地他行atm取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
		}
		else if((fee_ind[0] == '4') && s_ct_ind[0] =='2') /*异地他行ATM转帐*/
		{
			/******开封按笔收费不按金额收费 20090422 wyw 
			tx_amt2 = tx_amt * YDZZ_AMT_RATE; 
			vtcp_log("%s,%d,客户手续费[%lf]",__FILE__,__LINE__,tx_amt2);
			if(pub_base_CompDblVal(tx_amt2,YDZZ_ZDKH_FEE) <0)
			{
				vtcp_log("%s,%d,手续费[%f]",__FILE__,__LINE__,tx_amt1);
				tx_amt1 = YDZZ_ZDKH_FEE;
			}else if(pub_base_CompDblVal(tx_amt2,YDZZ_ZGKH_FEE)>0)
			{
				tx_amt1 = YDZZ_ZGKH_FEE;
				vtcp_log("%s,%d,手续费[%lf]",__FILE__,__LINE__,tx_amt1);
			}else{
				tx_amt1 = tx_amt * YDZZ_AMT_RATE;
				vtcp_log("%s,%d,手续费[%f]",__FILE__,__LINE__,tx_amt1);
			}
			*******/
			/** 开封每笔两块钱 20090422**/
			/**Del by hxc 100125**/
			/**
			ret = pub_base_GetParm_double("KFCAR",6,&tx_amt1);
			if(ret)
			{
				vtcp_log("%s,%d取客户应付费用失败!",__FILE__,__LINE__);
				tx_amt1=YDQX_KH_FEE;
				vtcp_log("%s,%d取客户应付费用失败!取默认值[%lf]",__FILE__,__LINE__,tckh_fee);
				goto ErrExit;
			}**/
			/**End by hxc 100125**/
			/**Add by hxc 100127**/
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额 add by hxc 100125**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,异地他行atm转帐,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
				/*垫付*/
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"垫付费用",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,同城他行atm转帐,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,异地他行atm转帐,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*垫付金额*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,异地他行atm转帐,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
			/**End by hxc 100127**/	
		}
		else if(fee_ind[0] == '5')/*境外交易收费*/
		{
			/**境外不支持转帐业务**/
			if(s_ct_ind[0] !='1')
			{
				vtcp_log("%s,%d,境外不支持转帐业务",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"CU06");
				goto ErrExit;
			}
			if(s_ct_ind[0] == '1')
			{
				/** 开封按笔数收费不按金额每笔15元 **/	
				/**Del by hxc 100125**/
				/**ret = pub_base_GetParm_double("KFCAR",9,&tx_amt1);
				if(ret)
				{
					vtcp_log("%s,%d取客户应付费用失败!",__FILE__,__LINE__);
					goto ErrExit;
				}**/
			
				/**End by hxc 100125**/
				/** 开封收客户15元钱全部给郑州 ,不存在手续费收入*/
				/*set_zd_double("1218",tx_amt1); * 银联收取手续费 */
				/**g_pub_tx.tx_amt1=0.0; del by hxc 100125**/
				/**Del by hxc 100225**/
				/**g_pub_tx.tx_amt1=dFee_amt; 取客户应付费用金额 add by hxc 100125**/
				/*收入*/
				/**if(cz_ind[0] =='1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
					tx_amt2= 0 - tx_amt1;
					set_zd_double("102F",tx_amt2);
					
				}else{
					set_zd_double("102F",tx_amt1);
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1);**/ /*手续费收入*/
				/*set_zd_data("0930",CR_FLAG);*/ /*借贷标志*/
				/*set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,"2",1);*/ /*现转*/
				/*strcpy(g_pub_tx.add_ind,"1");*/ /*借方标志*/
	     			/*memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,同城他行atm转帐,收入 ",__FILE__,__LINE__);*/
			
		    		/**End by hxc 100225**/
				dFee_amt=0.00;
				get_zd_double("0410",&dFee_amt);
				tx_amt1=dFee_amt; /**取客户应付费用金额 add by hxc 100225**/
				if(cz_ind[0] == '1')/*冲正*/
				{
					tx_amt2 = -1 *tx_amt1;
					set_zd_double("102F",tx_amt2);
				}
				else
				{
					set_zd_double("102F",tx_amt1);
				}
				vtcp_log("%s,%d,手续费tx_amt1[%lf],银联境外手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_jwfee);
				yl_fee=0.00;
				yl_fee=yl_jwfee;
				if(pub_base_CompDblVal(tx_amt1,yl_jwfee)<0)
				{
					g_pub_tx.tx_amt1 = yl_jwfee - tx_amt1;
					if(cz_ind[0] == '1')
					{
						g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
					}
					/*垫付*/
	     				set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
					set_zd_data("0152",zAc_no);
					set_zd_data("0930",DR_FLAG); /*借贷标志*/
					strcpy(g_pub_tx.ac_no,zAc_no);
	     				memcpy(g_pub_tx.brf,"垫付费用",10);
					strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
					vtcp_log("%s,%d,境外ATM取现,垫付",__FILE__,__LINE__);
				}
				else
				{
					/*收入*/
					g_pub_tx.tx_amt1 = tx_amt1 - yl_jwfee;
					vtcp_log("%s,%d,境外ATM取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
					if(cz_ind[0] == '1')
					{
						g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
					}
	     				set_zd_double("0420",g_pub_tx.tx_amt1); /*垫付金额*/
					set_zd_data("0930",CR_FLAG); /*借贷标志*/
					set_zd_data("0152",sAc_no);
					strcpy(g_pub_tx.ac_no,sAc_no);
	     				memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
					strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     				memcpy(g_pub_tx.brf,"手续费收入",10);
					vtcp_log("%s,%d,境外ATM取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				}
			}
		}
		else if(fee_ind[0] == '9' && s_ct_ind[0] =='1') /*农民工卡取现 Add by hxc 110513*/
		{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,客户手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				/*垫付*/
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,农民工卡取现,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,农民工卡取现,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,农民工卡取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,农民工卡取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
		}
		else if(fee_ind[0] == '8' && s_ct_ind[0] =='1') /*助农取款取现 Add by hxc 121217*/
		{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,客户手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				/*垫付*/
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,助农取款取现,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,助农取款取现,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,助农取款取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,助农取款取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
		}  
		else if(fee_ind[0]=='i')/*IC卡收费*/
		{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			yl_fee=IC_YL_FEE;
			vtcp_log("%s,%d,IC卡电子现金银联手续费[%.2f]",__FILE__,__LINE__,yl_fee);
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				/**set_zd_double("1208",tx_amt2);**/
				/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh begin**/
				set_zd_double("102F",tx_amt2);
				/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh end**/
			}
			else
			{
				/**set_zd_double("1208",tx_amt1);**/
				/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh begin**/
				set_zd_double("102F",tx_amt1);
				/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh end**/
			}
			vtcp_log("%s,%d,客户手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				/*垫付*/
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,IC卡交易,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
				set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
				memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,IC卡交易,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,IC卡交易,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
				set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
				memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
				memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,IC卡交易,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}
		}
		else if(fee_ind[0] == 'a' && s_ct_ind[0] =='1') /*他行柜面取现 Add by hzh 130324*/
		{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额**/
			if(cz_ind[0] == '1')/*冲正*/
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,客户手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				/*垫付*/
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,他行柜面取款取现,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"手续费垫付",10);
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				vtcp_log("%s,%d,他行柜面取款取现,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,他行柜面取款取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] == '1')
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,他行柜面取款取现,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}	
		}
		memcmp(g_pub_tx.cur_no,"01",2); /*币种*/
		strcpy(g_pub_tx.sub_tx_code,"F071");
		strcpy(g_pub_tx.tx_code,"6203");
		strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
		strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
		strcpy( g_pub_tx.ac_wrk_ind,"000" );
		strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
		g_pub_tx.svc_ind=9001;  /*内部帐存取*/
		strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
		if(strlen(g_pub_tx.crd_no)>12)
			set_zd_data("0770",sMdm_ac_rel.opn_br_no);
		else
			set_zd_data("0770",v_mdm_ac_rel.opn_br_no);
		vtcp_log("%s,%d,[0770]sMdm_ac_rel.opn_br_no[%s]",__FILE__,__LINE__,sMdm_ac_rel.opn_br_no);
		vtcp_log("%s,%d,手续费tx_amt1[%lf],tx_amt2[%lf],g_pub_tx.tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,tx_amt2,g_pub_tx.tx_amt1,yl_fee);	
		ret = pub_acct_trance();
		if(ret)
		{
		  vtcp_log("%s,%d,调用存款主控失败!",__FILE__,__LINE__);
		  goto ErrExit;
		}
		/**
	  if(fee_ind[0]=='i')
	  {
	  	strcpy(g_pub_tx.reply,"0002");
	  }
	  else
	  {
			strcpy(g_pub_tx.reply,"0002");
		}**/
		/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh begin**/
		strcpy(g_pub_tx.reply,"0002");
		/**电子现金业务客户手续费从客户借记账户收取 20131106 hzh end**/
		goto OkExit;
	}
		/*本行卡通过他行atm或者柜面存款时，收费*/
	if(fee_ind[0]=='b')
	{
			vtcp_log("%s,%d,交易金额[%.2f]",__FILE__,__LINE__,tx_amt);
			dFee_amt=0.00;
			get_zd_double("0410",&dFee_amt);
			tx_amt1=dFee_amt; /**取客户应付费用金额**/

			if(cz_ind[0] != '1' && cx_ind ==1)
			{
				tx_amt2 = -1 *tx_amt1;
				set_zd_double("102F",tx_amt2);
			}
			else
			{
				set_zd_double("102F",tx_amt1);
			}
			vtcp_log("%s,%d,客户手续费tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,yl_fee);	
			if(pub_base_CompDblVal(tx_amt1,yl_fee)<0)
			{
				/*垫付*/
				g_pub_tx.tx_amt1 = yl_fee - tx_amt1;
				vtcp_log("%s,%d,跨行存款,垫付[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] != '1' && cx_ind ==1)
				{
				g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费垫付*/
				set_zd_data("0152",zAc_no);
				set_zd_data("0930",DR_FLAG); /*借贷标志*/
				strcpy(g_pub_tx.add_ind,"0");/*借方标志*/
				strcpy(g_pub_tx.ac_no,zAc_no);
	     			memcpy(g_pub_tx.brf,"手续费垫付",10);
				vtcp_log("%s,%d,跨行存款,垫付",__FILE__,__LINE__);
			}
			else
			{
				/*收入*/
				g_pub_tx.tx_amt1 = tx_amt1 - yl_fee;
				vtcp_log("%s,%d,跨行存款,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
				if(cz_ind[0] != '1' && cx_ind ==1)
				{
					g_pub_tx.tx_amt1 = g_pub_tx.tx_amt1 * -1;
				}
	     			set_zd_double("0420",g_pub_tx.tx_amt1); /*手续费收入*/
				set_zd_data("0930",CR_FLAG); /*借贷标志*/
				memcpy(g_pub_tx.ct_ind,CR_FLAG,1); /*贷*/
				strcpy(g_pub_tx.add_ind,"1");/*贷方标志*/
				set_zd_data("0152",sAc_no);
				strcpy(g_pub_tx.ac_no,sAc_no);
	     			memcpy(g_pub_tx.brf,"手续费收入",10);
				vtcp_log("%s,%d,跨行存款,收入[%.2f] ",__FILE__,__LINE__,g_pub_tx.tx_amt1);
			}	
				memcmp(g_pub_tx.cur_no,"01",2); /*币种*/
				strcpy(g_pub_tx.sub_tx_code,"F071");
				strcpy(g_pub_tx.tx_code,"6203");
				strcpy( g_pub_tx.ac_id_type,"9" ); /*账户类型为内部*/
				strcpy( g_pub_tx.ac_get_ind,"00" ); /*本程序未读取分户*/
				strcpy( g_pub_tx.ac_wrk_ind,"000" );
				strcpy( g_pub_tx.prt_ind,"0" ); /*不登折*/
				g_pub_tx.svc_ind=9001;  /*内部帐存取*/
				strcpy(g_pub_tx.hst_ind,"1"); /*日间入明细*/
				if(strlen(g_pub_tx.crd_no)>12)
					set_zd_data("0770",sMdm_ac_rel.opn_br_no);
				else
					set_zd_data("0770",v_mdm_ac_rel.opn_br_no);
				vtcp_log("%s,%d,[0770]sMdm_ac_rel.opn_br_no[%s]",__FILE__,__LINE__,sMdm_ac_rel.opn_br_no);
				vtcp_log("%s,%d,手续费tx_amt1[%lf],tx_amt2[%lf],g_pub_tx.tx_amt1[%lf],银联手续费[%lf]",__FILE__,__LINE__,tx_amt1,tx_amt2,g_pub_tx.tx_amt1,yl_fee);	
				ret = pub_acct_trance();
				if(ret)
				{
				  vtcp_log("%s,%d,调用存款主控失败!",__FILE__,__LINE__);
				  goto ErrExit;
				}
	  		if(fee_ind[0]=='i')
	  		{
	  			strcpy(g_pub_tx.reply,"0003");
	  		}
	  		else
	  		{
					strcpy(g_pub_tx.reply,"0002");
				}
				goto OkExit;
	}
	/*Add by hxc for JZ 100125 本行pos收取手续费跟商户签约有关 */
	if (fee_ind[0]=='7')
	{
		vtcp_log("%s %d 进入本行pos分支。。。。。。",__FILE__,__LINE__);
		vtcp_log("%s %d crd_no2=[%s]",__FILE__,__LINE__,crd_no2);
		set_zd_data("1021",crd_no2);
		set_zd_data("1022","0");/*活期序号*/
		set_zd_data("1028",g_pub_tx.draw_pwd);
		set_zd_data("102K","2");/*现转标志*/
		set_zd_data("102J","01");

		if (cz_ind[0] == '1')
		{	
			if (dc_ind[0]=='2')
			{
				dFee_amt = dFee_amt;
			}
			else
			{
				dFee_amt = -1 * dFee_amt;
			}
			set_zd_data("121A","本行pos手续费冲正");
			set_zd_data("102Y","本行pos手续费冲正");
			set_zd_data("1026","N"); /*冲正交易不检查密码*/
		}
		else if(cz_ind[0]=='0')
		{
			if (dc_ind[0]=='1')
			{
				dFee_amt = dFee_amt;
			}
			else
			{
				dFee_amt = -1 * dFee_amt;
			}
			set_zd_data("121A","本行pos手续费");
			set_zd_data("102Y","本行pos手续费");
		}			
		set_zd_double("102F",dFee_amt);

		memset(sAc_Fee,0x00,sizeof(sAc_Fee));
		ret = pub_base_GetParm_Str("YLCSR",2,sAc_Fee);
		if (ret)
		{
			strncpy(sAc_Fee,BH_SQPOSFEE_KM,sizeof(sAc_Fee)-1);
			vtcp_log("%s,%d 没有找到收费科目所以启用默认帐户=%s",__FILE__,__LINE__,sAc_Fee);
			goto ErrExit;
		}
		
		set_zd_data("1211",sAc_Fee);  /* POS手续费收入科目 */
		set_zd_data("1215","2");    /*转帐*/
		set_zd_data("1214",g_pub_tx.cur_no);
		set_zd_double("1218",dFee_amt);	
		strcpy( g_pub_tx.reply, "0002" );
		goto OkExit;
	}
	/*End by hxc for JZ 100125 本行pos收取手续费跟商户签约有关*/
	
	
	vtcp_log("%s,%d不收费分支1",__FILE__,__LINE__);
	strcpy( g_pub_tx.reply, "0000" );
OkExit:
	sprintf(acErrMsg,"本行设备外行卡处理!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	if(strlen(g_pub_tx.reply)==0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}
	sprintf(acErrMsg,"ATM取款失败!! ");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
