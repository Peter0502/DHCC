#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include "find_debug.h"
#define EXTERN
/**
#include "mbfe_pub.h"
#include "public.h"
#include "hv_pub.h"
#include "hvnet.h"
#include "errlog.h"
***/
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "lv_define.h"
#include "prdt_ac_id_c.h"
#include "hv_poinfo_c.h"

#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
int iGetHvOrderno(char * orderno);
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);

/****************************************************************************/
/* 函数名称: iGetSendco(char * or_br_no,char * sendco)                      */
/* 功能描述: 根据支付系统行号得到(发报/收报)中心代号                        */
/* 创建日期: 2006-8-12 19:22                                                */
/* 作者:     ChenMing                                                       */
/****************************************************************************/ 
int iGetSendco( char *sendco, char *or_br_no)
{
	int ret=0;
	char cOr_br_no[13];
	struct hv_uniontab_c hv_uniontab; 
	memset(cOr_br_no , 0, sizeof(cOr_br_no));
	memset( &hv_uniontab, 0x00, sizeof(hv_uniontab) );   
	
	memcpy(cOr_br_no, or_br_no, sizeof(cOr_br_no)-1);

	ret = Hv_uniontab_Sel( g_pub_tx.reply, &hv_uniontab, "or_br_no = '%s' ", cOr_br_no );
	if(ret)
	{
		sprintf(acErrMsg,"find hvuniontab  ret= [%d]",ret);
		WRITEMSG
		return(ret);
	}
	memcpy(sendco,hv_uniontab.sendco,sizeof(hv_uniontab.sendco)-1);	
	return 0;
}

/****************************************************************************/
/* 函数名称: iGet_full_hv_no(char * or_br_no,char * sendco,char * qs_br_no) */
/* 功能描述: 根据支付系统行号得到(发报/收报)中心代号                        */
/* 创建日期: 2006-8-12 19:22                                                */
/* 作者:     ChenMing                                                       */
/****************************************************************************/ 
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no)
{
	int ret=0;
	struct hv_uniontab_c hv_uniontab;    
	char cOrbrno[13];
	memset(cOrbrno, 0 , sizeof(cOrbrno));
	memcpy(cOrbrno,or_br_no,sizeof(cOrbrno)-1);
	zip_space(cOrbrno);
	if  (strlen(cOrbrno) == 0 )
		return 1;
	memset( &hv_uniontab, 0x00, sizeof(hv_uniontab) );    
	ret = Hv_uniontab_Sel( g_pub_tx.reply, &hv_uniontab, "or_br_no = '%s' ", cOrbrno );
	if(ret)
	{
		sprintf(acErrMsg,"find hvuniontab error ret=[%d] or_brno=[%s]",ret,or_br_no);
		WRITEMSG
		return(ret);
	}
	memcpy(sendco,hv_uniontab.sendco,sizeof(hv_uniontab.sendco)-1);
	memcpy(qs_br_no,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1);		
	sprintf(acErrMsg,"sendco=[%s] or_brno=[%s]",hv_uniontab.sendco,hv_uniontab.qs_no);
	WRITEMSG
	sprintf(acErrMsg,"sendco=[%s] or_brno=[%s]",sendco,qs_br_no);
	WRITEMSG
	return ret;
}

/****************************************************************************/
/* 函数名称: int GetOr_br_noByBr_no(char *br_no,char *or_br_no)             */
/* 功能描述: 根据 机构号码 查找 支付系统行号                                */
/* 创建日期: 2006.08.08                                                     */
/* 作者:     ChenMing                                                       */
/* 说明:     根据 机构号码 查找 支付系统行号                                */
/****************************************************************************/ 
int GetOr_br_noByBr_no(char *br_no, char *or_br_no )
{
  int ret=0;
	char cBrno[BRNO_LEN+1];
	struct hv_brno_orno_c hv_brno_orno;
	memset(cBrno, 0 , sizeof(cBrno));
	memset( &hv_brno_orno, 0x00, sizeof(hv_brno_orno) );  
	memcpy(cBrno,br_no,BRNO_LEN);
	zip_space(cBrno);
  ret = Hv_brno_orno_Sel( g_pub_tx.reply, &hv_brno_orno, "br_no = '%s' ", cBrno );
  if ( ret!=0  && ret!=100)
	{
		sprintf(acErrMsg,"查找hv_brno_orno出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return(ret);
	}	
	memcpy(or_br_no,hv_brno_orno.or_br_no,sizeof(hv_brno_orno.or_br_no)-1);
  return 0;
}
/****************************************************************************/
/* 函数名称: int GetBr_noByOr_br_no(char *br_no, char *or_br_no )           */
/* 功能描述: 根据 支付系统行号 查找 机构号码                                */
/* 创建日期: 2006.08.08                                                     */
/* 作者:     ChenMing                                                       */
/* 说明:     根据 支付系统行号 查找 机构号码                                */
/****************************************************************************/ 
int GetBr_noByOr_br_no(char *or_br_no, char *br_no )
{
    int ret=0;
    char cOr_br_no[13];
    struct hv_orno_brno_c hv_orno_brno;   
    memset( cOr_br_no,0,sizeof(cOr_br_no));
    memset( &hv_orno_brno, 0x00, sizeof(hv_orno_brno) );        
    
    memcpy(cOr_br_no,or_br_no, sizeof(cOr_br_no)-1);
    zip_space(cOr_br_no);
    /* add by LiuHuafeng 20070501 */
    if(strlen(cOr_br_no)==0)
    {
        memcpy(cOr_br_no,HV_QS_BR_NO,sizeof(cOr_br_no)-1);
        vtcp_log("%s,%d 传入的支付系统号码为空设置为清算行行号",__FILE__,__LINE__);
    }

    ret = Hv_orno_brno_Sel( g_pub_tx.reply, &hv_orno_brno, "or_br_no = '%s' ", cOr_br_no );
    if ( ret!=0  && ret!=100)
    {
        sprintf(acErrMsg,"查找hv_brno_orno出错[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        return(ret);
    }	    
    memcpy(br_no,hv_orno_brno.br_no,BRNO_LEN);
    return 0;
}
/****************************************************************************/
/* 函数名称: int FetchRecToPayin(PAY_IN_AREA *hv_payin)                     */
/* 功能描述: 根据payin输入内容提取一条大额支付记录                          */
/* 创建日期: 2006.08.08                                                     */
/* 作者:     ChenMing                                                       */
/* 说明:                                                                    */
/**     认为调用此函数时，输入部分 F_xx 已经有值，但表记录区还没有内容      */
/**     所以利用输入区的关键字锁定表，得到表数据区                          */
/**     此函数使用FIND,虽然在更新时可能多访问了一次表，但减少了锁表的可能   */
/**     缺点是多个服务同时试图修改同一条记录时会发生不一致的问题，但可以避免*/
/****************************************************************************/
int FetchRecToPayin(HV_PAYIN_AREA *hv_payin)
{
	int iRc;
	char cTlrno[5];
	struct hv_zf_c hv_zf;
	vtcp_log("%s,%d hv_payin->F_or_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_or_br_no);
	memset(&hv_zf,'\0',sizeof(hv_zf));
	memset(cTlrno,0,sizeof(cTlrno));
	get_fd_data("0070",cTlrno);
	memcpy(hv_zf.cmtno,hv_payin->F_cmtno,sizeof(hv_zf.cmtno)-1);		
	memcpy(hv_zf.or_br_no,hv_payin->F_or_br_no,sizeof(hv_zf.or_br_no)-1);	
	if (strlen(hv_payin->F_wt_date) > 0)	   hv_zf.wt_date = apatoi(hv_payin->F_wt_date,8);
	memcpy(hv_zf.orderno,hv_payin->F_orderno,sizeof(hv_zf.orderno)-1);	
	vtcp_log("%s,%d hv_payin->F_orderno************=[%s]\n",__FILE__,__LINE__,hv_payin->F_orderno);	
	iRc = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%d ", \ 
		hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);
	if ( iRc==100 )
	{
		sprintf(acErrMsg,"未找到大额支付交易业务记录");
		strncpy( g_pub_tx.reply,"H031",4);
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" );*/
			return iRc; 
	}
	else if ( iRc )
	{
		sprintf(acErrMsg,"查询大额支付交易记录出错[%d]",iRc);
		strncpy( g_pub_tx.reply,"H031",4);
		WRITEMSG
			return iRc;
	}
	vtcp_log("[%s][%d]cTlrno=[%s]",__FILE__,__LINE__,cTlrno);
	vtcp_log("[%s][%d]hv_zf.tel=[%s]",__FILE__,__LINE__,hv_zf.tel);
	vtcp_log("[%s][%d]hv_payin->F_opcd=[%s]",__FILE__,__LINE__,hv_payin->F_opcd);
	if(!memcmp(hv_payin->F_opcd,"30",sizeof(hv_payin->F_opcd)) && !memcmp(cTlrno,hv_zf.tel,sizeof(cTlrno)-1) && strcmp(getenv("FHTEST"),"LVTEST")!=0)
	{
		vtcp_log("%s,%d,录入和复核不能同一个操作员",__FILE__,__LINE__);
		sprintf(acErrMsg,"录入和复核不能同一个操作员!");
		set_zd_data("0130",acErrMsg);
		strncpy( g_pub_tx.reply,"P080",4);
		return(-1);
	}
	memcpy(hv_payin->T_tx_br_no,hv_zf.br_no,sizeof(hv_payin->T_tx_br_no));

	sprintf(hv_payin->T_tx_date, "%d", hv_zf.tx_date);
	sprintf(hv_payin->T_wt_date, "%d", hv_zf.wt_date);	
	sprintf(hv_payin->T_tx_time, "%d", hv_zf.tx_time);
	
	memcpy(hv_payin->T_hv_sts,hv_zf.hv_sts,sizeof(hv_payin->T_hv_sts));
	vtcp_log("[%s][%d] hv_sts====[%c]",__FILE__,__LINE__,hv_zf.hv_sts[0]);
	memcpy(hv_payin->T_orderno,hv_zf.orderno,sizeof(hv_payin->T_orderno));	
	memcpy(hv_payin->T_cur_no,hv_zf.cur_no,sizeof(hv_payin->T_cur_no));	
	memcpy(hv_payin->T_note_type,hv_zf.note_type,sizeof(hv_payin->T_note_type));	
	memcpy(hv_payin->T_note_no,hv_zf.note_no,sizeof(hv_payin->T_note_no));
	dot_dbl2str(hv_zf.tx_amt,sizeof(hv_payin->T_tx_amt) ,2,hv_payin->T_tx_amt);	

	memcpy(hv_payin-> T_tx_chrg_ind,hv_zf.tx_chrg_ind,sizeof(hv_payin->T_tx_chrg_ind));	
	dbl2str(&hv_zf.chrg_amt,2,sizeof(hv_payin->T_chrg_amt),0,	hv_payin->T_chrg_amt); 
	memcpy(hv_payin->T_pay_qs_no,hv_zf.pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	
	memcpy(hv_payin->T_pay_opn_br_no,hv_zf.pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));
	memcpy(hv_payin->T_or_br_no,hv_zf.or_br_no,sizeof(hv_payin->T_or_br_no));	
	vtcp_log("%s,%d hv_payin->T_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->T_pay_opn_br_no);	
	vtcp_log("%s,%d hv_payin->F_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_pay_opn_br_no);
	
	memcpy(hv_payin->T_pay_ac_no,hv_zf.pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	
	memcpy(hv_payin->T_pay_name,hv_zf.pay_name,sizeof(hv_payin->T_pay_name));	
	memcpy(hv_payin->T_pay_addr,hv_zf.pay_addr,sizeof(hv_payin->T_pay_addr));	
	memcpy(hv_payin->T_cash_qs_no,hv_zf.cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	

	memcpy(hv_payin->T_ac_br_no,hv_zf.ac_br_no,sizeof(hv_payin->T_ac_br_no));	
	memcpy(hv_payin->T_cash_opn_br_no,hv_zf.cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));	
	memcpy(hv_payin->T_cash_ac_no,hv_zf.cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	
	memcpy(hv_payin->T_cash_name,hv_zf.cash_name,sizeof(hv_payin->T_cash_name));	
	memcpy(hv_payin->T_cash_addr,hv_zf.cash_addr,sizeof(hv_payin->T_cash_addr));	
	memcpy(hv_payin->T_yw_type,hv_zf.yw_type,sizeof(hv_payin->T_yw_type));	

	memcpy(hv_payin->T_sendco,hv_zf.sendco,sizeof(hv_payin->T_sendco));	
	memcpy(hv_payin->T_receco,hv_zf.receco,sizeof(hv_payin->T_receco));	
	memcpy(hv_payin->T_cmtno,hv_zf.cmtno,sizeof(hv_payin->T_cmtno));	

	memcpy(hv_payin->T_beg_sts,hv_zf.beg_sts,sizeof(hv_payin->T_beg_sts));	
	vtcp_log("[%s][%d] beg_sts====[%c]",__FILE__,__LINE__,hv_zf.beg_sts[0]);
	memcpy(hv_payin->T_operlevel,hv_zf.operlevel,sizeof(hv_payin->T_operlevel));	
	memcpy(hv_payin->T_tel,hv_zf.tel,sizeof(hv_payin->T_tel));		
	memcpy(hv_payin->T_resend_ind,hv_zf.beg_sts,sizeof(hv_payin->T_resend_ind));	
	memcpy(hv_payin->T_operlevel,hv_zf.operlevel,sizeof(hv_payin->T_operlevel));	

	sprintf(hv_payin->T_trace_no, "%d", hv_zf.trace_no);

	memcpy(hv_payin->T_resend_ind,hv_zf.resend_ind,sizeof(hv_payin->T_resend_ind));	
	memcpy(hv_payin->T_hv_osts,hv_zf.hv_osts,sizeof(hv_payin->T_hv_osts));	
	memcpy(hv_payin->T_checkflag,hv_zf.checkflag,sizeof(hv_payin->T_checkflag));	
	memcpy(hv_payin->T_hv_prt_ind,hv_zf.hv_prt_ind,sizeof(hv_payin->T_hv_prt_ind));

	sprintf(hv_payin->T_resend_date, "%d", hv_zf.resend_date);

	memcpy(hv_payin->T_o_orderno,hv_zf.o_orderno,sizeof(hv_payin->T_o_orderno));	

	sprintf(hv_payin->T_o_tx_date, "%d", hv_zf.o_tx_date);

	memcpy(hv_payin->T_o_cmtno,hv_zf.o_cmtno,sizeof(hv_payin->T_o_cmtno));	

	memcpy(hv_payin->T_lw_ind,hv_zf.lw_ind,sizeof(hv_payin->T_lw_ind));	
	memcpy(hv_payin->T_addid,hv_zf.addid,sizeof(hv_payin->T_addid));
	vtcp_log("%s,%d hv_zf.refno************=[%s]\n",__FILE__,__LINE__,hv_zf.refno);	
	memcpy(hv_payin->T_refno,hv_zf.refno,sizeof(hv_payin->T_refno));	
	vtcp_log("%s,%d hv_payin->T_refno******=[%s]\n",__FILE__,__LINE__,hv_payin->T_refno);
	memcpy(hv_payin->T_auth_tel,hv_zf.auth_tel,sizeof(hv_payin->T_auth_tel));		

	sprintf(hv_payin->T_auth_trace_no, "%d", hv_zf.auth_trace_no);

	memcpy(hv_payin->T_chk,hv_zf.chk,sizeof(hv_payin->T_chk));					

	sprintf(hv_payin->T_chk_trace_no, "%d", hv_zf.chk_trace_no);
	sprintf(hv_payin->T_dealerr_date, "%d", hv_zf.dealerr_date);

	memcpy(hv_payin->T_dealerr_tel,hv_zf.dealerr_tel,sizeof(hv_payin->T_dealerr_tel));	
	memcpy(hv_payin->T_resp1,hv_zf.resp1,sizeof(hv_payin->T_resp1));	
	memcpy(hv_payin->T_hv_brf,hv_zf.hv_brf,sizeof(hv_payin->T_hv_brf));	
  vtcp_log("%s,%d hv_payin->T_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->T_pay_opn_br_no);	
	vtcp_log("%s,%d hv_payin->F_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_pay_opn_br_no);
	sprintf(hv_payin->T_o_wt_date, "%d", hv_zf.o_wt_date);

	return(0);	                             
}

/*************************************************
* 函 数 名:  iHvTisToNotPayin
* 功能描述： 将前台传过来的 8583 赋给 NOTPAYIN 结构体相应项
* 作    者:  ChenMing
* 完成日期： 2006-8-9 
*
* 修改记录：
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
int iHvTisToNotPayin(HV_NOTPAY_AREA * phv_notpayin)
{
	HV_FD123_AREA fd123;
	memset(&fd123, 0 , sizeof(fd123));
	get_fd_data("1230",(char*)&fd123);
	/*操作代码***/
	memcpy(phv_notpayin->F_opcd,fd123.opcd,sizeof(phv_notpayin->F_opcd));
	/*往来表示***/
	get_fd_data("0200",phv_notpayin->F_lw_ind);	
	/* 交易机构 */
	get_fd_data("0030",phv_notpayin->F_tx_br_no);	
	/**委托日期       **/
	get_fd_data("0440",phv_notpayin->F_wt_date);	
	strncpy(phv_notpayin->F_tx_date,phv_notpayin->F_wt_date,sizeof(phv_notpayin->F_wt_date));
	/*cmt编号**/
	memcpy(phv_notpayin->F_cmtno,fd123.cmtno,sizeof(phv_notpayin->F_cmtno));
	/*原cmt编号**/
	memcpy(phv_notpayin->F_o_cmtno,fd123.o_cmtno,sizeof(phv_notpayin->F_o_cmtno));	
	/****Add by SSH,2006.9.7,对otype赋值****/
	memcpy(phv_notpayin->F_otype,fd123.otype,sizeof(phv_notpayin->F_otype));
	GetOr_br_noByBr_no(phv_notpayin->F_tx_br_no,phv_notpayin->F_or_br_no);
	/*接收行行号     **/
	memcpy(phv_notpayin->F_ac_br_no,fd123.ac_br_no,sizeof(phv_notpayin->F_ac_br_no));
	/*actno 非支付交易中涉及的帐号   ???  **/
	memcpy(phv_notpayin->F_actno,fd123.pay_ac_no,sizeof(phv_notpayin->F_actno));
	/*F_name 帐号户名**/
	memcpy(phv_notpayin->F_name,fd123.pay_name,sizeof(phv_notpayin->F_name));
	/* 内容   查询，查复，自由格式使用*/
	memcpy(phv_notpayin->F_content,fd123.notpay_content,sizeof(phv_notpayin->F_content));		
	/* 附言 */
	memcpy(phv_notpayin->F_hv_brf,fd123.brf,sizeof(phv_notpayin->F_hv_brf));	
	/**操作员号**/
	get_fd_data("0070",phv_notpayin->F_tel);
	get_fd_data("0070",phv_notpayin->F_send_tel);	
	/**退回/撤销应答**/
	memcpy(phv_notpayin->F_respcode,fd123.resp,sizeof(phv_notpayin->F_respcode));	
	/**原支付交易序号**/
	memcpy(phv_notpayin->F_o_payin_orderno,fd123.o_orderno,sizeof(phv_notpayin->F_o_payin_orderno));	
	/**原支付交易序号  查复之原查询书号等应答类对应的申请交易的序号 **/
	memcpy(phv_notpayin->F_o_notpay_orderno,fd123.qr_tx_num,sizeof(phv_notpayin->F_o_notpay_orderno));	

	/* 原货币符号,查复等交易使用 */
	get_fd_data("0210",phv_notpayin->F_o_cur_no);
	/* 支付交易金额,查复等使用 */
	get_fd_data("0390",phv_notpayin->F_o_txamt);	

	/**原交易种类**/
	memcpy(phv_notpayin->F_o_tx_type,fd123.tx_type,sizeof(phv_notpayin->F_o_tx_type));	

	/**支付交易对应发起行**/
	memcpy(phv_notpayin->F_o_or_br_no,fd123.or_br_no,sizeof(phv_notpayin->F_o_or_br_no));	

	/* 原委托日期  提供给对应的支付交易 */
	get_fd_data("0450",phv_notpayin->F_o_wt_date);	
	/* 原非支付交易日期,查复中用到的查询日期 */	
	get_fd_data("0460",phv_notpayin->F_o_fzf_date);	

	/**原CMT编号**/
	memcpy(phv_notpayin->F_o_cmtno,fd123.o_cmtno,sizeof(phv_notpayin->F_o_cmtno));
	/**业务回执状态   处理状态*/
	memcpy(phv_notpayin->F_rcpstat,fd123.proc_sts,sizeof(phv_notpayin->F_rcpstat));
	/** 报文参考号 
	if (iGetHvRefno(phv_notpayin.F_or_br_no,phv_notpayin->F_refno) != 0)
	{
		return 1;  	
	}**/
	/**原支付交易接受行号 == 发起行行号**/
	memcpy(phv_notpayin->F_o_ac_br_no,phv_notpayin->F_or_br_no,sizeof(phv_notpayin->F_o_ac_br_no)); /**原支付交易发起行号**/
	memcpy(phv_notpayin->F_o_or_br_no,phv_notpayin->F_ac_br_no,sizeof(phv_notpayin->F_o_or_br_no));
	/**ccpc应答码  处理状态**/
	memcpy(phv_notpayin->F_resp1,fd123.procode,sizeof(phv_notpayin->F_resp1));

	return 0;
}
/*************************************************
* 函 数 名:  iHvNotPayinFToT
* 功能描述： 非支付 前台 F ---> T
* 作    者:  ChenMing
* 完成日期： 2006-8-9 
* 修改记录：
*************************************************/
int iHvNotPayinFToT(HV_NOTPAY_AREA *phv_notpayin)
{
	char cFilename[1000];
	FILE * fpr;
	char reply[5];
	int iRet=0;
	char cBuff[2000];
	memset(reply, 0 , sizeof(reply));
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cFilename, 0  , sizeof(cFilename));

	memcpy(phv_notpayin->T_or_br_no,phv_notpayin->F_or_br_no,sizeof(phv_notpayin->T_or_br_no));/* 发起行行号*/
	memcpy(phv_notpayin->T_orderno,phv_notpayin->F_orderno,sizeof(phv_notpayin->T_orderno));/* 交易序号,查询书号，查复书号*/
	memcpy(phv_notpayin->T_tx_date ,phv_notpayin->F_tx_date,sizeof(phv_notpayin->T_tx_date ));/* 交易日期*/
	memcpy(phv_notpayin->T_wt_date ,phv_notpayin->F_wt_date ,sizeof(phv_notpayin->T_wt_date ));/* 委托日期,查询查复日期*/
	memcpy(phv_notpayin->T_br_no,phv_notpayin->F_tx_br_no,sizeof(phv_notpayin->T_br_no));/* 交易行行号*/

	memcpy(phv_notpayin->T_cmtno,phv_notpayin->F_cmtno,sizeof(phv_notpayin->T_cmtno    ));/* CMT编号      */
	get_fd_data("0060",phv_notpayin->T_tx_time);	/* 交易时间*/
	get_fd_data("0040",phv_notpayin->T_trace_no);	/* 流水号*/

	/*iFindQsbrno(phv_payin->T_or_br_no,phv_payin->T_pay_qs_no); 发起清算行行号 
	 *iFindQsbrno(phv_payin->T_ac_br_no,phv_payin->T_cash_qs_no); 接收行清算行号 
	 *iGetSendco(phv_payin->T_or_br_no,phv_payin->T_sendco); 发报中心代码
	 *iGetSendco(phv_payin->T_ac_br_no,phv_payin->T_receco); 收报中心代码
	 */	
	iGet_full_hv_no(phv_notpayin->T_or_br_no,phv_notpayin->T_sendco,phv_notpayin->T_pay_qs_no);
	iGet_full_hv_no(phv_notpayin->T_ac_br_no,phv_notpayin->T_receco,phv_notpayin->T_cash_qs_no);
	memcpy(phv_notpayin->T_ac_br_no ,phv_notpayin->F_ac_br_no,sizeof(phv_notpayin->T_ac_br_no));/* 接收行行号--退汇交易的原发起行 */
	memcpy(phv_notpayin->T_o_wt_date,phv_notpayin->F_o_wt_date,sizeof(phv_notpayin->T_o_wt_date));/*原委托日期*/			
	memcpy(phv_notpayin->T_o_or_br_no ,phv_notpayin->F_o_or_br_no,sizeof(phv_notpayin->T_o_or_br_no ));/* 原支付交易发起行*/
	memcpy(phv_notpayin->T_o_ac_br_no ,phv_notpayin->F_o_ac_br_no,sizeof(phv_notpayin->T_o_ac_br_no )); /* 原支付交易接受行 */
	memcpy(phv_notpayin->T_o_payin_orderno ,phv_notpayin->F_o_payin_orderno ,sizeof(phv_notpayin->T_o_payin_orderno )); /* 原支付交易交易序号*/
	memcpy(phv_notpayin->T_o_cur_no,phv_notpayin->F_o_cur_no,sizeof(phv_notpayin->T_o_cur_no));/* 对应支付交易的货币类型*/
	memcpy(phv_notpayin->T_o_tx_amt,phv_notpayin->F_o_txamt,sizeof(phv_notpayin->T_o_tx_amt));/* 对应支付交易的交易金额*/
	memcpy(phv_notpayin->T_o_tx_type,phv_notpayin->F_o_tx_type,sizeof(phv_notpayin->T_o_tx_type));/* 原交易种类 汇兑1 托收2 待定 */
	memcpy(phv_notpayin->T_o_cmtno  ,phv_notpayin->F_o_cmtno,sizeof(phv_notpayin->T_o_cmtno  ));/* 原CMT编号 */

	/* memcpy(phv_notpayin->T_req_date,phv_notpayin->     ,sizeof(phv_notpayin->T_req_date));申请日期 */
	/*memcpy(phv_notpayin->T_req_br_no,phv_notpayin->    ,sizeof(phv_notpayin->T_req_br_no)); 申请行号 */

	memcpy(phv_notpayin->T_req_content,phv_notpayin->F_content,sizeof(phv_notpayin->T_req_content));/**发起行行号**/
/******
	memcpy(phv_notpayin->T_req_orderno,phv_notpayin->   ,sizeof(phv_notpayin->T_req_orderno)); 申请行号 
	memcpy(phv_notpayin->T_res_date,phv_notpayin->      ,sizeof(phv_notpayin->T_res_date)); 应答日期 
	memcpy(phv_notpayin->T_res_br_no,phv_notpayin->     ,sizeof(phv_notpayin->T_res_br_no)); 应答行号 

	memcpy(phv_notpayin->T_respcode,phv_notpayin->    ,sizeof(phv_notpayin->T_respcode)); 退回/撤销应答 
	memcpy(phv_notpayin->T_addid,phv_notpayin->       ,sizeof(phv_notpayin->T_addid)); 附加域id*****/ 

	memcpy(phv_notpayin->T_send_tel,phv_notpayin->F_send_tel,sizeof(phv_notpayin->T_send_tel));/*  */

	memcpy(phv_notpayin->T_refno,phv_notpayin->F_refno,sizeof(phv_notpayin->T_refno));/* 应答行号 */

	/*memcpy(phv_notpayin->T_hv_prt_ind,phv_notpayin->  ,sizeof(phv_notpayin->T_hv_prt_ind)); 打印标记 */
	memcpy(phv_notpayin->T_req_orderno,phv_notpayin->F_o_notpay_orderno,sizeof(phv_notpayin->T_req_orderno));/* 原非支付交易序号 */

/****
	memcpy(phv_notpayin->T_lw_ind,phv_notpayin->F_lw_ind,sizeof(phv_notpayin->T_lw_ind)); 来往标志 */

	/*  写入附加域 ???  */

	if (!memcmp(phv_notpayin->F_opcd,"10",sizeof(phv_notpayin->F_opcd)))
	{/* 01录入 */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_REGIST;
	}
	if (!memcmp(phv_notpayin->F_opcd,"30",sizeof(phv_notpayin->F_opcd)))
	{/* 02复合 */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_CHECK;
	}
	if (!memcmp(phv_notpayin->F_opcd,"40",sizeof(phv_notpayin->F_opcd)))
	{/* 03发送 */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_WAIT;
	}
	if (!memcmp(phv_notpayin->F_opcd,"60",sizeof(phv_notpayin->F_opcd)))
	{/* 04接收 */
		if (!memcmp(reply,"0000",4))
		{
			phv_notpayin->T_hv_fzf_sts[0]=RECEIVE_ACCT_QS;
		}
		else
		{
			/* 记账失败了，要先判断接收机构是否存在，存在挂支行，不存在挂清算 */
			phv_notpayin->T_hv_fzf_sts[0]=RECEIVE_CUSTNAME_ERR;
		}
	}
	if (!memcmp(phv_notpayin->F_opcd,"50",sizeof(phv_notpayin->F_opcd)))
	{/* 05应答 */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_QS;
	}
	return 0;
}

/************************************************************************* 
* 函 数 名: iHvUpdFzfRecFromPayin                  
* 功能描述： 根据 非支付结构 T 部分 更新 hv_fzf    
* 作    者:  ChenMing                             
* 完成日期： 2006-8-9                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int iHvUpdFzfRecFromPayin(HV_NOTPAY_AREA *phv_notpayin,int insflg)
{          	/* insflg 0.更新   1.插入  */

	int ret=0;
	struct hv_fzf_c hv_fzf;
	memset(&hv_fzf, 0x00, sizeof(hv_fzf));

	vtcp_log("%s,%d iHvUpdFzfRecFromPayin() 开始 \n",__FILE__,__LINE__);
	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
	
	memcpy(hv_fzf.cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf.cmtno)-1);
	memcpy(hv_fzf.or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf.or_br_no)-1);
	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
	if ( strlen(phv_notpayin->T_wt_date) > 0 )  
		hv_fzf.wt_date = apatoi(phv_notpayin->T_wt_date,8);
	memcpy(hv_fzf.orderno,phv_notpayin->T_orderno,sizeof(hv_fzf.orderno)-1);


	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);

	if (insflg == 0) 
	{ /* 0.更新    */
		ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"cmtno='%s' and or_br_no='%s' and wt_date='%d' and orderno='%s'", \
			hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
				return ret;
		}
		ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"查询非支付交易表错误!![%d]",ret);
			WRITEMSG
				return ret;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"非支付交易表中不存在该记录!!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"O127");
			return ret;
		}		
		iHvNotPayin_Copy(phv_notpayin,&hv_fzf);

		ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
			WRITEMSG
				return ret;
		}    
		Hv_fzf_Clo_Upd();	

	}
	else
	{  /* 1.插入  */

    vtcp_log("%s,%d,  phv_notpayin->T_br_no=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_br_no );
    vtcp_log("%s,%d,  phv_notpayin->T_cmtno=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_cmtno );
    vtcp_log("%s,%d, T_orderno= [%.8s]",__FILE__,__LINE__,phv_notpayin->T_orderno );
    vtcp_log("%s,%d, T_or_br_no= [%.12s]",__FILE__,__LINE__,phv_notpayin->T_or_br_no);
        
		iHvNotPayin_Copy(phv_notpayin,&hv_fzf);

    vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf.br_no );
    vtcp_log("%s,%d,  hv_fzf.cmtno=[%.7s]",__FILE__,__LINE__,hv_fzf.cmtno );  
    
    vtcp_log("%s,%d,  hv_fzf.or_br_no=[%.12s]",__FILE__,__LINE__,hv_fzf.or_br_no );
    vtcp_log("%s,%d,  hv_fzf.orderno=[%.8s]",__FILE__,__LINE__,hv_fzf.orderno );  
      

    Hv_fzf_Print(&hv_fzf);
    
		ret = Hv_fzf_Ins(hv_fzf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"向非支付交易表中增加记录错误!![%d]",ret);
			WRITEMSG
				return ret;
		}			
	}	
}
/***********************************************************************     
* 函 数 名: iHvNotPayin_Copy                    
* 功能描述: 将HV_NOTPAY_AREA中相应字段值赋给hv_fzf_c中相应字段 
* 作    者:  ChenMing                            
* 完成日期： 2006-8-9                            
* 修改记录：                                     
*   日   期:                                      
*   修 改 人:                                    
*   修改内容:                                    
*************************************************************************/  
int iHvNotPayin_Copy(HV_NOTPAY_AREA *phv_notpayin, struct hv_fzf_c *hv_fzf)
{
	/*  	 */
	
	memcpy(hv_fzf->cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf->cmtno)-1);
	memcpy(hv_fzf->or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf->or_br_no)-1);
	if ( strlen(phv_notpayin->T_wt_date) > 0 )  
		hv_fzf->wt_date = apatoi(phv_notpayin->T_wt_date,8);
	memcpy(hv_fzf->orderno,phv_notpayin->T_orderno,sizeof(hv_fzf->orderno)-1);
	
  vtcp_log(" phv_notpayin->T_otype[%s] ",phv_notpayin->T_otype);		
  hv_fzf->otype[0] = phv_notpayin->T_otype[0];
	
	if( strlen(phv_notpayin->T_tx_date) > 0 ) 	   
	{
			hv_fzf->tx_date = apatoi(phv_notpayin->T_tx_date,8);
	}

  vtcp_log("%s,%d,  phv_notpayin->T_br_no=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_br_no );
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );

	
	memcpy(hv_fzf->br_no,phv_notpayin->T_br_no,BRNO_LEN);	  
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );
  
	vtcp_log("[%s] [%d] T_hv_fzf_sts = [%c]",__FILE__,__LINE__,phv_notpayin->T_hv_fzf_sts[0]); 
	hv_fzf->hv_fzf_sts[0] = phv_notpayin->T_hv_fzf_sts[0];    
	vtcp_log("[%s] [%d]hv_fzf->hv_fzf_sts = [%c]",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]); 
  
	if ( strlen(phv_notpayin->T_tx_time) > 0 )   	  hv_fzf->tx_time = apatoi(phv_notpayin->T_tx_time,6);
  if ( strlen(phv_notpayin->T_trace_no) > 0 )   	hv_fzf->trace_no = apatoi(phv_notpayin->T_trace_no,6);

	memcpy(hv_fzf->pay_qs_no,phv_notpayin->T_pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(hv_fzf->ac_br_no,phv_notpayin->T_ac_br_no,sizeof(hv_fzf->ac_br_no)-1);      	
	memcpy(hv_fzf->cash_qs_no,phv_notpayin->T_cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);

	vtcp_log("phv_notpayin->T_o_wt_date=[%s]   phv_notpayin->T_o_wt_date=[%s]",phv_notpayin->T_o_wt_date,phv_notpayin->T_o_wt_date);	
	hv_fzf->o_tx_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* 请在处理时保证任何时候o_wt_date和o_tx_date一致 */
	hv_fzf->o_wt_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* 请在处理时保证任何时候o_wt_date和o_tx_date一致 */

	memcpy(hv_fzf->o_or_br_no,phv_notpayin->T_o_or_br_no,sizeof(hv_fzf->o_or_br_no)-1);	
	memcpy(hv_fzf->o_ac_br_no,phv_notpayin->T_o_ac_br_no,sizeof(hv_fzf->o_ac_br_no)-1);
	vtcp_log("[%s] [%d]hv_fzf->o_tx_date = [%d]",__FILE__,__LINE__,hv_fzf->o_tx_date);

	memcpy(hv_fzf->tel,phv_notpayin->T_tel,sizeof(hv_fzf->tel)-1);
	memcpy(hv_fzf->send_tel,phv_notpayin->T_send_tel,sizeof(hv_fzf->send_tel)-1);
		
	memcpy(hv_fzf->o_orderno,phv_notpayin->T_o_payin_orderno,sizeof(hv_fzf->o_orderno)-1);
	memcpy(hv_fzf->o_cur_ind,phv_notpayin->T_o_cur_no,sizeof(hv_fzf->o_cur_ind)-1);
	vtcp_log("%s,%d,Gethere,T_AMT=%s",__FILE__,__LINE__,phv_notpayin->T_o_tx_amt);

 	if(strlen(phv_notpayin->T_o_tx_amt) >0 )  	  
		dot_str2dbl(phv_notpayin->T_o_tx_amt,sizeof(phv_notpayin->T_o_tx_amt), &hv_fzf->o_tx_amt);
	vtcp_log("%s,%d,Gethere,D_AMT=%f",__FILE__,__LINE__,hv_fzf->o_tx_amt);
  			
	memcpy(hv_fzf->o_cmtno,phv_notpayin->T_o_cmtno,sizeof(hv_fzf->o_cmtno)-1);
	/**
	memcpy(hv_fzf->o_tx_type,phv_notpayin->T_o_tx_type,sizeof(hv_fzf->o_tx_type)-1);
	**/
	hv_fzf->o_tx_type[0] = phv_notpayin->T_o_tx_type[0];
	memcpy(hv_fzf->sendco,phv_notpayin->T_sendco,sizeof(hv_fzf->sendco)-1);
	memcpy(hv_fzf->receco,phv_notpayin->T_receco,sizeof(hv_fzf->receco)-1);
	vtcp_log("[%s] [%d]hv_fzf->o_tx_type[0] = [%c]",__FILE__,__LINE__,hv_fzf->o_tx_type[0]);
	hv_fzf->req_date = apatoi(phv_notpayin->T_req_date,8);
	memcpy(hv_fzf->req_br_no,phv_notpayin->T_req_br_no,sizeof(hv_fzf->req_br_no)-1);
	memcpy(hv_fzf->req_content,phv_notpayin->T_req_content,sizeof(hv_fzf->req_content)-1);
	memcpy(hv_fzf->req_orderno,phv_notpayin->T_req_orderno,sizeof(hv_fzf->req_orderno)-1);	
	hv_fzf->res_date = apatoi(phv_notpayin->T_res_date,8);
	vtcp_log("[%s] [%d]hv_fzf->res_date = [%d]",__FILE__,__LINE__,hv_fzf->res_date);
	memcpy(hv_fzf->res_br_no,phv_notpayin->T_res_br_no,sizeof(hv_fzf->res_br_no)-1);
	memcpy(hv_fzf->res_content,phv_notpayin->T_res_content,sizeof(hv_fzf->res_content)-1);
	memcpy(hv_fzf->res_orderno,phv_notpayin->T_res_orderno,sizeof(hv_fzf->res_orderno)-1);
	/**
	memcpy(hv_fzf->res_sts,phv_notpayin->T_respcode,sizeof(hv_fzf->res_sts)-1);
	**/
	hv_fzf->res_sts[0] = phv_notpayin->T_respcode[0];
	vtcp_log("[%s] [%d]hv_fzf->res_sts = [%c]",__FILE__,__LINE__,hv_fzf->res_sts[0]);
	memcpy(hv_fzf->addid,phv_notpayin->T_addid,sizeof(hv_fzf->addid)-1);
	memcpy(hv_fzf->refno,phv_notpayin->T_refno,sizeof(hv_fzf->refno)-1);
 	
	memcpy(hv_fzf->hv_prt_ind,phv_notpayin->T_hv_prt_ind,sizeof(hv_fzf->hv_prt_ind)-1);
	memcpy(hv_fzf->hv_brf,phv_notpayin->T_hv_brf,sizeof(hv_fzf->hv_brf)-1);
	hv_fzf->send_trace_no = apatoi(phv_notpayin->T_o_trace_no,6);
	vtcp_log("[%s] [%d]hv_fzf->o_trace_no= [%d]",__FILE__,__LINE__,hv_fzf->send_trace_no);
	vtcp_log("[%s] [%d]hv_fzf->refno= [%s]",__FILE__,__LINE__,hv_fzf->refno);
	hv_fzf->lw_ind[0] = phv_notpayin->T_lw_ind[0];/* 来往标识 1-往 2-来  */
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );
  		
  	/* */
  return 0;	
}

/************************************************************************* 
* 函 数 名: iHvUpdZfRecFromPayin                  
* 功能描述： 根据 支付结构 T 部分 更新 hv_zf    
* 作    者:  ChenMing                             
* 完成日期： 2006-8-18                             
* 修改记录：                                      
*   日   期:                                      
*   修 改 人:                                     
*   修改内容:                                      
*************************************************************************/ 
int iHvUpdZfRecFromPayin(HV_PAYIN_AREA *phv_payin,int insflg)
{          	/* insflg 0.更新   1.插入  */

	int ret=0;
	char sqltmp[200];
	struct hv_zf_c hv_zf;
	char    cTempstr    [1024];
	
	
	memset(&sqltmp, 0x00, sizeof(sqltmp));	
	memset(&hv_zf, 0x00, sizeof(hv_zf));
	vtcp_log("iHvUpdRecFromPayin() 开始 ");

	
	memcpy(hv_zf.cmtno,phv_payin->T_cmtno,sizeof(phv_payin->T_cmtno));
	memcpy(hv_zf.or_br_no,phv_payin->T_or_br_no,sizeof(phv_payin->T_or_br_no));
	if ( strlen(phv_payin->T_wt_date) > 0 )  
		hv_zf.wt_date = apatoi(phv_payin->T_wt_date,8);
	memcpy(hv_zf.orderno,phv_payin->T_orderno,sizeof(phv_payin->T_orderno));
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 

	if (insflg == 0) 
	{ /* 0.更新    */
		ret = Hv_zf_Dec_Upd(g_pub_tx.reply," cmtno='%s' and or_br_no='%s' and wt_date=%d and orderno='%s' ", \
			hv_zf.cmtno,hv_zf.or_br_no,hv_zf.wt_date,hv_zf.orderno);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
				return ret;
		}
		ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"查询支付交易表错误!![%d]",ret);
			WRITEMSG
				return ret;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"支付交易表中不存在该记录!!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"O127");
			return ret;
		}

		iHvPayin_Copy(phv_payin,&hv_zf);

		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.pay_name, sizeof(hv_zf.pay_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]刘岳\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
			memcpy(hv_zf.pay_name, cTempstr, sizeof(hv_zf.pay_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		
		
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.cash_name, sizeof(hv_zf.cash_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]刘岳\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
			memcpy(hv_zf.cash_name, cTempstr, sizeof(hv_zf.cash_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name); 

		vtcp_log("[%s][%d] 更新支付交易表记录成功",__FILE__,__LINE__);	

		ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
			WRITEMSG
			return ret;
		}    
		Hv_zf_Clo_Upd();	
		vtcp_log("[%s][%d] 更新支付交易表记录成功",__FILE__,__LINE__);	
	}
	else
	{  /* 1.插入  */
		iHvPayin_Copy(phv_payin,&hv_zf);
			
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.pay_name, sizeof(hv_zf.pay_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]刘岳\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
			memcpy(hv_zf.pay_name, cTempstr, sizeof(hv_zf.pay_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		
		
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.cash_name, sizeof(hv_zf.cash_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]刘岳\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
			memcpy(hv_zf.cash_name, cTempstr, sizeof(hv_zf.cash_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name); 
			
		ret = Hv_zf_Ins(hv_zf, g_pub_tx.reply);
		vtcp_log("[%s][%d] reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"向支付交易表中增加记录错误!![%d][%s]",ret,g_pub_tx.reply);
			WRITEMSG
			Hv_zf_Debug(&hv_zf);
				return ret;
		}		
		vtcp_log("[%s][%d] 向支付交易表中增加记录成功",__FILE__,__LINE__);	
	}
	ret = dg_hv_zfhtr(phv_payin,phv_payin->T_tx_amt);
	if (ret != 0)
	{
	   sprintf(acErrMsg,"向支付交易明细表增加记录错误!![%d]",ret);
	   strcpy(g_pub_tx.reply,"P010");
	   WRITEMSG
	   Hv_zf_Debug(&hv_zf);
		return ret;
	}
	vtcp_log("[%s][%d] 插入 hv_zfhtr 表成功！",__FILE__,__LINE__); 
}
/***********************************************************************     
* 函 数 名: iHvPayin_Copy                    
* 功能描述: 将HV_NOTPAY_AREA中相应字段值赋给hv_zf_c中相应字段 
* 作    者:  ChenMing                            
* 完成日期： 2006-8-9                            
* 修改记录：                                     
*   日   期:                                      
*   修 改 人:                                    
*   修改内容:                                    
*************************************************************************/  
int iHvPayin_Copy(HV_PAYIN_AREA *phv_payin, struct hv_zf_c *hv_zf_c)
{
	
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	memcpy(hv_zf_c->cmtno,phv_payin->T_cmtno,sizeof(phv_payin->T_cmtno));
	memcpy(hv_zf_c->or_br_no,phv_payin->T_or_br_no,sizeof(phv_payin->T_or_br_no));
	if ( strlen(phv_payin->T_wt_date) > 0 )  
		hv_zf_c->wt_date = apatoi(phv_payin->T_wt_date,8);
	memcpy(hv_zf_c->orderno,phv_payin->T_orderno,sizeof(phv_payin->T_orderno));

	
	strncpy(hv_zf_c->br_no,phv_payin->T_tx_br_no,BRNO_LEN);
	hv_zf_c->tx_date=apatoi(phv_payin->T_tx_date,8);/*交易日期 */
	
  vtcp_log("phv_payin->T_tx_time==[%.6s]",phv_payin->T_tx_time);		
	hv_zf_c->tx_time = apatoi(phv_payin->T_tx_time,6);/* 交易时间 */
  vtcp_log("hv_zf_c->tx_time==[%d]",hv_zf_c->tx_time);	
  	
	strncpy(hv_zf_c->orderno,phv_payin->T_orderno,sizeof(hv_zf_c->orderno  )-1);/* 人行的交易序号 */
	strncpy(hv_zf_c->cur_no,phv_payin->T_cur_no,sizeof(hv_zf_c->cur_no   )-1);/* 货币符号 */
	strncpy(hv_zf_c->note_type,phv_payin->T_note_type,sizeof(hv_zf_c->note_type)-1);/* 凭证类型  */	  
	strncpy(hv_zf_c->note_no,phv_payin->T_note_no,sizeof(hv_zf_c->note_no  )-1);/* 凭证号  */

  vtcp_log("phv_payin->T_tx_amt==[%.16s]",phv_payin->T_tx_amt);			
	dot_str2dbl(phv_payin->T_tx_amt,sizeof(phv_payin->T_tx_amt),&hv_zf_c->tx_amt);/* 交易金额  */	   	
  vtcp_log("hv_zf_c->tx_amt==[%lf]",hv_zf_c->tx_amt);			
		   	
	hv_zf_c->tx_chrg_ind[0]=phv_payin->T_tx_chrg_ind[0]; /* 手续费标志*/
	/*暂时不处理
	*hv_zf_c->chrg_amt  手续费  
	*/	   	
	/* 发起清算行行号(帐号) accountno  */	
	
		   	
  strncpy(hv_zf_c->pay_qs_no,phv_payin->T_pay_qs_no,sizeof(hv_zf_c->pay_qs_no)-1); 
	vtcp_log("转换的发起清算行号=[%.12s]",phv_payin->T_pay_qs_no,hv_zf_c->pay_qs_no);TRACE 
	strncpy(hv_zf_c->or_br_no,phv_payin->T_or_br_no,sizeof(hv_zf_c->or_br_no)-1);/* 发起行行号--退汇交易的原接收行  */
	strncpy(hv_zf_c->pay_opn_br_no ,phv_payin->T_pay_opn_br_no,sizeof(hv_zf_c->pay_opn_br_no )-1);/* 付款人开户行行号  */
	vtcp_log("[%s][%d]phv_payin->T_pay_opn_br_no=[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	strncpy(hv_zf_c->pay_ac_no,phv_payin->T_pay_ac_no,sizeof(hv_zf_c->pay_ac_no)-1);/* 付款人帐号  */
	strncpy(hv_zf_c->pay_name,phv_payin->T_pay_name,sizeof(hv_zf_c->pay_name)-1);/* 付款人姓名--退汇交易的原收款人户名  */
	strncpy(hv_zf_c->pay_addr,phv_payin->T_pay_addr,sizeof(hv_zf_c->pay_addr)-1);/* 付款人地址  */
	strncpy(hv_zf_c->cash_qs_no,phv_payin->T_cash_qs_no,sizeof(hv_zf_c->cash_qs_no)-1);/* 接收行清算行号  */
	strncpy(hv_zf_c->ac_br_no,phv_payin->T_ac_br_no,sizeof(hv_zf_c->ac_br_no)-1);/* 接收行行号--退汇交易的原发起行  */
	strncpy(hv_zf_c->cash_opn_br_no,phv_payin->T_cash_opn_br_no,sizeof(hv_zf_c->cash_opn_br_no)-1);/* 收款人开户行行号  */
	strncpy(hv_zf_c->cash_ac_no,phv_payin->T_cash_ac_no,	sizeof(hv_zf_c->cash_ac_no)-1);/* 收款人帐号  */
 
	strncpy(hv_zf_c->cash_name,phv_payin->T_cash_name,sizeof(hv_zf_c->cash_name)-1);/* 收款人姓名--退汇交易的原付款人户名  */
	strncpy(hv_zf_c->cash_addr,phv_payin->T_cash_addr,sizeof(hv_zf_c->cash_addr)-1);/* 收款人地址  */
	strncpy(hv_zf_c->yw_type,phv_payin->T_yw_type,sizeof(hv_zf_c->yw_type)-1);/* 业务种类  */
	vtcp_log("[%s,%d]yw_type===[%s]",__FILE__,__LINE__,phv_payin->T_yw_type);
	strncpy(hv_zf_c->sendco,phv_payin->T_sendco,sizeof(hv_zf_c->sendco)-1);/* 发报中心代码  */
	strncpy(hv_zf_c->receco,phv_payin->T_receco,sizeof(hv_zf_c->receco)-1);/* 收报中心代码  */
	strncpy(hv_zf_c->cmtno,phv_payin->T_cmtno,	sizeof(hv_zf_c->cmtno)-1);/* CMT编号  */
	hv_zf_c->beg_sts  [0]=phv_payin->T_beg_sts  [0];/* 存放stat原始状态用于时候监督用  */
	hv_zf_c->operlevel[0]=phv_payin->T_operlevel[0];/*优先级别0-一般1-紧急2-特急(救灾战备款等支付) */
	hv_zf_c->hv_sts[0]=phv_payin->T_hv_sts[0];/* 状态 请参阅hv_define.h程序  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	strncpy(hv_zf_c->tel,phv_payin->T_tel,sizeof(hv_zf_c->tel)-1); /* 操作员员  */
	hv_zf_c->trace_no=apatoi(phv_payin->T_trace_no,sizeof(phv_payin->T_trace_no));/* 流水号 */
	hv_zf_c->hv_osts[0]=phv_payin->T_hv_osts[0];/* 退汇交易用到的原支付来帐交易状态 */
	hv_zf_c->resend_ind[0]=phv_payin->T_resend_ind[0]; /* 拒绝重发标志*/
	hv_zf_c->resend_date=apatoi(phv_payin->T_resend_date,8);/* 拒绝重新发送日期  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	vtcp_log("phv_payin->T_o_tx_date=[%.8s]   phv_payin->T_o_wt_date=[%.8s]",phv_payin->T_o_tx_date,phv_payin->T_o_wt_date); 
	hv_zf_c->o_tx_date=apatoi(phv_payin->T_o_tx_date,8);/* 原支付交易日期--退汇交易用的  */
	hv_zf_c->o_wt_date=apatoi(phv_payin->T_o_tx_date,8);/* 原委托日期  请在处理时保证任何时候o_wt_date和o_tx_date一致??  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	hv_zf_c->lw_ind[0]=phv_payin->T_lw_ind[0];/* 来往标识 1-往 2-来  */
	hv_zf_c->auth_trace_no=apatoi(phv_payin->T_auth_trace_no,sizeof(phv_payin->T_auth_trace_no));/* 授权操作员流水号 */
	hv_zf_c->chk_trace_no=apatoi(phv_payin->T_chk_trace_no,sizeof(phv_payin->T_chk_trace_no));/* 复核操作员流水号 */
	hv_zf_c->send_tel_trace_no=apatoi(phv_payin->T_send_tel_trace_no,sizeof(phv_payin->T_send_tel_trace_no));/* 发送操作员流水号 */
	hv_zf_c->dealerr_date=apatoi(phv_payin->T_dealerr_date,sizeof(phv_payin->T_dealerr_date));/* 错帐处理日期 */
	memcpy(hv_zf_c->o_orderno,phv_payin->T_o_orderno,sizeof(hv_zf_c->o_orderno)-1);/* 原支付交易序号--退汇交易用的  */
	memcpy(hv_zf_c->o_cmtno,phv_payin->T_o_cmtno,sizeof(hv_zf_c->o_cmtno)-1);/* 原cmtno--退汇交易用的  */
	memcpy(hv_zf_c->addid,phv_payin->T_addid,sizeof(hv_zf_c->addid)-1);/* 附加域 id --addit表 */
	memcpy(hv_zf_c->refno,phv_payin->T_refno,sizeof(hv_zf_c->refno)-1);/* 报文参考号  */
	memcpy(hv_zf_c->auth_tel,phv_payin->T_auth_tel,sizeof(hv_zf_c->auth_tel)-1);/* 授权操作员 */
	memcpy(hv_zf_c->chk,phv_payin->T_chk,sizeof(hv_zf_c->chk)-1);/* 复核操作员 */
	memcpy(hv_zf_c->send_tel,phv_payin->T_send_tel,sizeof(hv_zf_c->send_tel   )-1);/* 发送操作员 */
	memcpy(hv_zf_c->dealerr_tel,phv_payin->T_dealerr_tel,sizeof(hv_zf_c->dealerr_tel)-1);/* 错帐处理柜员 */
	memcpy(hv_zf_c->resp1,phv_payin->T_resp1,sizeof(hv_zf_c->resp1)-1);/* 应答码 */
	vtcp_log("[%s][%d]hv_zf_c->T_hv_brf======[%s]",__FILE__,__LINE__,hv_zf_c->hv_brf);
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	memcpy(hv_zf_c->hv_brf,phv_payin->T_hv_brf,sizeof(hv_zf_c->hv_brf)-1);/* 备注  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf);
	hv_zf_c->hv_sts[0] = phv_payin->T_hv_sts[0]; 
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
 
	memcpy(hv_zf_c->hv_prt_ind,phv_payin->T_hv_prt_ind,sizeof(phv_payin->T_hv_prt_ind));  /*打印标志 */ 
	/*	vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf); */	
	return 0;
}



int Hv_fzf_Print(struct hv_fzf_c *ps){
	vtcp_log("hv_fzf_c.rowid=[%s]",ps->rowid);
	vtcp_log("hv_fzf_c.or_br_no=[%s]",ps->or_br_no);
	vtcp_log("hv_fzf_c.orderno=[%s]",ps->orderno);
	vtcp_log("hv_fzf_c.tx_date=[%ld]",ps->tx_date);
	vtcp_log("hv_fzf_c.wt_date=[%ld]",ps->wt_date);
	vtcp_log("hv_fzf_c.br_no=[%s]",ps->br_no);
	vtcp_log("hv_fzf_c.hv_fzf_sts=[%s]",ps->hv_fzf_sts);
	vtcp_log("hv_fzf_c.cmtno=[%s]",ps->cmtno);
	vtcp_log("hv_fzf_c.tx_time=[%ld]",ps->tx_time);
	vtcp_log("hv_fzf_c.trace_no=[%ld]",ps->trace_no);
	vtcp_log("hv_fzf_c.pay_qs_no=[%s]",ps->pay_qs_no);
	vtcp_log("hv_fzf_c.ac_br_no=[%s]",ps->ac_br_no);
	vtcp_log("hv_fzf_c.cash_qs_no=[%s]",ps->cash_qs_no);
	vtcp_log("hv_fzf_c.o_tx_date=[%ld]",ps->o_tx_date);
	vtcp_log("hv_fzf_c.o_or_br_no=[%s]",ps->o_or_br_no);
	vtcp_log("hv_fzf_c.o_ac_br_no=[%s]",ps->o_ac_br_no);
	vtcp_log("hv_fzf_c.o_orderno=[%s]",ps->o_orderno);
	vtcp_log("hv_fzf_c.o_cur_ind=[%s]",ps->o_cur_ind);
	vtcp_log("hv_fzf_c.o_tx_amt=[%f]",ps->o_tx_amt);
	vtcp_log("hv_fzf_c.o_cmtno=[%s]",ps->o_cmtno);
	vtcp_log("hv_fzf_c.o_tx_type=[%s]",ps->o_tx_type);
	vtcp_log("hv_fzf_c.sendco=[%s]",ps->sendco);
	vtcp_log("hv_fzf_c.receco=[%s]",ps->receco);
	vtcp_log("hv_fzf_c.req_date=[%ld]",ps->req_date);
	vtcp_log("hv_fzf_c.req_br_no=[%s]",ps->req_br_no);
	vtcp_log("hv_fzf_c.req_content=[%s]",ps->req_content);
	vtcp_log("hv_fzf_c.req_orderno=[%s]",ps->req_orderno);
	vtcp_log("hv_fzf_c.res_date=[%ld]",ps->res_date);
	vtcp_log("hv_fzf_c.res_br_no=[%s]",ps->res_br_no);
	vtcp_log("hv_fzf_c.res_content=[%s]",ps->res_content);
	vtcp_log("hv_fzf_c.res_orderno=[%s]",ps->res_orderno);
	vtcp_log("hv_fzf_c.tx_sts=[%s]",ps->tx_sts);
	vtcp_log("hv_fzf_c.res_sts=[%s]",ps->res_sts);
	vtcp_log("hv_fzf_c.addid=[%s]",ps->addid);
	vtcp_log("hv_fzf_c.refno=[%s]",ps->refno);
	vtcp_log("hv_fzf_c.hv_prt_ind=[%s]",ps->hv_prt_ind);
	vtcp_log("hv_fzf_c.hv_brf=[%s]",ps->hv_brf);
	vtcp_log("hv_fzf_c.tel=[%s]",ps->tel);
	vtcp_log("hv_fzf_c.lw_ind=[%s]",ps->lw_ind);
	return(0);
}


int pub_com_CMT802_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack802)
{    
	/************支付业务收费清单********************************/
	char cTmpBuf[16];	
	double  dTxamt = 0.0;	
	HV_802_MAIN_ADD hv_802_main;

	memset(&hv_802_main,0,sizeof(hv_802_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*支付交易序号  一天只有一笔，暂时写死了*/
	
	hv_payin->T_operlevel[0] = pack802->PRI;			/*有限级别*/
	memcpy(hv_payin->T_cmtno,"802",3);					  /*CMT编号*/
	memcpy(hv_payin->T_tx_date,pack802->workDate,8);	/*交易日期*/
	iHVGETTAG( pack802,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*委托日期*/
	iHVGETTAG( pack802,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*收报中心代码*/	
	iHVGETTAG( pack802,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/	
	iHVGETTAG( pack802,"33H",cTmpBuf,sizeof(cTmpBuf));					/*  金额   没有货币 */

  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);
	
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*货币金额*/
  vtcp_log("[%s][%d] cTmpBuf=[%lf]:",__FILE__,__LINE__,dTxamt);	

	/******** 处理附加域**/	
	iHVGETTAG(pack802,"072",hv_802_main.beg_date,sizeof(hv_802_main.beg_date));		/*计费开始日期*/
	iHVGETTAG(pack802,"073",hv_802_main.end_date,sizeof(hv_802_main.end_date));		/*计费终止日期 */
	iHVGETTAG(pack802,"FN1",hv_802_main.zjcy_rate,sizeof(hv_802_main.zjcy_rate));	/*直接参与者收费比率*/
	iHVGETTAG(pack802,"FN2",hv_802_main.hb_rate,sizeof(hv_802_main.hb_rate));		  /*行别收费比率 */
	iHVGETTAG(pack802,"FN3",hv_802_main.ywl_rate,sizeof(hv_802_main.ywl_rate));		/*月累计业务量收费比率*/

	iHVGETTAG(pack802,"EN1",hv_802_main.zf_fee,sizeof(hv_802_main.zf_fee));		/*支付类业务收费金额 */
	iHVGETTAG(pack802,"ED1",hv_802_main.zf_cnt,sizeof(hv_802_main.zf_cnt));		/*支付类业务收费记录个数 */
	iHVGETTAG(pack802,"EN2",hv_802_main.db_fee,sizeof(hv_802_main.db_fee));		/*单边业务收费金额*/
	iHVGETTAG(pack802,"ED2",hv_802_main.db_cnt,sizeof(hv_802_main.db_cnt));		/*单边业务收费记录个数 */
	iHVGETTAG(pack802,"EN3",hv_802_main.xx_fee,sizeof(hv_802_main.xx_fee));		/*信息类业务收费金额*/
	iHVGETTAG(pack802,"ED3",hv_802_main.xx_cnt,sizeof(hv_802_main.xx_cnt));		/*信息类业务收费记录个数 */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_802_main,sizeof(HV_802_MAIN_ADD));
	return 0; 
}

int pub_com_CMT81x_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack81x)
{    
	/************支付业务收费清单********************************/
	char cTmpBuf[16];	
	double  dTxamt = 0.0;	
	HV_81X_MAIN_ADD hv_81x_main;

	memset(&hv_81x_main,0,sizeof(hv_81x_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*支付交易序号  一天只有一笔，暂时写死了*/
	
	hv_payin->T_operlevel[0] = pack81x->PRI;			/*有限级别*/
	memcpy(hv_payin->T_cmtno,pack81x->CMTCode,3);					  /*CMT编号*/
	memcpy(hv_payin->T_tx_date,pack81x->workDate,8);	/*交易日期*/
	iHVGETTAG( pack81x,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*委托日期*/
	iHVGETTAG( pack81x,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*收报中心代码*/	
	iHVGETTAG( pack81x,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*发起清算行*/	
	iHVGETTAG( pack81x,"33H",cTmpBuf,sizeof(cTmpBuf));					/*  金额   没有货币 */

  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);
	
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*计费总金额*/
  vtcp_log("[%s][%d] cTmpBuf=[%lf]:",__FILE__,__LINE__,dTxamt);	

	/******** 处理附加域**/	
	iHVGETTAG(pack81x,"072",hv_81x_main.beg_date,sizeof(hv_81x_main.beg_date));		/*计费开始日期*/
	iHVGETTAG(pack81x,"073",hv_81x_main.end_date,sizeof(hv_81x_main.end_date));		/*计费终止日期 */
	iHVGETTAG(pack81x,"33J",hv_81x_main.hv_amt,sizeof(hv_81x_main.hv_amt));		/** 大额计费金额 **/
	iHVGETTAG(pack81x,"33K",hv_81x_main.lv_amt,sizeof(hv_81x_main.lv_amt));			/** 小额计费金额 **/

	iHVGETTAG(pack81x,"ED1",hv_81x_main.hv_cnt,sizeof(hv_81x_main.hv_cnt));		/*参与者大额计费数目 */
	iHVGETTAG(pack81x,"ED2",hv_81x_main.lv_cnt,sizeof(hv_81x_main.lv_cnt));		/*参与者小额计费数目 */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_81x_main,sizeof(HV_81X_MAIN_ADD));
	return 0; 
}



/******收费803清单主表**END ADD BY ChenMing*****/
int pub_com_CMT803_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack803)
{    
	/************日间透支计息报文********************************/
	char cRecCnt[5];	
	char cTmpBuf[23];	
	char cTmpBuf_CLD[MAX_803_REC*56+10];	
	double  dTxamt = 0.0;	
	
	
	HV_803_MAIN_ADD hv_803_main;
  vtcp_log("pub_com_CMT803_TAG2T() 开始  !  "); 
	memset(&hv_803_main,0,sizeof(hv_803_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	memset(cTmpBuf_CLD, 0 , sizeof(cTmpBuf_CLD));	
	memset(cRecCnt, 0 , sizeof(cRecCnt));	
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*支付交易序号  一天只有一笔，暂时写死了*/
	
	hv_payin->T_operlevel[0] = pack803->PRI;			/*有限级别*/
	memcpy(hv_payin->T_cmtno,"803",3);					  /*CMT编号*/
	memcpy(hv_payin->T_tx_date,pack803->workDate,8);	/*交易日期*/
	iHVGETTAG( pack803,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*委托日期*/
	
	iHVGETTAG( pack803,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*收报中心代码*/	
	iHVGETTAG( pack803,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));	  /*接收行行号*/	
	iHVGETTAG( pack803,"CC3",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	  /*接收行行号*/	
	/*  透支利息合计  22 位 */
	iHVGETTAG( pack803,"0B4",cTmpBuf,sizeof(cTmpBuf));	
  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);	
	dTxamt = dTxamt/100;
  vtcp_log("[%s][%d] dTxamt=[%lf]:",__FILE__,__LINE__,dTxamt);		
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	 
  vtcp_log("hv_payin->T_tx_amt=[%.15s]:",hv_payin->T_tx_amt); 
  	
	/******** 处理附加域**/	
	iHVGETTAG(pack803,"CD4",hv_803_main.tag_CD4,sizeof(hv_803_main.tag_CD4));	/* 计息时点数 */
  vtcp_log("hv_803_main.tag_CD4=[%.4s] ",hv_803_main.tag_CD4 ); 
  memcpy(cRecCnt,hv_803_main.tag_CD4,4 );
  if(atoi(cRecCnt) > 30)
  {
     vtcp_log(" CD4  域记录数大于 MAX_803_REC，放不下了！！！  ");   	
	   return -1; 
  }
  
	iHVGETTAG(pack803,"CLD",hv_803_main.tag_CLD,sizeof(MAX_803_REC*56));	  /* 计息信息   */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_803_main,sizeof(HV_803_MAIN_ADD));
  vtcp_log("pub_com_CMT803_TAG2T() 成功结束!  "); 
	return 0; 
}



/*******************************************************************************
* 函数名称: iHvHang_Check_Brno                                                 *
* 描    述: 挂帐检查，主要是机构及状态 
            机构不存在，或状态不正常，则置为清算中心
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
*******************************************************************************/
int iHvHang_Check_Brno( char* cOpn_br_no, char* cKinbr)
{
	int  ret;
	char stat;
  struct com_branch_c sCom_branch;
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	
	if(cOpn_br_no)  /* 有开户行，则以帐号的开户行为准 */	
	{
    	memcpy(sCom_branch.br_no,cOpn_br_no,BRNO_LEN);
    	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
    	if(ret == 100)  /* 开户行不存在 */
    	{
        	vtcp_log("%s,%d 开户行不存在   ret=[%d]",__FILE__,__LINE__,ret);
    		  if(strlen(cKinbr) >0 ) /* 有接收机构号 */
    		  {
            	memcpy(sCom_branch.br_no,cKinbr,BRNO_LEN);
            	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
            	if(ret == 100)  /* 接收机构不存在*/
            	{
            		 vtcp_log("%s,%d 开户行不存在，接收机构也不存在,设置接收机构为清算中心ret=[%d]",__FILE__,__LINE__,ret);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);      		 
                 stat = RECEIVE_DATA_ERR;    /* 清算中心挂账  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0;    		 		
            	}
            	else if(ret)
            	{
                  sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
                  WRITEMSG
                  return -1;
            	}		
            	
            	if(sCom_branch.wrk_sts[0]=='*')
            	{
            		 vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);     		 
                 stat = RECEIVE_DATA_ERR;  /* 清算中心挂账  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0; 				
            	}

            	vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
        		  set_zd_data(DC_TX_BR_NO,cKinbr);	
        		  set_zd_data("0020",cKinbr);
        		  return 0;		  	
    		  }
    		  else  /* 没有接收机构号 */
    		  {
            		 vtcp_log("%s,%d 开户行不存在，接收机构也不存在,设置接收机构为清算中心ret=[%d]",__FILE__,__LINE__,ret);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);      		 
                 stat = RECEIVE_DATA_ERR;    /* 清算中心挂账  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0; 	
    		  }
    	}
    	else if(ret)
    	{
          sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
          WRITEMSG
          return -1;
    	}
    	if(sCom_branch.wrk_sts[0]=='*')
    	{
    		 vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
    		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
		     set_zd_data(DC_TX_BR_NO,cKinbr);	
		     set_zd_data("0020",cKinbr); 
         stat = RECEIVE_DATA_ERR;    /* 清算中心挂账  */
    		 set_zd_data("0980",&stat); 
		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
		     return 0; 
    	}
    	
   	 vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cOpn_br_no);
   	 /*memcpy(cKinbr, cOpn_br_no, BRNO_LEN);*/
		 set_zd_data(DC_TX_BR_NO,cOpn_br_no);	
		 set_zd_data("0020",cOpn_br_no);
		 return 0;
	}
	else   /* 没有开户行，则以接收机构号为准 */	
	{
		  if(strlen(cKinbr) >0 )  /* 接收机构号存在 */
		  {
        	memcpy(sCom_branch.br_no,cKinbr,BRNO_LEN);
        	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
        	if(ret == 100)
        	{
        		 vtcp_log("%s,%d 接收机构不存在,设置接收机构为清算中心ret=[%d]",__FILE__,__LINE__,ret);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);      		 
             stat = RECEIVE_DATA_ERR;    /* 清算中心挂账  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0;    		 		
        	}
        	else if(ret)
        	{
              sprintf(acErrMsg,"数据库操作失败[%d]",g_reply_int);
              WRITEMSG
              return -1;
        	}		
        	
        	if(sCom_branch.wrk_sts[0]=='*')
        	{
        		 vtcp_log("%s,%d 接收机构[%s]状态被关闭",__FILE__,__LINE__,sCom_branch.br_no);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);     		 
             stat = RECEIVE_DATA_ERR;   /* 清算中心挂账  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0; 				
        	}
        	
        	vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
    		  set_zd_data(DC_TX_BR_NO,cKinbr);	
    		  set_zd_data("0020",cKinbr);
    		  return 0;		  	
		  }
		  else  /* 接收机构号不存在 */
		  {
        		 vtcp_log("%s,%d 开户机构不存在误，来帐机构也不存在，设置清算中心为交易机构 ",__FILE__,__LINE__);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);     		 
             stat = RECEIVE_DATA_ERR;       /* 接收机构不存时，清算挂账  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d 得到新的交易机构===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0; 		
		  }
	}

  return 0;
}



/*******************************************************************************
* 函数名称: iHvHang_Check_Acno                                                 *
* 描    述: 挂帐检查，检查帐号及状态，以及户名 
            设置好一些8583域，为走帐准备
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
*******************************************************************************/
int iHvHang_Check_Acno(char* ac_no, char* ac_name, char* cKinbr)
{
  char stat;
  char cGZflag[2];
  int ret;
  struct com_branch_c sCom_branch;
	char cQs_br_no[BRNO_LEN+1];
  char  acc_v[33];
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;  
  int   ac_seqn_v; 
	char    cTempstr    [1024];
	
	char cOldStat[2];
  memset(cOldStat, 0, sizeof(cOldStat));        
	get_zd_data("0980",cOldStat);     /* 记录原状态 */
  vtcp_log("[%s][%d] cOldStat===[%s] ",__FILE__,__LINE__,cOldStat);
  	
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));  
  memset(acc_v,0x00,sizeof(acc_v));  

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
  if(pub_base_old_acno(acc_v))
  {
	   sprintf(acErrMsg,"pub_base_old_acno() 出错!! [%d]");
		 WRITEMSG
		return -1;
	}
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);

    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)   /* 1. 帐号不存时 */
    {
	      vtcp_log("%s,%d  账号[%s]不存在,支行挂账  ",__FILE__,__LINE__,acc_v);    	
              
			  set_zd_data("0680","6"); /* 帐户类型 */        
        /* 帐号不存时，设置挂帐机构 */
        ret = iHvHang_Check_Brno(0, cKinbr);
        if(ret)
        {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
        }
        get_zd_data(DC_TX_BR_NO,cKinbr);             
        vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);         
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        } 

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    vtcp_log("[%s][%d] cKinbr===[%s]  QS_BR_NO=[%s]",__FILE__,__LINE__,cKinbr,QS_BR_NO); 
    vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);    


    /* 2. 帐号存时，查找帐号的开户机构  机构号以帐号的开户机构为准    */
    ret = iHvHang_Check_Brno(mdm_ac_rel_v.opn_br_no, cKinbr);
    if(ret)
    {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
    }
    get_zd_data(DC_TX_BR_NO,cKinbr);             
    vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);

   /*  在这里如果机构号被重置，表示没有找到机构号，所以清算挂帐   
    if( memcmp(cKinbr,mdm_ac_rel_v.opn_br_no, BRNO_LEN) )  
    {
        stat = RECEIVE_CUSTNAME_ERR;
	      vtcp_log("%s,%d  机构号被重置: opn_br_no=[%s]     cKinbr=[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no,cKinbr);        
 			  set_zd_data("0680","6");        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }   */  
    
    /* ============= 以下机构号已确定 ============= */

    /*  判断介质状态 */
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
    {
        /* 介质状态不正常挂帐处理 */

	      vtcp_log("%s,%d  账号[%s]介质状态错误  ",__FILE__,__LINE__,acc_v);        
 			  set_zd_data("0680","6");/* 帐户类型 */        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }

    /*  是否活期存款户 */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)  /* 非活期存款户 */
    {
			  set_zd_data("0680","6");/* 帐户类型 */           
        vtcp_log("大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 

        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    /* 内部帐户不检查户名 */
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
        ac_seqn_v  =  1; 
        set_zd_data("0680","1");
                   
        g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
        if (g_reply_int )
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
            WRITEMSG
            return -1;
        }
        if (dd_mst_v.ac_sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  

            set_zd_data("0680","6");/* 帐户类型 */
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
        /*  判断是否被冻结  */
        if(dd_mst_v.hold_sts[0]=='1' )
        {
            /* 帐号id被完全冻结所以要挂帐 */
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
                
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        

        /*  判断是否是结算户 */
        if(dd_mst_v.sttl_type[0]=='S')
        {
            sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
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
            sprintf(acErrMsg,"大额来账收款人名称不正确,账号ac_id[%ld]户名[%s]错误户名[%s]", dd_mst_v.ac_id,dd_mst_v.name,ac_name);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  

            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }
    else /* 检查内部帐户 */
    {
        struct in_mst_c sIn_mst;
        memset(&sIn_mst,0,sizeof(sIn_mst));
        set_zd_data("0680","6");/* 设置帐户类型 */
                
        g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
        if (g_reply_int)
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
            WRITEMSG
            return -1;
        }
        
        if (sIn_mst.sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常 挂帐! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 设置帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }

    memset(cOldStat, 0, sizeof(cOldStat));        
	  get_zd_data("0980",cOldStat);     /* 记录原状态 */
    vtcp_log("[%s][%d] 没有挂帐，状态应该不变=='9' ?  cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	

	  vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
	  return 0;	
}


/*******************************************************************************
* 函数名称: iHvHang_Check_AcnoNoName                                                 *
* 描    述: 挂帐检查，只检查帐号及状态 
            设置好一些8583域，为走帐准备
* 参    数:                                                                    *
* 类    型:                                                                    *
* 返 回 值:int                                                                 *
*******************************************************************************/
int iHvHang_Check_AcnoNoName(char* ac_no, char* cKinbr)
{
  char stat;
  char cGZflag[2];
  int ret;
  struct com_branch_c sCom_branch;
	char cQs_br_no[BRNO_LEN+1];
  char  acc_v[33];
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;  
  int   ac_seqn_v; 

	char cOldStat[2];
  memset(cOldStat, 0, sizeof(cOldStat));        
	get_zd_data("0980",cOldStat);     /* 记录原状态 */
  vtcp_log("[%s][%d] cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	
  	
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));  
  memset(acc_v,0x00,sizeof(acc_v));  

  /* ========= 收款人账号检查开始  ==========*/
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
  memcpy(acc_v, ac_no ,sizeof(acc_v)-1);
  
  /*旧账号转新账号*/
  if(pub_base_old_acno(acc_v))
  {
	   sprintf(acErrMsg,"pub_base_old_acno() 出错!! [%d]");
		 WRITEMSG
		return -1;
	}
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);

    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)   /* 1. 帐号不存时 */
    {
	      vtcp_log("%s,%d  账号[%s]不存在,支行挂账  ",__FILE__,__LINE__,acc_v);    	
              
			  set_zd_data("0680","6"); /* 帐户类型 */        
        /* 帐号不存时，设置挂帐机构 */
        ret = iHvHang_Check_Brno(0, cKinbr);
        if(ret)
        {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
        }
        get_zd_data(DC_TX_BR_NO,cKinbr);             
        vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);         
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        } 

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    vtcp_log("[%s][%d] cKinbr===[%s]  QS_BR_NO=[%s]",__FILE__,__LINE__,cKinbr,QS_BR_NO); 
    vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);    


    /* 2. 帐号存时，查找帐号的开户机构  机构号以帐号的开户机构为准    */
    ret = iHvHang_Check_Brno(mdm_ac_rel_v.opn_br_no, cKinbr);
    if(ret)
    {
           sprintf(acErrMsg,"[%s]数据库操作失败[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
    }
    get_zd_data(DC_TX_BR_NO,cKinbr);             
    vtcp_log("%s,%d 得到的交易机构是[%s]",__FILE__,__LINE__,cKinbr);

   /*  在这里如果机构号被重置，表示没有找到机构号，所以清算挂帐   
    if( memcmp(cKinbr,mdm_ac_rel_v.opn_br_no, BRNO_LEN) )  
    {
        stat = RECEIVE_CUSTNAME_ERR;
	      vtcp_log("%s,%d  机构号被重置: opn_br_no=[%s]     cKinbr=[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no,cKinbr);        
 			  set_zd_data("0680","6");        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }   */  
    
    /* ============= 以下机构号已确定 ============= */

    /*  判断介质状态 */
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
    {
        /* 介质状态不正常挂帐处理 */

	      vtcp_log("%s,%d  账号[%s]介质状态错误  ",__FILE__,__LINE__,acc_v);        
 			  set_zd_data("0680","6");/* 帐户类型 */        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }

    /*  是否活期存款户 */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)  /* 非活期存款户 */
    {
			  set_zd_data("0680","6");/* 帐户类型 */           
        vtcp_log("大额来账收款人账号非活期存款户[%s],挂账!",acc_v);
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 

        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        sprintf(acErrMsg,"大额来账检查收款人账号[%s]数据库操作失败[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    /* 内部帐户不检查户名 */
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
        ac_seqn_v  =  1; 
        set_zd_data("0680","1");
                   
        g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
        if (g_reply_int )
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号活期主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
            WRITEMSG
            return -1;
        }
        
        if (dd_mst_v.ac_sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  

            set_zd_data("0680","6");/* 帐户类型 */
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
        /*  判断是否被冻结  */
        if(dd_mst_v.hold_sts[0]=='1' )
        {
            /* 帐号id被完全冻结所以要挂帐 */
            sprintf(acErrMsg,"大额来账收款人账号状态不正常ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
                
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        

        /*  判断是否是结算户 */
        if(dd_mst_v.sttl_type[0]=='S')
        {
            sprintf(acErrMsg,"大额来账收款人账号非结算户sttl_type[%s]",dd_mst_v.sttl_type);
            WRITEMSG
            set_zd_data("0680","6");/* 帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }

    }
    else /* 检查内部帐户 */
    {
        struct in_mst_c sIn_mst;
        memset(&sIn_mst,0,sizeof(sIn_mst));
        set_zd_data("0680","6");/* 设置帐户类型 */
                
        g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
        if (g_reply_int)
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"查询大额来账收款人账号内部帐主文件错sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
            WRITEMSG
            return -1;
        }
        
        if (sIn_mst.sts[0] != '1' )
        {
            sprintf(acErrMsg,"大额来账收款人账号状态不正常 挂帐! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* 设置帐户类型 */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* 支行挂账  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d 系统配置错误，得到机构%s的大额挂帐帐号错误[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }

    memset(cOldStat, 0, sizeof(cOldStat));        
	  get_zd_data("0980",cOldStat);     /* 记录原状态 */
    vtcp_log("[%s][%d] 没有挂帐，状态应该不变=='9' ?  cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	

	  vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
    set_zd_data("0300", acc_v );  /* 收款人账号 */ 
	  return 0;	
}


int iHv_HPCMT_proc( THVPACKAGE* phv_pack)
{
    int ret;
    struct hv_poinfo_c hv_poinfo;   
    char cPay_br_no[13];
    char cPo_no[9];
    char cLast_sts;
    
     memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
     memset(cPay_br_no, 0 , sizeof(cPay_br_no));    
     memset(cPo_no, 0 , sizeof(cPo_no));    
     hv_get_tag(phv_pack,"CC4",cPay_br_no);
     hv_get_tag(phv_pack,"21A",cPo_no);  
     vtcp_log("%s,%d cPay_br_no===[%s]",__FILE__,__LINE__,cPay_br_no);    
     vtcp_log("%s,%d cPo_no    ===[%s]",__FILE__,__LINE__,cPo_no);      
      
    ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply," po_no='%s' and pay_br_no='%s' ", cPo_no, cPay_br_no);
    if (ret)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    ret = Hv_poinfo_Fet_Upd(&hv_poinfo, g_pub_tx.reply);
    if (ret == 100)
    {
        sprintf(acErrMsg,"汇票登记表中无此记录!![%d]",ret);
        WRITEMSG
        Hv_poinfo_Clo_Upd();     
        return 0;
    }else if(ret )
    {
        sprintf(acErrMsg,"查询汇票登记表错误!![%d]",ret);
        WRITEMSG
        return ret;
    }

    cLast_sts = hv_poinfo.po_sts[0]; /* 汇票的上一次状态 */
    hv_poinfo.l_tx_date = g_pub_tx.tx_date;
    hv_poinfo.td_cnt ++;
    
    vtcp_log("[%s][%d] CMTCode== [%s] ",__FILE__,__LINE__,phv_pack->CMTCode);   
    if(memcmp(phv_pack->CMTCode,"122",sizeof(phv_pack->CMTCode) )==0)
    {
        if(hv_poinfo.po_sts[0] == HV_HP_JQ) /* 725先收到 已全额结清 */
        {
            vtcp_log(" [%s][%d] 已全额结清 725先收到 不用更新了!  ",__FILE__,__LINE__); 
            Hv_poinfo_Clo_Upd();  
            return 0; 
        }
        else if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH )  /* 申请兑付 */  
        {
            hv_poinfo.po_sts[0] = HV_HP_CASH;           /* 汇票兑付 */
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        }        
    }else if(memcmp(phv_pack->CMTCode,"123",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
            hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_CASH   )  
        {
           hv_poinfo.po_sts[0] =  HV_HP_PARTBACK;   /* 部分退回 */
           double dCash_amt=0.0;
           double dRem_amt=0.0;
           char   cCash_amt[16];
           /*char   cRem_amt[16];  del by  martin 2009/4/29*/
			     char   cRem_amt[19];
			     char   cTemp_amt[16];/**add by martin 2009/4/29**/

           memset(cCash_amt,0, sizeof(cCash_amt) );        
           memset(cRem_amt,0, sizeof(cRem_amt) ); 
           
           hv_get_tag(phv_pack,"32A",cRem_amt);   /* 退回金额==剩余金额*/
           hv_get_tag(phv_pack,"CNV",cCash_amt); /* 实际结算金额*/
           memcpy(cTemp_amt,cRem_amt+3,sizeof(cTemp_amt)-1);
           
           vtcp_log("[%s][%d]   cCash_amt===[%s]  ",__FILE__,__LINE__, cCash_amt);             
           vtcp_log("[%s][%d]   cRem_amt===[%s]  ",__FILE__,__LINE__, cRem_amt);      
           vtcp_log("[%s][%d]   cTemp_amt===[%s]  ",__FILE__,__LINE__,cTemp_amt );
               
            /*str2dbl(cCash_amt,strlen(cCash_amt),0,&dCash_amt);  实际结算金额   */
            str2dbl(cCash_amt,strlen(cTemp_amt),0,&dCash_amt); /* 实际结算金额  modified by martin */
            /*str2dbl(cRem_amt,strlen(cRem_amt),0,&dRem_amt);     退回金额==剩余金额*/  
            str2dbl(cTemp_amt,strlen(cTemp_amt),0,&dRem_amt); /**退回金额 modified by martin*/
            dCash_amt = dCash_amt/100;
            dRem_amt = dRem_amt/100;   
           vtcp_log("[%s][%d]   dCash_amt===[%lf]  ",__FILE__,__LINE__, dCash_amt);             
           vtcp_log("[%s][%d]   dRem_amt===[%lf]  ",__FILE__,__LINE__, dRem_amt);             
           hv_poinfo.cash_amt = dCash_amt ;   /* 实际结算金额 */           
           hv_poinfo.rem_amt = dRem_amt;   /* 退回金额 */ 
           hv_poinfo.pay_date =  g_pub_tx.tx_date;         
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        } 
    }else if(memcmp(phv_pack->CMTCode,"124",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_CASH ||
            hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER )  
        {
            long iPay_date;
            vtcp_log("[%s][%d]   hv_poinfo.sign_date===[%d]  ",__FILE__,__LINE__, hv_poinfo.sign_date);      
            vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
            pub_base_deadlineM(hv_poinfo.sign_date,1,&iPay_date);
            vtcp_log("[%s][%d]   g_pub_tx.tx_date===[%d]  ",__FILE__,__LINE__, g_pub_tx.tx_date);      
            vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
            if(iPay_date < g_pub_tx.tx_date)
            {
               hv_poinfo.po_sts[0] = HV_HP_OVERDATE_UNTREAD;  /* 逾期未用退回 */
            }
            else
            {
               hv_poinfo.po_sts[0] = HV_HP_FULLBACK;  /* 未用退回 */
            }  
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        } 

    }else if(memcmp(phv_pack->CMTCode,"725",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER || hv_poinfo.po_sts[0] == HV_HP_CASH)  
        {
        	 hv_poinfo.pay_date =  g_pub_tx.tx_date; 
           hv_poinfo.po_sts[0] =  HV_HP_JQ;  /* 全额结清 */
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," 汇票记录当前状态不对 ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        }
    }else
    {
        sprintf(acErrMsg,"CMTCode== [%s] 不在这里处理! ",phv_pack->CMTCode);
        WRITEMSG
        return -1;
    }
    hv_poinfo.l_po_sts[0] = cLast_sts;
    ret = Hv_poinfo_Upd_Upd(hv_poinfo, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"HV_POINFO_UPD_UPD ERROR ! [%d]",ret);
        WRITEMSG
        return ret;
    }    
    Hv_poinfo_Clo_Upd();  
    vtcp_log("[%s][%d] 更新支付交易表记录成功",__FILE__,__LINE__);      
    return 0;  
}

