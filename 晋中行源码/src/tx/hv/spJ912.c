/*************************************************
* 文 件 名:  spJBBB.c
* 功能描述： 小额来帐打印
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
#include "lv_pub.h"
#include "lv_define.h"
#include "lv_pkgreg_c.h"
#include "lv_fd123.h"
#include "hv_define.h"
#include "find_debug.h"
#include "lv_pldfr_c.h" /*add by chenggx 2011-6-14*/
#include "lv_pldsr_c.h" /*add by chenggx 2011-6-14*/
spJ912()
{
	int ret=0,i=0;
	
	struct lv_pkgreg_c lv_pkgreg;
	struct lv_pldfr_c  lv_pldfr;
	struct lv_pldsr_c  lv_pldsr;
	struct hv_fzf_c hv_fzf;
	struct com_branch_c com_branch;
	LV_FD123_AREA lv_fd123;
	HV_FD123_AREA fd123;
	char cBr_no[6];/*机构号*/
	char cTel[5];  /*操作员号*/
	char cTx_date[9];/*交易日期*/
	char cOrderno[9];/*顺序号*/
	char cOrbrno[25];
	char cNextOrderno[9];/*下一个顺序号*/
	char cMinno[9]; /*最小序号*/
	char cTemno[17]; /*临时序号*/
	char cTxstat[2];/*交易状态*/
	char cPkgno[4];/*汇票类型*/
	char cVtype[4];/*凭证类型*/
	char cVocnum[17];/*凭证号码*/
	char cNextVocnum[17];/*下一个凭证号码*/
	char wherelist[1024];  /**查询条件**/
	char cZf_type[2];
	char cNotpay_content[1024];
	char cPayqsno[25];

	memset(cNotpay_content,0x00,sizeof(cNotpay_content));
	memset(&lv_fd123, ' ',sizeof(lv_fd123));
	memset(&fd123, ' ',sizeof(fd123));
	memset(cBr_no     ,0, sizeof(cBr_no));
	memset(cTel     ,0, sizeof(cTel));
	memset(cTx_date   ,0, sizeof(cTx_date));	
	memset(cVtype     ,0, sizeof(cVtype));
	memset(cTxstat    ,0, sizeof(cTxstat));
	memset(cPkgno     ,0, sizeof(cPkgno));
	memset(cVocnum    ,0, sizeof(cVocnum));
	memset(cOrderno   ,0, sizeof(cOrderno));
	memset(cOrbrno    ,0, sizeof(cOrbrno));
	memset(cPayqsno, 0 , sizeof(cPayqsno));
	memset(cTemno     ,0,sizeof(cTemno));
	memset(&lv_pkgreg,0x00,sizeof(struct lv_pkgreg_c));
	memset(&lv_pldfr,0,sizeof(lv_pldfr));
	memset(&lv_pldsr,0,sizeof(lv_pldsr));
	memset(&hv_fzf,0x0,sizeof(struct hv_fzf_c));
	memset(cNextOrderno,0,sizeof(cNextOrderno));
	memset(cNextVocnum    ,0, sizeof(cNextVocnum));
	memset(&com_branch,0x00,sizeof(struct com_branch_c));
	memset(cZf_type,0,sizeof(cZf_type));
	vtcp_log("spJBBB开始！[%s][%d]",__FILE__,__LINE__);
	vtcp_log("spJBBB开始！cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	
	/**初始变量**/
	pub_base_sysinit();
	get_zd_data("0030",cBr_no);
	get_zd_data("0070",cTel);
	get_zd_data("0440",cTx_date);
	/*get_fd_data("0290",cTemno);*/
	get_fd_data("0580",cTemno);
	vtcp_log("0580=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	get_zd_data("0230",cPkgno);
	get_zd_data("0690",cTxstat);
	get_zd_data("0900",cVtype);
	get_zd_data("0600",cVocnum);
	get_zd_data("0720",cZf_type);
	get_zd_data("0300",cPayqsno);
	get_zd_data("0310",cOrbrno);
	vtcp_log("0310=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	pub_base_strpack(cTemno);
	i=strlen(cTemno);
		vtcp_log("cTemno=[%s][%s][%d]",cTemno,__FILE__,__LINE__);
	/*MEMCPY_DEBUG(cOrderno+8-i,cTemno,sizeof(cTemno)-1);*/
	MEMCPY_DEBUG(cOrderno,cTemno,8);
		vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	MEMCPY_DEBUG(cNextVocnum,cVocnum,sizeof(cNextVocnum)-1);
	vtcp_log("AP MSG: 开始小额来帐凭证打印程序![%s][%d]",__FILE__,__LINE__);
	vtcp_log("cOrderno=[%s][%s][%d]",cOrderno,__FILE__,__LINE__);
	vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	pub_base_strpack(cOrbrno);
	/***********检查凭证*************/
	if(cZf_type[0]=='0')
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
		vtcp_log("cOrbrno=[%s][%s][%d]",cOrbrno,__FILE__,__LINE__);
	}
	if(cZf_type[0]=='0')
	{
		/*ret =Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
		 in_date 改为 wt_date*/
		ret =Lv_pkgreg_Sel(g_pub_tx.reply,&lv_pkgreg, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
		vtcp_log("spJBBB 开始！[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"无此小额支付交易");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}		
		vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
		/*更新大额支付主表*/
		vtcp_log("更新小额支付主表[%s][%d]",__FILE__,__LINE__);
		/***检查机构是否为清算中心****/
		ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		if(com_branch.br_type[0]=='1')
		{
			/****Modified by SSH,2007.5.16,sqprtflag->qsprtflag****/
			/**ret = sql_execute("update lv_pkgreg set qsprtflag= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
			in_date 改为 wt_date*/
			ret = sql_execute("update lv_pkgreg set qsprtflag= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
			if (ret)
			{
				sprintf(acErrMsg,"更新小额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			/**ret = sql_execute("update lv_pkgreg set brprtflag= '1',qsprtflag='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2'",cBr_no,cTx_date,cOrderno);
			in_date 改为 wt_date**/
			ret = sql_execute("update lv_pkgreg set brprtflag= '1',qsprtflag='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and or_br_no='%s'",cBr_no,cTx_date,cOrderno,cOrbrno);
			if (ret)
			{
				sprintf(acErrMsg,"更新小额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		/**给lv_fd123域数据**/
		/*Package编号*/
		MEMCPY_DEBUG(lv_fd123.pkgno,lv_pkgreg.pkgno,sizeof(lv_fd123.pkgno));
		vtcp_log("[%s][%d]pkgno=[%s]",__FILE__,__LINE__,lv_pkgreg.pkgno); 	
		/**发起行号 原发起行号 */
		MEMCPY_DEBUG(lv_fd123.or_br_no        ,lv_pkgreg.or_br_no        ,sizeof(lv_fd123.or_br_no        ));
		vtcp_log("[%s][%d]or_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.or_br_no);
		
		/**付款人开户行**/
		MEMCPY_DEBUG(lv_fd123.pay_opn_br_no   ,lv_pkgreg.pay_opn_br_no   ,sizeof(lv_fd123.pay_opn_br_no   ));
		vtcp_log("[%s][%d]pay_opn_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_opn_br_no);
		/**付款清算行行号**/		
		MEMCPY_DEBUG(lv_fd123.pay_qs_no	,lv_pkgreg.pay_qs_no,sizeof(lv_fd123.pay_qs_no));	
		/**付款人帐号(原收款人帐号)**/
		MEMCPY_DEBUG(lv_fd123.pay_ac_no       ,lv_pkgreg.pay_ac_no       ,sizeof(lv_fd123.pay_ac_no       ));
		vtcp_log("[%s][%d]pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_ac_no);
		MEMCPY_DEBUG(lv_fd123.txnum, lv_pkgreg.txnum,sizeof(lv_fd123.txnum));
		vtcp_log("[%s][%d]pay_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.txnum);
		/**付款人名称(原收款人名称)**/
		MEMCPY_DEBUG(lv_fd123.pay_name        ,lv_pkgreg.pay_name        ,sizeof(lv_fd123.pay_name        ));
		vtcp_log("[%s][%d]pay_name=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_name);
  	
		/**付款人地址(原收款人地址)**/
		MEMCPY_DEBUG(lv_fd123.pay_addr        ,lv_pkgreg.pay_addr        ,sizeof(lv_fd123.pay_addr        ));
		vtcp_log("[%s][%d]pay_addr=[%s]",__FILE__,__LINE__,lv_pkgreg.pay_addr);
  	
		/**接受清算行号**/
		MEMCPY_DEBUG(lv_fd123.cash_qs_no      ,lv_pkgreg.cash_qs_no      ,sizeof(lv_fd123.cash_qs_no      ));
		vtcp_log("[%s][%d]cash_qs_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_qs_no);
  	
		/**接受行号(原发起行号)**/
		MEMCPY_DEBUG(lv_fd123.ac_br_no        ,lv_pkgreg.ac_br_no        ,sizeof(lv_fd123.ac_br_no        ));
		vtcp_log("[%s][%d]ac_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.ac_br_no);
		/**收款人开户行号**/
		MEMCPY_DEBUG(lv_fd123.cash_opn_br_no  ,lv_pkgreg.cash_opn_br_no  ,sizeof(lv_fd123.cash_opn_br_no  ));
		vtcp_log("[%s][%d]cash_opn_br_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_opn_br_no);
		/**收款人帐号,原付款人帐号**/
		MEMCPY_DEBUG(lv_fd123.cash_ac_no      ,lv_pkgreg.cash_ac_no      ,sizeof(lv_fd123.cash_ac_no      ));
		vtcp_log("[%s][%d]cash_ac_no=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_ac_no);
  	
		/**收款人姓名,原付款人姓名**/
		MEMCPY_DEBUG(lv_fd123.cash_name       ,lv_pkgreg.cash_name       ,sizeof(lv_fd123.cash_name       ));
		vtcp_log("[%s][%d]cash_name=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_name);
		
		/**收款人地址**/
		MEMCPY_DEBUG(lv_fd123.cash_addr       ,lv_pkgreg.cash_addr       ,sizeof(lv_fd123.cash_addr       ));
		vtcp_log("[%s][%d]cash_addr=[%s]",__FILE__,__LINE__,lv_pkgreg.cash_addr);
		
		/**业务种类**/	
		MEMCPY_DEBUG(lv_fd123.yw_type         ,lv_pkgreg.lv_yw_ind         ,sizeof(lv_fd123.yw_type         ));
		vtcp_log("[%s][%d]yw_type=[%s]",__FILE__,__LINE__,lv_pkgreg.lv_yw_ind);
		
		/*交易序号 撤销支付交易序号*/
		MEMCPY_DEBUG(lv_fd123.orderno         ,lv_pkgreg.orderno         ,sizeof(lv_fd123.orderno         ));
		vtcp_log("[%s][%d]orderno=[%s]",__FILE__,__LINE__,lv_pkgreg.orderno);
		
		/**附言**/
		MEMCPY_DEBUG(lv_fd123.brf             ,lv_pkgreg.lv_brf          ,sizeof(lv_fd123.brf             ));
		vtcp_log("[%s][%d]brf=[%s]",__FILE__,__LINE__,lv_pkgreg.lv_brf);
		
		/**原支付交易序号**/
		MEMCPY_DEBUG(lv_fd123.o_orderno       ,lv_pkgreg.o_orderno       ,sizeof(lv_fd123.o_orderno       ));
		/**手续费金额*/
		/*dot_dbl2str(&lv_pkgreg.chrg_amt,0,sizeof(lv_fd123.chrg_amt),0,&lv_fd123.chrg_amt);*/
		dot_dbl2str(lv_pkgreg.chrg_amt,sizeof(lv_fd123.chrg_amt),0,lv_fd123.chrg_amt);
		vtcp_log("[%s][%d]chrg_amt=[%s]",__FILE__,__LINE__,lv_pkgreg.chrg_amt);
  	
		/**委托日期*/
		sprintf(lv_fd123.wt_date,"%08d",lv_pkgreg.wt_date);
		vtcp_log("[%s][%d] lv_pkgreg.wt_date = [%d] wt_date=[%s]",__FILE__,__LINE__,lv_pkgreg.wt_date,lv_fd123.wt_date);
		/**回执状态**/	
		MEMCPY_DEBUG(lv_fd123.rcp_sts         , lv_pkgreg.rcpstat          ,sizeof( lv_fd123.rcp_sts ));
		/**来帐包号**/
		MEMCPY_DEBUG(lv_fd123.pack_id         , lv_pkgreg.packid          ,sizeof( lv_fd123.pack_id ));
		vtcp_log("[%s][%d]packid=[%s]",__FILE__,__LINE__,lv_pkgreg.packid);
		
		lv_fd123.operlevel[0]=lv_pkgreg.operlevel[0];
		lv_fd123.lv_sts[0]=lv_pkgreg.lv_sts[0];
		set_zd_double("0390",lv_pkgreg.tx_amt);   /* 交易金额 */
		
		/*beg add by chenggx 2011-6-14 20:25:55*/
		vtcp_log("[%s][%d]pkgno=[%s],txnum=[%s]",__FILE__,__LINE__,lv_pkgreg.pkgno,lv_pkgreg.txnum);
		vtcp_log("[%s][%d]PKGNO_TYXX=[%s],TXNUM_DF=[%s],TXNUM_DS=[%s]",__FILE__,__LINE__,PKGNO_TYXX,TXNUM_DF,TXNUM_DS);
		if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DF,5)==0){/*代付*/
			vtcp_log("[%s][%d] 代付 我看到了 哈哈哈 ^_^O(∩_∩)O哈哈~",__FILE__,__LINE__);
			ret = Lv_pldfr_Sel(g_pub_tx.reply,&lv_pldfr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"查询批量代付表失败!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "9999" );
				goto	ErrExit;
			}
			memcpy(lv_fd123.cash_ac_no      ,lv_pldfr.pay_ac_no      ,strlen(lv_pldfr.pay_ac_no  ));
			vtcp_log("[%s][%d]lv_fd123.cash_ac_no=[%s] ",__FL__,lv_fd123.cash_ac_no );
			memcpy(lv_fd123.cash_name       ,lv_pldfr.pay_name       ,strlen(lv_pldfr.pay_name   ));
			vtcp_log("[%s][%d]lv_fd123.cash_name=[%s] ",__FL__,lv_fd123.cash_name );
			set_zd_double("0390",lv_pldfr.totamt);   /* 交易金额 */
		}else if(memcmp(lv_pkgreg.pkgno,PKGNO_TYXX,3)==0 && memcmp(lv_pkgreg.txnum,TXNUM_DS,5)==0){/*代收*/
			vtcp_log("[%s][%d] 代收 我看到了 哈哈哈 ^_^O(∩_∩)O哈哈~",__FILE__,__LINE__);
			ret = Lv_pldsr_Sel(g_pub_tx.reply,&lv_pldsr, " packid='%s' and orderno='%s' and wt_date=%ld and tx_mx_no='00000000' ",lv_pkgreg.packid,lv_pkgreg.orderno,lv_pkgreg.wt_date);
			if ( ret==100 )
			{
				sprintf(acErrMsg,"查询批量代收表失败!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "9999" );
				goto	ErrExit;
			}
			memcpy(lv_fd123.cash_ac_no      ,lv_pldsr.cash_ac_no      ,strlen(lv_pldsr.cash_ac_no  ));
			vtcp_log("[%s][%d]lv_fd123.cash_ac_no=[%s] ",__FL__,lv_fd123.cash_ac_no );
			memcpy(lv_fd123.cash_name       ,lv_pldsr.cash_name       ,strlen(lv_pldsr.cash_name   ));
			vtcp_log("[%s][%d]lv_fd123.cash_name=[%s] ",__FL__,lv_fd123.cash_name );
			set_zd_double("0390",lv_pldsr.totamt);   /* 交易金额 */
		}
		/*end add by chenggx 2011-6-14 20:25:55*/
	
		pub_base_full_space((char*)&lv_fd123,sizeof(lv_fd123));
		set_zd_data("1230",(char*)&lv_fd123);
		vtcp_log("lv_fd123=[%s]",(char*)&lv_fd123);
		set_zd_data("0280",cNextOrderno);
		set_zd_data("0961",cNextVocnum);
	}
	else
	{/* 小额的情况 */
		/*针对传来的数据检查科目要查询的目的报文 */
		vtcp_log("%s,%d ---pay_qs_no==[%s],pkgno=[%s]",__FILE__,__LINE__,cPayqsno,cPkgno);
		if(memcmp(cPkgno,"302",3)==0 || memcmp(cPkgno,"301",3)==0 )
		{
			if(memcmp(cPayqsno,HV_QS_BR_NO,12)==0)
			{
				memcpy(cPkgno,"302",3);
			}
		}
		if(memcmp(cPkgno,"302",3)==0)
		{
/****修改为wt_date 由于打印的时间问题 by  jk 20110426
 ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cO
rderno,HV_QS_BR_NO);
*****/


			ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and wt_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
		}
		else if(memcmp(cPkgno,"301",3)==0)
		{
			ret=Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"br_no='%s' and wt_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
		}
		else
		{
			ret =Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf, "br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
		}
		vtcp_log("spJBBB 开始！[%s][%d]",__FILE__,__LINE__);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if ( ret==100 )
		{
			sprintf(acErrMsg,"无此小额支付交易");
			WRITEMSG
			goto	ErrExit;
		}
		else if ( ret )
		{
			sprintf(acErrMsg,"查询小额支付交易基本信息出错[%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("查询成功！  [%s][%d]",__FILE__,__LINE__);
		/*更新大额支付主表*/
		vtcp_log("更新小额支付主表[%s][%d]",__FILE__,__LINE__);
		/***检查机构是否为清算中心****/
		vtcp_log("[%s][%d]brno=[%s]",__FILE__,__LINE__,cBr_no);
		ret=Com_branch_Sel(g_pub_tx.reply,&com_branch,"br_no='%s'",cBr_no);
		vtcp_log("[%s][%d]",__FILE__,__LINE__);
		if(com_branch.br_type[0]=='1')
		{
			if(memcmp(cPkgno,"302",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
			}
			else if(memcmp(cPkgno,"301",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);
			}
			else
			{
		   	ret = sql_execute("update hv_fzf set hv_prt_ind='1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"更新小额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		else
		{
			if(memcmp(cPkgno,"302",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else if(memcmp(cPkgno,"301",3)==0)
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and tx_date='%s' and orderno='%s' and pay_qs_no!='%s' and otype='2'",cBr_no,cTx_date,cOrderno,HV_QS_BR_NO);	
			}
			else
			{
				ret = sql_execute("update hv_fzf set hv_prt_ind= '1' where br_no='%s' and wt_date='%s' and orderno='%s' and lw_ind='2' and otype='2'",cBr_no,cTx_date,cOrderno);
			}
			if (ret)
			{
				sprintf(acErrMsg,"更新小额支付交易基本信息出错[%d]",ret);
				WRITEMSG
				goto ErrExit;
			}
		}
		
		/**给lv_fd123域数据**/
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
		sprintf(fd123.cr_date,"%8d",hv_fzf.res_date);
		/**原交易金额**/								
		set_zd_double("0390",hv_fzf.o_tx_amt);
		/**应答内容**/
		vtcp_log("%s,%d cmtno==='%s'",__FILE__,__LINE__,fd123.cmtno);
		
		if(memcmp(fd123.cmtno,"314",3)==0 || memcmp(fd123.cmtno,"302",3)==0 ||memcmp(fd123.cmtno,"312",3)==0)
		{
			/* 判断要打印内容 */
			if(memcmp(fd123.cmtno,"302",3)==0)
			{
				vtcp_log("%s,%d pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
				if(memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,sizeof(hv_fzf.pay_qs_no)-1)==0)
				{
					/* 付款清算行是本行的情况打印查复内容 */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
				else
				{/* 打印查询内容 */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
			}
			else
			{
				MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
				deal_half_charcter(cNotpay_content);					
				MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
			}
		}
		else	/**申请内容**/
		{
			if(memcmp(fd123.cmtno,"301",3)==0)
			{
				vtcp_log("%s,%d pay_qs_no=[%s]",__FILE__,__LINE__,hv_fzf.pay_qs_no);
				if(memcmp(hv_fzf.pay_qs_no,HV_QS_BR_NO,sizeof(hv_fzf.pay_qs_no)-1)==0)
				{
					/* 付款清算行是本行的情况打印查复内容 */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.res_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
				else
				{/* 打印查询内容 */
					MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.res_content)-1);
					deal_half_charcter(cNotpay_content);					
					MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
				}
			}
			else
			{
				MEMCPY_DEBUG(cNotpay_content,hv_fzf.req_content,sizeof(hv_fzf.req_content)-1);
				deal_half_charcter(cNotpay_content);					
				MEMCPY_DEBUG(fd123.notpay_content,cNotpay_content,sizeof(cNotpay_content));
			}
		}
  	
		/* by liuyue 20060922*****处理查询时候原发起行与原接受行的问题*****/
		if(!memcmp(hv_fzf.o_or_br_no,hv_fzf.or_br_no,sizeof(hv_fzf.or_br_no)-1))/**发起行就是原发起行,即查询本行发出的交易**/
		{
			fd123.tx_type[0]	 =  'A';
		}
		else 
			fd123.tx_type[0]	=   'B';
  	vtcp_log("%s,%d content=====",__FILE__,__LINE__);
		vtcp_log("[%s]",fd123.notpay_content);
  	
		/**********add end by liuyue 借用了hv_fzf.tx_type**************/
			
		MEMCPY_DEBUG(fd123.cr_br_no,hv_fzf.res_br_no,sizeof(hv_fzf.res_br_no)-1);
		set_zd_double("0440",hv_fzf.wt_date);	
		set_zd_double("0450",hv_fzf.o_wt_date);	
		pub_base_full_space((char*)&fd123,sizeof(fd123));
		set_zd_data("1230",(char*)&fd123);
		vtcp_log("lv_fd123=[%s]",(char*)&fd123);
		set_zd_data("0280",cNextOrderno);
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


