/***************************************************************************/
/* 文件名称: lv_tabcomm.pc                                                 */
/* 功能描述: 小额支付数据库表操作公共函数                                  */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     石升辉                                                        */
/* 说明:                                                                   */
/***************************************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lv_define.h"
#include "hv_define.h"
#include "lvpack.h"
#include "exitlvpack.h"
#include "lv_pub.h"
#include "public.h"
#include "lv_pkgreg_c.h"
#include "lv_pkghtr_c.h"
#include "hv_orno_brno_c.h"

char *lv_date(long);
extern char * pcLvGetDay();
/***拷贝基本表数据到交换区的宏定义**/
/*#define LV_CP_PAY_IN_FIELD(wp_payin,wp_table)	\*/
void LV_CP_PAY_IN_FIELD(PAY_IN_AREA *wp_payin, struct lv_pkgreg_c *wp_table)
{
	/* NEWYX begin add by LiuHuafeng 2007-4-4 7:20 */
	wp_payin->T_lw_ind[0]=wp_table->lw_ind[0];
	/* NEWYX end by LiuHuafeng 2007-4-4 7:20 */
	memcpy(wp_payin->T_pkgno,wp_table->pkgno,sizeof(wp_payin->T_pkgno)); 
	memcpy(wp_payin->T_orderno,wp_table->orderno,sizeof(wp_payin->T_orderno)); 
	memcpy(wp_payin->T_txnum,wp_table->txnum,sizeof(wp_payin->T_txnum)); 
	memcpy(wp_payin->T_orbrno,wp_table->or_br_no,sizeof(wp_payin->T_orbrno)); 
	memcpy(wp_payin->T_payqsactno,wp_table->pay_qs_no,sizeof(wp_payin->T_payqsactno)); 
	memcpy(wp_payin->T_sendco,wp_table->sendco,sizeof(wp_payin->T_sendco)); 
	memcpy(wp_payin->T_acbrno,wp_table->ac_br_no,sizeof(wp_payin->T_acbrno)); 
	memcpy(wp_payin->T_cashqsactno,wp_table->cash_qs_no,sizeof(wp_payin->T_cashqsactno)); 
	memcpy(wp_payin->T_receco,wp_table->receco,sizeof(wp_payin->T_receco)); 

	memcpy(wp_payin->T_payno,wp_table->pay_opn_br_no,sizeof(wp_payin->T_payno)); 
	memcpy(wp_payin->T_payactno,wp_table->pay_ac_no,sizeof(wp_payin->T_payactno)); 
	memcpy(wp_payin->T_payname,wp_table->pay_name,sizeof(wp_payin->T_payname)); 
	memcpy(wp_payin->T_payaddress,wp_table->pay_addr,sizeof(wp_payin->T_payaddress)); 
	memcpy(wp_payin->T_cashno,wp_table->cash_opn_br_no,sizeof(wp_payin->T_cashno)); 
	memcpy(wp_payin->T_cashactno,wp_table->cash_ac_no,sizeof(wp_payin->T_cashactno)); 
	memcpy(wp_payin->T_cashname,wp_table->cash_name,sizeof(wp_payin->T_cashname)); 
	memcpy(wp_payin->T_cashaddress,wp_table->cash_addr,sizeof(wp_payin->T_cashaddress));
	memcpy(wp_payin->T_brno,wp_table->br_no,sizeof(wp_payin->T_brno)); 
	memcpy(wp_payin->T_voctype,wp_table->note_type,sizeof(wp_payin->T_voctype)); 
	memcpy(wp_payin->T_vocnum,wp_table->note_no,sizeof(wp_payin->T_vocnum)); 
	vtcp_log("%s,%d fetch from table brno is[%s] [%s]",__FILE__,__LINE__,wp_payin->T_brno,wp_table->br_no);

	memcpy(wp_payin->T_brprtflag,wp_table->brprtflag,sizeof(wp_payin->T_brprtflag)); 
	memcpy(wp_payin->T_qsprtflag,wp_table->qsprtflag,sizeof(wp_payin->T_qsprtflag)); 

	memcpy(wp_payin->T_o_packid,wp_table->o_packid,sizeof(wp_payin->T_o_packid)); 
	vtcp_log("[%s][%d]wp_payin->T_o_packid =[%.8s]",__FILE__,__LINE__,wp_payin->T_o_packid);

	vtcp_log("[%s][%d]wp_payin->T_o_pack_date =[%s]",__FILE__,__LINE__,wp_payin->T_o_pack_date);
	memcpy(wp_payin->T_packid,wp_table->packid,sizeof(wp_payin->T_packid)); 
	vtcp_log("[%s][%d]wp_payin->T_packid =[%.8s]",__FILE__,__LINE__,wp_payin->T_packid);

	vtcp_log("[%s][%d]wp_payin->T_packday =[%s]",__FILE__,__LINE__,wp_payin->T_packday);

	memcpy(wp_payin->T_content,wp_table->lv_brf,sizeof(wp_payin->T_content)); 
	memcpy(wp_payin->T_filler,wp_table->filler,sizeof(wp_payin->T_filler));

	memcpy(wp_payin->T_o_or_br_no,wp_table->o_or_br_no,sizeof(wp_payin->T_o_or_br_no)); 
	vtcp_log("[%s][%d]wp_table->o_or_br_no =[%.12s]",__FILE__,__LINE__,wp_table->o_or_br_no); 
	memcpy(wp_payin->T_o_ac_br_no,wp_table->o_ac_br_no,sizeof(wp_payin->T_o_ac_br_no));
	vtcp_log("[%s][%d]wp_table->T_o_ac_br_no =[%.12s]",__FILE__,__LINE__,wp_table->o_ac_br_no);
	memcpy(wp_payin->T_dfxyno,wp_table->protno,sizeof(wp_payin->T_dfxyno)); 

	memcpy(wp_payin->T_ywtype,wp_table->lv_yw_ind,sizeof(wp_payin->T_ywtype));	
	vtcp_log("[%s][%d]lv_yw_ind=[%s]\n",__FILE__,__LINE__,wp_table->lv_yw_ind);	
	vtcp_log("[%s][%d]T_ywtype=[%s]\n",__FILE__,__LINE__,wp_payin->T_ywtype);	

	memcpy(wp_payin->T_rcpstat,wp_table->rcpstat,	sizeof(wp_payin->T_rcpstat));	
	memcpy(wp_payin->T_resp1,wp_table->resp1,	sizeof(wp_payin->T_resp1));			
	memcpy(wp_payin->T_resp2,wp_table->resp2,	sizeof(wp_payin->T_resp2));	
	memcpy(wp_payin->T_addid,wp_table->addid,	sizeof(wp_payin->T_addid));	
	memcpy(wp_payin->T_inputno,wp_table->input_tel,	sizeof(wp_payin->T_inputno));	
	memcpy(wp_payin->T_checkno,wp_table->check_tel,	sizeof(wp_payin->T_checkno));	
	memcpy(wp_payin->T_authno,wp_table->auth_tel, 	sizeof(wp_payin->T_authno));	
	memcpy(wp_payin->T_sendno,wp_table->send_tel,		sizeof(wp_payin->T_sendno));	
	memcpy(wp_payin->T_respno,wp_table->resp_tel,		sizeof(wp_payin->T_respno));	
	memcpy(wp_payin->T_inwssrno,wp_table->input_trace_no,sizeof(wp_payin->T_inwssrno));	
	memcpy(wp_payin->T_checkwssrno,wp_table->check_trace_no,sizeof(wp_payin->T_checkwssrno));	
	memcpy(wp_payin->T_sendwssrno,wp_table->send_trace_no,sizeof(wp_payin->T_sendwssrno));	
	memcpy(wp_payin->T_respwssrno,wp_table->resp_trace_no,sizeof(wp_payin->T_respwssrno));	

	memcpy(wp_payin->T_dealno,wp_table->deal_tel,	sizeof(wp_payin->T_dealno));
	memcpy(wp_payin->T_dealwssrno,wp_table->deal_trace_no,	sizeof(wp_payin->T_dealwssrno));
	memcpy(wp_payin->T_oorderno,wp_table->o_orderno,	sizeof(wp_payin->T_oorderno));
	memcpy(wp_payin->T_otxnum,wp_table->o_txnum,	sizeof(wp_payin->T_otxnum));
	memcpy(wp_payin->T_o_or_br_no,wp_table->o_or_br_no,		sizeof(wp_payin->T_o_or_br_no));
	vtcp_log("[%s][%d]wd_lv_djths.o_or_br_no=[%.12s]\n",__FILE__,__LINE__,wp_table->o_or_br_no);
	memcpy(wp_payin->T_retcode,wp_table->retcode,		sizeof(wp_payin->T_retcode));
	memcpy(wp_payin->T_ywdtlid,wp_table->ywdtlid,		sizeof(wp_payin->T_ywdtlid));

	dbl2str(&wp_table->tx_amt,2,sizeof(wp_payin->T_txamt),0,wp_payin->T_txamt);
	dbl2str(&wp_table->chrg_amt,0,sizeof(wp_payin->T_feeamt),0,	wp_payin->T_feeamt);

	apitoa(wp_table->tx_time,sizeof(wp_payin->T_txtime),wp_payin->T_txtime);
	apitoa(wp_table->resp_date,8,wp_payin->T_respday);
	apitoa(wp_table->o_wt_date,8,wp_payin->T_owtday);
	apitoa(wp_table->jz_date,8,wp_payin->T_jzday);
	apitoa(wp_table->pack_date,8,wp_payin->T_packday);
	apitoa(wp_table->o_pack_date,8,wp_payin->T_o_pack_date);
	apitoa(wp_table->in_date,8,wp_payin->T_inday);
	apitoa(wp_table->wt_date,8,wp_payin->T_wtday); 

	wp_payin->T_feeflag[0]  = wp_table->tx_chrg_ind[0];
	wp_payin->T_begstat[0]  = wp_table->beg_sts[0];
	wp_payin->T_ostat[0]    = wp_table->lv_o_sts[0] ;
	wp_payin->T_stat[0]     = wp_table->lv_sts[0]; 
	vtcp_log("[%s][%d]lv_sts=[%c]beg_sts=[%c]",__FILE__,__LINE__,wp_table->lv_sts[0],wp_table->beg_sts[0]);
	vtcp_log("[%s][%d]T_stat=[%c]T_begstat=[%c]",__FILE__,__LINE__,wp_payin->T_stat[0],wp_payin->T_begstat[0]);
	wp_payin->T_operlevel[0]= wp_table->operlevel[0]; 
	wp_payin->T_checkflag[0]= wp_table->checkflag[0]; 
	wp_payin->T_actype[0]   = wp_table->ac_type[0];
	wp_payin->T_lw_ind[0]   = wp_table->lw_ind[0];
	vtcp_log("[%s][%d]lv_yw_ind=[%s]\n",__FILE__,__LINE__,wp_table->lv_yw_ind);	
	vtcp_log("[%s][%d]T_ywtype=[%s]\n",__FILE__,__LINE__,wp_payin->T_ywtype);	

	/****Add by SSH,2005.7.27,添加操作员和流水号****/
	if (memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))==0)
	{
		get_fd_data(DC_TEL,wp_payin->T_checkno);
		get_fd_data(DC_TRACE_NO,wp_payin->T_checkwssrno);
		memcpy(wp_payin->T_authno,wp_payin->F_authno,
			sizeof(wp_payin->T_authno));
	}else if(memcmp(wp_payin->F_opcd,OPCD_SEND,sizeof(wp_payin->F_opcd))==0)
	{
		get_fd_data(DC_TEL,wp_payin->T_sendno);
		get_fd_data(DC_TRACE_NO,wp_payin->T_sendwssrno);
	}else if(memcmp(wp_payin->F_opcd,OPCD_RESP,sizeof(wp_payin->F_opcd))==0)
	{
		get_fd_data(DC_TEL,wp_payin->T_respno);
		memcpy(wp_payin->T_respwssrno,wp_payin->A_wssrno,
			sizeof(wp_payin->T_respwssrno));
	}
}
/*****************************************************************************/	
void LV_CP_PAY_IN_TABLE( PAY_IN_AREA *wp_payin,struct lv_pkgreg_c *wp_table)
{
	/* NEWYX begin by LiuHuafeng 2007-4-4 7:19 */
	wp_table->lw_ind[0]=wp_payin->T_lw_ind[0];
	/* NEWYX end   by LiuHuafeng 2007-4-4 7:19 */
	memcpy(wp_table->pkgno     ,wp_payin->T_pkgno  ,sizeof(wp_payin->T_pkgno)); 
	memcpy(wp_table->orderno   ,wp_payin->T_orderno,sizeof(wp_payin->T_orderno));
	memcpy(wp_table->protno    ,wp_payin->T_dfxyno ,sizeof(wp_payin->T_dfxyno));
	memcpy(wp_table->sendco    ,wp_payin->T_sendco ,sizeof(wp_payin->T_sendco)); 
	memcpy(wp_table->ac_br_no  ,wp_payin->T_acbrno ,sizeof(wp_payin->T_acbrno)); 
	memcpy(wp_table->lv_brf    ,wp_payin->T_content,sizeof(wp_payin->T_content)); 
	memcpy(wp_table->filler    ,wp_payin->T_filler ,sizeof(wp_payin->T_filler));
	memcpy(wp_table->receco    ,wp_payin->T_receco ,sizeof(wp_payin->T_receco));
	memcpy(wp_table->note_no   ,wp_payin->T_vocnum ,sizeof(wp_payin->T_vocnum)); 
	memcpy(wp_table->pay_name  ,wp_payin->T_payname,sizeof(wp_payin->T_payname)); 
	memcpy(wp_table->txnum     ,wp_payin->T_txnum  ,sizeof(wp_payin->T_txnum)); 
	memcpy(wp_table->or_br_no  ,wp_payin->T_orbrno ,sizeof(wp_payin->T_orbrno)); 
	memcpy(wp_table->br_no     ,wp_payin->T_brno   ,sizeof(wp_payin->T_brno)); 
	memcpy(wp_table->note_type ,wp_payin->T_voctype ,sizeof(wp_payin->T_voctype));
	memcpy(wp_table->cash_name ,wp_payin->T_cashname,sizeof(wp_payin->T_cashname)); 
	memcpy(wp_table->o_packid  ,wp_payin->T_o_packid,sizeof(wp_payin->T_o_packid)); 
	vtcp_log("[%s][%d]T_o_packid=[%s]",__FILE__,__LINE__,wp_payin->T_o_packid);
	memcpy(wp_table->pay_ac_no ,wp_payin->T_payactno,sizeof(wp_payin->T_payactno));
	memcpy(wp_table->o_or_br_no,wp_payin->T_o_or_br_no,sizeof(wp_payin->T_o_or_br_no)); 
	memcpy(wp_table->o_ac_br_no,wp_payin->T_o_ac_br_no,sizeof(wp_payin->T_o_ac_br_no));
	memcpy(wp_table->pay_qs_no ,wp_payin->T_payqsactno,sizeof(wp_payin->T_payqsactno));

	memcpy(wp_table->pay_addr  ,wp_payin->T_payaddress,sizeof(wp_payin->T_payaddress)); 
	memcpy(wp_table->cash_qs_no,wp_payin->T_cashqsactno,sizeof(wp_payin->T_cashqsactno)); 
	memcpy(wp_table->cash_ac_no,wp_payin->T_cashactno,sizeof(wp_payin->T_cashactno)); 
	memcpy(wp_table->cash_addr ,wp_payin->T_cashaddress,sizeof(wp_payin->T_cashaddress)); 
	memcpy(wp_table->brprtflag ,wp_payin->T_brprtflag,sizeof(wp_payin->T_brprtflag)); 
	memcpy(wp_table->qsprtflag ,wp_payin->T_qsprtflag,sizeof(wp_payin->T_qsprtflag));
	memcpy(wp_table->packid    ,wp_payin->T_packid,sizeof(wp_payin->T_packid)); 
	memcpy(wp_table->lv_yw_ind,wp_payin->T_ywtype,sizeof(wp_payin->T_ywtype));	
	memcpy(wp_table->rcpstat,wp_payin->T_rcpstat ,sizeof(wp_payin->T_rcpstat));	
	memcpy(wp_table->resp1,wp_payin->T_resp1,		 sizeof(wp_payin->T_resp1));	
	memcpy(wp_table->resp2,wp_payin->T_resp2,		 sizeof(wp_payin->T_resp2));	
	memcpy(wp_table->addid,wp_payin->T_addid,		 sizeof(wp_payin->T_addid));	
	memcpy(wp_table->input_tel,wp_payin->T_inputno,sizeof(wp_payin->T_inputno));	
	memcpy(wp_table->check_tel,wp_payin->T_checkno,sizeof(wp_payin->T_checkno));	
	memcpy(wp_table->auth_tel,wp_payin->T_authno,	sizeof(wp_payin->T_authno));	
	memcpy(wp_table->send_tel,wp_payin->T_sendno,	sizeof(wp_payin->T_sendno));	
	memcpy(wp_table->resp_tel,wp_payin->T_respno,	sizeof(wp_payin->T_respno));	
	memcpy(wp_table->deal_tel,wp_payin->T_dealno,		sizeof(wp_payin->T_dealno));
	memcpy(wp_table->o_txnum,wp_payin->T_otxnum,     sizeof(wp_payin->T_otxnum));
	memcpy(wp_table->retcode,wp_payin->T_retcode,		sizeof(wp_payin->T_retcode));
	memcpy(wp_table->ywdtlid,wp_payin->T_ywdtlid,		sizeof(wp_payin->T_ywdtlid));
	memcpy(wp_table->send_trace_no,wp_payin->T_sendwssrno,sizeof(wp_payin->T_sendwssrno));	
	memcpy(wp_table->resp_trace_no,wp_payin->T_respwssrno,sizeof(wp_payin->T_respwssrno));	
	memcpy(wp_table->deal_trace_no,wp_payin->T_dealwssrno,sizeof(wp_payin->T_dealwssrno));
	memcpy(wp_table->input_trace_no,wp_payin->T_inwssrno ,sizeof(wp_payin->T_inwssrno));	
	memcpy(wp_table->check_trace_no,wp_payin->T_checkwssrno,sizeof(wp_payin->T_checkwssrno));

	memcpy(wp_table->cash_opn_br_no,wp_payin->T_cashno,sizeof(wp_payin->T_cashno));
	memcpy(wp_table->pay_opn_br_no,wp_payin->T_payno,sizeof(wp_payin->T_payno)); 

	/***add by xyy 2006-9-13 16:53如果是退汇oorderno放的是要退的那笔的序号，
	如果是回执oorderno放的就是来帐的orderno,其他情况置空***/
	if (memcmp(wp_payin->T_pkgno,"007",3)!=0 && memcmp(wp_payin->T_pkgno,"008",3)!=0 \
		&&memcmp(wp_payin->T_pkgno,"009",3)!=0 &&memcmp(wp_payin->T_pkgno,"010",3)!=0 \
		&&memcmp(wp_payin->T_pkgno,"011",3)!=0)
	{
		memcpy(wp_table->o_orderno,"        ",8); 
		vtcp_log("[%s][%d]wp_payin->T_oorderno =[%.8s]",__FILE__,__LINE__,wp_payin->T_oorderno);
	}
	else
		memcpy(wp_table->o_orderno,wp_payin->T_oorderno,sizeof(wp_payin->T_oorderno));
	vtcp_log("[%s][%d]=wp_payin->T_oorderno=[%s]\n",__FL__,wp_payin->T_oorderno);

	dot_str2dbl(wp_payin->T_txamt,sizeof(wp_payin->T_txamt),&wp_table->tx_amt);
	wp_table->tx_amt      = wp_table->tx_amt/100.0;
	str2dbl(wp_payin->T_feeamt,sizeof(wp_payin->T_feeamt),0,&wp_table->chrg_amt);
	wp_table->o_wt_date = apatoi(wp_payin->T_owtday, 8);
	wp_table->resp_date = apatoi(wp_payin->T_respday, 8);
	wp_table->jz_date   = apatoi(wp_payin->T_jzday, 8);
	wp_table->chrg_amt  = wp_table->chrg_amt/100.0;
	wp_table->beg_sts[0] = wp_payin->T_begstat[0];
	wp_table->tx_chrg_ind[0]=wp_payin->T_feeflag[0];
	wp_table->lv_o_sts[0] =wp_payin->T_ostat[0];
	vtcp_log("[%s][%d]tx_amt=[%f];",__FILE__,__LINE__,wp_table->tx_amt);
	wp_table->o_pack_date = apatoi(wp_payin->T_o_pack_date,8);
	wp_table->pack_date   = apatoi(wp_payin->T_packday,8);
	wp_table->in_date     = apatoi(wp_payin->T_inday,8); 
	wp_table->wt_date     = apatoi(wp_payin->T_wtday,8); 
	wp_table->lw_ind[0]   = wp_payin->T_lw_ind[0];
	wp_table->checkflag[0]= wp_payin->T_checkflag[0];
	wp_table->lv_sts[0]   = wp_payin->T_stat[0]; 
	wp_table->operlevel[0]= wp_payin->T_operlevel[0]; 
	get_zd_long(DC_TIME,&wp_table->tx_time);
	vtcp_log("[%s][%d]tx_time=[%ld]",__FILE__,__LINE__,wp_table->tx_time);
}
/*****************************************************************************/	

/***************************************************************************/
/* 函数名称: int iLvGetOrient(PAY_IN_AREA *wp_payin)                       */
/* 功能描述: 判断一笔交易是发送交易还是接收交易                            */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvGetOrient(PAY_IN_AREA *wp_payin)
{
	if (memcmp(wp_payin->F_opcd,OPCD_LR_QUERY,sizeof(wp_payin->F_opcd))==0||
		memcmp(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd))==0||
		memcmp(wp_payin->F_opcd,OPCD_CHECK,sizeof(wp_payin->F_opcd))==0||
		memcmp(wp_payin->F_opcd,OPCD_SEND_QUERY,sizeof(wp_payin->F_opcd))==0||
		memcmp(wp_payin->F_opcd,OPCD_SEND,sizeof(wp_payin->F_opcd))==0||
		memcmp(wp_payin->F_opcd,OPCD_RESP,sizeof(wp_payin->F_opcd))==0)
	{
		return(TX_SEND);
	}else
	{
		return(TX_RECEIVE);
	}
}
/***************************************************************************/
/* 函数名称: int iLvFetchRecToPayin(PAY_IN_AREA *wp_payin)                 */
/* 功能描述: 根据payin输入内容提取一条支付记录                             */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int iLvFetchRecToPayin(PAY_IN_AREA *wp_payin)
{
	/**认为调用此函数时，输入部分已经有值，但表记录区还没有内容**/
	/**所以利用输入区的关键字锁定表，得到表数据区**/
	/**此函数使用FIND,虽然在更新时可能多访问了一次表，但减少了锁表的可能**/
	/**缺点是多个服务同时试图修改同一条记录时会发生不一致的问题，但可以避免**/
	/**如果出现了错误，设置错误的描述，但不调用gs99或gs100**/
	int iRc =0;
	char cTlrno[5];
	struct lv_pkgreg_c sLv_packreg;
	memset(cTlrno,0,sizeof(cTlrno));
	memset(&sLv_packreg,'\0',sizeof(sLv_packreg)-1);
	get_zd_data("0070",cTlrno);
	vtcp_log("[%s][%d]cTlrno=[%s]",__FILE__,__LINE__,cTlrno);
	vtcp_log("[%s][%d]pkgno=[%.3s],orient=[%d]\n",__FILE__,__LINE__,	wp_payin->F_pkgno,iLvGetOrient(wp_payin));
	/* 发起行号 */
	memcpy(sLv_packreg.or_br_no,wp_payin->F_orbrno,	sizeof(sLv_packreg.or_br_no)-1);
	/* 委托日期 */
	sLv_packreg.wt_date = apatoi(wp_payin->F_wtday, 8);
	/* 支付交易序号 */
	memcpy(sLv_packreg.o_orderno,wp_payin->F_orderno,sizeof(sLv_packreg.o_orderno)-1);	
	/* 来往标识NEWYX2: */
	if(iLvGetOrient(wp_payin)==TX_SEND){
		sLv_packreg.lw_ind[0]='1';
	}else{
		sLv_packreg.lw_ind[0]='2';
	}
	/* 原支付交易序号 */
	memcpy(sLv_packreg.o_orderno,wp_payin->F_orderno,	sizeof(sLv_packreg.o_orderno)-1);	
  /**add by wangyongwei**20070503***/
  /**包编号***/
  memcpy(sLv_packreg.pkgno,wp_payin->F_pkgno,sizeof(wp_payin->F_pkgno));
  /**业务类型**/
  memcpy(sLv_packreg.txnum,wp_payin->F_txnum,sizeof(wp_payin->F_txnum));
	vtcp_log("[%s,%d],  sLv_packreg.txnum===[%s] \n",__FILE__,__LINE__,sLv_packreg.txnum);
	vtcp_log("[%s,%d],  sLv_packreg.pkgno===[%s] \n",__FILE__,__LINE__,sLv_packreg.pkgno);
  /**add by wangyongwei**20070503***/

	vtcp_log("[%s,%d],  sLv_packreg.lw_ind===[%s] \n",__FILE__,__LINE__,sLv_packreg.lw_ind);
	vtcp_log("[%s,%d],  sLv_packreg.or_br_no===[%s] \n",__FILE__,__LINE__,sLv_packreg.or_br_no);
	vtcp_log("[%s,%d],  sLv_packreg.wt_date===[%d] \n",__FILE__,__LINE__,sLv_packreg.wt_date);	
	vtcp_log("[%s,%d],  sLv_packreg.o_wt_date===[%d] \n",__FILE__,__LINE__,sLv_packreg.o_wt_date);		
	vtcp_log("[%s,%d],  sLv_packreg.o_orderno===[%s] \n",__FILE__,__LINE__,sLv_packreg.o_orderno);
	/**begin: add by wangyongwei 20070306  NEWYX 20070401**/
	/****NEWYX2:增加pkgno条件****/
	if(memcmp(sLv_packreg.pkgno,PKGNO_PTJJHZ,3)==0 && (memcmp(sLv_packreg.txnum,TXNUM_ZPJL,5)==0 || memcmp(sLv_packreg.txnum,TXNUM_TYJL,5)==0 || memcmp(sLv_packreg.txnum,"30104",5)==0 || memcmp(sLv_packreg.txnum,"30105",5)==0)&& sLv_packreg.lw_ind[0]=='1'){
		vtcp_log("[%s][%d]============是支票影像回执录入\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &sLv_packreg, "ac_br_no = '%s' and o_wt_date = %ld and o_orderno = '%s' and lw_ind='1'", 
			sLv_packreg.or_br_no, sLv_packreg.wt_date, sLv_packreg.o_orderno); 
	}
	/**	end: add by wangyongwei 20070306***/
	/*add by chenggx 2011-6-11 beg*/
	else if(memcmp(sLv_packreg.pkgno,PKGNO_DQJJHZ,3)==0){
		vtcp_log("[%s][%d]============是定期借记回执录入\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &sLv_packreg, " or_br_no = '%s' and wt_date = %ld and orderno = '%s' and lw_ind='%c' and pkgno='%.3s' ", 
			sLv_packreg.or_br_no, sLv_packreg.wt_date, sLv_packreg.o_orderno,sLv_packreg.lw_ind[0],wp_payin->F_pkgno);
	}
	/*add by chenggx 2011-6-11 end*/
	else if(wp_payin->F_pkgno[0]=='\0'){
		/****NEWYX2:没有传来包类型,直接找****/
		vtcp_log("[%s][%d]============是NEWYX2\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &sLv_packreg, " or_br_no = '%s' and wt_date = %ld and orderno = '%s' and lw_ind='%c' ", 
			sLv_packreg.or_br_no, sLv_packreg.wt_date, sLv_packreg.o_orderno,sLv_packreg.lw_ind[0]);
	}else
	{
		vtcp_log("[%s][%d]============是什么\n",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Sel(g_pub_tx.reply, &sLv_packreg, " or_br_no = '%s' and wt_date = %ld and orderno = '%s' and lw_ind='%c' and pkgno='%.3s' ", 
			sLv_packreg.or_br_no, sLv_packreg.wt_date, sLv_packreg.o_orderno,sLv_packreg.lw_ind[0],wp_payin->F_pkgno);
	}
	if (iRc!=DB_OK)
	{
		vtcp_log("[%s][%d] OR_BR_NO==[%s]  WT_DATE==[%ld] O_ORDERNO==[%s] LW_IND==[%c] PKGNO==[%s]",__FILE__,__LINE__, \
			sLv_packreg.or_br_no, sLv_packreg.wt_date, sLv_packreg.o_orderno,sLv_packreg.lw_ind[0],wp_payin->F_pkgno);
		vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"未找到原业务记录");
		strncpy( g_pub_tx.reply,"H031",4);
/** dongxy 20070313
		return(-1);
**/
		return(iRc);
	}
	vtcp_log("[%s][%d]F_opcd=[%s] ",__FILE__,__LINE__,wp_payin->F_opcd);
	vtcp_log("[%s][%d]lv_sts=[%s] beg_sts=[%s]",__FILE__,__LINE__,sLv_packreg.lv_sts,sLv_packreg.beg_sts);
	vtcp_log("[%s][%d]input_tel=[%s]",__FILE__,__LINE__,sLv_packreg.input_tel);
	vtcp_log("[%s][%d]cTlrno=[%s]",__FILE__,__LINE__,cTlrno);
	if (!memcmp(wp_payin->F_opcd,"30",sizeof(wp_payin->F_opcd)) && !memcmp(cTlrno,sLv_packreg.input_tel,sizeof(cTlrno)-1) && strcmp(getenv("FHTEST"),"LVTEST")!=0)
	{
		vtcp_log("%s,%d,录入和复核不能同一个操作员",__FILE__,__LINE__);
		sprintf(acErrMsg,"录入和复核不能同一个操作员!");
		strncpy( g_pub_tx.reply,"P080",4);
		return(-1);
	}
	LV_CP_PAY_IN_FIELD(wp_payin,(&sLv_packreg));

	iLvZipAll(wp_payin);

	vtcp_log("[%s][%d]******************lv_sts[%.1s]\n",__FILE__,__LINE__,wp_payin->F_lvsts);
	vtcp_log("[%s][%d]pkgno=[%.3s],orient=[%d]\n",__FILE__,__LINE__, wp_payin->F_pkgno,iLvGetOrient(wp_payin));
	return(0);	
}

/***************************************************************************/
/* 函数名称: int iLvUpdRecFromPayin(PAY_IN_AREA *wp_payin,int insflg)      */
/* 功能描述: 根据payin中的内容更新数据库表,并插入明细                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/*       insflg是更新标志，1－插入，0－更新                                */
/***************************************************************************/
int iLvUpdRecFromPayin(PAY_IN_AREA *wp_payin,int insflg)
{
	/**首先根据交换区表数据区中的主键内容锁定表**/
	/**然后把表数据区中的字段值依次赋到表结构体中执行更新**/
	/**最后插入批量交易历史明细**/
	/**insflg是更新标志，1－插入，0－更新**/
	int iRc = 0;
	struct lv_pkghtr_c wd_lv_pkghtr;
	int sql_maxid = 0;
	char sql_tbsdy[9];
	struct lv_pkgreg_c sLv_pkgreg;	
	char  cTempstr[1024];

	memset(&wd_lv_pkghtr,'\0',sizeof(wd_lv_pkghtr));
	memset(sql_tbsdy,'\0',sizeof(sql_tbsdy));
	memset(&sLv_pkgreg,'\0',sizeof(sLv_pkgreg));

	memcpy(sLv_pkgreg.or_br_no,wp_payin->F_orbrno,sizeof(sLv_pkgreg.or_br_no)-1);	
	sLv_pkgreg.wt_date = apatoi(wp_payin->F_wtday, 8);
	memcpy(sLv_pkgreg.orderno,wp_payin->F_orderno,sizeof(sLv_pkgreg.orderno)-1);	
	sLv_pkgreg.lw_ind[0]=wp_payin->F_lw_ind[0];
	vtcp_log("[%s][%d]来到了这里111111111",__FILE__,__LINE__);
	vtcp_log("[%s][%d]insflg===[%d]",__FILE__,__LINE__,insflg);
	vtcp_log("[%s][%d]opcd===[%.2s]",__FILE__,__LINE__,wp_payin->F_opcd);

	/***如果要更新记录,对字符进行检测***/
	vtcp_log("[%s,%d] iRc====*&*&*&*&*==[%d] OPCD==%s",__FILE__,__LINE__,iRc,OPCD_RECEIVE);
	if (memcmp(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd)))
	{
		/****add by liuyue  大小额的错帐处理时候不需要进行字符的检测liuyue 20061218***/
		vtcp_log("[%s][%d]长治tx_code==[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_code);
		if(memcmp(g_pub_tx.tx_code,"5880",4) && memcmp(g_pub_tx.tx_code,"5847",4) && memcmp(g_pub_tx.tx_code,"5848",4) )
				iRc = iLvPayin_characterCheck(wp_payin,g_pub_tx.reply);

		vtcp_log("[%s,%d] iRc====*&*&*&*&*==[%d]",__FILE__,__LINE__,iRc);
		if (iRc)
		{
			vtcp_log("[%s][%d]存在非法字符!\n",__FILE__,__LINE__);
			return (-1);
		}			 
	}
	if (!insflg) /*  更新  */
	{
		vtcp_log("[%s][%d]insflg===[%d]",__FILE__,__LINE__,insflg);
		g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "or_br_no = '%s' and wt_date = %ld and orderno = '%s' and lw_ind='%c' ",
			sLv_pkgreg.or_br_no, sLv_pkgreg.wt_date, sLv_pkgreg.orderno,sLv_pkgreg.lw_ind[0]);
		vtcp_log("[%s][%d]--------------",__FILE__,__LINE__);	
		if (g_reply_int)
		{
			sprintf(acErrMsg,"定义游标出错 - %d", g_reply_int);
			return(-1);
		}
		vtcp_log("[%s][%d]--------------",__FILE__,__LINE__);
		if ((g_reply_int = Lv_pkgreg_Fet_Upd(&sLv_pkgreg, g_pub_tx.reply)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,g_reply_int);
			Lv_pkgreg_Debug(&sLv_pkgreg);
			sprintf(acErrMsg,"未找到普通贷记业务记录");
			return(-1);
		}
		vtcp_log("[%s][%d]--------------",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,wp_payin->T_packid,wp_payin->F_packid);

	LV_CP_PAY_IN_TABLE(wp_payin,(&sLv_pkgreg));
	/* begin add by LiuHuafeng 2009-5-14 21:59:52 for 村镇银行 */
	/****增加判断为来帐时，才走此分支处理，往帐无需进入  add at 20130409****/
	if (memcmp(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd))==0)
	{
		struct hv_orno_brno_c sOrno_brno;
		char cReply[5];
		int iRet=0;
		memset(cReply, 0 , sizeof(cReply));
		memset(&sOrno_brno, 0 , sizeof(sOrno_brno));
		strncpy(sOrno_brno.or_br_no,sLv_pkgreg.ac_br_no,sizeof(sOrno_brno.or_br_no)-1);
		iRet=Hv_orno_brno_Sel(cReply,&sOrno_brno,"or_br_no='%s'",sOrno_brno.or_br_no);
		if(iRet ==0)
		{
			vtcp_log("[%s][%d] br_no=[%s] [%s]",__FILE__,__LINE__,sLv_pkgreg.br_no,sOrno_brno.br_no);
			if(memcmp(sOrno_brno.br_no,"9999" ,4)==0)
			{
				strncpy(sLv_pkgreg.br_no,sOrno_brno.br_no,BRNO_LEN);
			}
		}
	}
	vtcp_log("[%s][%d]packid==[%s][%s]",__FILE__,__LINE__,wp_payin->T_packid,wp_payin->F_packid);
	vtcp_log("[%s][%d] sLv_pkgreg.o_orderno       =[%s]",__FL__,sLv_pkgreg.o_orderno);

	vtcp_log("[%s][%d] sLv_pkgreg.pkgno           =[%s]",__FL__,sLv_pkgreg.pkgno         );    
	vtcp_log("[%s][%d] sLv_pkgreg.orderno         =[%s]",__FL__,sLv_pkgreg.orderno       );
	vtcp_log("[%s][%d] sLv_pkgreg.txnum           =[%s]",__FL__,sLv_pkgreg.txnum         );
	vtcp_log("[%s][%d] sLv_pkgreg.or_br_no        =[%s]",__FL__,sLv_pkgreg.or_br_no      );
	vtcp_log("[%s][%d] sLv_pkgreg.pay_qs_no       =[%s]",__FL__,sLv_pkgreg.pay_qs_no     );
	vtcp_log("[%s][%d] sLv_pkgreg.sendco          =[%s]",__FL__,sLv_pkgreg.sendco        );
	vtcp_log("[%s][%d] sLv_pkgreg.ac_br_no        =[%s]",__FL__,sLv_pkgreg.ac_br_no      );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_qs_no      =[%s]",__FL__,sLv_pkgreg.cash_qs_no    );
	vtcp_log("[%s][%d] sLv_pkgreg.receco          =[%s]",__FL__,sLv_pkgreg.receco        );
	vtcp_log("[%s][%d] sLv_pkgreg.pay_opn_br_no   =[%s]",__FL__,sLv_pkgreg.pay_opn_br_no );
	vtcp_log("[%s][%d] sLv_pkgreg.pay_ac_no       =[%s]",__FL__,sLv_pkgreg.pay_ac_no     );
	vtcp_log("[%s][%d] sLv_pkgreg.pay_name        =[%s]",__FL__,sLv_pkgreg.pay_name      );
	vtcp_log("[%s][%d] sLv_pkgreg.pay_addr        =[%s]",__FL__,sLv_pkgreg.pay_addr      );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_opn_br_no  =[%s]",__FL__,sLv_pkgreg.cash_opn_br_no);
	vtcp_log("[%s][%d] sLv_pkgreg.cash_ac_no      =[%s]",__FL__,sLv_pkgreg.cash_ac_no    );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_name       =[%s]",__FL__,sLv_pkgreg.cash_name     );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_addr       =[%s]",__FL__,sLv_pkgreg.cash_addr     );
	vtcp_log("[%s][%d] sLv_pkgreg.lv_yw_ind       =[%s]",__FL__,sLv_pkgreg.lv_yw_ind     );
	vtcp_log("[%s][%d] sLv_pkgreg.lv_brf          =[%s]",__FL__,sLv_pkgreg.lv_brf        );
	vtcp_log("[%s][%d] sLv_pkgreg.rcpstat         =[%s]",__FL__,sLv_pkgreg.rcpstat       );
	vtcp_log("[%s][%d] sLv_pkgreg.resp1           =[%s]",__FL__,sLv_pkgreg.resp1         );
	vtcp_log("[%s][%d] sLv_pkgreg.resp2           =[%s]",__FL__,sLv_pkgreg.resp2         );
	vtcp_log("[%s][%d] sLv_pkgreg.note_type       =[%s]",__FL__,sLv_pkgreg.note_type     );
	vtcp_log("[%s][%d] sLv_pkgreg.note_no         =[%s]",__FL__,sLv_pkgreg.note_no       );
	vtcp_log("[%s][%d] sLv_pkgreg.addid           =[%s]",__FL__,sLv_pkgreg.addid         );
	vtcp_log("[%s][%d] sLv_pkgreg.br_no           =[%s]",__FL__,sLv_pkgreg.br_no         );
	vtcp_log("[%s][%d] sLv_pkgreg.input_tel       =[%s]",__FL__,sLv_pkgreg.input_tel     );
	vtcp_log("[%s][%d] sLv_pkgreg.check_tel       =[%s]",__FL__,sLv_pkgreg.check_tel     );
	vtcp_log("[%s][%d] sLv_pkgreg.auth_tel        =[%s]",__FL__,sLv_pkgreg.auth_tel      );
	vtcp_log("[%s][%d] sLv_pkgreg.send_tel        =[%s]",__FL__,sLv_pkgreg.send_tel      );
	vtcp_log("[%s][%d] sLv_pkgreg.resp_tel        =[%s]",__FL__,sLv_pkgreg.resp_tel      );
	vtcp_log("[%s][%d] sLv_pkgreg.deal_tel        =[%s]",__FL__,sLv_pkgreg.deal_tel      );
	vtcp_log("[%s][%d] sLv_pkgreg.input_trace_no  =[%s]",__FL__,sLv_pkgreg.input_trace_no);
	vtcp_log("[%s][%d] sLv_pkgreg.check_trace_no  =[%s]",__FL__,sLv_pkgreg.check_trace_no);
	vtcp_log("[%s][%d] sLv_pkgreg.send_trace_no   =[%s]",__FL__,sLv_pkgreg.send_trace_no );
	vtcp_log("[%s][%d] sLv_pkgreg.resp_trace_no   =[%s]",__FL__,sLv_pkgreg.resp_trace_no );
	vtcp_log("[%s][%d] sLv_pkgreg.deal_trace_no   =[%s]",__FL__,sLv_pkgreg.deal_trace_no );
	vtcp_log("[%s][%d] sLv_pkgreg.lv_sts          =[%s]",__FL__,sLv_pkgreg.lv_sts    );
	vtcp_log("[%s][%d] sLv_pkgreg.beg_sts         =[%s]",__FL__,sLv_pkgreg.beg_sts    );
	vtcp_log("[%s][%d] sLv_pkgreg.operlevel       =[%s]",__FL__,sLv_pkgreg.operlevel  );
	vtcp_log("[%s][%d] sLv_pkgreg.tx_chrg_ind     =[%s]",__FL__,sLv_pkgreg.tx_chrg_ind);
	vtcp_log("[%s][%d] sLv_pkgreg.checkflag       =[%s]",__FL__,sLv_pkgreg.checkflag  );
	vtcp_log("[%s][%d] sLv_pkgreg.brprtflag       =[%s]",__FL__,sLv_pkgreg.brprtflag     );
	vtcp_log("[%s][%d] sLv_pkgreg.qsprtflag       =[%s]",__FL__,sLv_pkgreg.qsprtflag     );
	vtcp_log("[%s][%d] sLv_pkgreg.packid          =[%s]",__FL__,sLv_pkgreg.packid        );
	vtcp_log("[%s][%d] sLv_pkgreg.retcode         =[%s]",__FL__,sLv_pkgreg.retcode       );
	vtcp_log("[%s][%d] sLv_pkgreg.o_txnum         =[%s]",__FL__,sLv_pkgreg.o_txnum       );
	vtcp_log("[%s][%d] sLv_pkgreg.ywdtlid         =[%s]",__FL__,sLv_pkgreg.ywdtlid       );
	vtcp_log("[%s][%d] sLv_pkgreg.protno          =[%s]",__FL__,sLv_pkgreg.protno        );
	vtcp_log("[%s][%d] sLv_pkgreg.ac_type         =[%s]",__FL__,sLv_pkgreg.ac_type    );
	vtcp_log("[%s][%d] sLv_pkgreg.refno           =[%s]",__FL__,sLv_pkgreg.refno         );
	vtcp_log("[%s][%d] sLv_pkgreg.o_or_br_no      =[%s]",__FL__,sLv_pkgreg.o_or_br_no    );
	vtcp_log("[%s][%d] sLv_pkgreg.o_orderno       =[%s]",__FL__,sLv_pkgreg.o_orderno     );
	vtcp_log("[%s][%d] sLv_pkgreg.o_packid        =[%s]",__FL__,sLv_pkgreg.o_packid      );
	vtcp_log("[%s][%d] sLv_pkgreg.o_ac_br_no      =[%s]",__FL__,sLv_pkgreg.o_ac_br_no    );
	vtcp_log("[%s][%d] sLv_pkgreg.lv_o_sts        =[%s]",__FL__,sLv_pkgreg.lv_o_sts   );
	vtcp_log("[%s][%d] sLv_pkgreg.lw_ind          =[%s]",__FL__,sLv_pkgreg.lw_ind     );
	vtcp_log("[%s][%d] sLv_pkgreg.cur_no          =[%s]",__FL__,sLv_pkgreg.cur_no        );
	vtcp_log("[%s][%d] sLv_pkgreg.filler          =[%s]",__FL__,sLv_pkgreg.filler        );                                                                                           
	vtcp_log("[%s][%d] sLv_pkgreg.tx_time         =[%ld]",__FL__,sLv_pkgreg.tx_time       );
	vtcp_log("[%s][%d] sLv_pkgreg.jz_date         =[%ld]",__FL__,sLv_pkgreg.jz_date       );
	vtcp_log("[%s][%d] sLv_pkgreg.in_date         =[%ld]",__FL__,sLv_pkgreg.in_date       );
	vtcp_log("[%s][%d] sLv_pkgreg.wt_date         =[%ld]",__FL__,sLv_pkgreg.wt_date       );
	vtcp_log("[%s][%d] sLv_pkgreg.pack_date       =[%ld]",__FL__,sLv_pkgreg.pack_date     );
	vtcp_log("[%s][%d] sLv_pkgreg.resp_date       =[%ld]",__FL__,sLv_pkgreg.resp_date     );
	vtcp_log("[%s][%d] sLv_pkgreg.o_wt_date       =[%ld]",__FL__,sLv_pkgreg.o_wt_date     );
	vtcp_log("[%s][%d] sLv_pkgreg.o_pack_date     =[%ld]",__FL__,sLv_pkgreg.o_pack_date   );
	vtcp_log("[%s][%d] sLv_pkgreg.df_chrg_amt     =[%f]",__FL__,sLv_pkgreg.df_chrg_amt   );
	vtcp_log("[%s][%d] sLv_pkgreg.chrg_amt        =[%f]",__FL__,sLv_pkgreg.chrg_amt      );
	vtcp_log("[%s][%d] sLv_pkgreg.tx_amt          =[%f]",__FL__,sLv_pkgreg.tx_amt        );

	/*  葛总说的不对， 插入的帐号不用转换，还是用报文里的帐号  2006-10-21 14:41
	vtcp_log("[%s][%d] sLv_pkgreg.pay_ac_no       =[%s]",__FL__,sLv_pkgreg.pay_ac_no     );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_ac_no      =[%s]",__FL__,sLv_pkgreg.cash_ac_no    );
	新旧帐号转换  
	pub_base_old_acno(sLv_pkgreg.cash_ac_no);
	pub_base_old_acno(sLv_pkgreg.pay_ac_no); 
	vtcp_log("[%s][%d] sLv_pkgreg.pay_ac_no       =[%s]",__FL__,sLv_pkgreg.pay_ac_no     );
	vtcp_log("[%s][%d] sLv_pkgreg.cash_ac_no      =[%s]",__FL__,sLv_pkgreg.cash_ac_no    );
	*/    

	vtcp_log("[%s][%d] sLv_pkgreg.pay_name==[%.60s] ",__FILE__,__LINE__,sLv_pkgreg.pay_name);
	memset(cTempstr,0,sizeof(cTempstr));
	memcpy(cTempstr, sLv_pkgreg.pay_name, sizeof(sLv_pkgreg.pay_name)-1);
	if (wp_payin->F_lw_ind[0]=='2')
	{
		dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
		memcpy(sLv_pkgreg.pay_name, cTempstr, sizeof(sLv_pkgreg.pay_name)-1);
	}
	vtcp_log("[%s][%d] sLv_pkgreg.pay_name==[%.60s] ",__FILE__,__LINE__,sLv_pkgreg.pay_name);  

	vtcp_log("[%s][%d] sLv_pkgreg.cash_name==[%.60s] ",__FILE__,__LINE__,sLv_pkgreg.cash_name);
	memset(cTempstr,0,sizeof(cTempstr));
	memcpy(cTempstr, sLv_pkgreg.cash_name, sizeof(sLv_pkgreg.cash_name)-1);
	if (wp_payin->F_lw_ind[0]=='2')
	{
		dbc_zipspace(cTempstr);  /*压缩全角空格 2006-11-7 23:45  */
		memcpy(sLv_pkgreg.cash_name, cTempstr, sizeof(sLv_pkgreg.cash_name)-1);
	}
	vtcp_log("[%s][%d] sLv_pkgreg.cash_name==[%.60s] ",__FILE__,__LINE__,sLv_pkgreg.cash_name);  

	if (insflg)/**插入记录**/
	{	
	  /* Begin Added for process Sendco Receco  2006-11-13 15:57 **************/
	  if(strlen(sLv_pkgreg.sendco) < 4)
	  {
    		iGetSendco(sLv_pkgreg.sendco, sLv_pkgreg.pay_qs_no);
	  }
	  if(strlen(sLv_pkgreg.receco) < 4)
	  {
    		iGetSendco(sLv_pkgreg.receco, sLv_pkgreg.cash_qs_no);
	  }
	  /* End Added for process Sendco Receco  2006-11-13 15:57 ****************/
		
		/* Lv_xyy_Debug(&sLv_pkgreg);*/
		iRc=Lv_pkgreg_Ins(sLv_pkgreg, g_pub_tx.reply);
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Update error,iRc=%d\n",__FILE__,__LINE__,iRc);
			Lv_xyy_Debug(&sLv_pkgreg);
			sprintf(acErrMsg,"更新普通贷记发送表失败");
			return(-1);
		}
	}
	else /**更新记录**/
	{
		iRc=Lv_pkgreg_Upd_Upd(sLv_pkgreg, g_pub_tx.reply);
		if (iRc == DB_OK)
		{
			Lv_pkgreg_Clo_Upd();
		}
	}
	if (iRc!=DB_OK)
	{
		vtcp_log("%s,%d,Update error,iRc=%d\n",__FILE__,__LINE__,iRc);
		Lv_pkgreg_Debug(&sLv_pkgreg);
		sprintf(acErrMsg,"更新普通贷记发送表失败");
		return(-1);
	}
	vtcp_log("[%s][%d]来到了这里222222222",__FILE__,__LINE__);
	vtcp_log("[%s][%d]pkgno=[%.3s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
	vtcp_log("[%s][%d]F_acbrno=[%.12s]\n",__FILE__,__LINE__,wp_payin->F_acbrno);
	vtcp_log("[%s][%d]txnum=[%.5s]\n",__FILE__,__LINE__,wp_payin->F_txnum);

	/************************************************************/
	/************************************************************/
	/************************************************************/
	/****插入批量交易历史明细****/
	
	memset(sql_tbsdy,'\0',sizeof(sql_tbsdy));
	memcpy(sql_tbsdy,pcLvGetDay(),sizeof(sql_tbsdy)-1);
	vtcp_log("[%s][%d]--------sql_tbsdy=[%s]",__FILE__,__LINE__,sql_tbsdy);

/*	sql_maxid=0;

	sql_maxid = sql_max_int("lv_pkghtr", "ywdtlid", "in_date = %ld ",  apatoi(sql_tbsdy,8));
	if (memcmp(g_pub_tx.reply, "D101", 4)==0) 
		g_reply_int = sql_maxid; 
	else
		g_reply_int = DB_OK;
	if (g_reply_int!=DB_OK)
	{
		vtcp_log("%s,%d,查询批量历史明细,sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sql_maxid=1;
	}
	else
	{
		sql_maxid++;
	}
	vtcp_log("%s,%d,明细序号为%d\n",__FILE__,__LINE__,sql_maxid);
	wd_lv_pkghtr.in_date = apatoi(pcLvGetDay(), 8);
	vtcp_log("[%s][%d] wd_lv_pkghrt.in_date=[%ld]\n",__FILE__,__LINE__,wd_lv_pkghtr.in_date);
	wd_lv_pkghtr.dtlid=sql_maxid;*/
	
	
	/****填写历史明细的值****/
	get_zd_long(DC_TIME,&wd_lv_pkghtr.tx_time);	
	vtcp_log("[%s][%d]wd_lv_pkghtr.tx_time[%ld]",__FILE__,__LINE__,wd_lv_pkghtr.tx_time);
	/*****
	get_fd_data("0060", wd_lv_pkghtr.tx_time+8);
	******/
	vtcp_log("[%s][%d]   insflg=====[%d]",__FILE__,__LINE__,insflg);

	g_reply_int = iLvUpdZfRecFromPayin(wp_payin,insflg);
	if (g_reply_int)
	{
		vtcp_log("%s,%d，调用iLvUpdZfRecFromPayin错误,ERR=[%d]\n",__FILE__,__LINE__,g_reply_int);
		sprintf( acErrMsg,"iLvUpdZfRecFromPayin错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
	}
	vtcp_log("[%s][%d]here!!",__FILE__,__LINE__);
	return(0);
}

/*****************************************************/
/**	函数名称:lv_Recv_Ins_Tab(*lv,*wp_in,int flag)	**/
/** 参数:来帐解析的报文结构，输入包结构 			**/
/** 含义:对所有来帐记入lv_packlisthtr表中 			**/
/**	flag 1往帐,2来帐								**/
/** 编写:dongxy										**/
/*****************************************************/
int lv_Recv_Ins_Tab(TLVPACKAGE *lv, NOTPAY_IN_AREA *wp_notpayin,int flag)
{
	int		ret=0;
	static	struct	lv_recvlist_c 	wd_lv_recvlist;
	static	struct	lv_sendlist_c 	wd_lv_sendlist;
	/*static	struct	lv_packlisthtr_c 	wd_lv_packlisthtr;*/

	memset(&wd_lv_recvlist, 	'\0', sizeof(wd_lv_recvlist));
	memset(&wd_lv_sendlist, 	'\0', sizeof(wd_lv_sendlist));
	/*memset(&wd_lv_packlisthtr, 	'\0', sizeof(wd_lv_packlisthtr));*/

	vtcp_log("[%s][%d]-----是[%d]1往帐,2来帐\n",__FILE__,__LINE__,flag);

	if (flag == 1)	/** 往帐,写lv_sendlist表 **/
	{
		vtcp_log("[%s][%d]==进入往帐\n",__FILE__,__LINE__);

		memcpy(wd_lv_sendlist.br_no,  lvca.brno, sizeof(wd_lv_sendlist.br_no)-1);
		memcpy(wd_lv_sendlist.tel, lvca.tlrno, sizeof(wd_lv_sendlist.tel)-1);
		wd_lv_sendlist.trace_no = apatoi( lvca.wssrno,	8);
		memcpy(wd_lv_sendlist.refno,  	lv->reNo, 	sizeof(wd_lv_sendlist.refno)-1);
		memcpy(wd_lv_sendlist.packtype, lv->CMTNo, 	sizeof(wd_lv_sendlist.packtype)-1);
		memcpy(wd_lv_sendlist.packno, 	lv->CMTCode,sizeof(wd_lv_sendlist.packno)-1);
		memcpy(wd_lv_sendlist.orderno, 	 wp_notpayin->orderno,	   sizeof(wd_lv_sendlist.orderno)-1);
		memcpy(wd_lv_sendlist.or_br_no, 	 wp_notpayin->orbrno, 	   sizeof(wd_lv_sendlist.or_br_no)-1);
		memcpy(wd_lv_sendlist.pay_qs_no,wp_notpayin->payqsactno, sizeof(wd_lv_sendlist.pay_qs_no)-1);
		memcpy(wd_lv_sendlist.ac_br_no, 	 wp_notpayin->acbrno, 	   sizeof(wd_lv_sendlist.ac_br_no)-1);
		memcpy(wd_lv_sendlist.cash_qs_no,wp_notpayin->cashqsactno,sizeof(wd_lv_sendlist.cash_qs_no)-1);
		str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0, &wd_lv_sendlist.tx_amt);
		wd_lv_recvlist.tx_date = apatoi(lv->packDate,8);/***add by xyy 2007-4-15 18:38 NEWYX****/
		if (memcmp(lv->CMTCode, "911", 3) == 0)
		{
			wd_lv_sendlist.respflag[0] = '0';
		}
		else
		{
			wd_lv_sendlist.respflag[0] = '1';
		}
		/** respno **/
		/** respdesc **/
		memcpy(wd_lv_sendlist.filler, wp_notpayin->content, sizeof(wd_lv_sendlist.filler)-1);
		if (ret = Lv_sendlist_Ins(wd_lv_sendlist, g_pub_tx.reply) != DB_OK)
		{
			vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
			Lv_sendlist_Debug(&wd_lv_sendlist);
			sprintf(acErrMsg,"插入小额报文发送登记簿失败");
			return(-1);
		}
	}
	else if (flag == 2)	/** 来帐,写lv_recvlist表 **/
	{
		vtcp_log("[%s][%d]==进入来帐\n",__FILE__,__LINE__);
		memcpy(wd_lv_recvlist.br_no,  lvca.brno, sizeof(wd_lv_recvlist.br_no)-1);
		memcpy(wd_lv_recvlist.tel, lvca.tlrno, sizeof(wd_lv_recvlist.tel)-1);
		wd_lv_recvlist.trace_no = apatoi(lvca.wssrno, 8);
		memcpy(wd_lv_recvlist.refno,  	lv->reNo, 	sizeof(wd_lv_recvlist.refno)-1);
		memcpy(wd_lv_recvlist.packtype, lv->CMTNo, 	sizeof(wd_lv_recvlist.packtype)-1);
		memcpy(wd_lv_recvlist.packno, 	lv->CMTCode,sizeof(wd_lv_recvlist.packno)-1);
		memcpy(wd_lv_recvlist.orderno, 	 wp_notpayin->orderno,	   sizeof(wd_lv_recvlist.orderno)-1);
		memcpy(wd_lv_recvlist.or_br_no, 	 wp_notpayin->orbrno, 	   sizeof(wd_lv_recvlist.or_br_no)-1);
		memcpy(wd_lv_recvlist.pay_qs_no,wp_notpayin->payqsactno, sizeof(wd_lv_recvlist.pay_qs_no)-1);
		memcpy(wd_lv_recvlist.ac_br_no, 	 wp_notpayin->acbrno, 	   sizeof(wd_lv_recvlist.ac_br_no)-1);
		memcpy(wd_lv_recvlist.cash_qs_no,wp_notpayin->cashqsactno,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		wd_lv_recvlist.tx_date = apatoi(lv->packDate,8);/***add by xyy 2007-4-15 18:38 NEWYX****/
		str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0, &wd_lv_recvlist.tx_amt);
		if (memcmp(lv->CMTCode, "911", 3) == 0)
		{
			wd_lv_recvlist.respflag[0] = '0';
		}
		else
			wd_lv_recvlist.respflag[0] = '1';
		/** respno **/
		/** respdesc **/
		memcpy(wd_lv_recvlist.filler, wp_notpayin->content, sizeof(wd_lv_recvlist.filler)-1);
		if (ret = Lv_recvlist_Ins(wd_lv_recvlist, g_pub_tx.reply) != DB_OK)
		{
			vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
			Lv_recvlist_Debug(&wd_lv_recvlist);
			sprintf(acErrMsg,"插入小额报文接收登记簿失败");

			return(-1);
		}
	}
	vtcp_log("[%s][%d]==lv->packDate=[%s]\n",__FILE__,__LINE__,lv->packDate);
	/*
	memcpy(wd_lv_packlisthtr.in_date, lv->packDate, sizeof(wd_lv_packlisthtr.in_date)-1);
	apitoa(lv->pkgType, 1, wd_lv_packlisthtr.type);
	memcpy(wd_lv_packlisthtr.br_no, lvca.brno, sizeof(wd_lv_packlisthtr.br_no)-1);
	memcpy(wd_lv_packlisthtr.tlrno, lvca.tlrno, sizeof(wd_lv_packlisthtr.tlrno)-1);
	memcpy(wd_lv_packlisthtr.wssrno, 	get_format_seq( 6,dg_get_wssrno()),
	sizeof(wd_lv_packlisthtr.wssrno)-1);
	memcpy(wd_lv_packlisthtr.refno,  	lv->reNo, 	sizeof(wd_lv_packlisthtr.refno)-1);
	memcpy(wd_lv_packlisthtr.packtype,  lv->CMTNo, 	sizeof(wd_lv_packlisthtr.packtype)-1);
	memcpy(wd_lv_packlisthtr.packno, 	lv->CMTCode,sizeof(wd_lv_packlisthtr.packno)-1);
	memcpy(wd_lv_packlisthtr.orderno, 	wp_notpayin->orderno,    sizeof(wd_lv_packlisthtr.orderno)-1);
	memcpy(wd_lv_packlisthtr.or_br_no, 	wp_notpayin->orbrno, 	  sizeof(wd_lv_packlisthtr.or_br_no)-1);
	memcpy(wd_lv_packlisthtr.pay_qs_no,wp_notpayin->payqsactno, sizeof(wd_lv_packlisthtr.pay_qs_no)-1);
	memcpy(wd_lv_packlisthtr.ac_br_no, 	wp_notpayin->acbrno, 	  sizeof(wd_lv_packlisthtr.ac_br_no)-1);
	memcpy(wd_lv_packlisthtr.cash_qs_no,wp_notpayin->cashqsactno,sizeof(wd_lv_packlisthtr.cash_qs_no)-1);
	str2dbl(wp_notpayin->otxamt,sizeof(wp_notpayin->otxamt),0, &wd_lv_packlisthtr.txamt);
	if(memcmp(lv->CMTCode, "911", 3) == 0)
	{
	wd_lv_packlisthtr.respflag[0] = '0';
	}
	else
	wd_lv_packlisthtr.respflag[0] = '1';
	wd_lv_packlisthtr.resp_tel[0] = wp_notpayin->respcode;
	memcpy(wd_lv_packlisthtr.filler, wp_notpayin->content, sizeof(wd_lv_packlisthtr.filler)-1);

	if(ret = Lv_packlisthtr_Ins(wd_lv_packlisthtr, g_pub_tx.reply) != DB_OK)
	{
	vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
	Lv_packlisthtr_Debug(&wd_lv_packlisthtr);
	sprintf(acErrMsg,"插入小额报文发送接收历史失败");

	return(-1);
	}
	**/
	return(0);
}
/*****************************************************/
/**	函数名称:lv_Ins_Cmthtr(NOTPAY_IN_AREA *wp_in)	**/
/** 参数:来帐解析的报文结构，输入包结构 			**/
/** 含义:对所有来帐记入lv_packlisthtr表中 			**/
/** 编写:dongxy										**/
/*****************************************************/
#if 0
int lv_Ins_Cmthtr(NOTPAY_IN_AREA *wp_in)
{
	int	ret=0,cnt=0;
	char	Txday[9];
	/*static	struct	wd_lv_cmthtr_c	wd_lv_cmthtr;*/

	memset(Txday, '\0', sizeof(Txday));
	memset(&wd_lv_cmthtr, '\0', sizeof(wd_lv_cmthtr));
	memcpy(Txday, pcLvGetDay(), sizeof(Txday) -1);

	memcpy(wd_lv_cmthtr.in_date, pcLvGetDay(), sizeof(wd_lv_cmthtr.in_date)-1);
	get_zd_long(DC_TIME,wd_lv_cmthtr.tx_time);
	memcpy(wd_lv_cmthtr.txcd, lvca.aptype, 1);
	memcpy(wd_lv_cmthtr.txcd+1, lvca.txno, 2);
	memcpy(wd_lv_cmthtr.br_no, lvca.brno, sizeof(wd_lv_cmthtr.br_no)-1);
	memcpy(wd_lv_cmthtr.tlrno, lvca.tlrno, sizeof(wd_lv_cmthtr.tlrno)-1);

	/** 产生明细号 **/

	cnt = sql_count("lv_cmthtr", "txday = '%s' ",  Txday);
	if (0 == memcmp(g_pub_tx.reply, "D101", 4)) g_reply_int = cnt; else g_reply_int = DB_OK;
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"访问数据库错误!");

		return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"数据库错误!");

		return(-1);
	}
	vtcp_log("[%s][%d]cnt=[%d]\n",__FILE__,__LINE__,cnt);
	/*注掉gujy 060904
	if(cnt == 0)
	wd_lv_cmthtr.dtlid = 1;
	else
	wd_lv_cmthtr.dtlid = cnt+1;
	*/
	vtcp_log("[%s][%d]wd_lv_cmthtr.dtlid=[%f]\n",__FILE__,__LINE__,wd_lv_cmthtr.dtlid);

	memcpy(wd_lv_cmthtr.cmtno, wp_in->cmtno, sizeof(wd_lv_cmthtr.cmtno)-1);
	memcpy(wd_lv_cmthtr.in_date, pcLvGetDay(), sizeof(wd_lv_cmthtr.in_date)-1);
	memcpy(wd_lv_cmthtr.orderno, wp_in->orderno, sizeof(wd_lv_cmthtr.orderno)-1);
	memcpy(wd_lv_cmthtr.opkgno,  wp_in->opkgno, sizeof(wd_lv_cmthtr.opkgno)-1);
	memcpy(wd_lv_cmthtr.owtdate, wp_in->owtday, sizeof(wd_lv_cmthtr.owtdate)-1);
	memcpy(wd_lv_cmthtr.o_orderno, wp_in->oorderno, sizeof(wd_lv_cmthtr.o_orderno)-1);
	memcpy(wd_lv_cmthtr.filler, wp_in->content, sizeof(wd_lv_cmthtr.filler)-1);

	if (ret = Lv_cmthtr_Ins(wd_lv_cmthtr, g_pub_tx.reply) != DB_OK)
	{
		vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
		Lv_cmthtr_Debug(&wd_lv_cmthtr);
		sprintf(acErrMsg,"插入单包交易历史明细失败");
		return(-1);
	}
	return(0);

}
#endif

/***********************************************************/
/**	函数名称:iLv_UpdateThStat(PAY_IN_AREA *wp_payin_djth) **/
/** 参数:支付交易结构体                                   **/
/** 含义:根据退汇结构体找到原始贷记来帐业务并更新原始交易 **/
/**      交易状态为来帐退回状态                           **/
/** 返回:0成功 其他为失败                                 **/
/** 编写:	LiuHuafeng 20060309                           **/
/***********************************************************/
int iLv_UpdateThStat(PAY_IN_AREA *wp_payin_djth)
{
	PAY_IN_AREA wd_payin_or;
	int iRc=0;
	char cOr_br_no[13];
	char cOrderno[9];
	char cWt_date[9];
	
	iRc=0;
	memset(&wd_payin_or, 0 , sizeof(wd_payin_or));
	memset(cOr_br_no, 0 , sizeof(cOr_br_no));
	memset(cOrderno, 0 , sizeof(cOrderno));
	memset(cWt_date, 0 , sizeof(cWt_date));
			
	vtcp_log("[%s][%d] wp_payin_djth->T_stat ===[%.1s] ",__FILE__,__LINE__, wp_payin_djth->T_stat);
	vtcp_log("[%s][%d] wp_payin_djth->F_owtday ===[%.8s] ",__FILE__,__LINE__, wp_payin_djth->F_owtday);
	vtcp_log("[%s][%d] wp_payin_djth->F_oorderno ===[%.8s] ",__FILE__,__LINE__, wp_payin_djth->F_oorderno);
	vtcp_log("[%s][%d] wp_payin_djth->F_acbrno ===[%.12s] ",__FILE__,__LINE__, wp_payin_djth->F_acbrno);
	vtcp_log("[%s][%d] wp_payin_djth->F_otxnum ===[%.3s] ",__FILE__,__LINE__, wp_payin_djth->F_otxnum);

	if (wp_payin_djth->T_stat[0]!=STAT_WZSUCCESS)
	{
		vtcp_log("[%s][%d]非轧差状态不需要更新=%.2s\n",__FILE__,__LINE__,wp_payin_djth->F_rcpstat);
		return 0;
	}
	memcpy(wd_payin_or.F_wtday,wp_payin_djth->F_owtday,sizeof(wd_payin_or.F_owtday));
	memcpy(wd_payin_or.F_orderno,wp_payin_djth->F_oorderno,sizeof(wd_payin_or.F_orderno));
	memcpy(wd_payin_or.F_orbrno,wp_payin_djth->F_acbrno,sizeof(wd_payin_or.F_orbrno));

	if (memcmp(wp_payin_djth->F_otxnum,TXNUM_DQDJ,sizeof(wp_payin_djth->F_otxnum)))
	{
		memcpy(wd_payin_or.F_pkgno,PKGNO_PTDJ,sizeof(wd_payin_or.F_pkgno));
	}
	else
	{
		memcpy(wd_payin_or.F_pkgno,PKGNO_DQDJ,sizeof(wd_payin_or.F_pkgno));
	}

	vtcp_log("[%s][%d] wd_payin_or.F_wtday ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_wtday);
	vtcp_log("[%s][%d] wd_payin_or.F_orderno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orderno);
	vtcp_log("[%s][%d] wd_payin_or.F_orbrno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orbrno);
	vtcp_log("[%s][%d] wd_payin_or.F_otxnum ===[%.3s] ",__FILE__,__LINE__, wd_payin_or.F_otxnum);

	iRc=iLvFetchRecToPayin(&wd_payin_or);
	if (iRc)
	{
		vtcp_log("[%s][%d]查找原来的退回来帐表错误sqlcode=%d\n",__FILE__,__LINE__,iRc);
		return iRc;
	}	
	vtcp_log("[%s][%d] === After iLvFetchRecToPayin() !! === ",__FILE__,__LINE__);

	iLvResetPayin(&wd_payin_or);

	vtcp_log("[%s][%d] === After iLvResetPayin() !! === ",__FILE__,__LINE__);

	vtcp_log("[%s][%d] wd_payin_or.T_stat ===[%.1s] ",__FILE__,__LINE__, wd_payin_or.T_stat);

	if (wd_payin_or.T_stat[0] == STAT_LZBACK_WAIT)
	{
		wd_payin_or.T_stat[0] = STAT_LZBACK;

		/*	调用iLvUpdRecFromPayin() 会引起游标嵌套，所以改成 sql_execute
		iRc=iLvUpdRecFromPayin(&wd_payin_or,0);
		if (iRc)
		{
		vtcp_log("[%s][%d]更新原来的贷记来帐表错误sqlcode=%d",__FILE__,__LINE__,iRc);
		return iRc;
		}
		*/
		memcpy(cOr_br_no, wd_payin_or.T_orbrno, sizeof(cOr_br_no)-1);
		memcpy(cOrderno, wd_payin_or.T_orderno, sizeof(cOrderno)-1);
		memcpy(cWt_date, wd_payin_or.T_wtday, sizeof(cWt_date)-1);

		/*  更新原来的贷记来帐记录状态  */
		g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%c' where lw_ind = '2' and wt_date=%d and or_br_no='%s' and orderno='%s'", STAT_LZBACK, apatoi(cWt_date,8), cOr_br_no, cOrderno);
		if ( g_reply_int != DB_OK )
		{
			sprintf( acErrMsg,"更新数据库失败 ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			return(-1);
		}
		vtcp_log("[%s][%d] 更新原贷记来帐记录状态成功 !! \n",__FILE__,__LINE__);
		return 0;
	}
	else
	{
		vtcp_log("[%s][%d] 原贷记来帐状态错误! wd_payin_or.T_stat ===[%.1s] ",__FILE__,__LINE__, wd_payin_or.T_stat);
		return -1;
	}
	return 0;
}
int Lv_xyy_Debug(struct lv_pkgreg_c *ps)
{
	vtcp_log("lv_pkgreg_c.rowid=[%s]\n",ps->rowid);
	vtcp_log("lv_pkgreg_c.pkgno=[%s]\n",ps->pkgno);
	vtcp_log("lv_pkgreg_c.jz_date=[%ld]\n",ps->jz_date);
	vtcp_log("lv_pkgreg_c.in_date=[%ld]\n",ps->in_date);
	vtcp_log("lv_pkgreg_c.wt_date=[%ld]\n",ps->wt_date);
	vtcp_log("lv_pkgreg_c.pack_date=[%ld]\n",ps->pack_date);
	vtcp_log("lv_pkgreg_c.orderno=[%s]\n",ps->orderno);
	vtcp_log("lv_pkgreg_c.tx_time=[%ld]\n",ps->tx_time);
	vtcp_log("lv_pkgreg_c.txnum=[%s]\n",ps->txnum);
	vtcp_log("lv_pkgreg_c.or_br_no=[%s]\n",ps->or_br_no);
	vtcp_log("lv_pkgreg_c.pay_qs_no=[%s]\n",ps->pay_qs_no);
	vtcp_log("lv_pkgreg_c.sendco=[%s]\n",ps->sendco);
	vtcp_log("lv_pkgreg_c.ac_br_no=[%s]\n",ps->ac_br_no);
	vtcp_log("lv_pkgreg_c.cash_qs_no=[%s]\n",ps->cash_qs_no);
	vtcp_log("lv_pkgreg_c.receco=[%s]\n",ps->receco);
	vtcp_log("lv_pkgreg_c.tx_amt=[%f]\n",ps->tx_amt);
	vtcp_log("lv_pkgreg_c.pay_opn_br_no=[%s]\n",ps->pay_opn_br_no);
	vtcp_log("lv_pkgreg_c.pay_ac_no=[%s]\n",ps->pay_ac_no);
	vtcp_log("lv_pkgreg_c.pay_name=[%s]\n",ps->pay_name);
	vtcp_log("lv_pkgreg_c.pay_addr=[%s]\n",ps->pay_addr);
	vtcp_log("lv_pkgreg_c.cash_opn_br_no=[%s]\n",ps->cash_opn_br_no);
	vtcp_log("lv_pkgreg_c.cash_ac_no=[%s]\n",ps->cash_ac_no);
	vtcp_log("lv_pkgreg_c.cash_name=[%s]\n",ps->cash_name);
	vtcp_log("lv_pkgreg_c.cash_addr=[%s]\n",ps->cash_addr);
	vtcp_log("lv_pkgreg_c.lv_yw_ind=[%s]\n",ps->lv_yw_ind);
	vtcp_log("lv_pkgreg_c.lv_brf=[%s]\n",ps->lv_brf);
	vtcp_log("lv_pkgreg_c.resp_date=[%ld]\n",ps->resp_date);
	vtcp_log("lv_pkgreg_c.rcpstat=[%s]\n",ps->rcpstat);
	vtcp_log("lv_pkgreg_c.resp1=[%s]\n",ps->resp1);
	vtcp_log("lv_pkgreg_c.resp2=[%s]\n",ps->resp2);
	vtcp_log("lv_pkgreg_c.note_type=[%s]\n",ps->note_type);
	vtcp_log("lv_pkgreg_c.note_no=[%s]\n",ps->note_no);
	vtcp_log("lv_pkgreg_c.addid=[%s]\n",ps->addid);
	vtcp_log("lv_pkgreg_c.br_no=[%s]\n",ps->br_no);
	vtcp_log("lv_pkgreg_c.input_tel=[%s]\n",ps->input_tel);
	vtcp_log("lv_pkgreg_c.check_tel=[%s]\n",ps->check_tel);
	vtcp_log("lv_pkgreg_c.auth_tel=[%s]\n",ps->auth_tel);
	vtcp_log("lv_pkgreg_c.send_tel=[%s]\n",ps->send_tel);
	vtcp_log("lv_pkgreg_c.resp_tel=[%s]\n",ps->resp_tel);
	vtcp_log("lv_pkgreg_c.deal_tel=[%s]\n",ps->deal_tel);
	vtcp_log("lv_pkgreg_c.input_trace_no=[%s]\n",ps->input_trace_no);
	vtcp_log("lv_pkgreg_c.check_trace_no=[%s]\n",ps->check_trace_no);
	vtcp_log("lv_pkgreg_c.send_trace_no=[%s]\n",ps->send_trace_no);
	vtcp_log("lv_pkgreg_c.resp_trace_no=[%s]\n",ps->resp_trace_no);
	vtcp_log("lv_pkgreg_c.deal_trace_no=[%s]\n",ps->deal_trace_no);
	vtcp_log("lv_pkgreg_c.lv_sts=[%s]\n",ps->lv_sts);
	vtcp_log("lv_pkgreg_c.beg_sts=[%s]\n",ps->beg_sts);
	vtcp_log("lv_pkgreg_c.operlevel=[%s]\n",ps->operlevel);
	vtcp_log("lv_pkgreg_c.tx_chrg_ind=[%s]\n",ps->tx_chrg_ind);
	vtcp_log("lv_pkgreg_c.chrg_amt=[%f]\n",ps->chrg_amt);
	vtcp_log("lv_pkgreg_c.checkflag=[%s]\n",ps->checkflag);
	vtcp_log("lv_pkgreg_c.brprtflag=[%s]\n",ps->brprtflag);
	vtcp_log("lv_pkgreg_c.qsprtflag=[%s]\n",ps->qsprtflag);
	vtcp_log("lv_pkgreg_c.packid=[%s]\n",ps->packid);
	vtcp_log("lv_pkgreg_c.retcode=[%s]\n",ps->retcode);
	vtcp_log("lv_pkgreg_c.o_txnum=[%s]\n",ps->o_txnum);
	vtcp_log("lv_pkgreg_c.ywdtlid=[%s]\n",ps->ywdtlid);
	vtcp_log("lv_pkgreg_c.protno=[%s]\n",ps->protno);
	vtcp_log("lv_pkgreg_c.ac_type=[%s]\n",ps->ac_type);
	vtcp_log("lv_pkgreg_c.refno=[%s]\n",ps->refno);
	vtcp_log("lv_pkgreg_c.o_or_br_no=[%s]\n",ps->o_or_br_no);
	vtcp_log("lv_pkgreg_c.o_wt_date=[%ld]\n",ps->o_wt_date);
	vtcp_log("lv_pkgreg_c.o_pack_date=[%ld]\n",ps->o_pack_date);
	vtcp_log("lv_pkgreg_c.o_orderno=[%s]\n",ps->o_orderno);
	vtcp_log("lv_pkgreg_c.o_packid=[%s]\n",ps->o_packid);
	vtcp_log("lv_pkgreg_c.o_ac_br_no=[%s]\n",ps->o_ac_br_no);
	vtcp_log("lv_pkgreg_c.df_chrg_amt=[%f]\n",ps->df_chrg_amt);
	vtcp_log("lv_pkgreg_c.lv_o_sts=[%s]\n",ps->lv_o_sts);
	vtcp_log("lv_pkgreg_c.lw_ind=[%s]\n",ps->lw_ind);
	vtcp_log("lv_pkgreg_c.cur_no=[%s]\n",ps->cur_no);
	vtcp_log("lv_pkgreg_c.filler=[%s]\n",ps->filler);
	return(0);
}
/***********************************************************/
/**	函数名称:iLv_UpdateJjLzStat(PAY_IN_AREA *wp_payin_jjhz)*/
/** 参数:支付交易结构体                                   **/
/** 含义:根据回执结构体找到原始借记来帐业务并更新原始交易 **/
/**      交易状态为来帐退回状态                           **/
/** 返回:0成功 其他为失败                                 **/
/** 编写:	LiuHuafeng 2007-5-5 10:18                       **/
/***********************************************************/
int iLv_UpdateJjLzStat(PAY_IN_AREA *wp_payin_jjhz)
{
	PAY_IN_AREA wd_payin_or;
	int iRc=0;
	char cOr_br_no[13];
	char cOrderno[9];
	char cWt_date[9];
	
	iRc=0;
	memset(&wd_payin_or, 0 , sizeof(wd_payin_or));
	memset(cOr_br_no, 0 , sizeof(cOr_br_no));
	memset(cOrderno, 0 , sizeof(cOrderno));
	memset(cWt_date, 0 , sizeof(cWt_date));
			
	vtcp_log("[%s][%d] wp_payin_djth->T_stat ===[%.1s] ",__FILE__,__LINE__, wp_payin_jjhz->T_stat);
	vtcp_log("[%s][%d] wp_payin_jjhz->F_owtday ===[%.8s] ",__FILE__,__LINE__, wp_payin_jjhz->F_owtday);
	vtcp_log("[%s][%d] wp_payin_jjhz->F_oorderno ===[%.8s] ",__FILE__,__LINE__, wp_payin_jjhz->F_oorderno);
	vtcp_log("[%s][%d] wp_payin_jjhz->F_acbrno ===[%.12s] ",__FILE__,__LINE__, wp_payin_jjhz->F_acbrno);
	vtcp_log("[%s][%d] wp_payin_jjhz->F_otxnum ===[%.3s] ",__FILE__,__LINE__, wp_payin_jjhz->F_otxnum);

	if (wp_payin_jjhz->T_stat[0]!=STAT_WZSUCCESS)
	{
		vtcp_log("[%s][%d]非轧差状态不需要更新=%.2s\n",__FILE__,__LINE__,wp_payin_jjhz->F_rcpstat);
		return 0;
	}
	memcpy(wd_payin_or.F_wtday,wp_payin_jjhz->F_owtday,sizeof(wd_payin_or.F_owtday));
	memcpy(wd_payin_or.F_orderno,wp_payin_jjhz->F_oorderno,sizeof(wd_payin_or.F_orderno));
	memcpy(wd_payin_or.F_orbrno,wp_payin_jjhz->F_acbrno,sizeof(wd_payin_or.F_orbrno));

	if(memcmp(wp_payin_jjhz->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin_jjhz->F_pkgno))==0)
	{
		memcpy(wd_payin_or.F_pkgno,PKGNO_DQJJ ,sizeof(wd_payin_or.F_pkgno));
	}
	else if(memcmp(wp_payin_jjhz->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin_jjhz->F_pkgno))==0)
	{
		memcpy(wd_payin_or.F_pkgno,PKGNO_SSJJ,sizeof(wd_payin_or.F_pkgno));
	}
	else
	{
		memcpy(wd_payin_or.F_pkgno,PKGNO_PTJJ,sizeof(wd_payin_or.F_pkgno));
	}
	vtcp_log("[%s][%d] wd_payin_or.F_wtday ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_wtday);
	vtcp_log("[%s][%d] wd_payin_or.F_orderno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orderno);
	vtcp_log("[%s][%d] wd_payin_or.F_orbrno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orbrno);
	vtcp_log("[%s][%d] wd_payin_or.F_otxnum ===[%.3s] ",__FILE__,__LINE__, wd_payin_or.F_otxnum);

	iRc=iLvFetchRecToPayin(&wd_payin_or);
	if (iRc)
	{
		vtcp_log("[%s][%d]查找原来的退回来帐表错误sqlcode=%d\n",__FILE__,__LINE__,iRc);
		return iRc;
	}	
	vtcp_log("[%s][%d] === After iLvFetchRecToPayin() !! === ",__FILE__,__LINE__);

	iLvResetPayin(&wd_payin_or);

	vtcp_log("[%s][%d] === After iLvResetPayin() !! === ",__FILE__,__LINE__);

	vtcp_log("[%s][%d] wd_payin_or.T_stat ===[%.1s] ",__FILE__,__LINE__, wd_payin_or.T_stat);

	if (wd_payin_or.T_stat[0] == STAT_YES_NOTSEND)
		wd_payin_or.T_stat[0] = STAT_YES_SEND;
	else if(wd_payin_or.T_stat[0] == STAT_NO_NOTSEND)
		wd_payin_or.T_stat[0] = STAT_NO_SEND;
	if(wd_payin_or.T_stat[0] ==STAT_NO_SEND || wd_payin_or.T_stat[0] ==STAT_YES_SEND)
	{
		memcpy(cOr_br_no, wd_payin_or.T_orbrno, sizeof(cOr_br_no)-1);
		memcpy(cOrderno, wd_payin_or.T_orderno, sizeof(cOrderno)-1);
		memcpy(cWt_date, wd_payin_or.T_wtday, sizeof(cWt_date)-1);
		/*  更新原来的贷记来帐记录状态  */
		g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%c' where lw_ind = '2' and wt_date=%d and or_br_no='%s' and orderno='%s'", wd_payin_or.T_stat[0], apatoi(cWt_date,8), cOr_br_no, cOrderno);
		if ( g_reply_int != DB_OK )
		{
			sprintf( acErrMsg,"更新数据库失败 ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			return(-1);
		}
		vtcp_log("[%s][%d] 更新原贷记来帐记录状态成功 !! \n",__FILE__,__LINE__);
		return 0;
	}
	else
	{
		vtcp_log("[%s][%d] 原借记来帐状态错误! wd_payin_or.T_stat ===[%.1s] ",__FILE__,__LINE__, wd_payin_or.T_stat);
		return -1;
	}
	return 0;
}
