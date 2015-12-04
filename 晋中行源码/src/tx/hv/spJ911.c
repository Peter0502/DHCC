/*************************************************
* 文 件 名:  spJAAA.c
* 功能描述： 大额来帐打印
* 作    者:  ligl
* 完成日期： 2006年8月29日
* 修改记录： 
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define  EXTERN
#include <string.h>
#include "public.h"
#include "hv_pub.h"
#include "hv_define.h"
#include "hv_addit_c.h"
#include "find_debug.h"
#include "hv_poinfo_c.h"

spJ911()
{
	int ret=0,i=0;
	struct hv_zf_c  hv_zf;
	struct hv_fzf_c hv_fzf;
	struct hv_addit_c hv_addit;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*机构号*/
	char cTel[5];  /*操作员号*/
	char cTx_date[9];/*交易日期*/
	char cOrderno[8];/*顺序号*/
	char cNextOrderno[9];/*下一个顺序号*/
	char cMinno[9]; /*最小序号*/
	char cTemno[9]; /*临时序号*/
	char cTxstat[2];/*交易状态*/
	char cCmtno[4];/*汇票类型*/
	char cVtype[4];/*凭证类型*/
	char cVocnum[17];/*凭证号码*/
	char cNextVocnum[17];/*下一个凭证号码*/
	char cZf_type[2];
	char cPrint[2];  /*判断首次打印标志*/
	char wherelist[1024];  /**查询条件**/
	char cBuff[341];
	char cTmpcmtno[4];
	char cTmpday[9];
	char cOrbrno[61];
  char cTmpcmtno1[4];

	char cNotpay_content[1024];
	char cFilename[1000];
	memset(cOrbrno,0 , sizeof(cOrbrno));
	memset(cFilename, 0 , sizeof(cFilename));
	memset(cBuff, 0 , sizeof(cBuff));

	memset(&fd123, ' ',sizeof(fd123));
	memset(&hv_addit,0x0,sizeof(struct hv_addit_c));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cPrint     ,0, sizeof(cPrint));
	memset(cTmpcmtno  ,0, sizeof(cTmpcmtno));
	memset(cTmpcmtno1  ,0, sizeof(cTmpcmtno1));
	memset(cTel       ,0, sizeof(cTel));
	memset(cTmpday    ,0, sizeof(cTmpday));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cTxstat    ,0, sizeof(cTxstat));
	memset(cCmtno     ,0, sizeof(cCmtno));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cOrderno   ,'0', sizeof(cOrderno));
	memset(cTemno     ,0,sizeof(cTemno));
	memset(&hv_zf,0x00,sizeof(struct hv_zf_c));
	memset(cNextOrderno,0,sizeof(cNextOrderno));
	memset(cNextVocnum    ,0, sizeof(cNextVocnum));
	memset(cNotpay_content,0x00,sizeof(cNotpay_content));

  memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJAAA开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("spJAAA开始！cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0070",cTel);
	get_zd_data("0440",cTx_date);
	get_fd_data("0290",cTemno);
	vtcp_log("cOrderno=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	get_zd_data("0240",cCmtno);
	get_zd_data("0230",cTmpcmtno1);
	get_zd_data("0670",cPrint);
	get_zd_data("0690",cTxstat);
	get_zd_data("0720",cZf_type);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	get_zd_data("0250",cOrbrno);
	zip_space(cOrbrno);
	i=strlen(cTemno);
	MEMCPY_DEBUG(cOrderno+8-i,cTemno,sizeof(cTemno)-1);
	MEMCPY_DEBUG(cNextVocnum,cVocnum,sizeof(cNextVocnum)-1);
	vtcp_log("cOrderno=[%d][%s][%d]zftype=[%c]",i,__FILE__,__LINE__,cZf_type[0]);
	vtcp_log("[%s][%d]cCmtno=====================[%s]",__FILE__,__LINE__,cCmtno);
	vtcp_log("AP MSG: 开始大额来帐凭证打印程序![%s][%d]",__FILE__,__LINE__);
	/***********检查凭证*************/
	if(cZf_type[0]=='0' && memcmp(cTmpcmtno1,"802",3))
	{
	 ret=pub_com_ChkNoteUse(cVtype);/*检查凭证种类是否可用*/
	 if( ret )
	 {
		sprintf(acErrMsg,"凭证处于停用状态!");
		WRITEMSG
		goto ErrExit;
	 } 
		ret=pub_com_NoteUse(0,0,cVtype,cVocnum,cVocnum,cTel);/*凭证销号处理（交易过程中使用凭证）*/
		if( ret )
		{
			sprintf(acErrMsg,"柜员使用凭证错误!");
			WRITEMSG
			goto ErrExit;
		}   
		/**得到下一个凭证号**/
		ret=pub_com_GetNoteMin(cVtype,cNextVocnum,cTel);
		if (ret)
		{
			MEMCPY_DEBUG(cNextVocnum,"没有凭证了",sizeof(cNextVocnum)-1);
			WRITEMSG
			goto ErrExit;
		}
		/***********检查完毕*************/
		vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
		vtcp_log("AP MSG: 得到要打印的纪录![%s][%d]",__FILE__,__LINE__);
		vtcp_log("cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	}
	if(cZf_type[0]=='0')
	{
vtcp_log("111111111cmtno[%s]",cTmpcmtno1);
	 if(memcmp(cTmpcmtno1,"802",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='802'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 else if(memcmp(cTmpcmtno1,"812",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='812'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 	else if(memcmp(cTmpcmtno1,"814",3)==0)
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='814'",cBr_no,cTx_date,cOrderno);
	 		
	 	}
	 	else
	 	{
	  ret = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
	}
		vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"无此大额支付交易");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
		if(cPrint[0]==hv_zf.hv_prt_ind[0])
			{
				sprintf(acErrMsg,"该笔已做首次打印[%d]cPrint=[%c]hv_zf.hv_prt_ind[0]=[%c]",ret,cPrint[0],hv_zf.hv_prt_ind[0]);
			  WRITEMSG
			  goto ErrExit;
			}
		vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
		/*更新大额支付主表*/
		vtcp_log("更新大额支付主表[%s][%d]",__FILE__,__LINE__);
		if(memcmp(cTmpcmtno1,"812",3)==0)
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='812' ",cBr_no,cTx_date,cOrderno);
	  }
		else if(memcmp(cTmpcmtno1,"814",3)==0)
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and cmtno='814' ",cBr_no,cTx_date,cOrderno);
		}	
		else
		{
		ret = sql_execute("update hv_zf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s' ",cBr_no,cTx_date,cOrderno,cOrbrno);
	  }
		if (ret)
		{
			sprintf(acErrMsg,"更新大额支付交易基本信息出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		/**给FD123域数据**/
	  MEMCPY_DEBUG(fd123.cmtno,hv_zf.cmtno,sizeof(fd123.cmtno));
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		MEMCPY_DEBUG(fd123.o_cmtno,hv_zf.o_cmtno,sizeof(fd123.o_cmtno));
		MEMCPY_DEBUG(fd123.pay_qs_no,hv_zf.pay_qs_no,sizeof(fd123.pay_qs_no));
		/**发起行号 原发起行号 */
		MEMCPY_DEBUG(fd123.or_br_no        ,hv_zf.or_br_no        ,sizeof(fd123.or_br_no        ));
		/**发报中心代码**/
		MEMCPY_DEBUG(fd123.sendco          ,hv_zf.sendco          ,sizeof(fd123.sendco          ));
		/**付款人开户行**/
		MEMCPY_DEBUG(fd123.pay_opn_br_no   ,hv_zf.pay_opn_br_no   ,sizeof(fd123.pay_opn_br_no   ));
		/**付款人帐号(原收款人帐号)**/
		MEMCPY_DEBUG(fd123.pay_ac_no       ,hv_zf.pay_ac_no       ,sizeof(fd123.pay_ac_no       ));
		/**付款人名称(原收款人名称)**/
		MEMCPY_DEBUG(fd123.pay_name        ,hv_zf.pay_name        ,sizeof(fd123.pay_name        ));
		/**付款人地址(原收款人地址)**/
		MEMCPY_DEBUG(fd123.pay_addr        ,hv_zf.pay_addr        ,sizeof(fd123.pay_addr        ));
		/**接受清算行号**/
		MEMCPY_DEBUG(fd123.cash_qs_no      ,hv_zf.cash_qs_no      ,sizeof(fd123.cash_qs_no      ));
		/**接受行号(原发起行号)**/
		MEMCPY_DEBUG(fd123.ac_br_no        ,hv_zf.ac_br_no        ,sizeof(fd123.ac_br_no        ));
		/**收报中心代码**/
		MEMCPY_DEBUG(fd123.receco          ,hv_zf.receco          ,sizeof(fd123.receco          ));
		/**收款人开户行号**/
		MEMCPY_DEBUG(fd123.cash_opn_br_no  ,hv_zf.cash_opn_br_no  ,sizeof(fd123.cash_opn_br_no  ));
		/**收款人帐号,原付款人帐号**/
		MEMCPY_DEBUG(fd123.cash_ac_no      ,hv_zf.cash_ac_no      ,sizeof(fd123.cash_ac_no      ));
		/**收款人姓名,原付款人姓名**/
		MEMCPY_DEBUG(fd123.cash_name       ,hv_zf.cash_name       ,sizeof(fd123.cash_name       ));
		vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,hv_zf.cash_name);
		/**收款人地址**/
		MEMCPY_DEBUG(fd123.cash_addr       ,hv_zf.cash_addr       ,sizeof(fd123.cash_addr       ));
		vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,hv_zf.cash_addr);
		/**业务种类**/	
		MEMCPY_DEBUG(fd123.yw_type         ,hv_zf.yw_type         ,sizeof(fd123.yw_type         ));
		vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,hv_zf.yw_type);
		/*交易序号 撤销支付交易序号*/
		MEMCPY_DEBUG(fd123.orderno         ,hv_zf.orderno         ,sizeof(fd123.orderno         ));
		vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,hv_zf.orderno);
		/**附言**/
		MEMCPY_DEBUG(fd123.brf             ,hv_zf.hv_brf          ,sizeof(fd123.brf             ));
		vtcp_log("[%s][%d]hv_brf=[%s]",__FILE__,__LINE__,hv_zf.hv_brf);
		/**原支付交易序号**/
		MEMCPY_DEBUG(fd123.o_orderno       ,hv_zf.o_orderno       ,sizeof(fd123.o_orderno       ));
		/** add by wangyongwei 20071012 取汇票号码传到前台**/
		if(!memcmp(hv_zf.cmtno,"122",3) || !memcmp(hv_zf.cmtno,"123",3)||!memcmp(hv_zf.cmtno,"124",3)){
					char tmp_wtdate[9];
					char tmp_tagdata[1024];
					char sign_brno[13];
					struct hv_poinfo_c cHv_poinfo;
					memset(&cHv_poinfo,0,sizeof(cHv_poinfo));
					memset(tmp_wtdate,0,sizeof(tmp_wtdate));
					memset(tmp_tagdata,0,sizeof(tmp_tagdata));
					memset(sign_brno,0,sizeof(sign_brno));
					sprintf(tmp_wtdate,"%d",hv_zf.wt_date);
					vtcp_log("[%s][%d]******1",__FILE__,__LINE__);
				g_reply_int=iHvGetAddit(tmp_wtdate,tmp_wtdate,hv_zf.addid,"000",tmp_tagdata,0);
					vtcp_log("[%s][%d]******2",__FILE__,__LINE__);
				if(g_reply_int){
						sprintf(acErrMsg,"取汇票号码失败!");
						set_zd_data(DC_GET_MSG,acErrMsg);
						strcpy(g_pub_tx.reply,"Z999");
						goto ErrExit;	
				}	
				memcpy(fd123.o_pack_date,tmp_tagdata+12,8);	/*票据日期*/
				memcpy(fd123.po_no,tmp_tagdata+20,8);	/*票据号码*/
				GetOr_br_noByBr_no(g_pub_tx.tx_br_no,sign_brno);	
					vtcp_log("[%s][%d]******3",__FILE__,__LINE__);
				/***g_reply_int=Hv_poinfo_Sel(g_pub_tx.reply,&cHv_poinfo,"pay_br_no='%s' and po_no='%.8s'",sign_brno,fd123.po_no);
				if(g_reply_int=100)
				{
					
				}
				else if(g_reply_int){
						sprintf(acErrMsg,"未找到汇票记录!");
						set_zd_data(DC_GET_MSG,acErrMsg);
						strcpy(g_pub_tx.reply,"Z999");
						goto ErrExit;	
				}***/
					vtcp_log("[%s][%d]******3",__FILE__,__LINE__);
				if(!memcmp(hv_zf.cmtno,"124",3) &&(cHv_poinfo.po_sts[0]=='G'||cHv_poinfo.l_po_sts[0]=='G')){
						fd123.actype[0]='1'; /**判断汇票是否逾期**/
						char cAcno[33];
						memset(cAcno,0,sizeof(cAcno));
						pub_base_GetParm_Str(g_pub_tx.tx_br_no,PARM_HPTH,cAcno);
						memcpy(fd123.pay_ac_no,cAcno,32);
						strcpy(fd123.pay_name,"汇票逾期专用账号");
				}
				vtcp_log("%s,%d,汇票号码[%.8s],签发日期[%.8s],付款人账号[%.32s],付款人名称[%.60s]\n",__FILE__,__LINE__,fd123.po_no,fd123.o_pack_date,fd123.pay_ac_no,fd123.pay_name);
		}
		/** add by wangyongwei 20071012 取汇票号码传到前台**/
		fd123.operlevel[0]=hv_zf.operlevel[0];
		fd123.proc_sts[0]=hv_zf.hv_sts[0];
		set_zd_double("0390",hv_zf.tx_amt);   /* 交易金额 */
		set_zd_data("0890",hv_zf.note_type);
		set_zd_data("0580",hv_zf.note_no);
		set_zd_long("0450",hv_zf.trace_no);
		/*更新大额支付主表*/
		pub_base_full_space((char*)&fd123,sizeof(fd123));
		set_zd_data("1230",(char*)&fd123);
		vtcp_log("fd123=[%s]",(char*)&fd123);
		vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
		set_zd_data("0961",cNextVocnum);
		}
		else  
		{
			vtcp_log("%s,%d cmtno===[%s]",__FILE__,__LINE__,cCmtno);
			if(memcmp(cCmtno,"302",3)==0)
			{
				ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and otype='1' and pay_qs_no='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO,cTx_date,cTx_date);
			}
			else if(memcmp(cCmtno,"301",3)==0)
			{
			 	ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and otype='1' and pay_qs_no!='%s' and (req_date='%s' or res_date='%s') ",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO,cTx_date,cTx_date);
			}
			else
			{ 
				ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, "br_no='%s' and tx_date='%s' and orderno='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') and otype='1' ",cBr_no,cTx_date,cOrderno);
			}
			vtcp_log("sp9561 开始！[%s][%d]",__FILE__,__LINE__);
			vtcp_log("[%s][%d]",__FILE__,__LINE__);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"无此大额支付交易");
				WRITEMSG
				goto	ErrExit;
			}
			else if ( ret )
			{
				sprintf(acErrMsg,"查询大额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}		
			vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
			/*更新大额支付主表*/
			vtcp_log("更新大额支付主表[%s][%d]",__FILE__,__LINE__);
			 if(memcmp(cCmtno,"302",3)==0)
			{
					ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='1'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else if(memcmp(cCmtno,"301",3)==0)
			{
					ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='1'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and (lw_ind='2' or cmtno='301' or cmtno='302') and otype='1'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"更新大额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
			/**给FD123域数据**/
		  /**cmtno号码**/
			MEMCPY_DEBUG(fd123.cmtno	,hv_fzf.cmtno		,sizeof(fd123.cmtno));		
			/**o_cmtno**/		
			MEMCPY_DEBUG(fd123.o_cmtno	,hv_fzf.o_cmtno	,sizeof(fd123.cmtno));		
			/**发起行号 原发起行号 **/		
			MEMCPY_DEBUG(fd123.or_br_no	,hv_fzf.or_br_no,sizeof(fd123.or_br_no));	
			/**付款清算行行号**/		
			MEMCPY_DEBUG(fd123.pay_qs_no	,hv_fzf.pay_qs_no,sizeof(fd123.pay_qs_no));	
			/**发报中心代码**/		
			MEMCPY_DEBUG(fd123.sendco		,hv_fzf.sendco	,sizeof(fd123.sendco));
			/**接受清算行号**/				
			MEMCPY_DEBUG(fd123.cash_qs_no	,hv_fzf.cash_qs_no,sizeof(fd123.cash_qs_no));		
			/**接受行号(原发起行号)**/	
			MEMCPY_DEBUG(fd123.ac_br_no	,hv_fzf.ac_br_no,sizeof(fd123.ac_br_no));
			/**交易序号 撤销支付交易序号**/			
			MEMCPY_DEBUG(fd123.orderno	,hv_fzf.orderno	,sizeof(fd123.orderno));	
			/**附言**/		
			MEMCPY_DEBUG(fd123.brf		,hv_fzf.hv_brf	,sizeof(fd123.brf));	
			/**原支付交易序号**/			
			MEMCPY_DEBUG(fd123.o_orderno	,hv_fzf.o_orderno,sizeof(fd123.o_orderno));	
			/**非支付交易状态**/		
			fd123.proc_sts[0] = hv_fzf.hv_fzf_sts[0];	
			set_zd_data("0965",hv_fzf.hv_fzf_sts);
			vtcp_log("[%s][%d]proc_sts===[%c]",__FILE__,__LINE__,fd123.proc_sts[0]);	
			/**来往表示**/						
			fd123.lw_ind[0]	= hv_fzf.lw_ind[0];
			/**应答状态**/					
			fd123.resp[0]	= hv_fzf.res_sts[0];
			/**应答序号**/
			MEMCPY_DEBUG(fd123.cr_tx_num,hv_fzf.res_orderno,sizeof(hv_fzf.res_orderno)-1);
			/**应答行号**/
			MEMCPY_DEBUG(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
			vtcp_log("[%s][%d]fd123.cr_br_no=[%s]\n",__FILE__,__LINE__,fd123.cr_br_no);
			/**应答日期**/
			sprintf(cTmpday,"%8d",hv_fzf.res_date);
			MEMCPY_DEBUG(fd123.cr_date,cTmpday,sizeof(cTmpday)-1);
			/**原交易金额**/								
			set_zd_double("0390",hv_fzf.o_tx_amt);	
			if(memcmp(hv_fzf.cmtno,"725",sizeof(hv_fzf.cmtno)-1)==0)
			{
				FILE *fpw;
				char tmpstr[60];
				memset(tmpstr,0x0,sizeof(tmpstr));
				memset(&hv_addit,0x0,sizeof(struct hv_addit_c));
				iHvGetAddit(cTx_date,cTx_date,hv_fzf.addid,"000",cBuff,sizeof(cBuff)-1);
				vtcp_log("%s,%d 得到的附加域=[%s]",__FILE__,__LINE__,cBuff);
				vtcp_log("%s,%d aaaaaa=[%s]\n",__FILE__,__LINE__,cBuff);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+152,8);/*日期*/
				vtcp_log("%s,%d,tmpstr====[%s]",__FILE__,__LINE__,tmpstr);
				set_zd_data("0450",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+160,8);/*号码*/
				set_zd_data("0962",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+220,32);/*帐号*/
				set_zd_data("0963",tmpstr);
				memset(tmpstr,0x0,sizeof(tmpstr));
				MEMCPY_DEBUG(tmpstr,cBuff+252,60);/*姓名*/
				set_zd_data("0964",tmpstr);
				vtcp_log("%s,%d,接收流水号[%s]**********\n",__FILE__,__LINE__,hv_fzf.trace_no);
				memset(tmpstr,0x0,sizeof(tmpstr));
				sprintf(tmpstr,"%06d", hv_fzf.trace_no);
				set_zd_data("0040",tmpstr);
				/*ret=pub_base_AllDwnFilName(cFilename);
				if(ret)
				{
					vtcp_log("%s,%d 生成下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
				}
				fpw=fopen(cFilename,"w");
				if(fpw==NULL)
				{
				vtcp_log("%s,%d 打开下传文件错误 ret=%d ",__FILE__,__LINE__,ret);
				}
				HV_725_ADD hv_725;
				memset(&hv_725, 0 , sizeof(hv_725));
				MEMCPY_DEBUG((char *)&hv_725,cBuff, sizeof(hv_725));
				vtcp_log("%s,%d 写入附加域文件=[%s]",__FILE__,__LINE__,(char *)&hv_725);
				memset(cBuff, 0 , sizeof(cBuff));
				MEMCPY_DEBUG(cBuff,(char *)&hv_725,sizeof(hv_725));
				fprintf(fpw,"%s",cBuff);
				set_zd_data( DC_FILE_SND,"2");
				close(fpw);*/
			}
			MEMCPY_DEBUG(cTmpcmtno,hv_fzf.cmtno,sizeof(cTmpcmtno)-1);
			vtcp_log("[%s][%d]cTmpcmtno==[%s]",__FILE__,__LINE__,cTmpcmtno);
			vtcp_log("[%s][%d]hv_fzf.pay_qs_no==[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
			/**应答内容**/
			if(memcmp(cTmpcmtno,"314",sizeof(cTmpcmtno)-1)==0 || memcmp(cTmpcmtno,"302",sizeof(cTmpcmtno)-1)==0 ||memcmp(cTmpcmtno,"312",3)==0 \
			 || (memcmp(cTmpcmtno,"301",sizeof(cTmpcmtno)-1)==0 && memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,12)==0 ))
			{
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					vtcp_log("[%s][%d]hv_fzf.res_content==[%s]",__FILE__,__LINE__,hv_fzf.res_content);
  				vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
					deal_half_charcter(cNotpay_content);
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
			}
			else	/**申请内容**/	
			{
  				MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
					vtcp_log("[%s][%d]cNotpay_content==[%s]",__FILE__,__LINE__,cNotpay_content);
  		}
  		
			/* by liuyue 20060922*****处理查询时候原发起行与原接受行的问题*****/
			if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**发起行就是原发起行,即查询本行发出的交易**/
			{
				fd123.tx_type[0]	 =  'A';
			}
			else 
			fd123.tx_type[0]	=   'B';
			
			/*更新大额支付主表*/
			pub_base_full_space((char*)&fd123,sizeof(fd123));
			set_zd_data("1230",(char*)&fd123);
			vtcp_log("fd123=[%s]",(char*)&fd123);
			vtcp_log("cNextVocnum=[%s][%s][%d]",cNextVocnum,__FILE__,__LINE__);
			set_zd_data("0961",cNextVocnum);
		}	   
OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
        if(memcmp(g_pub_tx.reply,"0000",4)==0)
                MEMCPY_DEBUG(g_pub_tx.reply,"T063",4);
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
		
		
		

		printf("[%s]====\n",buf);
		size = strlen(buf);
		
		char cBuf[1024];
		
		memset(cBuf,0x00,sizeof(cBuf));


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
						cBuf[j] = buf[i];
						j++;
				}
		}

		MEMCPY_DEBUG(buf,cBuf,sizeof(cBuf));
		return 0;
}


