/*************************************************************************/
/* 文件: lv_packcomm.pc                                                  */
/* 功能: 有关报文处理的公共函数                                          */
/* 作者: 石升辉                                                          */
/* 日期: 2005.8.18                                                       */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include "lv_define.h"
#include "hv_define.h"
#include "lv_pub.h"
#include "public.h"
#include "lv_addit_c.h"
extern char * pcLvDesPswdCode(char *code);
extern char * pcUndesPswdCode(char *code);
extern char * pcLvGetDay();
/*****请注意:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!             *****
******初始化指针时不要把它置空,如memset(&wp_payin...... 一定要避免
*****                                                            *****
*****                                                    石升辉  *****
*****                                                  2005.12.3 *****
*********************************************************************/
/****Add by SSH,2005.12.1临时定义,当get_tag函数不再后补0时可以不用****/
extern char * GETTAG_SIZE(TLVPACKAGE *pack,char *name,char *value,int size);
static char* GETTAG(TLVPACKAGE *pack,char *name,char *value)
{
	lv_get_tag(pack,name,value); 
	return(value);
}
char* GETHEADTAG(TLVPACKAGE *pack,char *name,char *value)
{
	lv_pkg_getHeaderTag(pack,name,value); 
	vtcp_log("[%s][%d] name===[%s]",__FILE__,__LINE__,name);	
	vtcp_log("[%s][%d] value===[%s]",__FILE__,__LINE__,value);		
	return(value);
}

static char* GETTAG_AMT(TLVPACKAGE *pack,char *name,char *value,int length)
{
	char buf[32]; 
	double dblTmp=0;
	memset(buf,'\0',sizeof(buf));
	lv_get_tag(pack,name,buf); 
	dblTmp=atof(buf);
	sprintf(buf,"%*.0f",length,dblTmp);
	memcpy(value,buf,length);
	return(value);
}

/****委托收款附加数据域定义****/
typedef struct
{
	char pdate[8];
	char pnum[8];
	char ptype[2];
}WT_ADD_AREA;

/****托收承付附加数据域定义****/
typedef struct
{
	char pdate[8];
	char pnum[8];
	char ptxamt[15];
	char jtxamt[15];
}TSCF_ADD_AREA;

/*************************************************************************/
/* 函数名:lv_setwbctl_from_pack                                          */
/* 作用:  根据报文内容设置往包控制表                                     */
/*************************************************************************/
int lv_setwbctl_from_pack(TLVPACKAGE *wp_lvpack,struct lv_wbctl_c *wp_lv_wbctl)
{
	char caValue[61];
	memset(wp_lv_wbctl,'\0',sizeof(struct lv_wbctl_c));	
	iLvGETHEADTAG_DATE(wp_lvpack,"30E",  &wp_lv_wbctl->pack_date);/**包委托日期**/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BD",caValue);/**包序号**/
	lv_orderno_add0(caValue, sizeof(wp_lv_wbctl->packid)-1, wp_lv_wbctl->packid);
	/** rem by lhf 060303
	memcpy(wp_lv_wbctl->packid,caValue,sizeof(wp_lv_wbctl->packid)-1);
	***********************************************************/
	GETHEADTAG(wp_lvpack,"02C",wp_lv_wbctl->pkgno);/**包类型号**/
	GETHEADTAG(wp_lvpack,"011",wp_lv_wbctl->pay_qs_no);/*发起清算行*/
	GETHEADTAG(wp_lvpack,"012",wp_lv_wbctl->cash_qs_no);/*接收清算行*/
	/****进入系统日期****/
	wp_lv_wbctl->in_date = apatoi(pcLvGetDay(), 8);
	/***业务总笔数****/ 
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B63",caValue);
	wp_lv_wbctl->dtlcnt=atoi(caValue);
	/***业务总金额****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32B",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_wbctl->totamt);
	wp_lv_wbctl->totamt= wp_lv_wbctl->totamt/100.0;
	/****扎差节点类型****/
	GETHEADTAG(wp_lvpack,"BS1",wp_lv_wbctl->zcnode);
	/****扎差日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS2",&wp_lv_wbctl->zc_date);
	/****扎差场次****/
	GETHEADTAG(wp_lvpack,"BS3",wp_lv_wbctl->zcid);
	/****回执日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS6",&wp_lv_wbctl->hz_date);
	/****原来包日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"30I",&wp_lv_wbctl->o_pack_date);
	/***原来包id****/
	memset(caValue,0 , sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BE",caValue);
	lv_orderno_add0(caValue, sizeof(wp_lv_wbctl->o_packid)-1, wp_lv_wbctl->o_packid);
	vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
	/** rem by lhf 060303
	GETHEADTAG(wp_lvpack,"0BE",wp_lv_wbctl->o_packid);
	memcpy(wp_lv_wbctl->packid,caValue,sizeof(wp_lv_wbctl->packid)-1);
	****************************************************/
	/***明细业务成功笔数****/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B41",caValue);
	wp_lv_wbctl->dtlsuccnt=atoi(caValue);
	/***明细业务成功总金额****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32C",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_wbctl->dtlsucamt);
	wp_lv_wbctl->dtlsucamt = wp_lv_wbctl->dtlsucamt/100.0;
	/****原包类型号*****/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	GETHEADTAG(wp_lvpack,"02D",wp_lv_wbctl->o_pkgno);
	/****发起节点号,通用信息用****/
	GETHEADTAG(wp_lvpack,"C36",wp_lv_wbctl->ornode);
	/****接收节点号,通用信息用****/
	GETHEADTAG(wp_lvpack,"C37",wp_lv_wbctl->acnode);
	/****辖内分发标志****/
	GETHEADTAG(wp_lvpack,"BS7",wp_lv_wbctl->ffflag);
	/****业务类型号***/
	GETHEADTAG(wp_lvpack,"0BG",wp_lv_wbctl->txnum);
	/****补发标志****/
	GETHEADTAG(wp_lvpack,"BS4",wp_lv_wbctl->bfflag);
	/****清算日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS5",&wp_lv_wbctl->qs_date);
	/****包处理状态****/
	GETHEADTAG(wp_lvpack,"CIB",wp_lv_wbctl->packstat);
	/****包状态,自定义*****/
	if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SEND,2)==0 )
	{
		wp_lv_wbctl->stat[0]=PSTAT_SEND;
	}else
	{
		wp_lv_wbctl->stat[0]=PSTAT_LRESP;
	}
	/****打印标志,保留****/
	memset(wp_lv_wbctl->brprtflag,'0',sizeof(wp_lv_wbctl->brprtflag)-1);
	memset(wp_lv_wbctl->qsprtflag,'0',sizeof(wp_lv_wbctl->qsprtflag)-1);
	get_fd_data("0030", wp_lv_wbctl->br_no);
	/*memcpy(wp_lv_wbctl->br_no,TITA.kinbr,sizeof(wp_lv_wbctl->br_no)-1);*/
	return(0);
}
/*************************************************************************/
/* 函数名:lv_setlbctl_from_pack                                          */
/* 作用:  根据报文内容设置来包控制表                                     */
/*************************************************************************/
int lv_setlbctl_from_pack(TLVPACKAGE *wp_lvpack,struct lv_lbctl_c *wp_lv_lbctl)
{
	char caValue[61];
	memset(caValue,0 , sizeof(caValue));
	memset(wp_lv_lbctl,'\0',sizeof(struct lv_lbctl_c));	

	iLvGETHEADTAG_DATE(wp_lvpack,"30E",&wp_lv_lbctl->pack_date);/**包委托日期**/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BD",caValue);/**包序号**/
	lv_orderno_add0(caValue, sizeof(wp_lv_lbctl->packid)-1, wp_lv_lbctl->packid);
	/** rem by lhf 060303
	memcpy(wp_lv_lbctl->packid,caValue,sizeof(wp_lv_lbctl->packid)-1);
	***********************************************/
	GETHEADTAG(wp_lvpack,"02C",wp_lv_lbctl->pkgno);/**包类型号**/
	GETHEADTAG(wp_lvpack,"011",wp_lv_lbctl->pay_qs_no);/*发起清算行*/
	GETHEADTAG(wp_lvpack,"012",wp_lv_lbctl->cash_qs_no);/*接收清算行*/
	/****进入系统日期****/
	lv_ldate(&(wp_lv_lbctl->in_date),pcLvGetDay(),8);
	/***业务总笔数****/ 
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B63",caValue);
	wp_lv_lbctl->dtlcnt=atoi(caValue);
	/***业务总金额****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32B",caValue);
	vtcp_log("[%s][%d]=======czValue=[%s]\n",__FILE__,__LINE__,caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_lbctl->totamt);
	wp_lv_lbctl->totamt=wp_lv_lbctl->totamt/100.0;
	vtcp_log("[%s][%d]=======wp_lv_lbctl->totamt=[%f]\n",__FILE__,__LINE__,wp_lv_lbctl->totamt);
	/****扎差节点类型****/
	GETHEADTAG(wp_lvpack,"BS1",wp_lv_lbctl->zcnode);
	/****扎差日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS2",&wp_lv_lbctl->zc_date);
	/****扎差场次****/
	GETHEADTAG(wp_lvpack,"BS3",wp_lv_lbctl->zcid);
	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	/****回执日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS6",&wp_lv_lbctl->hz_date);
	/****原来包日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"30I",&wp_lv_lbctl->o_pack_date);
	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	/***原来包id****/
	/** by lhf 20060303
	GETHEADTAG(wp_lvpack,"0BE",wp_lv_lbctl->o_packid);
	* begin add by lhf ***/
	memset(caValue,0 , sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BE",caValue);
	lv_orderno_add0(caValue, sizeof(wp_lv_lbctl->o_packid)-1, wp_lv_lbctl->o_packid);
	vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_lv_lbctl->o_packid);
	/***明细业务成功笔数****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B41",caValue);
	wp_lv_lbctl->dtlsuccnt=atoi(caValue);
	/***明细业务成功总金额****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32C",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_lbctl->dtlsucamt);
	wp_lv_lbctl->dtlsucamt=wp_lv_lbctl->dtlsucamt/100.0;
	/****原包类型号*****/
	vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
	GETHEADTAG(wp_lvpack,"02D",wp_lv_lbctl->o_pkgno);
	/****发起节点号,通用信息用****/
	GETHEADTAG(wp_lvpack,"C36",wp_lv_lbctl->ornode);
	/****接收节点号,通用信息用****/
	GETHEADTAG(wp_lvpack,"C37",wp_lv_lbctl->acnode);
	/****辖内分发标志****/
	GETHEADTAG(wp_lvpack,"BS7",wp_lv_lbctl->ffflag);
	/****业务类型号***/
	GETHEADTAG(wp_lvpack,"0BG",wp_lv_lbctl->txnum);
	/****补发标志****/
	GETHEADTAG(wp_lvpack,"BS4",wp_lv_lbctl->bfflag);
	/****清算日期****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS5",&wp_lv_lbctl->qs_date);
	/****包处理状态****/
	GETHEADTAG(wp_lvpack,"CIB",wp_lv_lbctl->packstat);
	/****包状态,自定义*****/
	wp_lv_lbctl->sts[0]=PSTAT_RECEIVE;
	/****打印标志,保留****/
	memset(wp_lv_lbctl->brprtflag,'0',sizeof(wp_lv_lbctl->brprtflag)-1);
	memset(wp_lv_lbctl->qsprtflag,'0',sizeof(wp_lv_lbctl->qsprtflag)-1);
	get_fd_data("0030", wp_lv_lbctl->br_no);

	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	return(0);
}
/*************************************************************************/
/*   关于从报文到payin的转换,试图直接从批量报文中fetch出后填写payin,但 */
/*由于定期报文,实现起来比较麻烦，而且很容易引起内存泄漏,所以暂定:首先从批*/
/*量报文中提取子报文文,然后通过一个函数得到子报文的记录个数,最后循环从子 */
/*报文中提取数据,填写payin                                              */
/*                               ----Add by SSH,2005.8.18                */
/*************************************************************************/

/*************************************************************************/
/* 函数名:lv_count_recnt                                                 */
/* 作用:  计算子报文中记录的个数                                         */
/* 返回值:<=0表示失败                                                    */
/*************************************************************************/
int lv_count_recnt(TLVPACKAGE *wp_lvpack)
{
	char cNum[9];
	memset(cNum,'\0',sizeof(cNum));
	if (memcmp(wp_lvpack->CMTCode,PKGNO_DQDJ,sizeof(wp_lvpack->CMTCode))==0)
	{
		GETTAG_SIZE(wp_lvpack,"B61",cNum,sizeof(cNum)-1);
		return(atoi(cNum));
	}else if(memcmp(wp_lvpack->CMTCode,PKGNO_DQJJ,
	sizeof(wp_lvpack->CMTCode))==0)
	{
		GETTAG_SIZE(wp_lvpack,"B62",cNum,sizeof(cNum)-1);
		return(atoi(cNum));
    }
	else if( memcmp(wp_lvpack->CMTCode,"012",sizeof(wp_lvpack->CMTCode))==0 ) 
		/*** add by xyy 2007-04-13   NEWSYS***/
    {
        lv_pkg_getHeaderTag(wp_lvpack,"B42",cNum);
        return(atoi(cNum));
	}else
	{
		/***非定期业务只有1笔****/
		return(1);
	}
}
/*************************************************************************/
/* 函数名:lv_fetchpack_to_payin                                         */
/* 作用:  从子报文中取内容填充通用数据交换区                             */
/*************************************************************************/
int lv_fetchpack_to_payin(TLVPACKAGE *wp_parent,TLVPACKAGE *wp_lvpack, int id, PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char caValue[61];
	int nTxAddLen=0;
	/****HSYX BEGIN :放大附加域缓冲区****/
	char cTxAddText[1024*800];/***附加字段缓冲区****/
	char    cAcbrno[13];/** 不知为何，不用中间变量，F_acbrno就是没有值 **/
	/****HSYX END :放大附加域缓冲区****/
	WT_ADD_AREA *wp_wtadd=NULL;
	TSCF_ADD_AREA *wp_tscfadd=NULL;
	TCTD_ADD_AREA *wp_tcadd=NULL;
	TCTD_ADD_AREA *wp_tdadd=NULL;
	DQDJ_DTL_AREA wd_djdtl;
	DQJJ_DTL_AREA wd_jjdtl;
	memset(caValue, 0 , sizeof(caValue));

	memset(&wd_djdtl,'\0',sizeof(wd_djdtl));
	nTxAddLen=0;
	memset(cTxAddText,'\0',sizeof(cTxAddText));
	/****HSYX BEGIN :放大附加域缓冲区****/
	memset(cAcbrno,'\0',sizeof(cAcbrno));
	/****HSYX END :放大附加域缓冲区****/

	vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);
	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay()); 
	memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));	 	
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]  wp_lvpack->CMTCode=[%s]",__FILE__,__LINE__,wp_payin->A_tbsdy,wp_lvpack->CMTCode);  
	vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
	/****回执业务，从原往帐表中取内容****/
	if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0)
	{
		memcpy(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd));
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,3)==0)
			memcpy(wp_payin->F_pkgno,PKGNO_SSDJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,3)==0)
			memcpy(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,3)==0)
			memcpy(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,3)==0)
			memcpy(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno));		

		GETTAG_SIZE(wp_lvpack,"051",wp_payin->F_wtday,sizeof(wp_payin->F_wtday));
		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);   
		vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
		/** modified by dongxy 20070313 票交要素集中没有cc1,对应字段是58A  begin **/
		GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		if (wp_payin->F_orbrno[0]=='\0')
		{
			vtcp_log("[%s][%d]===========是票交\n",__FILE__,__LINE__);
			GETTAG_SIZE(wp_lvpack,"58A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		}
		vtcp_log("[%s][%d]===========wp_payin->F_orbrno=[%s]\n",__FILE__,__LINE__,wp_payin->F_orbrno);
		/** modified by dongxy 20070313 票交要素集中没有cc1,对应字段是58A  end **/
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"005",caValue,sizeof(caValue)-1);
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		
		GETTAG_SIZE(wp_lvpack,"0BH",wp_payin->F_txnum,sizeof(wp_payin->F_txnum));/**YHBP**/
		vtcp_log("[%s][%d]====xyy=======wp_payin->F_txnum=[%s]\n",__FILE__,__LINE__,wp_payin->F_txnum);
		wp_payin->F_lw_ind[0] = '1';		

		vtcp_log("%s,%d,F_wtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_wtday); 
		vtcp_log("%s,%d,F_orderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_orderno); 		
		vtcp_log("%s,%d,F_orbrno=[%.12s]",__FILE__,__LINE__,wp_payin->F_orbrno); 		
		/** 找到原小额往帐 **/
		iRc=iLvFetchRecToPayin(wp_payin);  
		vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
		/****HSYX BEGIN:支票回执未找到原交易则补充上****/
		if (iRc==100)
		{
			char pcTxnum[16];
			PAY_IN_AREA wd_bfpayin;
			memset(pcTxnum,'\0',sizeof(pcTxnum));
			memset(&wd_bfpayin,'\0',sizeof(wd_bfpayin));
			GETTAG(wp_lvpack,"0BG",wp_payin->F_txnum);
			vtcp_log("[%s][%d]===========wp_payin->F_txnum=[%s]\n",__FILE__,__LINE__,wp_payin->F_txnum);
			if (memcmp(wp_payin->F_txnum,"30102",5)==0||
				memcmp(wp_payin->F_txnum,"30103",5)==0||
				memcmp(wp_payin->F_txnum,"30104",5)==0||
				memcmp(wp_payin->F_txnum,"30105",5)==0)
			{
				GETTAG(wp_lvpack,"0BG",wd_bfpayin.F_txnum);
				/**addbywangyongwei20070808***/
				if(memcmp(wd_bfpayin.F_txnum,"30104",5)==0){
						memcpy(wd_bfpayin.F_txnum,"30102",5);
				}else if(memcmp(wd_bfpayin.F_txnum,"30105",5)==0){
						memcpy(wd_bfpayin.F_txnum,"30103",5);
				}				
				/**addbywangyongwei20070808***/
				GETTAG(wp_lvpack,"30A",wd_bfpayin.F_respday);
				GETTAG(wp_lvpack,"051",wd_bfpayin.F_wtday);
				GETTAG(wp_lvpack,"0BC",caValue);
				lv_orderno_add0(caValue, sizeof(wd_bfpayin.F_oorderno),wd_bfpayin.F_oorderno);
				GETTAG(wp_lvpack,"CC4",wd_bfpayin.F_payno);
				GETTAG(wp_lvpack,"50C",wd_bfpayin.F_payactno);
				GETTAG(wp_lvpack,"50A",wd_bfpayin.F_payname);
				GETTAG(wp_lvpack,"CC5",wd_bfpayin.F_cashno);
				GETTAG(wp_lvpack,"59C",wd_bfpayin.F_cashactno);
				GETTAG(wp_lvpack,"59A",wd_bfpayin.F_cashname);
				GETTAG(wp_lvpack,"52A",cAcbrno);/**注意此处组往包**/
				vtcp_log("[%s][%d]========cAcbrno=[%s]\n", __FILE__,__LINE__,cAcbrno);
				GETTAG(wp_lvpack,"58A",wd_bfpayin.F_orbrno);/**注意此处组往包**/
				memcpy(wd_bfpayin.F_acbrno, cAcbrno, sizeof(cAcbrno)-1);
				vtcp_log("[%s][%d]========wd_bfpayin.F_acbrno=[%s]\n",
					__FILE__,__LINE__,wd_bfpayin.F_acbrno);
				vtcp_log("[%s][%d]========wd_bfpayin.F_orbrno=[%s]\n",
					__FILE__,__LINE__,wd_bfpayin.F_orbrno);
				memset(caValue,'\0',sizeof(caValue));
				GETTAG(wp_lvpack,"33S",caValue);
				vtcp_log("%s,%d,Gethere,hzamt=[%s]\n",__FILE__,__LINE__,caValue);
				lv_orderno_add0(caValue, sizeof(wd_bfpayin.F_txamt),wd_bfpayin.F_txamt);
				vtcp_log("%s,%d,Gethere,hzamt=[%s]\n",__FILE__,__LINE__,caValue);

				lv_orderno_add0(caValue, sizeof(wd_bfpayin.F_txamt),wd_bfpayin.F_txamt);
				printf("%s,%d,Gethere,hzamt=[%s]\n",__FILE__,__LINE__,caValue);
				printf("%s,%d,Gethere,hzamt=[%.15s]\n",__FILE__,__LINE__,wd_bfpayin.F_txamt);
				GETTAG(wp_lvpack,"CIA",wd_bfpayin.F_rcpstat);
				GETHEADTAG(wp_parent,"012",wd_bfpayin.A_payqsactno);
				GETHEADTAG(wp_parent,"011",wd_bfpayin.A_cashqsactno);
				memcpy(wd_bfpayin.F_pkgno,PKGNO_PTJJ,3);
				memcpy(wd_bfpayin.F_opcd,OPCD_CHECK,sizeof(wd_bfpayin.F_opcd));
				memcpy(wd_bfpayin.F_orderno,wp_payin->F_orderno,sizeof(wp_payin->F_orderno));
				/***根据输入内容生成一条新的纪录****/
				iRc=iLvNewRecFromPayin(&wd_bfpayin);
				if (iRc)
				{
					vtcp_log("%s,%d,iLvNewRecFromPayin错误[%d]\n",__FILE__,__LINE__,lvca.rtcode);
					sprintf( acErrMsg,"根据输入内容生成一条新的纪录错误");
					WRITEMSG
					return(-1);
				}
				printf("[%s][%d]\n", __FILE__,__LINE__);
				/***插入新记录****/
				wd_bfpayin.F_lw_ind[0]='1';/* add by LiuHuafeng 2007-4-4 7:31 */
				wd_bfpayin.T_lw_ind[0]='1';/* add by LiuHuafeng 2007-4-4 7:31 */
				iRc=iLvUpdRecFromPayin(&wd_bfpayin,1);
				if (iRc)
				{
					vtcp_log("%s,%d,插入新记录错误\n",__FILE__,__LINE__,lvca.rtcode);
					sprintf( acErrMsg,"插入新记录错误");
					WRITEMSG
					return(-1);
				}
				memcpy(wp_payin->F_wtday,wd_bfpayin.F_wtday,sizeof(wp_payin->F_wtday));
				memcpy(wp_payin->F_orbrno,wd_bfpayin.F_orbrno,sizeof(wp_payin->F_orbrno));
				memcpy(wp_payin->F_orderno,wd_bfpayin.F_orderno,sizeof(wp_payin->F_orderno));
				wp_payin->F_lw_ind[0]='1';
				iRc=iLvFetchRecToPayin(wp_payin);
			}
		}
		/****HSYX END:支票回执未找到原交易则补充上****/
		if (iRc)
		{
			vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"取得报文内容错误");WRITEMSG
			return(iRc);
		}
		/* ======= 这里应该重置 T_wtday T_orderno T_orbrno等字段  ========= 
		======= 要不然插入的是原记录，重复                     =========   */
		/* begin add by LiuHuafeng 2007-8-22 10:10:27 
		 对于实时贷记业务，如果回执的应答是失败的情况，要把原来往帐的收付双方反过来
		 所以这里需要重新处理一下 begin NEWTCTD1
		****************************************************************************/
		vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
		vtcp_log("%s,%d iRc==%d,F_pkgno=%.3s",__FILE__,__LINE__,iRc,wp_payin->F_pkgno);
		GETTAG(wp_lvpack,"CIA",wp_payin->F_rcpstat);
		if(iRc!=100 && memcmp(wp_lvpack->CMTCode,PKGNO_SSDJHZ,sizeof(wp_lvpack->CMTCode))==0 )
		{
			PAY_IN_AREA wd_o_payin;
			memset(&wd_o_payin, 0 , sizeof(wd_o_payin));
			vtcp_log("%s,%d 实时贷记回执来帐根据原往帐找信息，然后对换收付信息 ",__FILE__,__LINE__);
			memcpy((char*)&wd_o_payin,wp_payin,sizeof(wd_o_payin));
			if(memcmp(wp_payin->F_rcpstat,"00",2)!=0)
			{ /* 如果是拒绝的情况才将收款人和付款人反过来 */
				memcpy(wp_payin->T_cashactno,wd_o_payin.T_payactno ,sizeof(wp_payin->T_cashactno));
				memcpy(wp_payin->T_payactno ,wd_o_payin.T_cashactno,sizeof(wp_payin->T_payactno));
				memcpy(wp_payin->T_payname    ,wd_o_payin.T_cashname,sizeof(wp_payin->T_payname));
				memcpy(wp_payin->T_cashname   ,wd_o_payin.T_payname,sizeof(wp_payin->T_cashname));
			}
			memcpy(wp_payin->T_orbrno     ,wd_o_payin.T_acbrno,sizeof(wp_payin->T_orbrno));
			memcpy(wp_payin->T_acbrno     ,wd_o_payin.T_orbrno,sizeof(wp_payin->T_acbrno));
			memcpy(wp_payin->T_receco     ,wd_o_payin.T_sendco,sizeof(wp_payin->T_receco));
			memcpy(wp_payin->T_sendco     ,wd_o_payin.T_receco,sizeof(wp_payin->T_sendco));
			memcpy(wp_payin->T_payno      ,wd_o_payin.T_cashno,sizeof(wp_payin->T_payno));
			memcpy(wp_payin->T_cashno     ,wd_o_payin.T_payno,sizeof(wp_payin->T_cashno));
			memcpy(wp_payin->T_payqsactno ,wd_o_payin.T_cashqsactno,sizeof(wp_payin->T_payqsactno));
			memcpy(wp_payin->T_cashqsactno,wd_o_payin.T_payqsactno,sizeof(wp_payin->T_cashqsactno));
			vtcp_log("%s,%d T_cashactno  [%.32s]",__FILE__,__LINE__,wp_payin->T_cashactno  );
			vtcp_log("%s,%d T_payactno   [%.32s]",__FILE__,__LINE__,wp_payin->T_payactno   );
			vtcp_log("%s,%d T_orbrno     [%.12s]",__FILE__,__LINE__,wp_payin->T_orbrno     );
			vtcp_log("%s,%d T_acbrno     [%.12s]",__FILE__,__LINE__,wp_payin->T_acbrno     );
			vtcp_log("%s,%d T_receco     [%.4s]",__FILE__,__LINE__,wp_payin->T_receco     );
			vtcp_log("%s,%d T_sendco     [%.4s]",__FILE__,__LINE__,wp_payin->T_sendco     );
			vtcp_log("%s,%d T_payno      [%.12s]",__FILE__,__LINE__,wp_payin->T_payno      );
			vtcp_log("%s,%d T_cashno     [%.12s]",__FILE__,__LINE__,wp_payin->T_cashno     );
			vtcp_log("%s,%d T_payname    [%.60s]",__FILE__,__LINE__,wp_payin->T_payname    );
			vtcp_log("%s,%d T_cashname   [%.60s]",__FILE__,__LINE__,wp_payin->T_cashname   );
			vtcp_log("%s,%d T_payqsactno [%.12s]",__FILE__,__LINE__,wp_payin->T_payqsactno );
			vtcp_log("%s,%d T_cashqsactno[%.12s]",__FILE__,__LINE__,wp_payin->T_cashqsactno);
			vtcp_log("%s,%d ",__FILE__,__LINE__);
			
		}                
		if(iRc!=100 && memcmp(wp_lvpack->CMTCode,PKGNO_SSJJHZ,sizeof(wp_lvpack->CMTCode))==0 )
		{
			PAY_IN_AREA wd_o_payin;
			memset(&wd_o_payin, 0 , sizeof(wd_o_payin));
			vtcp_log("%s,%d 实时借记回执来帐根据原往帐找信息，然后对换发送接收机构信息 ",__FILE__,__LINE__);
			memcpy((char*)&wd_o_payin,wp_payin,sizeof(wd_o_payin));
			memcpy(wp_payin->T_orbrno     ,wd_o_payin.T_acbrno,sizeof(wp_payin->T_orbrno));
			memcpy(wp_payin->T_acbrno     ,wd_o_payin.T_orbrno,sizeof(wp_payin->T_acbrno));
			memcpy(wp_payin->T_receco     ,wd_o_payin.T_sendco,sizeof(wp_payin->T_receco));
			memcpy(wp_payin->T_sendco     ,wd_o_payin.T_receco,sizeof(wp_payin->T_sendco));
			memcpy(wp_payin->T_payqsactno ,wd_o_payin.T_cashqsactno,sizeof(wp_payin->T_payqsactno));
			memcpy(wp_payin->T_cashqsactno,wd_o_payin.T_payqsactno,sizeof(wp_payin->T_cashqsactno));
			vtcp_log("%s,%d T_cashactno  [%.32s]",__FILE__,__LINE__,wp_payin->T_cashactno  );
			vtcp_log("%s,%d T_payactno   [%.32s]",__FILE__,__LINE__,wp_payin->T_payactno   );
			vtcp_log("%s,%d T_orbrno     [%.12s]",__FILE__,__LINE__,wp_payin->T_orbrno     );
			vtcp_log("%s,%d T_acbrno     [%.12s]",__FILE__,__LINE__,wp_payin->T_acbrno     );
			vtcp_log("%s,%d T_receco     [%.4s]",__FILE__,__LINE__,wp_payin->T_receco     );
			vtcp_log("%s,%d T_sendco     [%.4s]",__FILE__,__LINE__,wp_payin->T_sendco     );
			vtcp_log("%s,%d T_payno      [%.12s]",__FILE__,__LINE__,wp_payin->T_payno      );
			vtcp_log("%s,%d T_cashno     [%.12s]",__FILE__,__LINE__,wp_payin->T_cashno     );
			vtcp_log("%s,%d T_payname    [%.60s]",__FILE__,__LINE__,wp_payin->T_payname    );
			vtcp_log("%s,%d T_cashname   [%.60s]",__FILE__,__LINE__,wp_payin->T_cashname   );
			vtcp_log("%s,%d T_payqsactno [%.12s]",__FILE__,__LINE__,wp_payin->T_payqsactno );
			vtcp_log("%s,%d T_cashqsactno[%.12s]",__FILE__,__LINE__,wp_payin->T_cashqsactno);
			vtcp_log("%s,%d ",__FILE__,__LINE__);
		}
		/* end by LiuHuafeng  2007-8-22 11:03:28 */
		/* end NEWTCTD1 */ 
		vtcp_log("%s,%d,xyy_F_txnum=%.5s",__FILE__,__LINE__,wp_payin->F_txnum); 
		iLvResetPayin(wp_payin);  /**  T -> F  **/ 	
    vtcp_log("%s,%d,xyy_F_txnum=%.5s",__FILE__,__LINE__,wp_payin->F_txnum);          
		/* ======= 这里应该重置 T_wtday T_orderno T_orbrno等字段  ========= 
		======= 要不然插入的是原记录，重复                     =========   */

		memcpy(wp_payin->F_owtday,wp_payin->F_wtday,sizeof(wp_payin->F_owtday));	
		memcpy(wp_payin->F_oorderno,wp_payin->F_orderno,sizeof(wp_payin->F_oorderno));	
		/* rem by LiuHuafeng 2007-9-1 0:15:27 for NEWTCTD1
		memcpy(wp_payin->F_o_or_br_no,wp_payin->F_orbrno,sizeof(wp_payin->F_o_or_br_no));	
		*************replace by next code*******************************/
		memcpy(wp_payin->F_o_or_br_no,wp_payin->F_acbrno,sizeof(wp_payin->F_o_or_br_no));	
		memcpy(wp_payin->F_o_ac_br_no,wp_payin->F_orbrno,sizeof(wp_payin->F_o_ac_br_no));
		/* end by LiuHuafeng 2007-9-1 0:16:19 end NEWTCTD1 */
    vtcp_log("%s,%d,xyy_F_txnum=%.5s",__FILE__,__LINE__,wp_payin->F_txnum); 
		memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));	
		vtcp_log("%s,%d,F_pkgno=%.3s",__FILE__,__LINE__,wp_payin->F_pkgno); 

		/*GETTAG_SIZE(wp_lvpack,"30A",wp_payin->F_respday,sizeof(wp_payin->F_respday));		*/
		GETTAG_SIZE(wp_lvpack,"30A",wp_payin->F_wtday,sizeof(wp_payin->F_wtday));	
		memcpy(wp_payin->F_respday,wp_payin->F_wtday,sizeof(wp_payin->F_respday));
		vtcp_log("%s,%d,F_wtday=%.8s",__FILE__,__LINE__,wp_payin->F_wtday); 			
		/* begin NEWTCTD1 */
		/* rem by LiuHuafeng 2007-8-22 14:47:35 新系统是什么就是什么CC1是原发起行行号
		GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));	
		***** end by LiuHuafeng 2007-9-1 0:18:29 */
		/* end NEWTCTD1 */
		vtcp_log("%s,%d,F_orbrno=%.12s",__FILE__,__LINE__,wp_payin->F_orbrno); 
		vtcp_log("%s,%d,F_orderno=%.8s",__FILE__,__LINE__,wp_payin->F_orderno); 
		/*** dongxy 20070313 票交要素集中对应字段是58A begin **/
    		/**add by wangyongwei 20070814**/
    		/**取交易类型号***/
		/**20070912 NEWTCTD wangyongwei***/
		
		/**modify by YHBP**/
		vtcp_log("%s,%d,xyy_F_txnum=%.5s",__FILE__,__LINE__,wp_payin->F_txnum); 
		if(memcmp(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno))==0 && \    
			(memcmp(wp_payin->F_txnum,TXNUM_ZPJL,5)==0|| memcmp(wp_payin->F_txnum,TXNUM_TYJL,5)==0)){
			GETTAG_SIZE(wp_lvpack,"0BG",wp_payin->F_txnum,sizeof(wp_payin->F_txnum));
		}
		/**20070912 NEWTCTD wangyongwei***/
		vtcp_log("%s,%d,F_txnum=%.5s",__FILE__,__LINE__,wp_payin->F_txnum); 
		
     		/**add by wangyongwei 20070814**/
		if (memcmp(wp_payin->F_txnum,"30104",5)==0 || memcmp(wp_payin->F_txnum,"30105",5)==0)
		{
			/**modified wangyongwei 20070808**/
			GETTAG_SIZE(wp_lvpack,"58A",wp_payin->F_acbrno,sizeof(wp_payin->F_acbrno));	
			/**GETTAG_SIZE(wp_lvpack,"52A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));从52A里面取不出值来**/
			GETTAG(wp_lvpack,"52A",cAcbrno);
      			memcpy(wp_payin->F_orbrno,cAcbrno,sizeof(wp_payin->F_orbrno));
			/**modified wangyongwei 20070808**/
		}
		/*** dongxy 20070313 票交要素集中对应字段是58A begin **/

		GETTAG(wp_lvpack,"CIA",wp_payin->F_rcpstat);
		vtcp_log("%s,%d,F_rcpstat=%.2s",__FILE__,__LINE__,wp_payin->F_rcpstat); 

		memcpy(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd));
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"0BC",caValue,sizeof(caValue)-1);/*支付交易序号*/
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		vtcp_log("%s,%d,F_orbrno=%.12s",__FILE__,__LINE__,wp_payin->F_orbrno); 	
		wp_payin->A_stat[0]=STAT_LZRECV1;			
		wp_payin->F_lw_ind[0] = '2';		/*来帐*/	

		vtcp_log("%s,%d,F_wtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_wtday); 
		vtcp_log("%s,%d,F_orderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_orderno); 		
		vtcp_log("%s,%d,F_orbrno=[%.12s]",__FILE__,__LINE__,wp_payin->F_orbrno); 		

		vtcp_log("%s,%d,F_owtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_owtday); 
		vtcp_log("%s,%d,F_oorderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_oorderno); 		
		vtcp_log("%s,%d,F_o_or_br_no=[%.12s]",__FILE__,__LINE__,wp_payin->F_o_or_br_no); 		
		/*beg add by chenggx 定期借记业务的原发起行行号 成了 原接收行行号 修改为 从 CC1中取值*/
		if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0){
			GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_o_or_br_no,sizeof(wp_payin->F_o_or_br_no));
			vtcp_log("%s,%d,CHENGGX 定期借记业务回执走到这儿 否者出错了! PKGNO[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno);
			vtcp_log("%s,%d,F_o_or_br_no=[%.12s]",__FILE__,__LINE__,wp_payin->F_o_or_br_no);
		}
		/*end add by chenggx 定期借记业务的原发起行行号 成了 原接收行行号 修改为 从 CC1中取值*/
		/**   backup by chenming  
		memcpy(wp_payin->F_opcd,OPCD_LR,sizeof(wp_payin->F_opcd));
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,3)==0)
		memcpy(wp_payin->F_pkgno,PKGNO_SSDJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,3)==0)
		memcpy(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,3)==0)
		memcpy(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno));
		if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,3)==0)
		memcpy(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno));

		GETTAG_SIZE(wp_lvpack,"051",wp_payin->F_wtday,sizeof(wp_payin->F_wtday));
		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);   

		GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"005",caValue,sizeof(caValue)-1);
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		wp_payin->F_lw_ind[0] = '1';

		iRc=iLvFetchRecToPayin(wp_payin);
		if (iRc)
		{
		vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"取得报文内容错误");WRITEMSG
		return(iRc);
		}

		iLvResetPayin(wp_payin);

		memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));
		GETTAG_SIZE(wp_lvpack,"30A",wp_payin->F_respday,sizeof(wp_payin->F_respday));
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"0BC",caValue,sizeof(caValue)-1);
		lv_orderno_add0(caValue, sizeof(wp_payin->F_oorderno),wp_payin->F_oorderno);
		GETTAG_SIZE(wp_lvpack,"CIA",wp_payin->F_rcpstat,sizeof(wp_payin->F_rcpstat));
		memcpy(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd));
		memset(wp_payin->T_stat,'\0',sizeof(wp_payin->T_stat));
		wp_payin->A_stat[0]=STAT_LZRECV1;  
		***/

		/****包委托日期****/
		GETHEADTAG(wp_parent,"30E",wp_payin->F_packday);
		/****包id****/
		GETHEADTAG(wp_parent,"0BD",wp_payin->F_packid);
		/**add by wangyongwei **提取退票理**/
		/**附言或退票补充说明**/
		GETTAG_SIZE(wp_lvpack,"72A",wp_payin->F_content,sizeof(wp_payin->F_content));
		vtcp_log("%s,%d,F_content=[%.60s]",__FILE__,__LINE__,wp_payin->F_content);  
		/**支票号码**/
		GETTAG_SIZE(wp_lvpack,"CSH",wp_payin->F_vocnum,sizeof(wp_payin->F_vocnum));
		vtcp_log("%s,%d,F_vounum[%.12s]",__FILE__,__LINE__,wp_payin->F_vocnum); 
		GETHEADTAG(wp_parent,"011",wp_payin->A_payqsactno);/**包发起清算行*/
		vtcp_log("%s,%d,A_payqsactno[%.12s]",__FILE__,__LINE__,wp_payin->A_payqsactno); 
		GETHEADTAG(wp_parent,"012",wp_payin->A_cashqsactno);/*包接收清算行*/ 
		vtcp_log("%s,%d,A_cashqsactno[%.12s]",__FILE__,__LINE__,wp_payin->A_cashqsactno); 
		char tply[11];
		memset(tply,0,sizeof(tply));
		GETTAG_SIZE(wp_lvpack,"CCA",tply,sizeof(tply)-1);
		vtcp_log("%s,%d,退票代码为:[%s]\n",__FILE__,__LINE__,tply);
		/**NEWTCTD 20070912 wangyongwei **/
		if(memcmp(wp_payin->F_txnum,"30104",5)==0 || memcmp(wp_payin->F_txnum,"30105",5)==0){
			iRc=iLvSetAddit(wp_payin,"TPL",tply,10);
		}
		/**add by wangyongwei **提取退票理由**/
		
		return(0);	
	}
	/**** 以下为非回执业务  ****/	
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);
	/**** 分三类处理 ****/
	if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,sizeof(wp_payin->F_pkgno))==0)
	{		/****定期贷记****/
		int i=0;
		/* static int offset; add by LiuHuafeng 0406 */
		int offset=0; 
		char cAddLen[9];
		memset(cAddLen,'\0',sizeof(cAddLen));
		errLog(LOG_ERR,0,RPT_TO_TTY,(char*)NULL,0,"%s,%d,beginwhile time:",__FILE__,__LINE__);
		/** rem by LiuHuafeng 
		for(i=0,offset=0;i<id;i++){
		iRc=lv_get_tag_multi(wp_lvpack,"B81",cAddLen,
		offset+sizeof(DQDJ_DTL_AREA)-9,8);
		if(iRc){
		vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"取得报文内容错误");
		return(iRc);
		}
		offset+=sizeof(DQDJ_DTL_AREA)-1;
		}
		************************/
		/************************* Changed by ChenMing 2007.04.02.
		if (id==0) offset=0; * add by lhf 20060406 *
		errLog(LOG_ERR,0,RPT_TO_TTY,(char*)NULL,0,"%s,%d,endwhile time:",__FILE__,__LINE__);
		iRc=lv_get_tag_multi(wp_lvpack,"B81",(char*)&wd_djdtl,
		offset,sizeof(DQDJ_DTL_AREA)-1);
		if (iRc)
		{
		vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"取得报文内容错误");WRITEMSG
		return(iRc);
		}
		nTxAddLen=apatoi(wd_djdtl.addlen,sizeof(wd_djdtl.addlen));
		errLog(LOG_ERR,0,RPT_TO_TTY,(char*)NULL,0,"%s,%d,begin get_tag_mulit time:",__FILE__,__LINE__);
		iRc=lv_get_tag_multi(wp_lvpack,"B81",cTxAddText,
		offset+sizeof(DQDJ_DTL_AREA)-1,nTxAddLen);

		offset+=sizeof(DQDJ_DTL_AREA)-1; * add by lhf 20060406 *
		************************************************/
		vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);	
		for (i=0; i<=id; i++)
		{
			memset(&wd_jjdtl,'\0',sizeof(wd_jjdtl));
			/* 首先取收款人清单的前面几个固定的域  */
			iRc=lv_get_tag_multi(wp_lvpack,"B81", (char*)&wd_djdtl,	offset, sizeof(DQDJ_DTL_AREA)-1);
			if (iRc)
			{
				vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"取得报文内容错误");
				return(iRc);
			}
			vtcp_log("%s,%d addlen=[%s]  orderno=[%s]",__FILE__,__LINE__,wd_djdtl.addlen, wd_djdtl.orderno);
			/* 根据附加数据长度取附加数据 */
			nTxAddLen = apatoi(wd_djdtl.addlen,sizeof(wd_djdtl.addlen));
			iRc=lv_get_tag_multi(wp_lvpack,"B81", cTxAddText, offset+sizeof(DQDJ_DTL_AREA)-1, nTxAddLen);
			offset+=sizeof(DQDJ_DTL_AREA)-1+nTxAddLen;  /* 下条记录的位置 */

			vtcp_log("%s,%d offset=====[%d]",__FILE__,__LINE__,offset);	
			vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_djdtl.orderno);
		}
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0) 
	{   /****定期借记****/
		int i=0;
		int offset=0;
		char cAddLen[9];
		memset(cAddLen,'\0',sizeof(cAddLen));
		/* rem by LiuHuafeng 20060406 
		for(i=0,offset=0;i<id;i++){
			iRc=lv_get_tag_multi(wp_lvpack,"B82",cAddLen,
			offset+sizeof(DQJJ_DTL_AREA)-9,8);
			if(iRc){
				vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"取得报文内容错误");
				return(iRc);
			}
			offset+=sizeof(DQJJ_DTL_AREA)-1;
		}

		vtcp_log("%s,%d id=[%d] offset=[%d]",__FILE__,__LINE__,id,offset);
		if (id==0) offset=0;
		iRc=lv_get_tag_multi(wp_lvpack,"B82",(char*)&wd_jjdtl,
		offset,sizeof(DQJJ_DTL_AREA)-1);
		if (iRc)
		{
		vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"取得报文内容错误");WRITEMSG
		return(iRc);
		}
		nTxAddLen=apatoi(wd_jjdtl.addlen,sizeof(wd_jjdtl.addlen));
		iRc=lv_get_tag_multi(wp_lvpack,"B82",cTxAddText,
		offset+sizeof(DQJJ_DTL_AREA)-1,nTxAddLen);
		offset +=sizeof(DQJJ_DTL_AREA)-1+nTxAddLen;
		*************************************/

		vtcp_log("%s,%d,定期借记  \n",__FILE__,__LINE__);

		vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);	
		offset = 0;
		for (i=0; i<=id; i++)
		{
			memset(&wd_jjdtl,'\0',sizeof(wd_jjdtl));
			/* 首先取付款人清单的前面几个固定的域  */
			iRc=lv_get_tag_multi(wp_lvpack,"B82", (char*)&wd_jjdtl,	offset, sizeof(DQJJ_DTL_AREA)-1);
			if (iRc)
			{
				vtcp_log("%s,%d,取得报文内容错误\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"取得报文内容错误");
				return(iRc);
			}
			/* 根据附加数据长度取附加数据 */
			nTxAddLen=apatoi(wd_jjdtl.addlen,sizeof(wd_jjdtl.addlen));
			iRc=lv_get_tag_multi(wp_lvpack,"B82", cTxAddText, offset+sizeof(DQJJ_DTL_AREA)-1, nTxAddLen);
			offset+=sizeof(DQJJ_DTL_AREA)-1+nTxAddLen;  /* 下条记录的位置 */

			vtcp_log("%s,%d offset=[%d]",__FILE__,__LINE__,offset);	
			vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_jjdtl.orderno);

		}
		vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_jjdtl.orderno);
	}
	else		/****非定期业务****/
	{
		memset(caValue,'\0',sizeof(caValue));
		GETTAG(wp_lvpack,"B40",caValue);    /*  附加数据长度  */
		nTxAddLen=atoi(caValue);
		GETTAG(wp_lvpack,"72C",cTxAddText); /*  附加数据  */

	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	/* 置借贷标志 */	
	if (memcmp(wp_payin->F_pkgno,PKGNO_PTDJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DJTH,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno))==0)
	{  
		wp_payin->F_crdb[0]=CRDB_CR;
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno))==0)
	{
		wp_payin->F_crdb[0] = CRDB_DR;
	}
	else
	{
		vtcp_log("%s,%d,非法的pkgno:[%.3s]\n",	__FILE__,__LINE__,wp_payin->F_pkgno);
		sprintf( acErrMsg,"非法的报文类型");WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));	
	memcpy(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd)); /**操作代码 **/
	vtcp_log("%s,%d,F_opcd=%.1s\n",__FILE__,__LINE__,wp_payin->F_opcd);

	GETTAG_SIZE(wp_lvpack,"0BG",wp_payin->F_txnum,sizeof(wp_payin->F_txnum));	/****业务类型号****/
	vtcp_log("%s,%d,F_txnum=%.5s\n",__FILE__,__LINE__,wp_payin->F_txnum);

	GETTAG_SIZE(wp_lvpack,"CEG",wp_payin->F_ywtype,sizeof(wp_payin->F_ywtype));	/****业务种类****/
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	GETTAG_SIZE(wp_lvpack,"30A",wp_payin->F_wtday,8);	/****委托日期****/
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);

	wp_payin->F_operlevel[0]=wp_lvpack->paymPRI;	/****优先级****/
	vtcp_log("%s,%d,paymPRI=%.1s\n",__FILE__,__LINE__,wp_payin->F_operlevel);

	/****Modified by SSH,2005.12.1,汇总信息从父包中取****/
	GETHEADTAG(wp_parent,"30E",wp_payin->F_packday);	/****包委托日期****/
	vtcp_log("%s,%d,F_packday=%.8s\n",__FILE__,__LINE__,wp_payin->F_packday);	

	memset(caValue,0,sizeof(caValue));
	GETHEADTAG(wp_parent,"0BD",caValue); /* 包序号 */

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);	
	lv_orderno_add0(caValue, sizeof(wp_payin->F_packid),wp_payin->F_packid);
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);	

	vtcp_log("%s,%d,F_packid=%.8s\n",__FILE__,__LINE__,wp_payin->F_packid);

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);

	GETHEADTAG(wp_parent,"011",wp_payin->A_payqsactno); /* 包发起清算行行号 */
	GETHEADTAG(wp_parent,"012",wp_payin->A_cashqsactno);/* 包接收清算行行号 */
	vtcp_log("%s,%d A_payqsactno=%.12s\n",__FILE__,__LINE__,wp_payin->A_payqsactno);	
	vtcp_log("%s,%d A_cashqsactno=%.12s\n",__FILE__,__LINE__,wp_payin->A_cashqsactno);

	memcpy(wp_payin->F_respday,wp_lvpack->respDate,sizeof(wp_payin->F_respday));	/****应答日期****/
	vtcp_log("%s,%d,F_respday=%.8s\n",__FILE__,__LINE__,wp_payin->F_respday);
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,sizeof(wp_payin->F_pkgno))==0)
	{
		/********************定期贷记***********************************/
		/***************************************************************/
		/****支付交易序号****/
		memcpy(wp_payin->F_orderno,wd_djdtl.orderno,sizeof(wp_payin->F_orderno));
		/****发起行行号****/

		GETTAG_SIZE(wp_lvpack,"CCM",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		/****接收行行号****/
		memcpy(wp_payin->F_acbrno,wd_djdtl.acbrno,	
			sizeof(wp_payin->F_acbrno));
		/****交易金额****/
		memcpy(wp_payin->F_txamt,wd_djdtl.txamt,	
			sizeof(wp_payin->F_txamt));
		/****付款人开户行****/

		GETTAG_SIZE(wp_lvpack,"CC4",wp_payin->F_payno,sizeof(wp_payin->F_payno));
		/****付款人帐号*****/

		GETTAG_SIZE(wp_lvpack,"50C",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
		/****付款人名称****/

		GETTAG_SIZE(wp_lvpack,"50A",wp_payin->F_payname,sizeof(wp_payin->F_payname));
		/****付款人地址****/

		GETTAG_SIZE(wp_lvpack,"50B",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
		/****收款人开户行****/
		memcpy(wp_payin->F_cashno,wd_djdtl.cashno,	
			sizeof(wp_payin->F_cashno));
		/****收款人帐号****/
		memcpy(wp_payin->F_cashactno,wd_djdtl.cashactno,	
			sizeof(wp_payin->F_cashactno));
		/****收款人名称****/
		memcpy(wp_payin->F_cashname,wd_djdtl.cashname,	
			sizeof(wp_payin->F_cashname));
		/****收款人地址****/
		memcpy(wp_payin->F_cashaddress,wd_djdtl.cashaddress,	
			sizeof(wp_payin->F_cashaddress));
		/****附言****/
		memcpy(wp_payin->F_content,wd_djdtl.content,	
			sizeof(wp_payin->F_content));

	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0 || 
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ, sizeof(wp_payin->F_pkgno))==0)
	{
		/*********************定期借记***********************************/ 
		/***************************************************************/
		vtcp_log("%s,%d-------wd_jjdtl:\n%s\n------\n",__FILE__,__LINE__,&wd_jjdtl);
		/****支付交易序号****/ 
		memcpy(wp_payin->F_orderno,wd_jjdtl.orderno,	sizeof(wp_payin->F_orderno)); 
		/****发起行行号****/ 
		GETTAG_SIZE(wp_lvpack,"CCN",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		/****接收行行号****/ 
		memcpy(wp_payin->F_acbrno,wd_jjdtl.acbrno,	sizeof(wp_payin->F_acbrno)); 
		/****交易金额****/ 
		memcpy(wp_payin->F_txamt,wd_jjdtl.txamt,	sizeof(wp_payin->F_txamt));
		/****付款人开户行****/
		memcpy(wp_payin->F_payno,wd_jjdtl.payno,	sizeof(wp_payin->F_payno));
		/****付款人帐号*****/
		memcpy(wp_payin->F_payactno,wd_jjdtl.payactno,	sizeof(wp_payin->F_payactno));
		/****付款人名称****/
		memcpy(wp_payin->F_payname,wd_jjdtl.payname,	sizeof(wp_payin->F_payname));
		/****付款人地址****/
		memcpy(wp_payin->F_payaddress,wd_jjdtl.payaddress,sizeof(wp_payin->F_payaddress));
		/****收款人开户行****/
		GETTAG_SIZE(wp_lvpack,"CC5",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
		/****收款人帐号****/
		GETTAG_SIZE(wp_lvpack,"59C",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
		/****收款人名称****/
		GETTAG_SIZE(wp_lvpack,"59A",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
		/****收款人地址****/
		GETTAG_SIZE(wp_lvpack,"59B",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress));
		/****附言****/
		memcpy(wp_payin->F_content,wd_jjdtl.content,	sizeof(wp_payin->F_content));
		/***add by liuyue 20070110***/
		memcpy(wp_payin->F_dfxyno,wd_jjdtl.protno,sizeof(wd_jjdtl.protno));
		iLvSetAddit(wp_payin,"XYH",wd_jjdtl.protno,sizeof(wd_jjdtl.protno));

		vtcp_log("[%s][%d] wp_payin->F_orderno====[%s]\n",__FILE__,__LINE__,wp_payin->F_orderno);
		vtcp_log("[%s][%d] wp_payin->F_acbrno====[%s]\n",__FILE__,__LINE__,wp_payin->F_acbrno);
		vtcp_log("[%s][%d] wp_payin->F_txamt====[%s]\n",__FILE__,__LINE__,wp_payin->F_txamt);
		vtcp_log("[%s][%d] wp_payin->F_payno====[%s]\n",__FILE__,__LINE__,wp_payin->F_payno);
		vtcp_log("[%s][%d] wp_payin->F_payactno====[%s]\n",__FILE__,__LINE__,wp_payin->F_payactno);
		vtcp_log("[%s][%d] wp_payin->F_payname====[%s]\n",__FILE__,__LINE__,wp_payin->F_payname);
		vtcp_log("[%s][%d] wp_payin->F_payaddress====[%s]\n",__FILE__,__LINE__,wp_payin->F_payaddress);
		vtcp_log("[%s][%d] wp_payin->F_content====[%s]\n",__FILE__,__LINE__,wp_payin->F_content);

	}
	else
	{
		/*********************非定期业务**********************************/
		/***************************************************************/

		/****支付交易序号****/
		/* by lhf 060303  不明白
		GETTAG_SIZE(wp_lvpack,"0BC",wp_payin->F_orderno);
		*********************************************/
		memset(caValue, 0 , sizeof(caValue));
		GETTAG(wp_lvpack,"0BC",caValue);
		vtcp_log("[%s][%d] value====[%s]\n",__FILE__,__LINE__,caValue);

		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);		
		vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	  
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);	  
		vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 

		vtcp_log("[%s][%d] value====[%s]\n",__FILE__,__LINE__,caValue);
		vtcp_log("[%s][%d] value====[%.15s]\n",__FILE__,__LINE__,wp_payin->F_orderno);
		/****发起行行号****/
		if (GETTAG_SIZE(wp_lvpack,"CC2",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno))!=0)
		{
			/**没有原接收行,使用发起行***/
			GETTAG_SIZE(wp_lvpack,"52A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		}
		if (GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_acbrno,sizeof(wp_payin->F_acbrno))!=0)
		{
			/**没有原发起行,使用接收行***/
			GETTAG_SIZE(wp_lvpack,"58A",wp_payin->F_acbrno,sizeof(wp_payin->F_acbrno));
		}
		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
		vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 

		GETTAG_SIZE(wp_lvpack,"72A",wp_payin->F_content,sizeof(wp_payin->F_content));/*这里有问题，暂时空着	**附言****/
		vtcp_log("%s,%d,F_content=%.60s\n",__FILE__,__LINE__,wp_payin->F_content);
		if (memcmp(wp_payin->F_pkgno,PKGNO_DJTH,	sizeof(wp_payin->F_pkgno))==0)
		{
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);			
			/*******************贷记退汇********************************/
			/***********************************************************/
			/****交易金额****/
			GETTAG_AMT(wp_lvpack,"33S",wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
			/****付款人开户行****/
			GETTAG_SIZE(wp_lvpack,"CCK",wp_payin->F_payno,sizeof(wp_payin->F_payno));
			/****付款人帐号*****/
			GETTAG_SIZE(wp_lvpack,"CQ2",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
			/****付款人名称****/
			GETTAG_SIZE(wp_lvpack,"CR2",wp_payin->F_payname,sizeof(wp_payin->F_payname));
			/****付款人地址****/
			GETTAG_SIZE(wp_lvpack,"59F",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
			/****收款人开户行****/
			GETTAG_SIZE(wp_lvpack,"CCL",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
			/****收款人帐号****/
			GETTAG_SIZE(wp_lvpack,"CQ1",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
			/****收款人名称****/
			GETTAG_SIZE(wp_lvpack,"CR1",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
			/****收款人地址****/
			GETTAG_SIZE(wp_lvpack,"50F",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress));
			/****原委托日期****/
			GETTAG_SIZE(wp_lvpack,"051",wp_payin->F_owtday,sizeof(wp_payin->F_owtday));
			/****原业务类型号****/
			GETTAG_SIZE(wp_lvpack,"0BH",wp_payin->F_otxnum,sizeof(wp_payin->F_otxnum));
			/****原支付交易序号****/
			/* rem by lhf
			GETTAG_SIZE(wp_lvpack,"005",wp_payin->F_oorderno);
			*********************************************/
			memset(caValue, 0  , sizeof(caValue));
			GETTAG(wp_lvpack,"005",caValue);
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);			
			lv_orderno_add0(caValue, sizeof(wp_payin->F_oorderno),wp_payin->F_oorderno);
			/***退汇原因**/
			GETTAG_SIZE(wp_lvpack,"CIA",wp_payin->F_retcode,sizeof(wp_payin->F_retcode));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
		}else
		{
			/********************非贷记退汇*****************************/
			/***********************************************************/
			/****交易金额****/
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);		
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 	    	
			GETTAG_AMT(wp_lvpack,"33G",wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	

			vtcp_log("[%s][%d]===========wp_payin->F_txamt=[%s]\n",__FILE__,__LINE__,wp_payin->F_txamt);
			/****付款人开户行****/
			GETTAG_SIZE(wp_lvpack,"CC4",wp_payin->F_payno,sizeof(wp_payin->F_payno));
			/****付款人帐号*****/
			GETTAG_SIZE(wp_lvpack,"50C",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
			/****付款人名称****/
			GETTAG_SIZE(wp_lvpack,"50A",wp_payin->F_payname,sizeof(wp_payin->F_payname));
			vtcp_log("%s,%d 付款人户名=[%s]",__FILE__,__LINE__,wp_payin->F_payname);
			/****付款人地址****/
			GETTAG_SIZE(wp_lvpack,"50B",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
			/****收款人开户行****/
			GETTAG_SIZE(wp_lvpack,"CC5",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
			/****收款人帐号****/
			GETTAG_SIZE(wp_lvpack,"59C",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
			/****收款人名称****/
			GETTAG_SIZE(wp_lvpack,"59A",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
			vtcp_log("%s,%d 收款人户名=[%s]",__FILE__,__LINE__,wp_payin->F_payname);
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);				
			/****收款人地址  ****/
			GETTAG_SIZE(wp_lvpack,"59B",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress)); 
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 
		}
	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 	
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	/************************************************************/
	if (memcmp(wp_payin->F_txnum,TXNUM_WT,sizeof(wp_payin->F_txnum))==0)
	{/****委托收款业务的附加字段****/
		wp_wtadd=(WT_ADD_AREA *)cTxAddText;
		memcpy(wp_payin->F_ptype,wp_wtadd->ptype,sizeof(wp_payin->F_ptype));
		memcpy(wp_payin->F_pdate,wp_wtadd->pdate,sizeof(wp_payin->F_pdate));
		memcpy(wp_payin->F_pnum,wp_wtadd->pnum,sizeof(wp_payin->F_pnum));
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TSCF, sizeof(wp_payin->F_txnum))==0)
	{		/****托收承付业务的附加字段****/
		wp_tscfadd=(TSCF_ADD_AREA *)cTxAddText;
		memcpy(wp_payin->F_pdate,wp_tscfadd->pdate,sizeof(wp_payin->F_pdate));
		memcpy(wp_payin->F_pnum,wp_tscfadd->pnum,sizeof(wp_payin->F_pnum));
		memcpy(wp_payin->F_ptxamt,wp_tscfadd->ptxamt, sizeof(wp_payin->F_ptxamt));
		memcpy(wp_payin->F_jtxamt,wp_tscfadd->jtxamt, sizeof(wp_payin->F_jtxamt));
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TC, sizeof(wp_payin->F_txnum))==0)
	{/****通存业务****/
		wp_tcadd=(TCTD_ADD_AREA *)cTxAddText;
		wp_payin->F_actype[0]=wp_tcadd->payacttype[0];
		wp_payin->F_pswdtype[0]=wp_tcadd->pswdtype[0];
		memcpy(wp_payin->F_pswd,pcUndesPswdCode(wp_tcadd->pswdcode),
			sizeof(wp_payin->F_pswd));
		iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
		if (iRc)
		{
			vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"插入附加数据表失败");WRITEMSG
			return(-1);
		}
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TD, sizeof(wp_payin->F_txnum))==0)
	{		/****通兑业务****/
		wp_tdadd=(TCTD_ADD_AREA *)cTxAddText;
		wp_payin->F_actype[0]=wp_tdadd->payacttype[0];
		wp_payin->F_pswdtype[0]=wp_tdadd->pswdtype[0];
		memcpy(wp_payin->F_pswd,pcUndesPswdCode(wp_tdadd->pswdcode),
			sizeof(wp_payin->F_pswd));
		iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
		if (iRc)
		{
			vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"插入附加数据表失败");WRITEMSG
			return(-1);
		}
	}
	/****HSYX BEGIN:支票截留来帐,增加一个验印结果的附加域,以备将来使用***/
	else if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL, sizeof(wp_payin->F_txnum))==0||
		memcmp(wp_payin->F_txnum,TXNUM_TYJL, sizeof(wp_payin->F_txnum))==0)
	{
		vtcp_log("[%s][%d]-------pkgno=[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
		if(memcmp(wp_payin->F_pkgno,"002",sizeof(wp_payin->F_pkgno))==0)/**modify by YHBP**/
		{
		    /**add by wangyongwei 为测试添加审核结果**/
        iRc=iLvSetAddit(wp_payin,"TPL","",2);
		    /**add by wangyongwei 为测试添加审核结果**/
		    /****支票/通用截留****/
		    char pcChy[2];
		    memset(pcChy,'\0',sizeof(pcChy));
		    pcChy[0]=CHY_NOTCHECK;
		    iRc=iLvSetAddit(wp_payin,"CHY",pcChy,1);
		}
		if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****处理支票截留业务附加域****/
			int iAddLen1=0,iAddLen2=0,iAddLen3=0;
			ZPJL1_ADD_AREA wd_zp_add1;
			ZPJL2_ADD_AREA wd_zp_add2;
			ZPJL3_ADD_AREA wd_zp_add3;
			memset(&wd_zp_add1,'\0',sizeof(wd_zp_add1));
			memset(&wd_zp_add2,'\0',sizeof(wd_zp_add2));
			memset(&wd_zp_add3,'\0',sizeof(wd_zp_add3));
			iAddLen1=iAddLen2=iAddLen3=0;
			memcpy(&wd_zp_add1,cTxAddText,sizeof(wd_zp_add1));
			iAddLen1=sizeof(wd_zp_add1);
			memcpy(&wd_zp_add2,cTxAddText+iAddLen1,2);
			iAddLen2=apatoi(&wd_zp_add2,2)*60+2;
			memcpy((char *)&wd_zp_add2+2,cTxAddText+iAddLen1+2,iAddLen2-2);
			memcpy(&wd_zp_add3,cTxAddText+iAddLen1+iAddLen2,sizeof(wd_zp_add3));
			iAddLen3=sizeof(wd_zp_add3);
			iRc=iLvSetAddit(wp_payin,"001",&wd_zp_add1,iAddLen1);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"002",&wd_zp_add2,iAddLen2);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"003",&wd_zp_add3,iAddLen3);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
		}else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****处理通用截留业务附加域****/
			int iAddLen1,iAddLen2,iAddLen3,iAddLen4,iAddLen5;
			TYJL1_ADD_AREA wd_ty_add1;
			TYJL2_ADD_AREA wd_ty_add2;
			TYJL3_ADD_AREA wd_ty_add3;
			TYJL4_ADD_AREA wd_ty_add4;
			TYJL5_ADD_AREA wd_ty_add5;
			memset(&wd_ty_add1,'\0',sizeof(wd_ty_add1));
			memset(&wd_ty_add2,'\0',sizeof(wd_ty_add2));
			memset(&wd_ty_add3,'\0',sizeof(wd_ty_add3));
			memset(&wd_ty_add4,'\0',sizeof(wd_ty_add4));
			memset(&wd_ty_add5,'\0',sizeof(wd_ty_add5));
			iAddLen1=iAddLen2=iAddLen3=0;
			memcpy(&wd_ty_add1,cTxAddText,sizeof(wd_ty_add1));
			iAddLen1=sizeof(wd_ty_add1);
			memcpy(&wd_ty_add2,cTxAddText+iAddLen1,2);
			iAddLen2=apatoi(&wd_ty_add2,2)*60+2;
			memcpy((char *)&wd_ty_add2+2,cTxAddText+iAddLen1+2,iAddLen2-2);
			memcpy(&wd_ty_add3,cTxAddText+iAddLen1+iAddLen2,sizeof(wd_ty_add3));
			iAddLen3=sizeof(wd_ty_add3);
			memcpy(&wd_ty_add4,cTxAddText+iAddLen1+iAddLen2+iAddLen3,sizeof(wd_ty_add4));
			iAddLen4=sizeof(wd_ty_add4);
			memcpy(&wd_ty_add5,cTxAddText+iAddLen1+iAddLen2+iAddLen3+iAddLen4,sizeof(wd_ty_add5));
			iAddLen5=sizeof(wd_ty_add5);
			iRc=iLvSetAddit(wp_payin,"001",&wd_ty_add1,iAddLen1);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"002",&wd_ty_add2,iAddLen2);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"003",&wd_ty_add3,iAddLen3);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"004",&wd_ty_add4,iAddLen4);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"005",&wd_ty_add5,iAddLen5);
			if (iRc)
			{
				vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"插入附加数据表失败");
				WRITEMSG
					return(-1);
			}
		}
		/****HSYX END:支票截留来帐,增加一个验印结果的附加域,以备将来使用***/
	}
	else if (memcmp(wp_payin->F_txnum,TXNUM_DQJJ, sizeof(wp_payin->F_txnum))==0||memcmp(wp_payin->F_txnum,TXNUM_PLKS, sizeof(wp_payin->F_txnum))==0)
	{
		vtcp_log("[%s][%d]定期借记业务!!!!!!!!!!\n",__FILE__,__LINE__);
	}
	else 
	{		/****其他附加字段****/
		vtcp_log("%s,%d,=============未知的附加数据域\n",__FILE__,__LINE__);
		vtcp_log("[%s]\n",cTxAddText);
		if (nTxAddLen!=0)
		{
			/* Remed by Chenming  2006-10-11 17:40   =============
			iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
			if (iRc)
			{
			vtcp_log("%s,%d,插入附加数据表失败\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"插入附加数据表失败");WRITEMSG
			return(-1);
			}			
			==============      使用多方协议号，不再插附加域      */
			GETTAG_SIZE(wp_lvpack,"B82",cTxAddText,sizeof(cTxAddText));
			memcpy(wp_payin->F_dfxyno, cTxAddText+207, sizeof(wp_payin->F_dfxyno));
			vtcp_log("%s,%d,wp_payin->F_dfxyno===\n[%.60s]",__FILE__,__LINE__,wp_payin->F_dfxyno);
			/* Added by Chenming  2006-10-11 17:40   =============*/

		}
	}
	/* Added by ChenMing   2006.09.26. */
	/* rem by LiuHuafeng 2006-9-29 23:43 为什么要加这个*/
	/*  char cDtlAmt[18];                              */

	/*	GETHEADTAG(wp_parent,"B63",wp_payin->F_txamt);  ***业务总笔数****/ 
	/*	memset(cDtlAmt,'\0',sizeof(cDtlAmt));	  */
	/*	GETHEADTAG(wp_parent,"32B",cDtlAmt);   ***业务总金额****/
	/*	memcpy(wp_payin->F_txamt,cDtlAmt+3,sizeof(wp_payin->F_txamt));                     */
	/*                                                                                     */
	/*	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);              */
	/*	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy);    */

	iLvPayinAutoInfo(wp_payin);

	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 	
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	return(0);
}
/*************************************************************************/
/* 函数名:lv_fetchcmt_to_notpayin                                       */
/* 作  用:从CMT报文中取内容填充通用数据交换区                            */
/*************************************************************************/
int lv_fetchcmt_to_notpayin(TLVPACKAGE *lv, NOTPAY_IN_AREA *wp_notpayin)
{
	char	Txt[19];
	char cTmpOrderno[9]; /* add by LiuHuafeng 20060218 */
	memset(Txt, '\0', sizeof(Txt));

	vtcp_log("[%s][%d]in lv_fetchcmt_to_notpayin\n",__FILE__,__LINE__);
	vtcp_log("[%s][%d]===========lv->cmtcode=[%s]\n",__FILE__,__LINE__,lv->CMTCode);
	if (memcmp(lv->CMTCode,"301",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "301",		sizeof(wp_notpayin->cmtno));
		GETTAG_SIZE(lv,"056",wp_notpayin->orderno,sizeof(wp_notpayin->orderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/
		GETTAG_SIZE(lv,"01D",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno));
		wp_notpayin->rettype = '0';
		/** respcode **/
		/** opackday **/
		/** opkgno **/
		/** opackid **/
		/** otxnum **/
		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,sizeof(wp_notpayin->owtday));
		GETTAG_SIZE(lv,"005",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end add by LiuHuafeng 20060218 */
		vtcp_log("[%s][%d] oorderno===[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"CC1",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		GETTAG_SIZE(lv,"CC2",wp_notpayin->oacbrno,sizeof(wp_notpayin->oacbrno));
		GETTAG_SIZE(lv,"CE2",&(wp_notpayin->otxtype),1);
		GETTAG(lv,"CND",Txt);
		memcpy(wp_notpayin->cour,Txt,3);
		memcpy(wp_notpayin->otxamt,Txt+3,sizeof(wp_notpayin->otxamt));
		GETTAG_SIZE(lv,"053",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"01C",wp_notpayin->orbrno ,sizeof(wp_notpayin->orbrno ));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		GETTAG_SIZE(lv,"010",wp_notpayin->sendco    ,sizeof(wp_notpayin->sendco    ));
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		GETTAG_SIZE(lv,"0B9",wp_notpayin->receco,sizeof(wp_notpayin->receco));
		memcpy(wp_notpayin->wssrno,lvca.wssrno,sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"CJ9",wp_notpayin->txday,sizeof(wp_notpayin->txday));
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"302",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "302",		sizeof(wp_notpayin->cmtno));
		/** txnum **/
		GETTAG_SIZE(lv,"01C",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno));
		wp_notpayin->rettype = '0';
		/** respcode **/
		/** opackday **/
		/** opkgno **/
		/** opackid **/
		/** otxnum **/
		GETTAG_SIZE(lv,"CJ1",wp_notpayin->txday     ,sizeof(wp_notpayin->txday     ));
		GETTAG_SIZE(lv,"010",wp_notpayin->sendco    ,sizeof(wp_notpayin->sendco    ));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		GETTAG_SIZE(lv,"01D",wp_notpayin->orbrno    ,sizeof(wp_notpayin->orbrno    ));
		GETTAG_SIZE(lv,"043",wp_notpayin->orderno   ,sizeof(wp_notpayin->orderno   ));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		GETTAG_SIZE(lv,"0B9",wp_notpayin->receco     ,sizeof(wp_notpayin->receco     ));
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/*GETTAG_SIZE(lv,"01C",wp_notpayin->oorbrno    ,sizeof(wp_notpayin->oorbrno    )); 
		Remed by chenming  2006.10.10.*/
		GETTAG_SIZE(lv,"01C",wp_notpayin->qorbrno    ,sizeof(wp_notpayin->qorbrno));			
		GETTAG_SIZE(lv,"01C",wp_notpayin->acbrno     ,sizeof(wp_notpayin->acbrno     ));
		GETTAG_SIZE(lv,"CJA",wp_notpayin->qwtday     ,sizeof(wp_notpayin->qwtday     ));
		GETTAG_SIZE(lv,"CP1",wp_notpayin->qorderno   ,sizeof(wp_notpayin->qorderno   ));

		vtcp_log("%s,%d, wp_notpayin->qwtday =[%.8s]",__FILE__,__LINE__,wp_notpayin->qwtday );		
		vtcp_log("%s,%d, wp_notpayin->qorbrno =[%.12s]",__FILE__,__LINE__,wp_notpayin->qorbrno );
		vtcp_log("%s,%d, wp_notpayin->qorderno =[%.8s]",__FILE__,__LINE__,wp_notpayin->qorderno );

		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->qorderno, sizeof(wp_notpayin->qorderno), cTmpOrderno);
		memcpy(wp_notpayin->qorderno,cTmpOrderno,sizeof(wp_notpayin->qorderno));
		/* end by LiuHuafeng 20060218 */
		/*GETTAG_SIZE(lv,"051",wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday)); 

		GETTAG_SIZE(lv,"051",wp_notpayin->qwtday,sizeof(wp_notpayin->qwtday));
		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,sizeof(wp_notpayin->owtday));
		GETTAG_SIZE(lv,"CC1",wp_notpayin->qorbrno,sizeof(wp_notpayin->qorbrno));
		GETTAG_SIZE(lv,"CC2",wp_notpayin->racbrno,sizeof(wp_notpayin->racbrno));		
		Modified by chenming  2006.10.10.*/

		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,sizeof(wp_notpayin->owtday));
		GETTAG_SIZE(lv,"CC1",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno)); 
		GETTAG_SIZE(lv,"CC2",wp_notpayin->oacbrno,sizeof(wp_notpayin->oacbrno));
		GETTAG_SIZE(lv,"CE2",&(wp_notpayin->otxtype),1);
		GETTAG_SIZE(lv,"005",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));

		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->qorderno, sizeof(wp_notpayin->qorderno), cTmpOrderno);
		memcpy(wp_notpayin->qorderno,cTmpOrderno,sizeof(wp_notpayin->qorderno));
		/* end by LiuHuafeng 20060218 */
		GETTAG(lv,"CND",Txt);
		memcpy(wp_notpayin->cour,Txt,3);
		memcpy(wp_notpayin->otxamt,Txt+3,sizeof(wp_notpayin->otxamt));
		GETTAG_SIZE(lv,"053",wp_notpayin->content,sizeof(wp_notpayin->content));
		memcpy(wp_notpayin->wssrno, lvca.wssrno, sizeof(wp_notpayin->wssrno));
		wp_notpayin->stat = STAT_LZRECV1;

		vtcp_log("%s,%d, wp_notpayin->qwtday =[%.8s]",__FILE__,__LINE__,wp_notpayin->qwtday );		
		vtcp_log("%s,%d, wp_notpayin->qorbrno =[%.12s]",__FILE__,__LINE__,wp_notpayin->qorbrno );
		vtcp_log("%s,%d, wp_notpayin->qorderno =[%.8s]",__FILE__,__LINE__,wp_notpayin->qorderno );

	}
	else if (memcmp(lv->CMTCode,"303",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "303",		sizeof(wp_notpayin->cmtno));
		/** orderno **/
		/** txnum **/
		GETTAG_SIZE(lv,"58A",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno));
		wp_notpayin->rettype = '0';
		/** respcode **/
		/** opackday **/
		/** opkgno **/
		/** opackid **/
		/** otxnum **/
		/** owtday **/
		/** oorderno **/
		/** qorderno **/
		/** qwtday **/
		/** oorbrno **/
		/** oacbrno **/
		/** txtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"053",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno ,sizeof(wp_notpayin->orbrno ));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		GETTAG_SIZE(lv,"010",wp_notpayin->sendco    ,sizeof(wp_notpayin->sendco    ));
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		GETTAG_SIZE(lv,"0B9",wp_notpayin->receco,sizeof(wp_notpayin->receco));
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"319",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "319",		sizeof(wp_notpayin->cmtno));
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/
		GETTAG_SIZE(lv,"58A",wp_notpayin->acbrno  ,sizeof(wp_notpayin->acbrno));
		GETTAG_SIZE(lv,"BS8",&wp_notpayin->rettype,1);
		/** respcode **/
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,sizeof(wp_notpayin->opackday));
		/** opackday **/
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno));
		/** rem by lhf
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid,sizeof());
		****************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG_SIZE(lv,"0BE",cTmpOrderno,sizeof(cTmpOrderno));
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/* end by lhf 060303 */

		GETTAG_SIZE(lv,"0BH",wp_notpayin->otxnum,sizeof(wp_notpayin->otxnum));
		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,sizeof(wp_notpayin->owtday));
		GETTAG_SIZE(lv,"005",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		/** oorbrno **/
		/** oacbrno **/
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno ,sizeof(wp_notpayin->orbrno ));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		/** sendco **/
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	} 
	else if (memcmp(lv->CMTCode,"320",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "320",		sizeof(wp_notpayin->cmtno)); 
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno)); 
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/ 
		GETTAG_SIZE(lv,"58A",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
		lv_get_tag(lv,"BS8",&wp_notpayin->rettype); 	/** 只一位 **/
		lv_get_tag(lv,"018",&wp_notpayin->respcode); 
		vtcp_log("[%s][%d]============wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
		/** opackday **/ 
		/** opkgno **/ 
		/** opackid **/ 
		/** otxnum **/ 
		GETTAG_SIZE(lv,"CJB",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"0BE",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"58A",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno)); 
		GETTAG_SIZE(lv,"52A",wp_notpayin->oacbrno,sizeof(wp_notpayin->oacbrno));
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		/** sendco **/
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno , sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat= STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"322",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RESP,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "322",		sizeof(wp_notpayin->cmtno));
		/** orderno 报文中没有这个值，所以赋成oorderno **/
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/
		/** acbrno **/
		wp_notpayin->rettype = '0';
		GETTAG_SIZE(lv,"04C",&wp_notpayin->respcode,1);
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno));
		/* rem by lhf 060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid,sizeof());
		****************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno);
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/** otxnum **/
		GETTAG_SIZE(lv,"30A",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"0BD",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"011",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		/*GETTAG_SIZE(lv,"011",wp_notpayin->acbrno,sizeof());    Added by ChenMing 2006.09.25. */		
		/** oacbrno **/
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		/** 
		CIB	原包处理状态
		BS1	轧差节点类型
		BS2	轧差日期
		BS3	轧差场次
		BS5	清算日期
		**/
		/** orbrno 同oorbrno是一行 **/
		GETTAG_SIZE(lv,"011",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		/** payqsactno **/
		/** sendco **/
		GETTAG_SIZE(lv,"011",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"BS5",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"324",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RESP,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "324",		sizeof(wp_notpayin->cmtno));
		/** orderno **/
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/
		/** acbrno **/
		/** rettype **/
		wp_notpayin->rettype = '0';
		GETTAG_SIZE(lv,"BS9",&wp_notpayin->respcode,1);
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno));
		/**by lhf 060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid);
		**************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno);
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/** otxnum **/
		GETTAG_SIZE(lv,"30A",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"0BD",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"011",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		/** oacbrno **/
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		/** 
		CIB	原包处理状态
		BS1	轧差节点类型
		BS2	轧差日期
		BS3	轧差场次
		BS5	清算日期
		**/
		/** orbrno **/
		GETTAG_SIZE(lv,"011",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		/** payqsactno **/
		/** sendco **/
		GETTAG_SIZE(lv,"011",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/*GETTAG_SIZE(lv,"011",wp_notpayin->acbrno);    Added by ChenMing 2006.09.25. */
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"BS5",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"325",sizeof(lv->CMTCode))==0)
	{
		memcpy(wp_notpayin->optype,OPCD_RESP,sizeof(wp_notpayin->optype));
		memcpy(wp_notpayin->cmtno, "325",		sizeof(wp_notpayin->cmtno));
		/** orderno **/
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/
		/** acbrno **/
		/** rettype **/

		wp_notpayin->rettype = '0';
		/** respcode **/
		/** opackday **/
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno));
		vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);
		/** add by lhf060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid);
		*************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno); 
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/** otxnum **/
		GETTAG_SIZE(lv,"30A",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"0BD",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"011",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		/** oacbrno **/
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		/** orbrno **/
		GETTAG_SIZE(lv,"011",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		GETTAG_SIZE(lv,"CCR",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** sendco **/
		/*GETTAG_SIZE(lv,"011",wp_notpayin->acbrno);    Added by ChenMing 2006.09.25. */
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));		
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		memcpy(wp_notpayin->txday,pcLvGetDay(),sizeof(wp_notpayin->txday));
		wp_notpayin->stat = STAT_LZRECV1;
		vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);
	}
	else if (memcmp(lv->CMTCode,"327",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "327",		sizeof(wp_notpayin->cmtno)); 
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno)); 
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end by LiuHuafeng 20060218 */
		/** txnum **/ 
		GETTAG_SIZE(lv,"58A",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
		GETTAG_SIZE(lv,"BS8",&wp_notpayin->rettype,1); 
		/** respcode **/
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8); 
		/* rem by lhf 060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid);
		*************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno); 
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno));
		vtcp_log("[%s][%d]pkgno=[%.3s]\n",__FILE__,__LINE__,wp_notpayin->opkgno);
		GETTAG_SIZE(lv,"0BH",wp_notpayin->otxnum,sizeof(wp_notpayin->otxnum));
		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"005",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end by LiuHuafeng 20060218 */
		/** qorderno **/
		/** qwtday **/
		/** oorbrno **/
		/** oacbrno **/
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		/** sendco **/
		GETTAG_SIZE(lv,"012",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"328",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "328",		sizeof(wp_notpayin->cmtno)); 
		GETTAG_SIZE(lv,"0BD",wp_notpayin->orderno,sizeof(wp_notpayin->orderno)); 
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));
		/* end add by LiuHuafeng 20060218 */
		vtcp_log("[%s][%d] oorderno===[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);
		/** txnum **/ 
		/* add by LiuHuafeng 20060402 应该根据止付类型判断acbrno取cc1还是
		取cc0 
		GETTAG_SIZE(lv,"CC1",wp_notpayin->acbrno); 
		*********************************************************/
		GETTAG_SIZE(lv,"BSA",&wp_notpayin->rettype,1); 
		vtcp_log("[%s][%d] rettype[%c]\n",__FILE__,__LINE__,wp_notpayin->rettype);
		if (wp_notpayin->rettype=='0')
		{
			GETTAG_SIZE(lv,"CC1",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
			GETTAG_SIZE(lv,"CC1",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		}
		else
		{
			GETTAG_SIZE(lv,"CCO",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
			GETTAG_SIZE(lv,"CCO",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno));
		}
		vtcp_log("[%s][%d]acbrno|||acbrno==[%s]\n",__FILE__,__LINE__,wp_notpayin->acbrno);
		/* end by LiuHuafeng 20060402 */
		GETTAG_SIZE(lv,"BSB",&wp_notpayin->respcode,1); 
		/** opackday **/
		/** opkgno **/
		/** opackid **/
		/** otxnum **/
		GETTAG_SIZE(lv,"051",wp_notpayin->owtday,8);
		GETTAG_SIZE(lv,"0BE",wp_notpayin->oorderno,sizeof(wp_notpayin->oorderno));
		/* begin add by LiuHuafeng 20060218 */
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->oorderno, sizeof(wp_notpayin->oorderno), cTmpOrderno);
		memcpy(wp_notpayin->oorderno,cTmpOrderno,sizeof(wp_notpayin->oorderno));
		/* end add by LiuHuafeng 20060218 */
		vtcp_log("[%s][%d] oorderno===[%s]\n",__FILE__,__LINE__,wp_notpayin->oorderno);
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"52A",wp_notpayin->oacbrno,sizeof(wp_notpayin->oacbrno));
		/** otxtype **/
		/** cour **/
		/** otxamt **/
		GETTAG_SIZE(lv,"72A",wp_notpayin->content,sizeof(wp_notpayin->content));
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** 密押 **/
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		/* add by lhf 20060402 如果是整包退回应答去对方清算行做orbrno*/
		if (wp_notpayin->rettype!='0')
			GETTAG_SIZE(lv,"011",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));
		else
		/* end by lhf 20060402 */
			GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno));

		/** sendco **/
		GETTAG_SIZE(lv,"CC0",wp_notpayin->cashqsactno,sizeof(wp_notpayin->cashqsactno));
		/** receco **/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"254",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "254",		sizeof(wp_notpayin->cmtno)); 
		/** orderno **/
		/** txnum **/ 
		GETTAG_SIZE(lv,"CC3",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
		/** rettype **/
		/** respcode **/
		/** opackday **/
		/** opkgno **/
		/** opackid **/
		/** otxnum **/
		/** owtday **/
		/** oorderno **/
		/** qorderno **/
		/** qwtday **/
		/** oorbrno **/
		/** oacbrno **/
		/** otxtype **/
		GETTAG(lv,"32A",Txt);
		memcpy(wp_notpayin->cour,Txt,3);
		memcpy(wp_notpayin->otxamt,Txt+3,sizeof(wp_notpayin->otxamt));
		/** content **/
		/** pascode **/
		/** orbrno **/
		/** payqsactno **/
		/** sendco **/
		/** cashqsactno **/
		/** receco **/
		/** CEK	轧差类型标志
		BS1	轧差节点类型
		BS2	轧差日期
		BS3	轧差场次
		90C	借贷标志
		BS5	清算日期
		CIC	清算处理状态
		**/

		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"920",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "920",		sizeof(wp_notpayin->cmtno)); 
		/** orderno **/
		/** txnum **/ 
		GETTAG_SIZE(lv,"C37",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
		/** rettype **/
		/** respcode **/
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8); 
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno)); 
		/**rem by lhf060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid); 
		*************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno); 
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/** otxnum **/
		/** owtday **/
		/** oorderno **/
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"CCO",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno)); 
		/** oacbrno **/
		/** otxtype **/
		/** ocour **/
		/** otxamt **/
		/** content **/
		GETTAG_SIZE(lv,"C36",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno)); 
		/** orbrno **/
		/** payqsactno **/
		/** sendco **/
		/** cashqsactno **/
		/** receco **/
		/**BS1	轧差节点类型
		BS2	轧差日期
		BS3	轧差场次
		CIB	包处理状态
		C42	报文处理码
		**/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	else if (memcmp(lv->CMTCode,"316",sizeof(lv->CMTCode))==0)
	{ 
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->cmtno, "316",		sizeof(wp_notpayin->cmtno)); 
		/** orderno **/
		/** txnum **/ 
		GETTAG_SIZE(lv,"012",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 
		/** rettype **/
		/** respcode **/
		GETTAG_SIZE(lv,"30I",wp_notpayin->opackday,8); 
		vtcp_log("%s,%d 这里设置02d",__FILE__,__LINE__);
		GETTAG_SIZE(lv,"02D",wp_notpayin->opkgno,sizeof(wp_notpayin->opkgno)); 
		/** rem by lhf 060303
		GETTAG_SIZE(lv,"0BE",wp_notpayin->opackid); 
		***************************************/
		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		GETTAG(lv,"0BE",cTmpOrderno); 
		lv_orderno_add0(cTmpOrderno, sizeof(wp_notpayin->opackid), wp_notpayin->opackid);
		vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_notpayin->opackid);
		/** otxnum **/
		/** owtday **/
		/** oorderno **/
		/** qorderno **/
		/** qwtday **/
		GETTAG_SIZE(lv,"CCO",wp_notpayin->oorbrno,sizeof(wp_notpayin->oorbrno)); 
		/** oacbrno **/
		/** otxtype **/
		/** ocour **/
		/** otxamt **/
		/** content **/
		GETTAG_SIZE(lv,"011",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno)); 
		/** orbrno **/
		/** payqsactno **/
		/** sendco **/
		/** cashqsactno **/
		/** receco **/
		/**CIA	业务回执状态
		**/
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		wp_notpayin->stat = STAT_LZRECV1;
	}
	/** pkg012,pkg013 */
	else if (memcmp(lv->CMTCode,"012",sizeof(lv->CMTCode))==0 || memcmp(lv->CMTCode,"013",sizeof(lv->CMTCode))==0)
	{ 
		char tmpBuf[1024];
		memset(tmpBuf,'\0',sizeof(tmpBuf));
		memcpy(wp_notpayin->optype,OPCD_RECEIVE,sizeof(wp_notpayin->optype)); 
		memcpy(wp_notpayin->pkgno, lv->CMTCode,		sizeof(wp_notpayin->pkgno)); 
		GETTAG_SIZE(lv,"0BC",wp_notpayin->orderno,sizeof(wp_notpayin->orderno)); 

		memset(cTmpOrderno,0,sizeof(cTmpOrderno));
		lv_orderno_add0(wp_notpayin->orderno, sizeof(wp_notpayin->orderno), cTmpOrderno);
		memcpy(wp_notpayin->orderno,cTmpOrderno,sizeof(wp_notpayin->orderno));

		GETTAG_SIZE(lv,"0BG",wp_notpayin->txnum,sizeof(wp_notpayin->txnum)); 
		GETTAG_SIZE(lv,"58A",wp_notpayin->acbrno,sizeof(wp_notpayin->acbrno)); 

		GETTAG_SIZE(lv,"52A",wp_notpayin->orbrno,sizeof(wp_notpayin->orbrno)); 

		GETTAG_SIZE(lv,"30A",wp_notpayin->txday,8);
		memcpy(wp_notpayin->wssrno,lvca.wssrno, sizeof(wp_notpayin->wssrno));
		wp_notpayin->stat = STAT_LZRECV1;
		/**NEWTCTD:add by wangyongwei 20070817 取实时业务的附加域,并插入lv_addit表***/
		struct lv_addit_c ssxx_addit;
		memset(&ssxx_addit,0x00,sizeof(ssxx_addit));
		int ret=0;
		char cTaglen[9];
		memset(cTaglen,0,sizeof(cTaglen));
		char buffer[1024];
		memset(buffer,0x00,sizeof(buffer));
		GETTAG_SIZE(lv,"B40",cTaglen,sizeof(cTaglen)-1);
		ssxx_addit.taglen=atoi(cTaglen);	
		ssxx_addit.tx_date=apatoi( pcLvGetDay(),8);
		vtcp_log("%s,%d, iFIGHT!ssxx_addit.taglen==[%d]\n",__FILE__,__LINE__,ssxx_addit.taglen);
		vtcp_log("%s,%d, iFIGHT!buffer==[%d]\n",__FILE__,__LINE__,sizeof(buffer));
		if( sizeof(buffer) < ssxx_addit.taglen )
		{
				ssxx_addit.taglen=sizeof(buffer);
		}else{ /****此处为石总所加 20130329****/
			GETTAG_SIZE(lv,"72C",buffer,ssxx_addit.taglen);	
			vtcp_log("%s,%d, iFIGHT!\n",__FILE__,__LINE__);
			ret=iLvSetAddit_Notpay(wp_notpayin,wp_notpayin->txday,"000",buffer,ssxx_addit.taglen);
			if(ret){
				vtcp_log("%s,%d, 添加实时信息附加域失败![%d]\n",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D107");
				return(-1);
			}
		}/****此处为石总所加 20130329****/
		vtcp_log("%s,%d,通用信息业务，附加语长度=%d\n",__FILE__,__LINE__,ssxx_addit.taglen);
		/**add by wangyongwei 20070817 取实时业务的附加域,并插入lv_addit表***/
	}
	else /*Added by ChenMing 2006.09.26.*/ 
	{
		vtcp_log("[%s][%d]这里出错了，CMTNO=[%s] 在这里不处理! ",__FILE__,__LINE__,lv->CMTCode);	
	}
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);
	vtcp_log("[%s][%d]========p_notpayin->cmtno=[%s]\n",__FILE__,__LINE__,wp_notpayin->cmtno);
	vtcp_log("[%s][%d]============wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
	return(0);
}
/****Add by SSH,2005.11.28,增加插入发送和接收登记簿的函数****/
int lv_insSendList(TLVPACKAGE *lv)
{
	char	tmp[19];
	struct	lv_sendlist_c	wd_lv_sendlist;

	memset(tmp, '\0', sizeof(tmp));
	memset(&wd_lv_sendlist, '\0', sizeof(wd_lv_sendlist));
	wd_lv_sendlist.tx_date=apatoi( lv->workDate, 8);
	get_fd_data("0030", wd_lv_sendlist.br_no);
	get_fd_data("0060", wd_lv_sendlist.tel);
	get_zd_long("0040", &wd_lv_sendlist.trace_no);
	/*memcpy(wd_lv_sendlist.br_no, TITA.kinbr, sizeof(wd_lv_sendlist.br_no)-1);
	memcpy(wd_lv_sendlist.tel, TITA.tlrno, sizeof(wd_lv_sendlist.tel)-1);
	memcpy(wd_lv_sendlist.trace_no,get_format_seq( 6,dg_get_wssrno()),sizeof(wd_lv_sendlist.trace_no)-1);*/
	memcpy(wd_lv_sendlist.refno, lv->reNo, sizeof(wd_lv_sendlist.refno)-1);
	memcpy(wd_lv_sendlist.flagno, lv->flgNo, sizeof(wd_lv_sendlist.flagno)-1);
	memcpy(wd_lv_sendlist.packno, lv->CMTCode, sizeof(wd_lv_sendlist.packno)-1);

	vtcp_log("[%s][%d]----------lv->pkgType=[%d]\n",__FILE__,__LINE__,lv->pkgType);
	if (lv->pkgType==1)	/** 是pkg包 **/
	{
		vtcp_log("[%s][%d]--------\n",__FILE__,__LINE__);
		memcpy(wd_lv_sendlist.packtype, "PKG", sizeof(wd_lv_sendlist.packtype)-1);
		GETHEADTAG(lv,"0BD",wd_lv_sendlist.orderno);
		/** pkg报取清算行行号 **/
		GETHEADTAG(lv,"011",wd_lv_sendlist.pay_qs_no);
		GETHEADTAG(lv,"012",wd_lv_sendlist.cash_qs_no);
		if (memcmp(wd_lv_sendlist.packno,PKGNO_TYXX,3)!=0 && memcmp(wd_lv_sendlist.packno,PKGNO_SSXX,3)!=0)
		{
			GETHEADTAG(lv,"32B",tmp);
			str2dbl(tmp+3,15,0,&wd_lv_sendlist.tx_amt);
			wd_lv_sendlist.tx_amt=wd_lv_sendlist.tx_amt/100.0;
		}
	}
	else if (lv->pkgType==0)
	{
		memcpy(wd_lv_sendlist.packtype, "CMT", sizeof(wd_lv_sendlist.packtype)-1);
		/** orderno **/
		if (memcmp(lv->CMTCode,"301",3)==0)
		{
			GETTAG_SIZE(lv,"056",wd_lv_sendlist.orderno,sizeof(wd_lv_sendlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG_SIZE(lv,"043",wd_lv_sendlist.orderno,sizeof(wd_lv_sendlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"316",3)==0 || memcmp(lv->CMTCode,"920",3)==0)
		{
			GETTAG_SIZE(lv,"0BE",wd_lv_sendlist.orderno,sizeof(wd_lv_sendlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"319",3)==0 || memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"323",3)==0
			|| memcmp(lv->CMTCode,"321",3)==0 || memcmp(lv->CMTCode,"327",3)==0 || memcmp(lv->CMTCode,"328",3)==0
			|| memcmp(lv->CMTCode,"853",3)==0 )
		{
			GETTAG_SIZE(lv,"0BD",wd_lv_sendlist.orderno,sizeof(wd_lv_sendlist.orderno)-1);
		}
		/** orbrno  acbrno **/
		if (memcmp(lv->CMTCode,"301",3)==0)
		{
			GETTAG_SIZE(lv,"01C",wd_lv_sendlist.or_br_no,sizeof(wd_lv_sendlist.or_br_no)-1);
			GETTAG_SIZE(lv,"01D",wd_lv_sendlist.ac_br_no,sizeof(wd_lv_sendlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG_SIZE(lv,"01D",wd_lv_sendlist.or_br_no,sizeof(wd_lv_sendlist.or_br_no)-1);
			GETTAG_SIZE(lv,"01C",wd_lv_sendlist.ac_br_no,sizeof(wd_lv_sendlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"853",3)==0 || memcmp(lv->CMTCode,"542",3)==0)
		{
			GETTAG_SIZE(lv,"C38",wd_lv_sendlist.or_br_no,sizeof(wd_lv_sendlist.or_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"303",3)==0 || memcmp(lv->CMTCode,"319",3)==0 || 
			memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"327",3)==0 )
		{
			GETTAG_SIZE(lv,"52A",wd_lv_sendlist.or_br_no,sizeof(wd_lv_sendlist.or_br_no)-1);
			GETTAG_SIZE(lv,"58A",wd_lv_sendlist.ac_br_no,sizeof(wd_lv_sendlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"328",3)==0)
		{
			GETTAG_SIZE(lv,"52A",wd_lv_sendlist.or_br_no,sizeof(wd_lv_sendlist.or_br_no)-1);
			GETTAG_SIZE(lv,"CC1",wd_lv_sendlist.ac_br_no,sizeof(wd_lv_sendlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"920",3)==0)
		{
			GETTAG_SIZE(lv,"C36",wd_lv_sendlist.or_br_no  ,sizeof(wd_lv_sendlist.or_br_no  )-1);
			GETTAG_SIZE(lv,"C37",wd_lv_sendlist.ac_br_no  ,sizeof(wd_lv_sendlist.ac_br_no  )-1);
			GETTAG_SIZE(lv,"CCO",wd_lv_sendlist.cash_qs_no,sizeof(wd_lv_sendlist.cash_qs_no)-1);
		}
		/** payqsactno cashqsactno **/
		if (memcmp(lv->CMTCode,"301",3)==0 || memcmp(lv->CMTCode,"302",3)==0 || memcmp(lv->CMTCode,"303",3)==0 ||
			memcmp(lv->CMTCode,"319",3)==0 || memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"327",3)==0 || 
			memcmp(lv->CMTCode,"316",3)==0 )
		{
			GETTAG_SIZE(lv,"011",wd_lv_sendlist.pay_qs_no ,sizeof(wd_lv_sendlist.pay_qs_no )-1);
			GETTAG_SIZE(lv,"012",wd_lv_sendlist.cash_qs_no,sizeof(wd_lv_sendlist.cash_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"328",3)==0 )
		{
			GETTAG_SIZE(lv,"011",wd_lv_sendlist.pay_qs_no,sizeof(wd_lv_sendlist.pay_qs_no)-1);
			GETTAG_SIZE(lv,"CCO",wd_lv_sendlist.cash_qs_no,sizeof(wd_lv_sendlist.cash_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"321",3)==0 || memcmp(lv->CMTCode,"323",3)==0)
		{
			GETTAG_SIZE(lv,"011",wd_lv_sendlist.pay_qs_no,sizeof(wd_lv_sendlist.pay_qs_no)-1);
		}
		/** txamt **/
		if (memcmp(lv->CMTCode,"301",3)==0 || memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG(lv,"CND",tmp);
			str2dbl(tmp+3,15,0,&wd_lv_sendlist.tx_amt);
			wd_lv_sendlist.tx_amt=wd_lv_sendlist.tx_amt / 100.0;
		}
	}
	if ((lvca.rtcode = Lv_sendlist_Ins(wd_lv_sendlist, g_pub_tx.reply))!=DB_OK)
	{
		vtcp_log("%s,%d,Insert sendlist error[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		Lv_sendlist_Debug(&wd_lv_sendlist);
		sprintf( acErrMsg,"插入小额报文接收登记簿失败");WRITEMSG
		return(-1);
	}
	return(0);
}
int lv_insRecvList(TLVPACKAGE *lv)
{
	char	tmp[19];
	struct	lv_recvlist_c	wd_lv_recvlist;
	memset(tmp, '\0', sizeof(tmp));
	memset(&wd_lv_recvlist, '\0', sizeof(wd_lv_recvlist));

	vtcp_log("[%s][%d]=====For Test !!! ", __FILE__,__LINE__);

	wd_lv_recvlist.tx_date = apatoi( lv->workDate, 8);
	get_zd_data("0030", wd_lv_recvlist.br_no);
	get_zd_data("0070", wd_lv_recvlist.tel);
	get_zd_long("0040", &wd_lv_recvlist.trace_no);

	vtcp_log(" wd_lv_recvlist.br_no===[%s] ",wd_lv_recvlist.br_no);   
	vtcp_log(" wd_lv_recvlist.tel===[%s] ",wd_lv_recvlist.tel);   
	vtcp_log(" wd_lv_recvlist.trace_no===[%ld] ",wd_lv_recvlist.trace_no);   	

	/*memcpy(wd_lv_recvlist.br_no, TITA.kinbr, sizeof(wd_lv_recvlist.br_no)-1);
	memcpy(wd_lv_recvlist.tel, TITA.tlrno, sizeof(wd_lv_recvlist.tel)-1);
	memcpy(wd_lv_recvlist.trace_no,get_format_seq( 6,dg_get_wssrno()),sizeof(wd_lv_recvlist.trace_no)-1);*/
	memcpy(wd_lv_recvlist.refno, lv->reNo, sizeof(wd_lv_recvlist.refno)-1);
	memcpy(wd_lv_recvlist.flagno, lv->flgNo, sizeof(wd_lv_recvlist.flagno)-1);
	memcpy(wd_lv_recvlist.packno, lv->CMTCode, sizeof(wd_lv_recvlist.packno)-1);

	vtcp_log("[%s][%d]----------lv->pkgType=[%d]\n",__FILE__,__LINE__,lv->pkgType);
	if (lv->pkgType==1)	/** 是pkg包 **/
	{
		memcpy(wd_lv_recvlist.packtype, "PKG", sizeof(wd_lv_recvlist.packtype)-1);
		GETHEADTAG(lv,"0BD",wd_lv_recvlist.orderno);
		GETHEADTAG(lv,"011",wd_lv_recvlist.pay_qs_no);
		GETHEADTAG(lv,"012",wd_lv_recvlist.cash_qs_no);
		if (memcmp(wd_lv_recvlist.packno,PKGNO_TYXX,3)!=0 && memcmp(wd_lv_recvlist.packno,PKGNO_SSXX,3)!=0)
		{
			GETHEADTAG(lv,"32B",tmp);
			str2dbl(tmp+3,15,0,&wd_lv_recvlist.tx_amt);
			wd_lv_recvlist.tx_amt= wd_lv_recvlist.tx_amt /100.0;
		}
	}
	else if (lv->pkgType==0)
	{
		memcpy(wd_lv_recvlist.packtype, "CMT", sizeof(wd_lv_recvlist.packtype)-1);
		/** orderno **/
		if (memcmp(lv->CMTCode,"301",3)==0)
		{
			GETTAG_SIZE(lv,"056",wd_lv_recvlist.orderno,sizeof(wd_lv_recvlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG_SIZE(lv,"043",wd_lv_recvlist.orderno,sizeof(wd_lv_recvlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"316",3)==0 || memcmp(lv->CMTCode,"920",3)==0)
		{
			GETTAG_SIZE(lv,"0BE",wd_lv_recvlist.orderno,sizeof(wd_lv_recvlist.orderno)-1);
		}
		else if (memcmp(lv->CMTCode,"319",3)==0 || memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"323",3)==0
			|| memcmp(lv->CMTCode,"321",3)==0 || memcmp(lv->CMTCode,"327",3)==0 || memcmp(lv->CMTCode,"328",3)==0
			|| memcmp(lv->CMTCode,"854",3)==0 || memcmp(lv->CMTCode,"322",3)==0 || memcmp(lv->CMTCode,"324",3)==0
			|| memcmp(lv->CMTCode,"325",3)==0)
		{
			GETTAG_SIZE(lv,"0BD",wd_lv_recvlist.orderno,sizeof(wd_lv_recvlist.orderno)-1);
		}
		/** orbrno  acbrno **/
		if (memcmp(lv->CMTCode,"301",3)==0)
		{
			GETTAG_SIZE(lv,"01C",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"01D",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG_SIZE(lv,"01D",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"01C",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"854",3)==0 )
		{
			GETTAG_SIZE(lv,"C38",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"541",3)==0 )
		{
			GETTAG_SIZE(lv,"CC3",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"C38",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"543",3)==0 || memcmp(lv->CMTCode,"552",3)==0 || memcmp(lv->CMTCode,"554",3)==0)
		{
			GETTAG_SIZE(lv,"C38",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"303",3)==0 || memcmp(lv->CMTCode,"319",3)==0 || 
			memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"327",3)==0 )
		{
			GETTAG_SIZE(lv,"52A",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"58A",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"328",3)==0)
		{
			GETTAG_SIZE(lv,"52A",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"CC1",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
		}
		else if (memcmp(lv->CMTCode,"920",3)==0)
		{
			GETTAG_SIZE(lv,"C36",wd_lv_recvlist.or_br_no,sizeof(wd_lv_recvlist.or_br_no)-1);
			GETTAG_SIZE(lv,"C37",wd_lv_recvlist.ac_br_no,sizeof(wd_lv_recvlist.ac_br_no)-1);
			GETTAG_SIZE(lv,"CCO",wd_lv_recvlist.cash_qs_no,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		}
		/** payqsactno cashqsactno **/
		if (memcmp(lv->CMTCode,"301",3)==0 || memcmp(lv->CMTCode,"302",3)==0 || memcmp(lv->CMTCode,"303",3)==0 ||
			memcmp(lv->CMTCode,"319",3)==0 || memcmp(lv->CMTCode,"320",3)==0 || memcmp(lv->CMTCode,"327",3)==0 ||
			memcmp(lv->CMTCode,"316",3)==0)
		{
			GETTAG_SIZE(lv,"011",wd_lv_recvlist.pay_qs_no,sizeof(wd_lv_recvlist.pay_qs_no)-1);
			GETTAG_SIZE(lv,"012",wd_lv_recvlist.cash_qs_no,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"328",3)==0 )
		{
			GETTAG_SIZE(lv,"011",wd_lv_recvlist.pay_qs_no,sizeof(wd_lv_recvlist.pay_qs_no)-1);
			GETTAG_SIZE(lv,"CCO",wd_lv_recvlist.cash_qs_no,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"321",3)==0 || memcmp(lv->CMTCode,"323",3)==0)
		{
			GETTAG_SIZE(lv,"011",wd_lv_recvlist.pay_qs_no,sizeof(wd_lv_recvlist.pay_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"322",3)==0 || memcmp(lv->CMTCode,"324",3)==0 || memcmp(lv->CMTCode,"325",3)==0)
		{
			GETTAG_SIZE(lv,"011",wd_lv_recvlist.cash_qs_no,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		}
		else if (memcmp(lv->CMTCode,"254",3)==0 )
		{
			GETTAG_SIZE(lv,"CC3",wd_lv_recvlist.cash_qs_no,sizeof(wd_lv_recvlist.cash_qs_no)-1);
		}
		/** txamt **/
		if (memcmp(lv->CMTCode,"301",3)==0 || memcmp(lv->CMTCode,"302",3)==0)
		{
			GETTAG(lv,"CND",tmp);
			str2dbl(tmp+3,15,0,&wd_lv_recvlist.tx_amt);
			wd_lv_recvlist.tx_amt= wd_lv_recvlist.tx_amt /100.0;
		}
		/** respflag **/
		/** respno **/
		/** respdesc **/
		/** filler **/
	}
	vtcp_log(" wd_lv_recvlist.tx_date===[%d] ",wd_lv_recvlist.tx_date);   
	vtcp_log(" wd_lv_recvlist.or_br_no===[%s] ",wd_lv_recvlist.or_br_no);   
	vtcp_log(" wd_lv_recvlist.refno===[%s] ",wd_lv_recvlist.refno);  
	vtcp_log(" wd_lv_recvlist.flagno===[%s] ",wd_lv_recvlist.flagno);  

	vtcp_log(" wd_lv_recvlist.br_no===[%s] ",wd_lv_recvlist.br_no);   
	vtcp_log(" wd_lv_recvlist.tel===[%s] ",wd_lv_recvlist.tel);  
	vtcp_log(" wd_lv_recvlist.respflag===[%s] ",wd_lv_recvlist.respflag);  	

	Lv_recvlist_Debug(&wd_lv_recvlist);

	lvca.rtcode = Lv_recvlist_Ins(wd_lv_recvlist, g_pub_tx.reply);

	if ( lvca.rtcode != 0 )
	{
		vtcp_log("%s,%d,Insert recvlist error[%d]\n",__FILE__,__LINE__,lvca.rtcode);
		sprintf( acErrMsg,"插入小额报文接收登记簿失败");WRITEMSG
		return(-1);
	}
	return(0);
}                   
