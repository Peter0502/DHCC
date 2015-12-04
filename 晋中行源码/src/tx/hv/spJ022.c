/*************************************************
* 文 件 名:  spJ022.c
* 功能描述： 查复信息的录入
* 作    者:  xyy 
* 完成日期： 2006年8月18日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#define loc_t sys_loc_t
#include "find_debug.h"

int iGetHvAddid(char * addid);
int spJ022()
{
	int ret=0;	
	int iOrderno=0;
	int iWtdate=0;
	char cTxbank[13];
	char cOwtdate[9];
	char cOorderno[9];
	char cOrderno[9];
	char cOrbrno[13];
	char cRefno[21];
	char cWssrno[7];
	char cBrf[256];
	struct hv_fzf_c hv_fzf;
	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack302;
	
	/*把查询表中的相关记录导到查复表中**/    
	memset(cTxbank,0,sizeof(cTxbank));
	memset(cOorderno,0,sizeof(cOorderno));
	memset(cOrderno,0,sizeof(cOrderno));
	memset(cOrbrno,0,sizeof(cOrbrno));
	memset(cBrf	  ,0,sizeof(cBrf));
	memset(cRefno,0,sizeof(cRefno));
	memset(cWssrno,0,sizeof(cWssrno));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(cOwtdate,0,sizeof(cOwtdate));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&pack302, 0 , sizeof(pack302));
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/***得到大额支付业务8583数据***/
	g_reply_int = iHvTistoNotpay(&hv_notpay);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用iHvTistoNotpay错误![%s][%d]",__FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cWssrno,"%6d",g_pub_tx.trace_no);
	vtcp_log("[%s][%d]cWssrno=[%s]",__FILE__,__LINE__,cWssrno);
	vtcp_log("[%s][%d]hv_notpay.F_or_br_no        =[%s]",__FILE__,__LINE__,hv_notpay.F_or_br_no);
	vtcp_log("[%s][%d]hv_notpay.F_o_fzf_date      =[%s]",__FILE__,__LINE__,hv_notpay.F_o_fzf_date);
	vtcp_log("[%s][%d]hv_notpay.F_o_notpay_orderno=[%s]",__FILE__,__LINE__,hv_notpay.F_o_notpay_orderno);
	MEMCPY_DEBUG(cOrderno,hv_notpay.F_orderno,sizeof(cOrderno)-1);
	MEMCPY_DEBUG(cBrf    ,hv_notpay.F_content,sizeof(cBrf)-1);
	MEMCPY_DEBUG(cOrbrno ,hv_notpay.F_or_br_no,sizeof(cOrbrno)-1);
	MEMCPY_DEBUG(cOwtdate,hv_notpay.F_o_wt_date,sizeof(cOwtdate)-1);
	MEMCPY_DEBUG(cOorderno,hv_notpay.F_o_notpay_orderno,sizeof(cOorderno)-1);
	iWtdate = apatoi(hv_notpay.F_o_fzf_date,8);
	MEMCPY_DEBUG(cRefno,hv_notpay.F_refno,sizeof(cRefno)-1);
	
	vtcp_log("[%s][%d]cOrbrno      =[%s]",__FILE__,__LINE__,cOrbrno);
	vtcp_log("[%s][%d]iWtdate      =[%d]",__FILE__,__LINE__,iWtdate);
	vtcp_log("[%s][%d]cOorderno     =[%s]",__FILE__,__LINE__,cOorderno);
	
		
	/******检查营业的状态******/ 

	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"工作状态不正常![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]调用完iHv_checkstat",__FILE__,__LINE__);
		
	GetOr_br_noByBr_no(hv_notpay.F_tx_br_no,cTxbank);/*根据机构号找行号*/
	vtcp_log("[%s][%d]cTxbank=[%s]",__FILE__,__LINE__,cTxbank);
	/** 锁定原查询记录 **/
  	
  	
	g_reply_int = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"or_br_no = '%s' and wt_date ='%d' and orderno ='%s' and cmtno='301'",cOrbrno,iWtdate,cOorderno);       	
	if (g_reply_int)
	{
		sprintf( acErrMsg,"锁定接收行出错,[%s][%d]", __FILE__,__LINE__);
		set_zd_data("0130",acErrMsg);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	if(memcmp(hv_fzf.br_no,hv_notpay.F_tx_br_no,sizeof(hv_fzf.br_no)-1)!=0)
	{
		sprintf( acErrMsg,"不是本机构不允许做此业务,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit; 
	}                                                                      
	if(hv_fzf.hv_fzf_sts[0]!=RECEIVE_ACCT_QS) 
	{
		sprintf( acErrMsg,"查询表记录状态错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		goto ErrExit; 
	}                                                                                        
	
	/*更换为原查询交易的基本信息*/
	MEMCPY_DEBUG(hv_notpay.F_wt_date ,hv_notpay.F_o_fzf_date,sizeof(hv_notpay.F_wt_date));
	MEMCPY_DEBUG(hv_notpay.F_orderno ,hv_notpay.F_o_notpay_orderno,sizeof(hv_notpay.F_orderno));
	MEMCPY_DEBUG(hv_notpay.F_cmtno   ,"301",sizeof(hv_notpay.F_cmtno));
	
	iHvTable2Notpayin(&hv_notpay);/*得到原查询交易的信息*/
	vtcp_log("[%s][%d]hv_notpay.T_cmtno =[%s]",__FILE__,__LINE__,hv_notpay.T_cmtno);
	vtcp_log("[%s][%d]T_o_tx_amt=[%s]\n",__FILE__,__LINE__,hv_notpay.T_o_tx_amt);
	/************************
	**原委托日期**
	MEMCPY_DEBUG(hv_notpay.T_o_wt_date,cOwtdate,sizeof(cOwtdate)-1);
	*申请日期*
	MEMCPY_DEBUG(hv_notpay.T_req_date,hv_notpay.T_wt_date,sizeof(hv_notpay.T_req_date));
	*申请行号*
	MEMCPY_DEBUG(hv_notpay.T_req_br_no,hv_notpay.T_or_br_no,sizeof(hv_notpay.T_req_br_no));
	**申请清算行行号***
	iGet_full_hv_no(hv_notpay.T_req_br_no,hv_notpay.T_receco,hv_notpay.T_pay_qs_no);
	*申请序号*
	MEMCPY_DEBUG(hv_notpay.T_req_orderno,hv_notpay.T_orderno,sizeof(hv_notpay.T_req_orderno));
	*******************/
	MEMCPY_DEBUG(hv_notpay.T_cmtno,"301",3);
	MEMCPY_DEBUG(hv_notpay.F_cmtno,"301",3);
	hv_notpay.T_lw_ind[0]='2';
	hv_notpay.F_lw_ind[0]='2';
	/**应答日期,即当前日期**/
	get_zd_data("0440",hv_notpay.T_res_date);
	/*应答行号*/
	vtcp_log("[%s][%d]cTxbank=[%s]",__FILE__,__LINE__,cTxbank);
	MEMCPY_DEBUG(hv_notpay.T_res_br_no,cTxbank,sizeof(cTxbank)-1);
	vtcp_log("[%s][%d]hv_notpay.T_res_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_res_br_no);
	/*应答清算号*/
	iGet_full_hv_no(cTxbank,hv_notpay.T_receco,hv_notpay.T_cash_qs_no);
	/*应答内容*/
	MEMCPY_DEBUG(hv_notpay.T_res_content,cBrf,sizeof(cBrf)-1);
	/*应答序号*/
	MEMCPY_DEBUG(hv_notpay.T_res_orderno,cOrderno,sizeof(cOrderno)-1);
	/*记录状态*/
	hv_notpay.T_hv_fzf_sts[0]= RECEIVE_BACK_ACCT;
	/*流水号*/
	MEMCPY_DEBUG(hv_notpay.T_trace_no,cWssrno,sizeof(cWssrno)-1);
	/***报文头**/
	MEMCPY_DEBUG(hv_notpay.T_refno,cRefno,sizeof(cRefno)-1);
	vtcp_log("[%s][%d]hv_notpay.T_trace_no=[%.6s]",__FILE__,__LINE__,hv_notpay.T_trace_no);
	vtcp_log("[%s][%d]hv_notpay.T_hv_fzf_sts =[%c]",__FILE__,__LINE__,hv_notpay.T_hv_fzf_sts[0]);
	vtcp_log("[%s][%d]hv_notpay.T_refno =[%s]",__FILE__,__LINE__,hv_notpay.T_refno);
	vtcp_log("[%s][%d]hv_notpay.T_cmtno =[%s]",__FILE__,__LINE__,hv_notpay.T_cmtno);
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,0);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"调ivUpdFzfRecFromPayin出错[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
  vtcp_log("[%s][%d]hv_notpay.T_cmtno =[%s]",__FILE__,__LINE__,hv_notpay.T_cmtno);
	/*** 开始写历史明细 ***/ 
	   
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"插入自由格式明细表出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	/****Add by SSH,2006.8.28,直接发送****/
	pub_com_CMT302_T2TAG(&hv_notpay,&pack302);
	ret=hv_gen_pack(&pack302);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack302,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack302)){
		sprintf(acErrMsg,"发送302报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}


OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);

	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;			
}  

