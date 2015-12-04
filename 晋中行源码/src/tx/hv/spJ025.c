/***************************************************************
* 文 件 名:   spJ025.c
* 功能描述：  撤消报文录入
*
* 作    者:   xyy
* 完成日期：  2006-8-18 20:11
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_define.h"
#include "hvpack.h"
#include "find_debug.h"
#define loc_t sys_loc_t

spJ025()
{
	int  ret=0;	
	int  iCmtno=0;
	int  iWtdate=0;
    
	char cCmtno[4];
	char cOrbrno[13];
	char cOrderno[9];
	char cBuff[2000];

	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack311;
	struct hv_uniontab_c hv_uniontab;
	struct hv_fzf_c hv_fzf;
	struct hv_zf_c hv_zf;

	memset(cCmtno,0,sizeof(cCmtno));
	memset(cOrbrno ,0 ,sizeof(cOrbrno));
	memset(cOrderno,0,sizeof(cOrderno));
	memset(cBuff,0,sizeof(cBuff));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&hv_uniontab,0,sizeof(hv_uniontab));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_zf,0,sizeof(hv_zf));
	memset(&pack311, 0 , sizeof(pack311));

	vtcp_log("[%s][%d]spJ025撤消报文录入BEGIN",__FILE__,__LINE__);
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	/***得到大额支付业务8583数据***/
	g_reply_int = iHvTistoNotpay(&hv_notpay);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用iHvTistoNotpay错误![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	iWtdate = apatoi(hv_notpay.F_o_wt_date,8);
	MEMCPY_DEBUG(cOrbrno,hv_notpay.F_or_br_no,sizeof(cOrbrno)-1);
	
	MEMCPY_DEBUG(cOrderno,hv_notpay.F_o_payin_orderno,sizeof(cOrderno)-1);
	zip_space(cOrderno);


	vtcp_log("[%s][%d]cOrderno=[%s]",__FILE__,__LINE__,cOrderno);
	MEMCPY_DEBUG(cCmtno,hv_notpay.F_o_cmtno,sizeof(cCmtno)-1);
	/******检查营业的状态******/ 

	g_reply_int = iHv_checkstat(hv_notpay.F_cmtno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"工作状态不正常![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]调用完iHv_checkstat",__FILE__,__LINE__);	
	
	g_reply_int = Hv_zf_Sel(g_pub_tx.reply,&hv_zf,"or_br_no='%s' and cmtno='%s' and wt_date='%ld' and orderno=%s", cOrbrno,cCmtno,iWtdate,cOrderno);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"记录状态不正常，或不存在此记录![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	if(hv_zf.hv_sts[0] != SEND_INQUEUE && hv_zf.hv_sts[0] != SEND_ACCT_WAIT)
	{
		sprintf( acErrMsg,"记录状态不正常，不允许做撤消业务![%s][%d][%s]",__FILE__,__LINE__,g_reply_int);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	MEMCPY_DEBUG(hv_notpay.F_o_or_br_no,hv_zf.or_br_no,sizeof(hv_notpay.F_or_br_no));/**得到原交易的发起行行号**/
	MEMCPY_DEBUG(hv_notpay.F_ac_br_no,hv_zf.ac_br_no,sizeof(hv_notpay.F_ac_br_no));/**得到原交易的接收行行号**/
	MEMCPY_DEBUG(hv_notpay.F_o_ac_br_no,hv_zf.ac_br_no,sizeof(hv_notpay.F_ac_br_no));/**得到原交易的接收行行号**/
	vtcp_log("[%s][%d]hv_zf.tx_amt=[%f]",__FILE__,__LINE__,hv_zf.tx_amt);
	dot_dbl2str(hv_zf.tx_amt,sizeof(hv_notpay.F_o_txamt) ,2,hv_notpay.F_o_txamt);/**原交易金额**/
	vtcp_log("[%s][%d]hv_notpay.T_o_tx_amt=[%s]",__FILE__,__LINE__,hv_notpay.T_o_tx_amt);
 	g_reply_int = iHvfFrontToTable(&hv_notpay,cBuff);
	if(g_reply_int)
	{
		sprintf( acErrMsg,"调用iHvfFrontToTable错误![%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P018" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	hv_notpay.T_otype[0]='1';
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;/*更新状态*/
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,1);
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调iHvUpdFzfRecFromPayin出错[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	vtcp_log("[%s][%d]-----------------------------",__FILE__,__LINE__);
	/*** 开始写历史明细 ***/ 

	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if (g_reply_int)
	{
		sprintf( acErrMsg,"插入自由格式明细表出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay->T_otype[0]=[%c]",__FILE__,__LINE__,hv_notpay.T_otype[0]);
	/****Add by SSH,2006.8.28,直接发送****/
	pub_com_CMT311_T2TAG(&hv_notpay,&pack311);
	ret=hv_gen_pack(&pack311);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack311,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0){
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack311)){
		sprintf(acErrMsg,"发送311报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	{
		char pcBuf[1024];
		memset(pcBuf,'\0',sizeof(pcBuf));
		get_fd_data("1230",pcBuf);
		vtcp_log("[%s][%d]Gethere,123 area \n[%s]",__FILE__,__LINE__,pcBuf);
	}
	vtcp_log("[%s][%d]--------GAME OVER!!!--------------",__FILE__,__LINE__);
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
