/***************************************************************
* 文 件 名:   spJ060.c
* 功能描述：  小额业务撤销申请报文录入
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

spJ060()
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



	vtcp_log("[%s][%d]spJ060小额业务撤销申请报文录入BEGIN",__FILE__,__LINE__);



	/**********************
	 * 1. 初始化全局变量  *
	 **********************/

	pub_base_sysinit(); 
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}




	/**********************
	 * 2. 提取8583域内容  * 
	 **********************/	
        ret=iLvTistoNotpay(&hv_notpay);
        if( ret )
        {
                WRITEMSG
                goto ErrExit;
        }


	/*********************
	 * 3.执行基本的检查  *
	 *********************/		
	
	/** 撤销只允许清算中心做 **/
	ret = pub_base_StrCmp(hv_notpay.F_tx_br_no,LV_CZ_QSBRNO);
       	if( ret )
       	{
               	sprintf( acErrMsg,"此交易只能由清算中心来完成![%s][%d]",__FILE__,__LINE__);
               	WRITEMSG
		strcpy( g_pub_tx.reply, "S043" );
		/****Add by SSH,2006.10.11,增加错误描述信息****/
		set_zd_data(DC_GET_MSG,"此交易只能由清算中心来完成");
               	goto ErrExit;
       	}

	ret = iLvFzFCommonCheck(&hv_notpay);
	if(ret)
	{
		vtcp_log("[%s][%d]!执行交易基本检查错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	}

	vtcp_log("[%s][%d]检测原支付交易!\n",__FILE__,__LINE__);
	ret = lv_check_payin_record(&hv_notpay,&lv_addinfo,"321",'1');
	if(ret)
	{
		vtcp_log("[%s][%d]检测原支付交易失败!\n",__FILE__,__LINE__);
		goto ErrExit;
	}


	/**************************
	 * 4.数据库操作 	  *
	 **************************/ 
	
	 ret = lv_Ins_Notpay_record(&hv_notpay,&hv_fzf,&lv_addinfo);
	 if(ret)
	 {
		vtcp_log("[%s][%d]插入hv_fzf/hv_fzfhtr错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	 }	

	

	 /***********************
	  * 5. 准备写流水表     *
	  ***********************/
       	 ret = pub_ins_trace_log();
         if ( ret )
         {
                sprintf( acErrMsg, "记流水日志错" );
                WRITEMSG
                goto ErrExit;
         }

	
	/****************
	 * 6.发送报文   *
	 ****************/

	ret = lv_get_cmt321_tag(&hv_fzf);

	if(ret)
	{
		vtcp_log("[%s][%d]发送321错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}


	 /***********************
	  * 7. 准备返回值       *
  	  ***********************/	
	
	memcpy(fd123.orderno,hv_fzf.orderno,sizeof(fd123.orderno));
	memcpy(fd123.or_br_no,hv_fzf.or_br_no,sizeof(fd123.or_br_no));
        pub_base_full_space((char *)&fd123,sizeof(fd123));
        set_zd_data("1230",(char *)&fd123);

OkExit:
        strcpy(g_pub_tx.reply,"0000");
        vtcp_log("[%s]\n",(char *)&fd123);
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
