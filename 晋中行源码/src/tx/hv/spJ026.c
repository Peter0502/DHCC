/***************************************************************
* 文 件 名:     spJ026.c
* 功能描述：    银行汇票查询报文录入
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18  17:27
* 描述:更新汇票的状态,同时在非支付表中组合一条查询报文,并发送出去
* 修改记录： 修改票据号为从59域取值,支持16位票据号
* 日    期:  2011年04月15日
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

spJ026()
{
	int ret = 0;
	HV_NOTPAY_AREA     hv_notpayin;
	HV_PAYIN_AREA      hv_payin;
	HV_FD123_AREA      fd123;
	struct hv_fzf_c    hv_fzf;
	char cCD_date[9]         ;/***承兑日期****/
	/****
	char cTmplist[256];       
	char cFrontContent[256];
	****/
	char cTmplist[512];
	char cContent[512+128];       /***组建的查询报文的内容字段**/
        char l_po_no[17];         /** 加长票据号字段，支持16位长票据号 2011-4-15 **/
	char cFrontContent[101];
	char fmt[10];
	char cShowmsg[200];

	THVPACKAGE  pack301;


	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&hv_payin,     0x00 ,  sizeof(hv_payin));
	memset(&fd123,        0x00 ,  sizeof(fd123));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(cCD_date,      0x00 ,  sizeof(cCD_date));
	memset(cContent,      0x00 ,  sizeof(cContent)); 
	memset(&pack301,      0x00 ,  sizeof(pack301));
        memset(l_po_no ,      0x00 ,  sizeof(l_po_no));           /** 支持16位长票据号 2011-4-15 **/
	memset(cTmplist,	  0x00 ,  sizeof(cTmplist));
	memset(cFrontContent, 0x00 ,  sizeof(cFrontContent));
	memset(fmt,0x00,sizeof(fmt));	
	memset(cShowmsg,0x00,sizeof(cShowmsg));


    /*****************************
     *PART 1   得到前台数据      *
     *****************************/
	/***得到大额支付业务8583数据***/
	ret=iHvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		vtcp_log("[%s][%d]从起前台获取数据失败了!\n",__FILE__,__LINE__);
		WRITEMSG
		goto ErrExit;
	}
	ret = iHvTistoPayin(&hv_payin);
	if( ret )
	{
		vtcp_log("[%s][%d]从起前台获取数据失败了!\n",__FILE__,__LINE__);
		
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("_________O_txamt==[%16s]\n",hv_notpayin.F_o_txamt);


	get_fd_data("0470",cCD_date);/***承兑日期****/
	get_fd_data("1230",(char *)&fd123);/**获取123域****/
        get_fd_data("0590",l_po_no);       /** 支持16位长票据号 2011-4-15 **/	

	/**********************************/
	ret = GetOr_br_noByBr_no(g_pub_tx.tx_br_no,hv_notpayin.F_or_br_no);
	if(ret)
	{
		sprintf(acErrMsg,"根据交易机构获取行号错误!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	MEMCPY_DEBUG(hv_notpayin.F_ac_br_no,hv_payin.F_ac_br_no,sizeof(hv_notpayin.F_ac_br_no));
        /** 20131025 给hv_notpayin.F_content加结束符 hzh  start**/
        hv_notpayin.F_content[sizeof(hv_notpayin.F_content)-1]='\0';
        /** 20131025 给hv_notpayin.F_content加结束符 hzh  end**/
        vtcp_log("加长票据号码==[%s]\n",l_po_no);   /** 支持16位长票据号 2011-4-15 **/
	vtcp_log("出票日期==[%s]\n",hv_notpayin.F_o_wt_date);
	vtcp_log("汇票到期日期==[%.60s]\n",hv_notpayin.F_o_fzf_date);
	vtcp_log("出票人的帐号==[%.32s]\n",hv_payin.F_pay_ac_no);
	vtcp_log("出票人的全称==[%.60s]\n",hv_payin.F_pay_name);
	vtcp_log("收款人的帐号==[%.32s]\n",hv_payin.F_cash_ac_no);
	vtcp_log("收款人的全称==[%s]\n",hv_payin.F_cash_name);
	vtcp_log("付款行的全称==[%s]\n",hv_payin.F_pay_addr);
	vtcp_log("查询的序号====[%s]\n",hv_notpayin.F_orderno);
	vtcp_log("查询的日期====[%s]\n",hv_notpayin.F_wt_date);
	vtcp_log("查复的行号====[%s]\n",hv_payin.F_ac_br_no);
	vtcp_log("票据的金额====[%s]\n",hv_notpayin.F_o_txamt);
	vtcp_log("发起行号  ====[%s]\n",hv_notpayin.F_or_br_no);
	vtcp_log("附言  ====[%s]\n",hv_notpayin.F_content);

	vtcp_log("开始拼查询的附言部分[%s][%d]\n",__FILE__,__LINE__);	

/**     改为加长16为票据号 MOD BY YANQingQing 2011-4-15
	sprintf(cTmplist,"汇票号%.8s",fd123.po_no);
**/
        sprintf(cTmplist,"汇票号%.16s",l_po_no);

	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));

	sprintf(cTmplist,"出票日%.8s",hv_notpayin.F_o_wt_date);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	sprintf(cTmplist,"到期日%.8s",hv_notpayin.F_o_fzf_date);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	/*********add by liuyue 20061116  解决发给农行的报文中 超长的 问题  103是农行****/
	if(memcmp(hv_payin.F_ac_br_no,"103",3))
	{
		sprintf(cTmplist,"出票帐号%.32s",hv_payin.F_pay_ac_no);
		strcat(cContent,cTmplist);
		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	}
	
	sprintf(cTmplist,"出票人%.60s",hv_payin.F_pay_name);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));


	/*********add by liuyue 20061116  解决发给农行的报文中 超长的 问题****/
	if(memcmp(hv_payin.F_ac_br_no,"103",3))
	{
		sprintf(cTmplist," 收款帐号%.32s",hv_payin.F_cash_ac_no);
		strcat(cContent,cTmplist);
		memset(cTmplist,0x00,sizeof(cTmplist));
		vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	}

	sprintf(cTmplist,"收款人%.60s",hv_payin.F_cash_name);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));

	sprintf(cTmplist," 付款行%.40s",hv_payin.F_pay_addr);
	strcat(cContent,cTmplist);
	memset(cTmplist,0x00,sizeof(cTmplist));

	vtcp_log("[%s][%d] F_pay_addr==[%s]",__FILE__,__LINE__,hv_payin.F_pay_addr);
	vtcp_log("[%s][%d] CCONTENT==[%s]",__FILE__,__LINE__,cContent);
	vtcp_log("[%s][%d]strlen(cContent)===[%d]\n",__FILE__,__LINE__,strlen(cContent));
	
	vtcp_log("[%s][%d]HV_NOTPAYIN.CONTENT==[%s]\n",__FILE__,__LINE__,hv_notpayin.F_content);
	vtcp_log("[%s][%d]CFRONTCONTENT==[%s]\n",__FILE__,__LINE__,cFrontContent);
	MEMCPY_DEBUG(cFrontContent,hv_notpayin.F_content,sizeof(hv_notpayin.F_content));
	zip_space(cFrontContent);
	sprintf(fmt,"%%%ds",strlen(cFrontContent));
	strcat(cContent,"  ");
	sprintf(cTmplist,fmt,cFrontContent);
	strcat(cContent,cTmplist);

	vtcp_log("拼的附言的内容为:[%s]\n",cContent);

	if(strlen(cContent)>255)
	{
		sprintf(cShowmsg,"查询附言超过255小于 [%d]请裁减付款行名以及附言",strlen(cContent));
		strcpy(g_pub_tx.reply,"HV01");
		vtcp_log("[%s][%d]所拼附言超过255,strlen=[%d]\n",__FILE__,__LINE__,strlen(cContent));
		set_zd_data("0130",cShowmsg);
		goto ErrExit;
	}

	ret = iCharacterType(cContent,strlen(cContent),'g');
	if(ret)
	{
		vtcp_log("[%s][%d]组成的附言中存在非法字符\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"HV01");
		set_zd_data("0130","输入的项中存在半个汉字或者半角标点");
		goto ErrExit;
	}

	deal_half_charcter(cContent);
	vtcp_log("[%s]\n",cContent);




	/**********************
         *PART 2   基本检测   *
	 **********************/
	ret = iHv_checkstat(hv_notpayin.F_cmtno);
	if(ret)
	{
		sprintf(acErrMsg,"工作状态检查错误!ret=[%d]\n",ret);
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;
	}
	if(!memcmp(hv_notpayin.F_or_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_ac_br_no)))
	{
		sprintf(acErrMsg,"查询行和查复行不能是同一个行的!\n");
		set_zd_data("0130",acErrMsg);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P000");
		goto ErrExit;	
	}		

	/*********************
	 *PART 3  操作数据库 *
         *********************/
	hv_notpayin.T_hv_fzf_sts[0] = '1'; /*****改变状态*****/	
	MEMCPY_DEBUG(hv_notpayin.F_content,cContent,sizeof(hv_notpayin.F_content));/****设置附言内容***/
	/***获取收报中心以及接受清算行号****/
	/*
	ret = iGet_full_hv_no(hv_notpayin.F_ac_br_no,hv_notpayin.F_receco,hv_notpayin.F_cash_qs_no);*/
	/***设置原接受行作为对方的接受行*****/
	MEMCPY_DEBUG(hv_notpayin.F_o_ac_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_o_ac_br_no));


	vtcp_log("[%s][%d]cmtno===[%s]\n",__FILE__,__LINE__,hv_notpayin.F_cmtno);	
	ret = iHvfFrontToTable(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"F->T错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P018");
		goto ErrExit;
	}
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	vtcp_log("T_ac_br_no==[%s]\n",hv_notpayin.T_ac_br_no);
	/****需要获取收报中心了****/
	print_notpay(&hv_notpayin,'A');
	ret = iHvUpdFzfRecFromPayin(&hv_notpayin,1);
	if(ret)
	{
		sprintf(acErrMsg,"插入非支付数据表错误!ret=[%d]\n",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/***写历史明细****/
	ret = dg_hv_fzfhtr(&hv_notpayin);
	if(ret)
	{
		sprintf(acErrMsg,"插入非支付历史明细表错误!\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}
	/*****开始直接给发送出去了*****/
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);



	MEMCPY_DEBUG(hv_notpayin.T_req_content,cContent,sizeof(hv_notpayin.T_req_content));
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(hv_notpayin.T_o_or_br_no,hv_notpayin.T_or_br_no,sizeof(hv_notpayin.T_or_br_no));
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	pub_com_CMT301_T2TAG(&hv_notpayin,&pack301);
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	ret=hv_gen_pack(&pack301);
	vtcp_log("开始发报文了![%s][%d]\n",__FILE__,__LINE__);
	if(ret<0){
 		sprintf(acErrMsg,"组织大额支付报文错误!\n");
                WRITEMSG
                strcpy( g_pub_tx.reply, "P021" );
                goto ErrExit;
        }
        if(hv_insert_sendlist(&pack301,hv_notpayin.F_tx_br_no,hv_notpayin.F_tel\
                        ,hv_notpayin.T_trace_no,hv_notpayin.T_orderno)<0){
                sprintf(acErrMsg,"插入发送登记簿失败!\n");
                WRITEMSG
                strcpy( g_pub_tx.reply, "P010" );
                goto ErrExit;
        }

        if(hv_sendpack(&pack301)){
                sprintf(acErrMsg,"发送301报文到前置机失败!\n");
                strcpy( g_pub_tx.reply, "LS88" );
                WRITEMSG
                goto ErrExit;
        }

	vtcp_log("[%s][%d]发送成功了!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]发送成功了!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]发送成功了!\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]发送成功了!\n",__FILE__,__LINE__);

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
	MEMCPY_DEBUG(fd123.orderno,hv_notpayin.T_orderno,sizeof(hv_notpayin.T_orderno));
	MEMCPY_DEBUG(fd123.ac_br_no,hv_notpayin.T_ac_br_no,sizeof(hv_notpayin.T_ac_br_no));
	vtcp_log("[%s]\n",cContent);
	MEMCPY_DEBUG(fd123.notpay_content,cContent,strlen(cContent));
	/*
	deal_half_charcter(cContent,fd123.notpay_content,sizeof(cContent));*/

	

OkExit:
	 pub_base_full_space((char *)&fd123,sizeof(fd123));
     set_zd_data("1230",(char *)&fd123);
	
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


