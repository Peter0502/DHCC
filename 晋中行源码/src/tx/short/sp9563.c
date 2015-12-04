/*****************************************************
* 文 件 名:  sp9563.c
* 功能描述： 查询大额非支付交易基本信息
* 作    者:  xyy
* 完成日期： 2006-8-28 17:06 
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*****************************************************/
   
#define EXTERN
#include <string.h>
#include "public.h"
#include "hv_define.h"
#include "lv_define.h"
int sp9563()
{
	int ret=0,i;
	int iCmtno = 0;


	char cDate[9];
	char cOtype[2];
	char cCmtno[4];
	char _cmtno[4];/*获取前台输入的报文编号,解决查询内容查复内容的问题**/
	char _lw_ind[2];/*获取前台的输入,解决查询查复内容的问题*****/
	char cKinbr[BRNO_LEN+1];
	char cBuff[2001];
	char cFilename[200];
	char _fzf_content[256]; /**申请类应答类都给打印出来***/
	char _fzf_content1[256]; /**申请类应答类都给打印出来***/
	HV_FD123_AREA fd123;
	HV_725_ADD    hv_725;
	struct hv_fzf_c  hv_fzf;
	
	
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cDate, 0 , sizeof(cDate));
	memset(cKinbr , 0 , sizeof(cKinbr));
	memset(cCmtno,0, sizeof(cCmtno));
	memset(cFilename, 0 , sizeof(cFilename));
	memset(&fd123, 0,sizeof(fd123));
	memset(&hv_fzf,0x00,sizeof(struct hv_fzf_c));
	memset(&hv_725,0x00,sizeof(hv_725));
	memset(_fzf_content,0x00,sizeof(_fzf_content));
	memset(_fzf_content1,0x00,sizeof(_fzf_content1));
	
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);
 	
	pub_base_sysinit();
	get_zd_data("0030",cKinbr);
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("[%s,%d]1230=[%s]",__FILE__,__LINE__,(char*)&fd123);
	get_zd_data("0680",cOtype);
	
	memcpy(hv_fzf.cmtno,fd123.cmtno,sizeof(hv_fzf.cmtno)-1);
	memcpy(hv_fzf.orderno,fd123.orderno,sizeof(hv_fzf.orderno)-1);
	memcpy(hv_fzf.or_br_no,fd123.or_br_no,sizeof(hv_fzf.or_br_no)-1);
	vtcp_log("+++++hv_fzf.or_br_no==[%s]!+++++",hv_fzf.or_br_no);
	vtcp_log("%s,%d +++++lw_ind==[%c]!+++++",__FILE__,__LINE__,fd123.lw_ind[0]);
	
	memcpy(cCmtno,fd123.cmtno,sizeof(cCmtno)-1);


	iCmtno   = apatoi(hv_fzf.cmtno,sizeof(hv_fzf.cmtno)-1);
	get_zd_data("0440",cDate);
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]cDate=[%s]",__FILE__,__LINE__,cDate);	
	get_zd_long("0440",&hv_fzf.wt_date);	   /*委托日期*/	
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);	
	vtcp_log("[%s][%d]cmtno=[%s]orderno=[%s],or_br_no=[%s]wt_date=[%ld]",__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,hv_fzf.wt_date);
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);
	zip_space(hv_fzf.or_br_no);

	if(iCmtno!=0)
	{
		vtcp_log("%s,%d 尝试找到302",__FILE__,__LINE__);
		if(iCmtno==303 && strlen(hv_fzf.or_br_no)==0)
		{
			if(strlen(hv_fzf.or_br_no)==0)
				GetOr_br_noByBr_no(cKinbr, hv_fzf.or_br_no );
			ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"cmtno='%s' and orderno='%s' and (or_br_no='%s' or or_br_no is null)\
				and wt_date='%s' and otype='%c'", \ 
				hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,cDate,cOtype[0]);
		}
		/* begin add by LiuHuafeng 20070505 */
		else if(iCmtno==302)
		{
			if(strlen(hv_fzf.or_br_no)==0)
				GetOr_br_noByBr_no(cKinbr, hv_fzf.or_br_no );
			vtcp_log("%s,%d 尝试找到302",__FILE__,__LINE__);
			ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"cmtno='%s' and orderno='%s' and or_br_no='%s'\
				and wt_date='%s' and otype='%c'", \ 
				hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,cDate,cOtype[0]);
			
			vtcp_log("%s,%d ret==[%d]",__FILE__,__LINE__,ret);
			if(ret!=DB_OK)
			{
				vtcp_log("%s,%d 没有找到302，按照301找",__FILE__,__LINE__);
				/* 如果没有发现按301查 */
				ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"cmtno='%s' and res_orderno='%s' and req_br_no='%s'\
				and wt_date='%s' and otype='%c'", \ 
				"301",hv_fzf.orderno,hv_fzf.or_br_no,cDate,cOtype[0]);
			}
		}
		/** end by LiuHuafeng 20070505 */
		else
		{
			if(strlen(hv_fzf.or_br_no)==0)
				GetOr_br_noByBr_no(cKinbr, hv_fzf.or_br_no );
			ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"cmtno='%s' and orderno='%s' and or_br_no='%s'\
				and wt_date='%s' and otype='%c'", \ 
				hv_fzf.cmtno,hv_fzf.orderno,hv_fzf.or_br_no,cDate,cOtype[0]);
		}
	}
	else if(iCmtno==0)
	{
		zip_space(hv_fzf.or_br_no);
		if(strlen(hv_fzf.or_br_no)==0)
		{
			GetOr_br_noByBr_no(cKinbr, hv_fzf.or_br_no );
		}
		ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, " orderno='%s' and or_br_no='%s' and wt_date='%s' and otype='%c'", \ 
		hv_fzf.orderno,hv_fzf.or_br_no,cDate,cOtype[0]);
	}
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if ( ret==100 )
	{
		sprintf(acErrMsg,"无此大额非支付交易");
		WRITEMSG
		goto	ErrExit;
	}
	else if ( ret )
	{
		sprintf(acErrMsg,"查询大额支付非交易基本信息出错[%d]",ret);
		WRITEMSG
		goto ErrExit;
	}	
	vtcp_log("sp9563 开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__); 

	/**cmtno号码**/
	memcpy(fd123.cmtno	,hv_fzf.cmtno		,sizeof(fd123.cmtno));		
	/**o_cmtno**/	
	memcpy(fd123.o_cmtno	,hv_fzf.o_cmtno	,sizeof(fd123.cmtno));		
	/**发起行号 原发起行号 **/		
	memcpy(fd123.or_br_no	,hv_fzf.or_br_no,sizeof(fd123.or_br_no));	
	/**付款清算行行号**/		
	memcpy(fd123.pay_qs_no	,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));	
	/**发报中心代码**/		
	memcpy(fd123.sendco		,hv_fzf.sendco	,sizeof(fd123.sendco));
	/**接受清算行号**/				
	memcpy(fd123.cash_qs_no	,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));		
	/**接受行号(原发起行号)**/	
	memcpy(fd123.ac_br_no	,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
	/**交易序号 撤销支付交易序号**/			
	memcpy(fd123.orderno	,hv_fzf.orderno	,sizeof(fd123.orderno));	
	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);
	/**附言**/		
	memcpy(fd123.brf		,hv_fzf.hv_brf	,sizeof(fd123.brf));	
	/**原支付交易序号**/			
	memcpy(fd123.o_orderno	,hv_fzf.o_orderno,sizeof(fd123.o_orderno));	
	/**非支付交易状态**/		
	fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];			
	/**来往表示**/						
	fd123.lw_ind[0]	= hv_fzf.lw_ind[0];
	/**应答状态**/					
	fd123.resp[0]	= hv_fzf.res_sts[0];
	/**应答序号**/
	memcpy(fd123.cr_tx_num,hv_fzf.res_orderno,sizeof(hv_fzf.res_orderno)-1);
	/**应答行号**/
	memcpy(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
	vtcp_log("[%s][%d]fd123.cr_br_no=[%s]\n",__FILE__,__LINE__,fd123.cr_br_no);
	/**应答日期**/
	sprintf(fd123.cr_date,"%8d",hv_fzf.res_date);
	/**原交易金额**/								
	set_zd_double("0390",hv_fzf.o_tx_amt);	
	/**应答内容**/
	/* rem by liuyue
	if(memcmp(cCmtno,"314",sizeof(cCmtno)-1)==0 || memcmp(cCmtno,"302",sizeof(cCmtno)-1)==0 ||memcmp(cCmtno,"312",3)==0)
	{
		memcpy(_fzf_content,hv_fzf.res_content,sizeof(hv_fzf.res_content));
		memcpy(_fzf_content1,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);
		memcpy(fd123.notpay_content,hv_fzf.res_content,sizeof(fd123.notpay_content));	
	}
	else
	{
		memcpy(_fzf_content,hv_fzf.req_content,sizeof(hv_fzf.req_content));
		memcpy(_fzf_content1,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
		memcpy(fd123.notpay_content,hv_fzf.req_content,sizeof(fd123.notpay_content));
	}
	*************************/
	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);
	memcpy(_fzf_content,hv_fzf.req_content,sizeof(hv_fzf.req_content));
	memcpy(_fzf_content1,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);

	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);
	deal_half_charcter(_fzf_content);
	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);
	deal_half_charcter(_fzf_content1);
	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);

	
	/***modify by yanqq 20110114 查复内容赋值FD123域***/
	/*memcpy(fd123.notpay_content,_fzf_content,sizeof(fd123.notpay_content));*/
	memcpy(fd123.notpay_content,_fzf_content1,sizeof(fd123.notpay_content));
	/*** modify end ***/
	vtcp_log("fd123==[%s]\n",(char *) &fd123);

	vtcp_log("[查询内容]_fzf_content==[%s]\n",_fzf_content);
	/***modify by yanqq 20110114 查询内容赋值1240***/   
	/*vtcp_log("_fzf_content1==[%s]\n",_fzf_content);
	set_zd_data("1240",_fzf_content1);*/
	vtcp_log("[查复内容]_fzf_content1==[%s]\n",_fzf_content1);
	set_zd_data("1240",_fzf_content);
	/*** modify end ***/
	set_zd_data("1250",_fzf_content1);
		
		
	/* by liuyue 20060922*****处理查询时候原发起行与原接受行的问题*****/
	if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**发起行就是原发起行,即查询本行发出的交易**/
	{
		fd123.tx_type[0]	 =  'A';
	}
	else 
		fd123.tx_type[0]	=   'B';

	
	vtcp_log("[%s][%d]hv_fzf.cmtno==[%s]\n",__FILE__,__LINE__,hv_fzf.cmtno);
	if(!memcmp(hv_fzf.cmtno,"725",3))
	{
		 char cReal_js_amt[16];
		 char cCp_amt[16];
		 char cPj_date[9];
		
		iHvGetAddit(cDate,cDate,hv_fzf.addid,"000",(char *)&hv_725,sizeof(hv_725)-1);
		vtcp_log("[%s][%d]hv_725==[%s]\n",__FILE__,__LINE__,(char *)&hv_725);
		 memcpy(fd123.pay_ac_no,hv_725.pay_ac_no,sizeof(fd123.pay_ac_no));
		 memcpy(fd123.pay_name,hv_725.pay_name,sizeof(fd123.pay_name));
		 memcpy(fd123.pay_addr,hv_725.pay_addr,sizeof(fd123.pay_addr));
		 memcpy(fd123.cash_ac_no,hv_725.last_ac_no,sizeof(fd123.cash_ac_no));
		 memcpy(fd123.cash_name,hv_725.last_name,sizeof(fd123.cash_name));
		 memcpy(fd123.po_no,hv_725.pj_num,sizeof(fd123.po_no));
		/***
		 sprintf(fd123.pay_ac_no,"%32s",hv_725.pay_ac_no);
		 sprintf(fd123.pay_name,"%60s",hv_725.pay_name);
		 sprintf(fd123.pay_addr,"%60s",hv_725.pay_addr);
		 sprintf(fd123.cash_ac_no,"%32s",hv_725.last_ac_no);
		 sprintf(fd123.cash_name,"%60s",hv_725.last_name);
		 sprintf(fd123.po_no,"%8s",hv_725.pj_num);
		****/

		 memset(cCp_amt,0x00,sizeof(cCp_amt));
		 memcpy(cCp_amt,hv_725.cp_amt,sizeof(hv_725.cp_amt));
		 set_zd_data("0400",cCp_amt);	

		 memset(cReal_js_amt,0x00,sizeof(cReal_js_amt));
		 memcpy(cReal_js_amt,hv_725.real_js_amt,sizeof(hv_725.real_js_amt));
		 set_zd_data("0410",cReal_js_amt);

		 memset(cPj_date,0x00,sizeof(cPj_date));
		 memcpy(cPj_date,hv_725.pj_date,sizeof(hv_725.pj_date));
		 set_zd_data("0460",cPj_date);	

		vtcp_log("hv_725.last_ac_no====[%s]\n",hv_725.last_ac_no);
		vtcp_log("hv_725.last_name====[%s]\n",hv_725.last_name);
		vtcp_log("hv_725.pjnum====[%s]\n",hv_725.pj_num);
		vtcp_log("hv_725.cp_amt====[%s]\n",hv_725.cp_amt);
		vtcp_log("hv_725.real_js_amt====[%s]\n",hv_725.real_js_amt);
		vtcp_log("hv_725.pj_date====[%s]\n",hv_725.pj_date);
		vtcp_log("fd123.po_no==[%s]\n",fd123.po_no);
		 
	}
			

	/**********add end by liuyue 借用了hv_fzf.tx_type**************/
		
	memcpy(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
	set_zd_long("0440",hv_fzf.wt_date);	
	set_zd_long("0450",hv_fzf.o_wt_date);	
	vtcp_log("[%s][%d],wt_date=[%ld][%ld]\n",__FILE__,__LINE__,hv_fzf.wt_date,hv_fzf.o_wt_date);
	pub_base_full_space((char*)&fd123,sizeof(fd123));
	set_zd_data("1230",(char*)&fd123);
	vtcp_log("[%d]fd123=[%s]",__LINE__,(char*)&fd123);


	/*******************	
	if(memcmp(hv_fzf.cmtno,"721",sizeof(hv_fzf.cmtno)-1) ==0 || \
		memcmp(hv_fzf.cmtno,"724",sizeof(hv_fzf.cmtno)-1) ==0)
	{
		FILE *fpw;
		vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
		iHvGetAddit(cDate,cDate,hv_fzf.addid,"000",cBuff,sizeof(cBuff)-1);
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

		if(!memcmp(hv_fzf.cmtno,"721",sizeof(hv_fzf.cmtno)-1))
		{
			HV_721_ADD hv_721_add;
			memset(&hv_721_add, 0 , sizeof(hv_721_add));
			memcpy((char *)&hv_721_add,cBuff, sizeof(hv_721_add));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_721_add);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&hv_721_add,sizeof(hv_721_add));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		if(!memcmp(hv_fzf.cmtno,"724",sizeof(hv_fzf.cmtno)-1))
		{
			HV_724_ADD hv_724_add;
			memset(&hv_724_add, 0 , sizeof(hv_724_add));
			memcpy((char *)&hv_724_add,cBuff, sizeof(hv_724_add));
			vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_724_add);
			memset(cBuff, 0 , sizeof(cBuff));
			memcpy(cBuff,(char *)&hv_724_add,sizeof(hv_724_add));
			fprintf(fpw,"%s",cBuff);
			set_zd_data( DC_FILE_SND,"2" );
		}
		fclose(fpw);
	}
	****************/

		
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

int deal_half_charcter(char *buf)
{
		int i = 0;
		int j = 0;
		int size = 0;
		int index =0;
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));
		vtcp_log("[%s][%d]buf===[%s]====\n",__FILE__,__LINE__,buf);
		size = strlen(buf);

		for(i=0;i<size;i++)
		{
				if((int)buf[i]<128 &&(int)buf[i+1]>128 && (i+1+index)%2)
				{
						cBuf[j] = buf[i];
						cBuf[j+1]= ' ';
						j = j+2;
						index++;
				}
				else
				{
						cBuf[j] = buf[i];
						j++;
				}
		}

		memcpy(buf,cBuf,j);
		buf[j] = 0x00;
		return 0;
}
