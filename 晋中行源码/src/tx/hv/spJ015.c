/***************************************************************
* 文 件 名:     spJ015.c
* 功能描述：    银行汇票申请注销		
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18  17:27
* 描述:更新汇票的状态,同时在非支付表中组合一条自由格式,并发送出去
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_fzf_c.h"
#include "hvnet.h"
#include "hv_pub.h"
#include "hv_fzf_c.h"
#include "find_debug.h"

/**********************************
*挂失日期放到原委托日期字段       *
*汇票号码 用文件传递		  *
*在这里调用一个短通讯             *
***********************************/

spJ015()
{
	int ret = 0;
	HV_NOTPAY_AREA     hv_notpayin;
	HV_FD123_AREA      fd123;
	struct hv_poinfo_c hv_poinfo;
	struct hv_fzf_c    hv_fzf;
	char cPwd	[11]     ;/*****汇票密押****/
	THVPACKAGE pack303;


	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&fd123,        0x00 ,  sizeof(fd123));
	memset(&hv_poinfo,    0x00 ,  sizeof(hv_poinfo));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(cPwd     ,     0x00 ,  sizeof(cPwd));
	memset(&pack303,      0x00 ,  sizeof(pack303));


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
	vtcp_log("_________O_txamt==[%16s]\n",hv_notpayin.F_o_txamt);


	get_zd_data("0880",cPwd);/***获取密押*****/
	get_fd_data("1230",(char *)&fd123);/**获取123域****/

	/**********************
         *PART 2   基本检测   *
	 **********************/
		

	/*********************
	 *PART 3  操作数据库 *
         *********************/
	
	/******获取主键********************/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	
	ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s'  ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	if(ret)
	{
	sprintf(acErrMsg,"执行函数hv_poinfo_dec_upd错误!ret==[%d]\n",ret);
	WRITEMSG
	goto ErrExit;	
	}
	ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"执行函数Hv_poinfo_Fet_Upd错误！ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P001");
	goto ErrExit;	
	}

	/****判断一下汇票的状态*******************/
	if(hv_poinfo.po_sts[0]!=HV_HP_ASKCASH)
	{
	sprintf(acErrMsg,"该汇票没有兑付[%c]\n",hv_poinfo.po_sts[0]);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P004");
	goto ErrExit;
	}
	if(hv_poinfo.po_sts[0]=='5')
	{
	sprintf(acErrMsg,"该汇票已经清算[%c]\n",hv_poinfo.po_sts[0]);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P004");
	goto ErrExit;
	}
	if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	{	
	sprintf(acErrMsg,"汇票密押错误![");
	WRITEMSG
	strcpy(g_pub_tx.reply,"P005");
	goto ErrExit;
	}
	double money = 0.0;
	dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&money);
	if(hv_poinfo.po_bal!=money)
	{
	sprintf(acErrMsg,"输入的金额不符![%lf][%16s][%lf]\n",hv_poinfo.po_bal,hv_notpayin.F_o_txamt,money);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P008");
	goto ErrExit;
	}
		
	/****挂失日期写入到表的上次交易日期吧???????***/
	hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_wt_date,sizeof(hv_notpayin.F_wt_date));
	hv_poinfo.po_sts[0] = 'C';/***申请兑付取消状态***/

	/******现在开始更新那些字段吧*************************/		
	ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P009");
	goto ErrExit;	
	}
	/************向非支付交易表中插入一条记录**************/

	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
	sprintf(acErrMsg,"执行函数F->T失败!ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P012");
	goto ErrExit;
	}


	ret = iHvNotPayin_Copy(&hv_notpayin,&hv_fzf);/***从T字段到hv_fzf结构体中****/
	if(ret)
	{
	sprintf(acErrMsg,"拷贝错误ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P012");
	goto ErrExit;
	}


	MEMCPY_DEBUG(hv_fzf.receco,HP_CENTER,sizeof(hv_fzf.receco)-1); /*****这里需要强制添上上海汇票中心代码**************/
	sprintf(hv_notpayin.F_content,"汇票中心收，你好，这是从 行号代码为%.12s发出一个汇票兑付取消申请报文,汇票号码%20s，汇票金额%16.2f，签发行%12s， 请您查收!\n",hv_notpayin.F_or_br_no,hv_poinfo.po_no,hv_poinfo.po_bal,hv_poinfo.pay_br_no);
	memset(hv_fzf.cash_qs_no,0x00,sizeof(hv_fzf.cash_qs_no)-1);


	ret = Hv_fzf_Ins(hv_fzf,g_pub_tx.reply);
	if(ret)
	{
	sprintf(acErrMsg,"插入非支付交易表错误!ret=[%d]\n",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P010");
	goto ErrExit;
	}
	vtcp_log("INSERT END!!!!!!\n");
	vtcp_log("INSERT END!!!!!!\n");
	vtcp_log("INSERT END!!!!!!\n");





	/***这里定义几个附加的内容****/
	MEMCPY_DEBUG(hv_notpayin.T_receco,HP_CENTER,sizeof(hv_fzf.receco)-1);
	MEMCPY_DEBUG(hv_notpayin.T_req_content,hv_notpayin.F_content,sizeof(hv_notpayin.T_req_content));
	memset(hv_notpayin.T_ac_br_no,0x00, sizeof(hv_notpayin.T_ac_br_no));
	memset(hv_notpayin.T_cash_qs_no,0x00,sizeof(hv_notpayin.T_cash_qs_no));

	vtcp_log("T_req_content==[%s]\n",hv_notpayin.T_req_content);


	print_notpay(&hv_notpayin,'T');
	/*****插入大额非支付历史明细表***********/
	vtcp_log("___________INSERT HTR_______[%d]\n",__LINE__);
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
	sprintf(acErrMsg,"插入非支付明细表错误!ret=[%d]",ret);
	WRITEMSG
	strcpy(g_pub_tx.reply,"P011");
	goto ErrExit;
	}

	
		
	 

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
	vtcp_log("T_req_content==[%s]\n",hv_notpayin.T_req_content);
	 pub_com_CMT303_T2TAG(&hv_notpayin,&pack303);
	 ret=hv_gen_pack(&pack303);
	 if(ret<0){
	 sprintf(acErrMsg,"组织大额支付报文错误!\n");
	 WRITEMSG
	 strcpy( g_pub_tx.reply, "P021" );
	 goto ErrExit;
	 }
	 if(hv_insert_sendlist(&pack303,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
	 ,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
	 sprintf(acErrMsg,"插入发送登记簿失败!\n");
	 WRITEMSG
	 strcpy( g_pub_tx.reply, "P010" );
	 goto ErrExit;
	 }
	 if(hv_sendpack(&pack303)){
	 sprintf(acErrMsg,"发送303报文到前置机失败!\n");
	 strcpy( g_pub_tx.reply, "LS88" );
                WRITEMSG
                goto ErrExit;
        }
        /*************************************/


OkExit:
	strcpy(g_pub_tx.reply,"0000");
	set_zd_data("0280",hv_notpayin.T_orderno);/****返回一个序号就足够了*/
	set_zd_data("0440",hv_notpayin.T_wt_date);/****送委托日期过去******/
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
