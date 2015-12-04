/***************************************************************
* 文 件 名:   spJ061.c
* 功能描述：  小额业务冲正申请报文录入
*
* 作    者:   dongxy
* 完成日期：  2006-8-29 
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
#include "lv_define.h"
#include "hvpack.h"
#include "lvpack.h"
#define loc_t sys_loc_t
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"

spJ061()
{
	int  			iOrderno=0;
	int  			iRecordNum = 0;
	int 			ret;
	char 			cBrno[6];		/** 交易机构 **/
	HV_NOTPAY_AREA 		hv_notpay;
	struct lv_pkgreg_c 	lv_pkgreg;
	LV_NOTPAY_ADD  		lv_addinfo;
	HV_FD123_AREA  		fd123;
	struct	 hv_fzf_c 	hv_fzf;
	struct 	 hv_fzfhtr_c 	hv_fzfhtr;


	memset(cBrno,		0x00,	sizeof(cBrno));
	memset(&hv_notpay,	0x00,	sizeof(hv_notpay));
	memset(&lv_pkgreg,	0x00,	sizeof(lv_pkgreg));
	memset(&lv_addinfo,	0x00,	sizeof(lv_addinfo));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));
	memset(&hv_fzfhtr,	0x00,	sizeof(hv_fzfhtr));



	vtcp_log("[%s][%d]spJ061小额业务冲正申请报文录入BEGIN",__FILE__,__LINE__);



	/************************
	* 1. 初始化全局变量    *
	************************/

	pub_base_sysinit(); 

	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}




	/************************
	* 2.获取8583域内容     *
	************************/
	ret=iLvTistoNotpay(&hv_notpay);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}



	/*************************
	* 3. 执行交易基本检测   *
	*************************/
	ret = iLvFzFCommonCheck(&hv_notpay);
	if(ret)
	{
		vtcp_log("[%s][%d]执行交易基本检测错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	} 

	/****检测原支付交易*****/

	ret = lv_check_payin_record(&hv_notpay,&lv_addinfo,"323",'1');
	if(ret)
	{
		vtcp_log("[%s][%d]原支付交易存在问题!\n",__FILE__,__LINE__);
		goto ErrExit;
	}


	/*****************
	* 4.数据库操作  *
	*****************/
	ret = lv_Ins_Notpay_record(&hv_notpay,&hv_fzf,&lv_addinfo);
	if(ret)
	{
		 vtcp_log("[%s][%d]插入hv_fzf/hv_fzfhtr错误!\n",__FILE__,__LINE__);
		 goto ErrExit;
	}



	/*****************
	* 5.准备流水    *
	*****************/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}



	/****************
	 * 6.业务包发送 *
	 ****************/

	 ret = lv_get_cmt323_tag(&hv_fzf);
	 if(ret)
	 {
		vtcp_log("[%s][%d]发送业务包错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	 }


	/********************
	* 7. 提取返回值    *
	********************/
	memcpy(fd123.orderno,hv_fzf.req_orderno,sizeof(fd123.orderno));
	memcpy(fd123.or_br_no,hv_notpay.F_or_br_no,sizeof(fd123.or_br_no));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
	set_zd_data("1230",(char *)&fd123);
	vtcp_log("[%s]\n",(char *)&fd123);


OkExit:
        strcpy(g_pub_tx.reply,"0000");
        sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
        WRITEMSG
        set_zd_data(DC_REPLY,g_pub_tx.reply);
        return 0;

ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]\n",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
