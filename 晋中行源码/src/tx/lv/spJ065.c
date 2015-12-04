/***************************************************************
* 文 件 名:   spJ065.c
* 功能描述：  小额业务查复报文录入
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


/****注意了  对于查复就没有必要取找到原查询来更新状态了******/

spJ065()
{
	int  ret = 0;
	int  iOrderno=0;
   	char cBrno[6];		/** 交易机构 **/

	NOTPAY_IN_AREA lv_notpay;
	HV_NOTPAY_AREA hv_notpay;
	struct hv_fzf_c hv_fzf;
	struct hv_fzfhtr_c hv_fzfhtr;
	LV_NOTPAY_ADD lv_addinfo;
	HV_FD123_AREA fd123;

	memset(cBrno ,0 ,sizeof(cBrno));
	memset(&lv_notpay,0,sizeof(lv_notpay));
	memset(&hv_notpay,0x00,sizeof(hv_notpay));
	memset(&hv_fzf,	  0x00,sizeof(hv_fzf));
	memset(&hv_fzfhtr,0x00,sizeof(hv_fzfhtr));
	memset(&fd123,	  0x00,sizeof(fd123));

	vtcp_log("[%s][%d]spJ065小额业务查复报文录入BEGIN",__FILE__,__LINE__);


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
	ret  = iLvTistoNotpay(&hv_notpay);
	if(ret )
	{
		sprintf( acErrMsg,"调用iHvTistoNotpay错误![%s][%d]",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P053");
		WRITEMSG
		goto ErrExit;
	}


	/**********************
	 * 3.交易基本检查     *
 	 **********************/


	/* 检查了原记录是否存在; 状态是否正确; 对hv_fzf进行了更新**/
	 ret = lv_check_notpay_record(&hv_notpay,&hv_fzf,"302");
	 if(ret)
	 {
		vtcp_log("[%s][%d]对原非支付交易记录的处理失败\n",__FILE__,__LINE__);
		/***别对g_pub_tx.reply赋值*****/
		goto ErrExit;
	 }	

	/**********************
	 * 4. 数据库操作      *
 	 **********************/

        vtcp_log("[%s][%d]开始插入hv_fzfhtr\n",__FILE__,__LINE__);

        iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);/**hv_fzf ->hv_fzfhtr的拷贝***/

	get_zd_long("0060",&hv_fzfhtr.tx_time);/* 交易时间*/
        get_zd_long("0040",&hv_fzfhtr.trace_no);/* 流水号*/
	
	memcpy(hv_fzfhtr.orderno,hv_fzf.res_orderno,sizeof(hv_fzfhtr.orderno)-1);

        ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
        if(ret)
        {
                sprintf(acErrMsg,"插入数据表hv_fzfhtr错误!ret==[%d]\n",ret);
                WRITEMSG
                strcpy(g_pub_tx.reply,"HV01");
                goto ErrExit;
        }
	
	/**********************
	 * 5. 准备流水        *
 	 **********************/

	ret = pub_ins_trace_log();
	if ( ret )
	{
 		sprintf( acErrMsg, "记流水日志错" );
		WRITEMSG
		goto ErrExit;
	}
	
	


	/**********************
	 * 6.发送业务包       *
	 **********************/
	
	 ret = lv_get_cmt302_tag(&hv_fzf);
	
	 if(ret)
	 {
		vtcp_log("[%s][%d]发送302错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	 }

	 	
	/**********************
	 * 7.   准备返回值    *
 	 **********************/

	vtcp_log("ORDERNO===[%s]\n",hv_notpay.F_orderno);
	memcpy(fd123.orderno,hv_notpay.F_orderno,sizeof(fd123.orderno));
	memcpy(fd123.qr_tx_num,hv_fzf.req_orderno,sizeof(fd123.qr_tx_num));
	vtcp_log("qtyxnum===[%s]\n",hv_fzf.req_orderno);
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
	
