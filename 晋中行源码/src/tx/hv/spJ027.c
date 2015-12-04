/*************************************************
* 文 件 名:  spJ027.c
* 功能描述:  银行承兑汇票查复书录入
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
int CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack);

int spJ027()
{
	int ret=0;	
	char cContent_flag[2];/***查复内容的标签***/
	char cBrf[200];
	HV_FD123_AREA      fd123;
	char cContent[256];
	struct hv_fzf_c hv_fzf;
	HV_NOTPAY_AREA hv_notpay;
	THVPACKAGE pack302;
	
	/*把查询表中的相关记录导到查复表中**/   
	memset(&fd123,0x00,sizeof(fd123)); 
	memset(cContent_flag, 0x00, sizeof(cContent_flag));
	memset(cBrf	  ,0,sizeof(cBrf));
	memset(&hv_fzf,0,sizeof(hv_fzf));
	memset(&hv_notpay,0,sizeof(hv_notpay));
	memset(&pack302, 0 , sizeof(pack302));
	memset(cContent,0x00,sizeof(cContent));

	MEMCPY_DEBUG(cBrf,hv_notpay.F_content,sizeof(cBrf)-1);
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/***得到大额支付业务8583数据***/
	ret = iHvTistoNotpay(&hv_notpay);
	if( ret )
	{
		sprintf( acErrMsg,"从前台获取数据失败![%s][%d]",__FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "P015" );
		WRITEMSG
		goto ErrExit;
	}
	MEMCPY_DEBUG(cBrf    ,hv_notpay.F_content,sizeof(cBrf)-1);
	/**发起行和接受行不能相同***/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_notpay.F_or_br_no );
	if(ret)
	{
		sprintf(acErrMsg,"根据机构号取本行行号错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	if(!memcmp(hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_notpay.F_or_br_no)))
	{
		sprintf(acErrMsg,"发起行和接受行不能够是同一个or[%s]ac[%s]\n",hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	get_zd_data("0720",cContent_flag);/**获取一个标志,前台传来***/

	vtcp_log("cContent_flag==[%s][%d]\n",cContent_flag,__LINE__);
	vtcp_log("[%s][%d]开始填写查复的内容!\n",__FILE__,__LINE__);

	if(cContent_flag[0]=='0')/**信息相符***/
	{
		MEMCPY_DEBUG(cContent,"查询汇票与我行承兑汇票记载内容一致",34);	
		strncat(cContent+strlen(cContent),cBrf,sizeof(cBrf));
	}
	else if(cContent_flag[0]=='1')/***信息不符合,由一个固定字段+输入字段**/
	{
		zip_space(cBrf);
		MEMCPY_DEBUG(cContent, "查询汇票与我行承兑汇票记载内容不符，具体不符事项为：",52);
		strncat(cContent+strlen(cContent),cBrf,sizeof(cBrf));
	}
	vtcp_log("cContent==[%s]\n",cContent);
		
	/******检查营业的状态******/ 

	ret = iHv_checkstat(hv_notpay.F_cmtno);
	if( ret )
	{
		sprintf( acErrMsg,"工作状态不正常![%s][%d][%s]",__FILE__,__LINE__,ret);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("[%s][%d]调用完iHv_checkstat",__FILE__,__LINE__);
		
	
	/** 锁定原查询记录 **/
  	vtcp_log("hv_notpay.F_o_fzf_date==[%s]\n",hv_notpay.F_o_fzf_date);
	vtcp_log("hv_notpay.F_ac_br_no==[%s]\n",hv_notpay.F_ac_br_no);
	vtcp_log("hv_notpay.F_orderno==[%s]\n",hv_notpay.F_orderno);
	vtcp_log("hv_notpay.F_o_cmtno==[%s]\n",hv_notpay.F_o_cmtno);	
  	
	ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"or_br_no = '%.12s' and wt_date =%d and orderno ='%s' and cmtno='%s' ",hv_notpay.F_ac_br_no,apatoi(hv_notpay.F_o_fzf_date,sizeof(hv_notpay.F_o_fzf_date)),hv_notpay.F_orderno,hv_notpay.F_o_cmtno);
	if (ret)
	{
		sprintf( acErrMsg,"锁定接收行出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}
	if(hv_fzf.hv_fzf_sts[0]!=RECEIVE_ACCT_QS) 
	{
		sprintf( acErrMsg,"查询表记录状态错误,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit; 
	}
	/******更新原查询记录的信息*****/

	ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"or_br_no='%.12s' and wt_date='%s' and cmtno='%s' and orderno='%s' ",
			hv_notpay.F_ac_br_no,hv_notpay.F_o_fzf_date,hv_notpay.F_o_cmtno,hv_notpay.F_orderno);
	if(ret)
	{
		sprintf(acErrMsg,"数据表错误\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	ret = Hv_fzf_Fet_Upd(&hv_fzf,g_pub_tx.reply);
	if(ret&&ret!=100)
	{
		sprintf(acErrMsg,"数据库错误!ret==[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	else if(ret==100)
	{	
		sprintf(acErrMsg,"找不到原来的汇票查询的记录!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;	
	}
	
        if(hv_fzf.hv_fzf_sts[0]!=RECEIVE_ACCT_QS)
        {
                sprintf( acErrMsg,"查询表记录状态错误,[%s][%d]", __FILE__,__LINE__);
                strcpy( g_pub_tx.reply, "HV01" );
                WRITEMSG
                goto ErrExit;
        }
	
	
	
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpay.F_or_br_no);
	/*根据机构号找行号*/
        /*更换为原查询交易的基本信息*/
	/**应答日期,即当前日期**/
	get_zd_data("0440",hv_notpay.T_res_date);
	hv_fzf.res_date = apatoi(hv_notpay.T_res_date,sizeof(hv_notpay.T_res_date));
	/*应答行号*/
	MEMCPY_DEBUG(hv_fzf.res_br_no,hv_notpay.F_or_br_no,sizeof(hv_notpay.F_or_br_no));
	/*应答内容*/
	MEMCPY_DEBUG(hv_fzf.res_content,cContent,sizeof(cContent)-1);
	/*应答序号*/
	ret = iGetHvFzfOrderno(hv_fzf.res_orderno);
	vtcp_log("[%s][%d]res_orderno==[%s]\n",__FILE__,__LINE__,hv_fzf.res_orderno);
	MEMCPY_DEBUG(hv_fzf.refno,hv_notpay.F_refno,sizeof(hv_notpay.F_refno));
	/*记录状态*/
	hv_notpay.T_hv_fzf_sts[0]=SEND_ACCT_QS;
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]begin to update !\n",__FILE__,__LINE__);

	ret = Hv_fzf_Upd_Upd(hv_fzf,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"更新数据表失败!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P003");
		goto ErrExit;
	}
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]UPDATE END!!!!!!\n",__FILE__,__LINE__);

	/****开始来准备notpay结构体的内容*****/

	

	vtcp_log("[%s][%d]开始对原查询记录进行更新!\n",__FILE__,__LINE__);
	/***为了锁定原来的查询交易的记录,对几个字段进行强制更新***/
	MEMCPY_DEBUG(hv_notpay.F_or_br_no,hv_notpay.F_ac_br_no,sizeof(hv_notpay.F_or_br_no));
	MEMCPY_DEBUG(hv_notpay.F_wt_date,hv_notpay.F_o_fzf_date,sizeof(hv_notpay.F_wt_date));
	MEMCPY_DEBUG(hv_notpay.F_cmtno,hv_notpay.F_o_cmtno,sizeof(hv_notpay.F_cmtno));
	ret = iHvTable2Notpayin(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"从数据表中获取数据失败!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/*****还原被强制转换的字段*****/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no, hv_notpay.T_or_br_no );
	get_zd_data("0440",hv_notpay.T_wt_date);
	MEMCPY_DEBUG(hv_notpay.T_orderno,hv_fzf.res_orderno,sizeof(hv_notpay.T_orderno));
	MEMCPY_DEBUG(hv_notpay.T_cmtno,"302",3);
	sprintf(hv_notpay.T_trace_no,"%6d" ,g_pub_tx.trace_no);

	/****接口那不知道怎么无法对pay_qs_no赋值*****/
	MEMCPY_DEBUG(hv_notpay.T_pay_qs_no,hv_fzf.pay_qs_no,sizeof(hv_notpay.T_pay_qs_no));
	sprintf(hv_notpay.T_o_wt_date,"%8d",hv_fzf.o_wt_date);
	MEMCPY_DEBUG(hv_notpay.T_o_or_br_no,hv_fzf.o_or_br_no,sizeof(hv_notpay.T_o_or_br_no));

	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.pay_qs_no===[%s]\n",hv_fzf.pay_qs_no);	
	vtcp_log("hv_fzf.o_tx_date===[%ld]\n",hv_fzf.o_tx_date);	

	print_notpay(&hv_notpay,'T');
	   
	ret = dg_hv_fzfhtr(&hv_notpay);
	if (ret)
	{
		sprintf( acErrMsg,"插入自由格式明细表出错,[%s][%d]", __FILE__,__LINE__);
		strcpy( g_pub_tx.reply, "HV01" );
		WRITEMSG
		goto ErrExit;
	}


	/*******************
	ret =CMT302_T2TAG(&hv_notpay,&pack302);
	ret=hv_gen_pack(&pack302);
	if(ret<0){
		sprintf(acErrMsg,"组织大额支付报文错误!\n");
		WRITEMSG
		strcpy( g_pub_tx.reply, "P021" );
		goto ErrExit;
	}
	if(hv_insert_sendlist(&pack302,hv_notpay.F_tx_br_no,hv_notpay.F_tel\
			,g_pub_tx.trace_no,hv_notpay.T_orderno)<0){
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
	*************************/
	
	/*********准备输出*************/
	MEMCPY_DEBUG(fd123.orderno,hv_notpay.T_res_orderno,sizeof(hv_notpay.T_res_orderno));
	MEMCPY_DEBUG(fd123.ac_br_no,hv_notpay.T_req_br_no,sizeof(hv_notpay.T_req_br_no));
	deal_half_charcter(cContent);
	MEMCPY_DEBUG(fd123.notpay_content,cContent,sizeof(cContent));
	


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

int CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack)
{
 /****************查复报文*********************************/
	 char cTmpBuf[19];
	 memset(cTmpBuf,0x00,sizeof(cTmpBuf));

	 pack->opeType='2';/*其他*/
	 MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);
	 /*货币种类*/
	 del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***交易金额**/
	 MEMCPY_DEBUG(pack->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));
	 MEMCPY_DEBUG(pack->CMTCode,"302",3); /*CMT编号*/
	 MEMCPY_DEBUG(pack->workDate,hv_notpay->T_tx_date,8);/*交易日期*/
	 iHVSETTAG( pack,"CJ1",hv_notpay->T_res_date,sizeof(hv_notpay->T_wt_date));
	/*查复日期*/
	 iHVSETTAG(pack,"01D",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));/*查复行行号*/
	 iHVSETTAG(pack,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));/*查复清算行行号*/

	iHVSETTAG(pack,"043",hv_notpay->T_res_orderno,sizeof(hv_notpay->T_res_orderno));                     /*查复书号*/
	iHVSETTAG(pack,"CCB",hv_notpay->T_req_br_no,sizeof(hv_notpay->T_req_br_no));           /*原查询行行号*/
	iHVSETTAG(pack,"CCC",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));         /*原查询清算行号*/
	iHVSETTAG(pack,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));             /*原查询书号*/
	iHVSETTAG(pack,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));           /*原查询日期*/
	iHVSETTAG(pack,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));         /*原委托日期*/
	iHVSETTAG(pack,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));               /*原发起行行号*/
	iHVSETTAG(pack,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));               /*原接受行号*/
   	iHVSETTAG(pack,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));         /*原交易种类*/

	 iHVSETTAG(pack,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));     /*原支付交易序号*/
        iHVSETTAG(pack,"CND",cTmpBuf,sizeof(cTmpBuf));
                        /*原货币符号,金额*/
     	iHVSETTAG(pack,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));                       /*发报中心代码*/
    	iHVSETTAG(pack,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco)) ;                       /*收报中心代码*/
     	iHVSETTAG(pack,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));             /*查复内容*/

        /***********没有附加域***************/
        return 0;
}

int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						cBuf[j] = buf[i];
						j++;
				}
		}

		MEMCPY_DEBUG(buf,cBuf,sizeof(cBuf));
		return 0;
}
