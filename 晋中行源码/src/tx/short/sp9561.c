/*****************************************************
* 文 件 名:  sp9561.c
* 功能描述： 查询大额支付交易基本信息
* 作    者:  ChenMing
* 完成日期： 2006年8月8日 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "draw_loan_hst_c.h"
int sp9561()
{
	int ret=0,i;

	int iCmtno = 0;
	char cDate[9];
	struct hv_zf_c  hv_zf;
	char cKinbr[BRNO_LEN+1];
	char cBuff[2001];
	char cFilename[1000];
	struct draw_loan_hst_c m_draw_loan_hst;
	memset(&m_draw_loan_hst,0x00,sizeof(m_draw_loan_hst));
	memset(cFilename, 0 , sizeof(cFilename));
	HV_FD123_AREA fd123;
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cDate, 0 , sizeof(cDate));
	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(cKinbr , 0 , sizeof(cKinbr));
	vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_fd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);
	memcpy(hv_zf.cmtno,fd123.cmtno,sizeof(hv_zf.cmtno)-1);
	memcpy(hv_zf.orderno,fd123.orderno,sizeof(hv_zf.orderno)-1);
	memcpy(hv_zf.or_br_no,fd123.or_br_no,sizeof(hv_zf.or_br_no)-1);
	vtcp_log("+++++hv_zf.or_br_no==[%s]!+++++",hv_zf.or_br_no);
	zip_space(hv_zf.or_br_no);
	vtcp_log("+++++hv_zf.or_br_no==[%s]!+++++",hv_zf.or_br_no);


	iCmtno = apatoi(hv_zf.cmtno,sizeof(hv_zf.cmtno)-1);


	if(strlen(hv_zf.or_br_no)==0)
	{
		GetOr_br_noByBr_no(cKinbr, hv_zf.or_br_no );
	}
	get_zd_data("0440",cDate);
	get_zd_long("0440",&hv_zf.wt_date);	   /*委托日期*/	
	vtcp_log("[%s][%d]cmtno=[%s]orderno=[%s],or_br_no=[%s]wt_date=[%ld]",__FILE__,__LINE__,hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);
	vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);

	if(iCmtno!=0)
	{
		ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%s and br_no='%s'", \ 
		hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,cDate,cKinbr);
	}
	else if(iCmtno==0)
	{
		ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, " orderno='%s' and or_br_no='%s' and wt_date=%s and br_no='%s'", \ 
		hv_zf.orderno,hv_zf.or_br_no,cDate,cKinbr);
	}
	vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此大额支付交易");
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" ); */
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);
    vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
  memcpy(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	memcpy(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.cmtno));
	memcpy(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
	/**发起行号 原发起行号 */
	memcpy(fd123.or_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.or_br_no        ));
	/**发报中心代码**/
	memcpy(fd123.sendco          ,hv_zf.sendco          ,sizeof(fd123.sendco          ));
	/**付款人开户行**/
	memcpy(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
	/**付款人帐号(原收款人帐号)**/
	memcpy(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
	/**付款人名称(原收款人名称)**/
	memcpy(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
	/**付款人地址(原收款人地址)**/
	memcpy(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
	/**接受清算行号**/
	memcpy(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
	/**接受行号(原发起行号)**/
	memcpy(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
	/**收报中心代码**/
	memcpy(fd123.receco          ,hv_zf.receco          ,sizeof(fd123.receco          ));
	/**收款人开户行号**/
	memcpy(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
	/**收款人帐号,原付款人帐号**/
	memcpy(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
	/**收款人姓名,原付款人姓名**/
	memcpy(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
	vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);
	/**收款人地址**/
	memcpy(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
	vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);
	/**业务种类**/	
	memcpy(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
	vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);
	/*交易序号 撤销支付交易序号*/
	memcpy(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
	vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);
	/**附言**/
	memcpy(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
	vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);
	/**原支付交易序号**/
	memcpy(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
	fd123.operlevel[0]=hv_zf.operlevel[0];
	fd123.proc_sts[0]=hv_zf.hv_sts[0];
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("fd123=[%s]",(char*)&fd123);
    set_zd_double("0390",hv_zf.tx_amt);   /* 交易金额 */
    /*  根据需要再增加  */
    
    /****增加返回91域,行内行号 20100618****/
    pub_base_strpack(hv_zf.br_no);
    set_zd_data("0910",hv_zf.br_no);
    vtcp_log("[%s,%d]交易机构=[%s]",__FILE__,__LINE__,hv_zf.br_no);
    
    set_zd_data("0580",hv_zf.note_no);     /* 凭证号 ? 票据号码*/
    set_zd_double("0400",hv_zf.tx_amt);   /* 交易金额 */
    set_zd_data("0810",hv_zf.pay_ac_no);   /* 付款人帐号 */
    set_zd_data("0961",hv_zf.pay_name);   /* 付款人名称 */
    set_zd_data("0962",hv_zf.pay_addr);   /* 付款人地址 */
    set_zd_data("0820",hv_zf.cash_ac_no);   /* 收款人帐号 */
    set_zd_data("0963",hv_zf.cash_name);   /* 收款人姓名 */ 
    set_zd_data("0964",hv_zf.cash_addr);   /* 收款人地址 */
    set_zd_data("1222",hv_zf.pay_opn_br_no);   /* 付款人开户行号 */    
    set_zd_data("1223",hv_zf.cash_opn_br_no);   /* 收款人开户行号 */
    set_zd_data("1228",hv_zf.cur_no);   /* 货币符号 */
    set_zd_data("1226",hv_zf.tx_chrg_ind);   /* 优先级别  手续费标志 ?*/    
    set_zd_data("0890",hv_zf.note_type);   /* 凭证种类 */
    set_zd_data("1050",hv_zf.hv_brf);   /* 附言 60 */
    vtcp_log("%s,%d o_wt_date=[%d]",__FILE__,__LINE__,hv_zf.o_wt_date);
    set_zd_long("0450",hv_zf.o_wt_date);   /* 原委托日期 */
    set_zd_long("0440",hv_zf.wt_date);   /* 委托日期 */
		if(!memcmp(hv_zf.cur_no,"RMB",3))
			set_zd_data("0210","01");   /* 货币符号 */
		
		if(memcmp(hv_zf.cmtno,HV_HD,sizeof(hv_zf.cmtno)-1) &&
		  memcmp(hv_zf.cmtno,HV_GKZJHH,sizeof(hv_zf.cmtno)-1))
		{
			FILE *fpw;
			/* 写入文件传回去 */
			vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
			iHvGetAddit(cDate,cDate,hv_zf.addid,"000",cBuff,sizeof(cBuff)-1);
			vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
			ret=pub_base_AllDwnFilName(cFilename);
			if(ret)
			{
				vtcp_log("%s,%d 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
			}
			fpw=fopen(cFilename,"w");
			if(fpw==NULL)
			{
				vtcp_log("%s,%d 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
			}
  
			if(!memcmp(hv_zf.cmtno,HV_CJ,sizeof(hv_zf.cmtno)-1))
			{
				HV_TYCJ_ADD hv_cj;
				memset(&hv_cj, 0 , sizeof(hv_cj));
				memcpy((char *)&hv_cj,cBuff, sizeof(hv_cj));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_cj);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&hv_cj,sizeof(hv_cj));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			if(!memcmp(hv_zf.cmtno,HV_WT,sizeof(hv_zf.cmtno)-1))
			{
				HV_WTSK_ADD hv_wt;
				memset(&hv_wt, 0 , sizeof(hv_wt));
				memcpy((char *)&hv_wt,cBuff, sizeof(hv_wt));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_wt);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&hv_wt,sizeof(hv_wt));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			if(!memcmp(hv_zf.cmtno,HV_TSCF,sizeof(hv_zf.cmtno)-1))
			{
				HV_TSCF_ADD hv_tscf;
				memset(&hv_tscf, 0 , sizeof(hv_tscf));
				memcpy((char *)&hv_tscf,cBuff, sizeof(hv_tscf));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_tscf);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&hv_tscf,sizeof(hv_tscf));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			vtcp_log("cmtno=[%s][%s][%d]",hv_zf.cmtno,__FILE__,__LINE__);
			if(!memcmp(hv_zf.cmtno,HV_HPYC,sizeof(hv_zf.cmtno)-1))
			{
				HV_121_ADD hv_hpyc;
				memset(&hv_hpyc, 0 , sizeof(hv_hpyc));
				memcpy((char *)&hv_hpyc,cBuff, sizeof(hv_hpyc));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_hpyc);
				memset(cBuff, 0 , sizeof(cBuff));
				memcpy(cBuff,(char *)&hv_hpyc,sizeof(hv_hpyc));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2" );
			}
			fclose(fpw);
	 }
	 /*20110803 晋中商行 因信贷系统更改*/
	 
	 ret=Draw_loan_hst_Sel(g_pub_tx.reply,&m_draw_loan_hst,"brf1 like 'HV%s________%s' ",g_pub_tx.tx_br_no,hv_zf.orderno);
	 if(ret){
	 	 if(ret==100){
	 		 set_zd_data("0830","");
	 		 sprintf(acErrMsg,"没找到啊cmtno=[%s]orderno=[%s]",hv_zf.orderno);
		   WRITEMSG
	 	 }else{ 	 	
	 	 	sprintf(acErrMsg,"查询信贷系统审批号码出错[%d]",ret);
		  WRITEMSG
		  goto ErrExit;	 		 
	 	 }
	 }
	 set_zd_data("0830",m_draw_loan_hst.draw_no);
	 
		
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


