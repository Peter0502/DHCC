/***************************************************************
* 文 件 名:     spJ058.c
* 功能描述：    小额申请退回录入		
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
#include "lv_define.h"
#include "hvpack.h"
#include "lv_pkgreg_c.h"
#include "hv_fzfhtr_c.h"

spJ058()
{
	int ret = 0;
	int iOrderno = 0;
	HV_NOTPAY_AREA hv_notpayin;
	LV_NOTPAY_ADD   lv_addinfo;/****由于采用了大额的123域导致了几个数据的无法处理,设计此结构体来处理它们**/
        struct	lv_pkgreg_c    lv_pkgreg;
	struct  hv_fzf_c       hv_fzf;
	struct  hv_fzfhtr_c    hv_fzfhtr;
	HV_FD123_AREA          fd123 ;
	char cReq_type[2]            ;
	char cTx_num[6]		     ;
	int  iRecordNum	= 0 	     ;
	char cPackid	[9 ]	     ;
	char cPackday   [9]          ;
	char cWt_date	[9]	     ;
	
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&lv_pkgreg,	      0x00 ,  sizeof(lv_pkgreg));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&fd123 ,       0x00 ,  sizeof(fd123));
	memset(cReq_type,     0x00 ,  sizeof(cReq_type));
	memset(cTx_num,	      0x00 ,  sizeof(cTx_num));
	memset(&lv_addinfo,   0x00 ,  sizeof(lv_addinfo));

	memset(cPackid,	      0x00 ,  sizeof(cPackid));
	memset(cPackday,      0x00 ,  sizeof(cPackday));

	memset(cWt_date,      0x00 ,  sizeof(cWt_date));

       /***********************
	* 1.  初始化全局变量  *
  	***********************/
	pub_base_sysinit();
	if(iLvchkstat())
	{
		strncpy(g_pub_tx.reply,"P046",4);
		WRITEMSG
		goto ErrExit;
	}



       /*****************************
	* 2.   得到8583域的数据     *
	*****************************/
	ret=iLvTistoNotpay(&hv_notpayin);
	if( ret )
	{
		WRITEMSG
		goto ErrExit;
	}
	get_fd_data("1230",(char *)&fd123);
	/******获取退回类型0 单笔 1 整包****/
	cReq_type[0]     =   fd123.reqtype[0];
	vtcp_log("单笔0  整包 1====req_type==[%c]\n",cReq_type[0]);
	memcpy(cTx_num,fd123.o_txnum,sizeof(fd123.o_txnum));
	vtcp_log("原业务类型号:cTxnum==[%s]\n",cTx_num);


	/**************************
	 * 3.数据基本检测         *
	 **************************/
	vtcp_log("[%s][%d]hv_notpay.F_or_br_no==[%.12s]\n",__FILE__,__LINE__,hv_notpayin.F_or_br_no);
	 ret = iLvFzFCommonCheck(&hv_notpayin);
	 if(ret)
	 {
		vtcp_log("[%s][%d]执行数据基本检测失败!\n");
		goto ErrExit;
	 }

	 vtcp_log("[%s][%d]开始检查原支付交易!\n",__FILE__,__LINE__);

	vtcp_log("[%s][%d]hv_notpay.F_or_br_no==[%.12s]\n",__FILE__,__LINE__,hv_notpayin.F_or_br_no);
	/***检测原支付交易,存在性 以及状态,同时对lv_addinfo赋值*************/
	 ret = lv_check_payin_record(&hv_notpayin,&lv_addinfo,hv_notpayin.F_cmtno,cReq_type[0]);
	 if(ret)
	 {
		vtcp_log("[%s][%d]检测原支付交易记录失败!\n",__FILE__,__LINE__);
		goto ErrExit;
	 }
		vtcp_log("[%s][%d]lv_addinfo==[%s]\n",__FILE__,__LINE__,(char *)&lv_addinfo);
		vtcp_log("[%s][%d]lv_addinfo.opackid==[%.8s]\n",__FILE__,__LINE__,lv_addinfo.opackid);
		vtcp_log("[%s][%d]lv_addinfo.opack_date==[%.8s]\n",__FILE__,__LINE__,lv_addinfo.opack_date);
		vtcp_log("[%s][%d]lv_addinfo.otxnum==[%.5s]\n",__FILE__,__LINE__,lv_addinfo.otxnum);
		vtcp_log("[%s][%d]lv_addinfo.opkgno==[%.2s]\n",__FILE__,__LINE__,lv_addinfo.opkgno);
		vtcp_log("[%s][%d]lv_addinfo.reqtype==[%c]\n",__FILE__,__LINE__,lv_addinfo.reqtype[0]); 
	 
		

	/********************************
	 * 4. 数据库处理                *
	 *         插入非支付记录	*
	 ********************************/
	 
	memcpy(hv_notpayin.F_o_or_br_no,hv_notpayin.F_or_br_no,sizeof(hv_notpayin.F_o_or_br_no));
	memcpy(hv_notpayin.F_o_ac_br_no,hv_notpayin.F_ac_br_no,sizeof(hv_notpayin.F_o_ac_br_no));

	ret = lv_Ins_Notpay_record(&hv_notpayin,&hv_fzf,&lv_addinfo);
	if(ret)
	{
		vtcp_log("[%s][%d]插入hv_fzf/hv_fzfhtr错误!\n",__FILE__,__LINE__);
		goto ErrExit;
	}
	

     	/*******************
     	* 5. 准备流水      *
     	*******************/	
	ret=pub_ins_trace_log();
	if(ret)
	{
		sprintf(acErrMsg,"登记交易流水出错!");
		WRITEMSG
		goto ErrExit;
	}
	ret = lv_get_cmt319_tag(&hv_fzf);
	if(ret)
	{
		vtcp_log("[%s][%d]发送退回应答错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	}

	
     	/*******************
     	* 6. 准备输出数据  *
     	*******************/	
	/******提取返回值*****/
	vtcp_log("ORDERNO===[%s]\n",hv_notpayin.T_orderno);
	vtcp_log("ORDERNO===[%s]\n",hv_notpayin.F_orderno);
	vtcp_log("hv_fzf.orderno=[%s]\n",hv_fzf.orderno);
	memcpy(fd123.orderno,hv_notpayin.F_orderno,sizeof(fd123.orderno));
	memcpy(fd123.or_br_no,hv_notpayin.T_or_br_no,sizeof(fd123.or_br_no));
	memcpy(fd123.o_orderno,hv_fzf.o_orderno,sizeof(fd123.o_orderno));
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
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

