/***************************************************************
* 文 件 名:     spJ059.c
* 功能描述：    小额退回应答		
*
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "hv_define.h"
#include "hvpack.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "hv_fzfhtr_c.h"

spJ059()
{
	int ret = 0;
	int iOrderno = 0;
	int iRecordNum = 0;
	HV_NOTPAY_AREA hv_notpayin;
        struct	lv_pkgreg_c    lv_pkgreg;
	struct  hv_fzf_c       hv_fzf;
	struct  hv_fzfhtr_c    hv_fzfhtr;
	HV_FD123_AREA          fd123;
	int flag = 1;/**标志位,用来标志原非支付交易记录是否存在,0 不存在 1存在*/

	char cPackid[9]		;
	char cPack_date[9]	;/***因为小额的非支付交易中需要用到这两字段***/

	
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&lv_pkgreg,	      0x00 ,  sizeof(lv_pkgreg));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&hv_fzfhtr,    0x00 ,  sizeof(hv_fzfhtr));
	memset(&fd123,	      0x00 ,  sizeof(fd123));

	memset(cPackid,	      0x00 ,  sizeof(cPackid));
	memset(cPack_date,    0x00 ,  sizeof(cPack_date));



	/***********************
	* 1.  初始化全局变量   *
	************************/

	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}



	 /************************
     	 * 2.  获取8583域内容    *
    	 *************************/


	ret=iLvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	/********************
         * 3.     基本检测  *
	 ********************/

	ret = iLvFzFCommonCheck(&hv_notpayin);
	if(ret)
	{
		vtcp_log("[%s][%d]执行数据基本检测失败!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P054");
		goto ErrExit;
	}

	/******检测原申请类交易是否存在,状态是否正确****/	
	ret = lv_check_notpay_record(&hv_notpayin,&hv_fzf,"320");
	if(ret)
	{
		vtcp_log("检测原非支付交易失败!\n",__FILE__,__LINE__);
		goto ErrExit;
	}

		

	/********************
         * 4.   数据库操作  *
	 ********************/


	vtcp_log("[%s][%d]插入明细表!\n",__FILE__,__LINE__);
	iLvFzfToFzfhtr(&hv_fzf,&hv_fzfhtr);
	get_zd_long("0060",&hv_fzfhtr.tx_time);/* 交易时间*/
	get_zd_long("0040",&hv_fzfhtr.trace_no);/* 流水号*/
	vtcp_log("[%s][%d]!!!!!!!!!!![%ld][%ld]\n",hv_fzfhtr.tx_time,hv_fzfhtr.trace_no);
	ret = Hv_fzfhtr_Ins(hv_fzfhtr,g_pub_tx.reply);
	if(ret)
	{
		sprintf(acErrMsg,"插入hv_fzfhtr错误!\n");
		WRITEMSG
		vtcp_log("ret===[%d]\n",ret);
		strcpy(g_pub_tx.reply,"HV01");
		goto ErrExit;
	}


	/*******************
   	 *PART 5  写流水表 *
     	 *******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}



	/*******************
	 * 6.发送报文      *
	 *******************/

	ret = lv_get_cmt320_tag(&hv_fzf);
	if(ret)
	{
		vtcp_log("[%s][%d]发送320报文错误!ret=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}

	/********************
	 * 7.准备输出数据   *
	 ********************/
	memcpy(fd123.orderno,hv_notpayin.F_orderno,sizeof(fd123.orderno));
	pub_base_full_space((char *)&fd123,sizeof(fd123));
        set_zd_data("1230",(char *)&fd123);

	

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

