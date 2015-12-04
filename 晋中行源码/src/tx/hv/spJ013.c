/***************************************************************
* 文 件 名:     spJ013.c
* 功能描述：    银行汇票挂失/解挂 			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18  14:21
*
* 修改记录：  
* 日    期:   20060824
* 修 改 人:   liuyue
* 修改内容: 合并挂失和解挂
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hvpack.h"
#include "hv_define.h"
#include "hv_poinfo_c.h"
#include "hv_brno_orno_c.h"
#include "find_debug.h"


spJ013()
{
	int ret = 0;
	HV_NOTPAY_AREA hv_notpayin;
	struct hv_poinfo_c hv_poinfo;
	struct hv_brno_orno_c hv_brno_orno;
	HV_FD123_AREA fd123;

	char cPwd[11]     ;   /***从$FD88字段中获取密押************/
	char cBank_type[2];   /***从$FD72字段中获取行别****/
	char cOper_type[2];   /***从$FD71字段中获取操作标志 0-挂失 1-解挂失**/
	int  iExsit;/*****0 他行的那个汇票存在，1他行的的那个汇票不存在***/
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_notpayin,  0x00,      sizeof(hv_notpayin));
	memset(&hv_poinfo,    0x00,      sizeof(hv_poinfo));
	memset(&fd123,        0x00,      sizeof(fd123));
	iExsit = 0;


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
        /******从标签中获取几个标志*****/	
	get_zd_data("0720",cBank_type);
	get_zd_data("0710",cOper_type);	
	get_zd_data("0880",cPwd);
	get_fd_data("1230",(char *)&fd123);
	
	if(cBank_type[0]=='0')/***是本行****/
	 {
	    vtcp_log("[%s][%d]根据交易机构号查找交易行号!\n",__FILE__,__LINE__);
	    ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s'  ",g_pub_tx.tx_br_no);
	    if(ret) {
		 sprintf(acErrMsg,"根据机构号找交易机构错误!\n");
		 WRITEMSG
		 strcpy(g_pub_tx.reply,"D103");
		 goto ErrExit;
	    }
	 MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_brno_orno.or_br_no,sizeof(hv_poinfo.pay_br_no)-1);

        } else {
	  MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	}
	/******获取汇票号码*****/
	MEMCPY_DEBUG(hv_poinfo.po_no,fd123.po_no,sizeof(fd123.po_no));
	
		


	/**********************
         *PART 2   基本检测   *
	 **********************/
	vtcp_log("++banktype[%c]+oper[%c]++\n",cBank_type[0],cOper_type[0]);
	if(cBank_type[0]=='0')/*****本行签发的汇票的挂失和解挂失*****/
	{
		vtcp_log("+++++++开始进入本行的挂失和解挂+++++++\n");
		ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s' ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"执行函数hv_poinfo_Dec_Upd错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);
		if(ret==100)
		{
			sprintf(acErrMsg,"本行不存在该汇票!\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if(ret!=100 &&ret!=0)
		{
			sprintf(acErrMsg,"取查询汇票信息的游标错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		/*********开始比较输入的数据和数据表中的数据**********/
		if(memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
		{
			sprintf(acErrMsg,"输入的密押不符合!/n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P005");
			goto ErrExit;
		}
		if(cOper_type[0]=='0')/*****如果是挂失*******/
		{
		vtcp_log("++++++++开始进入本行的挂失程序!\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER)/*只有移存状态可以挂失*/	
		{
		sprintf(acErrMsg,"该汇票的状态不是移存状态,[%c] 不允许挂失\n",hv_poinfo.po_sts[0]);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
		}
		/*********开始更新数据*****************/
		hv_poinfo.l_po_sts[0]= hv_poinfo.po_sts[0];
		hv_poinfo.po_sts[0]  = HV_HP_LOSS ;  /**挂失****/
		if(hv_poinfo.l_tx_date== apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		else
			hv_poinfo.td_cnt     =1;     /**当日交易笔数+1**/
		hv_poinfo.lost_cnt   = 1;           /**挂失笔数+1*****/
		hv_poinfo.l_tx_date  = 
		  apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
			
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"更新汇票基本信息表错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	   }/*****本行挂失这个if到这里结束了******/
	else if(cOper_type[0]=='1')/*********如果是解挂失******/
	{
		vtcp_log("+++++++开始进入本行的解挂失程序!\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_LOSS)
		{
			sprintf(acErrMsg,"该汇票并没有挂失,无法解挂失!\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P004");
			goto ErrExit;
		}
		/**********开始更新必要的字段*********/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;
		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.po_sts[0]	= hv_poinfo.l_po_sts[0];/**挂失****/
		vtcp_log("++++++++++++++++++++++++++++++++++\n");
		vtcp_log("+++++[%ld]++++++\n",hv_poinfo.l_tx_date);
		vtcp_log("+++++[%c]+++++++\n",hv_poinfo.po_sts[0]);
		vtcp_log("+++++[%d]+++++++\n",hv_poinfo.td_cnt);
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"更新汇票基本信息表错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	      }/*****本行的解挂到这里结束了******/
 	 }/***********本行的处理就到这里结束了*******/
	 else if(cBank_type[0]=='1')/*****如果是他行汇票*************/
 	 {
		
           /****如果是他行的是不允许你输入本行的机构号的*****/
	   if(!memcmp(hv_poinfo.pay_br_no,hv_notpayin.F_or_br_no,sizeof(hv_poinfo.pay_br_no)-1))
	   {
		sprintf(acErrMsg,"处理他行的, 却输入了本行的机构号[%s][%s]\n",hv_poinfo.pay_br_no,hv_notpayin.F_or_br_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P007");
		goto ErrExit;
	   }
	   
	   vtcp_log("++++开始进入他行的挂失与解挂失程序!+++++++++\n");
	   ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no='%s' and pay_br_no='%s'  and bank_type='1'  ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
	   if(ret)
	   {
		sprintf(acErrMsg,"数据库错误!\n");
		WRITEMSG
		goto ErrExit;
	   }
	   ret = Hv_poinfo_Fet_Upd(&hv_poinfo,g_pub_tx.reply);/**找到了记录**/

           if(ret!=100 &&ret)/****是数据库调用错误***/
	   {
	       sprintf(acErrMsg,"执行函数hv_poinfo_dec_upd错误!ret=[%d]\n",ret);
	       WRITEMSG
	       goto ErrExit;
  	   }
	   else if(ret==100) /****记录不存在的情况*****/
           {
	     vtcp_log("++++++不存在他行的这条记录,需要插入!+++++++\n");
	     /*******设置一个标志来说明记录不存在,用来避开下面的那些检测条件**/
	     iExsit = 1; 

	     hv_poinfo.reg_date = apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date));/***插入reg_date********/

	     dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&hv_poinfo.po_bal);

	     MEMCPY_DEBUG(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1);

	     hv_poinfo.bank_type[0] = '1';

	     hv_poinfo.po_sts[0] = HV_HP_LOSS;
	     hv_poinfo.l_po_sts[0] = '0';
	      /*********开始插入记录************/
	      ret = Hv_poinfo_Ins(hv_poinfo,g_pub_tx.reply);TRACE
	      if(ret)
	      {
	  	    sprintf(acErrMsg,"插入汇票基本信息表错误!ret=[%d]\n",ret);
		    WRITEMSG
		    goto ErrExit;
	      }
	      vtcp_log("______________成功插入一笔他行的记录________\n");
	    }/**else****他行并且不存在该记录的情况****/
	 /***********对他行汇票的信息查询,出错的处理完毕!****/ 
	 /*********开始比较输入的数据和数据表中的数据**********/
	vtcp_log("___________iExsit==[%d]____________\n",iExsit);
	if(!iExsit&&memcmp(hv_poinfo.pwd,cPwd,sizeof(hv_poinfo.pwd)-1))
	 {
	  	 sprintf(acErrMsg,"输入的密押不符合!\n");
	   	 WRITEMSG
		 strcpy(g_pub_tx.reply,"P005");
	   	 goto ErrExit;
         }
	  	
	 if(!iExsit&&strlen(hv_notpayin.F_o_wt_date)!=0&&apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date))!=hv_poinfo.reg_date)
	 {
		sprintf(acErrMsg,"输入的签票日期不符合![%d][%d]\n",apatoi(hv_notpayin.F_o_wt_date,sizeof(hv_notpayin.F_o_wt_date)),hv_poinfo.reg_date);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P006");
		goto ErrExit;
	 }
	 double money = 0.0;
	 vtcp_log("+++++F_o_Txamt==[%s]\n",hv_notpayin.F_o_txamt);
	dot_str2dbl(hv_notpayin.F_o_txamt,sizeof(hv_notpayin.F_o_txamt),&money);	
	 vtcp_log("++++++++++pobal==[%lf]\n",hv_poinfo.po_bal);
         if(!iExsit&&money!=hv_poinfo.po_bal &&hv_poinfo.po_bal!=0.0)
 	 {
		sprintf(acErrMsg,"输入的出票金额不相符[%lf][%lf]!\n",hv_poinfo.po_bal,money);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P008");
		goto ErrExit;	
	 }
	 if(!iExsit&&cOper_type[0]=='0')/****如果是他行挂失*******/
	 {
		vtcp_log("_____________进入他行的挂失程序____________\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_MV_CENTER)/****只有移存状态可以挂失*/	
		{
		sprintf(acErrMsg,"该汇票的状态不是移存状态, 不允许挂失\n");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P004");
		goto ErrExit;
		}
		/*********开始更新数据*****************/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;
		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.l_po_sts[0] = hv_poinfo.po_sts[0];
		hv_poinfo.po_sts[0]	= HV_HP_LOSS;/**挂失****/
		hv_poinfo.lost_cnt     += 1;           /**挂失笔数+1*****/
			
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"更新汇票基本信息表错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
	   }/*****他行挂失这个if到这里结束了******/
	else if(cOper_type[0]=='1')/*********如果是解挂失******/
	{
		vtcp_log("___________进入他行的解挂失程序_______________\n");
		if(hv_poinfo.po_sts[0]!=HV_HP_LOSS)
		{
			sprintf(acErrMsg,"该汇票并没有挂失,无法解挂失!\n");
			WRITEMSG
			strcpy(g_pub_tx.reply,"P004");
			goto ErrExit;
		}
		/**********开始更新必要的字段*********/
		if(hv_poinfo.l_tx_date==apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date)))
		{
			hv_poinfo.td_cnt = hv_poinfo.td_cnt+1;
		}
		else
			hv_poinfo.td_cnt = 1;

		hv_poinfo.l_tx_date = apatoi(hv_notpayin.F_tx_date,sizeof(hv_notpayin.F_tx_date));
		hv_poinfo.po_sts[0]	= hv_poinfo.l_po_sts[0];/**解开挂失****/
		ret = Hv_poinfo_Upd_Upd(hv_poinfo,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"更新汇票基本信息表错误!ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;
		}
		Hv_poinfo_Clo_Upd();	     
		
         }
     }/****他行的情况处理完毕********/
	
	
		
			
	
		

     /*********************
	 *PART 3  操作数据库 *
     *********************/
	

	/*******************
     *PART 4  结尾工作 *
     *******************/	
	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	/*** 输出变量 ***/

OkExit:
	strcpy(g_pub_tx.reply,"0000");
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
