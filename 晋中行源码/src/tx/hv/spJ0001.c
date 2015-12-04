/***************************************************************
* 文 件 名:     spJ0001.c
* 功能描述：    银行汇票信息查询			
*
* 作    者:   LiuYue
* 完成日期：  2006-8-17 16:51
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

typedef struct{
	char po_ind[1];		/***汇票类型***/
	char deal_mode[1];	/***转入类型***/
	char po_no[16];		/***汇票号码***/
}struct_hp;
	

spJ0001()
{
	int ret = 0;
	char *cFilename ;
	FILE *fp;
	HV_PAYIN_AREA hv_payin;
	struct_hp	hv_hp;
	struct hv_poinfo_c hv_poinfo;
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();
	memset(&hv_payin,  0x00 ,  sizeof(hv_payin));
	memset(cFilename,  0x00 ,  sizeof(cFilename));
	memset(&hv_hp,	   0x00 ,  sizeof(hv_hp));
	memset(&hv_poinfo, 0x00 ,  sizeof(hv_poinfo));

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
		return (-1);
	}
	fget(&hv_hp,sizeof(hv_hp),fp);
	fclose(fp);


    /************************************************************************	
     *1.到这里得到了附加数据域里面的内容									*
	 *2.因为汇票信息的录入进入专门的汇票基本信息表							*
	 *3.开始对汇票基本信息结构体进行赋值								    *	
     ************************************************************************/
	
		/******获取主键********************/
		MEMCPY_DEBUG(hv_poinfo.po_no,hv_hp.po_no,sizeof(hv_poinfo.po_no)-1);
		MEMCPY_DEBUG(hv_poinfo.pay_br_no,hv_payin->F_pay_opn_br_no,sizeof(hv_poinfo.pay_br_no)-1);

		ret = Hv_poinfo_Sel(g_pub_tx.reply,&hv_poinfo,"po_no=%s and pay_br_no=%s ",hv_poinfo.po_no,hv_poinfo.pay_br_no);
		if(ret)
		{
			sprintf(acErrMsg,"查找汇票信息表错误号码＝[%s] 行号==[%s]ret=[%d]\n",hv_poinfo.po_no,hv_poinfo.pay_br_no);
			WRITEMSG
			return (-1);
		}
	 
	/* 写入业务流水 */
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	else 
		goto OkExit;
	/*** 输出变量 ***/

OkExit:
	set_zd_data("1235",hv_poinfo.pay_ac_no);
	set_zd_data("1236",hv_poinfo.pay_name);
	set_zd_data("1237",hv_poinfo.pay_addr);
	set_zd_data("0390",hv_poinfo.po_bal);
	set_zd_data("123D",hv_poinfo.cash_ac_no);
	set_zd_data("123E",hv_poinfo.cash_name);
	set_zd_data("123C",hv_ppinfo.cash_br_no);
	set_zd_data("123F",hv_poinfo.cash_br_name);/***放到收款人地址中传***/
	set_zd_data("123H",hv_poinfo.brf);
	set_zd_data("0700",hv_poinfo.po_ind);/**汇票种类**/
	set_zd_data("0710",hv_poinfo.deal_mode);/**转入种类**/
	
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	if(memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		MEMCPY_DEBUG(g_pub_tx.reply,"T062",4);
	}
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
