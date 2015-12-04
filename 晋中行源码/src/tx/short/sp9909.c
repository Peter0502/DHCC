/*************************************************
* 文 件 名:  sp9909.c
* 功能描述： 由承兑协议编号回显承兑汇票基本信息
*
* 作    者:  jane
* 完成日期： 2003年5月14日
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "com_parm_c.h"
#include "com_branch_c.h"
#include "hv_uniontab_c.h"
#include "mo_bank_po_fee_c.h"

int sp9909()
{
	struct com_parm_c wd_com_parm;	/* 银行承兑汇票基本信息登记薄 */
	struct mo_bank_acc_po_c sMoBankAccPo;	/* 银行承兑汇票基本信息登记薄 */
	struct mo_bank_po_c sMoBankPo;	/* 承兑汇票登记簿 */
	struct mo_sg_cash_po_c sMoSgCashPo;	/* 保兑仓登记簿 */
	struct com_branch_c sComBranch;
	struct hv_uniontab_c sHv_uniontab;	
	struct mo_bank_po_fee_c sMo_bank_po_fee;	
	/*add by yl 黄河 20090930*/
	struct dd_mst_c sDdmst;		/* 活期主表 */
	struct mdm_ac_rel_c sMdmAcRel;	/* 账户介质对照表 */
	double s_amt;
	/*add end*/
	double          dPoAmt = 0.0;	/* 已签发票据总金额 */
	double          dPickAmt = 0.0;	/* 可开提货单总金额 */
	int             ret;
	int             parm_ind = 0;
	char            sOrno_brno[13];	
	char            cTx_code[8];/** add by chenchao 2011-8-7 15:19:19 **/
	memset(&sMoBankAccPo, 0x00, sizeof(struct mo_bank_acc_po_c));
	memset(&sMoBankPo, 0x00, sizeof(struct mo_bank_po_c));
	memset(&sMoSgCashPo, 0x00, sizeof(struct mo_sg_cash_po_c));
	memset(&wd_com_parm, 0x00, sizeof(struct com_parm_c));
	memset(&sDdmst, 0x00, sizeof(struct dd_mst_c));
	memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sComBranch, 0x00, sizeof(struct com_branch_c));
	memset(&sHv_uniontab,0,sizeof(struct hv_uniontab_c));
	memset(cTx_code,0x00,sizeof(cTx_code));
	memset(&sMo_bank_po_fee,0,sizeof(struct mo_bank_po_fee_c));
	get_zd_data("0330", sMoBankAccPo.pact_no);	/* 承兑协议编号 */
	get_zd_data("0650",cTx_code);			/** 区分交易 **/
	pub_base_sysinit();
	sprintf(acErrMsg, "pact_no=[%s]", sMoBankAccPo.pact_no);
	WRITEMSG

	/*Add By Yl 20091209*
	get_zd_data("0030", sComBranch.br_no);	
	vtcp_log("[%s][%d]sComBranch.br_no==[%s]",__FILE__,__LINE__,sComBranch.br_no);
	ret = Com_branch_Sel(g_pub_tx.reply, &sComBranch, "br_no='%s'", sComBranch.br_no);
	if (ret == 100) {
                sprintf(acErrMsg, "无此机构号!");
                WRITEMSG
                        sprintf(g_pub_tx.reply, "O116");
                goto ErrExit;
        } else if (ret) {
                sprintf(acErrMsg, "执行Com_branch_Sel错![%d]", ret);
                WRITEMSG
                goto ErrExit;
        }
	set_zd_data("0961", sComBranch.br_name);
	vtcp_log("[%s][%d]sComBranch.br_name==[%s]",__FILE__,__LINE__,sComBranch.br_name);
	*End By Yl 20091209*/

	/* 根据承兑协议编号查找银行承兑汇票基本信息登记薄 */
	ret = Mo_bank_acc_po_Sel(g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s' ", sMoBankAccPo.pact_no);
	if (ret == 100) {
		sprintf(acErrMsg, "无此承兑协议编号!");
		WRITEMSG
			sprintf(g_pub_tx.reply, "D104");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "执行Mo_bank_acc_po_Sel错![%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	/***
		if( sMoBankAccPo.acc_po_sts[0] == '2'&& sMoBankAccPo.acc_po_sts[0] == '3' )
		{
			sprintf( acErrMsg, "此承兑协议已做签发!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B188" );
			goto ErrExit;
		}else if( sMoBankAccPo.acc_po_sts[0] == 'A' )
		{
			sprintf( acErrMsg, "此承兑未做录入!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B274" );
			goto ErrExit;
		}else if( sMoBankAccPo.acc_po_sts[0] == '0' )
		{
			sprintf( acErrMsg, "此承兑未做审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B275" );
			goto ErrExit;
		}
	***/
	/* 根据承兑协议编号查询已签发票据 */
	ret = Mo_bank_po_Dec_Sel(g_pub_tx.reply, "pact_no='%s' and po_sts!='3'", sMoBankAccPo.pact_no);
	if (ret) {
		sprintf(acErrMsg, "执行Mo_bank_po_Dec_Sel错![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dPoAmt = 0.00;
	while (1) {
		ret = Mo_bank_po_Fet_Sel(&sMoBankPo, g_pub_tx.reply);
		if (ret == 100) {
			break;
		} else if (ret) {
			sprintf(acErrMsg, "执行Mo_bank_po_Fet_Sel错![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		dPoAmt += sMoBankPo.po_amt;
	vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,sMoBankPo.sign_br_no);
	}

	Mo_bank_po_Clo_Sel();

	/*add by yl 20091003 保证金余额
	if( (sMoBankAccPo.acc_type[0]=='1' || sMoBankAccPo.acc_type[0]=='3') &&  )
	{*保证金承兑*
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", sMoBankAccPo.bail_ac_no );
		if (ret)
		{
			sprintf(acErrMsg, "执行Mdm_ac_rel_Sel错![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdmst, "ac_id = %d ", sMdmAcRel.ac_id );
		if (ret)
		{
			sprintf(acErrMsg, "执行Dd_mst_Sel错![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d]sDdmst.bal==[%f]",__FILE__,__LINE__,sDdmst.bal);
		s_amt = 0.00;
		ret = sql_sum_double("mo_bank_po", "po_amt", &s_amt, "pact_no='%s' and po_sts!='3'", sMoBankAccPo.pact_no);
		s_amt = sDdmst.bal - s_amt;
		vtcp_log("[%s][%d]s_amt==[%f]",__FILE__,__LINE__,s_amt);
		set_zd_double( "0430", s_amt );
	}
	add end */

	/*
	 * 如果承兑类型为保兑仓 if( sMoBankAccPo.acc_type[0] == '4' ) {
	 * 根据承兑协议编号查找保兑仓登记薄 * ret = Mo_sg_cash_po_Sel(
	 * g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMoBankAccPo.pact_no
	 * ); if( ret == 100 ) {
	 *
	 * sprintf( acErrMsg, "无此承兑协议编号!" ); WRITEMSG sprintf(
	 * g_pub_tx.reply, "B187" ); goto ErrExit;
	 *
	**memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );***
	 * } else if( ret ) { sprintf( acErrMsg,
	 * "执行Mo_sg_cash_po_Sel错![%d]", ret ); WRITEMSG goto ErrExit; }
	 *
	 * dPickAmt = sMoBankAccPo.bail_amt - sMoSgCashPo.pickup_sum; vtcp_log(
	 * "dPickAmt[%lf]", dPickAmt ); }	*
	 */


	if (sMoBankAccPo.acc_po_sts[0] == '9')	/** 外行汇票 **/
		parm_ind = 6;
	else {
		switch (sMoBankAccPo.acc_type[0]) {
		case '1':	/** 差额承兑 **/
			parm_ind = 1;
			break;
		case '2':	/** 质压存单 **/
			parm_ind = 2;
			break;
		case '3':	/** 100%保证金 **/
			parm_ind = 3;
			break;
		case '4':	/** 担保承兑 **/
			parm_ind = 4;
			break;
		case '5':	/** 大票换小票 **/
			parm_ind = 5;
			break;
		default:
			sprintf(acErrMsg, "[%s][%d]汇票类型错误![%s]", __FILE__, __LINE__, sMoBankAccPo.acc_type);
			WRITEMSG
			goto ErrExit;

		}
	}
	/** 从参数表com_parm取得不同类型承兑汇票的科目 **/
	ret = Com_parm_Sel(g_pub_tx.reply, &wd_com_parm, "parm_code='CDHPR' and parm_seqn=%d", parm_ind);
	if (ret) {
		sprintf(acErrMsg, "[%s][%d]查询参数表错误![%d]", __FILE__, __LINE__, ret);
		WRITEMSG
		goto ErrExit;
	}
	memset(sOrno_brno,0,sizeof(sOrno_brno));
	vtcp_log("%s,%d g_pub_tx.tx_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("%s,%d sOrno_brno=[%s]\n",__FILE__,__LINE__,sOrno_brno);
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,sOrno_brno);
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab,"or_br_no='%s'",sOrno_brno);
	if(ret)
	{
		vtcp_log("%s,%d,查询行名行号错误!",__FILE__,__LINE__);
		goto ErrExit;
	}
	/** add by chenchao 20110728 增加编号的取值回显和手续费金额 begin **/
	if(memcmp(cTx_code,"4333",4) == 0)
	{
		ret = Mo_bank_po_fee_Dec_Sel(g_pub_tx.reply,"pact_no='%s' and sts='X' order by num ",sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( acErrMsg, "定义游标出错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D101" );
			goto ErrExit;
		}
		ret = Mo_bank_po_fee_Fet_Sel(&sMo_bank_po_fee,g_pub_tx.reply);
		if(ret == 100)
		{
			sprintf( acErrMsg, "该协议信贷未做审核!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B275" );
			goto ErrExit;
		}else if(ret)
		{
			sprintf( acErrMsg, "查询承兑收费出错!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
		Mo_bank_po_fee_Clo_Sel();
		set_zd_double("1004",sMo_bank_po_fee.fee_amt); /** 收费金额 */
		set_zd_long("0500",sMo_bank_po_fee.num); /** 借据序号 **/
		set_zd_double("1131",sMo_bank_po_fee.prm_amt); /** 承诺金额 **/
		set_zd_double("0430",sMo_bank_po_fee.po_amt); /** 票面金额 **/
	}
	/** end add by chenchao  20110728 收费 ***/
	vtcp_log("%s,%d sOrno_brno=[%s]\n",__FILE__,__LINE__,sOrno_brno);
	/* 向前台屏幕赋值 */
	set_zd_data("0640", wd_com_parm.val);	/* 承兑汇票记帐科目 */
	set_zd_data("0260", sMoBankAccPo.payer_name);	/* 付款人全称 */
	set_zd_data("0380", sMoBankAccPo.payer_ac_no);	/* 付款人帐号 */
	set_zd_data("0300", sMoBankAccPo.payer_br_no);	/* 付款人开户行行号 */
	set_zd_data("0250", sMoBankAccPo.payer_br_name);	/* 付款人开户行行名 */
	set_zd_data("0270", sMoBankAccPo.payee_name);	/* 收款人全称 */
	set_zd_data("0670", sMoBankAccPo.acc_type);	/* 承兑类型 */
	set_zd_data("0830", sMoBankAccPo.payee_ac_no);	/* 收款人账号 */
	set_zd_double("0840", sMoBankAccPo.over_rate);   /*罚息利率*/
	set_zd_data("0810", sMoBankAccPo.payee_br_name);	/* 收款人开户行行名 */
	set_zd_double("1001", sMoBankAccPo.sign_amt);	/* 签发票面金额 */
	set_zd_long("0440", sMoBankAccPo.matr_date);	/* 到期日期 */
	set_zd_long("0460", sMoBankAccPo.req_date);	/* 到期日期 */
	set_zd_double("1003", sMoBankAccPo.bail_amt);	/* 保证金金额 */
	set_zd_double("1002", sMoBankAccPo.tot_mort_amt);	/* 质押总金额 */
	set_zd_data("0320", sMoBankAccPo.bail_ac_no);	/* 保证金帐号 */
	set_zd_data("0700", sMoBankAccPo.acc_po_sts);	/* 协议状态 */
	set_zd_data("0710", sMoBankAccPo.acc_po_ind);	/* 到期处理标志 */
	set_zd_double("0400", sMoBankAccPo.bail_amt_ratio);	/* 保证金比例 */
	set_zd_double("0410", dPoAmt);	/* 已签发票据总金额 */
	set_zd_double("0390", sMoSgCashPo.pickup_sum);	/* 已开提货单总金额 */
	set_zd_double("0420", dPickAmt);	/* 可开提货单总金额 */
	/*Mod By Yl 20091207 黄河银行审批至网点,但签发却在营业厅,所以不能用
	审批机构作为签票机构*/
	/**set_zd_data("0910", sMoBankAccPo.tx_br_no);	 出票行号 */
	set_zd_data("0910", sMoBankPo.sign_br_no);	 /**出票行号 */
	vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,sMoBankPo.sign_br_no);
	set_zd_data("0961",sHv_uniontab.addr);
	set_zd_data("0962",sOrno_brno);

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
