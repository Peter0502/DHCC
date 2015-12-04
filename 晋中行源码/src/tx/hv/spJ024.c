
/*************************************************
* 文 件 名:  spJ024.c
* 功能描述： 大额非支付类交易往账发送
*
* 作    者:  xyy 
* 完成日期： 2024年8月9日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/
#define EXTERN
#include <string.h>
#include <stdio.h>
#include "hv_pub.h"
#include "hvnet.h"
#include "hvpack.h"
#include "hv_define.h"
#include "find_debug.h"
#define loc_t sys_loc_t


int spJ024()
{	
	int  iCmtno = 0;
	int  ret    = 0;
	int  iRc   =0;
	int  iSocketID ;
	char cCmtno		[4];
	char cWssrno	[7];
	char cTel		[7];
	char cFindOrderno [9];
	char tmpBuf		[64];
	char cQsbrno[BRNO_LEN+1];
	
	HV_NOTPAY_AREA   hv_notpay;
	THVPACKAGE pack_fzf;
	
	memset(cCmtno			,0,sizeof(cCmtno));
	memset(cQsbrno			,0,sizeof(cQsbrno));
	memset(cFindOrderno		,0,sizeof(cFindOrderno));
	memset(cWssrno			,0,sizeof(cWssrno));
	memset(cTel			,0,sizeof(cTel));
	memset(&hv_notpay       	,0,sizeof(hv_notpay));
	memset(&pack_fzf		,'\0',sizeof(pack_fzf));
                  			
	pub_base_sysinit();

	sprintf(cWssrno,"%6d",g_pub_tx.trace_no);
	vtcp_log("[%s][%d]cWssrno=[%s]",__FILE__,__LINE__,cWssrno);
	vtcp_log("spJ024喽[%s][%d]",__FILE__,__LINE__);
	
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
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	MEMCPY_DEBUG(cCmtno	,hv_notpay.F_cmtno	,sizeof(cCmtno)-1);
	MEMCPY_DEBUG(cTel	    ,hv_notpay.F_tel	,sizeof(cTel)-1);
	
	iCmtno=apatoi(cCmtno,3);

	/*有哪些交易不能在清算中心发的*/
	if(memcmp(cCmtno,"724",sizeof(cCmtno)-1)==0 || memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0 )
	{
		if(find_qsbrno(cQsbrno)) 
		{
			sprintf(acErrMsg,"取得清算中心号出错!\n");
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
		/***检查是否是清算中心****/
		if(memcmp(hv_notpay.F_tx_br_no,cQsbrno,BRNO_LEN)==0) 
		{
			sprintf(acErrMsg,"清算中心不能做此交易!\n");
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}
	
	ret =	iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数iHvTable2Notpayin错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	
	/* 对流水号处理啊！	*/
	MEMCPY_DEBUG(hv_notpay.T_trace_no,cWssrno,sizeof(cWssrno)-1);
	vtcp_log("[%s][%d]hv_notpay.T_trace_no=[%.6s]",__FILE__,__LINE__,hv_notpay.T_trace_no);
	
	if(memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0)/*判断该笔业务的状态*/
	{
		if( hv_notpay.T_hv_fzf_sts[0]!=SEND_ACCT_CHECK)
		{
			sprintf(acErrMsg,"状态不是复核，不允许发送!\n");
			vtcp_log("T_hv_fzf_sts[0]=[%c]\n",hv_notpay.T_hv_fzf_sts[0]);
			strcpy( g_pub_tx.reply, "HV01" );
			WRITEMSG
			goto ErrExit;
		}
	}
	else
	{
	if( hv_notpay.T_hv_fzf_sts[0]!=SEND_REGIST)
	{
		vtcp_log("[%s][%d]f_sts=[%c]",__FILE__,__LINE__,hv_notpay.T_hv_fzf_sts[0]);
		sprintf(acErrMsg,"状态不正常，不允许发送[%c]!\n",hv_notpay.T_hv_fzf_sts[0]);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	}
	
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;/*更新状态*/
	MEMCPY_DEBUG(hv_notpay.T_tel,cTel,sizeof(cTel)-1);
	hv_notpay.T_lw_ind[0]='1';/*往*/
	g_reply_int = iHvUpdFzfRecFromPayin(&hv_notpay,0); /*更新该表*/
	if( g_reply_int )
	{
		sprintf( acErrMsg,"调用函数iHvUpdFzfRecFromPayin出错,[%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	g_reply_int = dg_hv_fzfhtr(&hv_notpay);
	if( g_reply_int )
	{
		sprintf(acErrMsg,"插入大额明细表错误!\n");
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	vtcp_log("[%s][%d]iCmtno=[%.3d]",__FILE__,__LINE__,iCmtno);
	switch(iCmtno) /*开始拼包,插入发送登记簿*/           
	{
	    case 311:  /*撤消申请*/
	    	if(cCheck_stat(hv_notpay))
	    	{
	    		sprintf(acErrMsg,"检查该笔撤消交易的原状态错误!\n");
        		WRITEMSG
	    		goto ErrExit;/*状态不正确*/
	    	}
	    	pub_com_CMT311_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	    case 313:  /*退回申请*/
	    	pub_com_CMT313_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	    case 314:  /*退回应答*/
	    	pub_com_CMT314_T2TAG(&hv_notpay,&pack_fzf);
	    	break;
	     case 724:  /*未用汇票退回,是否需要修改一下汇票基本信息的状态呢？*/
	    	pub_com_CMT724_T2TAG(&hv_notpay,&pack_fzf);
	    	break;		
	     case 721:  /*资金清算*/
	    	pub_com_CMT721_T2TAG(&hv_notpay,&pack_fzf);
	    	break;	
	    default:
	    	sprintf(acErrMsg,"没有该报文类型!\n");
        	WRITEMSG
	    	goto ErrExit;
	    	break;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	getPackageFlgNo(&pack_fzf); /**得到FlgNo**/ 
	ret=hv_gen_pack(&pack_fzf);
	if(ret<0)
	{
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("[%s][%d]hv_notpay.T_ac_br_no=[%s]",__FILE__,__LINE__,hv_notpay.T_ac_br_no);
	if(hv_insert_sendlist(&pack_fzf,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
		,hv_notpay.T_trace_no,hv_notpay.T_orderno)<0)
	{
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*发送报文到前置机**/
	if(hv_sendpack(&pack_fzf)){
		sprintf(acErrMsg,"发送报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
	/*发送交易要写流水*/
	if( pub_ins_trace_log() )
	{
	sprintf(acErrMsg,"大额发送交易写流水错误!");
	strcpy( g_pub_tx.reply, "S023" );
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

cCheck_stat(HV_NOTPAY_AREA hv_notpay)/*当撤消交易的时候，需要判断原交易的状态*/
{
	int ret;
	HV_NOTPAY_AREA hv_notpay_bak;
	memset(&hv_notpay_bak,0,sizeof(hv_notpay_bak));
	MEMCPY_DEBUG(&hv_notpay_bak,&hv_notpay,sizeof(HV_NOTPAY_AREA)); /*保存原来的结构体*/
	MEMCPY_DEBUG(hv_notpay.T_orderno ,hv_notpay.T_o_payin_orderno	,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_or_br_no,hv_notpay.T_or_br_no,sizeof(hv_notpay.T_or_br_no));
	MEMCPY_DEBUG(hv_notpay.T_wt_date ,hv_notpay.T_o_wt_date,sizeof(hv_notpay.T_wt_date));
	MEMCPY_DEBUG(hv_notpay.T_cmtno   ,hv_notpay.T_o_cmtno  ,sizeof(hv_notpay.T_cmtno));
	
	ret = iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数iHvTable2Notpayin错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		return (-1);	
	}
	
	if (hv_notpay.T_hv_fzf_sts[0] != SEND_INQUEUE && hv_notpay.T_hv_fzf_sts[0] != SEND_ACCT_WAIT)
	{
		return 1;
	}
	MEMCPY_DEBUG(&hv_notpay,&hv_notpay_bak,sizeof(HV_NOTPAY_AREA));/*恢复到原来的机构体*/
	return 0;
}

cChange_stat(HV_NOTPAY_AREA hv_notpay)
{
	HV_NOTPAY_AREA hv_notpay_bak;
	memset(&hv_notpay_bak,0,sizeof(hv_notpay_bak));
	MEMCPY_DEBUG(&hv_notpay_bak,&hv_notpay,sizeof(HV_NOTPAY_AREA));/*保存原来的结构体*/
	
	MEMCPY_DEBUG(hv_notpay.T_or_br_no ,hv_notpay.T_o_or_br_no,sizeof(hv_notpay.T_or_br_no));	
	MEMCPY_DEBUG(hv_notpay.T_wt_date,hv_notpay.T_o_wt_date,sizeof(hv_notpay.T_wt_date));
	MEMCPY_DEBUG(hv_notpay.T_orderno,hv_notpay.T_req_orderno,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_cmtno  ,hv_notpay.T_o_cmtno	,sizeof(hv_notpay.T_cmtno));
	iHvTable2Notpayin(&hv_notpay);
	hv_notpay.T_hv_fzf_sts[0]=RECEIVE_BACK_ACCT; /*已查复*/
	iHvUpdZfRecFromPayin(&hv_notpay,0);
	MEMCPY_DEBUG(&hv_notpay,&hv_notpay_bak,sizeof(HV_NOTPAY_AREA));/*恢复到原来的机构体*/
}	    	
