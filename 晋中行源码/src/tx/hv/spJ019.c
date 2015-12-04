/***************************************************************
* 文 件 名:     spJ019.c
* 功能描述：    大额申请退回录入		
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18  17:27
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hv_define.h"
#include "hvpack.h"
#include "find_debug.h"

spJ019()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
        struct	hv_zf_c        hv_zf;
	struct  hv_fzf_c       hv_fzf;
	HV_FD123_AREA          fd123 ;
	THVPACKAGE   pack313;
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_zf,	      0x00 ,  sizeof(hv_zf));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&fd123 ,       0x00 ,  sizeof(fd123));
	memset(&pack313, 0 , sizeof(pack313));


	/*****************************
     *PART 1   得到前台数据      *
     *****************************/
	/***得到大额支付业务8583数据***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/**********************
     *PART 2   基本检测   *
	 **********************/
	 /***********锁定一下支付表************/
	MEMCPY_DEBUG(hv_zf.orderno,hv_notpayin.F_o_payin_orderno,sizeof(hv_zf.orderno)-1);
        GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpayin.F_or_br_no);
	MEMCPY_DEBUG(hv_zf.or_br_no,hv_notpayin.F_or_br_no,sizeof(hv_zf.or_br_no)-1);
	hv_zf.wt_date = apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date));
	MEMCPY_DEBUG(hv_zf.cmtno, hv_notpayin.F_o_cmtno,sizeof(hv_zf.cmtno)-1);

	vtcp_log("orderno=[%s]\n",hv_zf.orderno);
	vtcp_log("or_br_no==[%s]\n",hv_zf.or_br_no);
	vtcp_log("wt_date==[%lf]\n",hv_zf.wt_date);
	vtcp_log("cmtno====[%s]\n",hv_zf.cmtno);

	ret = Hv_zf_Dec_Upd(g_pub_tx.reply,"orderno='%s' and or_br_no='%s' and wt_date=%d and cmtno='%s' ",hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date,hv_zf.cmtno);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_zf_dec_upd错误!ret=[%d]\n",ret);
		strcpy(g_pub_tx.reply,"P017");	
		WRITEMSG
		goto ErrExit;
	}
	ret = Hv_zf_Fet_Upd(&hv_zf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_zf_fet_Upd错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P017");	
		goto ErrExit;
	}
	/*****更行状态********/
	if(hv_zf.hv_sts[0]!=SEND_ACCT_QS)
	{
		sprintf(acErrMsg,"原支付交易不允许作退回申请[%c]\n",hv_zf.hv_sts[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P016");
		goto ErrExit;
	}
	
	ret = Hv_zf_Upd_Upd(hv_zf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数hv_zf_upd_upd失败!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P017");	
		goto ErrExit;
	}

		

	/*********************
	 *PART 3  操作数据库 *
         *********************/
	MEMCPY_DEBUG(hv_notpayin.F_o_or_br_no,hv_notpayin.F_or_br_no,sizeof(hv_notpayin.F_o_or_br_no));
	MEMCPY_DEBUG(hv_notpayin.F_o_ac_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_o_ac_br_no));
	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数F->T错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P015");
		goto ErrExit;
	}
	/**************获取新的非支付交易的序号
	ret = iGetHvOrderno(hv_notpayin.T_orderno);
	if(ret)
	{
		sprintf(acErrMsg,"生成新的交易序号错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P019");
		goto ErrExit;
	}
	************************************************/


	/*****************只能加到这里了***********/
	hv_notpayin.T_hv_fzf_sts[0]=SEND_ACCT_WAIT;	

	ret = iHvUpdFzfRecFromPayin(&hv_notpayin,1);
	if(ret)
	{
		sprintf(acErrMsg,"插入记录失败!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P010");
		goto ErrExit;
	}

	/************插入非支付明细表*****************/
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"插入非支付明细表错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P011");
		goto ErrExit;
	}
	/******提取返回值*****/
	MEMCPY_DEBUG(fd123.orderno,hv_notpayin.T_orderno,sizeof(fd123.orderno));
	MEMCPY_DEBUG(fd123.or_br_no,hv_notpayin.T_or_br_no,sizeof(fd123.or_br_no));

     /*******************
     *PART 4  结尾工作 *
     *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/
  /****Add by SSH,2006.8.28,直接发送****/
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s]",__FILE__,__LINE__,hv_notpayin.T_req_content);
	pub_com_CMT313_T2TAG(&hv_notpayin,&pack313);
	vtcp_log(" [%s][%d]hv_notpay->T_req_content==[%s],sizeof(pack313)=[%d] ",__FILE__,__LINE__,hv_notpayin.T_req_content);
	ret=hv_gen_pack(&pack313);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack313,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
			,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
		sprintf(acErrMsg,"插入发送登记簿失败!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P010" );
		goto ErrExit;
	}
	if(hv_sendpack(&pack313)){
		sprintf(acErrMsg,"发送313报文到前置机失败!\n");
		strcpy( g_pub_tx.reply, "LS88" );
		WRITEMSG
		goto ErrExit;
	}
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

