/***************************************************************
* 文 件 名:   spJ066.c
* 功能描述：  小额业务自由格式报文录入
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
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#define loc_t sys_loc_t

spJ066()
{
	int  	iOrderno = 0;
	int  	ret = 0;
	char 	cSendco[5];	
   	char cBrno[6];		/** 交易机构 **/
	char cAc_br_no[13];	/**接受行行号**/
	HV_NOTPAY_AREA  	hv_notpay;
	struct hv_fzf_c 	hv_fzf;
	struct hv_fzfhtr_c 	hv_fzfhtr;
	HV_FD123_AREA		fd123;
	LV_NOTPAY_ADD		lv_addinfo;

	memset(cBrno,		0x00,	sizeof(cBrno));
	memset(cSendco,		0x00,	sizeof(cSendco));
	memset(cAc_br_no,	0x00, 	sizeof(cAc_br_no));
	memset(&hv_notpay,	0x00,	sizeof(hv_notpay));
	memset(&hv_fzf,		0x00,	sizeof(hv_fzf));
	memset(&hv_fzfhtr,	0x00,	sizeof(hv_fzfhtr));
	memset(&fd123,		0x00,	sizeof(fd123));
	memset(&lv_addinfo,	0x00,	sizeof(lv_addinfo));

	vtcp_log("[%s][%d]spJ066小额业务自由格式报文录入BEGIN",__FILE__,__LINE__);



        /*************************
	 * 1.  初始化全局变量 	 *
       	 *************************/  
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}

       /*************************
	* 2. 提取8583域的内容   *
        *************************/	
	ret = iLvTistoNotpay(&hv_notpay);
	if( ret )
	{
		sprintf( acErrMsg,"调用front_to_notpayin错误![%s][%d]",__FILE__,__LINE__);
		WRITEMSG
		strcpy(g_pub_tx.reply,"P053");
		goto ErrExit;
	}

	/*************************
  	 * 3.基本的数据检测      *
 	 *************************/
	ret = iLvFzFCommonCheck(&hv_notpay);
	if(ret)
	{
		sprintf(acErrMsg,"执行函数基本检测错误!\n");
		WRITEMSG
		goto ErrExit;
	}

	/**************************
	 * 4.数据库操作           *
	 **************************/
	
	ret = lv_Ins_Notpay_record(&hv_notpay,&hv_fzf,&lv_addinfo);

	if(ret)
	{
		vtcp_log("[%s][%d]!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	vtcp_log("[%s][%d]该笔自由格式是发送给接受行的[%s]\n",__FILE__,__LINE__,cAc_br_no);

	/**************************
	 * 5. 流水处理            *
	 **************************/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	/************************
	 * 6. 发送业务包        *
	 ************************/


	vtcp_log("[%s][%d]开始发送业务包!\n",__FILE__,__LINE__);
	ret = lv_get_cmt303_tag(hv_notpay.F_wt_date,hv_fzf.orderno);
	if(ret)
	{
		vtcp_log("[%s][%d]发送失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}
	
	vtcp_log("[%s][%d]发送成功!\n",__FILE__,__LINE__);
	/**************************
	 * 7.  准备输出           *
	 **************************/
        memcpy(fd123.orderno,hv_notpay.F_orderno,sizeof(fd123.orderno));
	memcpy(fd123.ac_br_no,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
	memcpy(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));
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
	
