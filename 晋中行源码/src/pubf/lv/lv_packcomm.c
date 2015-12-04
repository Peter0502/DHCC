/*************************************************************************/
/* �ļ�: lv_packcomm.pc                                                  */
/* ����: �йر��Ĵ���Ĺ�������                                          */
/* ����: ʯ����                                                          */
/* ����: 2005.8.18                                                       */
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
/*****��ע��:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!             *****
******��ʼ��ָ��ʱ��Ҫ�����ÿ�,��memset(&wp_payin...... һ��Ҫ����
*****                                                            *****
*****                                                    ʯ����  *****
*****                                                  2005.12.3 *****
*********************************************************************/
/****Add by SSH,2005.12.1��ʱ����,��get_tag�������ٺ�0ʱ���Բ���****/
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

/****ί���տ����������****/
typedef struct
{
	char pdate[8];
	char pnum[8];
	char ptype[2];
}WT_ADD_AREA;

/****���ճи�������������****/
typedef struct
{
	char pdate[8];
	char pnum[8];
	char ptxamt[15];
	char jtxamt[15];
}TSCF_ADD_AREA;

/*************************************************************************/
/* ������:lv_setwbctl_from_pack                                          */
/* ����:  ���ݱ������������������Ʊ�                                     */
/*************************************************************************/
int lv_setwbctl_from_pack(TLVPACKAGE *wp_lvpack,struct lv_wbctl_c *wp_lv_wbctl)
{
	char caValue[61];
	memset(wp_lv_wbctl,'\0',sizeof(struct lv_wbctl_c));	
	iLvGETHEADTAG_DATE(wp_lvpack,"30E",  &wp_lv_wbctl->pack_date);/**��ί������**/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BD",caValue);/**�����**/
	lv_orderno_add0(caValue, sizeof(wp_lv_wbctl->packid)-1, wp_lv_wbctl->packid);
	/** rem by lhf 060303
	memcpy(wp_lv_wbctl->packid,caValue,sizeof(wp_lv_wbctl->packid)-1);
	***********************************************************/
	GETHEADTAG(wp_lvpack,"02C",wp_lv_wbctl->pkgno);/**�����ͺ�**/
	GETHEADTAG(wp_lvpack,"011",wp_lv_wbctl->pay_qs_no);/*����������*/
	GETHEADTAG(wp_lvpack,"012",wp_lv_wbctl->cash_qs_no);/*����������*/
	/****����ϵͳ����****/
	wp_lv_wbctl->in_date = apatoi(pcLvGetDay(), 8);
	/***ҵ���ܱ���****/ 
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B63",caValue);
	wp_lv_wbctl->dtlcnt=atoi(caValue);
	/***ҵ���ܽ��****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32B",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_wbctl->totamt);
	wp_lv_wbctl->totamt= wp_lv_wbctl->totamt/100.0;
	/****����ڵ�����****/
	GETHEADTAG(wp_lvpack,"BS1",wp_lv_wbctl->zcnode);
	/****��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS2",&wp_lv_wbctl->zc_date);
	/****�����****/
	GETHEADTAG(wp_lvpack,"BS3",wp_lv_wbctl->zcid);
	/****��ִ����****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS6",&wp_lv_wbctl->hz_date);
	/****ԭ��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"30I",&wp_lv_wbctl->o_pack_date);
	/***ԭ����id****/
	memset(caValue,0 , sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BE",caValue);
	lv_orderno_add0(caValue, sizeof(wp_lv_wbctl->o_packid)-1, wp_lv_wbctl->o_packid);
	vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_lv_wbctl->o_packid);
	/** rem by lhf 060303
	GETHEADTAG(wp_lvpack,"0BE",wp_lv_wbctl->o_packid);
	memcpy(wp_lv_wbctl->packid,caValue,sizeof(wp_lv_wbctl->packid)-1);
	****************************************************/
	/***��ϸҵ��ɹ�����****/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B41",caValue);
	wp_lv_wbctl->dtlsuccnt=atoi(caValue);
	/***��ϸҵ��ɹ��ܽ��****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32C",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_wbctl->dtlsucamt);
	wp_lv_wbctl->dtlsucamt = wp_lv_wbctl->dtlsucamt/100.0;
	/****ԭ�����ͺ�*****/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	GETHEADTAG(wp_lvpack,"02D",wp_lv_wbctl->o_pkgno);
	/****����ڵ��,ͨ����Ϣ��****/
	GETHEADTAG(wp_lvpack,"C36",wp_lv_wbctl->ornode);
	/****���սڵ��,ͨ����Ϣ��****/
	GETHEADTAG(wp_lvpack,"C37",wp_lv_wbctl->acnode);
	/****Ͻ�ڷַ���־****/
	GETHEADTAG(wp_lvpack,"BS7",wp_lv_wbctl->ffflag);
	/****ҵ�����ͺ�***/
	GETHEADTAG(wp_lvpack,"0BG",wp_lv_wbctl->txnum);
	/****������־****/
	GETHEADTAG(wp_lvpack,"BS4",wp_lv_wbctl->bfflag);
	/****��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS5",&wp_lv_wbctl->qs_date);
	/****������״̬****/
	GETHEADTAG(wp_lvpack,"CIB",wp_lv_wbctl->packstat);
	/****��״̬,�Զ���*****/
	if (memcmp(wp_lv_wbctl->packstat,PACKSTAT_SEND,2)==0 )
	{
		wp_lv_wbctl->stat[0]=PSTAT_SEND;
	}else
	{
		wp_lv_wbctl->stat[0]=PSTAT_LRESP;
	}
	/****��ӡ��־,����****/
	memset(wp_lv_wbctl->brprtflag,'0',sizeof(wp_lv_wbctl->brprtflag)-1);
	memset(wp_lv_wbctl->qsprtflag,'0',sizeof(wp_lv_wbctl->qsprtflag)-1);
	get_fd_data("0030", wp_lv_wbctl->br_no);
	/*memcpy(wp_lv_wbctl->br_no,TITA.kinbr,sizeof(wp_lv_wbctl->br_no)-1);*/
	return(0);
}
/*************************************************************************/
/* ������:lv_setlbctl_from_pack                                          */
/* ����:  ���ݱ������������������Ʊ�                                     */
/*************************************************************************/
int lv_setlbctl_from_pack(TLVPACKAGE *wp_lvpack,struct lv_lbctl_c *wp_lv_lbctl)
{
	char caValue[61];
	memset(caValue,0 , sizeof(caValue));
	memset(wp_lv_lbctl,'\0',sizeof(struct lv_lbctl_c));	

	iLvGETHEADTAG_DATE(wp_lvpack,"30E",&wp_lv_lbctl->pack_date);/**��ί������**/

	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BD",caValue);/**�����**/
	lv_orderno_add0(caValue, sizeof(wp_lv_lbctl->packid)-1, wp_lv_lbctl->packid);
	/** rem by lhf 060303
	memcpy(wp_lv_lbctl->packid,caValue,sizeof(wp_lv_lbctl->packid)-1);
	***********************************************/
	GETHEADTAG(wp_lvpack,"02C",wp_lv_lbctl->pkgno);/**�����ͺ�**/
	GETHEADTAG(wp_lvpack,"011",wp_lv_lbctl->pay_qs_no);/*����������*/
	GETHEADTAG(wp_lvpack,"012",wp_lv_lbctl->cash_qs_no);/*����������*/
	/****����ϵͳ����****/
	lv_ldate(&(wp_lv_lbctl->in_date),pcLvGetDay(),8);
	/***ҵ���ܱ���****/ 
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B63",caValue);
	wp_lv_lbctl->dtlcnt=atoi(caValue);
	/***ҵ���ܽ��****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32B",caValue);
	vtcp_log("[%s][%d]=======czValue=[%s]\n",__FILE__,__LINE__,caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_lbctl->totamt);
	wp_lv_lbctl->totamt=wp_lv_lbctl->totamt/100.0;
	vtcp_log("[%s][%d]=======wp_lv_lbctl->totamt=[%f]\n",__FILE__,__LINE__,wp_lv_lbctl->totamt);
	/****����ڵ�����****/
	GETHEADTAG(wp_lvpack,"BS1",wp_lv_lbctl->zcnode);
	/****��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS2",&wp_lv_lbctl->zc_date);
	/****�����****/
	GETHEADTAG(wp_lvpack,"BS3",wp_lv_lbctl->zcid);
	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	/****��ִ����****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS6",&wp_lv_lbctl->hz_date);
	/****ԭ��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"30I",&wp_lv_lbctl->o_pack_date);
	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	/***ԭ����id****/
	/** by lhf 20060303
	GETHEADTAG(wp_lvpack,"0BE",wp_lv_lbctl->o_packid);
	* begin add by lhf ***/
	memset(caValue,0 , sizeof(caValue));
	GETHEADTAG(wp_lvpack,"0BE",caValue);
	lv_orderno_add0(caValue, sizeof(wp_lv_lbctl->o_packid)-1, wp_lv_lbctl->o_packid);
	vtcp_log("[%s][%d] opackid==[%s] \n",__FILE__,__LINE__,wp_lv_lbctl->o_packid);
	/***��ϸҵ��ɹ�����****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"B41",caValue);
	wp_lv_lbctl->dtlsuccnt=atoi(caValue);
	/***��ϸҵ��ɹ��ܽ��****/
	memset(caValue,'\0',sizeof(caValue));
	GETHEADTAG(wp_lvpack,"32C",caValue);
	str2dbl(caValue+3,strlen(caValue)-3,0,&wp_lv_lbctl->dtlsucamt);
	wp_lv_lbctl->dtlsucamt=wp_lv_lbctl->dtlsucamt/100.0;
	/****ԭ�����ͺ�*****/
	vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
	GETHEADTAG(wp_lvpack,"02D",wp_lv_lbctl->o_pkgno);
	/****����ڵ��,ͨ����Ϣ��****/
	GETHEADTAG(wp_lvpack,"C36",wp_lv_lbctl->ornode);
	/****���սڵ��,ͨ����Ϣ��****/
	GETHEADTAG(wp_lvpack,"C37",wp_lv_lbctl->acnode);
	/****Ͻ�ڷַ���־****/
	GETHEADTAG(wp_lvpack,"BS7",wp_lv_lbctl->ffflag);
	/****ҵ�����ͺ�***/
	GETHEADTAG(wp_lvpack,"0BG",wp_lv_lbctl->txnum);
	/****������־****/
	GETHEADTAG(wp_lvpack,"BS4",wp_lv_lbctl->bfflag);
	/****��������****/
	iLvGETHEADTAG_DATE(wp_lvpack,"BS5",&wp_lv_lbctl->qs_date);
	/****������״̬****/
	GETHEADTAG(wp_lvpack,"CIB",wp_lv_lbctl->packstat);
	/****��״̬,�Զ���*****/
	wp_lv_lbctl->sts[0]=PSTAT_RECEIVE;
	/****��ӡ��־,����****/
	memset(wp_lv_lbctl->brprtflag,'0',sizeof(wp_lv_lbctl->brprtflag)-1);
	memset(wp_lv_lbctl->qsprtflag,'0',sizeof(wp_lv_lbctl->qsprtflag)-1);
	get_fd_data("0030", wp_lv_lbctl->br_no);

	vtcp_log("[%s][%d] hz_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->hz_date);
	vtcp_log("[%s][%d] o_pack_date==[%d] \n",__FILE__,__LINE__,wp_lv_lbctl->o_pack_date);		

	return(0);
}
/*************************************************************************/
/*   ���ڴӱ��ĵ�payin��ת��,��ͼֱ�Ӵ�����������fetch������дpayin,�� */
/*���ڶ��ڱ���,ʵ�������Ƚ��鷳�����Һ����������ڴ�й©,�����ݶ�:���ȴ���*/
/*����������ȡ�ӱ�����,Ȼ��ͨ��һ�������õ��ӱ��ĵļ�¼����,���ѭ������ */
/*��������ȡ����,��дpayin                                              */
/*                               ----Add by SSH,2005.8.18                */
/*************************************************************************/

/*************************************************************************/
/* ������:lv_count_recnt                                                 */
/* ����:  �����ӱ����м�¼�ĸ���                                         */
/* ����ֵ:<=0��ʾʧ��                                                    */
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
		/***�Ƕ���ҵ��ֻ��1��****/
		return(1);
	}
}
/*************************************************************************/
/* ������:lv_fetchpack_to_payin                                         */
/* ����:  ���ӱ�����ȡ�������ͨ�����ݽ�����                             */
/*************************************************************************/
int lv_fetchpack_to_payin(TLVPACKAGE *wp_parent,TLVPACKAGE *wp_lvpack, int id, PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	char caValue[61];
	int nTxAddLen=0;
	/****HSYX BEGIN :�Ŵ󸽼��򻺳���****/
	char cTxAddText[1024*800];/***�����ֶλ�����****/
	char    cAcbrno[13];/** ��֪Ϊ�Σ������м������F_acbrno����û��ֵ **/
	/****HSYX END :�Ŵ󸽼��򻺳���****/
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
	/****HSYX BEGIN :�Ŵ󸽼��򻺳���****/
	memset(cAcbrno,'\0',sizeof(cAcbrno));
	/****HSYX END :�Ŵ󸽼��򻺳���****/

	vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);
	vtcp_log("========  pcLvGetDay()=[%s]   ========",pcLvGetDay()); 
	memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));	 	
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]  wp_lvpack->CMTCode=[%s]",__FILE__,__LINE__,wp_payin->A_tbsdy,wp_lvpack->CMTCode);  
	vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
	/****��ִҵ�񣬴�ԭ���ʱ���ȡ����****/
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
		/** modified by dongxy 20070313 Ʊ��Ҫ�ؼ���û��cc1,��Ӧ�ֶ���58A  begin **/
		GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		if (wp_payin->F_orbrno[0]=='\0')
		{
			vtcp_log("[%s][%d]===========��Ʊ��\n",__FILE__,__LINE__);
			GETTAG_SIZE(wp_lvpack,"58A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		}
		vtcp_log("[%s][%d]===========wp_payin->F_orbrno=[%s]\n",__FILE__,__LINE__,wp_payin->F_orbrno);
		/** modified by dongxy 20070313 Ʊ��Ҫ�ؼ���û��cc1,��Ӧ�ֶ���58A  end **/
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"005",caValue,sizeof(caValue)-1);
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		
		GETTAG_SIZE(wp_lvpack,"0BH",wp_payin->F_txnum,sizeof(wp_payin->F_txnum));/**YHBP**/
		vtcp_log("[%s][%d]====xyy=======wp_payin->F_txnum=[%s]\n",__FILE__,__LINE__,wp_payin->F_txnum);
		wp_payin->F_lw_ind[0] = '1';		

		vtcp_log("%s,%d,F_wtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_wtday); 
		vtcp_log("%s,%d,F_orderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_orderno); 		
		vtcp_log("%s,%d,F_orbrno=[%.12s]",__FILE__,__LINE__,wp_payin->F_orbrno); 		
		/** �ҵ�ԭС������ **/
		iRc=iLvFetchRecToPayin(wp_payin);  
		vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
		/****HSYX BEGIN:֧Ʊ��ִδ�ҵ�ԭ�����򲹳���****/
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
				GETTAG(wp_lvpack,"52A",cAcbrno);/**ע��˴�������**/
				vtcp_log("[%s][%d]========cAcbrno=[%s]\n", __FILE__,__LINE__,cAcbrno);
				GETTAG(wp_lvpack,"58A",wd_bfpayin.F_orbrno);/**ע��˴�������**/
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
				/***����������������һ���µļ�¼****/
				iRc=iLvNewRecFromPayin(&wd_bfpayin);
				if (iRc)
				{
					vtcp_log("%s,%d,iLvNewRecFromPayin����[%d]\n",__FILE__,__LINE__,lvca.rtcode);
					sprintf( acErrMsg,"����������������һ���µļ�¼����");
					WRITEMSG
					return(-1);
				}
				printf("[%s][%d]\n", __FILE__,__LINE__);
				/***�����¼�¼****/
				wd_bfpayin.F_lw_ind[0]='1';/* add by LiuHuafeng 2007-4-4 7:31 */
				wd_bfpayin.T_lw_ind[0]='1';/* add by LiuHuafeng 2007-4-4 7:31 */
				iRc=iLvUpdRecFromPayin(&wd_bfpayin,1);
				if (iRc)
				{
					vtcp_log("%s,%d,�����¼�¼����\n",__FILE__,__LINE__,lvca.rtcode);
					sprintf( acErrMsg,"�����¼�¼����");
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
		/****HSYX END:֧Ʊ��ִδ�ҵ�ԭ�����򲹳���****/
		if (iRc)
		{
			vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
			sprintf( acErrMsg,"ȡ�ñ������ݴ���");WRITEMSG
			return(iRc);
		}
		/* ======= ����Ӧ������ T_wtday T_orderno T_orbrno���ֶ�  ========= 
		======= Ҫ��Ȼ�������ԭ��¼���ظ�                     =========   */
		/* begin add by LiuHuafeng 2007-8-22 10:10:27 
		 ����ʵʱ����ҵ�������ִ��Ӧ����ʧ�ܵ������Ҫ��ԭ�����ʵ��ո�˫��������
		 ����������Ҫ���´���һ�� begin NEWTCTD1
		****************************************************************************/
		vtcp_log("%s,%d,wp_payin->F_pkgno=[%.8s]  wp_lvpack->CMTCode=[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno,wp_lvpack->CMTCode);  
		vtcp_log("%s,%d iRc==%d,F_pkgno=%.3s",__FILE__,__LINE__,iRc,wp_payin->F_pkgno);
		GETTAG(wp_lvpack,"CIA",wp_payin->F_rcpstat);
		if(iRc!=100 && memcmp(wp_lvpack->CMTCode,PKGNO_SSDJHZ,sizeof(wp_lvpack->CMTCode))==0 )
		{
			PAY_IN_AREA wd_o_payin;
			memset(&wd_o_payin, 0 , sizeof(wd_o_payin));
			vtcp_log("%s,%d ʵʱ���ǻ�ִ���ʸ���ԭ��������Ϣ��Ȼ��Ի��ո���Ϣ ",__FILE__,__LINE__);
			memcpy((char*)&wd_o_payin,wp_payin,sizeof(wd_o_payin));
			if(memcmp(wp_payin->F_rcpstat,"00",2)!=0)
			{ /* ����Ǿܾ�������Ž��տ��˺͸����˷����� */
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
			vtcp_log("%s,%d ʵʱ��ǻ�ִ���ʸ���ԭ��������Ϣ��Ȼ��Ի����ͽ��ջ�����Ϣ ",__FILE__,__LINE__);
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
		/* ======= ����Ӧ������ T_wtday T_orderno T_orbrno���ֶ�  ========= 
		======= Ҫ��Ȼ�������ԭ��¼���ظ�                     =========   */

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
		/* rem by LiuHuafeng 2007-8-22 14:47:35 ��ϵͳ��ʲô����ʲôCC1��ԭ�������к�
		GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));	
		***** end by LiuHuafeng 2007-9-1 0:18:29 */
		/* end NEWTCTD1 */
		vtcp_log("%s,%d,F_orbrno=%.12s",__FILE__,__LINE__,wp_payin->F_orbrno); 
		vtcp_log("%s,%d,F_orderno=%.8s",__FILE__,__LINE__,wp_payin->F_orderno); 
		/*** dongxy 20070313 Ʊ��Ҫ�ؼ��ж�Ӧ�ֶ���58A begin **/
    		/**add by wangyongwei 20070814**/
    		/**ȡ�������ͺ�***/
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
			/**GETTAG_SIZE(wp_lvpack,"52A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));��52A����ȡ����ֵ��**/
			GETTAG(wp_lvpack,"52A",cAcbrno);
      			memcpy(wp_payin->F_orbrno,cAcbrno,sizeof(wp_payin->F_orbrno));
			/**modified wangyongwei 20070808**/
		}
		/*** dongxy 20070313 Ʊ��Ҫ�ؼ��ж�Ӧ�ֶ���58A begin **/

		GETTAG(wp_lvpack,"CIA",wp_payin->F_rcpstat);
		vtcp_log("%s,%d,F_rcpstat=%.2s",__FILE__,__LINE__,wp_payin->F_rcpstat); 

		memcpy(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd));
		memset(caValue, 0 , sizeof(caValue));
		GETTAG_SIZE(wp_lvpack,"0BC",caValue,sizeof(caValue)-1);/*֧���������*/
		lv_orderno_add0(caValue, sizeof(wp_payin->F_orderno),wp_payin->F_orderno);
		vtcp_log("%s,%d,F_orbrno=%.12s",__FILE__,__LINE__,wp_payin->F_orbrno); 	
		wp_payin->A_stat[0]=STAT_LZRECV1;			
		wp_payin->F_lw_ind[0] = '2';		/*����*/	

		vtcp_log("%s,%d,F_wtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_wtday); 
		vtcp_log("%s,%d,F_orderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_orderno); 		
		vtcp_log("%s,%d,F_orbrno=[%.12s]",__FILE__,__LINE__,wp_payin->F_orbrno); 		

		vtcp_log("%s,%d,F_owtday=[%.8s]",__FILE__,__LINE__,wp_payin->F_owtday); 
		vtcp_log("%s,%d,F_oorderno=[%.8s]",__FILE__,__LINE__,wp_payin->F_oorderno); 		
		vtcp_log("%s,%d,F_o_or_br_no=[%.12s]",__FILE__,__LINE__,wp_payin->F_o_or_br_no); 		
		/*beg add by chenggx ���ڽ��ҵ���ԭ�������к� ���� ԭ�������к� �޸�Ϊ �� CC1��ȡֵ*/
		if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0){
			GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_o_or_br_no,sizeof(wp_payin->F_o_or_br_no));
			vtcp_log("%s,%d,CHENGGX ���ڽ��ҵ���ִ�ߵ���� ���߳�����! PKGNO[%.3s]",__FILE__,__LINE__,wp_payin->F_pkgno);
			vtcp_log("%s,%d,F_o_or_br_no=[%.12s]",__FILE__,__LINE__,wp_payin->F_o_or_br_no);
		}
		/*end add by chenggx ���ڽ��ҵ���ԭ�������к� ���� ԭ�������к� �޸�Ϊ �� CC1��ȡֵ*/
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
		vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"ȡ�ñ������ݴ���");WRITEMSG
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

		/****��ί������****/
		GETHEADTAG(wp_parent,"30E",wp_payin->F_packday);
		/****��id****/
		GETHEADTAG(wp_parent,"0BD",wp_payin->F_packid);
		/**add by wangyongwei **��ȡ��Ʊ��**/
		/**���Ի���Ʊ����˵��**/
		GETTAG_SIZE(wp_lvpack,"72A",wp_payin->F_content,sizeof(wp_payin->F_content));
		vtcp_log("%s,%d,F_content=[%.60s]",__FILE__,__LINE__,wp_payin->F_content);  
		/**֧Ʊ����**/
		GETTAG_SIZE(wp_lvpack,"CSH",wp_payin->F_vocnum,sizeof(wp_payin->F_vocnum));
		vtcp_log("%s,%d,F_vounum[%.12s]",__FILE__,__LINE__,wp_payin->F_vocnum); 
		GETHEADTAG(wp_parent,"011",wp_payin->A_payqsactno);/**������������*/
		vtcp_log("%s,%d,A_payqsactno[%.12s]",__FILE__,__LINE__,wp_payin->A_payqsactno); 
		GETHEADTAG(wp_parent,"012",wp_payin->A_cashqsactno);/*������������*/ 
		vtcp_log("%s,%d,A_cashqsactno[%.12s]",__FILE__,__LINE__,wp_payin->A_cashqsactno); 
		char tply[11];
		memset(tply,0,sizeof(tply));
		GETTAG_SIZE(wp_lvpack,"CCA",tply,sizeof(tply)-1);
		vtcp_log("%s,%d,��Ʊ����Ϊ:[%s]\n",__FILE__,__LINE__,tply);
		/**NEWTCTD 20070912 wangyongwei **/
		if(memcmp(wp_payin->F_txnum,"30104",5)==0 || memcmp(wp_payin->F_txnum,"30105",5)==0){
			iRc=iLvSetAddit(wp_payin,"TPL",tply,10);
		}
		/**add by wangyongwei **��ȡ��Ʊ����**/
		
		return(0);	
	}
	/**** ����Ϊ�ǻ�ִҵ��  ****/	
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);
	/**** �����ദ�� ****/
	if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,sizeof(wp_payin->F_pkgno))==0)
	{		/****���ڴ���****/
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
		vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"ȡ�ñ������ݴ���");
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
		vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"ȡ�ñ������ݴ���");WRITEMSG
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
			/* ����ȡ�տ����嵥��ǰ�漸���̶�����  */
			iRc=lv_get_tag_multi(wp_lvpack,"B81", (char*)&wd_djdtl,	offset, sizeof(DQDJ_DTL_AREA)-1);
			if (iRc)
			{
				vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"ȡ�ñ������ݴ���");
				return(iRc);
			}
			vtcp_log("%s,%d addlen=[%s]  orderno=[%s]",__FILE__,__LINE__,wd_djdtl.addlen, wd_djdtl.orderno);
			/* ���ݸ������ݳ���ȡ�������� */
			nTxAddLen = apatoi(wd_djdtl.addlen,sizeof(wd_djdtl.addlen));
			iRc=lv_get_tag_multi(wp_lvpack,"B81", cTxAddText, offset+sizeof(DQDJ_DTL_AREA)-1, nTxAddLen);
			offset+=sizeof(DQDJ_DTL_AREA)-1+nTxAddLen;  /* ������¼��λ�� */

			vtcp_log("%s,%d offset=====[%d]",__FILE__,__LINE__,offset);	
			vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_djdtl.orderno);
		}
	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0 ||
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno))==0) 
	{   /****���ڽ��****/
		int i=0;
		int offset=0;
		char cAddLen[9];
		memset(cAddLen,'\0',sizeof(cAddLen));
		/* rem by LiuHuafeng 20060406 
		for(i=0,offset=0;i<id;i++){
			iRc=lv_get_tag_multi(wp_lvpack,"B82",cAddLen,
			offset+sizeof(DQJJ_DTL_AREA)-9,8);
			if(iRc){
				vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"ȡ�ñ������ݴ���");
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
		vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
		sprintf( acErrMsg,"ȡ�ñ������ݴ���");WRITEMSG
		return(iRc);
		}
		nTxAddLen=apatoi(wd_jjdtl.addlen,sizeof(wd_jjdtl.addlen));
		iRc=lv_get_tag_multi(wp_lvpack,"B82",cTxAddText,
		offset+sizeof(DQJJ_DTL_AREA)-1,nTxAddLen);
		offset +=sizeof(DQJJ_DTL_AREA)-1+nTxAddLen;
		*************************************/

		vtcp_log("%s,%d,���ڽ��  \n",__FILE__,__LINE__);

		vtcp_log("%s,%d id=[%d]",__FILE__,__LINE__,id);	
		offset = 0;
		for (i=0; i<=id; i++)
		{
			memset(&wd_jjdtl,'\0',sizeof(wd_jjdtl));
			/* ����ȡ�������嵥��ǰ�漸���̶�����  */
			iRc=lv_get_tag_multi(wp_lvpack,"B82", (char*)&wd_jjdtl,	offset, sizeof(DQJJ_DTL_AREA)-1);
			if (iRc)
			{
				vtcp_log("%s,%d,ȡ�ñ������ݴ���\n",__FILE__,__LINE__);
				sprintf( acErrMsg,"ȡ�ñ������ݴ���");
				return(iRc);
			}
			/* ���ݸ������ݳ���ȡ�������� */
			nTxAddLen=apatoi(wd_jjdtl.addlen,sizeof(wd_jjdtl.addlen));
			iRc=lv_get_tag_multi(wp_lvpack,"B82", cTxAddText, offset+sizeof(DQJJ_DTL_AREA)-1, nTxAddLen);
			offset+=sizeof(DQJJ_DTL_AREA)-1+nTxAddLen;  /* ������¼��λ�� */

			vtcp_log("%s,%d offset=[%d]",__FILE__,__LINE__,offset);	
			vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_jjdtl.orderno);

		}
		vtcp_log("%s,%d i=[%d] id=[%d] orderno=[%s]",__FILE__,__LINE__,i,id,wd_jjdtl.orderno);
	}
	else		/****�Ƕ���ҵ��****/
	{
		memset(caValue,'\0',sizeof(caValue));
		GETTAG(wp_lvpack,"B40",caValue);    /*  �������ݳ���  */
		nTxAddLen=atoi(caValue);
		GETTAG(wp_lvpack,"72C",cTxAddText); /*  ��������  */

	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	/* �ý����־ */	
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
		vtcp_log("%s,%d,�Ƿ���pkgno:[%.3s]\n",	__FILE__,__LINE__,wp_payin->F_pkgno);
		sprintf( acErrMsg,"�Ƿ��ı�������");WRITEMSG
		return(-1);
	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 
	memcpy(wp_payin->F_pkgno,wp_lvpack->CMTCode,sizeof(wp_payin->F_pkgno));	
	memcpy(wp_payin->F_opcd,OPCD_RECEIVE,sizeof(wp_payin->F_opcd)); /**�������� **/
	vtcp_log("%s,%d,F_opcd=%.1s\n",__FILE__,__LINE__,wp_payin->F_opcd);

	GETTAG_SIZE(wp_lvpack,"0BG",wp_payin->F_txnum,sizeof(wp_payin->F_txnum));	/****ҵ�����ͺ�****/
	vtcp_log("%s,%d,F_txnum=%.5s\n",__FILE__,__LINE__,wp_payin->F_txnum);

	GETTAG_SIZE(wp_lvpack,"CEG",wp_payin->F_ywtype,sizeof(wp_payin->F_ywtype));	/****ҵ������****/
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	GETTAG_SIZE(wp_lvpack,"30A",wp_payin->F_wtday,8);	/****ί������****/
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);

	wp_payin->F_operlevel[0]=wp_lvpack->paymPRI;	/****���ȼ�****/
	vtcp_log("%s,%d,paymPRI=%.1s\n",__FILE__,__LINE__,wp_payin->F_operlevel);

	/****Modified by SSH,2005.12.1,������Ϣ�Ӹ�����ȡ****/
	GETHEADTAG(wp_parent,"30E",wp_payin->F_packday);	/****��ί������****/
	vtcp_log("%s,%d,F_packday=%.8s\n",__FILE__,__LINE__,wp_payin->F_packday);	

	memset(caValue,0,sizeof(caValue));
	GETHEADTAG(wp_parent,"0BD",caValue); /* ����� */

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);	
	lv_orderno_add0(caValue, sizeof(wp_payin->F_packid),wp_payin->F_packid);
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);	

	vtcp_log("%s,%d,F_packid=%.8s\n",__FILE__,__LINE__,wp_payin->F_packid);

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);

	GETHEADTAG(wp_parent,"011",wp_payin->A_payqsactno); /* �������������к� */
	GETHEADTAG(wp_parent,"012",wp_payin->A_cashqsactno);/* �������������к� */
	vtcp_log("%s,%d A_payqsactno=%.12s\n",__FILE__,__LINE__,wp_payin->A_payqsactno);	
	vtcp_log("%s,%d A_cashqsactno=%.12s\n",__FILE__,__LINE__,wp_payin->A_cashqsactno);

	memcpy(wp_payin->F_respday,wp_lvpack->respDate,sizeof(wp_payin->F_respday));	/****Ӧ������****/
	vtcp_log("%s,%d,F_respday=%.8s\n",__FILE__,__LINE__,wp_payin->F_respday);
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 

	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	if (memcmp(wp_payin->F_pkgno,PKGNO_DQDJ,sizeof(wp_payin->F_pkgno))==0)
	{
		/********************���ڴ���***********************************/
		/***************************************************************/
		/****֧���������****/
		memcpy(wp_payin->F_orderno,wd_djdtl.orderno,sizeof(wp_payin->F_orderno));
		/****�������к�****/

		GETTAG_SIZE(wp_lvpack,"CCM",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		/****�������к�****/
		memcpy(wp_payin->F_acbrno,wd_djdtl.acbrno,	
			sizeof(wp_payin->F_acbrno));
		/****���׽��****/
		memcpy(wp_payin->F_txamt,wd_djdtl.txamt,	
			sizeof(wp_payin->F_txamt));
		/****�����˿�����****/

		GETTAG_SIZE(wp_lvpack,"CC4",wp_payin->F_payno,sizeof(wp_payin->F_payno));
		/****�������ʺ�*****/

		GETTAG_SIZE(wp_lvpack,"50C",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
		/****����������****/

		GETTAG_SIZE(wp_lvpack,"50A",wp_payin->F_payname,sizeof(wp_payin->F_payname));
		/****�����˵�ַ****/

		GETTAG_SIZE(wp_lvpack,"50B",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
		/****�տ��˿�����****/
		memcpy(wp_payin->F_cashno,wd_djdtl.cashno,	
			sizeof(wp_payin->F_cashno));
		/****�տ����ʺ�****/
		memcpy(wp_payin->F_cashactno,wd_djdtl.cashactno,	
			sizeof(wp_payin->F_cashactno));
		/****�տ�������****/
		memcpy(wp_payin->F_cashname,wd_djdtl.cashname,	
			sizeof(wp_payin->F_cashname));
		/****�տ��˵�ַ****/
		memcpy(wp_payin->F_cashaddress,wd_djdtl.cashaddress,	
			sizeof(wp_payin->F_cashaddress));
		/****����****/
		memcpy(wp_payin->F_content,wd_djdtl.content,	
			sizeof(wp_payin->F_content));

	}
	else if (memcmp(wp_payin->F_pkgno,PKGNO_DQJJ, sizeof(wp_payin->F_pkgno))==0 || 
		memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ, sizeof(wp_payin->F_pkgno))==0)
	{
		/*********************���ڽ��***********************************/ 
		/***************************************************************/
		vtcp_log("%s,%d-------wd_jjdtl:\n%s\n------\n",__FILE__,__LINE__,&wd_jjdtl);
		/****֧���������****/ 
		memcpy(wp_payin->F_orderno,wd_jjdtl.orderno,	sizeof(wp_payin->F_orderno)); 
		/****�������к�****/ 
		GETTAG_SIZE(wp_lvpack,"CCN",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		/****�������к�****/ 
		memcpy(wp_payin->F_acbrno,wd_jjdtl.acbrno,	sizeof(wp_payin->F_acbrno)); 
		/****���׽��****/ 
		memcpy(wp_payin->F_txamt,wd_jjdtl.txamt,	sizeof(wp_payin->F_txamt));
		/****�����˿�����****/
		memcpy(wp_payin->F_payno,wd_jjdtl.payno,	sizeof(wp_payin->F_payno));
		/****�������ʺ�*****/
		memcpy(wp_payin->F_payactno,wd_jjdtl.payactno,	sizeof(wp_payin->F_payactno));
		/****����������****/
		memcpy(wp_payin->F_payname,wd_jjdtl.payname,	sizeof(wp_payin->F_payname));
		/****�����˵�ַ****/
		memcpy(wp_payin->F_payaddress,wd_jjdtl.payaddress,sizeof(wp_payin->F_payaddress));
		/****�տ��˿�����****/
		GETTAG_SIZE(wp_lvpack,"CC5",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
		/****�տ����ʺ�****/
		GETTAG_SIZE(wp_lvpack,"59C",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
		/****�տ�������****/
		GETTAG_SIZE(wp_lvpack,"59A",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
		/****�տ��˵�ַ****/
		GETTAG_SIZE(wp_lvpack,"59B",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress));
		/****����****/
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
		/*********************�Ƕ���ҵ��**********************************/
		/***************************************************************/

		/****֧���������****/
		/* by lhf 060303  ������
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
		/****�������к�****/
		if (GETTAG_SIZE(wp_lvpack,"CC2",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno))!=0)
		{
			/**û��ԭ������,ʹ�÷�����***/
			GETTAG_SIZE(wp_lvpack,"52A",wp_payin->F_orbrno,sizeof(wp_payin->F_orbrno));
		}
		if (GETTAG_SIZE(wp_lvpack,"CC1",wp_payin->F_acbrno,sizeof(wp_payin->F_acbrno))!=0)
		{
			/**û��ԭ������,ʹ�ý�����***/
			GETTAG_SIZE(wp_lvpack,"58A",wp_payin->F_acbrno,sizeof(wp_payin->F_acbrno));
		}
		vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
		vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 

		GETTAG_SIZE(wp_lvpack,"72A",wp_payin->F_content,sizeof(wp_payin->F_content));/*���������⣬��ʱ����	**����****/
		vtcp_log("%s,%d,F_content=%.60s\n",__FILE__,__LINE__,wp_payin->F_content);
		if (memcmp(wp_payin->F_pkgno,PKGNO_DJTH,	sizeof(wp_payin->F_pkgno))==0)
		{
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);			
			/*******************�����˻�********************************/
			/***********************************************************/
			/****���׽��****/
			GETTAG_AMT(wp_lvpack,"33S",wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
			/****�����˿�����****/
			GETTAG_SIZE(wp_lvpack,"CCK",wp_payin->F_payno,sizeof(wp_payin->F_payno));
			/****�������ʺ�*****/
			GETTAG_SIZE(wp_lvpack,"CQ2",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
			/****����������****/
			GETTAG_SIZE(wp_lvpack,"CR2",wp_payin->F_payname,sizeof(wp_payin->F_payname));
			/****�����˵�ַ****/
			GETTAG_SIZE(wp_lvpack,"59F",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
			/****�տ��˿�����****/
			GETTAG_SIZE(wp_lvpack,"CCL",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
			/****�տ����ʺ�****/
			GETTAG_SIZE(wp_lvpack,"CQ1",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
			/****�տ�������****/
			GETTAG_SIZE(wp_lvpack,"CR1",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
			/****�տ��˵�ַ****/
			GETTAG_SIZE(wp_lvpack,"50F",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress));
			/****ԭί������****/
			GETTAG_SIZE(wp_lvpack,"051",wp_payin->F_owtday,sizeof(wp_payin->F_owtday));
			/****ԭҵ�����ͺ�****/
			GETTAG_SIZE(wp_lvpack,"0BH",wp_payin->F_otxnum,sizeof(wp_payin->F_otxnum));
			/****ԭ֧���������****/
			/* rem by lhf
			GETTAG_SIZE(wp_lvpack,"005",wp_payin->F_oorderno);
			*********************************************/
			memset(caValue, 0  , sizeof(caValue));
			GETTAG(wp_lvpack,"005",caValue);
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);			
			lv_orderno_add0(caValue, sizeof(wp_payin->F_oorderno),wp_payin->F_oorderno);
			/***�˻�ԭ��**/
			GETTAG_SIZE(wp_lvpack,"CIA",wp_payin->F_retcode,sizeof(wp_payin->F_retcode));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
		}else
		{
			/********************�Ǵ����˻�*****************************/
			/***********************************************************/
			/****���׽��****/
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);		
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 	    	
			GETTAG_AMT(wp_lvpack,"33G",wp_payin->F_txamt,sizeof(wp_payin->F_txamt));
			vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	

			vtcp_log("[%s][%d]===========wp_payin->F_txamt=[%s]\n",__FILE__,__LINE__,wp_payin->F_txamt);
			/****�����˿�����****/
			GETTAG_SIZE(wp_lvpack,"CC4",wp_payin->F_payno,sizeof(wp_payin->F_payno));
			/****�������ʺ�*****/
			GETTAG_SIZE(wp_lvpack,"50C",wp_payin->F_payactno,sizeof(wp_payin->F_payactno));
			/****����������****/
			GETTAG_SIZE(wp_lvpack,"50A",wp_payin->F_payname,sizeof(wp_payin->F_payname));
			vtcp_log("%s,%d �����˻���=[%s]",__FILE__,__LINE__,wp_payin->F_payname);
			/****�����˵�ַ****/
			GETTAG_SIZE(wp_lvpack,"50B",wp_payin->F_payaddress,sizeof(wp_payin->F_payaddress));
			/****�տ��˿�����****/
			GETTAG_SIZE(wp_lvpack,"CC5",wp_payin->F_cashno,sizeof(wp_payin->F_cashno));
			/****�տ����ʺ�****/
			GETTAG_SIZE(wp_lvpack,"59C",wp_payin->F_cashactno,sizeof(wp_payin->F_cashactno));
			/****�տ�������****/
			GETTAG_SIZE(wp_lvpack,"59A",wp_payin->F_cashname,sizeof(wp_payin->F_cashname));
			vtcp_log("%s,%d �տ��˻���=[%s]",__FILE__,__LINE__,wp_payin->F_payname);
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);				
			/****�տ��˵�ַ  ****/
			GETTAG_SIZE(wp_lvpack,"59B",wp_payin->F_cashaddress,sizeof(wp_payin->F_cashaddress)); 
			vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);	 
		}
	}
	vtcp_log("%s,%d,wp_payin->A_tbsdy=[%.8s]",__FILE__,__LINE__,wp_payin->A_tbsdy); 	
	vtcp_log("%s,%d,F_wtday=%.8s\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("%s,%d,F_ywtype=%.12s\n",__FILE__,__LINE__,wp_payin->F_ywtype);

	/************************************************************/
	if (memcmp(wp_payin->F_txnum,TXNUM_WT,sizeof(wp_payin->F_txnum))==0)
	{/****ί���տ�ҵ��ĸ����ֶ�****/
		wp_wtadd=(WT_ADD_AREA *)cTxAddText;
		memcpy(wp_payin->F_ptype,wp_wtadd->ptype,sizeof(wp_payin->F_ptype));
		memcpy(wp_payin->F_pdate,wp_wtadd->pdate,sizeof(wp_payin->F_pdate));
		memcpy(wp_payin->F_pnum,wp_wtadd->pnum,sizeof(wp_payin->F_pnum));
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TSCF, sizeof(wp_payin->F_txnum))==0)
	{		/****���ճи�ҵ��ĸ����ֶ�****/
		wp_tscfadd=(TSCF_ADD_AREA *)cTxAddText;
		memcpy(wp_payin->F_pdate,wp_tscfadd->pdate,sizeof(wp_payin->F_pdate));
		memcpy(wp_payin->F_pnum,wp_tscfadd->pnum,sizeof(wp_payin->F_pnum));
		memcpy(wp_payin->F_ptxamt,wp_tscfadd->ptxamt, sizeof(wp_payin->F_ptxamt));
		memcpy(wp_payin->F_jtxamt,wp_tscfadd->jtxamt, sizeof(wp_payin->F_jtxamt));
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TC, sizeof(wp_payin->F_txnum))==0)
	{/****ͨ��ҵ��****/
		wp_tcadd=(TCTD_ADD_AREA *)cTxAddText;
		wp_payin->F_actype[0]=wp_tcadd->payacttype[0];
		wp_payin->F_pswdtype[0]=wp_tcadd->pswdtype[0];
		memcpy(wp_payin->F_pswd,pcUndesPswdCode(wp_tcadd->pswdcode),
			sizeof(wp_payin->F_pswd));
		iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
		if (iRc)
		{
			vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"���븽�����ݱ�ʧ��");WRITEMSG
			return(-1);
		}
	}else if(memcmp(wp_payin->F_txnum,TXNUM_TD, sizeof(wp_payin->F_txnum))==0)
	{		/****ͨ��ҵ��****/
		wp_tdadd=(TCTD_ADD_AREA *)cTxAddText;
		wp_payin->F_actype[0]=wp_tdadd->payacttype[0];
		wp_payin->F_pswdtype[0]=wp_tdadd->pswdtype[0];
		memcpy(wp_payin->F_pswd,pcUndesPswdCode(wp_tdadd->pswdcode),
			sizeof(wp_payin->F_pswd));
		iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
		if (iRc)
		{
			vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"���븽�����ݱ�ʧ��");WRITEMSG
			return(-1);
		}
	}
	/****HSYX BEGIN:֧Ʊ��������,����һ����ӡ����ĸ�����,�Ա�����ʹ��***/
	else if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL, sizeof(wp_payin->F_txnum))==0||
		memcmp(wp_payin->F_txnum,TXNUM_TYJL, sizeof(wp_payin->F_txnum))==0)
	{
		vtcp_log("[%s][%d]-------pkgno=[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
		if(memcmp(wp_payin->F_pkgno,"002",sizeof(wp_payin->F_pkgno))==0)/**modify by YHBP**/
		{
		    /**add by wangyongwei Ϊ���������˽��**/
        iRc=iLvSetAddit(wp_payin,"TPL","",2);
		    /**add by wangyongwei Ϊ���������˽��**/
		    /****֧Ʊ/ͨ�ý���****/
		    char pcChy[2];
		    memset(pcChy,'\0',sizeof(pcChy));
		    pcChy[0]=CHY_NOTCHECK;
		    iRc=iLvSetAddit(wp_payin,"CHY",pcChy,1);
		}
		if (memcmp(wp_payin->F_txnum,TXNUM_ZPJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****����֧Ʊ����ҵ�񸽼���****/
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
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"002",&wd_zp_add2,iAddLen2);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"003",&wd_zp_add3,iAddLen3);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
		}else if(memcmp(wp_payin->F_txnum,TXNUM_TYJL,sizeof(wp_payin->F_txnum))==0)
		{
			/****����ͨ�ý���ҵ�񸽼���****/
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
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"002",&wd_ty_add2,iAddLen2);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"003",&wd_ty_add3,iAddLen3);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"004",&wd_ty_add4,iAddLen4);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
			iRc=iLvSetAddit(wp_payin,"005",&wd_ty_add5,iAddLen5);
			if (iRc)
			{
				vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��");
				WRITEMSG
					return(-1);
			}
		}
		/****HSYX END:֧Ʊ��������,����һ����ӡ����ĸ�����,�Ա�����ʹ��***/
	}
	else if (memcmp(wp_payin->F_txnum,TXNUM_DQJJ, sizeof(wp_payin->F_txnum))==0||memcmp(wp_payin->F_txnum,TXNUM_PLKS, sizeof(wp_payin->F_txnum))==0)
	{
		vtcp_log("[%s][%d]���ڽ��ҵ��!!!!!!!!!!\n",__FILE__,__LINE__);
	}
	else 
	{		/****���������ֶ�****/
		vtcp_log("%s,%d,=============δ֪�ĸ���������\n",__FILE__,__LINE__);
		vtcp_log("[%s]\n",cTxAddText);
		if (nTxAddLen!=0)
		{
			/* Remed by Chenming  2006-10-11 17:40   =============
			iRc=iLvSetAddit(wp_payin,"000",cTxAddText,strlen(cTxAddText));
			if (iRc)
			{
			vtcp_log("%s,%d,���븽�����ݱ�ʧ��\n", __FILE__,__LINE__);
			sprintf( acErrMsg,"���븽�����ݱ�ʧ��");WRITEMSG
			return(-1);
			}			
			==============      ʹ�ö෽Э��ţ����ٲ帽����      */
			GETTAG_SIZE(wp_lvpack,"B82",cTxAddText,sizeof(cTxAddText));
			memcpy(wp_payin->F_dfxyno, cTxAddText+207, sizeof(wp_payin->F_dfxyno));
			vtcp_log("%s,%d,wp_payin->F_dfxyno===\n[%.60s]",__FILE__,__LINE__,wp_payin->F_dfxyno);
			/* Added by Chenming  2006-10-11 17:40   =============*/

		}
	}
	/* Added by ChenMing   2006.09.26. */
	/* rem by LiuHuafeng 2006-9-29 23:43 ΪʲôҪ�����*/
	/*  char cDtlAmt[18];                              */

	/*	GETHEADTAG(wp_parent,"B63",wp_payin->F_txamt);  ***ҵ���ܱ���****/ 
	/*	memset(cDtlAmt,'\0',sizeof(cDtlAmt));	  */
	/*	GETHEADTAG(wp_parent,"32B",cDtlAmt);   ***ҵ���ܽ��****/
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
/* ������:lv_fetchcmt_to_notpayin                                       */
/* ��  ��:��CMT������ȡ�������ͨ�����ݽ�����                            */
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
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
		lv_get_tag(lv,"BS8",&wp_notpayin->rettype); 	/** ֻһλ **/
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
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
		/** orderno ������û�����ֵ�����Ը���oorderno **/
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
		/** 
		CIB	ԭ������״̬
		BS1	����ڵ�����
		BS2	��������
		BS3	�����
		BS5	��������
		**/
		/** orbrno ͬoorbrno��һ�� **/
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
		/** 
		CIB	ԭ������״̬
		BS1	����ڵ�����
		BS2	��������
		BS3	�����
		BS5	��������
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
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
		/* add by LiuHuafeng 20060402 Ӧ�ø���ֹ�������ж�acbrnoȡcc1����
		ȡcc0 
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
		GETTAG_SIZE(lv,"C15",wp_notpayin->pascode,sizeof(wp_notpayin->pascode));	/** ��Ѻ **/
		GETTAG_SIZE(lv,"011",wp_notpayin->payqsactno,sizeof(wp_notpayin->payqsactno));
		/* add by lhf 20060402 ����������˻�Ӧ��ȥ�Է���������orbrno*/
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
		/** CEK	�������ͱ�־
		BS1	����ڵ�����
		BS2	��������
		BS3	�����
		90C	�����־
		BS5	��������
		CIC	���㴦��״̬
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		/**BS1	����ڵ�����
		BS2	��������
		BS3	�����
		CIB	������״̬
		C42	���Ĵ�����
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
		vtcp_log("%s,%d ��������02d",__FILE__,__LINE__);
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
		/**CIA	ҵ���ִ״̬
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
		/**NEWTCTD:add by wangyongwei 20070817 ȡʵʱҵ��ĸ�����,������lv_addit��***/
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
		}else{ /****�˴�Ϊʯ������ 20130329****/
			GETTAG_SIZE(lv,"72C",buffer,ssxx_addit.taglen);	
			vtcp_log("%s,%d, iFIGHT!\n",__FILE__,__LINE__);
			ret=iLvSetAddit_Notpay(wp_notpayin,wp_notpayin->txday,"000",buffer,ssxx_addit.taglen);
			if(ret){
				vtcp_log("%s,%d, ���ʵʱ��Ϣ������ʧ��![%d]\n",__FILE__,__LINE__,ret);
				strcpy(g_pub_tx.reply,"D107");
				return(-1);
			}
		}/****�˴�Ϊʯ������ 20130329****/
		vtcp_log("%s,%d,ͨ����Ϣҵ�񣬸����ﳤ��=%d\n",__FILE__,__LINE__,ssxx_addit.taglen);
		/**add by wangyongwei 20070817 ȡʵʱҵ��ĸ�����,������lv_addit��***/
	}
	else /*Added by ChenMing 2006.09.26.*/ 
	{
		vtcp_log("[%s][%d]��������ˣ�CMTNO=[%s] �����ﲻ����! ",__FILE__,__LINE__,lv->CMTCode);	
	}
	vtcp_log("%s,%d opkgno===%.3s",__FILE__,__LINE__,wp_notpayin->opkgno);
	vtcp_log("[%s][%d]========p_notpayin->cmtno=[%s]\n",__FILE__,__LINE__,wp_notpayin->cmtno);
	vtcp_log("[%s][%d]============wp_notpayin->respcode=[%c]\n",__FILE__,__LINE__,wp_notpayin->respcode);
	return(0);
}
/****Add by SSH,2005.11.28,���Ӳ��뷢�ͺͽ��յǼǲ��ĺ���****/
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
	if (lv->pkgType==1)	/** ��pkg�� **/
	{
		vtcp_log("[%s][%d]--------\n",__FILE__,__LINE__);
		memcpy(wd_lv_sendlist.packtype, "PKG", sizeof(wd_lv_sendlist.packtype)-1);
		GETHEADTAG(lv,"0BD",wd_lv_sendlist.orderno);
		/** pkg��ȡ�������к� **/
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
		sprintf( acErrMsg,"����С��Ľ��յǼǲ�ʧ��");WRITEMSG
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
	if (lv->pkgType==1)	/** ��pkg�� **/
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
		sprintf( acErrMsg,"����С��Ľ��յǼǲ�ʧ��");WRITEMSG
		return(-1);
	}
	return(0);
}                   
