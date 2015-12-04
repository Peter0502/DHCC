/******************************************************
* 文 件 名:  spB380.c
* 功能描述:  宁夏需求-如果承兑保证金到期日当天，
*            需要着急扣款的话，那么做本交易!
* 交易目标:  检查承兑保证金和结出的利息是否够扣,如果够扣的话，扣款!
*            否则，再去检查对应活期户，再去扣款，如果不够扣，报错!
*
* 作    者:  xyy
* 完成日期： 2009-9-20 15:17:52
注意：保证金为年利率
*
*******************************************************/
#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bail_rate_c.h"
#include "mo_hold_c.h"
#include "com_parm_c.h"
static struct dd_mst_c sDdMst;    /* 活期主文件 */
static struct mdm_ac_rel_c sMdm_ac_rel;	/* 介质账号对照表 */
int spB380()
{
	int     ret = 0;
	long     iDay = 0;
	long     HQday = 0;
	double  HQ_rate   = 0.00;               /* 活期利率         */
	double  HQdealval = 0.00;               /* 保证金活期利息   */
	double  dYfintst  = 0.00;               /* 应付利息         */
	double  dTmpAmt   = 0.00;               /* 需要扣活期的金额 */

	double dSum_po_amt=0.00;                /** add by xyy 2010-6-28 17:10:35 DEBUG**/

	char    cPact_no[21];                   /* 承兑协议号       */
	char    cPay_ac_no[25];                 /* 承兑汇票对应活期账户*/

	struct dd_parm_c        gDd_parm;
	struct com_parm_c       sCom_parm;
	struct mo_bank_po_c     sMoBankPo;      /* 承兑汇票登记簿   */
	struct mo_bail_rate_c   sMoBailRate;    /* 保证金利率登记簿 */
	struct mo_bank_acc_po_c sMoBankAccPo;   /* 银行承兑汇票基本信息表 */

	memset(cPact_no     , 0 , sizeof(cPact_no));
	memset(cPay_ac_no   , 0 , sizeof(cPay_ac_no));
	memset(&sDdMst      , 0 , sizeof(struct dd_mst_c));
	memset(&sCom_parm   , 0 , sizeof(struct com_parm_c));
	memset(&gDd_parm    , 0 , sizeof(struct dd_parm_c));
	memset(&sMoBankPo   , 0 , sizeof(struct mo_bank_po_c));
	memset(&sMdm_ac_rel , 0 , sizeof(struct mdm_ac_rel_c));
	memset(&sMoBankAccPo, 0 , sizeof(struct mo_bank_acc_po_c));
	memset(&sMoBailRate , 0 , sizeof(struct mo_bail_rate_c));

	get_zd_data("0630",cPact_no);
	vtcp_log("[%s][%d] cPact_no=[%s] ",__FILE__,__LINE__,cPact_no);
	ret = Mo_bank_acc_po_Dec_Sel(g_pub_tx.reply, "pact_no='%s' ", cPact_no);
	if (ret) 
	{
		sprintf(acErrMsg, "执行Mo_bank_acc_po_Dec_Sel错![%d]", ret);
		WRITEMSG
		strncpy(g_pub_tx.reply,"B217",4);
		goto ErrExit;
	}
	ret = Mo_bank_acc_po_Fet_Sel(&sMoBankAccPo, g_pub_tx.reply);
	if (ret) 
	{
		sprintf(acErrMsg, "执行Mo_bank_acc_po_Fet_Sel错![%d]", ret);
		WRITEMSG
		strncpy(g_pub_tx.reply,"B060",4);
		goto ErrExit;
	}
	vtcp_log("[%s][%d] sMoBankAccPo.acc_po_sts=[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_sts);
	if (sMoBankAccPo.acc_po_sts[0] != '2') 
	{
		sprintf(acErrMsg, "此承兑协议未做签发或未全部签发!");
		WRITEMSG
		strncpy(g_pub_tx.reply,"B211",4);
		goto ErrExit;
	}
	/**************add by xyy 2010-6-28 17:11:53 DEBUG *******
	if(sMoBankAccPo.acc_type[0] !='1' && sMoBankAccPo.acc_type[0] !='3')
	{
	sprintf(acErrMsg, "本交易只处理保证型承兑汇票!");
	WRITEMSG
	strncpy(g_pub_tx.reply,"B189",4);
	goto ErrExit;
	}
	*******************end by xyy 2010-6-28 17:12:07**********************/

	vtcp_log("[%s][%d] sMoBankAccPo.acc_po_ind=[%s]",__FILE__,__LINE__,sMoBankAccPo.acc_po_ind);
	vtcp_log("[%s][%d] sMoBankAccPo.matr_date=[%d]",__FILE__,__LINE__,sMoBankAccPo.matr_date);
	if (sMoBankAccPo.matr_date > g_pub_tx.tx_date)
	{
		sprintf(acErrMsg, "承兑汇票未到期![%d]", sMoBankAccPo.matr_date);
		WRITEMSG
		strncpy(g_pub_tx.reply,"B227",4);
		goto ErrExit;
	}
	if (sMoBankAccPo.acc_po_ind[0]=='1')
	{
		sprintf(acErrMsg, "承兑汇票已做到期处理![%s]", sMoBankAccPo.acc_po_ind);
		WRITEMSG
		strncpy(g_pub_tx.reply,"B223",4);
		goto ErrExit;
	}
	vtcp_log("[%s][%d] sMoBankAccPo.tx_br_no=[%s]",__FILE__,__LINE__,sMoBankAccPo.tx_br_no);
	if (memcmp(sMoBankAccPo.tx_br_no,g_pub_tx.tx_br_no,sizeof(sMoBankAccPo.tx_br_no)-1)!=0)
	{
		sprintf(acErrMsg, "请签发机构做此交易!");
		WRITEMSG
		strncpy(g_pub_tx.reply,"B305",4);
		goto ErrExit;
	}
	/******add by xyy 2010-6-28 17:03:24 DEBUG********/
	sql_sum_double( "mo_bank_po","po_amt",&dSum_po_amt,"pact_no='%s' and po_sts<>'3' ",sMoBankAccPo.pact_no);
	vtcp_log("[%s][%d] dSum_po_amt =[%f] ",__FILE__,__LINE__,dSum_po_amt);
	/***********end by xyy 2010-6-28 17:03:40 DEBUG************/

	vtcp_log("[%s][%d] sMoBankAccPo.sign_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.sign_amt);
	/**保证金金额不能大于签发金额**/

	/************modify by xyy 2010-6-28 17:08:52 DEBUG *******
	if (pub_base_CompDblVal(sMoBankAccPo.bail_amt,sMoBankAccPo.sign_amt) > 0) 
	***********************************************************/

	if (pub_base_CompDblVal(sMoBankAccPo.bail_amt,dSum_po_amt) >= 0) 
	{
		/*************modify by xyy 2010-6-28 17:09:26 DEBUG **
		sMoBankAccPo.bail_amt = sMoBankAccPo.sign_amt;
		*******************************************************/
		sMoBankAccPo.bail_amt = dSum_po_amt;

		/***
		sprintf(acErrMsg, "保证金余额大于汇票签发金额!!");
		WRITEMSG
		strncpy(g_pub_tx.reply,"B074",4);
		goto ErrExit;
		***/
	}
	else
	{
		/**活期户再支出合计总数---暂时没有算结出的利息**/
		/*******modify by xyy 2010-6-28 17:04:02 DEBUG*****        
		dTmpAmt = sMoBankAccPo.sign_amt - sMoBankAccPo.bail_amt;
		***************************************************/
		dTmpAmt = dSum_po_amt - sMoBankAccPo.bail_amt;

		vtcp_log("[%s][%d] dTmpAmt =[%f] \n",__FILE__,__LINE__,dTmpAmt);
	}
	vtcp_log("[%s][%d] bail_amt=[%f]\n",__FILE__,__LINE__,sMoBankAccPo.bail_amt);
	strcpy(cPay_ac_no, sMoBankAccPo.payer_ac_no);
	pub_base_old_acno(cPay_ac_no);
	vtcp_log("[%s][%d] cPay_ac_no=[%s]",__FILE__,__LINE__,cPay_ac_no);

	/* 检查保证金账号余额是否够扣 */
	if (pub_base_CompDblVal(sMoBankAccPo.bail_amt,0.005) > 0)
	{
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",sMoBankAccPo.bail_ac_no);
		if (ret)
		{
			sprintf(acErrMsg, "保证金账号不存在![%s]", sMoBankAccPo.bail_ac_no);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B070");
			goto ErrExit;
		}
		vtcp_log("[%s][%d] ac_id =[%ld] ac_seqn=[%ld]",__FILE__,__LINE__,sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn);
		ret = Dd_mst_Dec_Upd(g_pub_tx.reply, "ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn);
		if (ret)
		{
			sprintf(acErrMsg, "执行Dd_mst_Sel错误![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B114");
			goto ErrExit;
		}
		ret = Dd_mst_Fet_Upd(&sDdMst, g_pub_tx.reply);
		if (ret )
		{
			sprintf(acErrMsg, "保证金账号不是活期账号!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "B196");
			goto ErrExit;
		}
		/** 如果是冻结, 先解冻 **/
		ret = igD090_DisFrz('1');
		if (ret) 
		{
			sprintf(acErrMsg, "自动解冻错误![%d]", ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "O016");
			goto ErrExit;
		}
		vtcp_log("[%s][%d] 更新保证金的冻结登记薄",__FILE__,__LINE__);
		ret = Dd_mst_Upd_Upd(sDdMst, g_pub_tx.reply);
		if (ret)
		{
			sprintf(acErrMsg, "修改保证金冻结状态错误!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "H020");
			goto ErrExit;
		}
		Dd_mst_Clo_Upd();
		vtcp_log("[%s][%d]承兑汇票保证金解冻成功!sMoBankAccPo.pact_no=[%s]",__FILE__,__LINE__,sMoBankAccPo.pact_no);
		/**--- 下一步，准备计算保证金利息 --- ***/
		ret = Mo_bail_rate_Dec_Sel(g_pub_tx.reply, " bail_type='1' and pact_no='%s' and sts='1' ", sMoBankAccPo.pact_no);
		if (ret) 
		{
			sprintf(acErrMsg, "执行Mo_bail_rate_Dec_Upd错![%d][%s]", ret, sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf(g_pub_tx.reply, "B334");
			goto ErrExit;
		}
		ret = Mo_bail_rate_Fet_Sel(&sMoBailRate, g_pub_tx.reply);
		if (ret)
		{
			sprintf(acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]", ret, sMoBankAccPo.pact_no);
			WRITEMSG
			sprintf(g_pub_tx.reply, "B334");
			goto ErrExit;
		}
		Mo_bail_rate_Clo_Sel();
		/******
		if(sMoBailRate.sts[0]!='1')
		{
		sprintf(acErrMsg, "执行Mo_bail_rate_Fet_Upd错![%d][%s]", ret, sMoBankAccPo.pact_no);
		WRITEMSG
		sprintf(g_pub_tx.reply, "B335");
		goto ErrExit;
		}
		vtcp_log("[%s][%d] beg_date=[%ld] end_date=[%ld]",__FILE__,__LINE__,sMoBailRate.beg_date,sMoBailRate.end_date);

		ret = pub_base_getllz(HQLLBH, "01", g_pub_tx.tx_date, &HQ_rate);
		if (ret) {
		sprintf(acErrMsg, "取利率错误!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O195");
		goto ErrExit;
		}
		* 判断是否到期 *
		if (g_pub_tx.tx_date < sMoBailRate.end_date) ** 提前支取 **
		{
		* 计算天数 *
		if (pub_base_CalDays(sMoBailRate.beg_date, g_pub_tx.tx_date, "0", &HQday)) {
		sprintf(acErrMsg, "计算天数错误!");
		WRITEMSG
		sprintf(g_pub_tx.reply, "O108");
		goto ErrExit;
		}
		vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
		iDay = 0;
		}
		else 
		{
		* 计算天数 *
		if (pub_base_CalDays(sMoBailRate.beg_date, sMoBailRate.end_date, "0", &iDay)) {
		sprintf(acErrMsg, "计算天数错误!");
		WRITEMSG
		goto ErrExit;
		}
		if (pub_base_CalDays(sMoBailRate.end_date, g_pub_tx.tx_date, "0", &HQday)) {
		sprintf(acErrMsg, "计算天数错误!");
		WRITEMSG
		goto ErrExit;
		}
		vtcp_log("[%s][%d] HQday =[%ld]",__FILE__,__LINE__,HQday);
		vtcp_log("[%s][%d] iDay =[%ld]",__FILE__,__LINE__,iDay);
		}
		sprintf(acErrMsg, "HQday=[%ld]HQ_rate=[%lf],day=[%ld],ic_rate=[%lf]", HQday, HQ_rate,iDay,sMoBailRate.ic_rate);
		WRITEMSG
		vtcp_log("[%s][%d]sMoBailRate.amt=[%f] g_pub_intst.dealval=[%f]",__FILE__,__LINE__,sMoBailRate.amt,g_pub_intst.dealval);
		vtcp_log("[%s][%d] 360=[%d] 100=[%d]",__FILE__,__LINE__,L360,L100);
		* 计算活期部分的利息 **
		HQdealval = HQ_rate * sMoBailRate.amt * HQday / L360 / L100;
		vtcp_log("[%s][%d] HQdealval=[%f]",__FILE__,__LINE__,HQdealval);
		** 计算高利率部分的利息 **
		dYfintst = sMoBailRate.ic_rate * sMoBailRate.amt * iDay / L360 / L100;
		vtcp_log("[%s][%d] dYfintst=[%f]",__FILE__,__LINE__,dYfintst);
		** 利息合计    **
		dYfintst = dYfintst + HQdealval;
		modify by chenchao 2011-8-9 9:14:29 
		咨询晋中商行业务,利息新算法:敞口保证金不计息,在维护的时候ic_rate为0,
		100%保证金就按输入利率计算因此改为以下计息方式 ****
		dYfintst = sMoBailRate.ic_rate * sMoBailRate.amt * iDay / L360 / L100;
		dYfintst = pub_base_PubDround_1(dYfintst);

		sprintf(acErrMsg, "保证金利息为=[%lf]", dYfintst);
		WRITEMSG

		if (pub_base_CompDblVal(dYfintst,0.00) < 0) 
		{
		sprintf(acErrMsg, "计算利息错误错误!");
		WRITEMSG
		strcpy(g_pub_tx.reply, "D158");
		goto ErrExit;
		}
		Mo_bail_rate_Clo_Sel();
		****/
		vtcp_log("[%s][%d] 本金[%f] ,利息为[%f]",__FILE__,__LINE__,sMoBankAccPo.bail_amt,dYfintst);
		vtcp_log("[%s][%d] 签发金额=[%f] 实际金额=[%f]",__FILE__,__LINE__,sMoBankAccPo.sign_amt,dSum_po_amt);
	}
	/* 检查保证金+利息是否够扣 */
	/***************modify by xyy 2010-6-28 17:05:19 DEBUG ************
	if (pub_base_CompDblVal(sMoBankAccPo.bail_amt+dYfintst,sMoBankAccPo.sign_amt) >= 0) 
	********************************************************************/
	if (pub_base_CompDblVal(sMoBankAccPo.bail_amt+dYfintst,dSum_po_amt) >= 0) 
	{
		sprintf(acErrMsg, "已经够扣了,不用往下走了!");
		WRITEMSG
		vtcp_log("[%s][%d] dYfintst =[%f] \n",__FILE__,__LINE__,dYfintst);
		set_zd_data("0680","2");/** 不去下客户结算户了 **/
		strcpy(g_pub_tx.reply, "0001");
	}
	else
	{
		sprintf(acErrMsg, "加上保证金的利息也不够扣哦,再去看看活期有钱没!");
		WRITEMSG

		vtcp_log("[%s][%d] 金额仍然不足,再去查客户帐 [%s]",__FILE__,__LINE__,sMoBankAccPo.payer_ac_no);
		/* 检查付款帐号的有效性:活期账号、账户状态正常 */
		ret = Mdm_ac_rel_Sel(g_pub_tx.reply, &sMdm_ac_rel, "ac_no='%s'",cPay_ac_no);
		if (ret )
		{
			sprintf(acErrMsg, "付款人账号不存在![%s][%d]", sMoBankAccPo.payer_ac_no,ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "F003");
			goto ErrExit;
		}
		ret = Dd_mst_Sel(g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn);
		if (ret)
		{
			sprintf(acErrMsg, "付款人账号不是活期账号[%d]!",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply, "B114");
			goto ErrExit;
		}
		/* 仅当账户状态正常时才去扣账 */
		if (sDdMst.ac_sts[0] == '1')
		{
			if (pub_base_CompDblVal(sDdMst.bal,0.00)>0) 
			{
				/**----- xxx ---- 冻结部分不能自动扣----------- 保留最低余额 ----------**/
				ret = Dd_parm_Sel(g_pub_tx.reply, &gDd_parm, "prdt_no='%s' ", sDdMst.prdt_no);
				if (ret)
				{
					sprintf(acErrMsg, "取产品参数表错误!");
					WRITEMSG
					strcpy(g_pub_tx.reply, "W024");
					goto ErrExit;
				}
				vtcp_log("[%s][%d] sDdMst.hold_sts=[%s] ",__FILE__,__LINE__,sDdMst.hold_sts);
				/**全部冻结或只进不出的情况**/
				if (sDdMst.hold_sts[0] == '1' || sDdMst.hold_sts[0] == '2')
				{
					/* mod by lwb 20141119   商行要求日间联机处理也进行垫款*/
					/*vtcp_log("[%s][%d] 付款帐户余额不足,该交易没法继续处理!!===[%f]",__FILE__,__LINE__,sDdMst.bal);
					strcpy(g_pub_tx.reply, "O161");
					goto ErrExit; */
					dTmpAmt -= (dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
					set_zd_double("0400", dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
					set_zd_data("0330",sMoBankAccPo.pact_no);
					vtcp_log("[%s][%d] 付款帐户余额不足[%f],将进行银行承兑垫款!!===[%f]",__FILE__,__LINE__,sDdMst.bal,dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
					
				}
				else if (sDdMst.hold_sts[0] == '3')/**部分冻结**/
				{
					if ( pub_base_CompDblVal(dTmpAmt, sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal + dYfintst)<=0)
					{
						vtcp_log("[%s][%d] sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal==[%f]",__FILE__,__LINE__,sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal);
						vtcp_log("[%s][%d] dTmpAmt =[%f] \n",__FILE__,__LINE__,dTmpAmt);
					}
					else
					{
						/* mod by lwb 20141119   商行要求日间联机处理也进行垫款*/
					/*	vtcp_log("[%s][%d] sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal==[%f]",__FILE__,__LINE__,sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal);
						vtcp_log("[%s][%d] 付款帐户余额不足,该交易没法继续处理!!===[%f]",__FILE__,__LINE__,sDdMst.bal);
						strcpy(g_pub_tx.reply, "0051");
						goto ErrExit;*/
						if(pub_base_CompDblVal(sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal,0.00)<0)
							dTmpAmt = 0.00;
						else
							dTmpAmt-= (dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-(sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal));
						set_zd_double("0400", dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-dTmpAmt);
						set_zd_data("0330",sMoBankAccPo.pact_no);
						vtcp_log("[%s][%d] dTmpAmt=[%f],付款帐户余额不足[%f],将进行银行承兑垫款!!===[%f]",__FILE__,__LINE__,dTmpAmt,sDdMst.bal - sDdMst.hold_amt - sDdMst.ctl_amt - gDd_parm.min_bal, dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-dTmpAmt);
					}
				}
				else 
				{    /** 非冻结 **/
					if (pub_base_CompDblVal(dTmpAmt, sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal +dYfintst)<=0)
					{
						vtcp_log("[%s][%d] sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal==[%f]",__FILE__,__LINE__,sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal);
						vtcp_log("[%s][%d] dTmpAmt =[%f] \n",__FILE__,__LINE__,dTmpAmt);
					}
					else
					{
						vtcp_log("[%s][%d] sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal==[%f]",__FILE__,__LINE__,sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal);
						vtcp_log("[%s][%d] dTmpAmt =[%f] \n",__FILE__,__LINE__,dTmpAmt);
					/* mod by lwb 20141119   商行要求日间联机处理也进行垫款*/
					/*	strcpy(g_pub_tx.reply, "P214");
						vtcp_log("[%s][%d] 付款帐户余额不足,该交易没法继续处理!!===[%f]",__FILE__,__LINE__,sDdMst.bal - sDdMst.ctl_amt - gDd_parm.min_bal);
						goto ErrExit;*/
						dTmpAmt-=(dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-(sDdMst.bal -  sDdMst.ctl_amt - gDd_parm.min_bal));
						set_zd_double("0400", dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-(sDdMst.bal -  sDdMst.ctl_amt - gDd_parm.min_bal));
						set_zd_data("0330",sMoBankAccPo.pact_no);
						vtcp_log("[%s][%d] 付款帐户余额不足[%f],将进行银行承兑垫款!!===[%f]",__FILE__,__LINE__,sDdMst.bal-  sDdMst.ctl_amt - gDd_parm.min_bal,dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst)-(sDdMst.bal -  sDdMst.ctl_amt - gDd_parm.min_bal));
					}
				}
			}
			else
			{
				/* mod by lwb 20141119   商行要求日间联机处理也进行垫款*/
				/*vtcp_log("[%s][%d] 付款帐户余额不足,该交易没法继续处理!!===[%f]",__FILE__,__LINE__,sDdMst.bal);
				strcpy(g_pub_tx.reply, "P214");
				goto ErrExit;*/
				dTmpAmt -= (dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
				set_zd_double("0400", dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
				set_zd_data("0330",sMoBankAccPo.pact_no);	
				vtcp_log("[%s][%d] 付款帐户余额不足[%f],将进行银行承兑垫款!!===[%f]",__FILE__,__LINE__,sDdMst.bal,dSum_po_amt-(sMoBankAccPo.bail_amt+dYfintst));
					
				
			}
		}
		set_zd_data("0680","1");/** 下客户结算户了 **/
		/** 由于晋中要求 质押存单也要做联机处理 因此 在这儿更新状态 **/
		if (pub_base_CompDblVal(sMoBankAccPo.bail_amt,0.00)==0 || sMoBankAccPo.acc_type[0] == '2')
		{
			ret = sql_execute2("update mo_bank_acc_po set acc_po_ind='1' where pact_no='%s' and acc_po_ind='0' and acc_po_sts='2'",sMoBankAccPo.pact_no);
			if (ret)
			{
				vtcp_log("[%s][%d] 更新mo_bank_acc_po失败",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply, "D105");
				goto ErrExit;
			}
			strcpy(g_pub_tx.reply, "0002");
		}else{
			strcpy(g_pub_tx.reply, "0001");
		}
	}
	/***else
	{
	sprintf(acErrMsg, "保证金账户余额错误!!");
	WRITEMSG
	strncpy(g_pub_tx.reply,"Z927",4);
	goto ErrExit;
	}***/
	/*活期取款金额*/
	set_zd_double("0420",dTmpAmt);
	vtcp_log("[%s][%d] dTmpAmt =[%f] \n",__FILE__,__LINE__,dTmpAmt);
	set_zd_data("0370",cPay_ac_no);

	/**应解汇款信息**/
	/***********modify by xyy 2010-6-28 17:07:24 DEBUG ****
	set_zd_double("0430",sMoBankAccPo.sign_amt);
	******************************************************/
	set_zd_double("0430",dSum_po_amt);
	/*
	char cQs_brno[BRNO_LEN+1];
	memset(cQs_brno,0x00,sizeof(cQs_brno));
	iFind_qs_br_no2(g_pub_tx.tx_br_no,cQs_brno);

	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='%s' and parm_seqn=10 ", cQs_brno);
	*/
	ret = Com_parm_Sel(g_pub_tx.reply, &sCom_parm, "parm_code='%s' and parm_seqn=10 ", g_pub_tx.opn_br_no);
	if (ret) 
	{
		sprintf(acErrMsg, "临时存款户不存在!!");
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data("0380",sCom_parm.val);
	vtcp_log("[%s][%d] pact_no=[%s] bail_ac_no=[%s]",__FILE__,__LINE__,cPact_no,sMoBankAccPo.bail_ac_no); 
	set_zd_data("0330", cPact_no);                  /* 承兑协议编号 */
	set_zd_data("0320", sMoBankAccPo.bail_ac_no);   /* 保证金帐号   */
	vtcp_log("[%s][%d] ac_no=[%s] sign_amt=[%f]",__FILE__,__LINE__,sMoBailRate.ac_no,sMoBankAccPo.sign_amt); 
	set_zd_data("0310", sMoBailRate.ac_no);         /* 收息帐号     */
	vtcp_log("[%s][%d] sMoBailRate.ac_no=[%s]",__FILE__,__LINE__,sMoBailRate.ac_no); 
	set_zd_data("0670", "Y");                       /* 计息标志     */
	set_zd_double("0390", sMoBankAccPo.bail_amt);   /* 取款金额     */
	vtcp_log("[%s][%d] sMoBankAccPo.sign_amt=[%f]",__FILE__,__LINE__,sMoBankAccPo.sign_amt);
	set_zd_double("1001", dYfintst);
	vtcp_log("[%s][%d] dYfintst=[%f]",__FILE__,__LINE__,dYfintst);
	Mo_bank_acc_po_Clo_Sel();
OkExit:
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
	if (memcmp(g_pub_tx.reply,"0000",4)==0)
		memcpy(g_pub_tx.reply,"B380",4);
	set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;

}
int igD380_DisFrz(char ac_type)
{
	int    g_reply_int = 0;
	struct mo_hold_c f_mo_hold;
	memset(&f_mo_hold,0x00,sizeof(f_mo_hold));

	/* 更新冻结登记薄 */
	g_reply_int = Mo_hold_Dec_Upd(g_pub_tx.reply,"ac_id=%ld and ac_seqn=%d and hold_sts='0' ", \
		sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn);
	if (g_reply_int) 
	{
		sprintf(acErrMsg, "查询冻结登记薄异常!");
		WRITEMSG
		goto ErrExit;
	}
	while (1) 
	{
		memset(&f_mo_hold, 0, sizeof(f_mo_hold));
		g_reply_int = Mo_hold_Fet_Upd(&f_mo_hold, g_pub_tx.reply);

		/** 旧数的冻结有记录, 新开承兑的冻结只改了主表状态 **/
		if (g_reply_int == 100) 
		{
			sprintf(acErrMsg, "已全部解冻!");
			WRITEMSG
			break;
		} 
		else if (g_reply_int) 
		{
			sprintf(acErrMsg, "冻结登记薄中不存在该笔记录!");
			WRITEMSG
			goto ErrExit;
		}
		/*** 冻结已解除 ***/
		if (f_mo_hold.hold_sts[0] == '1') 
		{
			sprintf(acErrMsg, "该账户已解除冻结!");
			WRITEMSG
			Mo_hold_Clo_Upd();
		}
		/*** 完全冻结、未解除、非完全解冻 ***/
		f_mo_hold.hold_sts[0] = '1';    /* 改为解除状态 */
		strcpy(f_mo_hold.unhold_br_no, g_pub_tx.opn_br_no);
		strcpy(f_mo_hold.unhold_tel, g_pub_tx.tel);
		strcpy(f_mo_hold.unhold_brf, "承兑汇票到期手工划款");

		/* 更新冻结登记薄 */
		g_reply_int = Mo_hold_Upd_Upd(f_mo_hold, g_pub_tx.reply);
		if (g_reply_int)
		{
			sprintf(acErrMsg, "更新冻结登记薄失败!");
			WRITEMSG
			goto ErrExit;
		}
	}
	Mo_hold_Clo_Upd();

	/*** 修改主文件 ***/
	switch (ac_type) 
	{
	case '1':
		if (sDdMst.hold_sts[0] == '0') 
		{
			sprintf(acErrMsg, "该账户已处于正常状态!");
			WRITEMSG
			goto OkExit;
		}
		sDdMst.hold_sts[0] = '0';
		sDdMst.hold_amt = 0.00;

		break;
	default:
		sprintf(acErrMsg, "该账户不属于解冻账户范围!!");
		WRITEMSG
		goto ErrExit;
		break;
	}    /* switch结束 */
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "冻结解冻操作成功![%s]", g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg, "冻结解冻操作失败![%s]", g_pub_tx.reply);
	if (memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		vtcp_log("[%s][%d] 返回错误但没有设置reply值 ,所以这里设置错误值",__FILE__,__LINE__);
		memcpy(g_pub_tx.reply,"B195",4);
		vtcp_log("[%s][%d] 返回错误但没有设置reply值 ,所以这里设置错误值",__FILE__,__LINE__);
	}
	WRITEMSG
	return 1;

}
int igD090_DisFrz(char ac_type)
{
	int    g_reply_int = 0;
	struct mo_hold_c f_mo_hold;
	memset(&f_mo_hold,0x00,sizeof(f_mo_hold));

	/* 更新冻结登记薄 */
	g_reply_int = Mo_hold_Dec_Upd(g_pub_tx.reply, \
		"ac_id=%ld and ac_seqn=%d and hold_sts='0' ", \
		sMdm_ac_rel.ac_id, sMdm_ac_rel.ac_seqn);
	if (g_reply_int) 
	{
		sprintf(acErrMsg, "查询冻结登记薄异常!");
		WRITEMSG
		goto ErrExit;
	}
	while (1) 
	{
		memset(&f_mo_hold, 0, sizeof(f_mo_hold));
		g_reply_int = Mo_hold_Fet_Upd(&f_mo_hold, g_pub_tx.reply);

		/** 旧数的冻结有记录, 新开承兑的冻结只改了主表状态 **/
		if (g_reply_int == 100) 
		{
			sprintf(acErrMsg, "已全部解冻!");
			WRITEMSG
			break;
		} else if (g_reply_int) 
		{
			sprintf(acErrMsg, "冻结登记薄中不存在该笔记录!");
			WRITEMSG
			goto ErrExit;
		}
		/*** 冻结已解除 ***/
		if (f_mo_hold.hold_sts[0] == '1') 
		{
			sprintf(acErrMsg, "该账户已解除冻结!");
			WRITEMSG
			strcpy(g_pub_tx.reply, "D225");
			goto ErrExit;
		}
		/*** 完全冻结、未解除、非完全解冻 ***/
		f_mo_hold.hold_sts[0] = '1';    /* 改为解除状态 */
		strcpy(f_mo_hold.unhold_br_no, g_pub_tx.opn_br_no);
		strcpy(f_mo_hold.unhold_tel, "9999");
		strcpy(f_mo_hold.unhold_brf, "承兑汇票到期自动划款");

		/* 更新冻结登记薄 */
		g_reply_int = Mo_hold_Upd_Upd(f_mo_hold, g_pub_tx.reply);
		if (g_reply_int) 
		{
			sprintf(acErrMsg, "更新冻结登记薄失败!");
			WRITEMSG
			goto ErrExit;
		}
	}
	Mo_hold_Clo_Upd();

	/*** 修改主文件 ***/
	switch (ac_type) 
	{
	case '1':
		if (sDdMst.hold_sts[0] == '0') 
		{
			sprintf(acErrMsg, "该账户已处于正常状态!");
			WRITEMSG
			goto OkExit;
		}
		sDdMst.hold_sts[0] = '0';
		sDdMst.hold_amt = 0.00;

		break;
	default:
		sprintf(acErrMsg, "该账户不属于解冻账户范围!!");
		WRITEMSG
		goto ErrExit;
		break;
	}    /* switch结束 */
OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "冻结解冻操作成功![%s]", g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg, "冻结解冻操作失败![%s]", g_pub_tx.reply);
	if (memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		vtcp_log("[%s][%d] 返回错误但没有设置reply值 ,所以这里设置错误值",__FILE__,__LINE__);
		memcpy(g_pub_tx.reply,"B195",4);
	}
	WRITEMSG
	return 1;
}
