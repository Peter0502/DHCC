/***************************************************************
* 文 件 名:     spJ062.c
* 功能描述：    小额申请止付录入		
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
#include "hv_brno_orno_c.h"
spJ062()
{
	int ret = 0;
	int iOrderno = 0;
	HV_NOTPAY_AREA hv_notpayin;
	LV_NOTPAY_ADD   lv_addinfo;/****由于采用了大额的123域导致了几个数据的无法处理,设计此结构体来处理它们**/
        struct	lv_pkgreg_c    lv_pkgreg;
	struct  hv_fzf_c       hv_fzf;
	struct  hv_fzfhtr_c    hv_fzfhtr;
	struct  hv_brno_orno_c hv_brno_orno;
	HV_FD123_AREA          fd123 ;
	char cReq_type[2]            ;
	char cTx_num[6]		     ;
	int  iRecordNum	= 0 	     ;
	char cPackid	[9 ]	     ;
	char cPackday   [9]          ;
	char cOrbrno [13];
	char cKinbr  [6];
	
	memset(&hv_notpayin,  0x00 ,  sizeof(hv_notpayin));
	memset(&lv_pkgreg,	      0x00 ,  sizeof(lv_pkgreg));
	memset(&hv_brno_orno ,0x00 ,  sizeof(hv_brno_orno));
	memset(&hv_fzf,       0x00 ,  sizeof(hv_fzf));
	memset(&fd123 ,       0x00 ,  sizeof(fd123));
	memset(cReq_type,     0x00 ,  sizeof(cReq_type));
	memset(cTx_num,	      0x00 ,  sizeof(cTx_num));
	memset(&lv_addinfo,   0x00 ,  sizeof(lv_addinfo));

	memset(cPackid,	      0x00 ,  sizeof(cPackid));
	memset(cPackday,      0x00 ,  sizeof(cPackday));
  memset(cOrbrno ,      0x00 ,  sizeof(cOrbrno));
  memset(cKinbr  ,      0x00 ,  sizeof(cKinbr));
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
	get_fd_data("0030",cKinbr);
	get_fd_data("0440",cPackday);
	vtcp_log("[%s][%d]cKinbr=[%s]\n",__FILE__,__LINE__,cKinbr);
	ret = Hv_brno_orno_Sel(g_pub_tx.reply,&hv_brno_orno,"br_no='%s' ",cKinbr);
	if(ret)
	{
	   vtcp_log("[%s][%d]执行数据基本检测失败!\n");
	   goto ErrExit;
	}
	else
	{
		  memcpy(cOrbrno,hv_brno_orno.or_br_no,sizeof(cOrbrno)-1);
	}
	vtcp_log("[%s][%d]cOrbrno=[%s]\n",__FILE__,__LINE__,cOrbrno);
	/******获取止付类型0 单笔 1 整包****/
	cReq_type[0]     =   fd123.reqtype[0];
	vtcp_log("单笔0  整包 1====req_type==[%c]\n",cReq_type[0]);
	memcpy(cTx_num,fd123.o_txnum,sizeof(fd123.o_txnum));
	vtcp_log("原业务类型号:cTxnum==[%s]\n",cTx_num);

  if(cReq_type[0] == '0')/**0表示单笔**/
  {
  	  vtcp_log("[%s][%d]cReq_type=[%c]",__FILE__,__LINE__,cReq_type[0] );
  	  ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg,"orderno='%.8s' and or_br_no='%s' and wt_date='%s' and lw_ind='1' ",fd123.o_orderno,cOrbrno,cPackday);
  	  if(ret)
	    {
		   vtcp_log("[%s][%d]执行数据基本检测失败!\n");
		   goto ErrExit;
	    }
	    memcpy(hv_notpayin.F_o_or_br_no,cOrbrno,sizeof(cOrbrno)-1);
	    memcpy(hv_notpayin.F_o_ac_br_no,lv_pkgreg.ac_br_no,sizeof(lv_pkgreg.ac_br_no)-1);
	    memcpy(hv_notpayin.F_o_cmtno   ,lv_pkgreg.pkgno   ,sizeof(lv_pkgreg.pkgno)-1);
	    apitoa(lv_pkgreg.wt_date,8,hv_notpayin.F_o_wt_date);
  }
  else
  {
  	  vtcp_log("[%s][%d]cReq_type=[%c]",__FILE__,__LINE__,cReq_type[0] );
  	  ret = Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg,"packid='%.8s' and or_br_no='%s' and pack_date='%.8ld' and lw_ind='1' ",fd123.o_packid,cOrbrno,fd123.o_pack_date);
  	  if(ret)
	    {
		     vtcp_log("[%s][%d]执行数据基本检测失败!\n");
		     goto ErrExit;
	    }
  }
  vtcp_log("[%s][%d]hv_notpayin.F_o_or_br_no=[%s]\n",__FILE__,__LINE__,hv_notpayin.F_o_or_br_no);
  vtcp_log("[%s][%d]hv_notpayin.F_o_ac_br_no=[%s]\n",__FILE__,__LINE__,hv_notpayin.F_o_ac_br_no);
  vtcp_log("[%s][%d]hv_notpayin.F_o_cmtno   =[%s]\n",__FILE__,__LINE__,hv_notpayin.F_o_cmtno);
  vtcp_log("[%s][%d]hv_notpayin.F_o_wt_date= [%s]\n",__FILE__,__LINE__,hv_notpayin.F_o_wt_date);
  vtcp_log("[%s][%d]hv_notpayin.F_o_or_br_no=[%s]\n",__FILE__,__LINE__,hv_notpayin.F_o_or_br_no);
	/**************************
	 * 3.数据基本检测         *
	 **************************/
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


	/*****************
	 * 6.发送业务包  *
	 *****************/
	
	 ret = lv_get_cmt327_tag(&hv_fzf);
	 if(ret)
	 {
		vtcp_log("[%s][%d]发送业务包错误!\n",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"P064");
		goto ErrExit;
	 }
	
     	/*******************
     	* 7. 准备输出数据  *
     	*******************/	
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

