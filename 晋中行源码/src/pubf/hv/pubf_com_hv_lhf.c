#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "prdt_ac_id_c.h"
#include "dd_mst_c.h"
#include "mdm_ac_rel_c.h"
#include "com_item_c.h"
#include "com_branch_c.h"
#include "com_tel_c.h"
#include "hv_poinfo_c.h"
/** 函数声明 */
int FetchRecToPayin(HV_PAYIN_AREA *hv_payin);
void vGet_Hv_Qs_no(char * qs_no);
extern double dGetHvMinTxamt( );
extern char * zip_space(char * );
/* 函数定义 */
/*********************************************************
*得到总行的清算行号
*函数名称:vGet_Hv_Qs_no
*参    数:char * qs_no
*参数描述:总行的清算行号
*作    者:LiuHuafeng 2006-9-5 21:26
*
**********************************************************/
void vGet_Hv_Qs_no(char * qs_no)
{
	char cHv_qs_no[13];
	memset(cHv_qs_no, 0 , sizeof(cHv_qs_no));
	strncpy(cHv_qs_no,(char *)getenv("MBFE_QS_BR_NO"),12);
	if (strlen(cHv_qs_no)!=12)
	{
		memcpy(cHv_qs_no,HV_QS_BR_NO,sizeof(cHv_qs_no)-1);
	}
	memcpy(qs_no,cHv_qs_no,sizeof(cHv_qs_no)-1);
	return;
}
/*********************************************************
*大额输入与表中数据比较函数
*函数名称:iHvCheckInput
*参    数:HV_PAYIN_AREA *hv_payin,char * errmsg
*参数描述:大额交易通用结构体(输入)，错误信息返回(输出)
*
*
**********************************************************/
int iHvCheckInput(HV_PAYIN_AREA *hv_payin,char * errmsg)
{
	int ret=0;
	double dValue1,dValue2;
	dValue1=0.0;
	dValue2=0.0;
	iHvZip_hvpayinF(hv_payin);
	if (memcmp(hv_payin->T_tx_br_no,hv_payin->F_tx_br_no,sizeof(hv_payin->T_tx_br_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tx_br_no,hv_payin->T_tx_br_no);
		strncpy(errmsg,"输入的交易行行号与原交易行号不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_tx_date ,hv_payin->F_tx_date ,sizeof(hv_payin->T_tx_date)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tx_date ,hv_payin->T_tx_date);
		strncpy(errmsg,"输入的交易日期与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_wt_date ,hv_payin->F_wt_date ,sizeof(hv_payin->T_wt_date)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_wt_date ,hv_payin->T_wt_date);
		strncpy(errmsg,"输入的委托日期与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cur_no,hv_payin->F_cur_no,sizeof(hv_payin->T_cur_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cur_no,hv_payin->T_cur_no);
		strncpy(errmsg,"输入的货币符号与原来不符",50);
		return -1;
	}
	pub_base_full_anti_zero(hv_payin->F_note_type,sizeof(hv_payin->F_note_type));
	pub_base_full_anti_zero(hv_payin->F_note_type,sizeof(hv_payin->F_note_type));
	if (memcmp(hv_payin->T_note_type,hv_payin->F_note_type,sizeof(hv_payin->T_note_type)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_note_type,hv_payin->T_note_type);
		strncpy(errmsg,"输入的凭证类型与原来不符",50);
		return -1;
	}
	pub_base_full_anti_zero(hv_payin->F_note_no,sizeof(hv_payin->F_note_no));
	if (memcmp(hv_payin->T_note_no ,hv_payin->F_note_no ,sizeof(hv_payin->T_note_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_note_no ,hv_payin->T_note_no);
		strncpy(errmsg,"输入的凭证号与原来不符",50);
		return -1;
	}
	dot_str2dbl(hv_payin->T_tx_amt,sizeof(hv_payin->T_tx_amt),&dValue1);
	dot_str2dbl(hv_payin->F_tx_amt,sizeof(hv_payin->F_tx_amt),&dValue2);
	if (pub_base_CompDblVal(dValue1,dValue2))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tx_amt,hv_payin->T_tx_amt);
		vtcp_log("[%s][%d] [%f]diff [%f]",__FILE__,__LINE__,dValue1,dValue2);
		strncpy(errmsg,"输入的交易金额与原来不符",50);
		return -1;
	}
	/****
	if(memcmp(hv_payin->T_tx_chrg_ind  ,hv_payin->F_tx_chrg_ind  ,sizeof(hv_payin->T_tx_chrg_ind)))
	{
	vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tx_chrg_ind  ,hv_payin->T_tx_chrg_ind  );
	strncpy(errmsg,"输入的手续费标志与原来不符",50);
	return -1;
	}
	*******/
	if (memcmp(hv_payin->T_pay_opn_br_no,hv_payin->F_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no )))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_pay_opn_br_no,hv_payin->T_pay_opn_br_no);
		strncpy(errmsg,"输入的付款人开户行行号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_ac_no,hv_payin->F_pay_ac_no,sizeof(hv_payin->T_pay_ac_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_pay_ac_no,hv_payin->T_pay_ac_no);
		strncpy(errmsg,"输入的付款人帐号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_name,hv_payin->F_pay_name,sizeof(hv_payin->T_pay_name)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_pay_name,hv_payin->T_pay_name);
		strncpy(errmsg,"输入的付款人/原收款人户名与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_addr,hv_payin->F_pay_addr,sizeof(hv_payin->T_pay_addr)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_pay_addr,hv_payin->T_pay_addr);
		strncpy(errmsg,"输入的付款人地址与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_ac_br_no,hv_payin->F_ac_br_no,sizeof(hv_payin->T_ac_br_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_ac_br_no,hv_payin->T_ac_br_no);
		strncpy(errmsg,"输入的接收行/原发起行与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_opn_br_no,hv_payin->F_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cash_opn_br_no,hv_payin->T_cash_opn_br_no);
		strncpy(errmsg,"输入的收款人开户行行号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_ac_no,hv_payin->F_cash_ac_no,sizeof(hv_payin->T_cash_ac_no)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cash_ac_no,hv_payin->T_cash_ac_no);
		strncpy(errmsg,"输入的收款人帐号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_name,hv_payin->F_cash_name,sizeof(hv_payin->T_cash_name)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cash_name,hv_payin->T_cash_name);
		strncpy(errmsg,"输入的收款人/原付款人户名与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_addr,hv_payin->F_cash_addr,sizeof(hv_payin->T_cash_addr)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cash_addr,hv_payin->T_cash_addr);
		strncpy(errmsg,"输入的收款人地址与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_yw_type ,hv_payin->F_yw_type ,sizeof(hv_payin->T_yw_type)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_yw_type ,hv_payin->T_yw_type);
		strncpy(errmsg,"输入的业务种类与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_cmtno,hv_payin->F_cmtno,sizeof(hv_payin->T_cmtno)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_cmtno,hv_payin->T_cmtno);
		strncpy(errmsg,"输入的CMT编号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_operlevel,hv_payin->F_operlevel,sizeof(hv_payin->T_operlevel)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_operlevel,hv_payin->T_operlevel);
		strncpy(errmsg,"输入的优先级别与原来不符",50);
		return -1;
	}
	if ((char*)getenv("CHECKTEL")!=NULL)
	{
		if (memcmp((char*)getenv("CHECKTEL"),"NO_CHECK",8))
		if (!memcmp(hv_payin->T_tel,hv_payin->F_tel,sizeof(hv_payin->T_tel)) && strcmp(getenv("FHTEST"),"LVTEST")!=0)
		{
			vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tel,hv_payin->T_tel);
			strncpy(errmsg,"复核与录入操作员不可以是同一操作员",50);
			return -1;
		}
	}
	else
	{
		if (!memcmp(hv_payin->T_tel,hv_payin->F_tel,sizeof(hv_payin->T_tel)))
		{
			vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_tel,hv_payin->T_tel);
			strncpy(errmsg,"复核与录入操作员不可以是同一操作员",50);
			return -1;
		}
	}
	if (memcmp(hv_payin->T_o_orderno,hv_payin->F_o_orderno,sizeof(hv_payin->T_o_orderno)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_o_orderno,hv_payin->T_o_orderno);
		strncpy(errmsg,"输入的原支付交易序号与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_o_tx_date,hv_payin->F_o_tx_date,sizeof(hv_payin->T_o_tx_date)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_o_tx_date,hv_payin->T_o_tx_date);
		strncpy(errmsg,"输入的原支付交易日期与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_o_cmtno ,hv_payin->F_o_cmtno ,sizeof(hv_payin->T_o_cmtno)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_o_cmtno ,hv_payin->T_o_cmtno);
		strncpy(errmsg,"输入的原cmtno与原来不符",50);
		return -1;
	}
	if (memcmp(hv_payin->T_o_wt_date,hv_payin->F_o_wt_date,sizeof(hv_payin->T_o_wt_date)))
	{
		vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_o_wt_date,hv_payin->T_o_wt_date);
		strncpy(errmsg,"输入的原委托日期与原来不符",50);
		return -1;
	}
	if ((char*)getenv("CHECKTEL")!=NULL)
	{
		if (memcmp((char*)getenv("CHECKTEL"),"NO_CHECK",8))
		if (memcmp(hv_payin->T_hv_brf,hv_payin->F_hv_brf,sizeof(hv_payin->T_hv_brf)))
		{
			vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_hv_brf,hv_payin->T_hv_brf);
			strncpy(errmsg,"输入的备注与原来不符",50);
			return -1;
		}
	}
	else
	{
		if (memcmp(hv_payin->T_hv_brf,hv_payin->F_hv_brf,sizeof(hv_payin->T_hv_brf)))
		{
			vtcp_log("[%s][%d] [%s]diff [%s]",__FILE__,__LINE__,hv_payin->F_hv_brf,hv_payin->T_hv_brf);
			strncpy(errmsg,"输入的备注与原来不符",50);
			return -1;
		}
	}
	if (memcmp(hv_payin->F_cmtno,HV_HD,sizeof(hv_payin->F_cmtno))!=0
		&&memcmp(hv_payin->F_cmtno,HV_GKZJHH,sizeof(hv_payin->F_cmtno))!=0 
		&&memcmp(hv_payin->F_cmtno,HV_TH,sizeof(hv_payin->F_cmtno))!=0  )
	{
		FILE * fpr;
		char cBuff1[2000];
		char cBuff2[2000];
		char cFilename[200];
		memset(cFilename , 0 , sizeof(cFilename));
		memset(cBuff2, 0 , sizeof(cBuff2));
		memset(cBuff1, 0 , sizeof(cBuff1));

		vtcp_log("有附加域cmtno=[%s]",hv_payin->T_cmtno);TRACE
			Gl_file_first=0;
		pub_base_AllDwnFilName( cFilename);
		fpr=fopen(cFilename,"rb");
		if (fpr==NULL)
		{
			vtcp_log("%s,%d 得到附加域文件错误%s",__FILE__,__LINE__,cFilename);TRACE
				strncpy(errmsg,"得到附加域文件错误",50);
			WRITEMSG
				return -1;
		}
		fgets(cBuff1,sizeof(cBuff1),fpr);
		ret=iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",cBuff2,sizeof(cBuff1));
		if (ret)
		{
			strncpy(errmsg,"读取附加域错误",50);
			vtcp_log("读取附加域错误");TRACE
				WRITEMSG
				return -1;
		}
		ret=iHvCheckInputAddit(hv_payin->F_cmtno,cBuff2,cBuff1,errmsg);
		if (ret)
		{
			WRITEMSG
				return -1;
		}
	}
	return 0;
}

int iHvCheckInputAddit(char * cmtno,char * newbuff,char * oldbuff,char * errmsg)
{
	vtcp_log("%s,%d newbuf=[%s],oldbuf=[%s]",__FILE__,__LINE__,newbuff,oldbuff);
	if (!memcmp(cmtno,HV_WT,3))
	{
		HV_WTSK_ADD hv_wtsk1,hv_wtsk2;
		memset(&hv_wtsk1, 0 , sizeof(hv_wtsk1));
		memset(&hv_wtsk2, 0 , sizeof(hv_wtsk2));
		memcpy((char*)&hv_wtsk1,newbuff,sizeof(hv_wtsk1));
		memcpy((char*)&hv_wtsk2,oldbuff,sizeof(hv_wtsk1));
		if (memcmp(hv_wtsk1.pj_date,hv_wtsk2.pj_date,sizeof(hv_wtsk1.pj_date)))
		{
			strncpy(errmsg,"委托收款的票据日期输入与原交易不同",50);
			vtcp_log("[%s][%d]票据日期%.8s输入与原交易%.8s不同",__FILE__,__LINE__,hv_wtsk1.pj_date,hv_wtsk2.pj_date);
			return -1;
		}
		if (memcmp(hv_wtsk1.pj_num,hv_wtsk2.pj_num,sizeof(hv_wtsk1.pj_num)))
		{
			strncpy(errmsg,"委托收款的票据号码输入与原交易不同",50);
			vtcp_log("[%s][%d]票据号码%.8s输入与原交易%.8s不同",__FILE__,__LINE__,hv_wtsk1.pj_num,hv_wtsk2.pj_num);
			return -1;
		}
	}
	/***托收承付********/
	if (!memcmp(cmtno,HV_TSCF,3))
	{
		HV_TSCF_ADD hv_tscf1,hv_tscf2;
		memset(&hv_tscf1, 0 , sizeof(hv_tscf1));
		memset(&hv_tscf2, 0 , sizeof(hv_tscf2));
		memcpy((char*)&hv_tscf1,newbuff,sizeof(hv_tscf1));
		memcpy((char*)&hv_tscf2,oldbuff,sizeof(hv_tscf1));
		if (memcmp(hv_tscf1.pj_date,hv_tscf2.pj_date,sizeof(hv_tscf1.pj_date)))
		{
			strncpy(errmsg,"托收承付的票据日期输入与原交易不同",50);
			vtcp_log("[%s][%d]票据日期%.8s输入与原交易%.8s不同",__FILE__,__LINE__,hv_tscf1.pj_date,hv_tscf2.pj_date);
			return -1;
		}
		if (memcmp(hv_tscf1.pj_num,hv_tscf2.pj_num,sizeof(hv_tscf1.pj_num)))
		{
			strncpy(errmsg,"托收承付的票据号码输入与原交易不同",50);
			vtcp_log("[%s][%d]票据号码%.8s输入与原交易%.8s不同",__FILE__,__LINE__,hv_tscf1.pj_num,hv_tscf2.pj_num);
			return -1;
		}
		if (memcmp(hv_tscf1.ptxamt,hv_tscf2.ptxamt,sizeof(hv_tscf1.ptxamt)))
		{
			strncpy(errmsg,"托收承付的票据金额输入与原交易不同",50);
			vtcp_log("[%s][%d]票据金额%.16s输入与原交易%.16s不同",__FILE__,__LINE__,hv_tscf1.ptxamt,hv_tscf2.ptxamt);
			return -1;
		}
		if (memcmp(hv_tscf1.jtxamt,hv_tscf2.jtxamt,sizeof(hv_tscf1.jtxamt)))
		{
			strncpy(errmsg,"托收承付的拒付金额输入与原交易不同",50);
			vtcp_log("[%s][%d]拒付金额%.16s输入与原交易%.16s不同",__FILE__,__LINE__,hv_tscf1.jtxamt,hv_tscf2.jtxamt);
			return -1;
		}
	}
	/****银行同业拆借*****/
	if (!memcmp(cmtno,HV_CJ,3))
	{
		HV_TYCJ_ADD hv_cj1,hv_cj2;
		memset(&hv_cj1, 0 , sizeof(hv_cj1));
		memset(&hv_cj2, 0 , sizeof(hv_cj2));
		memcpy((char*)&hv_cj1,newbuff,sizeof(hv_cj1));
		memcpy((char*)&hv_cj2,oldbuff,sizeof(hv_cj1));
		if (memcmp(hv_cj1.rate,hv_cj2.rate,sizeof(hv_cj1.rate)))
		{
			strncpy(errmsg,"拆借利率输入与原交易不同",50);
			vtcp_log("[%s][%d]拆借利率%.7s输入与原交易%.7s不同",__FILE__,__LINE__,hv_cj1.rate,hv_cj2.rate);
			return -1;
		}
		if (memcmp(hv_cj1.ratelimit,hv_cj2.ratelimit,sizeof(hv_cj1.ratelimit)))
		{
			strncpy(errmsg,"拆借期限输入与原交易不同",50);
			vtcp_log("[%s][%d]拆借期限%.5s输入与原交易%.5s不同",__FILE__,__LINE__,hv_cj1.ratelimit,hv_cj2.ratelimit);
			return -1;
		}
	}
	/****银行同业拆借*****/
	if (!memcmp(cmtno,HV_CJ,3))
	{
		HV_TYCJ_ADD hv_cj1,hv_cj2;
		memset(&hv_cj1, 0 , sizeof(hv_cj1));
		memset(&hv_cj2, 0 , sizeof(hv_cj2));
		memcpy((char*)&hv_cj1,newbuff,sizeof(hv_cj1));
		memcpy((char*)&hv_cj2,oldbuff,sizeof(hv_cj1));
		if (memcmp(hv_cj1.rate,hv_cj2.rate,sizeof(hv_cj1.rate)))
		{
			strncpy(errmsg,"拆借利率输入与原交易不同",50);
			vtcp_log("[%s][%d]拆借利率%.7s输入与原交易%.7s不同",__FILE__,__LINE__,hv_cj1.rate,hv_cj2.rate);
			return -1;
		}
		if (memcmp(hv_cj1.ratelimit,hv_cj2.ratelimit,sizeof(hv_cj1.ratelimit)))
		{
			strncpy(errmsg,"拆借期限输入与原交易不同",50);
			vtcp_log("[%s][%d]拆借期限%.5s输入与原交易%.5s不同",__FILE__,__LINE__,hv_cj1.ratelimit,hv_cj2.ratelimit);
			return -1;
		}
	}
}
int iHvZip_hvpayinF(HV_PAYIN_AREA *hv_payin)
{
	pub_base_full_anti_zero(hv_payin->F_crdb          ,sizeof(hv_payin->F_crdb          ));
	pub_base_full_anti_zero(hv_payin->F_opcd          ,sizeof(hv_payin->F_opcd          ));
	pub_base_full_anti_zero(hv_payin->F_lw_ind        ,sizeof(hv_payin->F_lw_ind        ));
	pub_base_full_anti_zero(hv_payin->F_tx_br_no      ,sizeof(hv_payin->F_tx_br_no      ));
	pub_base_full_anti_zero(hv_payin->F_tx_date       ,sizeof(hv_payin->F_tx_date       ));
	pub_base_full_anti_zero(hv_payin->F_wt_date       ,sizeof(hv_payin->F_wt_date       ));
	pub_base_full_anti_zero(hv_payin->F_orderno       ,sizeof(hv_payin->F_orderno       ));
	pub_base_full_anti_zero(hv_payin->F_cmtno         ,sizeof(hv_payin->F_cmtno         ));
	pub_base_full_anti_zero(hv_payin->F_cur_no        ,sizeof(hv_payin->F_cur_no        ));
	pub_base_full_anti_zero(hv_payin->F_note_type     ,sizeof(hv_payin->F_note_type     ));
	pub_base_full_anti_zero(hv_payin->F_note_no       ,sizeof(hv_payin->F_note_no       ));
	pub_base_full_anti_zero(hv_payin->F_tx_amt        ,sizeof(hv_payin->F_tx_amt        ));
	pub_base_full_anti_zero(hv_payin->F_tx_chrg_ind   ,sizeof(hv_payin->F_tx_chrg_ind   ));
	pub_base_full_anti_zero(hv_payin->F_tx_chrg_amt   ,sizeof(hv_payin->F_tx_chrg_amt   ));
	pub_base_full_anti_zero(hv_payin->F_or_br_no      ,sizeof(hv_payin->F_or_br_no      ));
	pub_base_full_anti_zero(hv_payin->F_ac_br_no      ,sizeof(hv_payin->F_ac_br_no      ));
	pub_base_full_anti_zero(hv_payin->F_pay_opn_br_no ,sizeof(hv_payin->F_pay_opn_br_no ));
	pub_base_full_anti_zero(hv_payin->F_pay_ac_no     ,sizeof(hv_payin->F_pay_ac_no     ));
	pub_base_full_anti_zero(hv_payin->F_pay_name      ,sizeof(hv_payin->F_pay_name      ));
	pub_base_full_anti_zero(hv_payin->F_pay_addr      ,sizeof(hv_payin->F_pay_addr      ));
	pub_base_full_anti_zero(hv_payin->F_userpswd      ,sizeof(hv_payin->F_userpswd      ));
	pub_base_full_anti_zero(hv_payin->F_cash_opn_br_no,sizeof(hv_payin->F_cash_opn_br_no));
	pub_base_full_anti_zero(hv_payin->F_cash_ac_no    ,sizeof(hv_payin->F_cash_ac_no    ));
	pub_base_full_anti_zero(hv_payin->F_cash_name     ,sizeof(hv_payin->F_cash_name     ));
	pub_base_full_anti_zero(hv_payin->F_cash_addr     ,sizeof(hv_payin->F_cash_addr     ));
	pub_base_full_anti_zero(hv_payin->F_yw_type       ,sizeof(hv_payin->F_yw_type       ));
	pub_base_full_anti_zero(hv_payin->F_brf           ,sizeof(hv_payin->F_brf           ));
	pub_base_full_anti_zero(hv_payin->F_operlevel     ,sizeof(hv_payin->F_operlevel     ));
	pub_base_full_anti_zero(hv_payin->F_tel           ,sizeof(hv_payin->F_tel           ));
	pub_base_full_anti_zero(hv_payin->F_chk           ,sizeof(hv_payin->F_chk           ));
	pub_base_full_anti_zero(hv_payin->F_auth_tel      ,sizeof(hv_payin->F_auth_tel      ));
	pub_base_full_anti_zero(hv_payin->F_auth_pwd      ,sizeof(hv_payin->F_auth_pwd      ));
	pub_base_full_anti_zero(hv_payin->F_refno         ,sizeof(hv_payin->F_refno         ));
	pub_base_full_anti_zero(hv_payin->F_o_tx_date     ,sizeof(hv_payin->F_o_tx_date     ));
	pub_base_full_anti_zero(hv_payin->F_o_cmtno       ,sizeof(hv_payin->F_o_cmtno       ));
	pub_base_full_anti_zero(hv_payin->F_hv_brf        ,sizeof(hv_payin->F_hv_brf        ));
	pub_base_full_anti_zero(hv_payin->F_o_wt_date     ,sizeof(hv_payin->F_o_wt_date     ));
	pub_base_full_anti_zero(hv_payin->F_o_orderno     ,sizeof(hv_payin->F_o_orderno     ));
	pub_base_full_anti_zero(hv_payin->F_beg_sts       ,sizeof(hv_payin->F_beg_sts       ));
	pub_base_full_anti_zero(hv_payin->F_hv_osts       ,sizeof(hv_payin->F_hv_osts       ));
	pub_base_full_anti_zero(hv_payin->F_resp1         ,sizeof(hv_payin->F_resp1         ));

	pub_base_full_anti_zero(hv_payin->T_tx_br_no      ,sizeof(hv_payin->T_tx_br_no      ));
	pub_base_full_anti_zero(hv_payin->T_tx_date       ,sizeof(hv_payin->T_tx_date       ));
	pub_base_full_anti_zero(hv_payin->T_wt_date       ,sizeof(hv_payin->T_wt_date       ));
	pub_base_full_anti_zero(hv_payin->T_orderno       ,sizeof(hv_payin->T_orderno       ));
	pub_base_full_anti_zero(hv_payin->T_cmtno         ,sizeof(hv_payin->T_cmtno         ));
	pub_base_full_anti_zero(hv_payin->T_cur_no        ,sizeof(hv_payin->T_cur_no        ));
	pub_base_full_anti_zero(hv_payin->T_note_type     ,sizeof(hv_payin->T_note_type     ));
	pub_base_full_anti_zero(hv_payin->T_note_no       ,sizeof(hv_payin->T_note_no       ));
	pub_base_full_anti_zero(hv_payin->T_tx_amt        ,sizeof(hv_payin->T_tx_amt        ));
	pub_base_full_anti_zero(hv_payin->T_or_br_no      ,sizeof(hv_payin->T_or_br_no      ));
	pub_base_full_anti_zero(hv_payin->T_ac_br_no      ,sizeof(hv_payin->T_ac_br_no      ));
	pub_base_full_anti_zero(hv_payin->T_pay_opn_br_no ,sizeof(hv_payin->T_pay_opn_br_no ));
	pub_base_full_anti_zero(hv_payin->T_pay_ac_no     ,sizeof(hv_payin->T_pay_ac_no     ));
	pub_base_full_anti_zero(hv_payin->T_pay_name      ,sizeof(hv_payin->T_pay_name      ));
	pub_base_full_anti_zero(hv_payin->T_pay_addr      ,sizeof(hv_payin->T_pay_addr      ));
	pub_base_full_anti_zero(hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));
	pub_base_full_anti_zero(hv_payin->T_cash_ac_no    ,sizeof(hv_payin->T_cash_ac_no    ));
	pub_base_full_anti_zero(hv_payin->T_cash_name     ,sizeof(hv_payin->T_cash_name     ));
	pub_base_full_anti_zero(hv_payin->T_cash_addr     ,sizeof(hv_payin->T_cash_addr     ));
	pub_base_full_anti_zero(hv_payin->T_yw_type       ,sizeof(hv_payin->T_yw_type       ));
	pub_base_full_anti_zero(hv_payin->T_hv_brf        ,sizeof(hv_payin->T_hv_brf        ));
	pub_base_full_anti_zero(hv_payin->T_operlevel     ,sizeof(hv_payin->T_operlevel     ));
	pub_base_full_anti_zero(hv_payin->T_tel           ,sizeof(hv_payin->T_tel           ));
	pub_base_full_anti_zero(hv_payin->T_chk           ,sizeof(hv_payin->T_chk           ));
	pub_base_full_anti_zero(hv_payin->T_auth_tel      ,sizeof(hv_payin->T_auth_tel      ));
	pub_base_full_anti_zero(hv_payin->T_refno         ,sizeof(hv_payin->T_refno         ));
	pub_base_full_anti_zero(hv_payin->T_o_tx_date     ,sizeof(hv_payin->T_o_tx_date     ));
	pub_base_full_anti_zero(hv_payin->T_o_cmtno       ,sizeof(hv_payin->T_o_cmtno       ));
	pub_base_full_anti_zero(hv_payin->T_hv_brf        ,sizeof(hv_payin->T_hv_brf        ));
	pub_base_full_anti_zero(hv_payin->T_o_wt_date     ,sizeof(hv_payin->T_o_wt_date     ));
	pub_base_full_anti_zero(hv_payin->T_o_orderno     ,sizeof(hv_payin->T_o_orderno     ));
	pub_base_full_anti_zero(hv_payin->T_beg_sts       ,sizeof(hv_payin->T_beg_sts       ));
	pub_base_full_anti_zero(hv_payin->T_hv_osts       ,sizeof(hv_payin->T_hv_osts       ));
	pub_base_full_anti_zero(hv_payin->T_resp1         ,sizeof(hv_payin->T_resp1         ));
}

int iHvCheckRecordStat(char * cmtno,char oldstat,char newstat,char lwbs,char * msg)
{
	vtcp_log("%s,%d oldstat[%c]newstat=[%c]",__FILE__,__LINE__,oldstat,newstat);
	if (lwbs=='1')
	{/* 往帐 */
		if (!memcmp(cmtno,HV_HD,3) || !memcmp(cmtno,HV_TSCF,3)
			||!memcmp(cmtno,HV_WT,3) ||!memcmp(cmtno,HV_CJ,3)
			||!memcmp(cmtno,HV_GKZJHH,3) ||!memcmp(cmtno,HV_TH,3))
		{
			if (newstat=='0')
			{
				if (oldstat!='C')
				{
					strncpy(msg,"原交易状态不是录入状态不可以复核",50);
					vtcp_log("%s,%d原交易状态不是录入状态[C]不可以复核",__FILE__,__LINE__);
					return -1;
				}
			}
			if (newstat=='1')
			{
				if (oldstat!='0')
				{
					strncpy(msg,"原交易状态不是复核状态不可以发送",50);
					vtcp_log("%s,%d原交易状态不是录入状态[0]不可以复核",__FILE__,__LINE__);
					return -1;
				}
			}
			if (newstat=='6')
			{
				if (oldstat!='1')
				{
					strncpy(msg,"原交易状态不是发送状态不可以撤销",50);
					vtcp_log("%s,%d原交易状态不是发送状态[1]不可以撤销",__FILE__,__LINE__);
					return -1;
				}
			}
		}
	}
	return 0;
}
/****************************************************************
*函数名称:iHvPayin_characterCheck                               *
*函数功能:检查支付类交易户名，地址等输入项目是否有非法字符存在  *
*参数描述: 输入：HV_PAYIN_AREA *hv_payin 大额支付类交易结构体   *
*          输出：char * errsmg           错误信息描述           *
*                                                               *
*                                                               *
*设计到表:                                                      *
*作    者:刘华锋 2006-8-26 16:20                                *
****************************************************************/
int iHvPayin_characterCheck(HV_PAYIN_AREA *hv_payin,char * msg)
{
	int iRet=0;
	vtcp_log("%s,%d payname=[%.60s],cashname=[%.60s]",__FILE__,__LINE__,
		hv_payin->F_pay_name,hv_payin->F_cash_name);
	iRet=iCharacterType(hv_payin->F_pay_name,sizeof(hv_payin->F_pay_name),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的付款人名字[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,hv_payin->F_pay_name,iRet);
		strncpy(msg,"输入的付款人名字中存在非法字符",50);
		/***return iRet; add by martin  20091222 根据lianghaoran指示 放开***/
	}
	iRet=iCharacterType(hv_payin->F_cash_name,sizeof(hv_payin->F_cash_name),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的收款人名字[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,hv_payin->F_cash_name,iRet);
		strncpy(msg,"输入的收款人名字中存在非法字符",50);
		/***return iRet;  lianghaoran, 20090709  ***/
	}
	iRet=iCharacterType(hv_payin->F_pay_addr,sizeof(hv_payin->F_pay_addr),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的付款人地址[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,hv_payin->F_pay_addr,iRet);
		strncpy(msg,"输入的付款人地址中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(hv_payin->F_cash_addr,sizeof(hv_payin->F_cash_addr),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的收款人地址[%.60s]中第%d位存在非法字符",__FILE__,__LINE__,hv_payin->F_cash_addr,iRet);
		strncpy(msg,"输入的收款人地址中存在非法字符",50);
		return iRet;
	}
	iRet=iCharacterType(hv_payin->F_hv_brf,sizeof(hv_payin->F_hv_brf),'g');
	if (iRet)
	{
		vtcp_log("%s,%d,输入的附言中[%.60s]第%d位存在非法字符",__FILE__,__LINE__,hv_payin->F_hv_brf,iRet);
		strncpy(msg,"输入的附言中存在非法字符",50);
		return iRet;
	}
	vtcp_log("%s,%d payname=[%.60s],cashname=[%.60s]",__FILE__,__LINE__,
		hv_payin->F_pay_name,hv_payin->F_cash_name);
	/* 检查大额支付起点金额 */
	if (hv_payin->F_operlevel[0]=='0' && 
		(!memcmp(hv_payin->F_cmtno,"100",sizeof(hv_payin->F_cmtno))
		||!memcmp(hv_payin->F_cmtno,"101",sizeof(hv_payin->F_cmtno))
		||!memcmp(hv_payin->F_cmtno,"102",sizeof(hv_payin->F_cmtno))
		||!memcmp(hv_payin->F_cmtno,"103",sizeof(hv_payin->F_cmtno))
		||!memcmp(hv_payin->F_cmtno,"105",sizeof(hv_payin->F_cmtno))
		||!memcmp(hv_payin->F_cmtno,"108",sizeof(hv_payin->F_cmtno))
		)
		)
	{
		char cInputTxamt[17];
		double dInputTxamt=0.0;
		double dMinTxamt=0.0;
		dMinTxamt=dGetHvMinTxamt();
		memset(cInputTxamt , 0 ,sizeof(cInputTxamt));
		memcpy(cInputTxamt, hv_payin->F_tx_amt,sizeof(cInputTxamt)-1);
		zip_space(cInputTxamt);
		str2dbl(cInputTxamt,strlen(cInputTxamt),0,&dInputTxamt);
		if (dInputTxamt < dMinTxamt)
		{
			vtcp_log("%s,%d 输入的金额=[%f], 最小金额=[%f] ",__FILE__,__LINE__,dInputTxamt , dMinTxamt);
			vtcp_log("%s,%d 请选择加急或特级优先级别",__FILE__,__LINE__);
			WRITEMSG
				strncpy(msg,"交易金额小于规定最小金额，优先级别错误",50);
			vtcp_log("%s,%d msg:%s",__FILE__,__LINE__,msg);
			return -1;
		}
	}
	return 0;
}

/****************************************************************
*函数名称:iHvGetThMsg()                                         *
*函数功能:根据往帐得到大额要支付退汇的原始来帐信息              *
*参数描述: 输入：HV_PAYIN_AREA *payin_wz,*payin_lz              *
*              大额支付结构体往帐,大额支付结构体来帐            *
*          输出：char * errsmg           错误信息描述           *
*                                                               *
*                                                               *
*设计到表:                                                      *
*作    者:刘华锋 2006-8-26 16:20                                *
****************************************************************/
int iHvGetThMsg(HV_PAYIN_AREA *payin_wz,HV_PAYIN_AREA *payin_lz,char * errmsg)
{
	int iRet=0;
	memcpy(payin_lz->F_orderno,payin_wz->F_o_orderno,sizeof(payin_lz->F_orderno));
	memcpy(payin_lz->F_ac_br_no,payin_wz->F_or_br_no,sizeof(payin_lz->F_ac_br_no));
	memcpy(payin_lz->F_or_br_no,payin_wz->F_ac_br_no,sizeof(payin_lz->F_ac_br_no));
	memcpy(payin_lz->F_wt_date, payin_wz->F_o_wt_date,sizeof(payin_lz->F_wt_date));
	memcpy(payin_lz->F_cmtno, payin_wz->F_o_cmtno,sizeof(payin_lz->F_cmtno));
	iRet=FetchRecToPayin(payin_lz);
	if (iRet)
	{
		memcpy(errmsg,acErrMsg,50);
		vtcp_log("%s,%d 根据输入退汇往帐信息查询来帐失败",__FILE__,__LINE__);
		return iRet;
	}
	if (memcmp(g_pub_tx.tx_br_no,payin_lz->T_tx_br_no,BRNO_LEN))
	{
		strncpy(errmsg,"非本机构来帐不可以做退汇",50);
		memcpy(acErrMsg,errmsg,50);
		vtcp_log("%s,%d 非本机构[%s]来帐不可以做退汇",__FILE__,__LINE__,g_pub_tx.tx_br_no);
		return -1;
	}
	if (payin_lz->T_hv_sts[0]=='0' || payin_lz->T_hv_sts[0]=='1' ||
		payin_lz->T_hv_sts[0]=='4' || payin_lz->T_hv_sts[0]=='5' ||
		payin_lz->T_hv_sts[0]=='6' || payin_lz->T_hv_sts[0]=='7' ||
		payin_lz->T_hv_sts[0]=='2' || payin_lz->T_hv_sts[0]=='3')
	{
		if (payin_lz->T_hv_sts[0]=='4')
		{
			memcpy(acErrMsg,"该笔业务已经退汇",50);
		}
		else
		{
			memcpy(acErrMsg,"该笔业务是往帐不可以做退汇",50);
		}
		memcpy(errmsg,acErrMsg,50);
		vtcp_log("%s,%d 该笔业务是往帐不可以做退汇[%c]",__FILE__,__LINE__,payin_lz->T_hv_sts[0]);
		return -1;
	}
	if (payin_lz->T_hv_sts[0]!='F' &&payin_lz->T_hv_sts[0]!='G' && 
		payin_lz->T_hv_sts[0]!='H' && payin_lz->T_hv_sts[0]!='I' && payin_lz->T_hv_sts[0]!='9')
	{
		memcpy(acErrMsg,"该笔业务不是正常来帐或挂帐状态不可以退回",50);
		memcpy(errmsg,acErrMsg,50);
		vtcp_log("%s,%d 该笔业务不是正常来帐或挂帐状态不可以退回",__FILE__,__LINE__);
		return -1;
	}
	/* 设置8583域中的帐务处理科目 */
	if (payin_lz->T_hv_sts[0]=='9')
	{
		char cAc_no[25];
		memset(cAc_no,0,sizeof(cAc_no));
		memcpy(cAc_no,payin_lz->T_cash_ac_no,sizeof(cAc_no)-1);
		set_zd_data("1021",cAc_no);
	}
	else if (payin_lz->T_hv_sts[0]=='F' &&payin_lz->T_hv_sts[0]=='G' && 
		payin_lz->T_hv_sts[0]=='H' && payin_lz->T_hv_sts[0]=='I' )
	{
		set_zd_data("1021","2620401");
		set_zd_data("0300","2620401");
	}
	if (memcmp(payin_wz->F_opcd,"30",2))
	{
		payin_wz->F_hv_osts[0]=payin_lz->T_hv_sts[0];
		payin_wz->T_hv_osts[0]=payin_lz->T_hv_sts[0];
	}
	else 
	{
		/***add by liuyue 20061218 保存退汇时候的原来状态***/
		vtcp_log("[%s][%d]lovexier,hv_osts=[%c],T_hv_sts=[%c]\n",__FILE__,__LINE__,payin_lz->F_hv_osts[0],payin_lz->T_hv_sts[0]);
		payin_lz->F_hv_osts[0] = payin_lz->T_hv_sts[0];
		payin_lz->T_hv_sts[0]=SEND_BACK_WAIT;
		vtcp_log("[%s][%d]lovexier,hv_osts=[%c],T_hv_sts=[%c]\n",__FILE__,__LINE__,payin_lz->F_hv_osts[0],payin_lz->T_hv_sts[0]);
		iHvZip_hvpayinF(payin_lz);
		iRet=iHvUpdZfRecFromPayin(payin_lz,0);
		if (iRet)
		{
			memcpy(errmsg,acErrMsg,50);
			vtcp_log("%s,%d 更新来帐状态失败",__FILE__,__LINE__);
			return iRet;
		}
	}
	vtcp_log("%s,%d ooooo sts ==[%c]",__FILE__,__LINE__,payin_wz->T_hv_osts[0]);
	iHvZip_hvpayinF(payin_lz);
	vtcp_log("%s,%d ooooo sts ==[%c]",__FILE__,__LINE__,payin_wz->T_hv_osts[0]);
	return 0;
}

/****************************************************************
*函数名称:iPrepareTh_ac_no()                                    *
*函数功能:准备退汇交易的帐号                                    *
*参数描述: 输入：HV_PAYIN_AREA *payin                           *
*              大额支付结构体退汇往帐                           *
*          输出：char * errsmg           错误信息描述           *
*                                                               *
*设计到表:                                                      *
*作    者:刘华锋 2006-8-26 16:20                                *
****************************************************************/
int iPrepareTh_ac_no(HV_PAYIN_AREA *payin)
{
	int iRet=0;
	struct  mdm_ac_rel_c    sMdm_ac_rel;
	struct  prdt_ac_id_c  sPrdt_ac_id;
	char cReply[5];
	char cAc_no[33];
	char cKinbr[BRNO_LEN+1];
	memset(cReply, 0 , sizeof(cReply));
	memset(&sPrdt_ac_id, 0 , sizeof(struct prdt_ac_id_c));
	memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 , sizeof(cKinbr));
	get_zd_data(DC_TX_BR_NO,cKinbr);
	vtcp_log("%s,%d 开始设置退汇的科目是===cmtno=[%.3s] ",__FILE__,__LINE__,payin->F_cmtno);
	vtcp_log("%s,%d ooooo sts ==[%c]",__FILE__,__LINE__,payin->T_hv_osts[0]);
	if (memcmp(payin->F_cmtno,HV_TH,sizeof(payin->F_cmtno)))
		return -1;
	vtcp_log("%s,%d ooooo sts ==[%c]",__FILE__,__LINE__,payin->T_hv_osts[0]);
	if (payin->T_hv_osts[0]==RECEIVE_ACCT_QS)
	{
		memcpy(cAc_no,payin->T_pay_ac_no,sizeof(cAc_no));
		pub_base_old_acno(cAc_no);
		if (!memcmp(payin->F_o_cmtno,HV_CJ,3))
		{
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cAc_no);
			if (iRet)
			{
				sprintf(acErrMsg,"得到本机构大额拆借帐号错误!\n");
				WRITEMSG
					sprintf(acErrMsg,"得到本机构大额拆借帐号错误!\n");
				vtcp_log("%s,%d 得到本机构大额拆借帐号错误\n",__FILE__,__LINE__);
				strcpy(g_pub_tx.reply,"P005");
				return iRet;
			}
			set_zd_data("0680","6");
		}
		iRet=Mdm_ac_rel_Sel(cReply,&sMdm_ac_rel,"ac_no='%s' ",cAc_no);
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_no=[%s]",__FILE__,__LINE__,cAc_no);
			WRITEMSG
				return(1);
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取介质帐号对照表错误!ac_no=[%s],ret=[%d]",__FILE__,__LINE__,cAc_no,iRet);
			WRITEMSG
				return(1);
		}
		if (sMdm_ac_rel.ac_seqn==9999)
		{
			/* 表明是多帐号的情况取其默认主帐户 */
			iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' and ac_seqn='1' ",sMdm_ac_rel.ac_id);
		}	else 
		{
			/* 否则取Mdm中的acseqn序号 */
			iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' and ac_seqn='%d' ",sMdm_ac_rel.ac_id,sMdm_ac_rel.ac_seqn);
		}
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1);
			WRITEMSG
				return(1);
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取产品帐号对照表错误!ac_id=[%d],ac_seqn=[%d],ret=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1,iRet);
			WRITEMSG
				return(1);
		}
		/** 根据ac_id确认是否是活期储蓄帐户 */
		if (sPrdt_ac_id.ac_id_type[0]=='1')  
		{
			set_zd_data("0680","1");
		}
		else
		{
			set_zd_data("0680","6");
		}
		set_zd_data("0300",cAc_no);
	}
	else
	{
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
		if (iRet)
		{
			sprintf(acErrMsg,"得到本机构大额挂帐帐号错误!\n");
			WRITEMSG
				sprintf(acErrMsg,"得到本机构大额挂帐帐号错误!\n");
			vtcp_log("%s,%d 得到本机构大额挂帐帐号错误\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P005");
			return iRet;
		}
		set_zd_data("0300",cAc_no);
	}
	set_zd_data("1021",cAc_no);
	vtcp_log("%s,%d 设置的科目是===[%s] ",__FILE__,__LINE__,cAc_no);
	return 0;
}
/****************************************************************
*函数名称:iHvCheckTh_input()                                    *
*函数功能:比较往帐输入的与大额支付退汇的原始来帐的信息          *
*参数描述: 输入：HV_PAYIN_AREA *payin_wz,*payin_lz              *
*              大额支付结构体往帐,大额支付结构体来帐            *
*          输出：char * errsmg           错误信息描述           *
*                                                               *
*设计到表:                                                      *
*作    者:刘华锋 2006-8-26 16:20                                *
****************************************************************/
int iHvCheckTh_input(HV_PAYIN_AREA *payin_wz,HV_PAYIN_AREA *payin_lz,char * errmsg)
{
	if (memcmp(payin_wz->F_cur_no,payin_lz->T_cur_no,sizeof(payin_wz->F_cur_no)))
	{
		vtcp_log("%s,%d 输入的币种与原来帐交易不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入的币种与原来帐交易不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_ac_br_no,payin_lz->T_or_br_no,sizeof(payin_wz->F_ac_br_no)))
	{
		vtcp_log("%s,%d 输入的接收行号与原发起行号不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入的接收行号与原发起行号不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_cash_ac_no,payin_lz->T_pay_ac_no,sizeof(payin_wz->F_cash_ac_no)))
	{
		vtcp_log("%s,%d 输入接收帐号与原付款人帐号不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入接收帐号与原付款人帐号不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_cash_name,payin_lz->T_pay_name,sizeof(payin_wz->F_cash_name)))
	{
		vtcp_log("%s,%d 输入收款人名字与原付款人名字不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入收款人名字与原付款人名字不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_pay_ac_no,payin_lz->T_cash_ac_no,sizeof(payin_wz->F_pay_ac_no)))
	{
		vtcp_log("%s,%d 输入付款帐号与原收款帐号不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入付款帐号与原收款帐号不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_pay_name,payin_lz->T_cash_name,sizeof(payin_wz->F_pay_name)))
	{
		vtcp_log("%s,%d 输入付款人名字与原收款人名字不同",__FILE__,__LINE__);
		memcpy(errmsg,"输入付款人名字与原收款人名字不同",50);
		return -1;
	}
	if (memcmp(payin_wz->F_tx_amt,payin_lz->T_tx_amt,sizeof(payin_wz->F_tx_amt)))
	{
		vtcp_log("%s,%d 输入的交易金额与原交易金额不同[%.15s][%.15s]",__FILE__,__LINE__,payin_wz->F_tx_amt,payin_lz->T_tx_amt);
		memcpy(errmsg,"输入的交易金额与原交易金额不同",50);
		return -1;
	}
	return 0;
}
int iHvBaseCheck1(HV_PAYIN_AREA *payin,char * errmsg)
{
	char cRececo[5];
	int iRet=0;
	memset(cRececo, 0 , sizeof(cRececo));
	if (payin->F_lw_ind[0]=='1')
	{
		/*往帐的时候检查接收行号的清算行号是否和发起行的清算行号一致*/
		if (memcmp(payin->T_cash_qs_no,payin->T_pay_qs_no,sizeof(payin->T_cash_qs_no)))
		{
			vtcp_log("%s,%d 接收行不可以是本机构的行号",__FILE__,__LINE__);
			memcpy(errmsg,"接收行不可以是本机构的行号",50);
			return -1;
		}
		memcpy(cRececo, payin->T_receco,sizeof(cRececo));
		zip_space(cRececo);
		if (strlen(cRececo)==0 ||!memcmp(cRececo,"NULL",sizeof(cRececo)-1))
		{
			vtcp_log("%s,%d 接收行收报中心=%s非法",__FILE__,__LINE__,cRececo);
			memcpy(errmsg,"接收行收报中心非法",50);
			return -1;
		}
		iRet=iHvPayin_characterCheck(payin,errmsg);
		if (iRet)
		{
			vtcp_log("%s,%d 发现非法字符",__FILE__,__LINE__,cRececo);
			return -1;
		}
	}
	else
	{
		/* 来帐的时候检查数据 */
	}
	return 0;
}
int iHvResetPayin(HV_PAYIN_AREA *payin)
{
	memcpy(payin->F_lw_ind  ,payin->T_lw_ind  ,sizeof(payin->F_lw_ind  ));
	memcpy(payin->F_tx_br_no,payin->T_tx_br_no,sizeof(payin->F_tx_br_no));
	memcpy(payin->F_tx_date ,payin->T_tx_date ,sizeof(payin->F_tx_date ));
	memcpy(payin->F_wt_date ,payin->T_wt_date ,sizeof(payin->F_wt_date ));
	memcpy(payin->F_orderno ,payin->T_orderno ,sizeof(payin->F_orderno ));
	memcpy(payin->F_cmtno   ,payin->T_cmtno   ,sizeof(payin->F_cmtno   ));
	memcpy(payin->F_cur_no  ,payin->T_cur_no  ,sizeof(payin->F_cur_no  ));
	memcpy(payin->F_note_type,payin->T_note_type,
		sizeof(payin->F_note_type));
	memcpy(payin->F_note_no ,payin->T_note_no ,sizeof(payin->F_note_no  ));
	memcpy(payin->F_tx_amt   ,payin->T_tx_amt ,sizeof(payin->F_tx_amt   ));
	memcpy(payin->F_tx_chrg_ind,payin->T_tx_chrg_ind,
		sizeof(payin->F_tx_chrg_ind));
	memcpy(payin->F_tx_chrg_amt,payin->T_tx_amt,
		sizeof(payin->F_tx_chrg_amt));
	memcpy(payin->F_or_br_no,payin->T_or_br_no,sizeof(payin->F_or_br_no));
	memcpy(payin->F_ac_br_no,payin->T_ac_br_no,sizeof(payin->F_ac_br_no));
	memcpy(payin->F_pay_opn_br_no ,payin->T_pay_opn_br_no ,
		sizeof(payin->F_pay_opn_br_no ));
	memcpy(payin->F_pay_ac_no,payin->T_pay_ac_no,
		sizeof(payin->F_pay_ac_no));
	memcpy(payin->F_pay_name ,payin->T_pay_name,sizeof(payin->F_pay_name));
	memcpy(payin->F_pay_addr ,payin->T_pay_addr ,sizeof(payin->F_pay_addr));
	memcpy(payin->F_userpswd ,payin->T_userpswd ,sizeof(payin->F_userpswd));
	memcpy(payin->F_cash_opn_br_no,payin->T_cash_opn_br_no,
		sizeof(payin->F_cash_opn_br_no));
	memcpy(payin->F_cash_ac_no,payin->T_cash_ac_no,
		sizeof(payin->F_cash_ac_no));
	memcpy(payin->F_cash_name ,payin->T_cash_name,
		sizeof(payin->F_cash_name));
	memcpy(payin->F_cash_addr ,payin->T_cash_addr,
		sizeof(payin->F_cash_addr));
	memcpy(payin->F_yw_type ,payin->T_yw_type ,sizeof(payin->F_yw_type));
	memcpy(payin->F_operlevel ,payin->T_operlevel,
		sizeof(payin->F_operlevel));
	memcpy(payin->F_tel      ,payin->T_tel     ,sizeof(payin->F_tel      ));
	memcpy(payin->F_chk      ,payin->T_chk     ,sizeof(payin->F_chk      ));
	memcpy(payin->F_auth_tel ,payin->T_auth_tel,sizeof(payin->F_auth_tel ));
	memcpy(payin->F_refno    ,payin->T_refno   ,sizeof(payin->F_refno    ));
	memcpy(payin->F_o_tx_date,payin->T_o_tx_date,
		sizeof(payin->F_o_tx_date));
	memcpy(payin->F_o_cmtno  ,payin->T_o_cmtno ,sizeof(payin->F_o_cmtno  ));
	memcpy(payin->F_hv_brf   ,payin->T_hv_brf  ,sizeof(payin->F_hv_brf   ));
	memcpy(payin->F_o_wt_date,payin->T_o_wt_date,
		sizeof(payin->F_o_wt_date));
	memcpy(payin->F_o_orderno,payin->T_o_orderno,
		sizeof(payin->F_o_orderno));
	memcpy(payin->F_beg_sts  ,payin->T_beg_sts ,sizeof(payin->F_beg_sts ));
	memcpy(payin->F_hv_osts  ,payin->T_hv_osts ,sizeof(payin->F_hv_osts ));
	memcpy(payin->F_resp1    ,payin->T_resp1   ,sizeof(payin->F_resp1   ));
}
int iHvReset8583(HV_PAYIN_AREA *payin)
{
	HV_FD123_AREA fd123;
	memset(&fd123, 0 , sizeof(fd123));
	get_fd_data("1230",(char*)&fd123);
	memcpy(fd123.cmtno    ,payin->F_cmtno    ,sizeof(fd123.cmtno    ));
	memcpy(fd123.o_cmtno  ,payin->F_o_cmtno  ,sizeof(fd123.o_cmtno  ));
	memcpy(fd123.pay_qs_no,payin->T_pay_qs_no,sizeof(fd123.pay_qs_no));
	memcpy(fd123.or_br_no ,payin->F_or_br_no ,sizeof(fd123.or_br_no ));
	memcpy(fd123.sendco   ,payin->T_sendco   ,sizeof(fd123.sendco   ));
	memcpy(fd123.pay_opn_br_no,payin->F_pay_opn_br_no,
		sizeof(fd123.pay_opn_br_no));
	memcpy(fd123.pay_ac_no ,payin->F_pay_ac_no ,sizeof(fd123.pay_ac_no ));
	memcpy(fd123.pay_name  ,payin->F_pay_name  ,sizeof(fd123.pay_name  ));
	memcpy(fd123.pay_addr  ,payin->F_pay_addr  ,sizeof(fd123.pay_addr  ));
	memcpy(fd123.cash_qs_no,payin->T_cash_qs_no,sizeof(fd123.cash_qs_no));
	vtcp_log("%s,%d cash_qs_no=[%.12s]",__FILE__,__LINE__,fd123.cash_qs_no);
	memcpy(fd123.ac_br_no,payin->F_ac_br_no,sizeof(fd123.ac_br_no));
	memcpy(fd123.receco  ,payin->T_receco  ,sizeof(fd123.receco  ));
	memcpy(fd123.cash_opn_br_no,payin->F_cash_opn_br_no,
		sizeof(fd123.cash_opn_br_no));
	memcpy(fd123.cash_ac_no,payin->F_cash_ac_no,sizeof(fd123.cash_ac_no));
	memcpy(fd123.cash_name,payin->F_cash_name,sizeof(fd123.cash_name));
	memcpy(fd123.cash_addr,payin->F_cash_addr,sizeof(fd123.cash_addr));
	memcpy(fd123.yw_type  ,payin->F_yw_type  ,sizeof(fd123.yw_type  ));
	memcpy(fd123.orderno  ,payin->F_orderno  ,sizeof(fd123.orderno  ));
	memcpy(fd123.brf ,payin->F_hv_brf,sizeof(fd123.brf ));
	memcpy(fd123.o_orderno,payin->F_o_orderno,sizeof(fd123.o_orderno));
	memcpy(fd123.procode  ,payin->F_resp1    ,sizeof(fd123.procode  ));
	memcpy(fd123.operlevel,payin->F_operlevel,sizeof(fd123.operlevel));
	memcpy(fd123.proc_sts ,payin->T_hv_sts   ,sizeof(fd123.proc_sts ));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
}
/****************************************************************
*int iHvCheckReceKinbr
*检查来帐机构是否是本行的机构
*参  数:THVPACKAGE * pack,char * brno
*         输  入           输出
*描  述:报文结构          新的机构号 
*返回值: 0正确      1 错误
*****************************************************************/
int iHvGetReceKinbr(THVPACKAGE * pack,char * brno)
{
	char cAc_br_no[13];
	char cReply[5];
	struct hv_orno_brno_c hv_orno_brno;
	struct  com_branch_c    sCom_branch;

	int iRet=0;
	char cQs_br_no[BRNO_LEN+1];
	memset(&sCom_branch, 0 , sizeof(sCom_branch));

	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));
	memset(cReply, 0 , sizeof(cReply));
	memset(&hv_orno_brno,0,sizeof(hv_orno_brno));
	memset(cAc_br_no, 0 , sizeof(cAc_br_no));
	iRet=iFind_qsbrno(cQs_br_no);
	if (iRet)
	{
		vtcp_log("%s,%d 极端情况清算中心也不存在[%d]",__FILE__,__LINE__,iRet);
		memcpy(cQs_br_no,QS_BR_NO,BRNO_LEN);
	}
	if (!memcmp(pack->CMTCode,"100",3) || !memcmp(pack->CMTCode,"101",3)
		||!memcmp(pack->CMTCode,"102",3) || !memcmp(pack->CMTCode,"103",3)
		||!memcmp(pack->CMTCode,"105",3) || !memcmp(pack->CMTCode,"108",3)
		||!memcmp(pack->CMTCode,"313",3) || !memcmp(pack->CMTCode,"314",3)
		||!memcmp(pack->CMTCode,"121",3) || !memcmp(pack->CMTCode,"122",3)
		||!memcmp(pack->CMTCode,"123",3) || !memcmp(pack->CMTCode,"124",3)
		||!memcmp(pack->CMTCode,"725",3)
		||!memcmp(pack->CMTCode,"803",3))
	{
		hv_get_tag(pack,"58A",cAc_br_no);
	}
	else if (!memcmp(pack->CMTCode,"301",3))
	{
		hv_get_tag(pack,"01D",cAc_br_no);
	}
	else if (!memcmp(pack->CMTCode,"303",3))
	{
		hv_get_tag(pack,"CCB",cAc_br_no);
	}
	else if (!memcmp(pack->CMTCode,"312",3))
	{
		hv_get_tag(pack,"CCD",cAc_br_no);
	}
	/** dongxy 20060402 增加812和814报文 **/
	if (!memcmp(pack->CMTCode,"253",3) || !memcmp(pack->CMTCode,"659",3)
		|| !memcmp(pack->CMTCode,"417",3) ||!memcmp(pack->CMTCode,"418",3)
		/* rem by LiuHuafeng 2009-5-17 19:17:39 for DEZF2.3.2
		 *|| !memcmp(pack->CMTCode,"232",3) 
		 */
		||!memcmp(pack->CMTCode,"802",3)
		|| !memcmp(pack->CMTCode,"812",3) ||!memcmp(pack->CMTCode,"814",3)
		|| !memcmp(pack->CMTCode,"253",3) ||!memcmp(pack->CMTCode,"910",3))
	{
		memcpy(brno,cQs_br_no,BRNO_LEN);
		vtcp_log("%s,%d 部分非支付类业务默认来帐机构为清算中心[%s]",__FILE__,__LINE__,cQs_br_no);
		return 0;
	}
	/* begin add by LiuHuafeng 2009-5-17 19:23:46 for DEZF2.3.2 */
	else if(!memcmp(pack->CMTCode,"232",3))
	{
		char cCrdb[2];
		memset(cCrdb , 0 , sizeof(cCrdb));
		/* 232，借的时候判断付款人开户行CC4
		 *      贷的时候判断收款人开户行CC9
		 */
		hv_get_tag(pack,"90C",cCrdb);
		vtcp_log("%s,%d crdb=%s",__FILE__,__LINE__,cCrdb);
		if(cCrdb[0]=='0')/* 被借记 */
		{
			hv_get_tag(pack,"CC4",cAc_br_no);
			vtcp_log("%s,%d ac_br_no=%s",__FILE__,__LINE__,cAc_br_no);
		}
		else
		{
			hv_get_tag(pack,"CC5",cAc_br_no);
			vtcp_log("%s,%d ac_br_no=%s",__FILE__,__LINE__,cAc_br_no);
		}
		vtcp_log("%s,%d ac_br_no=%s",__FILE__,__LINE__,cAc_br_no);
		pub_base_strpack(cAc_br_no);
		if(strlen(cAc_br_no)!=12)
		{
			memcpy(brno,cQs_br_no,BRNO_LEN);
			vtcp_log("%s,%d 232没有付款人或收款人开户行默认来帐机构为清算中心[%s]",__FILE__,__LINE__,cQs_br_no);
			return 0;
		}
		iRet=Hv_orno_brno_Sel(cReply,&hv_orno_brno,"or_br_no='%s'",cAc_br_no);
		if (iRet)
		{
			vtcp_log("%s,%d 来帐机构%s不是本行机构，设置交易机构为清算中心",__FILE__,__LINE__,cAc_br_no);
			memcpy(brno,cQs_br_no,BRNO_LEN);
			return iRet;
		}
	}
	/* end add by LiuHuafeng 2009-5-17 19:23:46 for DEZF2.3.2 */
	else
	{
		iRet=Hv_orno_brno_Sel(cReply,&hv_orno_brno,"or_br_no='%s'",cAc_br_no);
		if (iRet)
		{
			vtcp_log("%s,%d 来帐机构%s不是本行机构，设置交易机构为清算中心",__FILE__,__LINE__,cAc_br_no);
			memcpy(brno,cQs_br_no,BRNO_LEN);
			return iRet;
		}
	}
	/* 检查来帐机构状态是否正常 */
	/** 判断接收机构是否存在 */
	/* begin add by LiuHuafeng for 村镇银行 */
	if(!strncmp(hv_orno_brno.br_no,HV_CUNZHEN_BR_NO,4))
	{
		vtcp_log("%s,%d 接收机构是村镇银行,只进行登记，不进行账务处理 brno==[%s]",__FILE__,__LINE__,hv_orno_brno.br_no);
		strncpy(brno,hv_orno_brno.br_no,BRNO_LEN);
		WRITEMSG
		return 0;
	}
	/* end by LiuHuafeng 20090513 **/
	memcpy(sCom_branch.br_no,hv_orno_brno.br_no,BRNO_LEN);
	iRet=Com_branch_Sel(cReply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
	if (iRet)
	{
		vtcp_log("%s,%d 接收机构不存在,设置接收机构为清算中心iRet=[%d]",__FILE__,__LINE__,iRet);
		WRITEMSG
			memcpy(brno,cQs_br_no,BRNO_LEN);
		set_zd_data("0680","6");/* 帐户类型 */
		return iRet;
	}
	if (sCom_branch.wrk_sts[0]=='*')
	{
		vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
		WRITEMSG
			memcpy(brno,cQs_br_no,BRNO_LEN);
		set_zd_data("0680","6");/* 帐户类型 */
		return -1;
	}
	memcpy(brno,hv_orno_brno.br_no,BRNO_LEN);
	vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,brno);
	return 0;
}
/******************************************************************
*iHvGetReceOpnbr
*根据来帐帐号得到实际要处理的机构号
*参  数：const char * tx_br_no,THVPACKAGE *pack,char * opn_br_no
*                   输入                      | 输出 
*描  述：交易机构   大额来帐tag结构体             得到的实际机构
*返回值：0正确 1-需要挂账
*作  者：LiuHuafeng 2006-8-31 16:25
******************************************************************/
int iHvGetReceOpnbr(const char * tx_br_no,THVPACKAGE *pack,char * opn_br_no)
{
	char cAc_no[33];
	char cReply[5];
	struct hv_orno_brno_c hv_orno_brno;
	struct  prdt_ac_id_c    sPrdt_ac_id;
	struct  dd_mst_c        sDd_mst;
	struct  mdm_ac_rel_c    sMdm_ac_rel;
	struct  com_item_c      sCom_item;
	struct  com_branch_c    sCom_branch;

	int iRet=0;
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(&sCom_item , 0 , sizeof(sCom_item));
	memset(&sPrdt_ac_id,0x00,sizeof(struct prdt_ac_id_c));
	memset(cReply, 0 , sizeof(cReply));
	memset(&hv_orno_brno,0,sizeof(hv_orno_brno));
	memset(cAc_no, 0 , sizeof(cAc_no));
	if ( !memcmp(pack->CMTCode,"100",3) || !memcmp(pack->CMTCode,"101",3)
		|| !memcmp(pack->CMTCode,"102",3) || !memcmp(pack->CMTCode,"103",3))
	{
		hv_get_tag(pack,"59C",cAc_no);
	}
	else if (!memcmp(pack->CMTCode,"108",3))
		hv_get_tag(pack,"CQ1",cAc_no);
	else if (!memcmp(pack->CMTCode,"122",3))
		hv_get_tag(pack,"59E",cAc_no);
	else if (!memcmp(pack->CMTCode,"123",3) || !memcmp(pack->CMTCode,"124",3))
		hv_get_tag(pack,"50C",cAc_no);
	/* begin add by LiuHuafeng 2008-12-3 17:44:19 */
	else if(!memcmp(pack->CMTCode,"232",3))
	{
		char cHv232_crdb[2];
		memset(cHv232_crdb, 0 , sizeof(cHv232_crdb));
		hv_get_tag(pack,"90C",cHv232_crdb);
		if(cHv232_crdb[0]=='0')
		{
			/* 被借记行 */
			hv_get_tag(pack,"50C",cAc_no);
		}
		else
		{
			/* 被贷记行 */
			hv_get_tag(pack,"59C",cAc_no);
		}
		/* 压缩空格 */
		pub_base_strpack(cAc_no);
		if(strlen(cAc_no) < 13)
		{
			/* 认为账户不存在，就不再检查了 */
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return 0;	
		}
		else
		{
			/* 查一下 */
		}
	}
	/* end by LiuHuafeng 2008-12-3 17:53:30 */
	else
	{
		memcpy(opn_br_no,tx_br_no,BRNO_LEN);
		return 0;
	}
	zip_space(cAc_no);
	if (strlen(cAc_no)==7 || strlen(cAc_no)==5 || strlen(cAc_no)==3)
	{
		iRet=Com_item_Sel(cReply,&sCom_item,"acc_no='%s'",cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d得到科目错误",__FILE__,__LINE__);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return iRet;
		}
		/* 科目直接得到来帐机构作为开户机构 */
		iRet=iHvGetReceKinbr(pack,opn_br_no);
		if (iRet)
		{
			vtcp_log("%s,%d得到开户机构错误",__FILE__,__LINE__);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return iRet;
		}
	}
	else
	{
		pub_base_old_acno(cAc_no);
		iRet=Mdm_ac_rel_Sel(cReply,&sMdm_ac_rel,"ac_no='%s' ",cAc_no);
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_no=[%s]",__FILE__,__LINE__,cAc_no);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			WRITEMSG
				return(1);
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取介质帐号对照表错误!ac_no=[%s],ret=[%d]",__FILE__,__LINE__,cAc_no,iRet);
			memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			WRITEMSG
				return(1);
		}
		/**** 如果是内部账户ac_seqn是0 此处写死不对 20140126 wudawei 
		iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' and ac_seqn='1' ",
			sMdm_ac_rel.ac_id);
		20140126 wudawei end ***/
		iRet=Prdt_ac_id_Sel(cReply,&sPrdt_ac_id,"ac_id='%ld' ",
			sMdm_ac_rel.ac_id);
		if ( iRet==100 )
		{
			vtcp_log("%s,%d 帐号不存在!ac_id=[%d],ac_seqn=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return(1);
		}
		if ( iRet )
		{
			vtcp_log("%s,%d 取产品帐号对照表错误!ac_id=[%d],ac_seqn=[%d],ret=[%d]",__FILE__,__LINE__,sMdm_ac_rel.ac_id,1,iRet);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return(1);
		}
		/*** 对公活期 或 内部帐 ***/
		if ( sPrdt_ac_id.ac_id_type[0]=='1'||sPrdt_ac_id.ac_id_type[0]=='9')  
		{
		}
		else
		{
			vtcp_log("%s,%d 帐号类型错误![%s]",__FILE__,__LINE__,g_pub_tx.ac_id_type);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			return 1;
		}
		/** 判断开户机构是否存在 */
		memcpy(sCom_branch.br_no,sMdm_ac_rel.opn_br_no,BRNO_LEN);
		iRet=Com_branch_Sel(cReply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
		if (iRet)
		{
			vtcp_log("%s,%d 帐号对应的开户机构不存在,iRet=[%d]",__FILE__,__LINE__,iRet);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			set_zd_data("0680","6");/* 帐户类型 */
			return iRet;
		}
		if (sCom_branch.wrk_sts[0]=='*')
		{
			vtcp_log("%s,%d 帐号对应开户机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
			WRITEMSG
				memcpy(opn_br_no,tx_br_no,BRNO_LEN);
			set_zd_data("0680","6");/* 帐户类型 */
			return -1;
		}
		/* 检查开户机构是否存在 */
		memcpy(opn_br_no,sMdm_ac_rel.opn_br_no,sizeof(sMdm_ac_rel.opn_br_no)-1);
	}
	vtcp_log("%s,%d 得到的开户机构号码是[%s] ",__FILE__,__LINE__,opn_br_no);
	return 0;
}
/******************************************************************/
/*iSetHvTxBrNo(THVPACKAGE *pack)                                  */
/*设置来帐的交易机构                                              */
/*参  数：THVPACKAGE *pack                                        */
/*                   输入     | 输出                              */
/*描  述：大额来帐tag结构体     $FD30,$FD20                       */
/*返回值：0正确 1-需要支行挂账，其他！=0 需要清算挂账             */
/*        9999 表示只登记不进行任何账务处理,for 村镇银行          */
/*作  者：LiuHuafeng 2006-8-31 16:25                              */
/******************************************************************/
int iSetHvTxBrNo(THVPACKAGE *pack)
{
	char cKinbr[BRNO_LEN+1];
	char cOpnbr[BRNO_LEN+1];
	int iRet_kinbr=0,iRet_opnbr=0;
	memset(cKinbr,0,sizeof(cKinbr));
	memset(cOpnbr,0,sizeof(cOpnbr));
	iRet_kinbr=iHvGetReceKinbr(pack,cKinbr);
	/* begin add by LiuHuafeng for 村镇银行 */
	if(iRet_kinbr==0 && strncmp(cKinbr,HV_CUNZHEN_BR_NO,4)==0)
	{
		/* 机构是村镇银行只登记不做任何处理 */
		vtcp_log("%s %d 机构是村镇银行只登记不处理 ",__FILE__,__LINE__);
		return 9999;
	}
	/* end by LiuHuafeng 20090513 **/
	iRet_opnbr=iHvGetReceOpnbr(cKinbr,pack,cOpnbr);
	if (iRet_opnbr)
	{
		if (iRet_kinbr==0)
		{
			/* begin add by LiuHuafeng 2008-12-16 8:58:56 */
			if(!memcmp(pack->CMTCode,"232",3))
			{
				/* 232 不需要处理 */
				char cTel[7];
				memset(cTel  , 0 , sizeof(cTel));
				memcpy(cKinbr,cOpnbr,BRNO_LEN);
				set_zd_data(DC_TX_BR_NO,cKinbr);
				set_zd_data("0020",cKinbr);
				memcpy(cTel,cKinbr+1,2);
				memcpy(cTel+2,"99",2);
				set_zd_data("0070",cTel);/** 设置找到机构号后的默认操作员 */
				vtcp_log("%s,%d 得到新的交易机构=[%s]\n",__FILE__,__LINE__,cKinbr);
				return 0;
			}
			else
			{
				/* end by LiuHuafeng 2008-12-16 8:59:04 */
				vtcp_log("%s,%d 得到开户机构错误，设置来帐机构为交易机构[%s]",__FILE__,__LINE__,cKinbr);
				set_zd_data(DC_TX_BR_NO,cKinbr);
				set_zd_data("0020",cKinbr);
				vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=1\n",__FILE__,__LINE__,cKinbr);
				return 1;
			/* begin add by LiuHuafeng 2008-12-16 8:59:12 */
			}
			/* end by LiuHuafeng 2008-12-16 8:59:17 */
		}
		else
		{
			vtcp_log("%s,%d 得到开户机构错误，来帐机构也错误设置清算中心为交易机构[%s]",__FILE__,__LINE__,cKinbr);
			set_zd_data(DC_TX_BR_NO,cKinbr);
			set_zd_data("0020",cKinbr);
			vtcp_log("%s,%d 得到新的交易机构=[%s]挂账标志=%d\n",__FILE__,__LINE__,cKinbr,iRet_kinbr);
			return iRet_kinbr;
		}
	}
	else
	{
		char cTel[7];
		memset(cTel  , 0 , sizeof(cTel));
		memcpy(cKinbr,cOpnbr,BRNO_LEN);
		set_zd_data(DC_TX_BR_NO,cKinbr);	
		set_zd_data("0020",cOpnbr);
		memcpy(cTel,cOpnbr+1,2);
		memcpy(cTel+2,"99",2);
		set_zd_data("0070",cTel);/** 设置找到机构号后的默认操作员 */
		vtcp_log("%s,%d 得到新的交易机构=[%s]\n",__FILE__,__LINE__,cOpnbr);
		return 0;
	}
	return 0;
}
int iFind_qsbrno(char * qs_br_no)
{
	struct com_branch_c sCom_branch;
	char cReply[5];
	int iRet=0;
	memset(cReply, 0 ,sizeof(cReply));
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	iRet=Com_branch_Sel(cReply,&sCom_branch,"br_type='1' ");
	if (iRet)
	{
		vtcp_log("%s,%d得到机构号错误",__FILE__,__LINE__);
		return iRet;
	}
	memcpy(qs_br_no,sCom_branch.br_no,BRNO_LEN);
	return 0;
}
/* 得到大额来帐的操作员 */
/*******************************************************************/
/* iGetReceTel(const char * tx_br_no)                              */
/* 设置来帐的操作员                                                */
/* 参  数：char * tx_br_no                                         */
/*              输入         | 输出                                */
/* 描  述：来帐的机构     $FD7                                     */
/* 返回值：0正确 1-需要挂账                                        */
/* 作  者：LiuHuafeng 2006-8-31 16:25                              */
/*******************************************************************/
int iGetReceTel(const char * tx_br_no)
{
	struct com_tel_c sCom_tel;
	char cReply[5];
	char cTel[7];
	int iRet=0;
	memset(cTel, 0 , sizeof(cTel));
	memset(cReply, 0 , sizeof(cReply));
	memset(&sCom_tel,  0 , sizeof(sCom_tel));
	memcpy(cTel,tx_br_no+1,2);
	memcpy(cTel+2,"99",2);
	set_zd_data("0070",cTel);
	/* 直接取机构号2,3位+99 */	
	return 0;

	memcpy(sCom_tel.br_no,tx_br_no,BRNO_LEN);
	iRet=Com_tel_Sel(cReply,&sCom_tel,"br_no='%s' and lvl='6' ",sCom_tel.br_no);
	if (iRet)
	{
		Com_tel_Debug(&sCom_tel);
		vtcp_log("%s,%d得到机构号错误",__FILE__,__LINE__);
		vtcp_log( acErrMsg, "此柜员不存在" );
		WRITEMSG
			vtcp_log( cReply, "S009" );
		return(-1);
	}
	memcpy(cTel,sCom_tel.tel,sizeof(sCom_tel.tel)-1);
	set_zd_data("0070",cTel);
	return 0;
}
/***********************************************************/
/*检查大额253，910是否记帐                                 */
/*iHvCheck253_910_stat(THVPACKAGE * pack)                  */
/*                                                         */
/*                                                         */
/*返回 1需要记帐  0 不需要记帐                             */
/***********************************************************/
int iHvCheck253_910_stat(THVPACKAGE * pack )
{
	char cResp1[9];
	struct hv_fzf_c sHv_fzf;
	struct hv_zf_c sHv_zf;
	char cRefno[21];
	int iJzType=0;/* 1记帐，2，不记帐 */
	int iRet=0;
	char cAc_no[20];
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cResp1,0 , sizeof(cResp1));
	memset(cRefno, 0 , sizeof(cRefno));

	vtcp_log("[%s][%d] 2222   到这里了 !!  pack->CMTCode=[%.3s]",__FILE__,__LINE__,pack->CMTCode);    	

	if ( memcmp(pack->CMTCode,"253",3) && memcmp(pack->CMTCode,"910",3) 
		&& memcmp(pack->CMTCode,"312",3))
	{
		vtcp_log("[%s][%d] pack->CMTCode 不对!  ",__FILE__,__LINE__); 		
		return -1;
	}
	vtcp_log("[%s][%d] 2222   到这里了 !!  ",__FILE__,__LINE__); 		
	iHVGETTAG(pack,"C42",cResp1,sizeof(cResp1)-1);/*处理码*/
	vtcp_log("[%s][%d] 2222   到这里了 !! cResp1=[%s] ",__FILE__,__LINE__,cResp1); 	

	if (memcmp(pack->CMTCode,"312",3)==0)  
	{
		char resp_code[2];
		/* 原来312在第三个分支， cResp1=''时，会跑到另一分支，          */
		/* 所以将这个分支提前了 Modified by ChenMing  2006-10-15 10:39  */
		vtcp_log("[%s][%d] 312   到这里了 !!  ",__FILE__,__LINE__); 		
		memset(resp_code, 0 , sizeof(resp_code));
		iHVGETTAG(pack,"04C",resp_code,sizeof(resp_code)-1);/*处理码*/

		vtcp_log("[%s][%d] 2222   到这里了 !!  resp_code=[%s]",__FILE__,__LINE__,resp_code);    	

		if (resp_code[0]=='1')
		{
			/* 不能撤销 */
			iJzType=2;
		}
		else
		{
			double dTx_amt=0.0;
			iJzType=1;
			/* Begin Added by ChenMing 2006-10-15 10:55    应该先找到原来的那笔311记录  */
			memset(&sHv_fzf,0, sizeof(sHv_fzf));    
			iRet = iHvFindOld311ByHvpack(pack, &sHv_fzf);
			if (iRet == 100 )
			{
				vtcp_log("[%s][%d] 没有找到对应的 311 报文 或者 原撤销申请状态不正确 !!! ",__FILE__,__LINE__);      
				sprintf(acErrMsg," 没有找到对应的 311 报文 或者 原撤销申请状态不正确 !!");
				WRITEMSG
				/* begin add by LiuHuafeng 2009-5-14 8:37:52 */
				/* 由于有村镇银行，所以这里不进行处理，直接设置记账标志为不记账
				 *	return iRet;
				 **************************/				
				iJzType=2;
				/* end by LiuHuafeng 2009-5-14 8:38:01 for 村镇银行 */
			}
			else if (iRet)
			{
				sprintf(acErrMsg,"数据库错误 !!");
				WRITEMSG
				return iRet;
			}
			/*  再根据311找到原来的那笔支付交易记录 */
			if(iJzType == 2)
			{
				vtcp_log("%s,%d 由于没有找到原撤销记录，认为是村镇银行，所以不再进行账务处理",__FILE__,__LINE__);
			}
			else
			{
				memset(&sHv_zf,0, sizeof(sHv_zf));  
      	
				vtcp_log("[%s][%d] sHv_fzf->o_or_br_no=[%s] ",__FILE__,__LINE__,sHv_fzf.o_or_br_no);  
				vtcp_log("[%s][%d] sHv_fzf->o_orderno=[%s] ",__FILE__,__LINE__,sHv_fzf.o_orderno);        
				vtcp_log("[%s][%d] sHv_fzf->o_wt_date=[%lf] ",__FILE__,__LINE__,sHv_fzf.o_wt_date);
      	
				sHv_zf.wt_date = sHv_fzf.o_wt_date;
				memcpy(sHv_zf.orderno,sHv_fzf.o_orderno,sizeof(sHv_zf.orderno)-1);  
				memcpy(sHv_zf.or_br_no,sHv_fzf.o_or_br_no,sizeof(sHv_zf.or_br_no)-1);    	
				iRet=Hv_zf_Sel(g_pub_tx.reply,&sHv_zf,"orderno='%s' and or_br_no='%s' and wt_date=%d",sHv_zf.orderno,sHv_zf.or_br_no,sHv_zf.wt_date);
				vtcp_log("%s,%d  ret==[%d] ",__FILE__,__LINE__,iRet);
				if (iRet == 100 )
				{
					vtcp_log("%s,%d 根据CMT311找不到原来那笔支付交易记录 ret=[%d]",__FILE__,__LINE__,iRet);
					sprintf(acErrMsg,"根据CMT311找不到原来那笔支付交易记录!");
					WRITEMSG
						return iRet;
				}
				else if(iRet)
				{
					vtcp_log("%s,%d 支付交易表游标定义错误 iRet=[%d]",__FILE__,__LINE__,iRet);
					sprintf(acErrMsg,"支付交易表游标定义错误!");
					WRITEMSG
					return iRet;
				}
				if ( sHv_zf.hv_sts[0]==SEND_REJECT || sHv_zf.hv_sts[0]==SEND_ACCT_QS 
					|| sHv_zf.hv_sts[0]==SEND_CANCEL_ACCT || sHv_zf.hv_sts[0]== SEND_BACK_ACCT)
				{
					vtcp_log("当前状态是最终状态[%c]不需要再处理",sHv_zf.hv_sts[0]);
					sprintf(acErrMsg,"当前状态是最终状态 不能撤消 !");
					WRITEMSG
					return -1;
				}
				/* begin add by LiuHuafeng 20070505 
				  在这里设置往帐状态
				***********************************/
				g_reply_int=sql_execute("update hv_zf set hv_sts='6' where orderno='%s' and or_br_no='%s' and wt_date=%d",sHv_zf.orderno,sHv_zf.or_br_no,sHv_zf.wt_date);
				if(g_reply_int)
				{
					vtcp_log("%s,%d 更新当前状态是最终状态错误=%d",__FILE__,__LINE__,g_reply_int);
					sprintf(acErrMsg,"更新原状态是最终状态错误 !");
					WRITEMSG
					return -1;
				}
				/* end by LiuHuafeng 20070505 **/
				/* End Added by ChenMing 2006-10-15 10:55  ------------------------- */	
				/* begin add by LiuHuafeng 20090513 for 村镇银行 */
				if(strncmp(sHv_zf.br_no,HV_CUNZHEN_BR_NO,4)==0)
				{
					vtcp_log("当前状态是最终状态[%c]村镇银行数据需要再处理",sHv_zf.hv_sts[0]);
					iJzType=2;/* 不记帐 */
				}
				else
				{
				/* end by LiuHuafeng 20090513 for 村镇银行 */
					set_zd_double("0390",sHv_zf.tx_amt * -1);  /*原支付交易金额*/
					iRet=iHvSetRejectAndCancelAc_no(&sHv_zf);
					if (iRet)
					{
						vtcp_log("%s,%d根据参考号[%s]没有找到非支付报文",__FILE__,__LINE__,cRefno);
						vtcp_log("%s,%d啥都没有找到",__FILE__,__LINE__,cRefno);
						return iRet;
					}
					set_zd_double("0400", sHv_zf.tx_amt * -1);    /*原支付交易金额 */
					set_zd_data("0660","1");
					set_zd_data("0720","2");
					set_zd_data("0210","01");
					vtcp_log("%s,%d 收到支付类312 同意撤销!! [%s]",__FILE__,__LINE__,resp_code);
      		
					/* For Test  */   
      		
					get_zd_double("0390",&dTx_amt); 
					vtcp_log("%s,%d 0390  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);
					dTx_amt = 0.0;
					get_zd_double("0400",&dTx_amt); 
					vtcp_log("%s,%d 0400  dTx_amt=======[%lf]",__FILE__,__LINE__,dTx_amt);
				}
			}
		}
	}
	else if (memcmp(pack->CMTCode,"910",3)==0)
	{
		vtcp_log("[%s][%d] 910   到这里了 !!  ",__FILE__,__LINE__); 		

		memcpy(cRefno,pack->reNo,sizeof(cRefno)-1); /* 参考号 */
		iRet=Hv_zf_Sel(g_pub_tx.reply,&sHv_zf,"refno='%s' ",cRefno);
		if (iRet==0)
		{
			if ( sHv_zf.hv_sts[0]==SEND_REJECT || sHv_zf.hv_sts[0]==SEND_ACCT_QS 
				|| sHv_zf.hv_sts[0]==SEND_CANCEL_ACCT || sHv_zf.hv_sts[0]== SEND_BACK_ACCT)
			{
				vtcp_log("[%s][%d]当前状态是最终状态[%c]不需要再处理",__FILE__,__LINE__,sHv_zf.hv_sts[0]);
				iJzType=2;/* 不记帐 */
			}
			/* begin add by LiuHuafeng 20090513 for 村镇银行 */
			else if (strncmp(sHv_zf.br_no,HV_CUNZHEN_BR_NO,4)==0)
			{
				vtcp_log("[%s][%d] 村镇银行业务，不需要再处理",__FILE__,__LINE__);
				iJzType=2;/* 不记帐 */
			}
			/* end add by LiuHuafeng 20090513 for 村镇银行 */
			else
			{
				vtcp_log("%s,%d 收到支付类910需要走帐[%s]",__FILE__,__LINE__,cResp1);
				iJzType=1;/* 记帐 */
				set_zd_double("0390",sHv_zf.tx_amt * -1);
				iRet=iHvSetRejectAndCancelAc_no(&sHv_zf);
				if (iRet)
				{
					vtcp_log("%s,%d根据参考号[%s]没有找到非支付报文",__FILE__,__LINE__,cRefno);
					vtcp_log("%s,%d啥都没有找到",__FILE__,__LINE__,cRefno);
					return iRet;
				}
				set_zd_double("0400", sHv_zf.tx_amt * -1);   
				set_zd_data("0660","1");
				set_zd_data("0720","2");
				set_zd_data("0210","01");
			}
		}
		else
		{
			vtcp_log("%s,%d 收到非支付类910不需要走帐[%s]",__FILE__,__LINE__,cResp1);
			iJzType=2;/* 不记帐*/
		}
	}
	else if (cResp1[3]!='I' && memcmp(cResp1,"CS2O9010",8))
	{
		vtcp_log("[%s][%d] III   到这里了 !!  cResp1=[%s]",__FILE__,__LINE__,cResp1); 		

		/* 表明有可能要走帐 */
		/* 如果是910，根据标识号确定一笔交易,如果是253，根据orderno确定一笔交易 */
		if (memcmp(pack->CMTCode,"253",3)==0)
		{
			char cWt_date[9];
			char cOrderno[9];
			char cOr_br_no[23];
			/* begin add by LiuHuafeng 2009-5-13 22:07:56 for 村镇银行 */
			struct hv_orno_brno_c sOrno_brno;
			char cReply[5];
			/* end by LiuHuafeng 2009-5-13 22:08:11 for 村镇银行 */
			memset(cOr_br_no, 0 , sizeof(cOr_br_no));
			memset(cWt_date, 0 , sizeof(cWt_date));
			memset(cOrderno, 0 , sizeof(cOrderno));

			iHVGETTAG(pack,"30A",cWt_date  ,sizeof(cWt_date)-1);/*委托日期*/
			iHVGETTAG(pack,"0BC",cOrderno  ,sizeof(cOrderno)-1);	/* 支付交易序号*/
			iHVGETTAG(pack,"52A",cOr_br_no ,sizeof(cOr_br_no)-1);	/* 发起行号*/
			vtcp_log("%s,%d wt_date[%s]",__FILE__,__LINE__,cWt_date);
			vtcp_log("%s,%d orderno[%s]",__FILE__,__LINE__,cOrderno);
			vtcp_log("%s,%d or_br_no[%s]",__FILE__,__LINE__,cOr_br_no);
			/* begin add by LiuHuafeng 2009-5-13 21:22:51 for 村镇银行*/
			memset(cReply , 0 , sizeof(cReply));
			memset(&sOrno_brno , 0 , sizeof(sOrno_brno));
			iRet=Hv_orno_brno_Sel(cReply,&sOrno_brno,"or_br_no='%s'",cOr_br_no);
			if (iRet == 0   && strncmp(sOrno_brno.br_no,HV_CUNZHEN_BR_NO,4)==0) 
			{
				iJzType = 2; /* 不记账 */
				vtcp_log("%s,%d  原发起机构是村镇银行，所以收到253不处理",__FILE__,__LINE__);
			}
			else
			{
			/* end by LiuHuafeng 2009-5-13 21:22:56  for 村镇银行*/
				iRet=Hv_zf_Sel(g_pub_tx.reply,&sHv_zf,"orderno='%s' and wt_date='%s' and or_br_no='%s' ",cOrderno,cWt_date,cOr_br_no);
				if (iRet==0)
				{
					/* begin add by LiuHuafeng 20090513 for 村镇银行 */
					if(strncmp(sHv_zf.br_no,HV_CUNZHEN_BR_NO,4)==0)
					{
						vtcp_log("当前状态是最终状态[%c]村镇银行数据需要再处理",sHv_zf.hv_sts[0]);
						iJzType=2;/* 不记帐 */
					}
					else
					{
					/* end by LiuHuafeng 20090513  for 村镇银行 */
						if ( sHv_zf.hv_sts[0]==SEND_REJECT || sHv_zf.hv_sts[0]==SEND_ACCT_QS 
							|| sHv_zf.hv_sts[0]==SEND_CANCEL_ACCT || sHv_zf.hv_sts[0]== SEND_BACK_ACCT)
						{
							vtcp_log("当前状态是最终状态[%c]不需要再处理",sHv_zf.hv_sts[0]);
							iJzType=2;/* 不记帐 */
						}
						else
						{
							vtcp_log("%s,%d 收到253状态为拒绝%s肯定需要走帐",__FILE__,__LINE__,cResp1);
							vtcp_log("%s,%d 收到支付类253拒绝需要走帐[%s]",__FILE__,__LINE__,cResp1);
							iJzType=1;/* 记帐 */
							set_zd_double("0390",sHv_zf.tx_amt * -1);
							iRet=iHvSetRejectAndCancelAc_no(&sHv_zf);
							if (iRet)
							{
								vtcp_log("%s,%d根据参考号[%s]没有找到非支付报文",__FILE__,__LINE__,cRefno);
								vtcp_log("%s,%d啥都没有找到",__FILE__,__LINE__,cRefno);
								return iRet;
							}
							set_zd_double("0400", sHv_zf.tx_amt * -1);   
							set_zd_data("0660","1");
							set_zd_data("0720","2");
							set_zd_data("0210","01");
						}
					/* begin add  by LiuHuafeng 20090513  for 村镇银行*/
					}
					/* end by LiuHuafeng 20090513 */
				}
				else
				{
					vtcp_log("%s,%d 收到支付类253拒绝需要走帐,但找不到原始数据[%s]",__FILE__,__LINE__,cResp1);
					vtcp_log("%s,%d wt_date[%s]",__FILE__,__LINE__,cWt_date);
					vtcp_log("%s,%d orderno[%s]",__FILE__,__LINE__,cOrderno);
					vtcp_log("%s,%d or_br_no[%s]",__FILE__,__LINE__,cOr_br_no);
					iJzType=2;/* 不记帐 */
				}
			}
		}
	}
	else
	{
		iJzType=2;/* 不记帐 */
	}
	vtcp_log("[%s][%d] 2222   到这里了 !! iJzType=[%d] ",__FILE__,__LINE__,iJzType); 		

	if (iJzType==1) return 1;
	else return 0;
}
int iHv253_910_proc(HV_NOTPAY_AREA * hv_notpay)
{
	struct hv_zf_c sHv_zf;
	struct hv_fzf_c sHv_fzf;
	char cRefno[21];
	int iZfType=0;/* 1支付类交易，2，非支付类交易 */
	int iRet=0;
	memset(&sHv_zf, 0 , sizeof(sHv_zf));
	memset(&sHv_fzf,0,sizeof(sHv_fzf));
	memset(cRefno, 0 , sizeof(cRefno));

	if (memcmp(hv_notpay->T_cmtno,"253",3) && memcmp(hv_notpay->T_cmtno,"910",3) )
	{
		vtcp_log("%s,%d cmtno=%.3s 不是253或910，不需要处理",__FILE__,__LINE__,hv_notpay->T_cmtno);
		return 0;
	}
	vtcp_log("%s,%d hv_notpay->T_orderno [%s]",__FILE__,__LINE__,hv_notpay->T_orderno );
	vtcp_log("%s,%d hv_notpay->T_or_br_no[%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
	vtcp_log("%s,%d hv_notpay->T_wt_date [%s]",__FILE__,__LINE__,hv_notpay->T_wt_date );
	vtcp_log("%s,%d hv_notpay->T_refno   [%s]",__FILE__,__LINE__,hv_notpay->T_refno   );
	vtcp_log("%s,%d hv_notpay->T_resp1   [%s]",__FILE__,__LINE__,hv_notpay->T_resp1   );

	/* 应答错误需要根据标识号或支付交易序号找到原来那笔交易，再更新这笔交易 */
	if (!memcmp(hv_notpay->T_cmtno,"910",3))
	{
		memcpy(cRefno,hv_notpay->T_refno,sizeof(cRefno)-1);
		iRet=Hv_zf_Sel(g_pub_tx.reply,&sHv_zf,"refno='%s'",cRefno);
		if (iRet)
		{
			vtcp_log("%s,%d根据参考号[%s]没有找到支付报文",__FILE__,__LINE__,cRefno);
			iRet=Hv_fzf_Sel(g_pub_tx.reply,&sHv_fzf,"refno='%s'",cRefno);
			if (iRet)
			{
				vtcp_log("%s,%d根据参考号[%s]没有找到非支付报文",__FILE__,__LINE__,cRefno);
				vtcp_log("%s,%d啥都没有找到",__FILE__,__LINE__,cRefno);
				return iRet;
			}
			else
			{
				iZfType=2;/* 非支付交易 */
			}
		}
		else
		{
			iZfType=1; /* 支付交易 */
		}
	}
	if (!memcmp(hv_notpay->T_cmtno,"253",3))
	{
		memcpy(sHv_zf.orderno ,hv_notpay->T_orderno,sizeof(sHv_zf.orderno)-1);
		memcpy(sHv_zf.or_br_no,hv_notpay->T_or_br_no,sizeof(sHv_zf.or_br_no)-1);
		sHv_zf.wt_date=apatoi(hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));
		iZfType=1;
	}
	/* Begin Remed by ChenMing 312不再走这个分支了    2000-1-1 0:58
	根据312找到原 311 在根据311找到原支付交易
	if(!memcmp(hv_notpay->T_cmtno,"312",3))
	{

	struct hv_fzf_c sHv_fzf;
	memset(&sHv_fzf, 0 , sizeof(sHv_fzf));
	sHv_fzf.res_date=apatoi(hv_notpay->T_o_wt_date,8);
	memcpy(sHv_fzf.res_br_no,hv_notpay->T_res_br_no,sizeof(sHv_fzf.res_br_no)-1);
	memcpy(sHv_fzf.res_orderno,hv_notpay->T_res_orderno,sizeof(sHv_fzf.res_orderno)-1);
	memcpy(sHv_fzf.o_orderno, hv_notpay->T_o_payin_orderno,sizeof(sHv_fzf.o_orderno)-1);
	zip_space(sHv_fzf.res_orderno);
	vtcp_log("%s,%d hv_notpay->T_o_wt_date      [%.8s]",__FILE__,__LINE__,hv_notpay->T_o_wt_date      );
	vtcp_log("%s,%d hv_notpay->T_res_br_no      [%.12s]",__FILE__,__LINE__,hv_notpay->T_res_br_no      );
	vtcp_log("%s,%d hv_notpay->T_res_orderno    [%.8s]",__FILE__,__LINE__,hv_notpay->T_res_orderno    );
	vtcp_log("%s,%d hv_notpay->T_o_payin_orderno[%.8s]",__FILE__,__LINE__,hv_notpay->T_o_payin_orderno);

	iRet=Hv_fzf_Sel(g_pub_tx.reply,&sHv_fzf,
	"otype='1' and wt_date=%d and orderno='%s' and or_br_no='%s' and o_orderno='%s' ",
	sHv_fzf.res_date,sHv_fzf.res_orderno,sHv_fzf.res_br_no,sHv_fzf.o_orderno);
	if(iRet)
	{
	vtcp_log("%s,%d 非支付交易查询错误 ret=[%d]",__FILE__,__LINE__,iRet);
	vtcp_log(acErrMsg,"查询原撤销申请错误!");
	WRITEMSG
	return iRet;
	}

	memcpy(sHv_zf.orderno ,hv_notpay->T_o_payin_orderno,sizeof(sHv_zf.orderno)-1);
	memcpy(sHv_zf.or_br_no,hv_notpay->T_res_orderno,sizeof(sHv_zf.or_br_no)-1);
	sHv_zf.wt_date=sHv_fzf.o_wt_date;
	iZfType=1;
	}		

	End Remed by ChenMing 312不再走这个分支了    2000-1-1 0:58 */

	if (iZfType==1)
	{
		/* 支付类交易要根据状态判断 */ 
		/* begin add by LiuHuafeng 2009-5-13 21:54:18 for 村镇银行 */
		struct hv_orno_brno_c sOrno_brno;
		char cReply[5];
		memset(cReply , 0 , sizeof(cReply));
		memset(&sOrno_brno , 0 , sizeof(sOrno_brno));
		iRet=Hv_orno_brno_Sel(cReply,&sOrno_brno,"or_br_no='%s'",sHv_zf.or_br_no);
		if (iRet ==0 && strncmp(sOrno_brno.br_no,HV_CUNZHEN_BR_NO,4)==0)
		{
			vtcp_log("%s,%d 发起行的机构是村镇银行，所以不需要处理",__FILE__,__LINE__);
		}
		else
		{
		/* end by LiuHuafeng 2009-5-13 21:54:33 */
			iRet=Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d",sHv_zf.orderno,sHv_zf.or_br_no,sHv_zf.wt_date);
			if (iRet)
			{
				vtcp_log("%s,%d 支付交易表游标定义错误 ret=[%d]",__FILE__,__LINE__,iRet);
				vtcp_log(acErrMsg,"支付交易表游标定义错误!");
				WRITEMSG
					return iRet;
			}
			iRet=Hv_zf_Fet_Upd(&sHv_zf,g_pub_tx.reply);
			if (iRet)
			{
				vtcp_log("%s,%d 读取支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
				vtcp_log(acErrMsg,"读取支付交易表错误!");
				WRITEMSG
					return iRet;
			}
			if ( sHv_zf.hv_sts[0]==SEND_REJECT || sHv_zf.hv_sts[0]==SEND_ACCT_QS 
				|| sHv_zf.hv_sts[0]==SEND_CANCEL_ACCT || sHv_zf.hv_sts[0]== SEND_BACK_ACCT)
			{
				vtcp_log("当前状态是最终状态[%c]不需要再处理",sHv_zf.hv_sts[0]);
			}
			else
			{
				if (hv_notpay->T_resp1[3]!='I' || !memcmp(hv_notpay->T_cmtno,"910",3))
				{
					/* 支付类交易需要记帐处理,执行到这一步意味着已经处理结束，
					* 所以只需要找到原始交易设置其状态就可以了 
					*/
					if (memcmp(hv_notpay->T_resp1,"CS2O9010",8))
					{
						sHv_zf.hv_sts[0]=SEND_REJECT;
						iHvChangeLzStatForTh(&sHv_zf,'@');
    	
						/* Begin Added by ChenMing  汇票移存被拒绝后，将汇票状态置回“签发”状态 */ 
						if (memcmp(sHv_zf.cmtno,"121",sizeof(sHv_zf.cmtno)-1)==0)
						{
							vtcp_log("[%s,%d] 汇票移存被拒绝后，将汇票状态置回“签发”状态!",__FILE__,__LINE__);						
							iRet = iHvChangeHPStatForHPYC(&sHv_zf);
							if (iRet)
							{
								vtcp_log("%s,%d 更新汇票信息表错误 ret=[%d]",__FILE__,__LINE__,iRet);
								vtcp_log(acErrMsg,"更新汇票信息表错误");
								WRITEMSG
									return iRet;
							}		
							vtcp_log("[%s,%d] 更新汇票状态成功!  ",__FILE__,__LINE__);						
						}
						/* End Added by ChenMing   2006-11-1 11:58                        */
					}
					else
						sHv_zf.hv_sts[0]=SEND_INQUEUE; /*排队 */
				}
				else
				{
					sHv_zf.hv_sts[0]=SEND_ACCT_QS;
					iHvChangeLzStatForTh(&sHv_zf,RECEIVE_BACK_ACCT);
				}
				memcpy(sHv_zf.resp1,hv_notpay->T_resp1,sizeof(sHv_zf.resp1)-1);
			}
			iRet=Hv_zf_Upd_Upd(sHv_zf,g_pub_tx.reply);
			if (iRet)
			{
				vtcp_log("%s,%d 更新支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
				vtcp_log(acErrMsg,"更新支付交易表错误");
				WRITEMSG
				return iRet;
			}
		}
	}
	if (iZfType==2)
	{
		/* 是非支付交易，直接设置原始交易状态 */
		iRet=Hv_fzf_Dec_Upd(g_pub_tx.reply,"refno='%s'",cRefno);
		if (iRet)
		{
			vtcp_log("%s,%d 支付交易表游标定义错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"支付交易表游标定义错误!");
			WRITEMSG
				return iRet;
		}
		iRet=Hv_fzf_Fet_Upd(&sHv_fzf,g_pub_tx.reply);
		if (iRet)
		{
			vtcp_log("%s,%d 读取支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"读取支付交易表错误!");
			WRITEMSG
				return iRet;
		}
		/* 非支付类交易只有收到910才处理,状态时拒绝
		*/
		sHv_fzf.hv_fzf_sts[0]=SEND_REJECT;
		memcpy(sHv_fzf.resp1,hv_notpay->T_resp1,sizeof(sHv_fzf.resp1)-1);
		vtcp_log("%s %d 这里开设更新非支付表的状态[%c]和应答码[%s]",__FILE__,__LINE__,sHv_fzf.hv_fzf_sts[0],sHv_fzf.resp1);
		iRet=Hv_fzf_Upd_Upd(sHv_fzf,g_pub_tx.reply);
		if (iRet)
		{
			vtcp_log("%s,%d 更新支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"更新支付交易表错误");
			WRITEMSG
				return iRet;
		}
	}
	return 0;
}
int iHvChangeLzStatForTh(struct hv_zf_c * hv_zf_wz,char newstat)
{
	struct hv_zf_c hv_zf_lz;
	int iRet=0;
	if (!memcmp(hv_zf_wz->cmtno,HV_TH,3))
	{
		/* 108 交易的253报文要找到原始来帐交易并将交易状态设置为退回状态 */
		memset(&hv_zf_lz, 0 , sizeof(hv_zf_lz));
		memcpy(hv_zf_lz.cmtno,hv_zf_wz->o_cmtno,sizeof(hv_zf_lz.cmtno)-1);
		memcpy(hv_zf_lz.orderno,hv_zf_wz->o_orderno,sizeof(hv_zf_lz.orderno)-1);
		hv_zf_lz.wt_date=hv_zf_wz->o_wt_date;
		memcpy(hv_zf_lz.or_br_no,hv_zf_wz->ac_br_no,sizeof(hv_zf_lz.or_br_no)-1);
		iRet=Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d and cmtno='%s' ",hv_zf_lz.orderno,hv_zf_lz.or_br_no,hv_zf_lz.wt_date,hv_zf_lz.cmtno);
		if (iRet)
		{
			vtcp_log("%s,%d 支付交易表游标定义错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"支付交易表游标定义错误!");
			WRITEMSG
				return iRet;
		}
		iRet=Hv_zf_Fet_Upd(&hv_zf_lz,g_pub_tx.reply);
		if (iRet)
		{
			vtcp_log("%s,%d 读取支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"读取支付交易表错误!");
			WRITEMSG
				return iRet;
		}
		if (newstat!='@')
			hv_zf_lz.hv_sts[0]=newstat;
		else
			hv_zf_lz.hv_sts[0]=hv_zf_lz.beg_sts[0];
		iRet=Hv_zf_Upd_Upd(hv_zf_lz,g_pub_tx.reply);
		if (iRet)
		{
			vtcp_log("%s,%d 更新支付交易表错误 ret=[%d]",__FILE__,__LINE__,iRet);
			vtcp_log(acErrMsg,"更新支付交易表错误");
			WRITEMSG
				return iRet;
		}
	}
	return 0;
}
int iHvChangeHPStatForHPYC(struct hv_zf_c * hv_zf_wz)
{
	HV_121_ADD wd_add;
	int ret=0;
	char cTxday[9],cWtday[9];
	struct hv_poinfo_c hv_poinfo;
	memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
	memset(&wd_add, 0 ,sizeof(wd_add));
	memset(cTxday, 0 , sizeof(cTxday));
	memset(cWtday, 0 , sizeof(cWtday));
	sprintf(cTxday,"%08d",hv_zf_wz->tx_date);
	sprintf(cWtday,"%08d",hv_zf_wz->wt_date);
	/*得到附加域 */
	ret=iHvGetAddit(cTxday,cWtday,hv_zf_wz->addid,"000",(char*)&wd_add,sizeof(wd_add));
	if (ret)
	{
		vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	memcpy(hv_poinfo.po_no,wd_add.pj_num,sizeof(wd_add.pj_num));
	memcpy(hv_poinfo.pay_br_no,hv_zf_wz->pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if (ret)
	{
		sprintf(acErrMsg,"申明游标错误!ret=[%d]\n",ret);
		strcpy(g_pub_tx.reply,"P001");
		WRITEMSG
			return -1;;
	}
	memset(&hv_poinfo, 0 , sizeof(hv_poinfo));  	
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
		strcpy(g_pub_tx.reply,"P001");
		WRITEMSG
			return -1;
	}
	else if (ret==100)
	{
		sprintf(acErrMsg,"查询的记录不存在!ret=[%d]\n",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"P001");
		return -1;
	}
	hv_poinfo.l_po_sts[0] = hv_poinfo.po_sts[0];  
	hv_poinfo.po_sts[0] = '1'; /*汇票移存被拒绝后，将汇票状态置回“签发”状态 */

	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
		return (-1);
	}
	Hv_poinfo_Clo_Upd();
	return 0;
}
/********************************************
*
*准备大额来帐接收帐号
*注意，一定要在定好了机构号的8583域后调用此函数
*
*
*********************************************/
int iHvGetRece_AcValue(THVPACKAGE * pack,char *ac_no,char * name)
{
	char cAc_no[33];
	char cName[61];
	char cReply[5];
	char cKinbr[BRNO_LEN+1];
	char cAcno110[20];
	char cAcno244[20];
	int iRet=0;
	char cTest[33];
	char    cTempstr    [1024];

	memset(cAcno110, 0 , sizeof(cAcno110));
	memset(cTest, 0 , sizeof(cTest));
	memset(cAc_no, 0 ,sizeof(cAc_no));
	memset(cAcno244, 0 , sizeof(cAcno244));
	memset(cName, 0 , sizeof(cName));
	memset(cKinbr, 0  ,sizeof(cKinbr));
	get_zd_data(DC_TX_BR_NO,cKinbr);

	vtcp_log("%s,%d cmtno=%.3s",__FILE__,__LINE__,pack->CMTCode);
	if (!memcmp(pack->CMTCode,"100",3) || !memcmp(pack->CMTCode,"101",3)
		|| !memcmp(pack->CMTCode,"102",3) || !memcmp(pack->CMTCode,"103",3)
		|| !memcmp(pack->CMTCode,"105",3) || !memcmp(pack->CMTCode,"108",3)
		|| !memcmp(pack->CMTCode,"109",3) || !memcmp(pack->CMTCode,"122",3)
		|| !memcmp(pack->CMTCode,"123",3) || !memcmp(pack->CMTCode,"124",3)
		)
	{
		/* 设置默认110帐号 */
		set_zd_data("0660","2");
		set_zd_data("0720","1");
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的110户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("1201",cAcno110);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_data("0720","1");/* 借贷标志 */
		/* 设置默认2440402，2440401 帐号，不用的交易金额设置为0 */
		memset(cAcno244, 0 , sizeof(cAcno244));
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAcno244);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("0320",cAcno244);
		memset(cAcno244, 0 , sizeof(cAcno244));
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAcno244);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("0330",cAcno244);
		set_zd_double("0420",0.0);
		/* 设置来帐过渡科目 */
		set_zd_data("0640",HVLZGD);
		vtcp_log("%s,%d 过渡科目===[%s]",__FILE__,__LINE__,HVLZGD);
	}
	if ( !memcmp(pack->CMTCode,"100",3) || !memcmp(pack->CMTCode,"101",3)
		|| !memcmp(pack->CMTCode,"102",3) || !memcmp(pack->CMTCode,"103",3))
	{
		hv_get_tag(pack,"59C",cAc_no);
		hv_get_tag(pack,"59A",cName);
		vtcp_log("%s,%d ac_no=%.32s",__FILE__,__LINE__,cAc_no);
		vtcp_log("%s,%d name=%.60s",__FILE__,__LINE__,cName);
	}
	else if (!memcmp(pack->CMTCode,"108",3))
	{
		hv_get_tag(pack,"CQ1",cAc_no);
		hv_get_tag(pack,"CR1",cName);
	}
	else if (!memcmp(pack->CMTCode,"122",3))
	{
		hv_get_tag(pack,"59E",cAc_no);
		hv_get_tag(pack,"59D",cName);
	}
	else if (!memcmp(pack->CMTCode,"123",3) || !memcmp(pack->CMTCode,"124",3) )
	{
		char cPo_bal[16];
		char cPo_bal_32a[19];
		double dPo_bal=0;
		char cKinbr[BRNO_LEN+1];
		char cTmp_ac_no[20];
		int iRet=0;
		/* Begin Added by ChenMing  2006-11-14 15:01  */
		char cPay_br_no[13];
		memset(cPay_br_no,0,sizeof(cPay_br_no));
		hv_get_tag(pack,"CC4",cPay_br_no);
		/* End Added by ChenMing  2006-11-14 15:01  */

		memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
		memset(cKinbr, 0 , sizeof(cKinbr));
		get_zd_data(DC_TX_BR_NO,cKinbr);
		memset(cPo_bal, 0 , sizeof(cPo_bal));
		memset(cPo_bal_32a, 0 , sizeof(cPo_bal_32a));
		if (!memcmp(pack->CMTCode,"124",3))
		{
			hv_get_tag(pack,"32A",cPo_bal_32a);
			memcpy(cPo_bal,cPo_bal_32a+3,sizeof(cPo_bal)-1);
		}
		else
			hv_get_tag(pack,"33C",cPo_bal);
		hv_get_tag(pack,"50C",cAc_no);
		hv_get_tag(pack,"50A",cName);
		zip_space(cPo_bal);
		str2dbl(cPo_bal,strlen(cPo_bal),0,&dPo_bal);
		dPo_bal=dPo_bal/100.0;
		vtcp_log("%s,%d pobal===[%f]",__FILE__,__LINE__,dPo_bal);
		/* 设置32,42域，33,43域相关值 */
		if (!memcmp(pack->CMTCode,"123",3))
		{
			/* 30 域交易金额应该是未用退回金额 */
			/* 1 dr 2440401   cr 2440402  出票金额     */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cTmp_ac_no);
			if (iRet)
			{
				vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
				return iRet;
			}
			set_zd_data("0320",cTmp_ac_no);
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cTmp_ac_no);
			if (iRet)
			{
				vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
				return iRet;
			}
			set_zd_data("0330",cTmp_ac_no);
			set_zd_double("0420",dPo_bal);
			/* 2 dr 110       cr cust  未用退回金额30域已经设定 */
		}
		if (!memcmp(pack->CMTCode,"124",3))
		{
			int iYqFlag=0;/* 0未逾期,1逾期 */
			struct hv_poinfo_c sHv_poinfo;
			char cPono[HV_POLEN+1];
			struct mdm_ac_rel_c sMdm_ac_rel;
			struct in_mst_c     sIn_mst;
			memset(&sMdm_ac_rel, 0 ,sizeof(sMdm_ac_rel));
			memset(&sIn_mst , 0 , sizeof(sIn_mst));
			memset(&sHv_poinfo, 0 , sizeof(sHv_poinfo));
			memset(cPono, 0 , sizeof(cPono));
			hv_get_tag(pack,"21A",cPono);
			memcpy(sHv_poinfo.po_no,cPono,HV_POLEN);
			g_reply_int=Hv_poinfo_Sel(g_pub_tx.reply,&sHv_poinfo,"po_no='%s' and pay_br_no='%s' ",cPono, cPay_br_no);
			if (g_reply_int)
			{
				/* 汇票不存在的情况按照逾期处理 */
				iYqFlag=1;
			}
			pub_base_deadlineM(sHv_poinfo.sign_date,1,&sHv_poinfo.pay_date);
			if (sHv_poinfo.pay_date<g_pub_tx.tx_date)
			{
				iYqFlag=1;
			}
			else
			{
				iYqFlag=0;
			}
			/* 32域金额应该是汇票的本金 */
			/* 1 逾期，判断 dr 110  cr 2440403 */
			if (iYqFlag==1)
			{
				iRet=pub_base_GetParm_Str(cKinbr,PARM_HPTH,cTmp_ac_no);
				if (iRet)
				{
					vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
					return iRet;
				}
				memcpy(sMdm_ac_rel.ac_no,cTmp_ac_no,sizeof(sMdm_ac_rel.ac_no)-1);
				g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s' and ac_seqn='0'",cTmp_ac_no);
				if (g_reply_int)
				{
					vtcp_log("%d,%d 错误错误错误 %d",__FILE__,__LINE__,g_reply_int);
				}
				sIn_mst.ac_id=sMdm_ac_rel.ac_id;
				g_reply_int=In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id=%d and ac_seqn=0",sIn_mst.ac_id);
				if (g_reply_int)
				{
					vtcp_log("%d,%d 错误错误错误 %d",__FILE__,__LINE__,g_reply_int);
				}
				memcpy(cName,sIn_mst.name,sizeof(cName)-1);
				set_zd_data("0300",cTmp_ac_no);
				memcpy(cAc_no,cTmp_ac_no,sizeof(cAc_no)-1);
				vtcp_log("%s,%d acno=[%s] name=[%s]",__FILE__,__LINE__,cTmp_ac_no,cName);
				vtcp_log("%s,%d 汇票逾期",__FILE__,__LINE__);
				/*set_zd_double("0420",0.0);   Modified by ChenMing   2006-11-14 12:00 */
				set_zd_double("0420",dPo_bal); /* 不管是否逾期，都走 2440401 2440402 */
				vtcp_log("%s,%d 这里设置汇票签发金额 %f",__FILE__,__LINE__,dPo_bal);
			}
			else
			{
				/* 2,未逾期 dr 110     cr 2440402 */
				/*          dr 2440401 cr cust */
				vtcp_log("%s,%d 汇票未逾期",__FILE__,__LINE__);
				set_zd_double("0420",dPo_bal);
				vtcp_log("%s,%d 这里设置汇票签发金额 %f",__FILE__,__LINE__,dPo_bal);
			}
		}
	}
	else if (!memcmp(pack->CMTCode,"725",3) )
	{
		char cPo_bal[16];
		char cPo_bal_32a[19];
		double dPo_bal=0;
		char cKinbr[BRNO_LEN+1];
		char cTmp_ac_no[20];
		int iRet=0;

		memset(cKinbr, 0 , sizeof(cKinbr));
		get_zd_data(DC_TX_BR_NO,cKinbr);
		vtcp_log("%s,%d 根据帐号得到是 cKinbr===[%s]",__FILE__,__LINE__,cKinbr);		

		memset(cTmp_ac_no, 0 , sizeof(cTmp_ac_no));
		memset(cPo_bal, 0 , sizeof(cPo_bal));
		memset(cPo_bal_32a, 0 , sizeof(cPo_bal_32a));
		hv_get_tag(pack,"33C",cPo_bal);
		hv_get_tag(pack,"50C",cAc_no);
		hv_get_tag(pack,"50A",cName);
		zip_space(cPo_bal);
		str2dbl(cPo_bal,strlen(cPo_bal),0,&dPo_bal);
		dPo_bal=dPo_bal/100.0;
		vtcp_log("%s,%d pobal===[%f]",__FILE__,__LINE__,dPo_bal);

		/* 设置32,42域，33,43域相关值 */
		/* 1 dr 2440401   cr 2440402  出票金额     */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cTmp_ac_no);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("0320",cTmp_ac_no);
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cTmp_ac_no);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("0660","2");
		set_zd_data("0720","1"); 
		memset(cAcno110, 0 , sizeof(cAcno110));
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAcno110);
		if (iRet)
		{
			vtcp_log("%s,%d 没有找到交易机构指定的110户",__FILE__,__LINE__);
			return iRet;
		}
		set_zd_data("1201",cAcno110);
		set_zd_data("1204","01");
		set_zd_data("1205","2");
		set_zd_data("0720","1");/* 借贷标志 */

		set_zd_double("0390",0.0);		/* 30 域交易金额应该为 0  */  
		set_zd_double("0400",0.0);		/* 30 域交易金额应该为 0  */		
		set_zd_data("0330",cTmp_ac_no);
		set_zd_double("0420",dPo_bal);  /* 走帐金额 */
	}
	else if (!memcmp(pack->CMTCode,"105",3) )
	{
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 得到[%s]HVCJ参数错误，直接设置科目2620401",__FILE__,__LINE__,cKinbr);
			memcpy(cAc_no,"2620401",7);
		}
		set_zd_data("0300",cAc_no);
		set_zd_data("0680","6");
		vtcp_log("%s,%d acno=[%s]",__FILE__,__LINE__,cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	else if (!memcmp(pack->CMTCode,"232",3) )
	{
		int  iCrnt=0; /**add by xyy DPGOON**/
		char cQs_br_no[13];
		char cCur_qs_no[13];
		char cOpnbr[BRNO_LEN+1];
		char cName232[61];
		memset(cName232, 0 , sizeof(cName232));
		memset(cOpnbr, 0 , sizeof(cOpnbr));

		memset(cCur_qs_no, 0 , sizeof(cCur_qs_no));
		memset(cQs_br_no , 0 , sizeof(cQs_br_no));
		hv_get_tag(pack,"CC7",cQs_br_no);
		vGet_Hv_Qs_no(cCur_qs_no);
		if (memcmp(cQs_br_no,cCur_qs_no,sizeof(cCur_qs_no)-1))
		{
			/* 得到的被借记行不是本行，那么本行被贷记行 */
			vtcp_log("%s,%d 得到的被借记行不是本行，那么本行被贷记行,重新设置借贷标志为2",__FILE__,__LINE__);
			iCrnt = 54;  /*DPGOON*/
			set_zd_data("0660","2");
			set_zd_data("0720","1");
			/* 本行是被借记行 */
			/* 检查即时转账账户是否是本行账户，
				如果是按账户处理，否则走41433科目控制字 
				by LiuHuafeng 20081203 **/
			hv_get_tag(pack,"59C",cAc_no);
			hv_get_tag(pack,"59A",cName232);
			/* end by LiuHuafeng 2008-12-3 17:55:21 */
		}
		else
		{
			/* 本行是被借记行 */
			/* 检查即时转账账户是否是本行账户，
				如果是按账户处理，否则走41433科目控制字 
				by LiuHuafeng 20081203 **/
			iCrnt = 53;  /*DPGOON*/
			hv_get_tag(pack,"50C",cAc_no);
			hv_get_tag(pack,"50A",cName232);
			/* end by LiuHuafeng 2008-12-3 17:55:27 */
			vtcp_log("%s,%d 得到的被借记行是本行，那么本行被借记行,重新设置借贷标志为1",__FILE__,__LINE__);
			set_zd_data("0660","1");
			set_zd_data("0720","2");
		}
		/*Add by hxc 100203***/
		vtcp_log("%s,%d cAc_no=[%s]",__FILE__,__LINE__,cAc_no);
		if (memcmp(cAc_no,"21600001001",11)==0)
		{
			memset(cAc_no,0,sizeof(cAc_no));
			iRet=pub_base_GetParm_Str("HVCAR",1,cAc_no);/*HVCAR 晋中卡人行来帐232报文 设置记账科目*/
			if (iRet)
			{
				vtcp_log("%s,%d 得到HVCAR参数错误，直接设置科目41404",__FILE__,__LINE__);
				memset(cAc_no,0,sizeof(cAc_no));
				memcpy(cAc_no,"41404",5);
			}
			vtcp_log("%s,%d cAc_no=[%s]",__FILE__,__LINE__,cAc_no);
			/*add by ChengGX 2010-3-4 10:20 是科目号时就不需要检查账户状态和开户机构了*/
			set_zd_data("0300",cAc_no);
			strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
			return 0;	
			/*add by ChengGX 2010-3-4 10:20 是科目号时就不需要检查账户状态和开户机构了*/			
		}
		/*End by hxc 100203***/
		
		vtcp_log("[%s][%d] cAc_no=[%s] ,cOpnbr=[%s] \n",__FILE__,__LINE__,cAc_no,cOpnbr);
		vtcp_log("[%s][%d] cName232=[%s] \n",__FILE__,__LINE__,cName232);
		
		/**检查是否配置了该业务的账号add by xyy 2009-5-6 15:14:47 DPGOON**/
		char cYw_Type[4];
		char cTmpBuf[5];
		char cTmp_Acno[33];
		char cTmp_Name[61];
		memset(cTmpBuf   , 0 , sizeof(cTmpBuf));
		memset(cYw_Type  , 0 , sizeof(cYw_Type));
		memset(cTmp_Acno , 0 , sizeof(cTmp_Acno));
		memset(cTmp_Name , 0 , sizeof(cTmp_Name));
		hv_get_tag(pack,"CEB",cYw_Type);
		vtcp_log("[%s][%d] 注意 cYw_Type=[%s]\n",__FILE__,__LINE__,cYw_Type);
		
		zip_space(cAc_no);
		vtcp_log("[%s][%d] 账号:cAc_no=[%s] ",__FILE__,__LINE__,cAc_no);
		
		/**********************电票的业务类型如下***************
				D01-买断式贴现
				D02-回购式贴现
				D03-回购式贴现赎回
				D04-买断式转贴现
				D05-回购式转贴现
				D06-回购式转贴现赎回
				D07-买断式再贴现
				D08-回购式再贴现
				D09-回购式再贴现赎回
				D10-央行卖出商业汇票
				D11-提示付款
				D12-逾期提示付款
				D13-其他
		**********************************************************/
		if(cYw_Type[0]=='D' && memcmp(cAc_no,"0" , strlen(cAc_no))==0 )
		{
			char cTx_br_no[BRNO_LEN+1];
			char cRe_br_no[13];
			char cTmp_Acno[33];
			char cTmp_Name[61];
			
			memset(cTx_br_no , 0 , sizeof(cTx_br_no));
			memset(cRe_br_no , 0 , sizeof(cRe_br_no));
			memset(cTmp_Acno , 0 , sizeof(cTmp_Acno));
			memset(cTmp_Name , 0 , sizeof(cTmp_Name));
			if(iCrnt == 53 )
			{
				hv_get_tag(pack,"CC4",cRe_br_no);
			}
			else if(iCrnt == 54 )
			{
				hv_get_tag(pack,"CC5",cRe_br_no);
			}
			else
			{
				sprintf(acErrMsg, "得到借贷参数错误[%d]!", iCrnt);
				WRITEMSG
				return -1;
			}
			zip_space(cRe_br_no);
			vtcp_log("[%s][%d] 开户行行号=[%s] [%d]",__FILE__,__LINE__,cRe_br_no,iCrnt);
			
			if(strlen(cRe_br_no)<12)/**如果行号小于12位,那么赋值为清算中心**/
				memcpy(cRe_br_no,HV_QS_BR_NO,sizeof(cRe_br_no)-1);
				
			vtcp_log("[%s][%d] 开户行行号=[%s] [%d]",__FILE__,__LINE__,cRe_br_no,iCrnt);
			iRet = GetBr_noByOr_br_no(cRe_br_no,cTx_br_no);
			if(iRet)
			{
				sprintf(acErrMsg, "根据付款开户行[%s]找不到br_no!", cRe_br_no);
				WRITEMSG
				return iRet;
			}
			vtcp_log("[%s][%d] 对应机构号cTx_br_no=[%s][%s]",__FILE__,__LINE__,cTx_br_no,cYw_Type);
			
			
			/*******注意：这里需要各地根据不同的情况看业务类型为"提示付款"时，是否走应付银行承兑汇票款*******/
			if(memcmp(cYw_Type,"D11",sizeof(cYw_Type)-1)==0)
				iCrnt = 10;
			vtcp_log("[%s][%d] iCrnt=[%d]\n",__FILE__,__LINE__,iCrnt);
			/***************查找com_parm表中各机构对于的账号 **********
					10:应解付银行承兑汇票款
					53:各网点电票贴出的应解汇款专户
					54:各网点电票收到还款时的其他应收款专户
			********************************************************/
			iRet=pub_base_GetParm_Str(cTx_br_no,iCrnt,cTmp_Acno);
			if (iRet)
			{
				sprintf(acErrMsg, "没有找到[%s]机构的账户!", cTx_br_no);
				WRITEMSG
				return iRet;
			}
			else
			{
				zip_space(cTmp_Acno);
				vtcp_log("[%s][%d] cTmp_Acno=[%s]\n",__FILE__,__LINE__,cTmp_Acno);
    	
				iRet= pub_base_acnogetname(cTmp_Acno,cTmp_Name);/**注意:看一下该函数是否存在 DPGOON**/
				if (iRet != 0) {
					sprintf(acErrMsg, "配置的账号[%s]户名不存在![%d]", cTmp_Acno, iRet);
					WRITEMSG
					return iRet;
				}
				memcpy(cAc_no,cTmp_Acno,sizeof(cAc_no)-1);
				memcpy(cName232, cTmp_Name , sizeof(cName232)-1);
			}
			vtcp_log("[%s][%d] cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);
			
		}
		/* begin add by LiuHuafeng 2010-6-28 20:05:44 ADDNEW*/
		else
		{
		/* end by LiuHuafeng 2010-6-28 20:06:02 ADDNEW*/
			memcpy(cTmpBuf , "H" ,1);
			strcat(cTmpBuf , cYw_Type);
			vtcp_log("[%s][%d] cTmpBuf=[%s]\n",__FILE__,__LINE__,cTmpBuf);
			iRet=pub_base_GetParm_Str(cTmpBuf,1,cTmp_Acno);
			if (iRet)
			{
				vtcp_log("[%s][%d]没有配置该[%s]业务种类的账号 ",__FILE__,__LINE__,cYw_Type);
			}
			else
			{
				zip_space(cTmp_Acno);
				vtcp_log("[%s][%d] cTmp_Acno=[%s]\n",__FILE__,__LINE__,cTmp_Acno);
                	
				iRet = pub_base_acnogetname(cTmp_Acno,cTmp_Name);
				if (iRet != 0) {
					sprintf(acErrMsg, "配置的账号[%s]户名不存在![%d]", cTmp_Acno, iRet);
					WRITEMSG
					return iRet;
				}
				memcpy(cAc_no,cTmp_Acno,sizeof(cAc_no)-1);
				memcpy(cName232, cTmp_Name , sizeof(cName232)-1);
			}
			vtcp_log("[%s][%d] cAc_no=[%s]\n",__FILE__,__LINE__,cAc_no);
		/* begin add by LiuHuafeng 2010-6-28 20:06:23 ADDNEW*/
		}
		/* end by LiuHuafeng 2010-6-28 20:06:29  ADDNEW*/
		/*************************end by xyy 2009-5-6 15:15:00  DPGOON*********************/
		
		/* 检查账号状态和开户机构 */
		iRet=iHv_check_ac_no(cAc_no, cName232, cOpnbr);
		if(iRet!=0)
		{
			char cCEB[4];
			char cParm_code[6];
			memset(cParm_code, 0 , sizeof(cParm_code));
			memset(cCEB , 0 , sizeof(cCEB));
			memset(cAc_no, 0 , sizeof(cAc_no));
			/* modify by LiuHuafeng 2009-5-17 20:00:42 for DEZF2.3.2 */
			/* 如果账户识别错误，那么根据报文类型中的交易类型，
			 * 设置清算挂账科目 ,原则是H+CEB中的内容，其内容不足的后补0
			 */
			hv_get_tag(pack,"CEB",cCEB);
			pub_base_strpack(cCEB);
			if(strlen(cCEB)!=3)
			{
				if(strlen(cCEB)==1)
				{
					cCEB[1]='0';
					cCEB[2]='0';
				}
				if(strlen(cCEB)==2)
				{
					cCEB[2]='0';
				}
			}
			cParm_code[0]='H';
			strncpy(cParm_code+1,cCEB,3);
			vtcp_log("%s,%d cParm_code=[%s]",__FILE__,__LINE__,cParm_code);
			vtcp_log("%s,%d cAc_no=[%s]",__FILE__,__LINE__,cAc_no);
			iRet=pub_base_GetParm_Str(cParm_code,1,cAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d 得到%s参数错误，直接设置JSZZ参数",__FILE__,__LINE__,cParm_code);
				iRet=pub_base_GetParm_Str("JSZZ",1,cAc_no);
				if (iRet)
				{
					vtcp_log("%s,%d 得到JSZZ参数错误，直接设置科目2620401",__FILE__,__LINE__);
					memcpy(cAc_no,"2620401",7);
				}
			}
		}
		set_zd_data("0300",cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	else if (!memcmp(pack->CMTCode,"803",3) )
	{
		iRet=pub_base_GetParm_Str("HVTZ",1,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 得到HVTZ参数错误，直接设置科目2620401",__FILE__,__LINE__);
			memcpy(cAc_no,"2620401",7);
		}
		vtcp_log("%s,%d 支付业务透支计息报文 1",__FILE__,__LINE__);
		set_zd_data("0660","1");
		set_zd_data("0720","2");		
		set_zd_data("0300",cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	else if (!memcmp(pack->CMTCode,"802",3) )
	{
		iRet=pub_base_GetParm_Str("HVSF",1,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 得到HVSF参数错误，直接设置科目2620401",__FILE__,__LINE__);
			memcpy(cAc_no,"2620401",7);
		}
		vtcp_log("%s,%d 支付业务收费清单报文 1",__FILE__,__LINE__);
		set_zd_data("0660","1");
		set_zd_data("0720","2");
		set_zd_data("0300",cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	/** dongxy 20060402 增加812和814报文 **/
	/** 注意，需要往com_parm表中增加一条记录 **/
	/** insert into com_parm values('HVQD',1,'大额收费返还','6','51504'); **/
	else if (!memcmp(pack->CMTCode,"812",3) )
	{
		iRet=pub_base_GetParm_Str("HVSF",1,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 得到HVSF参数错误，直接设置科目2620401",__FILE__,__LINE__);
			memcpy(cAc_no,"2620401",7);
		}
		vtcp_log("%s,%d 支付业务收费清单报文 1",__FILE__,__LINE__);
		set_zd_data("0660","1");
		set_zd_data("0720","2");
		set_zd_data("0300",cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	else if (!memcmp(pack->CMTCode,"814",3) )
	{
		iRet=pub_base_GetParm_Str("HVQD",1,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 得到HVQD参数错误，直接设置科目2620315",__FILE__,__LINE__);
			memcpy(cAc_no,"2620315",7);
		}
		vtcp_log("%s,%d 支付业务收费清单报文 1",__FILE__,__LINE__);
		set_zd_data("0660","2");
		set_zd_data("0720","1");
		set_zd_data("0300",cAc_no);
		strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
		return 0;
	}
	pub_base_old_acno(cAc_no);
	memcpy(ac_no,cAc_no,sizeof(cAc_no)-1);
	memcpy(name,cName,sizeof(cName)-1);
	vtcp_log("[%s][%d] name==[%.60s] ",__FILE__,__LINE__,name); 
	memset(cTempstr,0,sizeof(cTempstr));
	strncpy(cTempstr, name, 60);
	dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
	strncpy(name, cTempstr, 60);
	vtcp_log("[%s][%d] name==[%.60s] ",__FILE__,__LINE__,name); 

	vtcp_log("%s,%d ac_no=%.32s",__FILE__,__LINE__,cAc_no);
	vtcp_log("%s,%d name=%.60s",__FILE__,__LINE__,cName);
	set_zd_data("0300",cAc_no);
	strncpy(ac_no,cAc_no,sizeof(cAc_no)-1);
	return 0;
}
int iHvCheckPoMoveToCenter(struct hv_poinfo_c *phv_poinfo,char * errmsg,char * reply)
{
	long l_endday=0;
	if (phv_poinfo->po_sts[0]!=HV_HP_SIGN)
	{
		strncpy(errmsg,"汇票状态错误，应该是[1]!",50);
		set_zd_data("0130",errmsg);
		WRITEMSG 
			strcpy(reply,"P002");
		vtcp_log("%s,%d 汇票状态错误，应该是[1]",__FILE__,__LINE__);
		return -1;
	}
	if (phv_poinfo->bank_type[0]!='0')
	{
		strncpy(errmsg,"非本行的汇票不可以移存\n",50);
		set_zd_data("0130",errmsg);
		WRITEMSG
			strcpy(reply,"P000");
		vtcp_log("%s,%d 非本行的汇票不可以移存",__FILE__,__LINE__);
		return -1;
	}
	if (phv_poinfo->add_pwd_ind[0]=='0')
	{
		strncpy(errmsg,"请先进行核押处理!",40);
		set_zd_data("0130",errmsg);
		WRITEMSG
			strcpy(reply,"P037");
		vtcp_log("%s,%d 请先进行核押处理",__FILE__,__LINE__);
		return -1;
	}
	/*********************
	* 检查汇票是否到期   *
	**********************/
	pub_base_deadlineM( g_pub_tx.tx_date, 1, &l_endday);
	if (g_pub_tx.tx_date-l_endday>0)
	{
		vtcp_log("[%s][%d] tx_date[%d]pay_date=[%d] \n",__FILE__,__LINE__,g_pub_tx.tx_date,l_endday);
		strncpy(errmsg,"汇票已经到期不可以签发!",40);
		set_zd_data("0130",errmsg);
		strcpy(reply,"P038");
		WRITEMSG
			return -1;
	}
	return 0;
}
int iCheckHvpoinfo(HV_PAYIN_AREA *hv_payin,struct hv_poinfo_c *pHv_poinfo,HV_121_ADD * wp_add,char * errmsg)
{
	double dTxamt;
	dTxamt=0.0;	
	dot_str2dbl(hv_payin->T_tx_amt,sizeof(hv_payin->T_tx_amt),&dTxamt);
	if (fabs(dTxamt-pHv_poinfo->po_bal)>0.001)
	{
		vtcp_log("[%s][%d]输入的出票金额与签发的汇票不一致,%f,%f \n",__FILE__,__LINE__,dTxamt,pHv_poinfo->po_bal);
		WRITEMSG
			strncpy(errmsg,"输入的出票金额与签发的汇票不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_opn_br_no,pHv_poinfo->pay_br_no, \
		strlen(pHv_poinfo->pay_br_no)))
	{
		vtcp_log("[%s][%d]输入的付款人开户机构与汇票的开户行不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"输入的付款人开户机构与汇票的开户行不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_opn_br_no,pHv_poinfo->cash_br_no, \
		sizeof(hv_payin->T_cash_opn_br_no)))
	{
		vtcp_log("[%s][%d]兑付行与签发的兑付行不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"兑付行与签发的兑付行不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_ac_no,pHv_poinfo->pay_ac_no, \
		sizeof(hv_payin->T_pay_ac_no)))
	{
		vtcp_log("[%s][%d]付款人帐号与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"付款人帐号与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_name,pHv_poinfo->pay_name, \
		strlen(pHv_poinfo->pay_name)))
	{
		vtcp_log("[%s][%d]付款人户名与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"付款人户名与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_pay_addr,pHv_poinfo->pay_addr, \
		sizeof(hv_payin->T_pay_addr)))
	{
		vtcp_log("[%s][%d]付款人地址与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"付款人地址与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_cash_name,pHv_poinfo->cash_name, \
		strlen(pHv_poinfo->cash_name)))
	{
		vtcp_log("[%s][%d]收款人户名与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"收款人户名与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (wp_add->hp_type[0]!=pHv_poinfo->po_ind[0])
	{
		vtcp_log("[%s][%d]汇票类别与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"汇票类别与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_cur_no,pHv_poinfo->cur_id,sizeof(pHv_poinfo->cur_id)-1))
	{
		vtcp_log("[%s][%d]币种[%.3s]与签发[%.3s]的不一致 \n",__FILE__,__LINE__,hv_payin->T_cur_no,pHv_poinfo->cur_id);
		WRITEMSG
			strncpy(errmsg,"币种与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	if (memcmp(hv_payin->T_hv_brf,pHv_poinfo->brf,strlen(pHv_poinfo->brf)))
	{
		vtcp_log("[%s][%d]附言与签发的不一致 \n",__FILE__,__LINE__);
		WRITEMSG
			strncpy(errmsg,"附言与签发的不一致!",40);
		set_zd_data("0130",errmsg);
		return -1;
	}
	return 0;
}
int iHv121Proc(HV_PAYIN_AREA *hv_payin)
{
	HV_121_ADD wd_add;
	int ret=0;
	struct hv_poinfo_c hv_poinfo;
	memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
	memset(&wd_add, 0 ,sizeof(wd_add));
	/*得到附加域 */
	ret=iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&wd_add,sizeof(wd_add));
	if (ret)
	{
		vtcp_log("%s,%d 读取附加域错误 %d",__FILE__,__LINE__,ret);
		strncpy(acErrMsg,"处理小额附加域错误",50);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return -1;
	}
	memcpy(hv_poinfo.po_no,wd_add.pj_num,sizeof(wd_add.pj_num));
	memcpy(hv_poinfo.pay_br_no,hv_payin->T_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s'  and  pay_br_no='%s'",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if (ret)
	{
		sprintf(acErrMsg,"申明游标错误!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		strcpy(g_pub_tx.reply,"P001");
		WRITEMSG
			return -1;;
	}
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if (ret&&ret!=100)
	{
		sprintf(acErrMsg,"取游标错误!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		strcpy(g_pub_tx.reply,"P001");
		WRITEMSG
			return -1;
	}
	else if (ret==100)
	{
		sprintf(acErrMsg,"查询的记录不存在!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			strcpy(g_pub_tx.reply,"P001");
		return -1;
	}
	ret=iHvCheckPoMoveToCenter(&hv_poinfo,acErrMsg,g_pub_tx.reply);
	if (ret)
	{
		vtcp_log("%s,%d 检查汇票基本信息出现错误",__FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			return -1;
	}
	ret=iCheckHvpoinfo(hv_payin,&hv_poinfo,&wd_add,acErrMsg);
	if (ret)
	{
		vtcp_log("%s,%d 对比输入信息与汇票基本信息出现错误",__FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
			strcpy(g_pub_tx.reply,"P001");
		return -1;
	}
	get_zd_long("0050",&hv_poinfo.reg_date);
	hv_poinfo.l_po_sts[0]=hv_poinfo.po_sts[0];
	hv_poinfo.po_sts[0]=HV_HP_MV_CENTER;
	if (hv_poinfo.l_tx_date!=hv_poinfo.reg_date)
	{
		get_zd_long("0050",&hv_poinfo.l_tx_date);
		hv_poinfo.td_cnt=1;
	}
	else
		hv_poinfo.td_cnt++;
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if (ret)
	{
		sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
		WRITEMSG
			strcpy(g_pub_tx.reply,"D105");
		return (-1);
	}
	memcpy(g_pub_tx.beg_note_no,hv_poinfo.po_no,sizeof(g_pub_tx.beg_note_no));
	get_fd_data(DC_TRACE_NO,hv_payin->T_chk_trace_no);
	memcpy(hv_payin->T_chk,g_pub_tx.tel,sizeof(hv_payin->T_chk));

	return 0;
}
int iHvSetRejectAndCancelAc_no(struct hv_zf_c *pHv_zf)
{
	char cKinbr[BRNO_LEN+1];
	int iRet;
	char cAc_no[20];
	memset(cAc_no, 0 , sizeof(cAc_no));
	memset(cKinbr, 0 , sizeof(cKinbr));

	vtcp_log("[%s][%d] 3333   到这里了 !!  pHv_zf->br_no=[%s] ",__FILE__,__LINE__,pHv_zf->br_no);  	

	memcpy(cKinbr,pHv_zf->br_no,BRNO_LEN);
	vtcp_log("[%s][%d] 3333   到这里了 !!  cKinbr=[%s] ",__FILE__,__LINE__,cKinbr);  	
	set_zd_data(DC_TX_BR_NO,pHv_zf->br_no);/* 这里重新设置机构号和操作员 */
	iGetReceTel(pHv_zf->br_no);
	if (memcmp(pHv_zf->cmtno,"121",3)==0)
	{
		vtcp_log("[%s][%d] 121   到这里了 !!  ",__FILE__,__LINE__);  

		/* dr 2440402 cr 1100000 */
		iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 系统配置错误，得到机构%s的汇票移存帐号错误[%d]",__FILE__,__LINE__,cKinbr,iRet);
			strncpy(acErrMsg,"得到本机构汇票移存账号错误",50);
			set_zd_data("0130",acErrMsg);
			return iRet;
		}
	}
	else if (memcmp(pHv_zf->cmtno,HV_TH,3)==0)
	{
		vtcp_log("[%s][%d] HV_TH   到这里了 !!  ",__FILE__,__LINE__);  

		if (pHv_zf->hv_osts[0]!=RECEIVE_ACCT_QS)
		{
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d 系统配置错误，得到机构%s的挂账帐号错误[%d]",__FILE__,__LINE__,cKinbr,iRet);
				strncpy(acErrMsg,"得到本机构汇票挂账账号错误",50);
				set_zd_data("0130",acErrMsg);
				return iRet;
			}
		}
		else
		{
			memcpy(cAc_no,pHv_zf->pay_ac_no,sizeof(cAc_no)-1);
		}
	}
	else if (memcmp(pHv_zf->cmtno,HV_CJ,3)==0)
	{
		vtcp_log("[%s][%d] HV_CJ   到这里了 !!  ",__FILE__,__LINE__);  

		iRet=pub_base_GetParm_Str(cKinbr,PARM_HVCJ,cAc_no);
		if (iRet)
		{
			vtcp_log("%s,%d 系统配置错误，得到机构%s的挂账帐号错误[%d]",__FILE__,__LINE__,iRet);
			strncpy(acErrMsg,"得到本机构汇票拆借账号错误",50);
			set_zd_data("0130",acErrMsg);
			return iRet;
		}
	}
	else
	{
		vtcp_log("[%s][%d] else   到这里了 !!  ",__FILE__,__LINE__);
		strncpy(cAc_no,pHv_zf->pay_ac_no,sizeof(cAc_no)-1);
		if (strlen(cAc_no)==5 &&	!memcmp(cAc_no,"10101",5))
		{
			/* 现金科目的往帐业务要拒绝到挂账科目上 */
			iRet=pub_base_GetParm_Str(cKinbr,PARM_HVGZ,cAc_no);
			if (iRet)
			{
				vtcp_log("%s,%d 系统配置错误，得到机构%s的挂账帐号错误[%d]",__FILE__,__LINE__,iRet);
				strncpy(acErrMsg,"得到本机构挂账账号错误",50);
				set_zd_data("0130",acErrMsg);
				return iRet;
			}
		}
	}
	vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  

	/* 设置0300帐号 */
	pub_base_old_acno(cAc_no);
	set_zd_data("0300",cAc_no);
	g_reply_int=iGetSeqn(cAc_no);
	if (g_reply_int==6) set_zd_data("0680","6");
	if (g_reply_int==1) set_zd_data("0680","1");
	g_reply_int=0;
	/* 设置110帐号 */
	memset(cAc_no, 0 , sizeof(cAc_no));
	vtcp_log("[%s][%d]  到这里了 !!  ",__FILE__,__LINE__);  

	iRet=pub_base_GetParm_Str(cKinbr,PARM_HVQS,cAc_no);
	if (iRet)
	{
		vtcp_log("%s,%d 系统配置错误，得到机构%s的110帐号错误[%d]",__FILE__,__LINE__,cKinbr,iRet);
		strncpy(acErrMsg,"得到本机构清算账号错误",50);
		set_zd_data("0130",acErrMsg);
		return -1;
	}
	vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  

	set_zd_data("1201",cAc_no);
	set_zd_data("0720","2");/* 设置借贷标志2 */

	/* 设置来帐的2440402，2440401帐号和余额，一定为0 否则会走帐 */
	memset(cAc_no, 0 , sizeof(cAc_no));
	iRet=pub_base_GetParm_Str(cKinbr,PARM_HPQF,cAc_no);
	if (iRet)
	{
		vtcp_log("%s,%d 没有找到交易机构指定的汇票签发户",__FILE__,__LINE__);
		strncpy(acErrMsg,"没有找到交易机构的汇票签发户",50);
		set_zd_data("0130",acErrMsg);
		return -1;
	}
	vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  
	set_zd_data("0320",cAc_no);

	memset(cAc_no, 0 , sizeof(cAc_no));
	iRet=pub_base_GetParm_Str(cKinbr,PARM_HPYC,cAc_no);
	if (iRet)
	{
		vtcp_log("%s,%d 没有找到交易机构指定的汇票移存户",__FILE__,__LINE__);
		strncpy(acErrMsg,"没有找到交易机构的汇票移存户",50);
		set_zd_data("0130",acErrMsg);
		return -1;
	}
	vtcp_log("[%s][%d]   cAc_no===[%s]  ",__FILE__,__LINE__,cAc_no);  

	set_zd_data("0330",cAc_no);
	set_zd_double("0420",0.0);
	set_zd_data("0640",HVWZGD);
	return 0;
}
int iGetSeqn(char * ac_no)
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct prdt_ac_id_c sPrdt_ac_id;
	memset(&sMdm_ac_rel, 0 , sizeof(sMdm_ac_rel));
	memset(&sPrdt_ac_id, 0 , sizeof(sPrdt_ac_id));
	memcpy(sMdm_ac_rel.ac_no,ac_no,sizeof(sMdm_ac_rel.ac_no)-1);
	g_reply_int=Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
	if (g_reply_int)
	{
		return 6;
	}
	if (sMdm_ac_rel.ac_seqn==9999) return 1;
	if (sMdm_ac_rel.ac_seqn==0) return 6;
	if (sMdm_ac_rel.ac_seqn==1) return 1;
}
int iHvFindOld311ByHvpack(THVPACKAGE * phv_pack, struct hv_fzf_c * sHv_fzf )
{
	int  position = 69;
	int  ret = 0;
    int  iOrderno=0;
	char cOr_br_no[13];
	char cWt_date[9];
	char cOrderno[9];     
    char cTempBuff[9]; /****add by xyy 2007-04-09****/
	char cmtno   [4 ];   
	HV_PAYIN_AREA hv_payin;
	HV_NOTPAY_AREA hv_notpay;    
	int zfflg = 0;

	memset(cmtno,0,sizeof(cmtno));
    memset(cTempBuff,0,sizeof(cTempBuff));

	/* 分析来帐报文    2. 分析来帐报文业务头块 */      
	ret = split_ophead(phv_pack,position); 
	position = 93;    
	if ( ret == 100 )  /*没有业务头块，直接 split_body */
	{
		position = 69;
	} 
	else if ( ret != 0 ) 
	{
		vtcp_log("[%s][%d] split_ophead 分析来帐报文业务头块错误",__FILE__,__LINE__);
		sprintf(acErrMsg," 大额来帐报文业务头块分析错误 ! ");
		MBFEWRITEMSG
			return -1;        
	}
	/* 分析来帐报文    3. 分析来帐报文正文块块标识 */  
	vtcp_log("[%s][%d] ret=[%d]  position=[%d] ",__FILE__,__LINE__,ret,position);               
	if ( split_body(phv_pack,position) )
	{
		vtcp_log("[%s][%d] split_body 分析来帐报文正文块块标识错误",__FILE__,__LINE__);
		sprintf(acErrMsg," 大额来帐报文正文块块标识分析错误 !  ");
		MBFEWRITEMSG
			return -1;        
	}    
	memcpy(cmtno,phv_pack->CMTCode,3);  
	vtcp_log("[%s][%d]hv_pack.CMTCode=[%-3s]",__FILE__,__LINE__,cmtno);

	vtcp_log("[%s][%d]split_body() 完毕 !! ",__FILE__,__LINE__);
	memset(&hv_payin,0x00,sizeof(HV_PAYIN_AREA));        
	memset(&hv_notpay,0x00,sizeof(HV_NOTPAY_AREA));       

	/* 根据cmtno，由 hv_pack 设置 HV_PAYIN_AREA/HV_NOTPAY_AREA的T字段   */      
	ret = pub_hv_set_AREA(phv_pack, cmtno, &zfflg, &hv_payin, &hv_notpay); 
	if (ret<0)
	{
		sprintf(acErrMsg,"设置相应的AREA的T字段时出错! ");
		MBFEWRITEMSG
			return -1;        
	}
	vtcp_log("[%s][%d]pub_hv_set_AREA() 完毕 !! ",__FILE__,__LINE__);

	memset(cOr_br_no, 0, sizeof(cOr_br_no));
	memset(cWt_date, 0, sizeof(cWt_date));   
	memset(cOrderno, 0, sizeof(cOrderno));            

	memcpy( cOr_br_no,hv_notpay.T_ac_br_no,sizeof(cOr_br_no)-1 ); 
	memcpy( cWt_date,hv_notpay.T_wt_date,sizeof(cWt_date)-1 );     
	memcpy( cOrderno,hv_notpay.T_req_orderno,sizeof(cOrderno)-1 );  

	vtcp_log("[%s][%d] cOr_br_no=[%s] ",__FILE__,__LINE__, cOr_br_no);  
	vtcp_log("[%s][%d] cWt_date=[%s] ",__FILE__,__LINE__, cWt_date);     
	vtcp_log("[%s][%d] cOrderno=[%s] ",__FILE__,__LINE__, cOrderno);    
    zip_space(cOrderno);
    iOrderno = apatoi(cOrderno,8);
	vtcp_log("[%s][%d] cOrderno=[%d] ",__FILE__,__LINE__,iOrderno);    
    sprintf(cTempBuff,"%03d",iOrderno);
    zip_space(cTempBuff);
	vtcp_log("[%s][%d] cTempBuff=[%s] ",__FILE__,__LINE__,cTempBuff);    

	ret = Hv_fzf_Sel(g_pub_tx.reply, sHv_fzf,"otype='1' and or_br_no='%s' and wt_date=%ld and orderno like '%%%s' and cmtno='%s'", 
		cOr_br_no, apatoi(cWt_date,8), cTempBuff, "311");
	if (ret == 100)
	{
		vtcp_log("[%s][%d] 没有找到对应的 311 报文 !!! ",__FILE__,__LINE__);      
		sprintf(acErrMsg,"非支付交易表中不存在该记录!!");
		WRITEMSG
			return ret;
	}else if (ret != 0)
	{
		sprintf(acErrMsg,"数据库错误 !! [%d]",ret);
		WRITEMSG
			return ret;
	}
	if (sHv_fzf->hv_fzf_sts[0] != SEND_ACCT_WAIT) /*往账发送(发送)*/
	{
		vtcp_log("[%s][%d] 原撤销申请状态不正确 !!! ",__FILE__,__LINE__);          	
		sprintf(acErrMsg," 原撤销申请状态不正确 !!");
		WRITEMSG
			return -1;
	}
	vtcp_log("[%s][%d] 找到了对应的 311 报文 !!! ",__FILE__,__LINE__);    	
	return 0; 	
}
/*** begin add by LiuHuafeng 2008-12-3 20:58:26 for CDCB CMT232 */
/**************************
* 函数名称: iHv_check_ac_no                                                 *
*账户的基本检查
*
* 1- 介质状态错误
* 2- 账户开户机构不存在
* 4- 账户id状态不正常
* 5- 账户被冻结
* 6- 账户非个人结算户
* 7- 账户户名不服
* -1 新旧账户不存在
* -2 开户机构状态错误
* -3 账户非内部账户或活期户
* -4 数据库操作失败
* LiuHuafeng 2008-12-3 20:50:37
**************************/
int iHv_check_ac_no(char* ac_no, char* ac_name, char* cKinbr)
{
	int ret=0;
	struct com_branch_c sCom_branch;
	char cQs_br_no[BRNO_LEN+1];
	char  acc_v[33];
	struct mdm_ac_rel_c mdm_ac_rel_v;
	struct prdt_ac_id_c prdt_ac_id_v;
	struct dd_mst_c dd_mst_v;
	int   ac_seqn_v;
	char    cTempstr    [1024];
	
	
	char cReply[5];
	memset(cReply , 0 , sizeof(cReply));
	
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));
	memset(acc_v,0x00,sizeof(acc_v));
	memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	
	vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
	memset(cTempstr,0,sizeof(cTempstr));
	memcpy(cTempstr, ac_name, 60);
	dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
	memcpy(ac_name, cTempstr, 60);
	vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
	
	/* ========= 收款人账号检查开始  ==========*/
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
	memcpy(acc_v, ac_no ,sizeof(acc_v)-1);
	
	/*旧账号转新账号*/
	if (pub_base_old_acno(acc_v))
	{
		sprintf(acErrMsg,"pub_base_old_acno() 出错!! [%d]",__LINE__);
		WRITEMSG
		return -1;
	}
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
	
	memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
	g_reply_int = Mdm_ac_rel_Sel(cReply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
	if (g_reply_int==100)   /* 1. 帐号不存时 */
	{
		vtcp_log("%s,%d 账户不存在 需要挂账处理",__FILE__,__LINE__);
		return -1;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"[%s]数据库操作失败需要挂账处理[%d]",acc_v,g_reply_int);
		WRITEMSG
		return -1;
	}
	vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);
	
	/* 2. 帐号存时，查找帐号的开户机构  机构号以帐号的开户机构为准    */
	ret = iHv_check_brno(mdm_ac_rel_v.opn_br_no);
	if (ret == 100)
	{
		sprintf(acErrMsg,"[%s][%d]账户开户机构不存在",acc_v,ret);
		WRITEMSG
		return 2;
	}
	else  if (ret !=0)
	{
		sprintf(acErrMsg,"[%s][%d]账户开户机构状态错误",acc_v,ret);
		WRITEMSG
		return -2;
	}
	/* ============= 以下机构号已确定 ============= */
	/*  判断介质状态 */
	if (mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' )
	{
		/* 介质状态不正常挂帐处理 */
		vtcp_log("%s,%d  账号[%s]介质状态错误  ",__FILE__,__LINE__,acc_v);
		return 1;
	}
	/*  是否活期存款户 */
	memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
	g_reply_int = Prdt_ac_id_Sel( cReply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')",
		mdm_ac_rel_v.ac_id );
	if (g_reply_int==100)  /* 非活期存款户 */
	{
		/* 账户非活期、内部账户 */
		vtcp_log("%s %d 账户非活期和内部账户",__FILE__,__LINE__);
		return -3;
	}
	else if (g_reply_int)
	{
		sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
		WRITEMSG
		return -4;
	}
	/* 内部帐户不检查户名 */
	if (prdt_ac_id_v.ac_id_type[0]=='1')
	{
		ac_seqn_v  =  1;
		g_reply_int = Dd_mst_Sel(cReply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
		if (g_reply_int )
		{
			sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,
			    mdm_ac_rel_v.ac_id,ac_seqn_v);
			WRITEMSG
			return -4;
		}
		if (dd_mst_v.ac_sts[0] != '1' )
		{
			sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
			WRITEMSG
			return 4;
		}
		/*  判断是否被冻结  */
		if (dd_mst_v.hold_sts[0]=='1' )
		{
			/* 帐号id被完全冻结所以要挂帐 */
			vtcp_log("%s,%d 账户被冻结",__FILE__,__LINE__);
			return 5;
		}
		/*  判断是否是结算户 */
		if (dd_mst_v.sttl_type[0]=='S')
		{
			sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
			WRITEMSG
			return 6;
		}
		pub_base_strpack(dd_mst_v.name);
		pub_base_strpack(ac_name);
		
		vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, dd_mst_v.name, sizeof(dd_mst_v.name)-1);
		dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
		memcpy(dd_mst_v.name, cTempstr, sizeof(dd_mst_v.name)-1);
		vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name);
		
		vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, ac_name, 60);
		dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
		memcpy(ac_name, cTempstr, 60);
		vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
		
		if (strcmp(dd_mst_v.name,ac_name))
		{
			sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,
			    ac_name);
			WRITEMSG
			return 7;
		}
		/* 设置68域 */
		set_zd_data("0680","1");

	}
	else /* 检查内部帐户 */
	{
		struct in_mst_c sIn_mst;
		memset(&sIn_mst,0,sizeof(sIn_mst));
		
		g_reply_int = In_mst_Sel(cReply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
		if (g_reply_int)
		{
			sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,
			    mdm_ac_rel_v.ac_id);
			WRITEMSG
			return -1;
		}
		if (sIn_mst.sts[0] != '1' )
		{
			sprintf(acErrMsg,"大额来账收款人账号状态不正常 挂帐! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
			WRITEMSG
			return 4;
		}
		/* 设置68域 */
		set_zd_data("0680","9");
	}
	vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
	return 0;
}


int iHv_check_brno( char* cOpn_br_no)
{
	int  ret;
	char stat;
	char cReply[5];
	memset(cReply, 0 , sizeof(cReply));
	struct com_branch_c sCom_branch;
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memcpy(sCom_branch.br_no,cOpn_br_no,BRNO_LEN);
	pub_base_strpack(sCom_branch.br_no);
	if(strlen(sCom_branch.br_no)>0)/* 有开户行，则以帐号的开户行为准 */	
	{
		ret=Com_branch_Sel(cReply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
		if (ret == 100)  /* 开户行不存在 */
		{
			vtcp_log("%s,%d 机构号不存在 \n",__FILE__,__LINE__);
			return ret;
		}
		else if (ret)
		{
			sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
			WRITEMSG
			return ret;
		}
		if (sCom_branch.wrk_sts[0]=='*')
		{
			vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
			return 1;
		}
	}
	return 0;
}
/*** end by LiuHuafeng 2008-12-3 20:58:36 for CDCB CMT232***/
/******************************************************
*
* 函数名：根据账号查找户名
* xyy   2009-5-6 16:52:56
*
*****************************************************/
int pub_base_acnogetname( char * ac_no ,char *name )
{
	int ret = 0;
	struct dd_mst_c dd_mst_c;
	struct td_mst_c td_mst_c;
	struct in_mst_c in_mst_c;
	struct com_item_c com_item_c;
	struct mdm_ac_rel_c sMdm_ac_rel;
	
	memset(&td_mst_c   ,0,sizeof(td_mst_c));
	memset(&dd_mst_c   ,0,sizeof(dd_mst_c));
	memset(&in_mst_c   ,0,sizeof(in_mst_c));
	memset(&com_item_c ,0,sizeof(com_item_c));
	memset(&sMdm_ac_rel,0,sizeof(struct mdm_ac_rel_c));
	
	zip_space(ac_no);
	
	if(strlen(ac_no)==4 || strlen(ac_no)==6 || strlen(ac_no)==7)
	{
		ret=Com_item_Sel( g_pub_tx.reply,&com_item_c,"acc_no='%s'",ac_no);
		if( ret ) {
			vtcp_log("%s,%d,账户不存在!",__FILE__,__LINE__);
		}
		else
		{
			strcpy( name,com_item_c.acc_name );
		}
		return 0; 
	}

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",ac_no); 
	if(ret)
	{
		strcpy(g_pub_tx.reply,"D104");
		vtcp_log("%s,%d,账户不存在!",__FILE__,__LINE__);
		return(-1);
	}
	TRACE
	ret=Dd_mst_Sel( g_pub_tx.reply,&dd_mst_c,"ac_id=%d and ac_seqn=1",sMdm_ac_rel.ac_id );
	if( ret )
	{
		TRACE
		ret=Td_mst_Sel( g_pub_tx.reply,&td_mst_c,"ac_id=%d",sMdm_ac_rel.ac_id );
		if( ret )
		{
			TRACE
			ret=In_mst_Sel( g_pub_tx.reply,&in_mst_c,"ac_id=%d",sMdm_ac_rel.ac_id );
			if( ret )
			{
				TRACE
				strcpy( g_pub_tx.reply,"W011" );
				return ret;
			}
			else
			{
				strcpy( name,in_mst_c.name );
			}
		}
		else
		{
			strcpy( name,td_mst_c.name );
		}
	}
	else
	{
		strcpy( name,dd_mst_c.name );
	}

	return (0);
}