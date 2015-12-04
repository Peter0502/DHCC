#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "lv_define.h"
#include "hv_define.h"
#include "exitlvpack.h"
#include "lv_pkgreg_c.h"
#include "lv_sysctl_c.h"
#include "lv_chk842_d_c.h"
#include "lv_chk842_m_c.h"
#include "lv_chk851_d_c.h"
#include "lv_chk851_m_c.h"
#include "lv_sendlist_c.h"
#include "public.h"
#include "lv_c554r_c.h"
#include "hv_orno_brno_c.h"
#include "lv_c552r_c.h"
#include "hv_fzfhtr_c.h"
#include "lv_pldsr_c.h"
#include "lv_pldscx_c.h"
#include "lv_pldfr_c.h"
#include "lv_pldsfqugx_c.h"
#include "lv_dsfsendlist_c.h"
#include "lv_chgbank_c.h"
#include "lv_oths_c.h"
#include "lv_c503r_c.h"
#include "dd_mst_c.h"

#define MXHD850_MAX_ZCCC 6
extern char *pcLvGetDay_work();/* add by lhf0421 */
extern char *cGetSysDate();/**add by xyy 0908**/
extern char* pcLvGetDay();
extern int Ins_cmt554_to_table(TLVPACKAGE *lv);
extern int Ins_cmt552_to_table(TLVPACKAGE *lv);
extern char * pcLvDesPswdCode(char *code);
extern void vGet_Hv_Qs_no(char * qs_no);
extern char* pcLvGetRecvno();

/***************************************************************************/
/* 文件名称: lv_deal_tyxxlz(TLVPACKAGE *wp_lvpack)                         */
/* 功能描述: 通用信息来帐处理                                         */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_tyxxlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt,index;
	TLVPACKAGE subpack;
	NOTPAY_IN_AREA notpayin;
	TLVPACKAGE return40506Pack;  /*add by xyy for NEWYX 2006-12-27 10:31用来返回40506包*/
	struct lv_lbctl_c wd_lv_lbctl;
	char pcPackday[9];
	char pcPackid[9];
	char cTmporderno[9];
	char tx_br_no[13];
	char tel[7];  

	/*********数据初始化******************/
	vtcp_log("%s,%d,lv_deal_tyxxlz() 开始!!!  [%.3s]",__FILE__,__LINE__, wp_lvpack->CMTCode);
	vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));
	int  iTmpIndex = 0;    
	long  iSubpackIndex = 0;   /* 包中子报文序号 */   

	get_zd_long("0700",&iSubpackIndex);
	vtcp_log("[%s][%d] chkflg=[%d]   包中子报文序号 iSubpackIndex=[%d]",__FILE__,__LINE__,chkflg,iSubpackIndex);

	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(pcPackday,'\0',sizeof(pcPackday));
	memset(pcPackid,'\0',sizeof(pcPackid));
	memset(cTmporderno, 0 , sizeof(cTmporderno));
	/****检查是否通用信息来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_TYXX,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是通用信息来账\n",__FILE__,__LINE__, wp_lvpack->CMTCode);
		sprintf(acErrMsg,"来账不是通用信息报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
	/******************************************************************/
	/*   LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用 */
	/*   lv_destroy                                                   */
	/******************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));  

	lv_pkg_getHeaderTag(wp_lvpack,"30E",pcPackday);
	vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
	/*
	lv_pkg_getHeaderTag(wp_lvpack,"0BD",pcPackid);
	*/
	vtcp_log("]%s][%d]pcPackday==[%s]",__FILE__,__LINE__,pcPackday);
	lv_pkg_getHeaderTag(wp_lvpack,"0BD",cTmporderno);
	lv_orderno_add0(cTmporderno, sizeof(pcPackid)-1, pcPackid);
	vtcp_log("]%s][%d]pcPackid==[%s]",__FILE__,__LINE__,pcPackid);
	/****得到明细业务的笔数****/
	/*modify by xyy for NEWYX 2006-12-27 10:31**/
	recnt=lv_count_recnt(wp_lvpack);
	vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
	vtcp_log("[%s][%d]  得到明细业务的笔数 recnt=[%d]",__FILE__,__LINE__,recnt);  
	for (index=0;index<recnt;index++)
	{
		lv_create(&subpack);   /*modify by xyy  for NEWYX 2006-12-27 10:31**/
		vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
		iRc=lv_fetchpkg(wp_lvpack,&subpack);
		vtcp_log("%s,%d,Gethere,pkgType=[%d],isMulti=[%d]",__FILE__,__LINE__,wp_lvpack->pkgType,wp_lvpack->isMulti);
		vtcp_log("[%s][%d] xyy  iRc=[%d]",__FILE__,__LINE__,iRc);
		if (iRc<0)
		{
			vtcp_log("%s,%d,提取报文时出错\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"提取报文时出错");WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}
		else if (iRc>0)
		{
			vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
			set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
			lv_destroy(&subpack);
			return 0;
		}
		/*end xyy for NEWYX 2006-12-27 10:31**/
		vtcp_log("[%s][%d]  index=[%d]",__FILE__,__LINE__,index);  
		memset(&notpayin,'\0',sizeof(notpayin));
		iRc=lv_fetchcmt_to_notpayin(&subpack,&notpayin);
		if (iRc<0)
		{
			vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
			sprintf(acErrMsg,"根据子报文填写notpayin失败");
			WRITEMSG
			lv_destroy(&subpack);
			return(-1);
		}
		vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,notpayin.opkgno);
		memcpy(notpayin.opackid,pcPackid,sizeof(notpayin.opackid));  
		vtcp_log("%s,%d,Gethere!PKGNO=%.3s\n",__FILE__,__LINE__,notpayin.pkgno);

		/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
		memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
		pubf_lv_GetBr_noTel(tx_br_no);

		vtcp_log("[%s][%d]  notpayin_to_table [%d]",__FILE__,__LINE__,index);  
		iRc=notpayin_to_table(&notpayin);
		if (iRc)
		{
			vtcp_log("%s,%d,插入tyxx登记簿错误\n",__FILE__,__LINE__);
			sprintf(acErrMsg,"插入tyxx登记簿错误");
			WRITEMSG  
				lv_destroy(&subpack);          
			return(-1);
		}
		/*处理附加域中的代收付业务*/
		lv_get_tag(&subpack,"0BC",cTmporderno);
		cTmporderno[8]='\0';
		printf("%s,%d 开始处理附加域中的代收付业务%s\n",__FILE__,__LINE__,cTmporderno);
		iRc = lv_dsf_deal(&subpack,pcPackid,cTmporderno,pcPackday,&return40506Pack,index+1,recnt);
		if (iRc<0)
		{
			vtcp_log("%s,%d,处理附加域中的代收付业务出错\n",__FILE__,__LINE__);
			sprintf(acErrMsg,"处理附加域中的代收付业务出错");
			WRITEMSG  
				lv_destroy(&subpack);  
			return(-1);
		}
		printf("%s,%d 处理附加域中的代收付业务完毕%s\n",__FILE__,__LINE__,cTmporderno);
		lv_destroy(&subpack);
	}
	vtcp_log("[%s][%d]郁闷中。。。。。。\n",__FILE__,__LINE__);
	/****子报文循环完毕****/
	/******插入来包控制表************/
	iRc=lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
	if (iRc)
	{
		vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"产生来包控制表记录错误");
		WRITEMSG
			return(-1);
	}
	iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
	if (iRc!=DB_OK)
	{
		vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"插入来包控制表出错");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
	set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/

	return(0);
}

/***************************************************************************/
/* 文件名称: lv_deal_ssxxlz(TLVPACKAGE *wp_lvpack)                         */
/* 功能描述: 实时信息来帐处理                                         */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
/****NEWTCTD:实时信息账户查询自动应答(在王永卫基础上修改)****/
int lv_deal_ssxxlz(TLVPACKAGE *wp_lvpack, int chkflg)
{
	int iRc=0;
	int recnt=0,index=0;
	TLVPACKAGE subpack;
	NOTPAY_IN_AREA notpayin;
	struct lv_lbctl_c wd_lv_lbctl;
	char pcPackday[9];
	char pcPackid[9];
	char cTmporderno[9];
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(cTmporderno, 0 , sizeof(cTmporderno));
	memset(&wd_lv_lbctl,'\0',sizeof(wd_lv_lbctl));
	memset(pcPackday,'\0',sizeof(pcPackday));
	memset(pcPackid,'\0',sizeof(pcPackid));
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否实时信息来账报文****/
	if (memcmp(wp_lvpack->CMTCode,PKGNO_SSXX,sizeof(wp_lvpack->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是实时信息来账\n",__FILE__,__LINE__,  wp_lvpack->CMTCode);
		sprintf(acErrMsg,"来账不是实时信息报文");
		WRITEMSG
			return(-1);
	}
	/****LVPACKAGE的操作过程可能动态分配内存,一定要确保任何分支都调用*/
	/*   lv_destroy                                                  */
	/*****************************************************************/
	memset(&subpack,0,sizeof(TLVPACKAGE));  
	lv_create(&subpack);
	vtcp_log("[%s][%d]========\n",__FILE__,__LINE__);
	iRc=lv_fetchpkg(wp_lvpack,&subpack);
	lv_pkg_getHeaderTag(wp_lvpack,"30E",pcPackday);
	/**
	lv_pkg_getHeaderTag(wp_lvpack,"0BD",pcPackid);
	**/
	lv_pkg_getHeaderTag(wp_lvpack,"0BD",cTmporderno);
	lv_orderno_add0(cTmporderno, sizeof(pcPackid)-1, pcPackid);

	/****得到明细业务的笔数****/
	vtcp_log("[%s][%d]========\n",__FILE__,__LINE__);
	recnt=lv_count_recnt(&subpack);
	for (index=0;index<recnt;index++)
	{
		memset(&notpayin,'\0',sizeof(notpayin));

		/**TCTD:add by wangyongwei 20070818***/
		memcpy(notpayin.opackday,pcPackday,sizeof(pcPackday)-1);
		memcpy(notpayin.opackid,pcPackid,sizeof(pcPackid)-1);
		/**TCTD:add by wangyongwei 20070818***/

		vtcp_log("[%s][%d]========notpayin.pkgno=[%s]\n",__FILE__,__LINE__,notpayin.pkgno);
		iRc=lv_fetchcmt_to_notpayin(&subpack,&notpayin);
		if (iRc<0)
		{
			vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
			sprintf(acErrMsg,"根据子报文填写notpayin失败");
			WRITEMSG
				lv_destroy(&subpack);
			return(-1);
		}
		lv_pkg_getHeaderTag(wp_lvpack,"C36",notpayin.payqsactno);
		lv_pkg_getHeaderTag(wp_lvpack,"C37",notpayin.cashqsactno);
		vtcp_log("[%s][%d]Gethere11[%.12s]\n",__FILE__,__LINE__,notpayin.payqsactno);
		

		memcpy(notpayin.opackid,pcPackid,sizeof(notpayin.opackid));
		vtcp_log("[%s][%d]========notpayin.pkgno=[%s]\n",__FILE__,__LINE__,notpayin.pkgno);

		/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
		memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
		pubf_lv_GetBr_noTel(tx_br_no);

		iRc=notpayin_to_table(&notpayin);
		if (iRc)
		{
			vtcp_log("%s,%d,插入ssxx登记簿错误\n",__FILE__,__LINE__);
			lv_destroy(&subpack);      
			return(-1);
		}
	} /****子报文循环完毕****/
	lv_destroy(&subpack);
	vtcp_log("[%s][%d]========\n",__FILE__,__LINE__);

	/******插入来包控制表************/
	iRc=lv_setlbctl_from_pack(wp_lvpack,&wd_lv_lbctl);
	if (iRc)
	{
		vtcp_log("%s,%d,产生来包控制表记录错\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"产生来包控制表记录错误");
		WRITEMSG
			return(-1);
	}  
	vtcp_log("[%s][%d]========\n",__FILE__,__LINE__);
	iRc=Lv_lbctl_Ins(wd_lv_lbctl, g_pub_tx.reply);
	if (iRc!=DB_OK)
	{
		vtcp_log("%s,%d,插入来包控制表错[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"插入来包控制表出错");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,提取报文结束\n",__FILE__,__LINE__);
	set_zd_data("0500","1");     /*'1'=PKG包已处理完毕*/
	if (!memcmp(notpayin.txnum,TXNUM_ZHCX,TXNUM_LEN))
	{
		NOTPAY_IN_AREA notpayin_hz;
		int iOrderno=0,ret=0,iPackid=0,flag=0;
		char cOrderno[9];
		char buffer[1024];
		char cActno[33];
		char cAddid[9];
		char cAddDay[9];
		char cPackDay[9];
		char cPackid[9];
		/*ZHCX_ADD  cZhcx_add; */
		ZHCX_ADD_PWD16 cZhcx_add;
		ZHCXYD_ADD  cZhcxyd_add;
		TD_ADD_AREA  cTd_add;
		PAY_IN_AREA  cWp_payin;
		struct lv_pkgreg_c  lv_pkgreg_cxyd;
		struct mdm_ac_rel_c  cMdmacrel;
		struct dd_mst_c  cDdmst;
		struct lv_wbctl_c  lv_wbctl;
		memset(cAddid,'\0',sizeof(cAddid));
		memset(cAddDay,'\0',sizeof(cAddDay));
		memset(cPackDay,'\0',sizeof(cPackDay));
		memset(cPackid,'\0',sizeof(cPackid));
		memset(&lv_pkgreg_cxyd,0x00,sizeof(lv_pkgreg_cxyd));
		memset(&notpayin_hz,0x00,sizeof(notpayin_hz));
		memset(buffer,0x00,sizeof(buffer));
		memset(&cZhcx_add,' ',sizeof(ZHCX_ADD));
		memset(&cZhcxyd_add,' ',sizeof(ZHCXYD_ADD));
		memset(&cTd_add,' ',sizeof(TD_ADD_AREA));
		memset(&cWp_payin,0x00,sizeof(PAY_IN_AREA));
		memset(&cMdmacrel,0x00,sizeof(cMdmacrel));
		memset(&cDdmst,0x00,sizeof(cDdmst));
		memset(&lv_wbctl,0x00,sizeof(lv_wbctl));
		memset(cActno,0x00,sizeof(cActno));
		memcpy(cAddid,notpayin.addid,sizeof(cAddid)-1);
		memcpy(cAddDay,notpayin.txday,sizeof(cAddDay)-1);
		ret=iLvGetAddit_Notpay(cAddDay,cAddid,"000",buffer,0);
		if(ret){
			vtcp_log("%s,%d,查询实时信息附加域错误![%d]\n",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"查询实时信息附加域错误!ret[%d]",ret);
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
		    strcpy(g_pub_tx.reply,"D103");
			return(-1);
		}
		memcpy(&cZhcx_add,buffer,sizeof(ZHCX_ADD));
		memcpy(cActno,cZhcx_add.actno,sizeof(cActno)-1);
		pub_base_strpack(cActno);
		memcpy(cZhcxyd_add.opackday,notpayin.opackday,sizeof(cZhcxyd_add.opackday));/**原包日期**/
		memcpy(cZhcxyd_add.opackid,notpayin.opackid,sizeof(cZhcxyd_add.opackid));/**原包序号**/
		memcpy(cZhcxyd_add.owtday,notpayin.txday,sizeof(cZhcxyd_add.owtday));/**原委托日期**/
		memcpy(cZhcxyd_add.oorderno,notpayin.orderno,sizeof(cZhcxyd_add.oorderno));/**原信息序号**/
		memcpy(cZhcxyd_add.actno,cZhcx_add.actno,sizeof(cZhcxyd_add.actno));/*原查询帐号*/
		cZhcxyd_add.qrytype=cZhcx_add.qtype;/**原查询类型**/
		ret=pub_base_old_acno(cActno);
		if(ret){
			vtcp_log("%s,%d,新旧帐号转换失败或帐号不存在!\n",__FILE__,__LINE__);
			flag=1;/**账号不存在**/
			ret=0;
		}
		pub_base_strpack(cActno);
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&cMdmacrel," ac_no='%s'",cActno);
		if(ret){
			vtcp_log("%s,%d,帐号不存在!\n",__FILE__,__LINE__);
			flag=1;/*账号不存在**/
			ret=0;
		}else{
			/***开始检查密码**发过来的密码是不是加密后的密码???**/
			memcpy(cTd_add.pswdcode,cZhcx_add.passwd,sizeof(cTd_add.pswdcode));
			memcpy(cTd_add.pswdlength,cZhcx_add.pswdlen,sizeof(cTd_add.pswdlength));
			memcpy(cTd_add.pswdmethod,"01",2);
			ret=nBepsDescrypt(&cWp_payin,&cTd_add);/**对密码进行解密**/
			CBshuff(cTd_add.pswdcode);
			/*if(memcmp(cMdmacrel.qry_pwd,cTd_add.pswdcode,6)!=0)rem by martin 20071120*/
			if(memcmp(cMdmacrel.draw_pwd,cTd_add.pswdcode,6)!=0){
				vtcp_log("%s,%d,密码不正确!\n",__FILE__,__LINE__);
				cZhcxyd_add.resptype='1';
				flag=2;/**密码不正确**/
			}
		}
		/**检验密码结束**/
		ret=Dd_mst_Sel(g_pub_tx.reply,&cDdmst,"ac_id=%d",cMdmacrel.ac_id);
		if(ret){
			vtcp_log("%s,%d,帐号不存在!\n",__FILE__,__LINE__);
			ret=0;
			flag=1;/**账号不存在**/
		}
		memset(buffer,0,sizeof(buffer));
		sprintf(buffer,"RMB%015.0f",cDdmst.bal*100);
		memcpy(cZhcxyd_add.otxamt,buffer,sizeof(cZhcxyd_add.otxamt));
		if(cDdmst.hold_sts[0]=='0' && cDdmst.ac_sts[0]=='1'){
			memcpy(cZhcxyd_add.actStat,"00",2);
		} else if(cDdmst.hold_sts[0]=='1' || cDdmst.hold_sts[0]=='3'){
			memcpy(cZhcxyd_add.actStat,"01",2);
			memcpy(cZhcxyd_add.content,"账户冻结状态",12);
		} else{
			memcpy(cZhcxyd_add.actStat,"99",2);
			vtcp_log("%s,%d,Gethere,hold_sts=[%c],ac_sts=[%c]\n",__FILE__,__LINE__,cDdmst.hold_sts[0],cDdmst.ac_sts[0]);
			memcpy(cZhcxyd_add.content,"账户其他状态",12);
		}
		if(flag==1){/*账号不存在时**/
			cZhcxyd_add.resptype='1';
			memset(cZhcxyd_add.otxamt,'0',sizeof(cZhcxyd_add.otxamt));
			memset(cZhcxyd_add.actStat,' ',sizeof(cZhcxyd_add.actStat));
			memset(cZhcxyd_add.content,' ',sizeof(cZhcxyd_add.content));
		} else if(flag==2){
			cZhcxyd_add.resptype='2';
			memset(cZhcxyd_add.otxamt,'0',sizeof(cZhcxyd_add.otxamt));
			memset(cZhcxyd_add.actStat,' ',sizeof(cZhcxyd_add.actStat));
			memset(cZhcxyd_add.content,' ',sizeof(cZhcxyd_add.content));
		} else if(flag==0){
			cZhcxyd_add.resptype='0';
		}
		memset(cZhcxyd_add.content,' ',sizeof(cZhcxyd_add.content));
		memset(buffer,0,sizeof(buffer));
		/****设置应答的notpayin结构体****/
		memcpy(&notpayin_hz,&notpayin,sizeof(notpayin_hz));
		memcpy(notpayin_hz.optype,OPCD_LR,sizeof(notpayin_hz.optype));
		iOrderno=iLvGetOrderno();
		sprintf(cOrderno,"%08d",iOrderno);
		memcpy(notpayin_hz.orderno,cOrderno,sizeof(cOrderno)-1);/**交易序号**/
		memcpy(notpayin_hz.txnum,TXNUM_CXYD,sizeof(notpayin_hz.txnum));
		memcpy(notpayin_hz.orbrno,notpayin.acbrno,sizeof(notpayin_hz.orbrno));
		memcpy(notpayin_hz.acbrno,notpayin.orbrno,sizeof(notpayin_hz.acbrno));
		memcpy(notpayin_hz.payqsactno,notpayin.cashqsactno,sizeof(notpayin_hz.payqsactno));
		memcpy(notpayin_hz.cashqsactno,notpayin.payqsactno,sizeof(notpayin_hz.cashqsactno));
		memcpy(notpayin_hz.sendco,notpayin.receco,sizeof(notpayin_hz.sendco));
		memcpy(notpayin_hz.receco,notpayin.sendco,sizeof(notpayin_hz.receco));
		notpayin_hz.stat=STAT_WZLR;
		memcpy(notpayin_hz.opackday,pcLvGetDay(),8);
		sprintf(cOrderno,"%08d",iLvGetPackid());
		memcpy(notpayin_hz.opackid,cOrderno,8);
		memset(notpayin_hz.addid,'\0',sizeof(notpayin_hz.addid));
		ret=iLvSetAddit_Notpay(&notpayin_hz,cAddDay,"000",&cZhcxyd_add,sizeof(ZHCXYD_ADD));
		if(ret){
			vtcp_log("%s,%d,插入附加域失败![%d]\n",__FILE__,__LINE__,ret);
			return(-1);
		}
		ret=notpayin_to_table(&notpayin_hz);
		if(ret){
			vtcp_log("%s,%d,插入非支付登记簿失败![%d]\n",__FILE__,__LINE__,ret);
			return(-1);
		}
		/***********************************/
		memcpy(lv_wbctl.pkgno,PKGNO_SSXX,sizeof(lv_wbctl.pkgno)-1);		/***包编号**/
		lv_wbctl.pack_date = apatoi(notpayin_hz.opackday, 8); 		  	/***包日期**/
		lv_wbctl.in_date=lv_wbctl.pack_date;
		lv_wbctl.dtlcnt=1;
		memcpy(lv_wbctl.br_no,g_pub_tx.tx_br_no,sizeof(g_pub_tx.tx_br_no)-1);
		memcpy(lv_wbctl.packid,notpayin_hz.opackid,sizeof(lv_wbctl.packid)-1); 				/***包序号**/
		memcpy(lv_wbctl.pay_qs_no,notpayin_hz.payqsactno,sizeof(lv_wbctl.pay_qs_no)-1);/**付款清算行号***/
		memcpy(lv_wbctl.ornode,notpayin_hz.payqsactno,sizeof(lv_wbctl.ornode)-1);/**付款清算行号***/
		memcpy(lv_wbctl.cash_qs_no,notpayin_hz.cashqsactno,sizeof(lv_wbctl.cash_qs_no)-1);/**收款清算行号**/
		memcpy(lv_wbctl.acnode,notpayin_hz.cashqsactno,sizeof(lv_wbctl.acnode)-1);/**付款清算行号***/
		memcpy(lv_wbctl.txnum,notpayin_hz.txnum,	sizeof(lv_wbctl.txnum)-1);           /**业务类型**/
		lv_wbctl.stat[0] = PSTAT_PACKED;
		lv_wbctl.ffflag [0] =  '0';
		lv_wbctl.dtlcnt = 1;
		/*****到这里已经具备了实时信息必须的要素了******/
	 
		Lv_wbctl_Debug(&lv_wbctl);	
		ret = Lv_wbctl_Ins(lv_wbctl,g_pub_tx.reply);
		if(ret) {
			vtcp_log("[%s][%d]插入lv_wbctl错误!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"P026");
			return(-1);
		}
		sprintf(cPackDay,"%08d",lv_wbctl.pack_date);	
		memcpy(cPackid,lv_wbctl.packid,8);	
		ret=lv_get_ssxx_tag(cPackDay,cPackid);
		if(ret){
			vtcp_log("%s,%d,发送实时信息包失败!\n",__FILE__,__LINE__);
			strcpy(g_pub_tx.reply,"SL011");
			sprintf(acErrMsg,"发送实时信息报文失败!");
			set_zd_data(DC_GET_MSG,acErrMsg);
			WRITEMSG
			return(-1);
		}
	}

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt301(TLVPACKAGE *lv)                            */
/* 功能描述: CMT301查询报文来账处理                                         */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt301(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否CMT301查询报文****/
	if (memcmp(lv->CMTCode,"301",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT301查询报文\n",__FILE__,__LINE__,
			lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT301查询报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]========\n",__FILE__,__LINE__);
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]========notpayin->cmtno=[%.3s]\n",__FILE__,__LINE__,notpayin.cmtno);
	pubf_lv_print_NOTPAY_IN_AREA(&notpayin);

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	vtcp_log("[%s][%d]========notpayin->notpayin.acbrno=[%.12s]\n",__FILE__,__LINE__,notpayin.acbrno);
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	vtcp_log("[%s][%d]========tx_br_no=[%.12s]\n",__FILE__,__LINE__,tx_br_no);

	pubf_lv_GetBr_noTel(tx_br_no);

	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT301登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT301登记簿错误");
		WRITEMSG
			return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	} Modified By ChenMing 2006.09.15.      ****/

	/***********
	vtcp_log("[%s][%d]==========lv->packDate=[%s]\n",__FILE__,__LINE__,lv->packDate);
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	***********/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt302(TLVPACKAGE *lv)                            */
/* 功能描述: CMT302查复报文来账处理                                       */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt302(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));
	/****检查是否CMT302查复报文****/
	if (memcmp(lv->CMTCode,"302",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT302查复报文\n",__FILE__,__LINE__,
			lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT302查复报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d, notpayin->qwtday =[%.8s]",__FILE__,__LINE__,notpayin.qwtday );    
	vtcp_log("%s,%d, notpayin->qorbrno =[%.12s]",__FILE__,__LINE__,notpayin.qorbrno );
	vtcp_log("%s,%d, notpayin->qorderno =[%.8s]",__FILE__,__LINE__,notpayin.qorderno );

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	vtcp_log("%s,%d, notpayin->qwtday =[%.8s]",__FILE__,__LINE__,notpayin.qwtday );    
	vtcp_log("%s,%d, notpayin->qorbrno =[%.12s]",__FILE__,__LINE__,notpayin.qorbrno );
	vtcp_log("%s,%d, notpayin->qorderno =[%.8s]",__FILE__,__LINE__,notpayin.qorderno );
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT302登记簿错误\n",__FILE__,__LINE__);

		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}   Modified By ChenMing 2006.09.15.      ****/

	/*********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	*********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt303(TLVPACKAGE *lv)                            */
/* 功能描述:CMT303自由格式报文来账处理                                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt303(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	int iOrderno = 0;
	NOTPAY_IN_AREA notpayin;
	char tx_br_no[13];
	char tel[7];  

	memset(&notpayin, '\0', sizeof(notpayin));
	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否CMT303自由格式报文****/
	if (memcmp(lv->CMTCode,"303",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT303自由格式报文\n",__FILE__,__LINE__,  lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT303自由格式报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	pubf_lv_print_NOTPAY_IN_AREA(&notpayin);
	/****add by liuyue 20060117*******/
	/*****给自由格式的交易序号赋值****/
	iOrderno=iLvGetFzfOrderno();
	apitoa(iOrderno, sizeof(notpayin.orderno), notpayin.orderno);

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	if (strlen(tx_br_no)==0)
	{
		vtcp_log("%s,%d tx_br_no is null",__FILE__,__LINE__);
	}
	pubf_lv_GetBr_noTel(tx_br_no);

	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT303登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);
	return(-1);
	}
	Modified By ChenMing 2006.09.15.      ****/    
	/****    Modified By ChenMing 2006.09.15.      ****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt319(TLVPACKAGE *lv)                         */
/* 功能描述:CMT319退回申请报文来账处理                                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt319(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	char tx_br_no[13];
	char tel[7];

	memset(&notpayin, '\0', sizeof(notpayin));

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否CMT319退回申请报文****/
	if (memcmp(lv->CMTCode,"319",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT319退回申请报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT319退回申请报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT319登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}  
	Modified By ChenMing 2006.09.15.      ****/  
	/**********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	*********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt320(TLVPACKAGE *lv)                             */
/* 功能描述:CMT320退回申请应答报文来账处理                                 */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt320(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));
	/****检查是否CMT320退回申请应答报文****/
	if (memcmp(lv->CMTCode,"320",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT320退回申请应答报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT320退回申请应答报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT320登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT320登记簿错误");
		WRITEMSG    
			return(-1);
	}
	/*   这个函数中会形成游标嵌套 
	iRc=Rev_Pkg_Stat(&notpayin);  
	*/
	iRc=iLv_Reset_Pkg_Stat(&notpayin);
	if (iRc)
	{
		sprintf("%s,%d,修改交易状态错误\n",__FILE__,__LINE__);
		WRITEMSG
			return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}
	Modified By ChenMing 2006.09.15.      ****/    
	/*********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt322(TLVPACKAGE *lv)                            */
/* 功能描述: CMT322退回撤销应答报文来账处理                                */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt322(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	char tx_br_no[13];
	char tel[7];  

	/* begin add by lhf20060301 */
	struct lv_wbctl_c wd_lv_wbctl;
	char cCancelCode[2];
	memset(cCancelCode, 0 , sizeof(cCancelCode));
	/* end by lhf */
	memset(&notpayin, '\0', sizeof(notpayin));
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/*********数据初始化******************/

	/****检查是否CMT322退回撤销应答报文****/
	if (memcmp(lv->CMTCode,"322",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT322退回撤销应答报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT322退回撤销应答报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	/*   这个函数中会形成游标嵌套 
	iRc=Rev_Pkg_Stat(&notpayin);  
	*/
	iRc=iLv_Reset_Pkg_Stat(&notpayin);
	if (iRc)
	{
		sprintf("%s,%d,修改交易状态错误\n",__FILE__,__LINE__);
		WRITEMSG
			return(-1);
	}  
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT322登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT322登记簿错误");
		WRITEMSG
			return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}  
	Modified By ChenMing 2006.09.15.      ****/ 
	/* begin add by LiuHuafeng 20060301 */
	/****Add by SSH,2005.12.3,下面处理原往帐****/

	memset(&wd_lv_wbctl, 0 , sizeof(wd_lv_wbctl));
	wd_lv_wbctl.pack_date=apatoi(notpayin.opackday,8);
	lv_orderno_add0(notpayin.opackid, sizeof(wd_lv_wbctl.packid)-1, wd_lv_wbctl.packid);

	if (lvca.rtcode = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid))
	{
		sprintf(acErrMsg,"定义游标出错 - %d", lvca.rtcode);
		WRITEMSG
			return(-1);
	}
	if ((lvca.rtcode = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
	{
		vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"锁定原往包失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]================\n",__FILE__,__LINE__);
	lv_get_tag(lv,"CIB",wd_lv_wbctl.packstat);/****包处理状态****/
	lv_get_tag(lv,"BS1",wd_lv_wbctl.zcnode);/****扎差节点类型****/
	iLv_GETTAG_DATE(lv,"BS2",&wd_lv_wbctl.zc_date);/****扎差日期****/
	lv_get_tag(lv,"BS3",wd_lv_wbctl.zcid);/****扎差场次****/
	lv_get_tag(lv,"04C",cCancelCode);/****撤销应答****/
	if (cCancelCode[0]=='1') /* 已经撤销成功 */
	{
		char pRespCode[9];
		memset(pRespCode, 0 , sizeof(pRespCode));
		memcpy(wd_lv_wbctl.packstat,PACKSTAT_CANCEL,sizeof(wd_lv_wbctl.packstat)-1);    /***包状态－已撤销****/
		vtcp_log("[%s][%d]================\n",__FILE__,__LINE__);
		iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
		if ( iRc ==DB_OK)
		{
			iRc = Lv_wbctl_Clo_Upd();
		}
		if (iRc)
		{
			vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
			Lv_wbctl_Debug(&wd_lv_wbctl);
			sprintf(acErrMsg,"更新原往包失败");
			WRITEMSG
				return(-1);
		}
		/****处理原交易****/
		memcpy(pRespCode,"CS2O9007",8); /* 撤销 */
		iRc=lv_dealpkg_resp(&wd_lv_wbctl,pRespCode);
		if (iRc)
		{
			vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
			if (memcmp(acErrMsg,"应答重复",8))
			{
				sprintf(acErrMsg,"处理原交易失败");
				WRITEMSG
			}
			return(-1);
		}
	}
	/* end add by LiuHuafeng 20060301 */
	/* add by LiuHuafeng 20060311 恢复原来状态 
	else
	{
	}
	add by LiuHuafeng 20060311 恢复原来状态 */
	/********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	*******/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt324(TLVPACKAGE *lv)                       */
/* 功能描述: CMT324实时业务冲正应答报文来账处理                            */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt324(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否CMT324实时业务冲正应答报文****/
	if (memcmp(lv->CMTCode,"324",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT324实时业务冲正应答报文\n",__FILE__,__LINE__,
			lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT324实时业务冲正应答报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	/*   这个函数中会形成游标嵌套 
	iRc=Rev_Pkg_Stat(&notpayin);  
	*/
	iRc=iLv_Reset_Pkg_Stat(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,修改交易状态错误\n",__FILE__,__LINE__);
		return(-1);
	}  
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT324登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中

	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}   
	Modified By ChenMing 2006.09.15.      ****/   
	/****Add by SSH,2005.12.3,下面处理原往帐****/
	/****Add by SSH,2005.12.3,下面处理原往帐****/
	{
		/****只有不能冲正的时候才需要按应答处理原交易****/
		struct lv_wbctl_c wd_lv_wbctl;
		char pRespCode[9];
		memset(pRespCode,'\0',sizeof(pRespCode));
		memset(&wd_lv_wbctl,'\0',sizeof(wd_lv_wbctl));
		wd_lv_wbctl.pack_date=apatoi(notpayin.opackday,8);
		memcpy(wd_lv_wbctl.packid,notpayin.opackid,sizeof(wd_lv_wbctl.packid)-1);

		if (lvca.rtcode = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid))
		{
			sprintf(acErrMsg,"定义游标出错 - %d", lvca.rtcode);
			WRITEMSG
				return(-1);
		}
		if ((lvca.rtcode = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"锁定原往包失败");
			WRITEMSG
				return(-1);
		}
		vtcp_log("[%s][%d]================\n",__FILE__,__LINE__);

		lv_get_tag(lv,"CIB",wd_lv_wbctl.packstat);/****包处理状态****/
		lv_get_tag(lv,"BS1",wd_lv_wbctl.zcnode);/****扎差节点类型****/
		iLv_GETTAG_DATE(lv,"BS2",&wd_lv_wbctl.zc_date);/****扎差日期****/
		lv_get_tag(lv,"BS3",wd_lv_wbctl.zcid);/****扎差场次****/
		wd_lv_wbctl.stat[0]=PSTAT_WRESP;    /***包状态－已应答****/
		vtcp_log("[%s][%d]================\n",__FILE__,__LINE__);

		iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
			Lv_wbctl_Debug(&wd_lv_wbctl);
			sprintf(acErrMsg,"更新原往包失败");
			WRITEMSG
				return(-1);
		}
		Lv_wbctl_Clo_Upd();    
		vtcp_log("[%s][%d]respcond==[%c]\n",__FILE__,__LINE__,notpayin.respcode);

		/****处理原交易****/
		if (notpayin.respcode=='2')
		{
			if (memcmp(wd_lv_wbctl.packstat,PACKSTAT_SUCCESS,sizeof(wd_lv_wbctl.packstat)-1)!=0 &&
				memcmp(wd_lv_wbctl.packstat,"10",2)!=0)
			{
				/****非扎差，认为被拒绝****/
				memcpy(pRespCode,"CB0O0000",8);
			}
			else
			{
				memcpy(pRespCode,"CB0I0000",8);
				memcpy(wd_lv_wbctl.packstat,PACKSTAT_SUCCESS,sizeof(wd_lv_wbctl.packstat)-1);
			}
		}
		/*begin add by LiuHuafeng 20060318 */
		else
		{
			/* 应答同意撤销情况 */
			memcpy(pRespCode,"CS0OCZ00",8);
		}
		/*end add by LiuHuafeng 20060318 */
		iRc = lv_dealpkg_resp(&wd_lv_wbctl,pRespCode);
		if (iRc)
		{
			vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
			if (memcmp(acErrMsg,"应答重复",8))    sprintf(acErrMsg,"处理原交易失败");
			WRITEMSG
				return(-1);
		}
	}
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt325(TLVPACKAGE *lv)                       */
/* 功能描述: CMT325实时业务冲正通知报文来账处理                           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt325(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));
	/****检查是否CMT325实时业务冲正通知报文****/
	if (memcmp(lv->CMTCode,"325",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT325实时业务冲正通知报文\n",__FILE__,__LINE__,  lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT325实时业务冲正通知报文");
		WRITEMSG
			return(-1);
	}
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	notpayin.respcode='1';
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,notpayin.opkgno);

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	iRc=iLv_Reset_325_Stat_Sig(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,修改交易状态错误\n",__FILE__,__LINE__);
		return(-1);
	}
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT325登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}   Modified By ChenMing 2006.09.15.      ****/
	/********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt327(TLVPACKAGE *lv)                       */
/* 功能描述: CMT327借记业务止付申请报文来账处理                           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt327(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	vtcp_log("[%s][%d]lv_deal_cmt327 \n",__FILE__,__LINE__);
	/****检查是否CMT327借记业务止付申请报文****/
	if (memcmp(lv->CMTCode,"327",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT327借记业务止付申请报文\n",__FILE__,__LINE__, lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT327借记业务止付申请报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] opkgno=[%.3s] \n",__FILE__,__LINE__,notpayin.opkgno);
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]打印notpay结构体中相关兴趣字段 \n",__FILE__,__LINE__);
	vtcp_log("[%s][%d] rettype=[%c] respcode[%c]\n",__FILE__,__LINE__,notpayin.rettype,notpayin.respcode);
	vtcp_log("[%s][%d] opackid=[%.8s] opackday[%.8s]\n",__FILE__,__LINE__,notpayin.opackid,notpayin.opackday);
	vtcp_log("[%s][%d] orderno=[%.8s] oorderno=[%.8s]\n",__FILE__,__LINE__,notpayin.orderno,notpayin.oorderno);
	vtcp_log("[%s][%d] owtday=[%.8s] oacbrno=[%.12s] \n",__FILE__,__LINE__,notpayin.owtday,notpayin.oacbrno);
	vtcp_log("[%s][%d] opkgno=[%.3s] \n",__FILE__,__LINE__,notpayin.opkgno);

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	vtcp_log("[%s][%d] opkgno=[%.3s] \n",__FILE__,__LINE__,notpayin.opkgno);
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT327登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT327登记簿错误");
		WRITEMSG    
			return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中

	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}Modified By ChenMing 2006.09.15.      ****/  
	/********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt328(TLVPACKAGE *lv)                       */
/* 功能描述: CMT328借记业务止付应答报文来账处理                           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt328(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));
	char tx_br_no[13];
	char tel[7];

	/*********数据初始化******************/
	memset(tx_br_no,0,sizeof(tx_br_no));
	memset(tel,0,sizeof(tel));

	/****检查是否CMT328借记业务止付应答报文****/
	if (memcmp(lv->CMTCode,"328",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT328借记业务止付应答报文\n",__FILE__,__LINE__,  lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT328借记业务止付应答报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] orderno=[%.8s],oorderno=[%.8s]\n",__FILE__,__LINE__,notpayin.orderno,notpayin.oorderno);
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] orderno=[%.8s],oorderno=[%.8s]\n",__FILE__,__LINE__,notpayin.orderno,notpayin.oorderno);

	/* 根据支付系统行号设置行内系统的机构号和相应的小额接收柜员号 */
	memcpy(tx_br_no, notpayin.acbrno,sizeof(tx_br_no)-1);
	pubf_lv_GetBr_noTel(tx_br_no);

	/*   这个函数中会形成游标嵌套 
	iRc=Rev_Pkg_Stat(&notpayin);  
	*/
	iRc=iLv_Reset_Pkg_Stat(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,修改交易状态错误\n",__FILE__,__LINE__);
		return(-1);
	}  
	iRc=notpayin_to_table(&notpayin);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT328登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}   Modified By ChenMing 2006.09.15.      ****/    
	/*********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt254(TLVPACKAGE *lv)                       */
/* 功能描述: CMT254小额轧差净额清算通知报文来账处理                         */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt254(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	memset(&notpayin, '\0', sizeof(notpayin));

	/*********数据初始化******************/

	/****检查是否CMT254小额轧差净额清算通知报文****/
	if (memcmp(lv->CMTCode,"254",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT254小额轧差净额清算通知报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT254小额轧差净额清算通知报文");
		WRITEMSG
		return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	vtcp_log("[%s][%d]cmtcode=[%d] \n",__FILE__,__LINE__,iRc);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]cmtcode=[%s] \n",__FILE__,__LINE__,lv->CMTCode);
	iRc = notpayin_to_table_msg(lv);
	vtcp_log("[%s][%d]cmtcode=[%s] \n",__FILE__,__LINE__,lv->CMTCode);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT254登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT254登记簿错误");
		WRITEMSG    
			return(-1);
	}
	/****  插入交易历史明细 并入notpayin_to_table()中
	iRc=dg_hv_fzfhtr(&notpayin);
	vtcp_log("[%s][%d]cmtcode=[%s] \n",__FILE__,__LINE__,lv->CMTCode);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);

	return(-1);
	}   Modified By ChenMing 2006.09.15.      ****/
	/**********
	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);

	return(-1);
	}
	*********/
	/****子报文循环完毕****/
	/******插入来包控制表************/

	return(0);
}

/***************************************************************************/
/* 文件名称: int lv_deal_cmt920(TLVPACKAGE *lv)                       */
/* 功能描述: CMT920小额批量包处理确认通知报文来账处理                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:     请注意: 不要对地址再取地址 !!!!!!!!                           */
/***************************************************************************/
int lv_deal_cmt920(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	char pRespCode[9];
	PAY_IN_AREA payin;
	NOTPAY_IN_AREA notpayin;
	struct lv_wbctl_c wd_lv_wbctl;

	char cTemp[51];  
	char cPack_date[9];  

	memset(&notpayin, '\0', sizeof(notpayin));
	memset(&payin, '\0', sizeof(payin));
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));
	memset(pRespCode, '\0', sizeof(pRespCode));

	/*********数据初始化******************/

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/****检查是否CMT920小额批量包处理确认通知报文****/
	if (memcmp(lv->CMTCode,"920",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT920小额批量包处理确认通知报文\n",__FILE__,__LINE__,  lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT920小额批量包处理确认通知报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);

	/*********插入CMT920登记簿******************/
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,Gethere! lv_fetchcmt_to_notpayin() 完毕!! \n",__FILE__,__LINE__);

	iRc=notpayin_to_table_msg(lv);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT920登记簿错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入CMT920登记簿失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,Gethere! notpayin_to_table_msg() 完毕!! \n",__FILE__,__LINE__);

	/*************下面处理原往包****************/
	vtcp_log("%s,%d, =========== 下面处理原往包 ================== \n",__FILE__,__LINE__);

	wd_lv_wbctl.pack_date=apatoi(notpayin.opackday,8);
	memcpy(wd_lv_wbctl.packid,notpayin.opackid,sizeof(wd_lv_wbctl.packid)-1);

	g_reply_int = Lv_wbctl_Dec_Upd(g_pub_tx.reply, "pack_date = %d and packid = '%s'", wd_lv_wbctl.pack_date, wd_lv_wbctl.packid);
	if (g_reply_int)
	{
		sprintf(acErrMsg,"定义游标出错 - %d", g_reply_int);
		WRITEMSG
			return(-1);
	}
	g_reply_int = Lv_wbctl_Fet_Upd(&wd_lv_wbctl, g_pub_tx.reply);
	/* begin add by LiuHuafeng 2009-5-15 11:19:34 for 村镇银行 */
	/* 如果没有找到，认为是村镇银行的920应答 */
	if(g_reply_int == 100)
	{
		vtcp_log("%s,%d 往包没有找到认为是村镇银行业务，直接返回0 ",__FILE__,__LINE__);
		return 0;
	}
	/* end by LiuHuafeng 2009-5-15 11:24:04 */
	if (g_reply_int)
	{
		vtcp_log("[%s][%d]-----Lv_wbctl_Fet_Upd error [%d]\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"Lv_wbctl_Fet_Upd error ");
		WRITEMSG
			Lv_wbctl_Debug(&wd_lv_wbctl);
		return(-1);
	}
	vtcp_log("[%s][%d]================\n",__FILE__,__LINE__);
	lv_get_tag(lv,"CIB",wd_lv_wbctl.packstat);/**** 包处理状态 ****/
	lv_get_tag(lv,"BS1",wd_lv_wbctl.zcnode);/****扎差节点类型****/
	iLv_GETTAG_DATE(lv,"BS2",&wd_lv_wbctl.zc_date);/****扎差日期****/
	lv_get_tag(lv,"BS3",wd_lv_wbctl.zcid);/****扎差场次****/
	lv_get_tag(lv,"C42",pRespCode);/****应答码****/
	vtcp_log("%s,%d,pRespCode=[%s]\n",__FILE__,__LINE__,pRespCode);  
	wd_lv_wbctl.stat[0]=PSTAT_WRESP;    /***包状态－已应答****/

	vtcp_log("[%s][%d]====Before  Lv_wbctl_Upd_Upd() =========\n",__FILE__,__LINE__);
	iRc=Lv_wbctl_Upd_Upd(wd_lv_wbctl, g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d,更新原往包错误\n",__FILE__,__LINE__);
		Lv_wbctl_Debug(&wd_lv_wbctl);
		sprintf(acErrMsg,"更新原往包失败");
		WRITEMSG
			return(-1);
	}  
	iRc = Lv_wbctl_Clo_Upd();

	/****************下面根据应答码pRespCode处理原交易***********************/  
	vtcp_log("[%s][%d] 开始处理原来交易\n",__FILE__,__LINE__);
	iRc=lv_dealpkg_resp(&wd_lv_wbctl,pRespCode);
	if (iRc)
	{
		vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
		if (memcmp(acErrMsg,"应答重复",8))
		{
			sprintf(acErrMsg,"处理原交易失败");
			WRITEMSG
		}
		return(-1);
	}
	vtcp_log("[%s][%d] 原来交易处理结束\n",__FILE__,__LINE__);
	return(0);
}

/***************************************************************************/
/* 文件名称: int lv_dealpkg_resp()                                         */
/* 功能描述: 处理应答报文                                                   */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                    */
/***************************************************************************/
int lv_dealpkg_resp( struct lv_wbctl_c *wp_lv_wbctl,char *pRespCode)
{
	PAY_IN_AREA payin;
	int iRc=0;
	int iJzFlag=0;/****是否记帐的标志****/
	int iZcFlag=0;/****是否应扎差标志****/
	char sql_orbrno[13];
	char sql_wt_date[9];
	char sql_orderno[9];
	char sql_pkgno[4];
	char sql_string[512];
	/* begin add by LiuHuafeng order by HSYX for NEWYX xyy 2007-4-6 20:26 */
	long lWtdate;
	char cTxtype[6];
	char cOrbrno[13];
	char cOrderno[9];
	char cPackid[9];
	struct lv_pkgreg_c sLv_pkgreg;
	struct lv_pldsr_c lv_pldsr;
	struct lv_pldscx_c lv_pldscx;
	struct lv_dsfsendlist_c lv_dsfsendlist;
	struct lv_pldfr_c lv_pldfr;
	struct lv_pldsfqugx_c lv_pldsfqugx;
	char cRespStr[21];   /*Added    2006-11-17 16:14 */
	lWtdate = 0;
	memset(cRespStr,  0 , sizeof(cRespStr));
	memset(cTxtype ,  0 , sizeof(cTxtype));
	memset(cPackid ,  0 , sizeof(cPackid));
	memset(cOrderno,  0 , sizeof(cOrderno));
	memset(cOrbrno ,  0 , sizeof(cOrbrno));
	memset(cRespStr,  0 , sizeof(cRespStr));
	/* end by LiuHuafeng order by HSYX for NEWYX xyy 2007-4-6 20:46 */
	memset(sql_orbrno, '\0', sizeof(sql_orbrno));
	memset(sql_wt_date, '\0', sizeof(sql_wt_date));
	memset(sql_orderno, '\0', sizeof(sql_orderno));
	memset(sql_pkgno, '\0', sizeof(sql_pkgno));
	memset(sql_string, '\0', sizeof(sql_string));
	memset(&sLv_pkgreg,  0 , sizeof(sLv_pkgreg));

	/* begin add by LiuHuafeng order by HSYX for NEWYX xyy 2007-4-6 20:26 */
	memset(&lv_pldsr  ,  0 , sizeof(lv_pldsr));
	memset(&lv_pldscx ,  0 , sizeof(lv_pldscx));
	memset(&lv_pldsfqugx,0 , sizeof(lv_pldsfqugx));
	memset(&lv_pldfr  ,  0 , sizeof(lv_pldfr));
	memset(&lv_dsfsendlist  ,  0 , sizeof(lv_dsfsendlist));
	/* end by LiuHuafeng order by HSYX for NEWYX xyy 2007-4-6 20:46 */
	vtcp_log("[%s][%d]xyy -->wp_lv_wbctl->pkgno=[%s]",__FILE__,__LINE__,wp_lv_wbctl->pkgno);
	vtcp_log("[%s][%d]xyy -->wp_lv_wbctl->txnum=[%s]",__FILE__,__LINE__,wp_lv_wbctl->txnum);
	vtcp_log("wp_lv_wbctl->pay_qs_no=[%s] pack_date=[%d] packid=[%s]",wp_lv_wbctl->pay_qs_no,wp_lv_wbctl->pack_date,wp_lv_wbctl->packid);
	vtcp_log("[%s][%d]wp_lv_wbctl->opackid=[%s]",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
	/*** HSYX BEGIN:支票业务找原交易的方式不同****/
	if (memcmp(wp_lv_wbctl->o_packid,"99999999",8)==0  && 
		memcmp(wp_lv_wbctl->pkgno,PKGNO_PTJJHZ,strlen(PKGNO_PTJJHZ))==0)
	{
		iRc = Lv_pkgreg_Dec_Upd(g_pub_tx.reply," pkgno = '008' and txnum in ('30102','30103','30104','30105') and pay_qs_no='%s' and pack_date=%ld and packid='%s'", wp_lv_wbctl->pay_qs_no, wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
		/****HSYX END:支票业务找原交易的方式不同****/
	}   /****modify by xyy 2007-2-27 14:07*****/
	else if (memcmp(wp_lv_wbctl->pkgno,PKGNO_TYXX,strlen(PKGNO_TYXX))==0 && 
		(memcmp(wp_lv_wbctl->txnum,"40501",5)==0 || memcmp(wp_lv_wbctl->txnum,"40502",5)==0 || memcmp(wp_lv_wbctl->txnum,"40503",5)==0 || memcmp(wp_lv_wbctl->txnum,"40504",5)==0 || memcmp(wp_lv_wbctl->txnum,"40505",5)==0 ||memcmp(wp_lv_wbctl->txnum,"40506",5)==0) || memcmp(wp_lv_wbctl->txnum,"40507",5)==0)
	{
		vtcp_log("[%s][%d]这里开始了对通用信息的处理",__FILE__,__LINE__);
		g_reply_int = Lv_dsfsendlist_Sel(g_pub_tx.reply,&lv_dsfsendlist,"packid='%s' and wt_date=%ld ",wp_lv_wbctl->packid,wp_lv_wbctl->pack_date);
		if (g_reply_int)
		{
			vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
			sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
			return(-1);
		}
		else
		{
			memcpy(cTxtype,lv_dsfsendlist.o_tx_type,sizeof(cTxtype)-1); 
			memcpy(cOrderno,lv_dsfsendlist.o_orderno,sizeof(cOrderno)-1);
			lWtdate = lv_dsfsendlist.o_wt_date;
			memcpy(cOrbrno ,lv_dsfsendlist.or_br_no,sizeof(cOrbrno)-1);
		}
		vtcp_log("[%s][%d]cTxtype==[%s]",__FILE__,__LINE__,cTxtype);
		vtcp_log("[%s][%d]cOrderno==[%s]",__FILE__,__LINE__,cOrderno);
		vtcp_log("[%s][%d]lWtdate==[%ld]",__FILE__,__LINE__,lWtdate);
		if (memcmp(cTxtype,"40502",5)==0 || memcmp(cTxtype,"40504",5)==0)
		{
			g_reply_int = Lv_pldsr_Sel(g_pub_tx.reply,&lv_pldsr,"orderno='%s' and wt_date=%ld ",cOrderno,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
				sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
				return(-1);
			}
			memcpy(cPackid,lv_pldsr.packid,sizeof(cPackid)-1);
			vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
			g_reply_int = sql_execute("update lv_pldsr set repstat2='%s' where  packid='%s'  and wt_date=%ld ",wp_lv_wbctl->packstat,cPackid,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
				sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
				return(g_reply_int);
			}
		}
		else if (memcmp(cTxtype,"40501",5)==0 ||memcmp(cTxtype,"40503",5)==0)
		{
			g_reply_int = Lv_pldfr_Sel(g_pub_tx.reply,&lv_pldfr,"orderno='%s' and wt_date=%ld ",cOrderno,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
				sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
				return(-1);
			}
			memcpy(cPackid,lv_pldfr.packid,sizeof(cPackid)-1);
			vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
			g_reply_int = sql_execute("update lv_pldfr set repstat2='%s' where  packid='%s'  and wt_date=%ld ",wp_lv_wbctl->packstat,cPackid,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
				sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
				return(g_reply_int);
			}
		}
		else if (memcmp(cTxtype,"40505",5)==0)
		{
			g_reply_int = Lv_pldscx_Sel(g_pub_tx.reply,&lv_pldscx,"orderno='%s' and wt_date=%ld ",cOrderno,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
				sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
				return(-1);
			}
			memcpy(cPackid,lv_pldscx.packid,sizeof(cPackid)-1);
			vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
			g_reply_int = sql_execute("update lv_pldscx set repstat2='%s' where  packid='%s'  and wt_date=%ld ",wp_lv_wbctl->packstat,cPackid,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
				sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
				return(g_reply_int);
			}
		}
		else if (memcmp(cTxtype,"40507",5)==0)
		{
			g_reply_int = Lv_pldsfqugx_Sel(g_pub_tx.reply,&lv_pldsfqugx,"orderno='%s' and wt_date=%ld ",cOrderno,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],查找原交易出错sqlcode=[%d]\n",__FILE__,__LINE__,g_reply_int);
				sprintf( acErrMsg,"[%s][%d]查找原交易出错[%d]",__FL__,g_reply_int);
				return(-1);
			}
			memcpy(cPackid,lv_pldsfqugx.packid,sizeof(cPackid)-1);
			vtcp_log("[%s][%d]cPackid==[%s]",__FILE__,__LINE__,cPackid);
			g_reply_int = sql_execute("update lv_pldsfqugx set repstat='%s' where  packid='%s'  and wt_date=%ld ",wp_lv_wbctl->packstat,cPackid,lWtdate);
			if (g_reply_int)
			{
				vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
				sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
				return(g_reply_int);
			}
		}
		else
		{
			vtcp_log("[%s][%d],没有此交易类型[%s]",__FILE__,__LINE__, cTxtype);
			sprintf(acErrMsg,"没有此交易类型[%s][%d]",__FILE__,__LINE__);
			return(-1);
		}
		/***最后更新lv_dsfsendlist***/
		g_reply_int = sql_execute("update lv_dsfsendlist set repstat='%s' where packid='%s' and wt_date=%ld and or_br_no='%s' ",wp_lv_wbctl->packstat,wp_lv_wbctl->packid,wp_lv_wbctl->pack_date,cOrbrno);
		if (g_reply_int)
		{
			vtcp_log("[%s][%d],更新lv_pldsr表失败[%d]",__FILE__,__LINE__, g_reply_int);
			sprintf(acErrMsg,"更新lv_pldsr表失败[%s][%d]",__FILE__,__LINE__);
			return(g_reply_int);
		}
		return(0);    
	}/**end by xyy 2007-2-27 14:07**/
	else
	{
		iRc = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"pay_qs_no='%s' and pack_date=%d and packid='%s' and lw_ind ='1'",
			wp_lv_wbctl->pay_qs_no, wp_lv_wbctl->pack_date, wp_lv_wbctl->packid);
	}
	if (iRc)
	{
		vtcp_log("%s,%d,游标声明出错[%d]\n",__FILE__,__LINE__,iRc);
		strcpy(acErrMsg,"游标声明出错!!");WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,=====  Lv_pkgreg_Dec_Sel() 成功!!! ===== [%d]\n",__FILE__,__LINE__,iRc);
	int iTemp=0;     
	while (1)
	{
		memset(&payin,'\0',sizeof(payin));
		memset(&sLv_pkgreg, 0 , sizeof(sLv_pkgreg));

		vtcp_log("%s,%d,=====  Before Lv_pkgreg_Fet_Sel() !!! ===== ",__FILE__,__LINE__);
		iRc = Lv_pkgreg_Fet_Upd( &sLv_pkgreg, g_pub_tx.reply );
		if (iRc == 100)
		{
			if (iTemp == 0)
			{
				vtcp_log("[%s,%d], ====  一条记录也没有!!!!  =====",__FILE__,__LINE__);  
				strcpy(acErrMsg,"没有找到原实时贷记往帐记录");WRITEMSG
					return(-1);          
			}
			vtcp_log(" [%s,%d],====  循环处理完毕!!!!  =====",__FILE__,__LINE__);    
			break;
		}
		else  if(iRc)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,iRc);
			strcpy(acErrMsg,"数据库错误");WRITEMSG
				return(-1);
		}
		iTemp ++;
		vtcp_log("%s,%d,=====  Lv_pkgreg_Fet_Sel() 取到一条记录 !!! ===== ",__FILE__,__LINE__);

		memcpy(payin.F_pkgno, sLv_pkgreg.pkgno,sizeof(payin.F_pkgno));
		memcpy(payin.F_orbrno,sLv_pkgreg.or_br_no,sizeof(payin.F_orbrno));
		apitoa(sLv_pkgreg.wt_date,sizeof(payin.F_wtday),payin.F_wtday);
		memcpy(payin.F_orderno,sLv_pkgreg.orderno,sizeof(payin.F_orderno));
		memcpy(payin.F_opcd,OPCD_RESP,sizeof(payin.F_opcd));
		payin.F_lw_ind[0] = '1';  /*  原往帐 Added by ChenMing    2006.09.28.  */

		/*  引起游标的嵌套, 直接调用LV_CP_PAY_IN_FIELD() 
		iRc=iLvFetchRecToPayin(&payin);
		if(iRc)
		{
		vtcp_log("%s,%d,iLvFetchRecToPayin()出错 !!![%d]\n",__FILE__,__LINE__,iRc);
		strcpy(acErrMsg,"数据库错误");
		return(-1);
		}
		*****Remed by ChenMing   */  
		vtcp_log("[%s][%d] payin.T_resp1==[%.10s] ",__FILE__,__LINE__,payin.T_resp1);
		vtcp_log("[%s][%d] payin.T_resp2==[%.10s] ",__FILE__,__LINE__,payin.T_resp2);
		vtcp_log("[%s][%d] sLv_pkgreg.resp1==[%.10s] ",__FILE__,__LINE__,sLv_pkgreg.resp1);
		vtcp_log("[%s][%d] sLv_pkgreg.resp2==[%.10s] ",__FILE__,__LINE__,sLv_pkgreg.resp2);

		LV_CP_PAY_IN_FIELD(&payin,&sLv_pkgreg);  
		iLvZipAll(&payin);    

		vtcp_log("[%s][%d] payin.T_resp1==[%.10s] ",__FILE__,__LINE__,payin.T_resp1);
		vtcp_log("[%s][%d] payin.T_resp2==[%.10s] ",__FILE__,__LINE__,payin.T_resp2);

		vtcp_log("[%s][%d] iLvFetchRecToPayin成功!! ",__FILE__,__LINE__);
		iLvResetPayin(&payin);

		vtcp_log("[%s][%d]  iLvResetPayin 成功 ",__FILE__,__LINE__);
		vtcp_log("[%s][%d] payin.T_resp1==[%.10s] ",__FILE__,__LINE__,payin.T_resp1);
		vtcp_log("[%s][%d] payin.T_resp2==[%.10s] ",__FILE__,__LINE__,payin.T_resp2);

		if (payin.T_resp1[0]=='\0')
		{
			memcpy(payin.T_resp1,pRespCode,sizeof(payin.T_resp1));
		}
		else if (payin.T_resp2[0]=='\0')
		{
			memcpy(payin.T_resp2,pRespCode,sizeof(payin.T_resp2));
		}
		else
		{
			if (payin.T_stat[0]==STAT_WZSUCCESS || payin.T_stat[0]==STAT_WZCANCEL || payin.T_stat[0]==STAT_WZREJECT)
			{
				/* 已经是轧差，撤销，拒绝确实是重复了*/
				vtcp_log("%s,%d,应答重复packid=[%.8s]\n",__FILE__,__LINE__,payin.F_packid);
				strcpy(acErrMsg,"应答重复!!!");
				WRITEMSG

				/*Added  传给前置机，使得不报错  2006-11-17 16:14 */
					memcpy(cRespStr, "应答重复", sizeof(cRespStr)-1 );  
				set_zd_data("0130", cRespStr);

				return(-1);
			}
			else
			{
				memcpy(payin.T_resp2,pRespCode,sizeof(payin.T_resp2));
				vtcp_log("%s,%d,假应答重复packid=[%.8s]\n",__FILE__,__LINE__,payin.F_packid);
			}
		}
		if (memcmp(pRespCode,"CB0O6102",8)==0)
		{
			vtcp_log("%s,%d,变态版应答重复packid=[%.8s]\n",__FILE__,__LINE__,payin.F_packid);
			strcpy(acErrMsg,"应答重复");WRITEMSG

			/*Added  传给前置机，使得不报错  2006-11-17 16:14 */
				memcpy(cRespStr, "应答重复", sizeof(cRespStr)-1 );  
			set_zd_data("0130", cRespStr);

			return(-1);
		}
		vtcp_log("[%s][%d]A_brno==[%.7s]\n",__FILE__,__LINE__,payin.A_brno);
		vtcp_log("%s,%d,回执状态[%.2s]T_resp2=[%.8s]\n",__FILE__,__LINE__,wp_lv_wbctl->packstat,payin.T_resp2);
		if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SUCCESS,sizeof(wp_lv_wbctl->packstat)-1)==0 && payin.T_resp2[0]=='\0')
		{      /****已经扎差了,必须为resp2赋值****/
			memcpy(payin.T_resp2,pRespCode,sizeof(payin.T_resp2));
		}
		/* --------下面检查是否需要记帐  注意：不再在些记帐了，只置一些域的值  
		--------   2006-10-23 18:08  ChenMing     --------------------- */    
		iJzFlag=0;  /*  是否要记帐 1=需要记帐  */
		iZcFlag=0;  /*  是否已经扎差 1=已经扎差  */
		vtcp_log("[%s][%d] packstat==[%.2s]\n",__FILE__,__LINE__,wp_lv_wbctl->packstat);
		if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SUCCESS,sizeof(wp_lv_wbctl->packstat)-1)==0)
		{ /****已经扎差****/
			iZcFlag=1;
		}
		if ((iZcFlag || pRespCode[3]!='I') && (
			memcmp(payin.F_pkgno,PKGNO_PTDJ,sizeof(payin.F_pkgno))==0||
			memcmp(payin.F_pkgno,PKGNO_DQDJ,sizeof(payin.F_pkgno))==0||
			memcmp(payin.F_pkgno,PKGNO_SSDJ,sizeof(payin.F_pkgno))==0||
		/*begin add by LiuHuafeng 20060222 */
			memcmp(payin.F_pkgno,PKGNO_DJTH,sizeof(payin.F_pkgno))==0) )
		{
			/* end by LiuHuafeng 20060222 */
			/***扎差或者失败的贷记业务****/
			iJzFlag=1;
		}
		vtcp_log("[%s][%d]A_brno==[%.7s]\n",__FILE__,__LINE__,payin.A_brno);
		if ((iZcFlag ||pRespCode[3]!='I') &&  (
			memcmp(payin.F_pkgno,PKGNO_PTJJHZ,sizeof(payin.F_pkgno))==0||
			memcmp(payin.F_pkgno,PKGNO_DQJJHZ,sizeof(payin.F_pkgno))==0||
			memcmp(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno))==0) &&
			memcmp(payin.F_rcpstat,"00",2)==0)
		{
			/***扎差或者失败的借记成功回执业务****/
			iJzFlag=1;
		}
		vtcp_log("[%s][%d]A_brno==[%.7s]\n",__FILE__,__LINE__,payin.A_brno);
		vtcp_log("[%s][%d]pkgno==[%.3s],rcpstat==[%.2s] zcflag=[%d]\n",
			__FILE__,__LINE__,payin.F_pkgno,payin.F_rcpstat,iZcFlag);
		if (iZcFlag &&
			memcmp(payin.F_pkgno,PKGNO_SSDJHZ,sizeof(payin.F_pkgno))==0 &&
			memcmp(payin.F_rcpstat,"00",2)==0)
		{
			payin.F_feeflag[0]='2';
			iJzFlag=1;  /***扎差的实时贷记成功回执业务****/
		}
		/****Add by SSH,2006.2.6,对排队的特殊处理***/
		if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_INQUEUE,2)==0)
		{
			if (memcmp(pRespCode,"CS2O9007",8))
			{
				iJzFlag=0;
				payin.T_resp2[0]='\0';/* 排队状态应答码认为是中间状态放弃 */
				payin.F_resp2[0]='\0';/* 排队状态应答码认为是中间状态放弃 */
			}
			else
			{
				iJzFlag=1;
			}
		}
		vtcp_log("[%s][%d]检查是否需要记帐结果：  iJzFlag==[%d]\n",__FILE__,__LINE__,iJzFlag);

		if (iJzFlag)   /* 注意：现在不再在这里记帐了，只处理一些状态 */
		{
			if (payin.T_stat[0]==STAT_WZSUCCESS||payin.T_stat[0]==STAT_WZREJECT)
			{
				vtcp_log("%s,%d,原交易状态[%c]错误",__FILE__,__LINE__,payin.T_stat[0]);
				strcpy(acErrMsg,"原交易状态错误");WRITEMSG
					return(-1);
			}
			vtcp_log("%s,%d,Brno=%.7s\n",__FILE__,__LINE__,payin.A_brno);  
			vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
			vtcp_log("%s,%d,Brno=%.7s\n",__FILE__,__LINE__,payin.A_brno);
			vtcp_log("%s,%d,Wssrno=%.6s\n",__FILE__,__LINE__,payin.A_wssrno);
			if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SUCCESS, sizeof(wp_lv_wbctl->packstat)-1)==0)
			{
				payin.T_stat[0]=STAT_WZSUCCESS;
				vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
			}
			/* add by LiuHuafeng 20060307 */
			else if (memcmp(pRespCode,"CS2O9007",8)==0)
			{
				payin.T_stat[0]=STAT_WZCANCEL;
				vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
			}
			/* add by LiuHuafeng 20060318 */
			else if (memcmp(pRespCode,"CS0OCZ00",8)==0)
			{
				vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
				payin.T_stat[0]=STAT_WZCZ;
			}
			/*end by lhf */
			else
			{
				payin.T_stat[0]=STAT_WZREJECT;
			}
			vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);

		}
		else         
		{
			if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_INQUEUE,2)==0)
			{
				payin.T_stat[0]=STAT_WZINQUEUE;
			}
			else if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SUCCESS,2)==0)
			{
				payin.T_stat[0]=STAT_WZSUCCESS;
			}
			vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
		}
		vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
		vtcp_log("[%s][%d]=======  开始更新原交易表  =======",__FILE__,__LINE__);

		/****Add by SSH,2006.1.14,借记业务不记帐,直接判断状态****/
		if (pRespCode[3]!='I')
		{
			if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_INQUEUE,2)!=0)
			{
				/* add by LiuHuafeng 20060318 */
				if (memcmp(pRespCode,"CS0OCZ00",8)==0)
					payin.T_stat[0]=STAT_WZCZ;
				/* add by LiuHuafeng 20060318 */
				else if (memcmp(pRespCode,"CS2O9007",8)==0)
					payin.T_stat[0]=STAT_WZCANCEL; /* add by lhf0307 */
				else
					payin.T_stat[0]=STAT_WZREJECT;
			}
			if (payin.T_resp1[0]!='\0')
				memcpy(payin.T_resp2,pRespCode,sizeof(payin.T_resp2));
			else
				memcpy(payin.T_resp1,pRespCode,sizeof(payin.T_resp1));
		}
		vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
		/*begin add by LiuHuafeng 20060302 排队状态不写resp2 */
		if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_INQUEUE,2)==0 )
		{
			if (memcmp(pRespCode,"CS2O9007",8))
			{
				payin.T_resp2[0]='\0';/* 排队状态应答码认为是中间状态放弃 */
				payin.F_resp2[0]='\0';/* 排队状态应答码认为是中间状态放弃 */
			}
		}
		vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]);
		/*end by LiuHuafeng */

		/********** 这里调用  iLvUpdRecFromPayin()会引起游标嵌套，所以改成直接在此更新
		iRc=iLvUpdRecFromPayin(&payin,0);
		if (iRc)
		{
		vtcp_log("%s,%d,更新原交易表错误\n",__FILE__,__LINE__);
		strcpy(acErrMsg,"更新原交易表错误");WRITEMSG
		return(-1);
		}
		vtcp_log("[%s][%d]    iLvUpdRecFromPayin() Finished ! \n",__FILE__,__LINE__);
		****************************************************************************/

		LV_CP_PAY_IN_TABLE(&payin,&sLv_pkgreg);  

		iRc=Lv_pkgreg_Upd_Upd(sLv_pkgreg, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,Update error,iRc=%d\n",__FILE__,__LINE__,iRc);
			Lv_pkgreg_Debug(&sLv_pkgreg);
			sprintf(acErrMsg,"更新普通贷记发送表失败");
			return(-1);
		}    
		vtcp_log("[%s][%d] After Lv_pkgreg_Upd_Upd()  !!",__FILE__,__LINE__);

		/* 贷记退汇业务还要更新原来业务状态 */
		if (memcmp(payin.F_pkgno,PKGNO_DJTH,sizeof(payin.F_pkgno))==0 && payin.T_stat[0]==STAT_WZSUCCESS)
		{
			vtcp_log("[%s,%d] 是贷记退汇业务，开始更新原来业务状态\n",__FILE__,__LINE__);
			iRc=iLv_UpdateThStat(&payin);
			if (iRc)
			{
				vtcp_log("%s,%d,更新原来帐交易表错误\n",__FILE__,__LINE__);
				strcpy(acErrMsg,"更新原来帐交易表错误");WRITEMSG
					return(-1);
			}
		}
		/* add by NEWBP1 实时借记回执业务还要更新原来业务状态 */
		vtcp_log("[%s][%d]xyy请注意啦[%.3s][%c]\n",__FILE__,__LINE__,payin.F_pkgno,payin.T_stat[0]);
		if (memcmp(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno))==0 && payin.T_stat[0]==STAT_WZREJECT)
		{
			vtcp_log("[%s,%d] 实时借记回执被拒绝，开始更新原来业务状态\n",__FILE__,__LINE__);
			iRc=iLv_UpdateSSJJLZStat(&payin);
			if (iRc)
			{
				vtcp_log("%s,%d,更新原来帐交易表错误\n",__FILE__,__LINE__);
				strcpy(acErrMsg,"更新原来帐交易表错误");WRITEMSG
					return(-1);
			}
		}
		/***end by NEWBP1 ***/
		/* add by LiuHuafeng 2007-5-5 1:14 借记回执业务更新原来帐状态为应答发送 */
		if((memcmp(payin.F_pkgno,PKGNO_PTJJHZ,sizeof(payin.F_pkgno))==0 
				||memcmp(payin.F_pkgno,PKGNO_DQJJHZ,sizeof(payin.F_pkgno))==0
				||memcmp(payin.F_pkgno,PKGNO_SSJJHZ,sizeof(payin.F_pkgno))==0)
			&& payin.T_stat[0]==STAT_WZSUCCESS)/**WSMNE 这里只更改往帐成功的记录么？？**/
		{
			iRc=iLv_UpdateJjLzStat(&payin);
			if (iRc)
			{
				vtcp_log("%s,%d,更新原来帐交易表错误\n",__FILE__,__LINE__);
				strcpy(acErrMsg,"更新原来帐交易表错误");WRITEMSG
					return(-1);
			}
		}
		vtcp_log("[%s][%d]T_STAT===[%c]\n",__FILE__,__LINE__,payin.T_stat[0]); 
	}
	Lv_pkgreg_Clo_Upd();  
	vtcp_log("[%s][%d] lv_dealpkg_resp() Finished !!",__FILE__,__LINE__);
	return(0);
}

/***************************************************************************/
/* 文件名称: int lv_deal_cmt316(TLVPACKAGE *lv)           */
/* 功能描述: CMT316小额批量包处理状态通知报文来账处理           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_cmt316(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt,index;
	NOTPAY_IN_AREA notpayin;
	/*********数据初始化******************/
	memset(&notpayin, '\0', sizeof(notpayin));

	/****检查是否CMT316小额批量包处理状态通知报文****/
	if (memcmp(lv->CMTCode,"316",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT316小额批量包处理状态通知报文\n",__FILE__,__LINE__, lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT316小额批量包处理状态通知报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] 太奇怪了 !!! CMTNO=[%s] \n",__FILE__,__LINE__,lv->CMTCode);
	iRc=lv_fetchcmt_to_notpayin(lv,&notpayin);
	if (iRc<0)
	{
		vtcp_log("%s,%d,根据子报文填写notpayin失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"根据子报文填写notpayin失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] 太奇怪了 !!! CMTNO=[%s] \n",__FILE__,__LINE__,lv->CMTCode);
	iRc=notpayin_to_table_msg(lv);
	if (iRc)
	{
		vtcp_log("%s,%d,插入CMT316登记簿错误\n",__FILE__,__LINE__);
		return(-1);
	}
	/********

	iRc=dg_hv_fzfhtr(&notpayin);
	if(iRc){
	vtcp_log("%s,%d,插入单包交易历史明细错误\n",__FILE__,__LINE__);
	return(-1);
	}  

	iRc=lv_Recv_Ins_Tab(&lv,&notpayin,2);
	if(iRc){
	vtcp_log("%s,%d,插入小额报文发送接收历史登记簿错误\n",__FILE__,__LINE__);
	return(-1);
	}
	********/
	/****子报文循环完毕****/
	/******插入来包控制表************/
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt552(TLVPACKAGE *lv)           */
/* 功能描述: CMT552小额收费通知报文来账处理               */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_cmt552(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	/*********数据初始化******************/
	memset(&notpayin, '\0', sizeof(notpayin));

	/****检查是否CMT552自由格式报文****/
	if (memcmp(lv->CMTCode,"552",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT552小额收费通知报文报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT552小额收费通知报文报文");
		WRITEMSG
			return(-1);
	}
	iRc=Ins_cmt552_to_table(lv);
	if (iRc<0)
	{
		vtcp_log("%s,%d,插入552表失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入552表失败");
		WRITEMSG
			return(-1);
	}
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt554(TLVPACKAGE *lv)           */
/* 功能描述: CMT554小额对外返还通知报文               */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_cmt554(TLVPACKAGE *lv)
{
	int iRc=0;
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	/*********数据初始化******************/
	memset(&notpayin, '\0', sizeof(notpayin));

	/****检查是否CMT554自由格式报文****/
	if (memcmp(lv->CMTCode,"554",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT554小额对外返还通知报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT554小额对外返还通知报文");
		WRITEMSG
			return(-1);
	}
	iRc=Ins_cmt554_to_table(lv);
	if (iRc<0)
	{
		vtcp_log("%s,%d,插入554表失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"插入554表失败");
		WRITEMSG
			return(-1);
	}
	return(0);
}

/*********************************************************/
/**    函数名称:Ins_cmt554_to_table(NOTPAY_IN_AREA *wp_in)  **/
/** 参数:来帐解析的报文结构，输入包结构           **/
/** 含义:对所有cmt554记入lv_554r表中             **/
/** 编写:dongxy                        **/
/*********************************************************/
int Ins_cmt554_to_table(TLVPACKAGE *lv)
{
	int  ret=0,cnt=0,num=0,i=0,count=0,iDtlid=0;
	char  cTxday[9];
	char  Txt[6144];/*****modify by xyy 20060406*****/
	char  cTxamt[16];
	char  qsbrno[8];
	struct  lv_c554r_c  wd_lv_c554r;
	struct  lv_c554r_c  lv_c554r;  
	struct  hv_orno_brno_c sHv_orno_brno;
	memset(&sHv_orno_brno, 0 , sizeof(sHv_orno_brno));
	memset(Txt,   '\0', sizeof(Txt));
	memset(cTxday,   '\0', sizeof(cTxday));
	memset(qsbrno,   '\0', sizeof(qsbrno));
	memset(cTxamt,   '\0', sizeof(cTxamt));
	memset(&wd_lv_c554r,  '\0', sizeof(wd_lv_c554r));
	memcpy(cTxday, pcLvGetDay(), sizeof(cTxday) -1);

	/** 先插入lv_c554r **/
	/** 产生明细号 **/
	cnt = sql_max_int("lv_c554r", "dtlid", "in_date = '%s'", cTxday); 
	if (!memcmp(g_pub_tx.reply, "D101", 4))
	{
		g_reply_int = cnt;
	}else g_reply_int = DB_OK;
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"访问数据库错误!");WRITEMSG
			return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"数据库错误!");WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d]=======cnt=[%d]\n",__FILE__,__LINE__,cnt);
	if (cnt == 0)
		wd_lv_c554r.dtlid = 1;
	else
		wd_lv_c554r.dtlid = cnt+1;

	vtcp_log("[%s][%d]=============wd_lv_c554r.dtlid=[%d]\n",__FILE__,__LINE__,wd_lv_c554r.dtlid);
	iDtlid = wd_lv_c554r.dtlid;
	/*
	apitoa(wd_lv_c554r.dtlid, sizeof(iDtlid)-1, iDtlid);
	*/

	lv_get_tag(lv,"B6U",wd_lv_c554r.rebnum);
	num = apatoi(wd_lv_c554r.rebnum,6);
	printf("[%s][%d]========此笔包含的数量=[%d]\n",__FILE__,__LINE__,num);

	lv_get_tag(lv,"B8U",Txt);

	memcpy(wd_lv_c554r.cmtno, "554" , sizeof(wd_lv_c554r.cmtno)-1);
	wd_lv_c554r.in_date = apatoi(cTxday,8);

	iLv_GETTAG_DATE(lv,"30A",&wd_lv_c554r.wt_date);
	lv_get_tag(lv,"C36",wd_lv_c554r.sendco);
	lv_get_tag(lv,"C38",wd_lv_c554r.acbrno);
	lv_get_tag(lv,"BSD",wd_lv_c554r.iftype);
	lv_get_tag(lv,"CCM",wd_lv_c554r.payno);
	lv_get_tag(lv,"B21",wd_lv_c554r.paymon);
	iLv_GETTAG_DATE(lv,"B22",&wd_lv_c554r.sdate);
	iLv_GETTAG_DATE(lv,"B23",&wd_lv_c554r.edate);
	lv_get_tag(lv,"B24",wd_lv_c554r.aorm);
	lv_get_tag(lv,"C32",wd_lv_c554r.rebakno);

	lv_get_tag(lv,"B2E",cTxamt);
	wd_lv_c554r.rebamt = atof(cTxamt)/100.0;  /* Number(15)型的单位都为元*/
	lv_get_tag(lv,"B2F",wd_lv_c554r.rebrate);

	memcpy(sHv_orno_brno.or_br_no, wd_lv_c554r.acbrno, sizeof(sHv_orno_brno.or_br_no)-1);
	ret = Hv_orno_brno_Sel(g_pub_tx.reply, &sHv_orno_brno, "or_br_no = '%s'", sHv_orno_brno.or_br_no);
	if (ret != DB_OK)
	{
		vtcp_log("接收机构号有错误et=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(sHv_orno_brno.br_no, LV_RECV_BRNO);
	}
	set_zd_data("0030", sHv_orno_brno.br_no);
	set_zd_data("0070", pcLvGetRecvno());
	strncpy(lvca.brno, sHv_orno_brno.br_no,sizeof(sHv_orno_brno.br_no)-1);
	strncpy(lvca.tlrno, pcLvGetRecvno(),sizeof(lvca.tlrno));

	for (i=0;i<num;i++)
	{
		memset(&lv_c554r, 0, sizeof(lv_c554r));
		memcpy( (char *)&lv_c554r, (char *)&wd_lv_c554r,sizeof(struct  lv_c554r_c) );
		/** 产生明细号 **/
		count = sql_count("lv_c554r", "in_date = %d and dtlid=%d",  wd_lv_c554r.in_date, iDtlid);
		if (0 == memcmp(g_pub_tx.reply, "D101", 4))
		{
			g_reply_int = count;
		}else
		{
			g_reply_int = DB_OK;
		}
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
			sprintf(acErrMsg,"访问数据库错误!");
			WRITEMSG
				return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
			sprintf(acErrMsg,"数据库错误!");
			WRITEMSG
				return(-1);
		}
		vtcp_log("[%s][%d]=======count=[%d]\n",__FILE__,__LINE__,count);
		printf("[%s][%d]=======count=[%d]\n",__FILE__,__LINE__,count);
		vtcp_log("[%s][%d]=======count=[%d]\n",__FILE__,__LINE__,count);
		if (count == 0)
			lv_c554r.seqno = 1;
		else
			lv_c554r.seqno = count+1;

		vtcp_log("[%s][%d]=======wd_lv_c554r.seqno=[%d]\n",__FILE__,__LINE__,lv_c554r.seqno);

		vtcp_log("[%s][%d]==========i=[%d]\n",__FILE__,__LINE__,i);
		printf("[%s][%d]===========Txt=[%s]\n",__FILE__,__LINE__,Txt+38*i);
		memcpy(lv_c554r.pkgno, Txt+38*i, sizeof(lv_c554r.pkgno)-1);
		memcpy(lv_c554r.totnum, Txt+38*i+3, sizeof(lv_c554r.totnum)-1);
		memcpy(lv_c554r.brate, Txt+38*i+11, sizeof(lv_c554r.brate)-1);
		memcpy(lv_c554r.price, Txt+38*i+17, sizeof(lv_c554r.price)-1);
		memcpy(lv_c554r.txamt, Txt+38*i+23, sizeof(lv_c554r.txamt)-1);

		ret = Lv_c554r_Ins(lv_c554r, g_pub_tx.reply);
		if (ret != DB_OK)
		{
			vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf(acErrMsg,"插入CMT554小额收费通知报文失败");WRITEMSG
				Lv_pkgreg_Debug(&lv_c554r);        
			return(-1);
		}
		vtcp_log("[%s,%d]  插入Lv_c554r 成功 !!! ret=[%d]\n",__FILE__,__LINE__,ret);
	}
	return(0);
}

/*********************************************************/
/** 函数名称:Ins_cmt552_to_table(NOTPAY_IN_AREA *wp_in)  **/
/** 参数:来帐解析的报文结构，输入包结构     **/
/** 含义:对所有cmt552记入lv_552r表中       **/
/** 编写:dongxy            **/
/*********************************************************/
int Ins_cmt552_to_table(TLVPACKAGE *lv)
{
	int  ret=0,cnt=0,num=0,i=0,count=0,Dtlid=0;
	long  lTxday=0;
	char  Txt[6144];/*****modify by xyy 20060406*****/
	char  qsbrno[8];
	char  cWtdate[9];
	/*
	char  Dtlid[4];
	memset(Dtlid,   '\0', sizeof(Dtlid));
	*/
	static  struct  lv_c552r_c  wd_lv_c552r;
	static  struct  hv_fzfhtr_c  hv_fzfhtr;
	static  struct  hv_orno_brno_c  sHv_orno_brno;
	char cSdate[9];
	char cEdate[9];

	memset(Txt,   '\0', sizeof(Txt));
	memset(qsbrno,   '\0', sizeof(qsbrno));
	memset(cWtdate,   '\0', sizeof(cWtdate));
	memset(&wd_lv_c552r,  '\0', sizeof(wd_lv_c552r));
	memset(&hv_fzfhtr, '\0', sizeof(hv_fzfhtr));
	memset(&sHv_orno_brno,'\0', sizeof(sHv_orno_brno));
	lTxday =apatoi(pcLvGetDay(),8);

	/** 先插入lv_c552r **/

	/** 产生明细号 **/
	/** dxy 20060118 ,用count(*) 会使dtlid不联系
	**/
	cnt = sql_max_int("lv_c552r", "dtlid", "in_date = '%ld'", lTxday); 
	if (!memcmp(g_pub_tx.reply, "D101", 4))
	{
		g_reply_int = cnt;
	}else g_reply_int = DB_OK;
	if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"访问数据库错误!");
		return(-1);
	}
	if (g_reply_int==DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
		sprintf(acErrMsg,"数据库错误!");
		return(-1);
	}
	vtcp_log("[%s][%d]cnt=[%d]\n",__FILE__,__LINE__,cnt);
	if (cnt == 0)
		wd_lv_c552r.dtlid = 1;
	else
		wd_lv_c552r.dtlid = cnt+1;

	vtcp_log("[%s][%d]wd_lv_c552r.dtlid=[%d]\n",__FILE__,__LINE__,wd_lv_c552r.dtlid);
	Dtlid = wd_lv_c552r.dtlid;
	/**
	apitoa(wd_lv_c552r.dtlid, sizeof(Dtlid)-1, Dtlid);
	**/

	lv_get_tag(lv,"B6S",wd_lv_c552r.num);
	num = apatoi(wd_lv_c552r.num,8);
	vtcp_log("[%s][%d]此包中共有记录数num=[%d]\n",__FILE__,__LINE__,num);
	lv_get_tag(lv,"B8S",Txt);

	memcpy(wd_lv_c552r.cmtno, "552" , sizeof(wd_lv_c552r.cmtno)-1);
	wd_lv_c552r.in_date =  apatoi(pcLvGetDay(),8);
	lv_get_tag(lv,"30A",cWtdate);

	vtcp_log("[%s][%d]  Before Hv_orno_brno_Sel()  ",__FILE__,__LINE__);  
	wd_lv_c552r.wt_date = apatoi(cWtdate,8);
	lv_get_tag(lv,"C36",wd_lv_c552r.sendco);
	lv_get_tag(lv,"C38",wd_lv_c552r.acbrno);
	lv_get_tag(lv,"BSD",wd_lv_c552r.iftype);
	lv_get_tag(lv,"CCN",wd_lv_c552r.cashno);
	lv_get_tag(lv,"B21",wd_lv_c552r.paymon);

	pubf_lv_print_dm(&(lv->dm));
	vtcp_log("[%s][%d]  Before Hv_orno_brno_Sel()  ",__FILE__,__LINE__);
	memset(cSdate,0,sizeof(cSdate));
	memset(cEdate,0,sizeof(cEdate));  
	lv_get_tag(lv,"B22",cSdate);
	lv_get_tag(lv,"B23",cEdate);  
	wd_lv_c552r.sdate = atoi(cSdate);
	wd_lv_c552r.edate = atoi(cEdate);
	vtcp_log("[%s][%d]  wd_lv_c552r.sendco===[%s]  ",__FILE__,__LINE__,wd_lv_c552r.sendco);  
	vtcp_log("[%s][%d]  wd_lv_c552r.sdate===[%d]  ",__FILE__,__LINE__,wd_lv_c552r.sdate);  
	vtcp_log("[%s][%d]  wd_lv_c552r.edate===[%d]  ",__FILE__,__LINE__,wd_lv_c552r.edate);  

	lv_get_tag(lv,"B24",wd_lv_c552r.aorm);
	lv_get_tag(lv,"C31",wd_lv_c552r.countno);
	lv_get_tag(lv,"B25",wd_lv_c552r.totamt);
	lv_get_tag(lv,"B26",wd_lv_c552r.ferate);
	lv_get_tag(lv,"B27",wd_lv_c552r.acrate);
	lv_get_tag(lv,"B28",wd_lv_c552r.porr);
	lv_get_tag(lv,"B29",wd_lv_c552r.ccpcrate);
	lv_get_tag(lv,"B2A",wd_lv_c552r.totnomonth);
	lv_get_tag(lv,"B2B",wd_lv_c552r.monrate);
	lv_get_tag(lv,"B2C",wd_lv_c552r.totcrmonth);
	lv_get_tag(lv,"B2D",wd_lv_c552r.crrate);

	vtcp_log("[%s][%d]  Before Hv_orno_brno_Sel()  ",__FILE__,__LINE__);

	memcpy(sHv_orno_brno.or_br_no, wd_lv_c552r.acbrno, sizeof(sHv_orno_brno.or_br_no)-1);
	if (ret = Hv_orno_brno_Sel(g_pub_tx.reply, &sHv_orno_brno, "or_br_no = '%s'", sHv_orno_brno.or_br_no) != DB_OK)
	{
		vtcp_log("[%s][%d] 接收机构号有错误et=[%d]\n",__FILE__,__LINE__,ret);
		strcpy(sHv_orno_brno.br_no, LV_RECV_BRNO);
	}
	vtcp_log("[%s][%d]  After Hv_orno_brno_Sel()  ",__FILE__,__LINE__);

	set_zd_data("0030", sHv_orno_brno.br_no);
	set_zd_data("0070", pcLvGetRecvno());
	strcpy(lvca.brno, sHv_orno_brno.br_no);
	strcpy(lvca.tlrno, pcLvGetRecvno());

	memcpy(wd_lv_c552r.brno,  lvca.brno, sizeof(wd_lv_c552r.brno)-1);
	memcpy(wd_lv_c552r.tlrno, lvca.tlrno, sizeof(wd_lv_c552r.tlrno)-1);

	for (i=0;i<num;i++)
	{
		/** 产生明细号 **/
		count = sql_count("lv_c552r", "in_date = '%ld' and dtlid = %d",lTxday, Dtlid);
		if (0 == memcmp(g_pub_tx.reply, "D101", 4))
		{
			g_reply_int = count;
		}else
		{
			g_reply_int = DB_OK;
		}
		if (g_reply_int!=DB_OK && g_reply_int!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
			sprintf(acErrMsg,"访问数据库错误!");
			return(-1);
		}
		if (g_reply_int==DB_NOTFOUND)
		{
			vtcp_log("%s,%d,数据库错误sqlcode=%d\n",__FILE__,__LINE__,g_reply_int);
			sprintf(acErrMsg,"数据库错误!");
			return(-1);
		}
		vtcp_log("[%s][%d]=======count=[%d]\n",__FILE__,__LINE__,count);
		if (count == 0)
			wd_lv_c552r.seqno = 1;
		else
			wd_lv_c552r.seqno = count+1;

		vtcp_log("[%s][%d]wd_lv_c552r.seqno=[%f]\n",__FL__,wd_lv_c552r.seqno);

		vtcp_log("[%s][%d]i=[%d]\n",__FILE__,__LINE__,i);
		vtcp_log("[%s][%d]Txt=[%s]\n",__FILE__,__LINE__,Txt+60*i);

		memcpy(wd_lv_c552r.pkgno, Txt+60*i, sizeof(wd_lv_c552r.pkgno)-1);
		wd_lv_c552r.sorr[0] = Txt[60*i+4];
		wd_lv_c552r.iorw[0] = Txt[60*i+5];
		memcpy(wd_lv_c552r.stime  ,(Txt+60*i)+5,  sizeof(wd_lv_c552r.stime)-1);
		memcpy(wd_lv_c552r.etime  ,(Txt+60*i)+9,  sizeof(wd_lv_c552r.etime)-1);
		memcpy(wd_lv_c552r.totnum  ,(Txt+60*i)+13, sizeof(wd_lv_c552r.totnum)-1);
		memcpy(wd_lv_c552r.brate  ,(Txt+60*i)+21, sizeof(wd_lv_c552r.brate)-1);
		memcpy(wd_lv_c552r.iorwrate  ,(Txt+60*i)+27, sizeof(wd_lv_c552r.iorwrate)-1);
		memcpy(wd_lv_c552r.timerate  ,(Txt+60*i)+33, sizeof(wd_lv_c552r.timerate)-1);
		memcpy(wd_lv_c552r.price  ,(Txt+60*i)+39, sizeof(wd_lv_c552r.price)-1);
		memcpy(wd_lv_c552r.txamt  ,(Txt+60*i)+45, sizeof(wd_lv_c552r.txamt)-1);

		vtcp_log("[%s][%d]  wd_lv_c552r.sendco===[%s]  ",__FILE__,__LINE__,wd_lv_c552r.sendco);  
		vtcp_log("[%s][%d]  wd_lv_c552r.sdate===[%d]  ",__FILE__,__LINE__,wd_lv_c552r.sdate);  
		vtcp_log("[%s][%d]  wd_lv_c552r.edate===[%d]  ",__FILE__,__LINE__,wd_lv_c552r.edate);  

		if (ret = Lv_c552r_Ins(wd_lv_c552r, g_pub_tx.reply) != DB_OK)
		{
			vtcp_log("%s,%d,插入错误ret=[%d]\n",__FILE__,__LINE__,ret);
			Lv_pkgreg_Debug(&wd_lv_c552r);
			sprintf(acErrMsg,"插入CMT552小额收费通知报文失败");
			return(-1);
		}
		vtcp_log("[%s][%d]   Hv_orno_brno_Sel()第[%d]笔  ",__FILE__,__LINE__,wd_lv_c552r.seqno);

	}
	return(0);
}

/***************************************************************************/
/* 文件名称: int lv_deal_cmt500(TLVPACKAGE *lv)           */
/* 功能描述: CMT500小额系统状态变更通知               */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_cmt500(TLVPACKAGE *lv)
{
	int iRc=0;
	char *ptr;
	char pcOldDay[9];
	char pcNewDay[9];
	char pcHolFlag[2];/****节假日标志****/
	char pcChgNode[5];
	char pcSysStat[3];
	char pcChgTime[15];
	char pcMaybeTime[5];/****日切参考时点****/
	char pcContent[61];/****附言****/
	struct lv_sysctl_c sLv_sysctl;  

	memset(pcOldDay,'\0',sizeof(pcOldDay));
	memset(pcNewDay,'\0',sizeof(pcNewDay));
	memset(pcHolFlag,'\0',sizeof(pcHolFlag));
	memset(pcChgNode,'\0',sizeof(pcChgNode));
	memset(pcSysStat,'\0',sizeof(pcSysStat));
	memset(pcChgTime,'\0',sizeof(pcChgTime));
	memset(pcMaybeTime,'\0',sizeof(pcMaybeTime));
	memset(pcContent,'\0',sizeof(pcContent));
	lv_get_tag(lv,"CJ6",pcOldDay);
	lv_get_tag(lv,"CJ2",pcNewDay);
	lv_get_tag(lv,"BSC",pcHolFlag);
	lv_get_tag(lv,"C41",pcChgNode);
	lv_get_tag(lv,"CI5",pcSysStat);
	lv_get_tag(lv,"CK4",pcChgTime);
	lv_get_tag(lv,"BS0",pcMaybeTime);
	lv_get_tag(lv,"72A",pcContent);
	ptr = pcLvGetDay_work();
	if (ptr==NULL)
	{
		vtcp_log("%s,%d,访问系统参数表失败",__FILE__,__LINE__);
		sprintf(acErrMsg,"访问系统参数表失败");
		WRITEMSG
			return(-1);
	}
	vtcp_log("%s,%d,  ptr====[%s]  pcOldDay===[%s] \n",__FILE__,__LINE__,ptr,pcOldDay);

	if (memcmp(pcOldDay,ptr,sizeof(pcOldDay)-1)<0)
	{
		/*** 报文传来的原日期 <= 当前工作日期, 说明已经换过日了 ****/
		vtcp_log("%s,%d,已经换过日了[%.8s][%.8s]\n",__FILE__,__LINE__,pcOldDay,ptr);
		sprintf(acErrMsg,"已经换过日了");
		WRITEMSG
			return(-1);
	}
	memcpy(pcOldDay,ptr,sizeof(pcOldDay)-1);/****保存旧日期****/
	/**
	if(memcmp(pcSysStat,"02",2)!=0 && pcHolFlag[0]!='1')
	**/

	iRc=Lv_sysctl_Dec_Upd(g_pub_tx.reply,"1=1");
	if (iRc)
	{
		vtcp_log("%s,%d,数据库错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"数据库错误");  
		WRITEMSG
			return(-1);
	}
	memset(&sLv_sysctl, 0 , sizeof(sLv_sysctl));
	iRc=Lv_sysctl_Fet_Upd( &sLv_sysctl , g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d,数据库错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"数据库错误");  
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s,%d], sLv_sysctl.tx_date====[%d] ",__FILE__,__LINE__,sLv_sysctl.tx_date);  
	vtcp_log("[%s,%d], pcNewDay====[%s] ",__FILE__,__LINE__,pcNewDay);  

	memset(sLv_sysctl.filler,0,sizeof(sLv_sysctl.filler)-1);
	memcpy(sLv_sysctl.filler, pcOldDay, 8);  /* 前一天日期 */
	sLv_sysctl.tx_date = atoi(pcNewDay);  /* 新日期 */

	memcpy(sLv_sysctl.chgnode,pcChgNode,sizeof(sLv_sysctl.chgnode)-1);
	memcpy(sLv_sysctl.curstat,pcSysStat,sizeof(sLv_sysctl.curstat)-1);
	memcpy(sLv_sysctl.chgtime,pcChgTime,sizeof(sLv_sysctl.chgtime)-1);
	memcpy(sLv_sysctl.message,pcContent,sizeof(sLv_sysctl.message)-1);

	if (memcmp(pcSysStat,"02",2) !=0 ) /****启运****/
	{
		sLv_sysctl.stat[0]='1';
	}
	else     /****收到停运或者节假日的日切通知****/
	{
		sLv_sysctl.stat[0]='0';
	}
	iRc = Lv_sysctl_Upd_Upd( sLv_sysctl ,g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d,数据库错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"数据库错误");  
		WRITEMSG
			return(-1);
	}
	lv_deal_chgday(pcOldDay,pcNewDay);/****处理换日的一些操作****/
	Lv_sysctl_Clo_Upd();
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_chgday(char *pcOldDay,char *pcNewDay)       */
/* 功能描述: 换日的工作                      */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_chgday(char *pcOldDay,char *pcNewDay)
{
	char sql_actype[3];
	char sql_brno[8];
	char sql_lastday[9];
	int i=0,iRc=0;
	char del_where[100];

	struct lv_parad_c wd_lv_parad;
	struct lv_chk842_m_c lv_chk842_m;
	struct lv_chk842_d_c lv_chk842_d;
	struct lv_chk850_m_c lv_chk850_m;
	struct lv_chk850_d_c lv_chk850_d;
	struct lv_chk851_m_c lv_chk851_m;
	struct lv_chk851_d_c lv_chk851_d;
	struct lv_chk852_m_c lv_chk852_m;
	struct lv_chk852_d_c lv_chk852_d; 
	memset(&lv_chk842_m, 0 ,sizeof(lv_chk842_m));
	memset(&lv_chk842_d, 0 ,sizeof(lv_chk842_d));
	memset(&lv_chk850_m, 0 ,sizeof(lv_chk850_m));
	memset(&lv_chk850_d, 0 ,sizeof(lv_chk850_d));
	memset(&lv_chk851_m, 0 ,sizeof(lv_chk851_m));
	memset(&lv_chk851_d, 0 ,sizeof(lv_chk851_d));
	memset(&lv_chk852_m, 0 ,sizeof(lv_chk852_m));
	memset(&lv_chk852_d, 0 ,sizeof(lv_chk852_d));
	memset(sql_lastday , 0 ,sizeof(sql_lastday));
	memset(sql_actype  , 0 ,sizeof(sql_actype));
	memset(sql_brno    ,0  ,sizeof(sql_brno));

	/****序号表****/
	/****历史数据保存1周****/
	/****以下计算方法不正确  delete at 20120113
	i=atoi(pcOldDay)-7;
	if ((i%100)>50 || (i/100)%100==0)
	{
		i=(i/100)*100+25;
	}
	if ((i/100)%100>50 || (i/100)%100==0)
	{
		i=(i/10000)*10000+12*100+i%100;
	}
	****/
	/****修改为下面方式，时间也延长为10天 add at 20120113****/
	i=atoi(pcOldDay);
	i=pub_base_daynumSUB(i,10);
	/**** add end 20120113****/
	sprintf(sql_lastday,"%08d",i);
	/****清除lv_recvlist*****/
	/*begin modify by LiuHuafneg txday-->wt_date */
	lvca.rtcode = sql_execute( " delete from lv_recvlist where tx_date<='%s'", sql_lastday);
	/*end modify by LiuHuafneg */
	if (lvca.rtcode && lvca.rtcode!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"数据库错误");
		WRITEMSG
			return(-1);
	}
	/* add by LiuHuafeng 20060215 */
	/* 清除lv_chk842_d lv_chk842_m lv_chk850_m lv_chk850_d lv_chk851_m lv_chk851_d lv_chk852_m lv_chk852_d */

	/* 842和851的清除放到 收到842报文时清除  Modified by ChenMing 20070306
	deldb("lv_chk842_m", NULL);
	deldb("lv_chk842_d", NULL); 
	memset(del_where   , 0 ,sizeof(del_where));
	sprintf(del_where, "zc_date = %s", pcOldDay);
	del_where[sizeof(del_where)-1] = 0;
	deldb("lv_chk842_d", del_where);*/

	deldb("lv_chk850_m", NULL);
	deldb("lv_chk850_d", NULL);

	/* 842和851的清除放到 收到842报文时清除  Modified by ChenMing 20070306
	deldb("lv_chk851_m", NULL);
	deldb("lv_chk851_d", NULL); 
	memset(del_where   , 0 ,sizeof(del_where));
	sprintf(del_where, "zc_date = %s", pcOldDay);
	del_where[sizeof(del_where)-1] = 0;
	deldb("lv_chk851_d", del_where); */

	deldb("lv_chk852_m", NULL);
	deldb("lv_chk852_d", NULL);
	/*****处理定时的更新参数****/
	lvca.rtcode = Lv_parad_Dec_Sel(g_pub_tx.reply, "eff_date <= '%s' and flg = '0' ", pcNewDay);
	if (DB_OK != lvca.rtcode)
	{
		sprintf(acErrMsg,"定义游标出错 - %d", lvca.rtcode);
		WRITEMSG
			return(-1);
	}
	while (1)
	{
		memset(&wd_lv_parad,'\0',sizeof(wd_lv_parad));
		lvca.rtcode = Lv_parad_Fet_Sel(&wd_lv_parad, g_pub_tx.reply);
		if (lvca.rtcode!=DB_NOTFOUND && lvca.rtcode!=0)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误");
			WRITEMSG
				Lv_parad_Clo_Sel();
			return(-1);
		}
		if (lvca.rtcode==DB_NOTFOUND)
		{
			break;
		}
		iRc=iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,
			wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
		if (iRc)
		{
			vtcp_log("%s,%d,设置参数主表错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"设置参数主表错误");
			WRITEMSG
				Lv_parad_Clo_Sel();
			return(-1);
		}
	}
	Lv_parad_Clo_Sel();
	lvca.rtcode=0;
	lvca.rtcode = sql_execute(" update lv_parad set flg='1' where eff_date<='%s' and flg='0'", pcNewDay) ;
	if (lvca.rtcode!=0 && lvca.rtcode!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,设置参数辅表错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"设置辅数主表错误");
		WRITEMSG
			return(-1);
	}
	/****************************************************************
	note:add by LiuHuafeng 20060317 将所有5天前的往帐借记业务没有回执的
	*    和所有5天前的来帐借记业务没有回执的明细状态设置为已止付状态
	***************************************************************/
	{
		long lSday=0;
		long lEday=0;
		lEday= apatoi(cGetSysDate(),8);
		lSday = pub_base_daynumSUB(lEday, 5);
		vtcp_log("[%s][%d]lSday=[%ld]",__FILE__,__LINE__,lSday);
		/* 20060327 txday--->wt_date lhf *//**modif by xyy 2007-4-18 13:47 NEWYX**/
		sql_execute(" update lv_pkgreg set lv_sts='D' where lv_sts in ('1','4') and wt_date <='%ld' and pkgno='002' ", lSday);
		if (lvca.rtcode!=0 && lvca.rtcode!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,更新5日前普通借记往帐错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"更新5日前普通借记往帐错误");
			WRITEMSG
				return(-1);
		}
		/* 20060327 txday--->wt_date lhf *//**modif by xyy 2007-4-18 13:47 NEWYX**/
		sql_execute(" update lv_pkgreg set lv_sts='D' where lv_sts in ('1','4') and wt_date <='%ld' and pkgno='006' ", lSday);
		if (lvca.rtcode!=0 && lvca.rtcode!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,更新5日前定期借记往帐错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"更新5日前定期借记往帐错误");
			WRITEMSG
				return(-1);
		}
		/* 20060327 txday--->wt_date lhf *//**modif by xyy 2007-4-18 13:47 NEWYX**/
		sql_execute(" update lv_pkgreg set lv_sts='V' where lv_sts in ('H','Q','S') and wt_date <='%ld' and pkgno='008' ", lSday);
		if (lvca.rtcode!=0 && lvca.rtcode!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,更新5日前普通借记来帐错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"更新5日前普通借记来帐错误");
			WRITEMSG
				return(-1);
		}
		/* 20060327 txday--->wt_date lhf *//**modif by xyy 2007-4-18 13:47 NEWYX**/
		sql_execute(" update lv_pkgreg set lv_sts='V' where lv_sts in ('H','Q','S') and wt_date <='%ld' and pkgno='011' ", lSday);
		if (lvca.rtcode!=0 && lvca.rtcode!=DB_NOTFOUND)
		{
			vtcp_log("%s,%d,更新5日前定期借记来帐错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"更新5日前定期借记来帐错误");
			WRITEMSG
				return(-1);
		}
	}
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt502(TLVPACKAGE *lv)           */
/* 功能描述: CMT502小额公共数据变更通知                  */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                          */
/***************************************************************************/
/****下面为小薛所改函数****/
int lv_deal_cmt502(TLVPACKAGE *lv)
{
	int iRc=0;
	int iRecCnt=0;
	int i=0,offset=0;
	char pcTmpBuf[32];
	char cWtdate[9];
	char cEffdate[9];
	char cChgdate[9];
	struct lv_parad_c wd_lv_parad;
	char cTmpBuf[3000];

	memset(cTmpBuf,'\0',sizeof(cTmpBuf));

	memset(&wd_lv_parad,'\0',sizeof(wd_lv_parad));
	memset(cWtdate,0,sizeof(cWtdate));
	memset(cEffdate,0,sizeof(cEffdate));
	memset(cChgdate,0,sizeof(cChgdate));
	lv_get_tag(lv,"30A",cWtdate);
	wd_lv_parad.wt_date = apatoi(cWtdate,8);
	vtcp_log("[%s][%d]cWtdate=[%s]",__FILE__,__LINE__,cWtdate);
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B68",pcTmpBuf);
	get_zd_long("0060",&wd_lv_parad.tx_time);
	vtcp_log("[%s][%d]pcTmpBuf=[%c]",__FILE__,__LINE__,pcTmpBuf[0]);
	iRecCnt = atoi(pcTmpBuf);
	offset = 0;
	for (i=0; i<iRecCnt; i++)
	{
		wd_lv_parad.flg[0]='0';/**未处理**/
		vtcp_log("[%s][%d]cTmpBuf=[%s]",__FILE__,__LINE__,cTmpBuf);
		iRc=lv_get_tag(lv,"B88",cTmpBuf );
		vtcp_log("[%s][%d]cTmpBuf=[%s]",__FILE__,__LINE__,cTmpBuf);
		wd_lv_parad.chg_type[0] = cTmpBuf[offset];

		/****变更类型,1-增加,2-更新,3-删除****/
		offset+=1;
		vtcp_log("[%s][%d]offset=[%d]",__FILE__,__LINE__,offset);
		vtcp_log("[%s][%d]chg_type=[%c]",__FILE__,__LINE__,wd_lv_parad.chg_type[0]);
		/****生效类型,0-立即,1-定时****/
		wd_lv_parad.eff_type[0] = cTmpBuf[offset];
		offset+=1;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]eff_type=[%c]",__FILE__,__LINE__,wd_lv_parad.eff_type[0]);
		/****生效日期****/

		wd_lv_parad.eff_date = apatoi(cTmpBuf+offset,8);
		offset+=8;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]eff_date=[%f]",__FILE__,__LINE__,wd_lv_parad.eff_date);
		/****变更日期****/

		wd_lv_parad.chg_date = apatoi(cTmpBuf+offset,8);
		offset+=8;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]chg_date=[%f]",__FILE__,__LINE__,wd_lv_parad.chg_date);
		/****公共数据代码****/
		memset(wd_lv_parad.par_code,'\0',sizeof(wd_lv_parad.par_code));
		memcpy(wd_lv_parad.par_code,cTmpBuf+offset,8);
		offset+=8;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]par_code=[%s]",__FILE__,__LINE__,wd_lv_parad.par_code);
		/****公共数据名称****/
		memset(wd_lv_parad.par_name,'\0',sizeof(wd_lv_parad.par_name));
		memcpy(wd_lv_parad.par_name,cTmpBuf+offset,40);
		offset+=40;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]par_name=[%s]",__FILE__,__LINE__,wd_lv_parad.par_name);
		/****公共数据值****/
		memset(wd_lv_parad.par_value,'\0',sizeof(wd_lv_parad.par_value));
		memcpy(wd_lv_parad.par_value,cTmpBuf+offset,250);
		offset+=250;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]par_value=[%s]",__FILE__,__LINE__,wd_lv_parad.par_value);
		/****附言****/
		memset(wd_lv_parad.filler,'\0',sizeof(wd_lv_parad.filler));
		memcpy(wd_lv_parad.filler,cTmpBuf+offset,60);
		offset+=60;
		vtcp_log("[%s][%d]offset=[%d]errcode=[%d]",__FILE__,__LINE__,offset,iRc);
		vtcp_log("[%s][%d]par_filler=[%s]",__FILE__,__LINE__,wd_lv_parad.filler);
		/***
		if (iRc)
		{
		vtcp_log("%s,%d,报文错误[%d],记录序号[%d/%d]\n",__FILE__,__LINE__,iRc,i,iRecCnt);
		sprintf(acErrMsg,"报文错误");
		WRITEMSG
		return(-1);
		}
		***/
		/****判断是否需要立即更新****/
		if (wd_lv_parad.eff_type[0]=='0' || wd_lv_parad.eff_date-apatoi(pcLvGetDay(),8)<=0)
		{
			vtcp_log("%s,%d,here11111",__FILE__,__LINE__);
			iRc = iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
			if (iRc)
			{
				vtcp_log("%s,%d,设置参数主表错误[%d]\n",__FILE__,__LINE__,iRc);
				sprintf(acErrMsg,"设置参数主表错误");
				WRITEMSG
					return(-1);
			}
			wd_lv_parad.flg[0]='1';/***置为已处理****/
		}
		vtcp_log("%s,%d,here22222",__FILE__,__LINE__);
		iRc = Lv_parad_Ins(wd_lv_parad, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,here3333333",__FILE__,__LINE__);
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,iRc);
			Lv_parad_Debug(&wd_lv_parad);
			sprintf(acErrMsg,"数据库错误");
			WRITEMSG
				return(-1);
		}
	}
	return(0);
}

/**************** Begin  Added by ChenMing 2007.04.14. NEWYX  *************/

/*********************************************************************/
/* 文件名称: int lv_deal_cmt503(TLVPACKAGE *lv)				               */
/* 功能描述: CMT503  业务权限控制通知报文  	               */
/* 创建日期: 2007.04.14.                                                                       */
/* 作者:     ChenMing                                                                                 */
/* 说明:     					                                                              */
/*********************************************************************/
int lv_deal_cmt503(TLVPACKAGE *lv)
{
	int iRc=0;
	long lOrderno=0;
	char pcTmpBuf[2001];
	char cTmpBuff[1024*8];
	struct lv_oths_c wd_lv_oths;
	memset(&wd_lv_oths,'\0',sizeof(wd_lv_oths));
	memset(cTmpBuff ,0,sizeof(cTmpBuff));

	/* 委托日期 */
	memset(pcTmpBuf,0,sizeof(pcTmpBuf));
	lv_get_tag(lv,"30A",pcTmpBuf);
	zip_space(pcTmpBuf);
	vtcp_log("[%s][%d] 30A===[%s]",__FILE__,__LINE__,pcTmpBuf);
	memcpy(wd_lv_oths.wtday ,pcTmpBuf,8);
	vtcp_log("[%s][%d] wd_lv_oths.wtday===[%.8s]",__FILE__,__LINE__,wd_lv_oths.wtday);
	/* 发起节点号 */
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"C40",pcTmpBuf);
	zip_space(pcTmpBuf);
	vtcp_log("[%s][%d] C40===[%s]",__FILE__,__LINE__,pcTmpBuf);
	memcpy(wd_lv_oths.brno, pcTmpBuf, 4);
	vtcp_log("[%s][%d] wd_lv_oths.brno===[%.4s]",__FILE__,__LINE__,wd_lv_oths.brno);
	/* 直接参与者数目 ===  暂时放到orbrno 字段中*/
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B69",pcTmpBuf);
	zip_space(pcTmpBuf);
	vtcp_log("[%s][%d]  B69===[%s]",__FILE__,__LINE__,pcTmpBuf);
	memcpy(wd_lv_oths.orbrno, pcTmpBuf,sizeof(wd_lv_oths.orbrno)-1);
	vtcp_log("[%s][%d] wd_lv_oths.orbrno===[%.12s]",__FILE__,__LINE__,wd_lv_oths.orbrno);
	/* 业务权限控制清单 */	
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B89", cTmpBuff);
	zip_space(cTmpBuff);
	memcpy(pcTmpBuf,cTmpBuff,sizeof(pcTmpBuf)-1);
	vtcp_log("[%s][%d]  B89===[%s]",__FILE__,__LINE__,pcTmpBuf);
	memcpy(wd_lv_oths.content, pcTmpBuf, sizeof(wd_lv_oths.content)-1);
	vtcp_log("[%s][%d] wd_lv_oths.content===[%s]",__FILE__,__LINE__,wd_lv_oths.content);
	/* 生成一个orderno (  主键 ) */	
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));	
	lOrderno = iLvGetOrderno();
	vtcp_log("%s,%d, ======lOrderno=[%ld] \n",__FILE__,__LINE__,lOrderno);
	sprintf(pcTmpBuf,"%08d",lOrderno);
	zip_space(pcTmpBuf);
	memcpy(wd_lv_oths.orderno, pcTmpBuf,sizeof(wd_lv_oths.orderno)-1);
	vtcp_log("%s,%d, wd_lv_oths.orderno=[%.8s] \n",__FILE__,__LINE__,wd_lv_oths.orderno);
	vtcp_log("[%s,%d]  开始插入lv_oths ",__FILE__,__LINE__);
	memcpy(wd_lv_oths.cmtno,"503",sizeof(wd_lv_oths.cmtno)-1);
	wd_lv_oths.lwflg[0] ='1';
	iRc = Lv_oths_Ins(wd_lv_oths, g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d, Lv_oths_Ins  出错[%d]\n",__FILE__,__LINE__,iRc);
		Lv_oths_Debug(&wd_lv_oths);
		return(-1);
	}
	iRc=lv_deal_cmt503_detail(lv);
	if (iRc)
	{
		vtcp_log("%s,%d, Lv_c503r_Ins  出错[%d]\n",__FILE__,__LINE__,iRc);
		return(-1);
	}
	return(0);
}

/*********************************************************************/
/* 文件名称: int lv_deal_cmt504(TLVPACKAGE *lv)				               */
/* 功能描述: CMT504  下级节点变更通知报文 	               */
/* 创建日期: 2007.04.14.                                                                       */
/* 作者:     ChenMing                                                                                 */
/* 说明:     					                                                              */
/*********************************************************************/
int lv_deal_cmt504(TLVPACKAGE *lv)
{
	int iRc=0;
	long lOrderno=0;
	char pcTmpBuf[2001];
	struct lv_oths_c wd_lv_oths;
	memset(&wd_lv_oths,'\0',sizeof(wd_lv_oths));

	/* 委托日期 */
	memset(pcTmpBuf,0,sizeof(pcTmpBuf));
	lv_get_tag(lv,"30A",pcTmpBuf);
	vtcp_log("[%s][%d] 30A===[%s]",__FILE__,__LINE__,pcTmpBuf);
	zip_space(pcTmpBuf);
	memcpy(wd_lv_oths.wtday, pcTmpBuf, 8);
	/* 变更记录数目 ===  暂时放到orbrno 字段中*/
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B6A",pcTmpBuf);
	vtcp_log("[%s][%d]  B6A===[%s]",__FILE__,__LINE__,pcTmpBuf);
	zip_space(pcTmpBuf);		
	memcpy(wd_lv_oths.orbrno, pcTmpBuf, 8);
	/* 变更记录清单*/	
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B8A", pcTmpBuf);
	vtcp_log("[%s][%d]  B8A===[%s]",__FILE__,__LINE__,pcTmpBuf);
	zip_space(pcTmpBuf);		
	memcpy(wd_lv_oths.content, pcTmpBuf, sizeof(wd_lv_oths.content)-1);

	/* 生成一个orderno (  主键 ) */	
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));	
	lOrderno = iLvGetOrderno();
	sprintf(pcTmpBuf,"%08d",lOrderno);
	vtcp_log("%s,%d, ======lOrderno=[%ld] ",__FILE__,__LINE__,lOrderno);
	zip_space(pcTmpBuf);		  
	memcpy(wd_lv_oths.orderno, pcTmpBuf, 8);

	memcpy(wd_lv_oths.cmtno,"504",sizeof(wd_lv_oths.cmtno)-1);
	wd_lv_oths.lwflg[0] ='1';

	vtcp_log("[%s,%d]  开始插入lv_oths ",__FILE__,__LINE__);
	iRc = Lv_oths_Ins(wd_lv_oths, g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d, Lv_oths_Ins  出错[%d]\n",__FILE__,__LINE__,iRc);
		Lv_oths_Debug(&wd_lv_oths);
		return(-1);
	}
	/*begin add by LiuHuafeng 20070508 22:51 */
	iRc=lv_deal_cmt504_detail(lv);
	if (iRc)
	{
		vtcp_log("%s,%d, Lv_c503r_Ins  出错[%d]\n",__FILE__,__LINE__,iRc);
		return(-1);
	}
	/*end add by LiuHuafeng 20070508 22:51 */
	return(0);
}

/**************** End    Added by ChenMing 2007.04.14. NEWYX  *************/

/*************************************************************************************************/
/* 文件名称: int iLvSetParam(char *type,char *par_code,char *par_name,char *par_value,char *filler) */
/* 功能描述: 设置参数主表                          */
/* 创建日期: 2005.07.20                                                        */
/* 作者:     SSH                                                                */
/* 说明:                                                */
/*************************************************************************************************/
int iLvSetParam(char *type,char *par_code,char *par_name,char *par_value,char *filler)
{
	/****type=3时为删除,增加和修改相同处理****/
	int iRc=0;
	struct lv_param_c wd_lv_param;
	memset(&wd_lv_param,'\0',sizeof(wd_lv_param));
	strncpy(wd_lv_param.par_code,par_code,sizeof(wd_lv_param.par_code)-1);
	zip_space(wd_lv_param.par_code);
	if (type[0]=='3')
	{
		/****删除****/
		sprintf(lvca.sqlbuf, "par_code = '%s'", wd_lv_param.par_code);
		iRc = deldb("lv_param", lvca.sqlbuf);/***modify by xyy 2007-4-18 20:52 NEWYX***/
		if (iRc)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误\n");
			WRITEMSG
				return(-1);
		}
		return(0);
	}
	/****下面是更新和插入****/
	if (lvca.rtcode = Lv_param_Dec_Upd(g_pub_tx.reply, "par_code = '%s' ", wd_lv_param.par_code ))
	{
		sprintf(acErrMsg,"定义游标出错 - %d", lvca.rtcode);
		WRITEMSG
			return(-1);
	}
	iRc = Lv_param_Fet_Upd(&wd_lv_param, g_pub_tx.reply);
	if (iRc && iRc!=DB_NOTFOUND)
	{
		vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"数据库错误\n");
		WRITEMSG
			return(-1);
	}
	if (iRc==DB_NOTFOUND)
	{
		strncpy(wd_lv_param.par_name,par_name,sizeof(wd_lv_param.par_name)-1);
		strncpy(wd_lv_param.par_value,par_value,sizeof(wd_lv_param.par_value)-1);
		strncpy(wd_lv_param.filler,filler,sizeof(wd_lv_param.filler)-1);
		zip_space(wd_lv_param.par_name);
		zip_space(wd_lv_param.par_value);
		zip_space(wd_lv_param.filler);
		iRc = Lv_param_Ins(wd_lv_param, g_pub_tx.reply);
		if (iRc)
		{
			Lv_param_Clo_Upd();
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误\n");
			WRITEMSG
				return(-1);
		}
	}
	else
	{
		/****已经存在,更新****/
		strncpy(wd_lv_param.par_value,par_value,sizeof(wd_lv_param.par_value)-1);
		strncpy(wd_lv_param.filler,filler,sizeof(wd_lv_param.filler)-1);
		strncpy(wd_lv_param.par_name,par_name,sizeof(wd_lv_param.par_name)-1);
		zip_space(wd_lv_param.par_value);
		zip_space(wd_lv_param.filler);
		iRc = Lv_param_Upd_Upd(wd_lv_param, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误\n");
			WRITEMSG
				return(-1);
		}
	}
	Lv_param_Clo_Upd();
	return(0);
}

/******************************************************************************************/
/* 文件名称: int strqs_2chk842_d(FINISH_QS * str_qs,struct lv_chk842_d_c * wd_lvchk842_d)*/
/* 功能描述: CMT842小额业务包汇总核对报文处理                        */
/* 创建日期: 2005.07.20                                                           */
/* 作者:     SSH                                                                  */
/* 说明:                                                */
/******************************************************************************************/
void strqs_2chk842_d(FINISH_QS * str_qs,struct lv_chk842_d_c * wd_lvchk842_d)
{
	char cCnt[9],*endptr;
	char cTxamt[23];
	memset(cTxamt, 0 , sizeof(cTxamt));
	memset(cCnt, 0,sizeof(cCnt));
	memcpy(wd_lvchk842_d->zc_jd_lx,str_qs->zclx,sizeof(wd_lvchk842_d->zc_jd_lx)-1);
	wd_lvchk842_d->zc_date=apatoi(str_qs->zcday,8);
	memcpy(wd_lvchk842_d->zc_cc,str_qs->zccc,sizeof(wd_lvchk842_d->zc_cc)-1);
	memcpy(wd_lvchk842_d->pkgno,str_qs->pkgno,sizeof(wd_lvchk842_d->pkgno)-1);
	memcpy(cCnt,str_qs->wbcnt,sizeof(cCnt)-1);zip_space(cCnt);
	wd_lvchk842_d->wb_cnt=atoi(cCnt);
	memcpy(cCnt,str_qs->lbcnt,sizeof(cCnt)-1);zip_space(cCnt);
	wd_lvchk842_d->lb_cnt=atoi(cCnt);
	memcpy(cTxamt, str_qs->lbtxamt,sizeof(cTxamt)-1);
	wd_lvchk842_d->lb_tx_amt=strtod(cTxamt, &endptr);
	memset(cTxamt , 0 , sizeof(cTxamt));
	memcpy(cTxamt, str_qs->wbtxamt,sizeof(cTxamt)-1);
	wd_lvchk842_d->wb_tx_amt=strtod(cTxamt, &endptr);
	memcpy(wd_lvchk842_d->pack_sts,PACKSTAT_QS,sizeof(wd_lvchk842_d->pack_sts));
}
/******************************************************************************************/
/* 文件名称: int strUqs_2chk842_d(FINISH_QS * str_qs,struct lv_chk842_d_c * wd_lvchk842_d)*/
/* 功能描述: CMT842小额业务包汇总核对报文处理                        */
/* 创建日期: 2005.07.20                                                           */
/* 作者:     SSH                                                                  */
/* 说明:                                                */
/******************************************************************************************/
void strUqs_2chk842_d(UNFINISH_QS * str_uqs,struct lv_chk842_d_c * wd_lvchk842_d)
{
	char cCnt[9],*endptr;
	char cTxamt[23];
	memset(cTxamt, 0 , sizeof(cTxamt));
	memset(cCnt, 0,sizeof(cCnt));
	memcpy(wd_lvchk842_d->pkgno,str_uqs->pkgno,sizeof(wd_lvchk842_d->pkgno)-1);
	memcpy(wd_lvchk842_d->pack_sts,str_uqs->packstat,sizeof(wd_lvchk842_d->pack_sts)-1);
	memcpy(cCnt,str_uqs->wbcnt,sizeof(cCnt)-1);zip_space(cCnt);
	wd_lvchk842_d->wb_cnt=atoi(cCnt);
	memcpy(cCnt,str_uqs->lbcnt,sizeof(cCnt)-1);zip_space(cCnt);
	wd_lvchk842_d->lb_cnt=atoi(cCnt);
	memcpy(cTxamt, str_uqs->lbtxamt,sizeof(cTxamt)-1);
	wd_lvchk842_d->lb_tx_amt=strtod(cTxamt, &endptr);
	memset(cTxamt , 0 , sizeof(cTxamt));
	memcpy(cTxamt, str_uqs->wbtxamt,sizeof(cTxamt)-1);
	wd_lvchk842_d->wb_tx_amt=strtod(cTxamt, &endptr);
}
int iPrepareMxdzDtl(TLVPACKAGE *lv,int *qscnt)
{
	char h_zcnode[2];
	char h_zc_date [9];
	char h_zcid  [3];
	char h_pkgno [4];
	char h_lw_flg  [2];
	int  offset=0;
	char h_pack_sts[3];
	char cTxday[9];
	MXHD_850_QS str_mxhd_850_qs;
	MXHD_850_QT str_mxhd_850_qt;
	int iQscnt=0;
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(cTxday,lvca.tbsdy,sizeof(cTxday)-1);
	memset(&str_mxhd_850_qs, 0 , sizeof(str_mxhd_850_qs));
	memset(h_zcnode, 0,sizeof(h_zcnode));
	memset(h_zc_date , 0,sizeof(h_zc_date ));
	memset(h_zcid  , 0,sizeof(h_zcid  ));
	memset(h_pkgno , 0,sizeof(h_pkgno ));
	memset(h_lw_flg  , 0,sizeof(h_lw_flg  ));
	memset(h_pack_sts, 0 , sizeof(h_pack_sts));

	vtcp_log("[%s][%d]code=[%s] \n",__FILE__,__LINE__,lv->CMTNo);
	offset=0;
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	{
		int iCount=0;
		int iRet =0;
		/***
		char cBuff850_qs[15*28*(MXHD850_MAX_ZCCC+1)+1];modify by xyy 2010-7-5 20:31:21***/
		char cBuff850_qs[15*84*(MXHD850_MAX_ZCCC+1)*10+1];
		char cTmpbuf[2048];
		memset(cTmpbuf, 0 , sizeof(cTmpbuf));
		memset(cBuff850_qs, 0 ,sizeof(cBuff850_qs));
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		iRet = vCreateMxhd850_qs_buff(cBuff850_qs,&iCount);
		vtcp_log("[%s][%d] tmpbuf=[%s]\n",__FILE__,__LINE__,cTmpbuf);
		if(iRet)
		{
			vtcp_log("[%s][%d] 设置850明细错误!!!![%d]",__FILE__,__LINE__,iRet);
			return -1;
		}
		vtcp_log("[%s][%d] 总计iCount=[%d]条轧差的需要申请!!\n",__FILE__,__LINE__,iCount);
		if(iCount < 1 || iCount >9 )
		{
			vtcp_log("[%s][%d] 得到的轧差日期的条数错误[%d]",__FILE__,__LINE__,iCount);
			return -1;
		}
		vtcp_log("[%s][%d] buff=[%s]\n",__FILE__,__LINE__,cBuff850_qs);
		lv_set_tag(lv,"B85",cBuff850_qs);
		*qscnt=28*MXHD850_MAX_ZCCC*iCount;
		vtcp_log("[%s][%d] 总长度为[%d]\n",__FILE__,__LINE__,*qscnt);
	}
	/* 如果ccpc是根据我发送的报文申请去下传明细，那就会存在一个问题，就是如果我的某一个包的inday不是txday
	的数据那么我就选择不出来，则不会被申请？那该怎么办？所以，提交所有全部的报文
	*/
	/* for test */
	lv_set_tag(lv,"B86","\0");
	/* end test  */
	return 0;
}

int lv_deal_cmt842(TLVPACKAGE *lv)
{
	int iRc,i,iQscnt=0;
	int recnt,index;
	int offset=0;
	int iSocketID=0;
	char cQscnt[7];/* 已经清算状态数目 */
	char cQtcnt[7];/* 其他清算状态数目 */
	double dLbqscnt=0;
	double dLbqtcnt=0;
	double dWbqscnt=0;
	double dWbqtcnt=0;

	double dLbqsamt=0;
	double dLbqtamt=0;
	double dWbqsamt=0;
	double dWbqtamt=0;
	char cTxday[9];
	char respCode[9]; 
	char cDzdate[9];
	char cPackdate[9];
	char respBuf [100];
	TLVPACKAGE lv850;
	FINISH_QS   str_qs;
	UNFINISH_QS str_uqs;
	struct lv_chk842_m_c wd_lvchk842_m;
	struct lv_chk842_d_c wd_lvchk842_d;
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

	iQscnt=0;
	memset(&str_qs , 0 , sizeof(str_qs ));
	memset(&str_uqs, 0 , sizeof(str_uqs));
	memset(&wd_lvchk842_m, 0 , sizeof(wd_lvchk842_m));
	memset(&wd_lvchk842_d, 0 , sizeof(wd_lvchk842_d));
	memset(cQscnt , 0 , sizeof(cQscnt));
	memset(cQtcnt , 0 , sizeof(cQtcnt));
	memset(cDzdate, 0 , sizeof(cDzdate));
	memset(cPackdate,0,sizeof(cPackdate));
	memset(respCode, 0 , sizeof(respCode));
	memset(respBuf , 0 , sizeof(respBuf ));

	/*******************数据初始化******************** 
	收到842时，先清除842和851表的数据  原来在换日时清除 
	Modified by ChenMing  20070306              *****/

	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	deldb("lv_chk842_m", NULL);
	deldb("lv_chk842_d", NULL);
	deldb("lv_chk851_m", NULL);
	deldb("lv_chk851_d", NULL);

	/****检查是否CMT842实时业务冲正应答报文****/
	if (memcmp(lv->CMTCode,"842",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT842包汇总核对报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT842包汇总核对报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

	lv_get_tag(lv,"30A",cPackdate);
	wd_lvchk842_m.pack_date = apatoi(wd_lvchk842_m.pack_date,8);
	lv_get_tag(lv,"B10",wd_lvchk842_m.cash_qs_no);
	lv_get_tag(lv,"B11",cDzdate);
	wd_lvchk842_m.dz_date = apatoi(cDzdate,8);
	lv_get_tag(lv,"B65",cQscnt);zip_space(cQscnt);
	lv_get_tag(lv,"B66",cQtcnt);zip_space(cQtcnt);
	wd_lvchk842_m.qs_zt_cnt=atoi(cQscnt);
	wd_lvchk842_m.qt_zt_cnt=atoi(cQtcnt);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 处理汇总核对报文主表 */
	/****此句前少了lvca.rtcode，modify at 20120114
	Lv_chk842_m_Ins(wd_lvchk842_m, g_pub_tx.reply);
	****/
	lvca.rtcode=Lv_chk842_m_Ins(wd_lvchk842_m, g_pub_tx.reply);
	if (lvca.rtcode)
	{
		vtcp_log("[%s][%d] insert into lv_chk842_m error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"写入汇总核对报文主表错误");
		WRITEMSG
			return -1;
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 处理汇总核对报文明细表 */
	/* 1 处理成功清算的包 */
	for (i=0,offset=0;i<wd_lvchk842_m.qs_zt_cnt;i++)
	{
		memset(&str_qs, 0 , sizeof(str_qs));
		lv_get_tag_multi(lv,"B85",(char*)&str_qs,offset,sizeof(str_qs));
		strqs_2chk842_d(&str_qs,&wd_lvchk842_d);
		lvca.rtcode = Lv_chk842_d_Ins(wd_lvchk842_d, g_pub_tx.reply);
		if (lvca.rtcode)
		{
			vtcp_log("[%s][%d] insert into lv_chk842_d error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"写入汇总核对报文明细表错误");
			WRITEMSG
				return -1;
		}
		offset+=sizeof(str_qs);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	for (i=0,offset=0;i<wd_lvchk842_m.qt_zt_cnt;i++)
	{
		memset(&str_qs, 0 , sizeof(str_qs));
		lv_get_tag_multi(lv,"B86",(char*)&str_qs,offset,sizeof(str_qs));
		strUqs_2chk842_d(&str_uqs,&wd_lvchk842_d);
		lvca.rtcode = Lv_chk842_d_Ins(wd_lvchk842_d, g_pub_tx.reply);
		if (lvca.rtcode)
		{
			vtcp_log("[%s][%d] insert into lv_chk842_d error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"写入汇总核对报文明细表错误");
			WRITEMSG
				return -1;
		}
		offset+=sizeof(str_qs);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	/* 申请下载小额业务包明细核对申请报文 850 */
	memset(&lv850,0,sizeof(TLVPACKAGE));  
	lv_create(&lv850);
	memset(cTxday, 0 , sizeof(cTxday));
	memcpy(cTxday,pcLvGetDay(),sizeof(cTxday)-1);
	lv_set_tag(&lv850,"30A",cTxday);
	lv_set_tag(&lv850,"B10",LV_QSBRNO);
	memset(cTxday, 0 , sizeof(cTxday));

	vtcp_log("[%s][%d] lvca.tbsdy=[%s]\n",__FILE__,__LINE__,lvca.tbsdy);
	vtcp_log("[%s][%d] g_pub_tx.tx_date=[%d]\n",__FILE__,__LINE__,g_pub_tx.tx_date);

	/*  memcpy(cTxday,lvca.tbsdy,sizeof(cTxday)-1); */
	apitoa(g_pub_tx.tx_date, 8, cTxday);  

	vtcp_log("[%s][%d] cTxday=[%s]\n",__FILE__,__LINE__,cTxday);

	lv_set_tag(&lv850,"B11",cTxday);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */

	lv850.opeType = '1';
	lv850.pkgType = 0;
	memcpy(lv850.CMTCode,"850",3);
	memcpy(lv850.workDate,pcLvGetDay_work(),8);

	dLbqscnt=0;
	dLbqtcnt=0;
	dWbqscnt=0;
	dWbqtcnt=0;

	dLbqsamt=0;
	dLbqtamt=0;
	dWbqsamt=0;
	dWbqtamt=0;
	memset(cQscnt , 0 , sizeof(cQscnt));
	memset(cQtcnt , 0 , sizeof(cQtcnt));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	lvca.rtcode=iPrepareMxdzDtl(&lv850,&iQscnt);
	if (lvca.rtcode)
	{
		vtcp_log("[%s][%d] 准备850报文错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"准备850报文错误!");
		WRITEMSG
			lv_destroy(&lv850);    
		return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	sprintf(cQscnt,"%06d",iQscnt);
	lv_set_tag(&lv850,"B65",cQscnt);
	/*
	lv_set_tag(&lv850,"B66","000264");
	*/
	lv_set_tag(&lv850,"B66","000000");
	lv850.opeType = '1';   /** 小额 **/
	lv850.pkgType = 0;     /** 是cmt包 **/
	memcpy(lv850.packDate, lvca.tbsdy, sizeof(lv850.packDate));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	lv_gen_pack(&lv850);
	iRc=lv_initPara(NULL);
	if (iRc)
	{
		vtcp_log("%s,%d,初始化网络参数错误 return = %d!",__FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"初始化网络参数错误!");
		WRITEMSG
			lv_destroy(&lv850);
		return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	iRc = lv_connect(LV_SERVER_BATCH);
	iSocketID = iRc;
	if (iSocketID<=0)
	{
		vtcp_log("%s,%d,连接前置机错误 return = %d!",__FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"连接前置机错误!");
		WRITEMSG
			lv_destroy(&lv850);
		lv_netClose(iSocketID);
		return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 2 处理其他状态的包 */
	iRc = lv_send(iRc,&lv850);
	if (iRc)
	{
		vtcp_log("%s,%d,发送数据包到前置机出错 return = %d!", __FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"发送数据包到前置机出错!");
		WRITEMSG
			lv_destroy(&lv850);
		lv_netClose(iSocketID);
		return(-1);
	}
	lv_reset(&lv850);

	iRc = lv_recv(iSocketID,&lv850);
	if (iRc)
	{
		vtcp_log("%s,%d,接收数据包到前置机出错 return = %d!",
			__FILE__,__LINE__,iRc);
		sprintf(acErrMsg,"从前置机接收数据包出错!");
		WRITEMSG
			lv_destroy(&lv850);
		lv_netClose(iSocketID);
		return(-1);
	}
	lv_get_tag(&lv850,"C42",respCode);
	lv_get_tag(&lv850,"72A",respBuf);
	if ( respCode[3]!='I' )
	{
		vtcp_log("%s,%d,报文发送到前置机失败 ERRNO=%s\nERRNOINFO=%s!",
			__FILE__,__LINE__,respCode,respBuf);
		sprintf(acErrMsg,respBuf);
		WRITEMSG
			lv_netClose(iSocketID);
		lv_destroy(&lv850);
		return(-1);
	}
	lv_netClose(iSocketID);
	lv_destroy(&lv850);
	return(0);
}

void strqs_2chk851_d(MX_FINISH_QS_BODY *str_qs_body,MX_FINISH_QS_HEAD *str_qs_head,struct lv_chk851_d_c *wd_lvchk851_d)
{
	char cCnt[8],*endptr;
	char cTxamt[23];
	memset(cTxamt, 0 , sizeof(cTxamt));
	memset(cCnt, 0 , sizeof(cCnt));
	memcpy(wd_lvchk851_d->lw_flg       ,str_qs_head->lwbs,sizeof(wd_lvchk851_d->lw_flg)-1);
	memcpy(wd_lvchk851_d->zc_jd_lx     ,str_qs_head->zcjdlx,sizeof(wd_lvchk851_d->zc_jd_lx)-1);
	wd_lvchk851_d->zc_date     =apatoi(str_qs_head->zcday,8);
	memcpy(wd_lvchk851_d->zc_cc       ,str_qs_head->zccc,sizeof(wd_lvchk851_d->zc_cc)-1);
	memcpy(wd_lvchk851_d->pkgno     ,str_qs_head->pkgno,sizeof(wd_lvchk851_d->pkgno)-1);
	memcpy(wd_lvchk851_d->pay_qs_no ,str_qs_body->payqsactno,sizeof(wd_lvchk851_d->pay_qs_no)-1);
	wd_lvchk851_d->pack_date    =apatoi(str_qs_body->packday,8);
	memcpy(wd_lvchk851_d->packid     ,str_qs_body->packid,sizeof(wd_lvchk851_d->packid)-1);
	wd_lvchk851_d->qs_date  =apatoi(str_qs_body->qsday,8);
	memcpy(wd_lvchk851_d->pack_sts   ,PACKSTAT_QS,sizeof(wd_lvchk851_d->pack_sts)-1);
	memcpy(cCnt,str_qs_head->cnt,sizeof(cCnt)-1);
	zip_space(cCnt);
	wd_lvchk851_d->cnt=atoi(cCnt);
	memset(cCnt, 0 , sizeof(cCnt));
	memcpy(cCnt,str_qs_body->dtlcnt,sizeof(cCnt)-1);
	wd_lvchk851_d->dtlcnt=atoi(cCnt);
	memcpy(cTxamt,str_qs_body->totamt,sizeof(cTxamt)-1);
	wd_lvchk851_d->tot_amt=strtod(cTxamt,&endptr);
}
int iCheckWithCcpcAndPrepare852(int line,struct lv_chk851_d_c * wp_lvchk851_d,TLVPACKAGE *lv852,int *flag)
{
	vtcp_log("[%s][%d] 调用本行数程序所在行==[%d],flag==[%d]\n",__FILE__,__LINE__,line,*flag);
	vtcp_log("[%s][%d] pack_date[%d]\n",__FILE__,__LINE__,wp_lvchk851_d->pack_date);
	lvca.rtcode=iCheckWithCcpc(wp_lvchk851_d);
	if (lvca.rtcode)
	{
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		return lvca.rtcode;
	}
	if (!memcmp(wp_lvchk851_d->dhcc_sts,"00",2))
	{
		char cTmpBuf[33];
		char cPackdate[9];
		memset(cPackdate,0,sizeof(cPackdate));
		memset(cTmpBuf, 0 , sizeof(cTmpBuf));
		if (*flag==0)
			lv_set_tag(lv852,"B87",wp_lvchk851_d->lw_flg);
		else
			memcpy(cTmpBuf,wp_lvchk851_d->lw_flg,sizeof(wp_lvchk851_d->lw_flg)-1);
		*flag=(*flag)+1;
		apitoa(wp_lvchk851_d->pack_date, sizeof(cPackdate)-1,cPackdate);
		strncat(cTmpBuf,wp_lvchk851_d->pkgno,sizeof(wp_lvchk851_d->pkgno)-1);
		strncat(cTmpBuf,wp_lvchk851_d->pay_qs_no,sizeof(wp_lvchk851_d->pay_qs_no)-1);
		strncat(cTmpBuf,cPackdate,sizeof(cPackdate)-1);
		strncat(cTmpBuf,wp_lvchk851_d->packid,sizeof(wp_lvchk851_d->packid)-1);
		vtcp_log("[%s][%d]开始处理补发情况\ncTmpbuf==[%s] \n",__FILE__,__LINE__,cTmpBuf);
		lv_set_tag_multi(lv852,"B87",cTmpBuf);
		dm_show(&(lv852->dm));
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	}
	return 0;
}
/* 根据lv_chk851_d表中的数据和综合业务各个表中的数据进行比较查找各个包中
来帐少帐，和往帐少帐的情况 */
int iCheckWithCcpc(struct lv_chk851_d_c * wp_lvchk851_d)
{
	struct lv_lbctl_c wd_lv_lbctl;
	struct lv_wbctl_c wd_lv_wbctl;
	memset(&wd_lv_lbctl, 0 , sizeof(wd_lv_lbctl));
	memset(&wd_lv_wbctl, 0 , sizeof(wd_lv_wbctl));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	if (wp_lvchk851_d->lw_flg[0]=='1')
	{
		/* 往包标识 */
		wd_lv_wbctl.pack_date=wp_lvchk851_d->pack_date;
		vtcp_log("[%s][%d] pack_date[%d]\n",__FILE__,__LINE__,wp_lvchk851_d->pack_date);
		vtcp_log("[%s][%d] pack_date[%d]\n",__FILE__,__LINE__,wd_lv_wbctl.pack_date);
		memcpy(wd_lv_wbctl.pay_qs_no,wp_lvchk851_d->pay_qs_no,sizeof(wd_lv_wbctl.pay_qs_no)-1);
		memcpy(wd_lv_wbctl.packid,wp_lvchk851_d->packid,sizeof(wd_lv_wbctl.packid)-1);
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		lvca.rtcode=Lv_wbctl_Sel(g_pub_tx.reply, &wd_lv_wbctl, "pack_date = %d and pay_qs_no = '%s' and packid = '%s' ", wd_lv_wbctl.pack_date, wd_lv_wbctl.pay_qs_no, wd_lv_wbctl.packid);
		if (lvca.rtcode==DB_NOTFOUND)
		{
			memcpy(wp_lvchk851_d->dhcc_sts,"00",2);
		}
		else if (lvca.rtcode!=0)
		{
			Lv_wbctl_Debug(&wd_lv_wbctl);
			vtcp_log("[%s][%d]数据库出现错误 [%d]\n",__FILE__,__LINE__,lvca.rtcode);
			return lvca.rtcode;
		}
		else
		{
			memcpy(wp_lvchk851_d->dhcc_sts,wd_lv_wbctl.packstat,2);
		}
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		return 0;
	}
	if (wp_lvchk851_d->lw_flg[0]=='2')
	{
		/* 来包标识 */
		wd_lv_lbctl.pack_date=wp_lvchk851_d->pack_date;
		memcpy(wd_lv_lbctl.pay_qs_no,wp_lvchk851_d->pay_qs_no,sizeof(wd_lv_lbctl.pay_qs_no)-1);
		memcpy(wd_lv_lbctl.packid,wp_lvchk851_d->packid,sizeof(wd_lv_lbctl.packid)-1);
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

		lvca.rtcode = Lv_lbctl_Sel(g_pub_tx.reply, &wd_lv_lbctl, "pack_date = %ld and packid = '%s' and pay_qs_no = '%s'", wd_lv_lbctl.pack_date, wd_lv_lbctl.packid, wd_lv_lbctl.pay_qs_no);

		if (lvca.rtcode==DB_NOTFOUND)
		{
			memcpy(wp_lvchk851_d->dhcc_sts,"00",2);
		}
		else if (lvca.rtcode!=0)
		{
			Lv_lbctl_Debug(&wd_lv_lbctl);
			vtcp_log("[%s][%d]数据库出现错误 \n",__FILE__,__LINE__,lvca.rtcode);
			return lvca.rtcode;
		}
		else
			memcpy(wp_lvchk851_d->dhcc_sts,wd_lv_lbctl.packstat,2);
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
		return 0;
	}
	return 0;
}
void strUqs_2chk851_d(MX_UNFINISH_QS_BODY *str_unqs_body,MX_UNFINISH_QS_HEAD *str_unqs_head,struct lv_chk851_d_c *wd_lvchk851_d)
{
	char cCnt[8],*endptr;
	char cTxamt[23];
	memset(cTxamt, 0 , sizeof(cTxamt));
	memset(cCnt, 0 , sizeof(cCnt));
	wd_lvchk851_d->lw_flg[0]  =str_unqs_head->lwbs[0];
	memcpy(wd_lvchk851_d->pkgno     ,str_unqs_head->pkgno,sizeof(wd_lvchk851_d->pkgno)-1);
	memcpy(wd_lvchk851_d->pack_sts   ,str_unqs_head->packstat,sizeof(wd_lvchk851_d->pack_sts)-1);
	memcpy(wd_lvchk851_d->pay_qs_no ,str_unqs_body->payqsactno,sizeof(wd_lvchk851_d->pay_qs_no)-1);
	wd_lvchk851_d->pack_date  =apatoi(str_unqs_body->packday,8);
	memcpy(wd_lvchk851_d->packid     ,str_unqs_body->packid,sizeof(wd_lvchk851_d->packid)-1);
	memcpy(cCnt,str_unqs_head->cnt,sizeof(cCnt)-1);
	zip_space(cCnt);
	wd_lvchk851_d->cnt=atoi(cCnt);
	memset(cCnt, 0 , sizeof(cCnt));
	memcpy(cCnt,str_unqs_body->dtlcnt,sizeof(cCnt)-1);
	wd_lvchk851_d->dtlcnt=atoi(cCnt);
	memcpy(cTxamt,str_unqs_body->totamt,sizeof(cTxamt)-1);
	wd_lvchk851_d->tot_amt=strtod(cTxamt,&endptr);
}
/* 小额业务包明细核对应答报文cmt851 */
int lv_deal_cmt851(TLVPACKAGE *lv,char * dzday)
{
	TLVPACKAGE lv852;
	int iRc=0,i=0,j=0,iSocketID=0,iFirstMulit=0;
	int iRetcnt=0;
	int offset=0;
	char cQscnt[7];/* 已经清算状态数目 */
	char cQtcnt[7];/* 其他清算状态数目 */
	char cQsactno[13];/* 发起行清算行号 */
	char respCode[9]; 
	char respBuf [100];
	char cSubcnt[9];
	char cPackdate[9];
	char cDzdate[9];
	char cDelWhere[51];
	int  iSubcnt=0;
	int iSubOffset=0;
	char cWtday[9];
	struct lv_chk851_m_c wd_lvchk851_m;
	struct lv_chk851_d_c wd_lvchk851_d;
	struct lv_chk851_m_c lv_chk851_m;
	struct lv_chk851_d_c lv_chk851_d;
	MX_FINISH_QS_HEAD   str_qs_head;
	MX_FINISH_QS_BODY   str_qs_body;
	MX_UNFINISH_QS_HEAD str_unqs_head;
	MX_UNFINISH_QS_BODY str_unqs_body;
	char cDzday[9];
	char cRetcnt[9];
	memset(cWtday, 0 , sizeof(cWtday));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	iFirstMulit=0;
	memset(respCode, 0 , sizeof(respCode));
	memset(respBuf , 0 , sizeof(respBuf ));
	memset(cRetcnt,0 , sizeof(cRetcnt));
	memset(cDzday,0 , sizeof(cDzday));
	memset(cPackdate,0,sizeof(cPackdate));
	memset(cDzdate,0,sizeof(cDzdate));
	memset(&str_qs_head  ,0 , sizeof(str_qs_head  ));  
	memset(&str_unqs_head,0 , sizeof(str_unqs_head));
	memset(&str_qs_body, 0 , sizeof(str_qs_body));
	memset(&str_unqs_body,0,sizeof(str_unqs_body));
	memset(cQsactno , 0 , sizeof(cQsactno));
	memset(&wd_lvchk851_m, 0 , sizeof(wd_lvchk851_m));
	memset(&wd_lvchk851_d, 0 , sizeof(wd_lvchk851_d));
	memset(cQscnt , 0 , sizeof(cQscnt));
	memset(cQtcnt , 0 , sizeof(cQtcnt));
	memset(cDelWhere,0, sizeof(cDelWhere));
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

	sprintf(cDelWhere, "qs_date <> %ld ", g_pub_tx.tx_date);	
	/*********数据初始化******************/
	deldb("lv_chk851_m", NULL);
	deldb("lv_chk851_d", cDelWhere);/***add by xyy 2010-6-28 16:22:43 for 村镇银行***/

	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	memcpy(cQsactno,LV_QSBRNO,sizeof(cQsactno)-1);
	/****检查是否CMT851实时业务冲正应答报文****/
	if (memcmp(lv->CMTCode,"851",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT851包明细核对应答报文\n",__FILE__,__LINE__,
			lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT851包明细核对应答报文");
		WRITEMSG
			return(-1);
	}
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);

	lv_get_tag(lv,"30A",cPackdate);
	wd_lvchk851_m.pack_date=apatoi(cPackdate,8);
	lv_get_tag(lv,"B10",wd_lvchk851_m.cash_qs_no);
	lv_get_tag(lv,"B11",cDzdate);
	wd_lvchk851_m.dz_date=apatoi(cDzdate,8);
	lv_get_tag(lv,"B65",cQscnt);zip_space(cQscnt);
	lv_get_tag(lv,"B66",cQtcnt);zip_space(cQtcnt);

	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	wd_lvchk851_m.qs_zt_cnt=atoi(cQscnt);
	wd_lvchk851_m.qt_zt_cnt=atoi(cQtcnt);
	vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	/* 处理汇总核对报文主表 */
	Lv_chk851_m_Ins(wd_lvchk851_m, g_pub_tx.reply);
	if (lvca.rtcode)
	{
		vtcp_log("[%s][%d] insert into lv_chk851_m error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);

		sprintf(acErrMsg,"写入汇总核对报文主表错误");
		WRITEMSG
			return -1;
	}
	memset(&lv852,0,sizeof(TLVPACKAGE));  
	lv_create(&lv852);

	/* 处理汇总核对报文明细表 */
	/* 1 处理成功清算的包 */
	memcpy(cWtday,pcLvGetDay(),8);
	lv852.opeType = '1';  
	lv852.pkgType = 0; 
	memcpy(lv852.CMTCode,"852",3);
	memcpy(lv852.workDate,pcLvGetDay_work(),8);

	memset(cDzday, 0 , sizeof(cDzday));
	memcpy(cDzday,dzday,sizeof(cDzday)-1);
	zip_space(cDzday);
	if (strlen(cDzday)<8)
	{
		get_zd_data(DC_DATE,cDzday);
		zip_space(cDzday);
	}
	if (strlen(cDzday)==0)
	{  
		memcpy(cDzday,pcLvGetDay(),sizeof(cDzday)-1);
	}
	lv_set_tag(&lv852,"30A",cWtday);
	lv_set_tag(&lv852,"B10",cQsactno);
	lv_set_tag(&lv852,"B11",cDzday);
	/**
	lv_set_tag(&lv852,"B87","");
	**/

	vtcp_log("[%s][%d] qsztcnt=[%d] \n",__FILE__,__LINE__,wd_lvchk851_m.qs_zt_cnt);
	for (i=0,offset=0;i<wd_lvchk851_m.qs_zt_cnt;i++)
	{
		memset(cSubcnt, 0 , sizeof(cSubcnt));
    vtcp_log("[%s][%d]sizeof(str_qs_head)= [%d]\n",__FILE__,__LINE__,sizeof(str_qs_head));
		lv_get_tag_multi(lv,"B85",(char*)&str_qs_head,offset,sizeof(str_qs_head));
    vtcp_log("[%s][%d]str_qs_head= [%s]\n",__FILE__,__LINE__,&str_qs_head);
		offset+=sizeof(str_qs_head);
    vtcp_log("[%s][%d]str_qs_head= [%s]\n",__FILE__,__LINE__,&str_qs_head);
    vtcp_log("[%s][%d]sizeof(str_qs_head)= [%d]\n",__FILE__,__LINE__,sizeof(str_qs_head));
		memcpy(cSubcnt,str_qs_head.cnt,sizeof(cSubcnt)-1);
		iSubcnt=atoi(cSubcnt);
		vtcp_log("[%s][%d][%d]^^^^^^内部循环次数＝＝＝%d\n",__FILE__,__LINE__,i,iSubcnt);
		for (j=0,iSubOffset=0;j<iSubcnt;j++)
		{
			memset(&str_qs_body, 0 , sizeof(str_qs_body));
			lv_get_tag_multi(lv,"B85",(char*)&str_qs_body,offset+iSubOffset,sizeof(str_qs_body));
			vtcp_log("[%s][%d] subcnt[%d]\n",__FILE__,__LINE__,iSubcnt);
			vtcp_log("[%s][%d] [%.12s] ",__FILE__,__LINE__,str_qs_body.payqsactno);
			vtcp_log("[%.8s] [%.8s] \n",str_qs_body.packday,str_qs_body.packid);
			vtcp_log("[%s][%d][%.8s] [%.22s]\n",__FILE__,__LINE__,str_qs_body.dtlcnt,str_qs_body.totamt);
			vtcp_log("[%s][%d] [%.8s]\n",__FILE__,__LINE__,str_qs_body.qsday);
			strqs_2chk851_d(&str_qs_body,&str_qs_head,&wd_lvchk851_d);
			vtcp_log("[%s][%d] [%s]\n",__FILE__,__LINE__,(char*)&wd_lvchk851_d);
			vtcp_log("wd_lvchk851_d.lw_flg   [%s]", wd_lvchk851_d.lw_flg   );   /* 来往帐务表识 1往帐2来帐 */
			vtcp_log("wd_lvchk851_d.zc_jd_lx [%s]", wd_lvchk851_d.zc_jd_lx );   /* 扎差节点类型 */
			vtcp_log("wd_lvchk851_d.zc_date  [%ld]",wd_lvchk851_d.zc_date  ); /* 轧差日期    **/
			vtcp_log("wd_lvchk851_d.zc_cc    [%s]", wd_lvchk851_d.zc_cc    );   /* 轧差场次 */
			vtcp_log("wd_lvchk851_d.pkgno    [%s]", wd_lvchk851_d.pkgno    );   /* 包类型号 */
			vtcp_log("wd_lvchk851_d.cnt      [%ld]",wd_lvchk851_d.cnt      );   /* 本类包数目 */
			vtcp_log("wd_lvchk851_d.pay_qs_no[%s]", wd_lvchk851_d.pay_qs_no);   /* 发起清算行 */
			vtcp_log("wd_lvchk851_d.pack_date[%ld]",wd_lvchk851_d.pack_date); /* 委托日期    **/
			vtcp_log("wd_lvchk851_d.packid   [%s]", wd_lvchk851_d.packid   );   /* 包id */
			vtcp_log("wd_lvchk851_d.dtlcnt   [%d]", wd_lvchk851_d.dtlcnt   );   /* 明细业务总笔数 */
			vtcp_log("wd_lvchk851_d.tot_amt  [%f]", wd_lvchk851_d.tot_amt  );   /* 包总金额*/
			vtcp_log("wd_lvchk851_d.qs_date  [%ld]",wd_lvchk851_d.qs_date  ); /* 清算日期 */
			vtcp_log("wd_lvchk851_d.pack_sts [%s]", wd_lvchk851_d.pack_sts );   /* ccpc包状态 */
			vtcp_log("wd_lvchk851_d.dhcc_sts [%s]", wd_lvchk851_d.dhcc_sts );   /* dhcc包状态00 表示要重新发送  其他和ccpc标识一致*/
			vtcp_log("wd_lvchk851_d.filler   [%s]", wd_lvchk851_d.filler   );   /* 备注            **/

			lvca.rtcode=iCheckWithCcpcAndPrepare852(__LINE__,&wd_lvchk851_d,&lv852,&iFirstMulit);
			if (lvca.rtcode)
			{
				vtcp_log("[%s][%d]find lwbctl error sqlcode=[%d]\n",__FILE__,__LINE__,lvca.rtcode);

				sprintf(acErrMsg,"核对综合业务包报文和CCPC错误");
				WRITEMSG
					lv_destroy(&lv852);        
				return -1;
			}
			vtcp_log("[%s][%d]lllllloooooookkkkkkk [%d]\n",__FILE__,__LINE__,iFirstMulit);
			if (lvca.rtcode)
			{
				vtcp_log("[%s][%d]find lwbctl error sqlcode=[%d]\n",__FILE__,__LINE__,lvca.rtcode);

				sprintf(acErrMsg,"写入汇总核对报文主表错误");
				WRITEMSG
					lv_destroy(&lv852);        
				return -1;
			}
			vtcp_log("[%s][%d]lllllloooooookkkkkkk [%d]\n",__FILE__,__LINE__,iFirstMulit);
			Lv_chk851_d_Debug(&wd_lvchk851_d);
		  lvca.rtcode = 	Lv_chk851_d_Ins(wd_lvchk851_d, g_pub_tx.reply);
			if (lvca.rtcode)
			{
				vtcp_log("[%s][%d] insert into lv_chk851_d error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
				lv_destroy(&lv852);        
				sprintf(acErrMsg,"写入汇总核对报文明细表错误");
				WRITEMSG
				return -1;
			}
			vtcp_log("[%s][%d]lllllloooooookkkkkkk [%d]\n",__FILE__,__LINE__,iFirstMulit);
			iSubOffset+=sizeof(str_qs_body);
			if (!memcmp(wd_lvchk851_d.dhcc_sts,"00",2))
				iRetcnt++;
			vtcp_log("[%s][%d]lllllloooooookkkkkkk [%d]\n",__FILE__,__LINE__,iFirstMulit);
		}
		offset+=iSubOffset;
	}
	/* 2 处理其他状态的包 */
	for (i=0,offset=0;i<wd_lvchk851_m.qt_zt_cnt;i++)
	{
		lv_get_tag_multi(lv,"B86",(char*)&str_unqs_head,offset,sizeof(str_unqs_head));
		offset+=sizeof(str_unqs_head);
		memset(cSubcnt, 0 , sizeof(cSubcnt));
		memcpy(cSubcnt,str_unqs_head.cnt,sizeof(cSubcnt)-1);
		iSubcnt=atoi(cSubcnt);
		for (j=0;iSubOffset=0;j<iSubcnt,j++)
		{
			memset(&str_unqs_body, 0 , sizeof(str_unqs_body));
			lv_get_tag_multi(lv,"B86",(char*)&str_unqs_body,offset+iSubOffset,sizeof(str_unqs_body));
			strUqs_2chk851_d(&str_unqs_body,&str_unqs_head,&wd_lvchk851_d);
			lvca.rtcode=iCheckWithCcpcAndPrepare852(__LINE__,&wd_lvchk851_d,&lv852,&iFirstMulit);
			if (lvca.rtcode)
			{
				vtcp_log("[%s][%d] find lwbctl error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
				lv_destroy(&lv852);        
				sprintf(acErrMsg,"写入汇总核对报文主表错误");
				WRITEMSG
					return -1;
			}
			Lv_chk851_d_Ins(wd_lvchk851_d, g_pub_tx.reply);
			if (lvca.rtcode)
			{
				vtcp_log("[%s][%d] insert into lv_chk851_d error sqlcode=[%d] \n",__FILE__,__LINE__,lvca.rtcode);
				lv_destroy(&lv852);        
				sprintf(acErrMsg,"写入汇总核对报文明细表错误");
				WRITEMSG
					return -1;
			}
			iSubOffset+=sizeof(str_unqs_head);
			if (!memcmp(wd_lvchk851_d.dhcc_sts,"0",2))
				iRetcnt++;
		}
		offset+=iSubOffset;
	}
	sprintf(cRetcnt,"%08d",iRetcnt);
	lv_set_tag(&lv852,"B67",cRetcnt);
	lv_gen_pack(&lv852);
	if (iRetcnt!=0)
	{
		vtcp_log("[%s][%d] rcnt==[%d]code=[%s] \n",__FILE__,__LINE__,iRetcnt,lv852.CMTCode);
		/*** 开始发送明细核对下在申请报文 ***/
		iRc = lv_initPara(NULL);
		if (iRc)
		{
			vtcp_log("%s,%d,初始化网络参数错误 = %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"");
			WRITEMSG
				lv_destroy(&lv852);
			return(-1);
		}
		iRc = lv_connect(LV_SERVER_BATCH);
		iSocketID = iRc;
		if (iSocketID<=0)
		{
			vtcp_log("%s,%d,return =连接前置机器错误 %d!",__FILE__,__LINE__,iSocketID);
			sprintf(acErrMsg,"");
			WRITEMSG
				lv_netClose(iSocketID);
			lv_destroy(&lv852);
			return(-1);
		}
		iRc = lv_send(iRc,&lv852);
		if (iRc)
		{
			vtcp_log("%s,%d,return =发送数据包到前置机器错误 %d!",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"!");
			WRITEMSG
				lv_netClose(iSocketID);
			lv_destroy(&lv852);
			return(-1);
		}
		lv_reset(&lv852);

		iRc = lv_recv(iSocketID,&lv852);
		if (iRc)
		{
			vtcp_log("%s,%d,接收数据包到前置机出错 return = %d!",
				__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"从前置机接收数据包出错!");
			WRITEMSG
				lv_destroy(&lv852);
			lv_netClose(iSocketID);
			return(-1);
		}
		lv_get_tag(&lv852,"C42",respCode);
		lv_get_tag(&lv852,"72A",respBuf);
		if ( respCode[3]!='I' )
		{
			vtcp_log("%s,%d,报文发送到前置机失败 ERRNO=%s\nERRNOINFO=%s!",
				__FILE__,__LINE__,respCode,respBuf);
			sprintf(acErrMsg,respBuf);
			WRITEMSG
				lv_netClose(iSocketID);
			lv_destroy(&lv852);
			return(-1);
		}
		lv_netClose(iSocketID);
		vtcp_log("[%s][%d] \n",__FILE__,__LINE__);
	}
	lv_destroy(&lv852);
	return(0);
}

vSetMxhd850_qt(int buf_len,char * pkgno,char * pack_sts,char * cBuff,char lwbs)
{
	MXHD_850_QT str_mxhd_850_qt;
	memset(&str_mxhd_850_qt,0,sizeof(str_mxhd_850_qt));
	str_mxhd_850_qt.lwbs[0]=lwbs;
	memcpy(str_mxhd_850_qt.pkgno,pkgno,3);
	memcpy(str_mxhd_850_qt.packstat,pack_sts,2);
	memcpy(cBuff+buf_len,(char*)&str_mxhd_850_qt,sizeof(str_mxhd_850_qt));
}
/***********************************************************
对于晋中商行而言，村镇银行周六日发起的往包业务，咱们申请850时
只申请了当天轧差的，并未申请其他日期轧差的，所以，要申请一下
其他时间清算报文。
modify  by xyy 2010-7-5 20:18:08 增加一个参数 zcday
************************************************************/
vSetMxhd850_qs(int buf_len,char * pkgno,char * zc_cc,char * cBuff,char zc_jd_lx,char lwbs,char * zcday)
{
	MXHD_850_QS str_mxhd_850_qs;
	memset(&str_mxhd_850_qs,0,sizeof(str_mxhd_850_qs));
	str_mxhd_850_qs.lwbs[0]=lwbs;
	str_mxhd_850_qs.zcjdlx[0]=zc_jd_lx;
	/**
	memcpy(str_mxhd_850_qs.zcday,pcLvGetDay(),sizeof(str_mxhd_850_qs)); 这里是系统时间??**/
	/**
	vtcp_log("[%s][%d] 轧差日期为[%s]\n",__FILE__,__LINE__,zcday);**/
	memcpy(str_mxhd_850_qs.zcday,zcday,sizeof(str_mxhd_850_qs)); 
	
	memcpy(str_mxhd_850_qs.zccc,zc_cc,sizeof(str_mxhd_850_qs.zccc));
	memcpy(str_mxhd_850_qs.pkgno,pkgno,sizeof(str_mxhd_850_qs.pkgno));
	memcpy(cBuff+buf_len,(char*)&str_mxhd_850_qs,sizeof(str_mxhd_850_qs));
}
/***************************************************************************/
/* 文件名称: int vCreateMxhd850_qs_buff(char * buff)                      */
/* 功能描述:                                      */
/* 创建日期:                                                       */
/* 作者:                                                                 */
/* 说明:                                                                   */
/***************************************************************************/
vCreateMxhd850_qs_buff(char * buff ,int *cnt)
{
	/* 设计到清算的包有 来帐 001 005 007 008 010 011 
	往帐有 001 005 007 008 009 010 011
	*/
	/******modify by xyy 2010-7-5 20:52:41 增加长度可能申请前几日的对账报文
	char cBuff[15*28*MXHD850_MAX_ZCCC+1];
	****************************************************/
	char cBuff[15*84*MXHD850_MAX_ZCCC*10+1];
	char cZcDate[9];
	long lQsDate =0;
	int i=0;
	int ret=0;
	int iCnt =0;
	
	memset(cZcDate,0,sizeof(cZcDate));
	memset(cBuff, 0 ,sizeof(cBuff));
	
	lQsDate = apatoi(pcLvGetDay(),8);
	
	vtcp_log("[%s][%d] xyy 注意了,到了申请850报文的地方了,清算日期=[%d]!! \n",__FILE__,__LINE__,lQsDate);
	
	ret = distinct_dec_sel( "lv_c254r", "zc_date", " qs_date='%ld' ", lQsDate);
	if(ret)
	{
		vtcp_log("[%s][%d] DEC SQL 语句有错误!!!![%d]",__FILE__,__LINE__,ret);
		return -1;
	}
	while(1)
	{
		double dSum_zc_bal=0;
		memset(cZcDate , 0 , sizeof(cZcDate));
		ret = distinct_fet_sel(cZcDate);
		if(ret == 1403)
		{
			if(iCnt == 0)
			{
				vtcp_log("[%s][%d] 错，没有可申请的轧差日期!! [%d][%d]",__FILE__,__LINE__,iCnt,ret);
				return -1;
			}
			else
			{
				vtcp_log("[%s][%d] 已经最后一条了!",__FILE__,__LINE__);
				break;
			}
		}
		if(ret!=0 && ret !=1403 )
		{
			vtcp_log("[%s][%d] FET SQL 语句有错误!!!![%d]",__FILE__,__LINE__,ret);
			return -1;
		}
		dSum_zc_bal=0.0;
		vtcp_log("[%s][%d] cZcDate=[%s]",__FILE__,__LINE__,cZcDate);
		/* begin add by LiuHuafeng 2010-10-8 19:36:23*/
		ret = sql_sum_double("lv_c254r","zc_amt",&dSum_zc_bal,"zc_date=%s and qs_date=%ld",cZcDate,lQsDate);
		vtcp_log("[%s][%d] cZcDate=[%s]",__FILE__,__LINE__,cZcDate);
		if(pub_base_CompDblVal(dSum_zc_bal ,0.00)==0) continue;
		/**** end by LiuHuafeng 2010-10-8 19:36:32 **/
		for (i=0;i<MXHD850_MAX_ZCCC;i++)
		{
			char czc_cc[3];
			memset(czc_cc, 0 , sizeof(czc_cc));
			sprintf(czc_cc,"%02d",i+1);
			vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'1','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'2','1',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'2','1',cZcDate);
  	
			vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"009",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'1','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'1','2',cZcDate);  
			vSetMxhd850_qs(strlen(cBuff),"001",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"005",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"007",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"008",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"010",czc_cc,cBuff,'2','2',cZcDate);
			vSetMxhd850_qs(strlen(cBuff),"011",czc_cc,cBuff,'2','2',cZcDate);
		}
		iCnt ++;
		vtcp_log("[%s][%d]cBuff=[%s] iCnt=[%d]\n",__FILE__,__LINE__,cBuff,iCnt);
	}
	distinct_clo_sel();
	
	*cnt = iCnt ;
	vtcp_log("[%s][%d] iCnt=[%d] cnt=[%d]",__FILE__,__LINE__,iCnt,*cnt);
	
	memcpy(buff,cBuff,strlen(cBuff));
	buff[strlen(cBuff)]='\0';
	vtcp_log("[%s][%d] 申请850明细完成!!\n",__FILE__,__LINE__);
	return 0;
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt507(TLVPACKAGE *lv)                       */
/* 功能描述: CMT507贷记金额上限变更通知                              */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt507(TLVPACKAGE *lv)
{
	int iRc=0;
	int iRecCnt=0;
	int i=0,offset=0;
	int iPos=0;
	char pcTmpBuf[32];
	char cWtdate[9];
	char cTxtime[7];

	struct lv_parad_c wd_lv_parad;
	memset(&wd_lv_parad,'\0',sizeof(wd_lv_parad));
	memset(cWtdate,0,sizeof(cWtdate));
	memset(cTxtime,0,sizeof(cTxtime));
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B68",pcTmpBuf);
	vtcp_log("[%s][%d]xyy开始处理507报文!!",__FILE__,__LINE__);
	lv_get_tag(lv,"30A",cWtdate);
	wd_lv_parad.wt_date=apatoi(cWtdate,8);
	get_fd_data("0060", cTxtime);
	wd_lv_parad.tx_time=apatoi(cTxtime,8);

	iRecCnt=atoi(pcTmpBuf);
	offset=0;
	/****HSYX BEGIN:金额上限报文变更****/
	for (i=0;i<iRecCnt;i++)
	{
		wd_lv_parad.flg[0]='0';/**未处理**/
		/****注意与502不同,变更类型,1-增加,3-更新,2-删除****/
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.chg_type,offset,1);
		if (wd_lv_parad.chg_type[0]=='2')
			wd_lv_parad.chg_type[0]='3';
		else if (wd_lv_parad.chg_type[0]=='3')
			wd_lv_parad.chg_type[0]='2';
		offset+=1;
		/****生效类型,0-立即,1-定时****/
		wd_lv_parad.eff_type[0]='1';
		/****生效日期****/
		wd_lv_parad.eff_date=wd_lv_parad.wt_date;
		/****变更日期****/
		wd_lv_parad.chg_date=wd_lv_parad.wt_date;
		/****公共数据代码:CREBD+PKG号****/
		memset(wd_lv_parad.par_code,'\0',sizeof(wd_lv_parad.par_code));
		memcpy(wd_lv_parad.par_code,"CREBD",5);
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.par_code+7,offset,5);
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		offset+=5;
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.par_code+1,offset,6);
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		offset+=6;/****跳过参与者类别****/
		/****公共数据名称****/
		memset(wd_lv_parad.par_name,'\0',sizeof(wd_lv_parad.par_name));
		memcpy(wd_lv_parad.par_name,"贷记金额上限",sizeof(wd_lv_parad.par_name)-1);
		/****公共数据值****/
		memset(wd_lv_parad.par_value,'\0',sizeof(wd_lv_parad.par_value));
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.par_value,offset,15);
		offset+=15;
		memset(wd_lv_parad.filler,'\0',sizeof(wd_lv_parad.filler));
		vtcp_log("[%s][%d]par_value===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_value);
		for (iPos=0;iPos<12;iPos++)
		{
			if (wd_lv_parad.par_code[iPos]==' ')
			{
				wd_lv_parad.par_code[iPos]='X';
				vtcp_log("[%s][%d]here xyy\n",__FILE__,__LINE__);
			}
		}
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		if (iRc)
		{
			vtcp_log("%s,%d,报文错误[%d],记录序号[%d/%d]\n",__FILE__,__LINE__,iRc,i,iRecCnt);
			sprintf(acErrMsg,"报文错误");
			WRITEMSG
				return(-1);
		}
		/****判断是否需要立即更新****/
		if (wd_lv_parad.eff_type[0]=='0' || wd_lv_parad.eff_date-apatoi(pcLvGetDay(),8)<=0)
		{
			iRc=iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
			if (iRc)
			{
				vtcp_log("%s,%d,设置参数主表错误\n",__FILE__,__LINE__);
				sprintf(acErrMsg,"设置参数主表错误");
				WRITEMSG
					return(-1);
			}
			wd_lv_parad.flg[0]='1';/***置为已处理****/
		}
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		iRc=Lv_parad_Ins(wd_lv_parad, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误");
			WRITEMSG
				return(-1);
		}
		vtcp_log("[%s][%d]par_code===[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
	}
	/****HSYX END:金额上限报文变更****/
	return(0);
}

/****HSYX BEGIN:代理关系变更****/
/***************************************************************************/
/* 文件名称: int lv_deal_cmt508(TLVPACKAGE *lv)                            */
/* 功能描述: CMT502小额公共数据变更通知                                    */
/* 创建日期: 2006.10.18                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt508(TLVPACKAGE *lv)
{
	int iRc=0;
	int iRecCnt=0;
	int i=0,offset=0;
	char pcOageCode[13];
	char pcTmpBuf[32];
	struct lv_chgbank_c lv_chgbank;
	struct lv_chgbank_c lv_chgbank_upd;
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"B68",pcTmpBuf);
	iRecCnt=atoi(pcTmpBuf);
	offset=0;
	for (i=0;i<iRecCnt;i++)
	{
		memset(&lv_chgbank,'\0',sizeof(lv_chgbank));
		memset(&lv_chgbank_upd,'\0',sizeof(lv_chgbank_upd));
		memset(pcOageCode,'\0',sizeof(pcOageCode));
		/****票交机构代码****/
		iRc=lv_get_tag_multi(lv,"B88",lv_chgbank.bakcode,offset,12);
		offset+=12;
		/****小额代理行号****/
		iRc=lv_get_tag_multi(lv,"B88",lv_chgbank.agecode,offset,12);
		offset+=12;
		/****变更类型,1-增加,2-撤销,3-修改****/
		iRc=lv_get_tag_multi(lv,"B88",lv_chgbank.stat,offset,1);
		if (lv_chgbank.stat[0]=='1'||lv_chgbank.stat[0]=='3')
		{
			lv_chgbank.stat[0]='1';/****有效****/
		}else
		{
			lv_chgbank.stat[0]='0';/****无效****/
		}
		offset+=1;
		/****生效日期****/
		iRc=lv_get_tag_multi(lv,"B88",lv_chgbank.effday,offset,8);
		offset+=8;
		if (iRc)
		{
			vtcp_log("%s,%d,报文错误[%d],记录序号[%d/%d]\n",__FILE__,__LINE__,iRc,i,iRecCnt);
			sprintf( acErrMsg, "报文错误!");
			WRITEMSG
				return(-1);

		}
		vtcp_log("[%s][%d]lv_chgbank.bakcode=[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
		vtcp_log("[%s][%d]lv_chgbank.agecode=[%s]\n",__FILE__,__LINE__,lv_chgbank.agecode);
		vtcp_log("[%s][%d]lv_chgbank.stat=[%s]\n",__FILE__,__LINE__,lv_chgbank.stat);
		iRc=Lv_chgbank_Sel(g_pub_tx.reply, &lv_chgbank, " bakcode='%s' ",lv_chgbank.bakcode);
		if (iRc!=0 && iRc!=100)
		{
			vtcp_log("%s,%d,未找到票交机构[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
			sprintf( acErrMsg, "未找到票交机构!");
			WRITEMSG
				return(-1);
		}
		if (iRc==100)
		{
			/****没有记录,需要新插入****/
			vtcp_log("[%s][%d]没有记录这里要准备插入lv_chgbank\n",__FILE__,__LINE__);
			iRc=Lv_chgbank_Ins(lv_chgbank,g_pub_tx.reply);
			if (iRc)
			{
				Lv_chgbank_Debug(&lv_chgbank);
				vtcp_log("%s,%d,未找到票交机构[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
				sprintf( acErrMsg, "未找到票交机构!");
				WRITEMSG
					return(-1);
			}
		}else
		{
			/****记录已经存在****/
			iRc=Lv_chgbank_Dec_Upd(g_pub_tx.reply, " bakcode = '%s' ",lv_chgbank.bakcode);
			if (iRc)
			{
				Lv_chgbank_Debug(&lv_chgbank);
				vtcp_log("%s,%d,未找到票交机构[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
				sprintf( acErrMsg, "未找到票交机构!");
				WRITEMSG
					return(-1);
			}
			iRc=Lv_chgbank_Fet_Upd(&lv_chgbank_upd,g_pub_tx.reply);
			if (iRc)
			{
				vtcp_log("%s,%d,未找到票交机构[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
				sprintf( acErrMsg, "未找到票交机构!");
				WRITEMSG
					return(-1);
			}
			memcpy(lv_chgbank_upd.agecode, lv_chgbank.agecode, sizeof(lv_chgbank.agecode)-1);
			memcpy(lv_chgbank_upd.oagecode, lv_chgbank.oagecode, sizeof(lv_chgbank.oagecode)-1);
			lv_chgbank_upd.stat[0]=lv_chgbank.stat[0];
			memcpy(lv_chgbank_upd.effday, lv_chgbank.effday, sizeof(lv_chgbank.effday)-1);
			memcpy(lv_chgbank_upd.filler, lv_chgbank.filler, sizeof(lv_chgbank.filler)-1);

			iRc=Lv_chgbank_Upd_Upd(lv_chgbank,g_pub_tx.reply);
			if (iRc)
			{
				vtcp_log("%s,%d,未找到票交机构[%s]\n",__FILE__,__LINE__,lv_chgbank.bakcode);
				sprintf( acErrMsg, "未找到票交机构!");
				WRITEMSG
					return(-1);
			}
		}
		if (iRc)
		{
			printf("%s,%d,访问数据库错误[%d]\n",__FILE__,__LINE__,iRc);
			WRITEMSG
				return(-1);
		}
	}
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt509(TLVPACKAGE *lv)           */
/* 功能描述: CMT509小额公共数据变更通知                  */
/* 创建日期: 2006.10.31                                                    */
/* 作者:     Dongxy                                                        */
/* 说明:                                          */
/***************************************************************************/
int lv_deal_cmt509(TLVPACKAGE *lv)
{
	int iRc=0;
	int iRecCnt=0;
	int i=0,offset=0;
	char pcTmpBuf[32];
	char  cDate[9];
	char cTpreason[6];
	struct lv_parad_c wd_lv_parad;

	memset(&wd_lv_parad,'\0',sizeof(wd_lv_parad));
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	memset(cDate, '\0',sizeof(cDate));

	lv_get_tag(lv,"30A",cDate);
	wd_lv_parad.wt_date=apatoi(cDate,8);
	wd_lv_parad.tx_time=g_pub_tx.tx_time;

	/****公共数据代码****/
	memset(wd_lv_parad.par_code,'\0',sizeof(wd_lv_parad.par_code));
	/****这个tag值一直是01所以不用管它,xyy 2007-4-18 22:08 NEWYX
	iRc=lv_get_tag(lv,"CEF",wd_lv_parad.par_code);
	****/
	lv_get_tag(lv,"B68",pcTmpBuf);

	iRecCnt=atoi(pcTmpBuf);
	vtcp_log("[%s][%d]iRecCnt=[%d]\n",__FILE__,__LINE__,iRecCnt);
	offset=0;
	for (i=0;i<iRecCnt;i++)
	{
		wd_lv_parad.flg[0]='0';/**未处理**/
		/****变更类型,1-增加,3-更新,2-删除****/
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.chg_type,offset,1);
		if (wd_lv_parad.chg_type[0]=='2')
			wd_lv_parad.chg_type[0]='3';
		else if (wd_lv_parad.chg_type[0]=='3')
			wd_lv_parad.chg_type[0]='2';
		offset+=1;

		/****生效类型,0-立即,1-定时****/
		wd_lv_parad.eff_type[0]='1';
		/****生效日期****/

		wd_lv_parad.eff_date=wd_lv_parad.wt_date;
		/****变更日期****/
		wd_lv_parad.chg_date=wd_lv_parad.wt_date;

		/****公共数据值****/
		memset(wd_lv_parad.par_value,'\0',sizeof(wd_lv_parad.par_value));
		memset(cTpreason            ,'\0',sizeof(cTpreason));

		memcpy(wd_lv_parad.par_code,"TPLY",4);
		iRc=lv_get_tag_multi(lv,"B88",cTpreason,offset,5);

		zip_space(cTpreason);
		memcpy(wd_lv_parad.par_value,cTpreason,sizeof(cTpreason)-1);
		vtcp_log("[%s][%d]cTpreason=[%s]\n",__FILE__,__LINE__,cTpreason);
		memcpy(wd_lv_parad.par_code+4,cTpreason,sizeof(cTpreason)-1);
		vtcp_log("[%s][%d]par_code=[%s]\n",__FILE__,__LINE__,wd_lv_parad.par_code);
		offset+=5;
		/****公共数据名称****/
		memset(wd_lv_parad.par_name,'\0',sizeof(wd_lv_parad.par_name));
		iRc=lv_get_tag_multi(lv,"B88",wd_lv_parad.par_name,offset,60);
		offset+=60;

		if (iRc)
		{
			vtcp_log("%s,%d,报文错误[%d],记录序号[%d/%d]\n",__FILE__,__LINE__,iRc,i,iRecCnt);
			sprintf(acErrMsg,"报文错误");
			WRITEMSG
				return(-1);
		}
		/****判断是否需要立即更新****/
		memset(cDate,'\0',sizeof(cDate));
		apitoa(wd_lv_parad.eff_date,sizeof(cDate)-1, cDate);
		vtcp_log("[%s][%d]-------cDate=[%s]\n",__FILE__,__LINE__,cDate);
		if (wd_lv_parad.eff_type[0]=='0' || memcmp(cDate,pcLvGetDay_work(),sizeof(cDate)-1)<=0)
		{
			/****/
			iRc=iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
			if (iRc)
			{
				vtcp_log("%s,%d,设置参数主表错误\n",__FILE__,__LINE__);
				sprintf(acErrMsg,"设置参数主表错误");
				WRITEMSG
					return(-1);
			}
			wd_lv_parad.flg[0]='1';/***置为已处理****/
		}
		iRc=Lv_parad_Ins(wd_lv_parad, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
			sprintf(acErrMsg,"数据库错误");
			WRITEMSG
				return(-1);
		}
	}
	return(0);
}
/***************************************************************************/
/* 文件名称: int iLvChkChgbank(char *bakcode,char *agecode)*/
/* 功能描述: 检查票交机构和代理行的关系*/
/* 创建日期: 2006.10.18                                                    */
/* 作者:     SSH                                                           */
/* 返回值:   0/-1                                                          */
/* 说明:                                                                   */
/***************************************************************************/
int iLvChkChgbank(char *bakcode,char *agecode)
{
	int iRc=0;
	struct lv_chgbank_c lv_chgbank;
	memset(&lv_chgbank,'\0',sizeof(lv_chgbank));
	memcpy(lv_chgbank.bakcode,bakcode,sizeof(lv_chgbank.bakcode)-1);
	iRc=Lv_chgbank_Sel(g_pub_tx.reply, &lv_chgbank, " bakcode='%s' ",lv_chgbank.bakcode);
	if (iRc)
	{
		vtcp_log("[%s][%d]未找到票交机构[%s]errcode=[%d]\n",__FILE__,__LINE__,lv_chgbank.bakcode,iRc);
		sprintf( acErrMsg, "未找到票交机构!");
		WRITEMSG
			return(-1);
	}
	if (lv_chgbank.stat[0]=='0' &&
		memcmp(lv_chgbank.effday,pcLvGetDay(),8)>0)
	{
		vtcp_log("机构[%s]被撤销但未到生效期\n",lv_chgbank.bakcode);
		if (memcmp(agecode,lv_chgbank.agecode,sizeof(lv_chgbank.agecode)-1)!=0)
		{
			vtcp_log("[%s][%d]机构[%s]与代理行[%s]不符\n",__FILE__,__LINE__,lv_chgbank.bakcode,agecode);
			sprintf( acErrMsg, "机构与代理行不符!");
			WRITEMSG
				return(-1);
		}
		return(0);
	}
	if (lv_chgbank.stat[0]=='1')
	{
		if (memcmp(lv_chgbank.effday,pcLvGetDay(),8)>0)
		{
			vtcp_log("[%s][%d]机构[%s]存在但未到生效期\n",__FILE__,__LINE__,lv_chgbank.bakcode);
			if (memcmp(agecode,lv_chgbank.oagecode,sizeof(lv_chgbank.agecode)-1)!=0)
			{
				vtcp_log("机构[%s]与代理行[%s]不符\n",lv_chgbank.bakcode,agecode);
				sprintf( acErrMsg, "机构与代理行不符!");
				WRITEMSG
					return(-1);
			}
			return(0);
		}else
		{
			vtcp_log("[%s][%d]机构[%s]存在且已到生效期\n",__FILE__,__LINE__,lv_chgbank.bakcode);
			if (memcmp(agecode,lv_chgbank.agecode,sizeof(lv_chgbank.agecode)-1)!=0)
			{
				vtcp_log("机构[%s]与代理行[%s]不符\n",lv_chgbank.bakcode,agecode);
				sprintf( acErrMsg, "机构与代理行不符!");
				WRITEMSG
					return(-1);
			}
			return(0);
		}
	}
	else
	{
		vtcp_log("[%s][%d]该票交机构[%s]已被撤销\n",__FILE__,__LINE__,lv_chgbank.bakcode);
		return(-1);
	}
}
/***************************************************************************/
/* 文件名称: int lv_deal_oths(TLVPACKAGE *lv)                              */
/* 功能描述: oths其他报文                                                  */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_oths(TLVPACKAGE *lv)
{
	int iOrderno=0;
	int iRc=0;
	struct lv_oths_c lv_oths;
	memset(&lv_oths,'\0',sizeof(lv_oths));
	memcpy(lv_oths.orbrno,LV_QSBRNO,sizeof(lv_oths.orbrno));
	memcpy(lv_oths.wtday,pcLvGetDay(),sizeof(lv_oths.wtday)-1);
	lv_oths.lwflg[0]='1'; /**来**/
	lv_oths.stat[0]=STAT_LZRECV1;
	iOrderno=iLvGetOrderno();
	if (iOrderno<=0)
	{
		vtcp_log("[%s][%d]===得到支付交易序号错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg, "得到支付交易序号错误!");
		WRITEMSG
			return(-1);
	}
	apitoa(iOrderno,sizeof(lv_oths.orderno)-1,lv_oths.orderno);
	memcpy(lv_oths.brno,LV_CZ_QSBRNO,BRNO_LEN);
	strncpy(lv_oths.content,lv->dm.block.mem,sizeof(lv_oths.content)-1);
	memcpy(lv_oths.cmtno,lv->CMTCode,sizeof(lv_oths.cmtno)-1);
	iRc=Lv_oths_Ins(&lv_oths,g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("[%s][%d]===插入报文登记簿错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg, "插入报文登记簿错误!");
		WRITEMSG
			return(-1);
	}
	return(0);
}
/****HSYX END:代理关系变更****/
/***************************************************************************/
/* 文件名称: int lv_deal_cmt911(TLVPACKAGE *lv)                       */
/* 功能描述: CMT911报文来账处理                                 */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt911(TLVPACKAGE *lv)
{
	int recnt=0,index=0;
	NOTPAY_IN_AREA notpayin;
	struct lv_wbctl_c wd_lv_wbctl;
	int ret=0;
	int iPkgFlag=0;
	char pcPkgNo[4];/****原报文类型号****/
	char pcDataSource[2];/****标识数据来源****/
	char pcPackDay[9];/****原报文日期****/
	char pcPackSender[13];/****原报文发起人****/
	char pcOrderno[21];/****原报文序号  ****/
	char pcRespCode[9];/****处理结果  ****/
	char cTmporderno[9];

	char pcNewOrderno[21];
	struct lv_sendlist_c lv_sendlist;
	struct hv_fzf_c hv_fzf_c;   

	memset(pcNewOrderno,'\0',sizeof(pcNewOrderno));
	memset(pcPackSender,'\0',sizeof(pcPackSender));

	memset(cTmporderno, 0 , sizeof(cTmporderno));
	memset(&notpayin, '\0', sizeof(notpayin));
	memset(&wd_lv_wbctl, '\0', sizeof(wd_lv_wbctl));

	memset(pcRespCode, '\0', sizeof(pcRespCode));
	memset(pcPkgNo, '\0', sizeof(pcPkgNo));
	memset(pcDataSource, '\0', sizeof(pcDataSource));
	memset(pcPackSender, '\0', sizeof(pcPackSender));
	memset(pcPackDay, '\0', sizeof(pcPackDay));
	memset(pcOrderno, '\0', sizeof(pcOrderno));

	memset(&lv_sendlist, '\0', sizeof(lv_sendlist));

	/*********数据初始化******************/

	if (memcmp(lv->CMTCode,"911",sizeof(lv->CMTCode))!=0)
	{
		vtcp_log("%s,%d,来账[%.3s]不是CMT911报文\n",__FILE__,__LINE__,lv->CMTCode);
		sprintf(acErrMsg,"来账不是CMT911报文");
		WRITEMSG
			return(-1);
	}
	lv_get_tag(lv,"02D",pcPkgNo);
	lv_get_tag(lv,"CEL",pcDataSource);
	lv_get_tag(lv,"051",pcPackDay);
	lv_get_tag(lv,"CCO",pcPackSender);
	/**
	lv_get_tag(lv,"0BF",pcOrderno);
	****************************/
	memset(cTmporderno, 0 , sizeof(cTmporderno));
	lv_get_tag(lv,"0BF",cTmporderno);

	/* begin by LiuHuafeng 20060313根据具体情况处理 */
	if (pcDataSource[0]=='1') /**** 1 报头: 按照20位进行处理否则按8位处理****/
	{
		lv_orderno_add0(cTmporderno, sizeof(pcOrderno)-1, pcOrderno);
	}
	else  /* 其他情况是8位 */
	{
		lv_orderno_add0(cTmporderno, 8, pcOrderno);
	}
	/* end by LiuHuafeng 20060313 */

	lv_get_tag(lv,"C42",pcRespCode);
	if (pcPkgNo[0]=='0')  /***PKG包****/
	{
		iPkgFlag=1;
		vtcp_log("%s,%d,======PKG包!=======",__FILE__,__LINE__);
		if (pcDataSource[0]=='1')
		{
			return(0);/****对传报文参考号的情况不与处理****/
		}
		else
		{
			return(0);/***这个分支是走不到的,因为此时肯定传的是920****/
		}
	}
	else/****CMT包***/
	{
		vtcp_log("%s,%d,====CMT包!=======[%s]",__FILE__,__LINE__,pcPkgNo);

		/* Added by ChenMing  2006-12-1 18:30*/
		if ( memcmp(pcPkgNo, "301", 3) && memcmp(pcPkgNo, "302", 3) &&
			memcmp(pcPkgNo, "319", 3) && memcmp(pcPkgNo, "320", 3) &&
			memcmp(pcPkgNo, "321", 3) && memcmp(pcPkgNo, "323", 3) &&
			memcmp(pcPkgNo, "327", 3) && memcmp(pcPkgNo, "328", 3) )
		{
			return(0);  /* 这些报文不用处理了 */
		}
		iPkgFlag=0;
		if (pcDataSource[0]=='1') /****1 报头: 取报文参考号****/
		{
			vtcp_log("%s,%d,pcPackDay==[%s]      pcOrderno==[%s]",__FILE__,__LINE__,pcPackDay,pcOrderno);
			ret = Lv_sendlist_Sel(g_pub_tx.reply, &lv_sendlist, "tx_date=%d and refno='%s' ", atoi(pcPackDay), pcOrderno);
			if (ret)
			{
				vtcp_log("[%s][%d]查询报文发送登记簿Lv_sendlist_Sel()出错!  ret=%d ",__FILE__,__LINE__,ret);
				WRITEMSG
					return -1;
			} 
			vtcp_log("%s,%d,lv_sendlist.orderno==[%s]  lv_sendlist.or_br_no==[%s]",__FILE__,__LINE__,lv_sendlist.orderno,lv_sendlist.or_br_no);

			strcpy(pcNewOrderno, lv_sendlist.orderno);
			strcpy(pcPackSender, lv_sendlist.or_br_no);
			memset(pcOrderno,'\0',sizeof(pcOrderno));
			memcpy(pcOrderno,pcNewOrderno,sizeof(pcOrderno)-1);
			zip_space(pcOrderno);

		}
		if (pcRespCode[3] != 'I')
		{
			switch(atoi(pcPkgNo))
			{
			case 301:
			case 302:
			case 319:
			case 320:
			case 321:
			case 323:
			case 327:
			case 328:
				{
					memset(&hv_fzf_c, 0, sizeof(struct hv_fzf_c));     
					vtcp_log(" wt_date==[%s] or_br_no==[%s]   orderno==[%s]  cmtno==[%s] " , pcPackDay,pcPackSender,pcOrderno,pcPkgNo);
					ret = Hv_fzf_Dec_Upd(g_pub_tx.reply, " otype='2' and pay_qs_no='%s' and orderno='%s' and cmtno='%d'  ", HV_QS_BR_NO, pcOrderno, atoi(pcPkgNo) );
					if (ret)
					{
						vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
						WRITEMSG
							return -1;
					}    
					ret = Hv_fzf_Fet_Upd( &hv_fzf_c, g_pub_tx.reply );
					if ( ret == 100 )  /* 原查询记录不存在 */
					{
						vtcp_log("[%s] [%d]   原查询记录不存在  !",__FILE__,__LINE__ );
						sprintf( acErrMsg, " 原查询记录不存在 ![%d]", ret );
						strcpy ( g_pub_tx.reply , "S029" );
						WRITEMSG
							return -1;  /* 报错 */
					}else if( ret == 0)  /* 找到原记录，则更新原记录 */
					{
						hv_fzf_c.hv_fzf_sts[0] = STAT_WZREJECT;

						ret = Hv_fzf_Upd_Upd( hv_fzf_c, g_pub_tx.reply );
						if ( ret )
						{
							sprintf( acErrMsg, "更新非支付登记薄错[%d]", ret );
							WRITEMSG
								return -1;
						}
					}
					else   /* 出错 */
					{
						sprintf( acErrMsg, "执行Hv_fzf_Fet_Upd 错![%d]", ret );
						WRITEMSG
							return -1;                   
					}
					Hv_fzf_Clo_Upd();                    
				}
			default:
				return(0);
			}      
		}
	}
	return(0);
}
/***************************************************************************/
/* 文件名称: int lv_deal_cmt516(TLVPACKAGE *lv)                       */
/* 功能描述: CMT516净借记可用额度预警及解除通知                           */
/* 创建日期: 2005.07.20                                                    */
/* 作者:     SSH                                                           */
/* 说明:                                                                   */
/***************************************************************************/
int lv_deal_cmt516(TLVPACKAGE *lv)
{
	int iRc=0;
	int iRecCnt=0;
	int i=0,offset=0;
	char pcTmpBuf[32];
	char cWtdate[9];
	char cTxtime[7];
	struct lv_parad_c wd_lv_parad;
	memset(&wd_lv_parad,'\0',sizeof(wd_lv_parad));
	memset(cWtdate,0,sizeof(cWtdate));
	memset(cTxtime,0,sizeof(cTxtime));

	lv_get_tag(lv,"30A",cWtdate);
	wd_lv_parad.wt_date=apatoi(cWtdate,8);
	get_fd_data("0060", cTxtime);
	wd_lv_parad.tx_time=apatoi(cTxtime,8);
	memset(pcTmpBuf,'\0',sizeof(pcTmpBuf));
	lv_get_tag(lv,"BSK",pcTmpBuf);/*1-预警,2-解除***/

	wd_lv_parad.flg[0]='0';/**未处理**/
	/****注意与502不同,变更类型,1-增加,3-更新,2-删除****/
	wd_lv_parad.chg_type[0]=pcTmpBuf[0];
	if (wd_lv_parad.chg_type[0]=='2')
		wd_lv_parad.chg_type[0]='3';
	else if (wd_lv_parad.chg_type[0]=='3')
		wd_lv_parad.chg_type[0]='2';
	/****生效类型,0-立即,1-定时****/
	wd_lv_parad.eff_type[0]='0';
	/****生效日期****/
	wd_lv_parad.eff_date=wd_lv_parad.wt_date;
	/****变更日期****/
	wd_lv_parad.chg_date=wd_lv_parad.wt_date;
	/****公共数据代码:****/
	memset(wd_lv_parad.par_code,'\0',sizeof(wd_lv_parad.par_code));
	memcpy(wd_lv_parad.par_code,"B1K00000",8);
	/****公共数据名称****/
	memset(wd_lv_parad.par_name,'\0',sizeof(wd_lv_parad.par_name));
	memcpy(wd_lv_parad.par_name,"可用净借记额度预警值",sizeof(wd_lv_parad.par_name)-1);
	/****公共数据值****/
	memset(wd_lv_parad.par_value,'\0',sizeof(wd_lv_parad.par_value));
	iRc=lv_get_tag(lv,"B1K",wd_lv_parad.par_value);
	memset(wd_lv_parad.filler,'\0',sizeof(wd_lv_parad.filler));
	iRc=iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
	if (iRc)
	{
		vtcp_log("%s,%d,设置参数主表错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"设置参数主表错误");
		WRITEMSG
			return(-1);
	}
	wd_lv_parad.flg[0]='1';/***置为已处理****/
	iRc=Lv_parad_Ins(wd_lv_parad, g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf(acErrMsg,"数据库错误");
		WRITEMSG
			return(-1);
	}
	/****公共数据代码:****/
	memset(wd_lv_parad.par_code,'\0',sizeof(wd_lv_parad.par_code));
	memcpy(wd_lv_parad.par_code,"B1N00000",8);
	/****公共数据名称****/
	memset(wd_lv_parad.par_name,'\0',sizeof(wd_lv_parad.par_name));
	memcpy(wd_lv_parad.par_name,"可用净借记额度值",sizeof(wd_lv_parad.par_name)-1);
	/****公共数据值****/
	memset(wd_lv_parad.par_value,'\0',sizeof(wd_lv_parad.par_value));
	iRc=lv_get_tag(lv,"B1N",wd_lv_parad.par_value);
	memset(wd_lv_parad.filler,'\0',sizeof(wd_lv_parad.filler));
	iRc=iLvSetParam(wd_lv_parad.chg_type,wd_lv_parad.par_code,wd_lv_parad.par_name,wd_lv_parad.par_value,wd_lv_parad.filler);
	if (iRc)
	{
		vtcp_log("%s,%d,设置参数主表错误\n",__FILE__,__LINE__);
		sprintf(acErrMsg,"设置参数主表错误");
		WRITEMSG
			return(-1);
	}
	wd_lv_parad.flg[0]='1';/***置为已处理****/
	iRc=Lv_parad_Ins(wd_lv_parad, g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d,数据库错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		Lv_parad_Debug(&wd_lv_parad);
		sprintf(acErrMsg,"数据库错误");
		WRITEMSG
			return(-1);
	}
	return(0);
}
/************************************************************
*  根据接收的cmt报文,对需要退回撤销的业务进行状态的更改  *
*  本函数只根据能否退回或撤销等修改lv_pkgreg和wbctl的状态    *
*  函数名: iLv_Reset_Pkg_Stat                  *
************************************************************/
int iLv_Reset_Pkg_Stat(NOTPAY_IN_AREA *wp_notpayin)
{
	char  cCmtno[4];
	char  Hvf_Cmtno[4];
	char  HvW_Packid[9];
	char  cTmpBuff[2];
	int   iCmtno=0,ret=0;

	struct  hv_fzf_c  sHvFzf;
	struct  lv_wbctl_c  sLvWbctl;

	memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
	memset(&sLvWbctl,   '\0', sizeof(struct lv_wbctl_c));

	memset(cCmtno, '\0', sizeof(cCmtno));
	memset(Hvf_Cmtno, '\0', sizeof(cCmtno));
	memset(HvW_Packid, '\0', sizeof(HvW_Packid));
	memset(cTmpBuff , 0 , sizeof(cTmpBuff));
	memcpy(cCmtno, wp_notpayin->cmtno, sizeof(cCmtno)-1);
	cTmpBuff[0]='2';
	iCmtno = apatoi(cCmtno,3);
	vtcp_log("[%s][%d]进入改写原交易状态函数,传入报文是cmt[%d]\n",__FILE__,__LINE__,iCmtno);

	if (memcmp(wp_notpayin->cmtno,"320", 3) == 0)
	{
		memcpy(Hvf_Cmtno, "319", sizeof(Hvf_Cmtno)-1);
		cTmpBuff[0]='1';
	}
	else if (memcmp(wp_notpayin->cmtno,"322", 3) == 0)
	{
		memcpy(Hvf_Cmtno, "321", sizeof(Hvf_Cmtno)-1);
		cTmpBuff[0]='1';
	}
	else if (memcmp(wp_notpayin->cmtno,"324", 3) == 0)
	{
		memcpy(Hvf_Cmtno, "323", sizeof(Hvf_Cmtno)-1);
		cTmpBuff[0]='1';
	}
	else if (memcmp(wp_notpayin->cmtno,"328", 3) == 0)
	{
		memcpy(Hvf_Cmtno, "327", sizeof(Hvf_Cmtno)-1);
		cTmpBuff[0]='1';
	}
	vtcp_log("[%s][%d]====notpay发起行=[%.12s],委托日期=[%.8s],序号=[%.8s]\n",
		__FILE__,__LINE__,wp_notpayin->oorbrno,wp_notpayin->owtday,wp_notpayin->oorderno);
	/*   小额otype='2'   Modified by ChenMing  2006-10-24 16:39 
	ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '1' and or_br_no = '%s' and wt_date = %d and orderno = '%s' and cmtno = '%s'", wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,sizeof(wp_notpayin->owtday)),wp_notpayin->oorderno,Hvf_Cmtno);
	*/

	memcpy(sHvFzf.or_br_no, wp_notpayin->oorbrno, sizeof(sHvFzf.or_br_no)-1 );
	memcpy(sHvFzf.orderno, wp_notpayin->oorderno, sizeof(sHvFzf.orderno)-1 );
	sHvFzf.wt_date = apatoi(wp_notpayin->owtday,sizeof(wp_notpayin->owtday));    

	vtcp_log("[%s][%d]  sHvFzf.or_br_no====[%s] ",__FILE__,__LINE__,sHvFzf.or_br_no);
	vtcp_log("[%s][%d]  sHvFzf.orderno====[%s] ",__FILE__,__LINE__,sHvFzf.orderno);
	vtcp_log("[%s][%d]  sHvFzf.wt_date====[%d] ",__FILE__,__LINE__,sHvFzf.wt_date);    

	vtcp_log("[%s][%d] ====  下面开始更新原申请类报文记录的状态 ====[%d] ",__FILE__,__LINE__);    

	ret = Hv_fzf_Dec_Upd(g_pub_tx.reply," otype = '2' and pay_qs_no='%s' and orderno = '%s' and cmtno = '%s'",
		HV_QS_BR_NO, sHvFzf.orderno,Hvf_Cmtno);
	if (ret)
	{
		vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
		return -1;
	}
	memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
	ret = Hv_fzf_Fet_Upd( &sHvFzf, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "原查询记录不存在!" );
		WRITEMSG
		return -1;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Hv_fzf_Fet_Upd错![%d]", ret );
		WRITEMSG
		return -1;
	}
	if (sHvFzf.hv_fzf_sts[0] != STAT_WZSEND)
	{
		vtcp_log("[%s][%d]应该是4,实际状态=[%c]\n",__FILE__,__LINE__,sHvFzf.hv_fzf_sts[0]);
		sprintf( acErrMsg, "状态错!" );
		WRITEMSG
		return -1;
	}
	/** 下列情况需要找到原止付业务编号 **/
	if ((memcmp(wp_notpayin->cmtno,"328",3)==0||memcmp(wp_notpayin->cmtno,"320",3)==0)
		&& wp_notpayin->rettype == '0')
	{
		memcpy(wp_notpayin->oorderno, sHvFzf.o_orderno, sizeof(sHvFzf.o_orderno)-1);
	}
	if (wp_notpayin->opkgno[0]=='\0')
	{
		memcpy(wp_notpayin->opkgno,sHvFzf.o_cmtno, sizeof(sHvFzf.o_cmtno)-1);
	}
	if (wp_notpayin->opackid[0]=='\0')
	{
		memcpy(wp_notpayin->opackid,sHvFzf.o_packid, sizeof(sHvFzf.o_packid)-1);
	}
	lv_orderno_add0(wp_notpayin->opackid, sizeof(HvW_Packid)-1, HvW_Packid);

	if (wp_notpayin->opackday[0]=='\0')
	{
		apitoa(sHvFzf.o_pack_date, sizeof(wp_notpayin->opackday),wp_notpayin->opackday);
	}
	sHvFzf.res_sts[0] = wp_notpayin->respcode;  /** 是否同意退回 dxy 20060206**/

	vtcp_log("[%s][%d] ====  下面开始更新原往包记录的状态 ====[%d] ",__FILE__,__LINE__);    

	ret = Lv_wbctl_Dec_Upd(g_pub_tx.reply," pack_date = %d and packid = '%s' ", sHvFzf.o_pack_date,HvW_Packid);
	if (ret)
	{
		vtcp_log("[%s][%d]锁定查询报文接收登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_wbctl_Fet_Upd( &sLvWbctl, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "原往包记录不存在!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Lv_wbctl_Fet_Upd错![%d]", ret );
		WRITEMSG
			return -1;
	}
	if (memcmp(sLvWbctl.packstat, PACKSTAT_SEND, sizeof(sLvWbctl.packstat)-1) != 0 &&
		memcmp(sLvWbctl.packstat, PACKSTAT_SUCCESS, sizeof(sLvWbctl.packstat)-1) != 0 &&
		memcmp(sLvWbctl.packstat, PACKSTAT_INQUEUE, sizeof(sLvWbctl.packstat)-1) != 0 &&
		memcmp(sLvWbctl.packstat, PACKSTAT_QS, sizeof(sLvWbctl.packstat)-1) != 0)
	{
		vtcp_log("[%s][%d]应该是02,实际状态=[%s]\n",__FILE__,__LINE__,sLvWbctl.packstat);
		sprintf( acErrMsg, "状态错误" );
		WRITEMSG
			return -1;
	}
	vtcp_log("[%s][%d]============wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
	/** 根据能否退回等修改状态 **/
	if (wp_notpayin->respcode == '1')   /** 同意  **/
	{
		if (memcmp(wp_notpayin->cmtno,"322", 3) == 0)  /** 撤销 **/
		{
			sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
			wp_notpayin->stat = STAT_WZBACK;
			memcpy(sLvWbctl.packstat, PACKSTAT_CANCEL, sizeof(sLvWbctl.packstat)-1); 
		}
		/** added by dxy 20060206 收到退回应答时，修改原319的状态 **/
		else if (memcmp(wp_notpayin->cmtno,"320", 3) == 0)  /** 退回应答 **/
		{
			sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
			wp_notpayin->stat = STAT_WZBACK;
		}
		else if (memcmp(wp_notpayin->cmtno,"324", 3) == 0)  /** 冲正 **/
		{
			sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
			wp_notpayin->stat = STAT_WZBACK;
			memcpy(sLvWbctl.packstat, PACKSTAT_CZ, sizeof(sLvWbctl.packstat)-1); 
		}
		else if (memcmp(wp_notpayin->cmtno,"328", 3) == 0 )  /** 止付 **/
		{
			sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
			if (wp_notpayin->rettype == '1')
			{
				memcpy(sLvWbctl.packstat, PACKSTAT_STOPPAY, sizeof(sLvWbctl.packstat)-1); 
				wp_notpayin->stat = STAT_WZBACK;
			}
		}
	}
	else if (wp_notpayin->respcode == '2')  /** 不同意 **/
	{
		sHvFzf.hv_fzf_sts[0] = STAT_WZBACK;
		if (wp_notpayin->rettype == '1')
		{
			wp_notpayin->stat = STAT_WZBACK;
		}
	}
	vtcp_log("[%s][%d]===========退回类型0单笔,1整包rettype=[%c]\n",__FILE__,__LINE__,wp_notpayin->rettype);
	switch(iCmtno)
	{
	case 328:
		if (wp_notpayin->rettype == '0')    /** 单笔 **/
		{
			ret = iLv_Reset_Pkg_Stat_Sig(wp_notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]更新单笔错误=%d\n",__FILE__,__LINE__,ret);
				sprintf( acErrMsg, "更新单笔错误" );
				WRITEMSG
					return -1;
			}
		}
		else if (wp_notpayin->rettype == '1')    /** 整包 **/
		{
			ret = iLv_Reset_Pkg_Stat_Mul(wp_notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]更新整包错误=%d\n",__FILE__,__LINE__,ret);
				sprintf( acErrMsg, "更新整包错误" );
				WRITEMSG
					return -1;
			}
		}
		break;
	case 322:
	case 324:
		if (wp_notpayin->respcode == '1')  /** 同意 **/
		{
			ret = iLv_Reset_Pkg_Stat_Mul(wp_notpayin);
			if (ret!=0)
			{
				vtcp_log("[%s][%d]更新整包错误=%d\n",__FILE__,__LINE__,ret);
				sprintf( acErrMsg, "更新整包错误" );
				WRITEMSG
					return -1;
			}
		}
		break;
	}
	ret = Lv_wbctl_Upd_Upd(sLvWbctl,g_pub_tx.reply);
	if (ret)
	{
		vtcp_log("[%s][%d]sqlcode=%d\n",__FILE__,__LINE__,ret);
		sprintf( acErrMsg, "更新整包错误" );
		WRITEMSG
			return -1;
	}
	Lv_wbctl_Clo_Upd();

	ret = Hv_fzf_Upd_Upd( sHvFzf, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改非支付登记薄错[%d]", ret );
		WRITEMSG
			return -1;
	}
	Hv_fzf_Clo_Upd();

	return(0);
}
/****************************************
*  要求退回/撤销/冲正/止付整包      *
*  函数名: iLv_Reset_Pkg_Stat_Mul      *
****************************************/
int iLv_Reset_Pkg_Stat_Mul(NOTPAY_IN_AREA *wp_notpayin)
{
	char  cPkgno[4];
	char  cStat[2];
	char  cStat2[2];
	char  cPackid[9];
	char  cPackday[9];
	char  Pkgno[4];
	char  Orbrno[13];
	char  Wtday[9];
	char  Orderno[9];
	char  sql_string[512];
	char  sql_string2[512];
	char  cLvwPackid[9];
	int    iPkgno=0,ret=0;

	struct  hv_fzf_c  sHvFzf;
	struct  lv_wbctl_c  sLvWbctl;
	struct  lv_pkgreg_c     sLvPkgreg;

	memset(&sHvFzf,     '\0', sizeof(struct hv_fzf_c));
	memset(&sLvWbctl,   '\0', sizeof(struct lv_wbctl_c));

	memset(cStat,    '\0', sizeof(cStat));
	memset(cStat2,  '\0', sizeof(cStat2));
	memset(cPkgno,   '\0', sizeof(cPkgno));
	memset(cPackid, '\0', sizeof(cPackid));
	memset(cLvwPackid, '\0', sizeof(cLvwPackid));
	memset(cPackday,'\0', sizeof(cPackday));
	memset(Pkgno,   '\0', sizeof(Pkgno));
	memset(Orbrno,   '\0', sizeof(Orbrno));
	memset(Wtday,   '\0', sizeof(Wtday));
	memset(Orderno, '\0', sizeof(Orderno));
	memset(sql_string, '\0', sizeof(sql_string));
	memset(sql_string2, '\0', sizeof(sql_string2));

	vtcp_log("[%s][%d]==============cPackday==[%s]\n",__FILE__,__LINE__,cPackday);
	vtcp_log("[%s][%d]==============cLvwPackid==[%s]\n",__FILE__,__LINE__,cLvwPackid);  

	cStat[0] = wp_notpayin->stat;
	memcpy(cPkgno,    wp_notpayin->opkgno,   sizeof(cPkgno)-1);
	memcpy(cPackid,  wp_notpayin->opackid,  sizeof(cPackid)-1);
	memcpy(cPackday, wp_notpayin->opackday, sizeof(cPackday)-1);

	lv_orderno_add0(wp_notpayin->opackid,sizeof(cLvwPackid)-1,cLvwPackid);

	ret = Lv_wbctl_Sel(g_pub_tx.reply, &sLvWbctl," pack_date = %d and packid = '%s'", apatoi(cPackday,8) ,cLvwPackid);
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "查询往包控制表记录不存在!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Lv_wbctl_Sel错![%d]", ret );
		WRITEMSG
			return -1;
	}
	iPkgno = apatoi(cPkgno,3);

	vtcp_log("[%s][%d]==============stat[%s]\n",__FILE__,__LINE__,cStat);
	vtcp_log("[%s][%d]==============cPackid[%s]\n",__FILE__,__LINE__,cPackid);
	vtcp_log("[%s][%d]==============cPackday[%s]\n",__FILE__,__LINE__,cPackday);

	vtcp_log("[%s][%d]==============原包类型号=[%d]\n",__FILE__,__LINE__,iPkgno);

	vtcp_log("[%s][%d]==============opkgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);

	if (memcmp(wp_notpayin->opkgno,PKGNO_PTDJ,strlen(PKGNO_PTDJ))==0 || 
		memcmp(wp_notpayin->opkgno,PKGNO_DQDJ,strlen(PKGNO_DQDJ))==0 ||
		memcmp(wp_notpayin->opkgno,PKGNO_DJTH,strlen(PKGNO_DJTH))==0)
	{
		vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_PTDJ);
		sprintf(sql_string," pack_date=%d and packid='%s' and lw_ind='1' ",apatoi(cPackday,8),cPackid);/**modify by xyy YHBP**/
	}else if(memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_SSDJ))==0 ||
	memcmp(wp_notpayin->opkgno,PKGNO_PTJJ,strlen(PKGNO_PTJJ))==0 ||
		memcmp(wp_notpayin->opkgno,PKGNO_DQJJ,strlen(PKGNO_DQDJ))==0 ||
		memcmp(wp_notpayin->opkgno,PKGNO_SSJJ,strlen(PKGNO_SSJJ))==0)
	{
		vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_SSDJ);
		sprintf(sql_string," pack_date=%d and packid='%s' and pay_qs_no='%s' and lw_ind='1' ",apatoi(cPackday,8),cPackid,LV_QSBRNO);/**modify by xyy YHBP**/
	}else if(memcmp(wp_notpayin->opkgno,PKGNO_PTJJHZ,strlen(PKGNO_PTJJHZ))==0 ||
	memcmp(wp_notpayin->opkgno,PKGNO_DQJJHZ,strlen(PKGNO_DQJJHZ))==0)
	{
		vtcp_log("[%s][%d]==========现在业务是=[%s]\n",__FILE__,__LINE__,PKGNO_PTJJHZ);
		sprintf(sql_string," pack_date=%d  and packid='%s' and pay_qs_no='%s' and lw_ind='1' ",sLvWbctl.o_pack_date,sLvWbctl.o_packid,sLvWbctl.cash_qs_no);/**modify by xyy YHBP**/
	}
	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,sql_string);
	if (ret)
	{
		vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	vtcp_log("[%s,%d]  下面开始更新原交易状态 !\n",__FILE__,__LINE__);
	while (1)
	{
		memset(&sLvPkgreg,  '\0', sizeof(struct lv_pkgreg_c));    
		ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
		if ( ret == 100 )    
		{
			vtcp_log("%s,%d,数据处理完毕!\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "数据处理完毕!" );
			break;
		}  
		else  if ( ret )
		{
			sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
			WRITEMSG
				return -1;
		}
		vtcp_log("[%s][%d]=========sLvPkgreg.or_br_no=[%s]\n",__FILE__,__LINE__,sLvPkgreg.or_br_no);
		vtcp_log("[%s][%d]=========sLvPkgreg.wt_date=[%d]\n",__FILE__,__LINE__,sLvPkgreg.wt_date);
		vtcp_log("[%s][%d]=========sLvPkgreg.orderno=[%s]\n",__FILE__,__LINE__,sLvPkgreg.orderno);
		vtcp_log("[%s][%d]=========sLvPkgreg.pkgno=[%s]\n",__FILE__,__LINE__,sLvPkgreg.pkgno);
		vtcp_log("[%s][%d]=========sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,sLvPkgreg.lv_sts);
		/** 先判断状态是否正确 **/
		if (memcmp(wp_notpayin->cmtno,"320",3)==0)
		{
			if (sLvPkgreg.lv_sts[0] != STAT_WZSEND && sLvPkgreg.lv_sts[0] != STAT_WZSUCCESS )
			{
				/* begin NEWTCTD2 */
				/* rem by LiuHuafeng 2007-9-2 23:26 
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%c]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat,sLvPkgreg.lv_sts);
				************* replace by next line ****/
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%c]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat[0],sLvPkgreg.lv_sts);
				/* end NEWTCTD2 */
				sprintf( acErrMsg, "状态错误![%d]", ret );
				WRITEMSG
					return -1;
			}
		}
		if (memcmp(wp_notpayin->cmtno,"324",3)==0 || memcmp(wp_notpayin->cmtno,"328",3)==0)
		{
			if (sLvPkgreg.lv_sts[0] != STAT_WZSEND )
			{
				/* begin NEWTCTD2 */
				/* rem by LiuHuafeng 2007-9-2 23:26 
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%c]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat,sLvPkgreg.lv_sts);
				************* replace by next line ****/
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%s]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat,sLvPkgreg.lv_sts);
				/* end NEWTCTD2 */
				sprintf( acErrMsg, "状态错误![%d]", ret );
				WRITEMSG
				return -1;
			}
		}
		else if (memcmp(wp_notpayin->cmtno, "322", 3) == 0 )
		{
			if (sLvPkgreg.lv_sts[0] != STAT_WZSEND && sLvPkgreg.lv_sts[0] != STAT_WZINQUEUE)
			{
				/* begin NEWTCTD2 */
				/* rem by LiuHuafeng 2007-9-2 23:26 
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%c]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat,sLvPkgreg.lv_sts);
				************* replace by next line ****/
				vtcp_log("[%s][%d]=====状态应该是A,现在是 cStat=[%s]  sLvPkgreg.lv_sts=[%s]\n",__FILE__,__LINE__,cStat,sLvPkgreg.lv_sts);
				/* end NEWTCTD2 */
				sprintf( acErrMsg, "状态错误![%d]", ret );
				WRITEMSG
				return -1;
			}
		}
		vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
		vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
		vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
		vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

		memcpy(wp_notpayin->opkgno, sLvPkgreg.pkgno, sizeof(wp_notpayin->opkgno));
		memcpy(wp_notpayin->oorbrno,sLvPkgreg.or_br_no,sizeof(wp_notpayin->oorbrno));
		apitoa(sLvPkgreg.wt_date, sizeof(wp_notpayin->owtday),wp_notpayin->owtday);
		memcpy(wp_notpayin->oorderno,sLvPkgreg.orderno,sizeof(wp_notpayin->oorderno));

		vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
		vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
		vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
		vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

		/** 328只需改状态 **/
		if (memcmp(wp_notpayin->cmtno,"328",3)==0 )
		{
			cStat2[0] = STAT_WZZF;
		}
		else if (memcmp(wp_notpayin->cmtno,"324",3)==0 )
		{
			cStat2[0] = STAT_WZCZ;
			/** 实时贷记需要回滚帐务 
			if (memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_DQJJHZ))==0)
			{
			ret=lv_Reb_Pkg(wp_notpayin,OPCD_RESP);
			if(ret)
			{
			vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "处理原交易错误!" );
			WRITEMSG
			return(-1);
			}
			}**/
		}
		else if (memcmp(wp_notpayin->cmtno,"322",3)==0 )
		{
			cStat2[0] = STAT_WZCANCEL;

			/*    下面一段感觉没啥用
			vtcp_log("[%s][%d]-----------wp_notpayin->okgno=[%s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
			vtcp_log("[%s][%d]-----------wp_notpayin->oorbrno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorbrno);
			vtcp_log("[%s][%d]-----------wp_notpayin->owtday=[%s]\n",__FILE__,__LINE__,wp_notpayin->owtday);
			vtcp_log("[%s][%d]-----------wp_notpayin->oorderno=[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);

			ret = lv_Reb_Pkg(wp_notpayin, &sLvPkgreg, OPCD_RESP);
			if(ret)
			{
			vtcp_log("%s,%d,处理原交易错误\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "处理原交易错误!" );
			WRITEMSG
			return(-1);
			}     -------------*/
		}
		if (memcmp(wp_notpayin->cmtno,"320",3)!=0 )
		{
			if (memcmp(wp_notpayin->opkgno,PKGNO_PTDJ,strlen(PKGNO_PTDJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_DQDJ,strlen(PKGNO_DQDJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_SSDJ,strlen(PKGNO_SSDJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_PTJJ,strlen(PKGNO_PTJJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_PTJJHZ,strlen(PKGNO_PTJJHZ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_DQJJ,strlen(PKGNO_DQJJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_DQJJHZ,strlen(PKGNO_DQJJHZ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_SSJJ,strlen(PKGNO_SSJJ))==0 ||
				memcmp(wp_notpayin->opkgno,PKGNO_DJTH,strlen(PKGNO_DQJJHZ))==0)
			{
				sLvPkgreg.lv_sts[0]=cStat2[0];
			}
		}
		ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
			WRITEMSG
				return -1;
		}    
	}
	Lv_pkgreg_Clo_Upd();
	vtcp_log("[%s,%d]  更新原交易状态完毕 !\n",__FILE__,__LINE__);

	return 0;
}
/************************************************
*  要求退回或止付单笔              *
*  退回的情况有ptdj/dqdj,止付的情况有ptjj,dqjj  *
*  均不用还原帐务                *
*  函数名: iLv_Reset_Pkg_Stat_Sig          *
************************************************/
int iLv_Reset_Pkg_Stat_Sig(NOTPAY_IN_AREA *wp_notpayin)
{
	char  cPkgno[4];
	char  cOrderno[9];
	char  cWtday[9];
	char  cOrbrno[13];
	char  cStat[2];
	char  cStat2[2];
	int  ret=0;

	struct lv_pkgreg_c              sLvPkgreg;
	memset(&sLvPkgreg, '\0', sizeof(struct lv_pkgreg_c));

	memset(cPkgno,  '\0', sizeof(cPkgno));
	memset(cOrderno,'\0', sizeof(cOrderno));
	memset(cWtday,  '\0', sizeof(cWtday));
	memset(cOrbrno, '\0', sizeof(cOrbrno));
	memset(cStat,  '\0', sizeof(cStat));
	memset(cStat2,  '\0', sizeof(cStat2));

	memcpy(cOrderno, wp_notpayin->oorderno, sizeof(cOrderno)-1);
	memcpy(cWtday,    wp_notpayin->owtday,   sizeof(cWtday)-1);
	memcpy(cOrbrno,  wp_notpayin->oorbrno,  sizeof(cOrbrno)-1);
	memcpy(cPkgno,   wp_notpayin->opkgno,   sizeof(cPkgno)-1);

	vtcp_log("[%s][%d]==============原包类型号=[%s]\n",__FILE__,__LINE__,cPkgno);
	vtcp_log("[%s][%d]======orbrno=[%s],orderno=[%s]wtday=[%s]\n",__FILE__,__LINE__,cOrbrno,cOrderno,cWtday);

	ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date=%d and orderno = '%s' ", cOrbrno,apatoi(cWtday,8),cOrderno);
	if (ret)
	{
		vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "原查询记录不存在!" );
		WRITEMSG
			return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
		WRITEMSG
			return -1;
	}
	cStat[0] = sLvPkgreg.lv_sts[0];
	vtcp_log("[%s][%d]-----------------cStat=[%s]\n",__FILE__,__LINE__,cStat);

	if (memcmp(wp_notpayin->cmtno,"320",3)==0)
	{
		if (cStat[0] != STAT_WZSEND && cStat[0] != STAT_WZSUCCESS)
		{
			vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
			sprintf( acErrMsg, "状态错误" );
			WRITEMSG
				return -1;
		}
	}
	if (memcmp(wp_notpayin->cmtno,"324",3)==0 || memcmp(wp_notpayin->cmtno,"328",3)==0)
	{
		if (cStat[0] != STAT_WZSEND )
		{
			vtcp_log("[%s][%d]=====状态应该是A,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
			sprintf( acErrMsg, "状态错误" );
			WRITEMSG
				return -1;
		}
	}
	else if (memcmp( wp_notpayin->cmtno, "322", 3) == 0 )
	{
		if (cStat[0] != STAT_WZSEND && cStat[0] != STAT_WZINQUEUE)
		{
			vtcp_log("[%s][%d]=====状态应该是8,现在是=[%c]\n",__FILE__,__LINE__,cStat[0]);
			sprintf( acErrMsg, "状态错误" );
			WRITEMSG
				return -1;
		}
	}
	if (wp_notpayin->respcode == '1')  /** 同意 **/
	{
		if (memcmp( wp_notpayin->cmtno, "328", 3) == 0 )
		{
			cStat2[0] = STAT_WZZF;
		}
	}
	else
	{
		if (memcmp( wp_notpayin->cmtno, "320", 3) == 0 )
		{
			cStat2[0] = STAT_WZSUCCESS;
		}
		else if (memcmp( wp_notpayin->cmtno, "328", 3) == 0 )
		{
			cStat2[0] = STAT_WZSEND;
		}
	}
	ret = Lv_pkgreg_Upd_Upd( sLvPkgreg, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_pkgreg_Clo_Upd();

	return(0);

}
/************************************************************************************
*  冲正325                                      *
*  325和324的区别是:假设A已经发了一笔业务给B,这时A发起冲正,CCPC或NPC收到冲正申请时  *
*  告诉A可以或不可以冲正的是用324,告诉B要冲正是用325                *
*  所以325需要读来包控制表                              *
*  函数名: iLv_Reset_325_Stat_Sig                            *
************************************************************************************/
int iLv_Reset_325_Stat_Sig(NOTPAY_IN_AREA *wp_notpayin)
{
	int  ret=0;
	struct  lv_lbctl_c   sLvLbctl;
	struct  lv_pkgreg_c  sLvPkgreg;

	memset(&sLvLbctl,  '\0', sizeof(struct  lv_lbctl_c));
	memset(&sLvPkgreg, '\0', sizeof(struct  lv_pkgreg_c));
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);

	ret = Lv_lbctl_Dec_Upd(g_pub_tx.reply," pack_date =%d and packid = '%s' and pay_qs_no='%s'", apatoi(wp_notpayin->opackday,8), wp_notpayin->opackid,wp_notpayin->payqsactno);
	if (ret)
	{
		vtcp_log("[%s][%d]锁定来包控制表错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
		WRITEMSG
			return -1;
	}
	ret = Lv_lbctl_Fet_Upd( &sLvLbctl, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "原查询记录不存在!" );
		WRITEMSG
		/* 如果真的没有找到也就算正确了，不需要再处理原来包了 */
			return 0;
		/* rem by LiuHuafeng 2007-4-4 5:03
		return -1;
		********************************/
	}
	else if ( ret )
	{
		sprintf( acErrMsg, "执行Lv_lbctl_Fet_Upd错误![%d]", ret );
		WRITEMSG
		return -1;
	}
	if (sLvLbctl.sts[0] != PSTAT_RECEIVE)
	{
		vtcp_log("[%s][%d]应该是5,实际状态=[%s]\n",__FILE__,__LINE__,sLvLbctl.sts);
		sprintf( acErrMsg, "状态错误!");
		WRITEMSG
		return(0);
	}
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);

	if ((memcmp(sLvLbctl.packstat, PACKSTAT_SUCCESS,sizeof(sLvLbctl.packstat)-1) != 0 &&
		  memcmp(sLvLbctl.packstat, PACKSTAT_QS,sizeof(sLvLbctl.packstat)-1) != 0)
		/* begin NEWTCTD2 */
		&& strlen(sLvLbctl.packstat)!=0)
		/* end NEWTCTD2 */
	{
		vtcp_log("[%s][%d]应该是03,实际状态=[%s]\n",__FILE__,__LINE__,sLvLbctl.packstat);
		sprintf( acErrMsg, "状态错误!");
		WRITEMSG
		return -1;
	}
	sLvLbctl.sts[0] = PSTAT_LRESP;
	memcpy(sLvLbctl.packstat, PACKSTAT_CZ,sizeof(sLvLbctl.packstat)-1);

	ret = Lv_lbctl_Upd_Upd(sLvLbctl,g_pub_tx.reply);
	if ( ret )
	{
		sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
		WRITEMSG
			return -1;
	}
	Lv_lbctl_Clo_Upd();
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);

	if (memcmp(wp_notpayin->opkgno, PKGNO_SSJJ, 3) == 0)  /** 实时借记 **/
	{
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"packid = '%.8s' and pack_date =%d and pkgno='%.3s' and pay_qs_no='%.12s' and lw_ind = '2' ",\
		          wp_notpayin->opackid,apatoi(wp_notpayin->opackday,8),wp_notpayin->opkgno,wp_notpayin->oorbrno);/**modify by xyy YHBP**/
		if (ret)
		{
			vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
			WRITEMSG
				return -1;
		}
		ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "原查询记录不存在!" );
			WRITEMSG
				return -1;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
			WRITEMSG
				return -1;
		}
		if (sLvPkgreg.lv_sts[0] != STAT_YES_NOTSEND && sLvPkgreg.lv_sts[0] != STAT_YES_SEND)/**modify by NEWBP1**/
		{
			vtcp_log("[%s][%d]原来帐状态错误!应该是Q or R,实际是=[%c]\n",__FILE__,__LINE__,sLvPkgreg.lv_sts[0]);
			sprintf( acErrMsg, "原来帐状态错误");
			WRITEMSG
				return (0);
		}
		vtcp_log("[%s][%d]==========\n",__FILE__,__LINE__);
		/******NEWBP1 暂时注掉这里，因为这个函数没啥意义
		ret=lv_Reb_Pkg(wp_notpayin,OPCD_RECEIVE);
		if (ret!=0)
		{
			vtcp_log("%s,%d,处理原交易错误ret=[%d]\n",__FILE__,__LINE__,ret);
			sprintf( acErrMsg, "处理原交易错误");
			WRITEMSG
				return(0);
		}
		***add by NEWBP1**/
		g_reply_int = sql_execute("update lv_pkgreg set lv_sts='5' where  o_or_br_no='%s'  and o_wt_date=%ld and o_orderno='%s'\
		                      and pkgno='010' and lw_ind='1' ", sLvPkgreg.or_br_no,sLvPkgreg.wt_date,sLvPkgreg.orderno);
		if (g_reply_int)
		{
			  vtcp_log("[%s][%d],更新lv_pkgreg表失败[%d]",__FILE__,__LINE__, g_reply_int);
			  sprintf(acErrMsg,"更新lv_pkgreg表失败[%s][%d]",__FILE__,__LINE__);
			  return(g_reply_int);
		}
		vtcp_log("[%s][%d]~~~~~~~~~~~~~~~~~~~~~~~~~~~~xyy!!!",__FILE__,__LINE__);
		/***********end by NEWBP1**********/
		sLvPkgreg.lv_sts[0] = STAT_LZCZ;

		ret = Lv_pkgreg_Upd_Upd(sLvPkgreg,g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
			WRITEMSG
				return -1;
		}  
		Lv_pkgreg_Clo_Upd();

	}
	else if (memcmp(wp_notpayin->opkgno, PKGNO_SSDJ, 3) == 0)    /** 实时贷记 **/
	{
		/* rem by NEWTCTD2
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%s' and wt_date =%d and orderno = '%s' ", wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,8),wp_notpayin->oorderno);
		** replace by next line ***/
		ret = Lv_pkgreg_Dec_Upd(g_pub_tx.reply,"  or_br_no = '%.12s' and wt_date =%d and lw_ind='2' and pkgno='003' and pack_date=%d and packid='%.8s' ",
			wp_notpayin->oorbrno,apatoi(wp_notpayin->owtday,8),apatoi(wp_notpayin->opackday,8),wp_notpayin->opackid);
		if (ret)
		{
			vtcp_log("[%s][%d]锁定pkg登记簿错误!sqlcode=%d\n",__FILE__,__LINE__,ret);
			WRITEMSG
				return -1;
		}
		ret = Lv_pkgreg_Fet_Upd( &sLvPkgreg, g_pub_tx.reply );
		if ( ret == 100 )
		{
			sprintf( acErrMsg, "原查询记录不存在!" );
			WRITEMSG
			return -1;
		}else if( ret )
		{
			sprintf( acErrMsg, "执行Lv_pkgreg_Fet_Upd错![%d]", ret );
			WRITEMSG
				return -1;
		}
		if (sLvPkgreg.lv_sts[0] != STAT_LZRECV2)
		{
			vtcp_log("[%s][%d]状态错误\n",__FILE__,__LINE__);
			sprintf( acErrMsg, "状态错误!");
			WRITEMSG
				return (0);
		}
		sLvPkgreg.lv_sts[0] = STAT_LZCZ;

		ret = Lv_pkgreg_Upd_Upd(sLvPkgreg,g_pub_tx.reply );
		if ( ret )
		{
			sprintf( acErrMsg, "修改支付登记薄错[%d]", ret );
			WRITEMSG
				return -1;
		}
		Lv_pkgreg_Clo_Upd();
		/* begin NEWTCTD2 */
		/* 设置冲帐会计分录 */
		if(sLvPkgreg.lv_sts[0]==STAT_LZRECV2)
		{
			set_zd_data("0660","2");/* 客户来帐方向 */
			set_zd_data("0720","1");/* 110,414 来帐方向 */
			set_zd_data("0330",LV_WAIT_QS);/* 414等待清算科目 */
			set_zd_data("0320",LVLZGD);/* 设置过渡科目 */
			set_zd_data("0300",sLvPkgreg.cash_ac_no);
			
		}
		/* end   NEWTCTD2 */
	}
	return(0);

}
/****NEWYX3:把业务权限503插入明细登记簿,为人行验收提供查询****/
int lv_deal_cmt503_detail(TLVPACKAGE *lv)
{
	int iRc=0;
	char pcTmpBuf[64];
	char pcUpdNode[5];
	char pcUpdDate[9];
	int nBankCnt,nSendCnt,nRecvCnt;
	int nBankId;
	int offset;
	int iInsFlag;
	struct lv_c503r_c wd_lv_c503r;
	memset(pcTmpBuf ,0,sizeof(pcTmpBuf));
	memset(pcUpdNode ,0,sizeof(pcUpdNode));
	memset(pcUpdDate ,0,sizeof(pcUpdDate));
	nBankCnt=nSendCnt=nRecvCnt=0;
	lv_get_tag(lv,"B69",pcTmpBuf);
	nBankCnt=atoi(pcTmpBuf);
	lv_get_tag(lv,"C40",pcUpdNode);
	lv_get_tag(lv,"30A",pcUpdDate);
	offset=0;
	for (nBankId=0;nBankId<nBankCnt;nBankId++)
	{
		memset(pcTmpBuf ,0,sizeof(pcTmpBuf));
		memset(&wd_lv_c503r,'\0',sizeof(wd_lv_c503r));
		iInsFlag=1;
		lv_get_tag_multi(lv,"B89",wd_lv_c503r.bakcode,offset,12);
		offset+=12;
		iRc=Lv_c503r_Dec_Upd(g_pub_tx.reply, "bakcode = '%s' ", wd_lv_c503r.bakcode);
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,处理lv_c503r表错[%d]\n",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"处理lv_c503r表出错");
			WRITEMSG
				return(-1);
		}
		iRc = Lv_c503r_Fet_Upd(&wd_lv_c503r, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,未找到原503记录[%d]-[%s]，插入\n",__FILE__,__LINE__,iRc,wd_lv_c503r.bakcode);
			iInsFlag=1;
		}else
		{
			vtcp_log("%s,%d,找到原503记录，更新\n",__FILE__,__LINE__);
			iInsFlag=0;
		}
		lv_get_tag_multi(lv,"B89",pcTmpBuf,offset,3);
		nSendCnt=atoi(pcTmpBuf);
		offset+=3;
		lv_get_tag_multi(lv,"B89",pcTmpBuf,offset,3);
		nRecvCnt=atoi(pcTmpBuf);
		offset+=3;
		lv_get_tag_multi(lv,"B89",wd_lv_c503r.sendgrant,offset,nSendCnt*9);
		offset+=nSendCnt*9;
		lv_get_tag_multi(lv,"B89",wd_lv_c503r.recvgrant,offset,nRecvCnt*9);
		offset+=nRecvCnt*9;
		memcpy(wd_lv_c503r.upd_date,pcUpdDate,8);	
		memcpy(wd_lv_c503r.upd_node,pcUpdNode,8);	
		if (iInsFlag)
		{
			iRc=Lv_c503r_Ins(wd_lv_c503r, g_pub_tx.reply);
		}else
		{
			iRc=Lv_c503r_Upd_Upd(wd_lv_c503r, g_pub_tx.reply);
		}
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,操作lv_c503r表错[%d]\n",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"操作lv_c503r表出错");
			WRITEMSG
				Lv_c503r_Clo_Upd();
			return(-1);
		}
	}
	Lv_c503r_Clo_Upd();
	return(0);
}
/* begin add by LiuHuafeng 2007-5-8 22:52 */
int lv_deal_cmt504_detail(TLVPACKAGE *lv)
{
	int iRc=0;
	char pcTmpBuf[64];
	char pcUpdNode[5];
	char pcUpdDate[9];
	int nBankCnt,nSendCnt,nRecvCnt;
	int nBankId;
	int offset;
	int iInsFlag;
	struct STR_C504_DTL{
		char chrgtype[1]; /* 变更类型 */
		char begtype[1];  /* 生效类型 0立即,1定时*/
		char begday[8];   /* 生效日期 */
		char chgday[8];   /* 变更日期 */
		char chgtime[14]; /* 变更时间 */
		char bakcode[12]; /* 节点号或行号 */
	}str_c504dtl;
	struct lv_c503r_c wd_lv_c503r;
	memset(pcTmpBuf ,0,sizeof(pcTmpBuf));
	memset(pcUpdNode ,0,sizeof(pcUpdNode));
	memset(pcUpdDate ,0,sizeof(pcUpdDate));
	memset(&str_c504dtl, 0 , sizeof(str_c504dtl));
	nBankCnt=nSendCnt=nRecvCnt=0;
	lv_get_tag(lv,"B6A",pcTmpBuf);
	nBankCnt=atoi(pcTmpBuf);
	lv_get_tag(lv,"30A",pcUpdDate);
	offset=0;
	for (nBankId=0;nBankId<nBankCnt;nBankId++)
	{
		memset(pcTmpBuf ,0,sizeof(pcTmpBuf));
		memset(&wd_lv_c503r,'\0',sizeof(wd_lv_c503r));
		iInsFlag=1;
		lv_get_tag_multi(lv,"B8A",(char*)&str_c504dtl,offset,44);
		offset+=44;
		memcpy(wd_lv_c503r.bakcode,str_c504dtl.bakcode,sizeof(wd_lv_c503r.bakcode)-1);
		iRc=Lv_c503r_Dec_Upd(g_pub_tx.reply, "bakcode = '%s' ", wd_lv_c503r.bakcode);
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,处理lv_c503r表错[%d]\n",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"处理lv_c503r表出错");
			WRITEMSG
			return(-1);
		}
		iRc = Lv_c503r_Fet_Upd(&wd_lv_c503r, g_pub_tx.reply);
		if (iRc)
		{
			vtcp_log("%s,%d,未找到原503记录[%d]-[%s]，插入\n",__FILE__,__LINE__,iRc,wd_lv_c503r.bakcode);
			iInsFlag=1;
		}else
		{
			vtcp_log("%s,%d,找到原503记录，更新\n",__FILE__,__LINE__);
			iInsFlag=0;
		}
		/* 由于503表中缺少生效日期所以直接设为立刻更新 */
		/* 由于503表中缺少状态字段所以借用sendcnt <0表示失效,>=0表示生效 */
		if(str_c504dtl.chrgtype[0]=='0')/* 退出小额系统 */
			wd_lv_c503r.sendcnt=-1;
		else
			wd_lv_c503r.sendcnt=0;/* 生效 */
		if (iInsFlag)
		{
			iRc=Lv_c503r_Ins(wd_lv_c503r, g_pub_tx.reply);
		}else
		{
			iRc=Lv_c503r_Upd_Upd(wd_lv_c503r, g_pub_tx.reply);
		}
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,操作lv_c503r表错[%d]\n",__FILE__,__LINE__,iRc);
			sprintf(acErrMsg,"操作lv_c503r表出错");
			WRITEMSG
				Lv_c503r_Clo_Upd();
			return(-1);
		}
	}
	Lv_c503r_Clo_Upd();
	return(0);
}
/* end add by LiuHuafeng 2007-5-8 22:52 */
