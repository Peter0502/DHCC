/***************************************************************
* 文 件 名:     spJ014.c
* 功能描述：    银行汇票解挂失			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-18  14:21
*
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
#include "find_debug.h"

/**********************************
*挂失日期放到原委托日期字段       *
*汇票号码 用文件传递			  *
***********************************/
typedef struct{
	char po_no[20];		/***汇票号码***/
}STRUCT_HP_AREA;

spJ014()
{
	int ret = 0;
	char *cFilename ;
	FILE *fp;
	HV_PAYIN_AREA hv_payin;
	STRUCT_HP_AREA	hv_hp;
	STRUCT_HP_AREA	hv_hp_tb;
	struct hv_poinfo_c hv_poinfo;
	struct hv_poinfo_c hv_poinfo_tb;/***需要对输入要素进行合法性检测***/

	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(cFilename,  0x00 ,  sizeof(cFilename));
	memset(&hv_hp,	   0x00 ,  sizeof(hv_hp));
	memset(&hv_hp_tb,	   0x00 ,  sizeof(hv_hp_tb));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));
	memset(&hv_poinfo_tb, 0x00 ,  sizeof(hv_poinfo_tb));


	/*****************************
     *PART 1   得到前台数据      *
     *****************************/
	/***得到大额支付业务8583数据***/
	ret=iHvTistoPayin(&hv_payin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
   	/*****得到附加域里面的内容**********/
	MEMCPY_DEBUG(cFilename,"../tmp/",7);
	strcat(cFilename,hv_payin.F_tx_br_no);
	strcat(cFilename,hv_payin.F_tel);
	sprintf(acErrMsg,"附加域对应的文件名称[%s]\n",cFilename);
	WRITEMSG
	pub_base_AllDwnFilName(cFilename);
	fp = fopen(cFilename,"rb");
	if(fp==NULL)
	{
		sprintf(acErrMsg,"附加域对应的文件名称[%s]不存在\n",cFilename);
		WRITEMSG
		goto ErrExit;	
	}
	fget(&hv_hp,sizeof(hv_hp),fp);
	fclose(fp);
	/******前台获取数据读入到hv_poinfo结构中***/
	MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
	MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);
	str2dbl(hv_payin.F_tx_amt,sizeof(hv_payin.F_tx_amt),0,hv_poinfo.po_bal);
	str2dbl(hv_payin.F_o_wt_date,sizeof(hv_payin.F_o_wt_date),0,hv_poinfo.l_tx_date);/**放入到l_tx_date字段中来******/
	/**********************
     *PART 2   基本检测   *
	 **********************/
	
		

	/*********************
	 *PART 3  操作数据库 *
     *********************/
	
	/******获取主键********************/
	MEMCPY_DEBUG(hv_poinfo_tb.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
	MEMCPY_DEBUG(hv_poinfo_tb.pay_br_no,hv_payin.F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

		ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply,"po_no=%s and pay_br_no=%s ",hv_poinfo_tb.po_no,hv_poinfo_tb.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"执行函数hv_poinfo_dec_upd错误!ret==[%d]\n",ret);
			WRITEMSG
			goto ErrExit;	
		}
		ret = Hv_poinfo_Fet_Upd(&hv_poinfo_tb,g_pub_tx.reply);
		if(ret)
		{
			sprintf(acErrMsg,"执行函数Hv_poinfo_Fet_Upd错误！ret=[%d]\n",ret);
			WRITEMSG
			goto ErrExit;	
		}
		
		/************比较一下金额和挂失日期两个字段******************/
		if(hv_poinfo.po_bal!=hv_poinfo_tb.po_bal ||hv_poinfo.l_tx_date!=hv_poinfo_tb.l_tx_date)
		{
			sprintf(acErrMsg,"输入的金额或者挂失日期与数据表不匹配\n");
			WRITEMSG
			goto ErrExit;
		}
		/***如果都匹配了，更新两个字段，加密方式和汇票状态***/
		/****挂失日期写入到表的上次交易日期吧???????***/
		hv_poinfo_tb.po_sts[0]		= 'F';/**挂失****/
		/******现在开始更新那些字段吧*************************/		
		ret = Hv_poinfo_Upd_Upd(&hv_poinfo_tb,g_pub_tx.reply);
		{
			sprint(acErrMsg,"执行函数hv_poinfo_upd_upd错误!ret==[%d]\n",ret);
			WRITEMSG
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
