#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lv_define.h"
#include "hv_define.h"
#include "lv_tito.h"
#include "lv_pub.h"
#include "hv_orno_brno_c.h"
#include "lv_newxyh_c.h"
#include "lv_addit_c.h"
#include "lv_sysctl_c.h"
#include "seqctl_c.h"
#include "lv_fd123.h"
#include "lv_sysctl_c.h"
#include "hv_addit_c.h"
#include "public.h"
#include "lv_c254r_c.h"
#include "hv_orno_brno_c.h"
#include "lv_c552r_c.h"
#include "hv_fzfhtr_c.h"
#include "lv_pkghtr_c.h"
#include "cashier_c.h"/**YHBP**/
/***add by YH_BP****/
#include"stdio.h"
#include"stdlib.h"
/***end by YH_BP****/
extern char* pcLvGetDay();
extern char* pcPayinRetdtl(PAY_IN_AREA *wp_payin);
extern char * pcLvDesPswdCode(char *code);
extern char* pcLvGetRecvno();

int iLvGetSysStat(char *hvsts)
{	
	struct lv_sysctl_c lv_sysctl;
	memset(&lv_sysctl,0,sizeof(lv_sysctl));

	g_reply_int = Lv_sysctl_Sel(g_pub_tx.reply, &lv_sysctl,"1=1");
	if(g_reply_int)
	{
		sprintf(acErrMsg,"[%s][%d]��С��ϵͳ״̬�����,ERR=[%d]",__FL__,g_reply_int);
		vtcp_log("[%s][%d]��ѯС��ϵͳ״̬�����,ERR=[%d]",__FL__,g_reply_int);
		return (g_reply_int);
	}
	vtcp_log("%s,%d stat ====[%s]",__FL__,lv_sysctl.stat);
	hvsts[0]=lv_sysctl.stat[0];
	return (0);
}

/***************************************************************************/
/* ��������: iLvGetAddit_Notpay                                            */
/* ��������: �õ�����������                                                */
/* ����:                                                                   */
/*            wp_notpay   -֧������ͨ������ṹ��                           */
/*            wtday      -ί������                                         */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���                                 */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ                       */
/* from pubf_notpay_comm_dxy.c 																						 */
/***************************************************************************/
int iLvGetAddit_Notpay(char *wtday,char *addid,char *name,char *buf,int length)
{
	int ret=0;
	int i=0,j=0;
	/****NEWYX2:ʹ��С��汾****/
	struct lv_addit_c sLvAddit;
	memset(&sLvAddit,'\0',sizeof(struct hv_addit_c));

	/*begin NEWYX rem by LiuHuafeng 2007-4-4 0:49
	ret = Hv_addit_Sel(g_pub_tx.reply, &sLvAddit,"tx_date = '%s' and addid = '%s' and tagname= '%s' ",wtday,addid,name);
	******replace by next line ******************************/
	ret = Lv_addit_Sel(g_pub_tx.reply, &sLvAddit,"tx_date = '%s' and addid = '%s' and tagname= '%s' ",wtday,addid,name);
	/** end NEWYX by LiuHuafeng 2007-4-4 0:51 */
	if ( ret == 1403 )
	{
		sprintf( acErrMsg, "ԭ��ѯ��¼������!" );
		WRITEMSG
		return -1;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Lv_wbctl_Sel��![%d]", ret );
		WRITEMSG
		return -1;
	}
	/****NEWYX2:�����г���ȡ,������ո�****/
	if (length<=0)
	{
		strcpy(buf,sLvAddit.tagdata);
		length=sLvAddit.taglen;
	}else
	{
		memcpy(buf,sLvAddit.tagdata,length);
	}
	for(i=0;i<length;i++){
		if(buf[i]=='\0'){
			break;
		}
	}
	for(j=i;j<length;j++){
		buf[j]=' ';
	}
	return(0);
}
/***************************************************************************/
/* ��������: iLvSetAddit_Notpay                                            */
/* ��������: ���Ӹ���������                                                */
/* ����:                                                                   */
/*            wp_payin   -֧������ͨ������ṹ��                           */
/*            wtday      -ί������                                         */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���                                 */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ, Ϊ000��ʾ�˸�����Ҫ  */
/*           ֱ��ƴ����������������,����Ϊ�����ڲ�����                     */
/* from pubf_notpay_comm_dxy.c */
/***************************************************************************/
int iLvSetAddit_Notpay(NOTPAY_IN_AREA *wp_notpay,char *wtday,char *name,char *buf,int length)
{
	int iRc=0;
	/****NEWYX2:ʹ��С��汾****/
	struct lv_addit_c lv_addit;
	memset(&lv_addit,'\0',sizeof(lv_addit));

	lv_addit.tx_date=apatoi(wtday,8);
	vtcp_log("%s,%d,Gethere[%.8s][%.8s]!\n",__FILE__,__LINE__,wtday,wp_notpay->addid); 
	if (wp_notpay->addid[0]=='\0')
	{
		apitoa(iLvGetAddid(),sizeof(lv_addit.addid)-1,lv_addit.addid);
	}else
	{
		memcpy(lv_addit.addid,wp_notpay->addid,sizeof(lv_addit.addid)-1);
	}
	vtcp_log("%s,%d,Gethere[%.8s][%.8s]!\n",__FILE__,__LINE__,wtday,wp_notpay->addid); 
	memcpy(lv_addit.tagname,name, sizeof(lv_addit.tagname)-1);
	memcpy(lv_addit.tagdata,buf,length);
	lv_addit.taglen=length;
	iRc=Lv_addit_Ins(lv_addit,g_pub_tx.reply);
	if (iRc)
	{
		vtcp_log("%s,%d, Insert lv_addit error=%d\n",__FILE__,__LINE__,iRc);
		sprintf( acErrMsg, "ִ��Lv_addit_Ins����!ret=[%d]", iRc );
		WRITEMSG
		return -1;

	}
	memcpy(wp_notpay->addid,lv_addit.addid,sizeof(wp_notpay->addid));
	return(0);
}


/***********************************************************
* �� �� ��:  iGetLvRefno
* ���������� �õ�С��֧��ϵͳ�Ĳο��ţ�ȫ��ͳһ����
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
***********************************************************/
int iGetLvRefno(char * br_no,char * refno)
{
	struct seqctl_c hv_seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	int ret=0;
	memset(cSeqno, 0 ,sizeof(cSeqno));
	memset(cFullRefno, 0 , sizeof(cFullRefno));
	memset(&hv_seqctl_c, 0 , sizeof(hv_seqctl_c));
	memcpy(cFullRefno,br_no,12);
	vtcp_log("%s,%d Seqctl_Dec_Upd",__FILE__,__LINE__);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",
		HV_BR_NO,LV_FLGACTYPE);
	if (ret)
	{
		sprintf( acErrMsg,"ִ��Point_point_mst_Dec_Upd��![%s][%d][%d]",__FL__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Point_point_mst_Dec_Upd��");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Fet_Upd",__FILE__,__LINE__);
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"��ѯ���֧��������ű����![%s][%d][%d]",__FL__,ret);
		set_zd_data(DC_GET_MSG,"��ѯ���֧��������ű����");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"ִ��seqctl_Fet_Upd����![%s][%d][%d]",__FL__,ret);
		set_zd_data(DC_GET_MSG,"ִ��seqctl_Fet_Upd����");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08ld",hv_seqctl_c.seqno);
	strncat(cFullRefno,cSeqno,sizeof(cSeqno)-1);
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d Seqctl_Upd_Upd",__FILE__,__LINE__);
	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"�޸Ļ����������![%s][%d][%d]",__FL__,ret);
		set_zd_data(DC_GET_MSG,"�޸Ļ����������");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Clo_Upd",__FILE__,__LINE__);
	ret=Seqctl_Clo_Upd();
OkExit:
	memcpy(refno,cFullRefno,sizeof(cFullRefno)-1);
	sprintf(acErrMsg,"ȡ��֧��������ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	return 0;
ErrExit:
	sprintf(acErrMsg,"ȡ��֧���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	return 1;
}

/************************************************************************* 
* �� �� ��:  iLvUpdZfRecFromPayin                  
* ���������� ���� ֧���ṹ T ���� ���� lv_pkghtr    
* ��    ��:  xyy                            
* ������ڣ� 2006-9-14 16:32                             
* �޸ļ�¼��                                      
* ��   ��:                                      
* �� �� ��:                                     
* �޸�����:                                      
*************************************************************************/ 
int iLvUpdZfRecFromPayin(PAY_IN_AREA *lv_payin,int insflg)/* insflg 0.���� 1.����*/
{          	
	int ret=0;
	struct lv_pkghtr_c lv_pkghtr;
	memset(&lv_pkghtr, 0, sizeof(lv_pkghtr));
	vtcp_log("iLvUpdZfRecFromPayin() ��ʼ ");

	
	memcpy(lv_pkghtr.or_br_no,lv_payin->T_orbrno,sizeof(lv_payin->T_orbrno));
	if ( strlen(lv_payin->T_wtday) > 0 )  
		lv_pkghtr.wt_date = apatoi(lv_payin->T_wtday,8);
	memcpy(lv_pkghtr.orderno,lv_payin->T_orderno,sizeof(lv_payin->T_orderno));


	if (insflg == 0) 
	{ /* 0.����    */
		ret = Lv_pkghtr_Dec_Upd(g_pub_tx.reply," or_br_no='%s' and wt_date=%ld and orderno='%s' ", \
			lv_pkghtr.or_br_no,lv_pkghtr.wt_date,lv_pkghtr.orderno);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
			return ret;
		}
		ret = Lv_pkghtr_Fet_Upd(&lv_pkghtr, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"��ѯ֧�����ױ����!![%d]",ret);
			WRITEMSG
			return ret;
		}
		else if (ret == 100)
		{
			Lv_pkghtr_Clo_Upd();
			sprintf(acErrMsg,"֧�����ױ��в����ڸü�¼!!");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O127");
			return 0; /* �����Ҳ�����¼�ݲ����� ���� Added by ChenMing  2006-11-9 23:31 */
		}		
		iLv_pay_to_tab(lv_payin,&lv_pkghtr);
  	
		vtcp_log("[%s][%d] ����֧�����ױ��¼�ɹ�",__FILE__,__LINE__);	

		ret = Lv_pkghtr_Upd_Upd(lv_pkghtr, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
			WRITEMSG
			return ret;
		}    
		Lv_pkghtr_Clo_Upd();	
		vtcp_log("[%s][%d] ����֧�����ױ��¼�ɹ�",__FILE__,__LINE__);	
	}
	else
	{  /* 1.����  */

		iLv_pay_to_tab(lv_payin,&lv_pkghtr);
		ret = Lv_pkghtr_Ins(lv_pkghtr, g_pub_tx.reply);

		if (ret != 0)
		{
			sprintf(acErrMsg,"��֧�����ױ������Ӽ�¼����!![%d]",ret);
			WRITEMSG
			Lv_pkghtr_Debug(&lv_pkghtr);
			return ret;
		}	
		vtcp_log("[%s][%d] ��֧�����ױ������Ӽ�¼�ɹ�",__FILE__,__LINE__);
	}	
	return 0;
}

int iLv_pay_to_tab(PAY_IN_AREA *wp_payin,struct lv_pkghtr_c * wd_lv_pkghtr)
{
	get_fd_data("0030", lvca.brno);
	vtcp_log("[%s][%d]lvca.brno=[%s]",__FILE__,__LINE__,lvca.brno);
	memcpy(wd_lv_pkghtr->br_no,lvca.brno,BRNO_LEN);
	memcpy(wd_lv_pkghtr->pkgno , wp_payin->T_pkgno, sizeof(wp_payin->T_pkgno));
	wd_lv_pkghtr->wt_date = apatoi(wp_payin->T_wtday, 8);
	wd_lv_pkghtr->pack_date = apatoi(wp_payin->T_packday, 8);
	memcpy(wd_lv_pkghtr->orderno,wp_payin->T_orderno,sizeof(wd_lv_pkghtr->orderno)-1);
	memcpy(wd_lv_pkghtr->txnum,wp_payin->T_txnum,5);	
	memcpy(wd_lv_pkghtr->or_br_no,wp_payin->T_orbrno,sizeof(wd_lv_pkghtr->or_br_no)-1);
	memcpy(wd_lv_pkghtr->ac_br_no,wp_payin->T_acbrno,sizeof(wd_lv_pkghtr->ac_br_no)-1);
	str2dbl(wp_payin->T_txamt,sizeof(wp_payin->T_txamt),0,&wd_lv_pkghtr->tx_amt);
	wd_lv_pkghtr->tx_amt/=100;
	memcpy(wd_lv_pkghtr->pay_ac_no,wp_payin->T_payactno,sizeof(wd_lv_pkghtr->pay_ac_no)-1);
	memcpy(wd_lv_pkghtr->cash_ac_no,wp_payin->T_cashactno,sizeof(wd_lv_pkghtr->cash_ac_no)-1);
	memcpy(wd_lv_pkghtr->cash_name,wp_payin->T_cashname,sizeof(wd_lv_pkghtr->cash_name)-1);
	memcpy(wd_lv_pkghtr->lv_yw_ind,wp_payin->T_ywtype,sizeof(wd_lv_pkghtr->lv_yw_ind)-1);
	memcpy(wd_lv_pkghtr->lv_brf,wp_payin->T_content,sizeof(wd_lv_pkghtr->lv_brf)-1);
	wd_lv_pkghtr->resp_date = apatoi(wp_payin->T_respday, 8);
	memcpy(wd_lv_pkghtr->rcpstat,wp_payin->T_rcpstat,sizeof(wd_lv_pkghtr->rcpstat)-1);
	memcpy(wd_lv_pkghtr->resp1,wp_payin->T_resp1,sizeof(wd_lv_pkghtr->resp1)-1);
	memcpy(wd_lv_pkghtr->resp2,wp_payin->T_resp2,sizeof(wd_lv_pkghtr->resp2)-1);
	wd_lv_pkghtr->lv_sts[0] = wp_payin->T_stat[0];
	wd_lv_pkghtr->operlevel[0] = wp_payin->T_operlevel[0];
	wd_lv_pkghtr->tx_chrg_ind[0] = wp_payin->T_feeflag[0];
	wd_lv_pkghtr->checkflag[0] = wp_payin->T_checkflag[0];
	str2dbl(wp_payin->T_feeamt,sizeof(wp_payin->T_feeamt),0,&wd_lv_pkghtr->chrg_amt);
	wd_lv_pkghtr->chrg_amt/=100;
	memcpy(wd_lv_pkghtr->receco,wp_payin->T_receco,sizeof(wd_lv_pkghtr->receco)-1);	
	memcpy(wd_lv_pkghtr->qsprtflag,wp_payin->T_qsprtflag,sizeof(wd_lv_pkghtr->qsprtflag)-1);
	memcpy(wd_lv_pkghtr->brprtflag,wp_payin->T_brprtflag,sizeof(wd_lv_pkghtr->brprtflag)-1);
	memcpy(wd_lv_pkghtr->packid,wp_payin->T_packid,sizeof(wd_lv_pkghtr->packid)-1);
	memcpy(wd_lv_pkghtr->filler,wp_payin->T_filler,sizeof(wd_lv_pkghtr->filler)-1);
	/** add by zyl 20110407 **/
	if(memcmp(wp_payin->F_opcd,"10",2)==0)
	{
		apitoa(g_pub_tx.trace_no,6,wd_lv_pkghtr->input_trace_no);
	}
	else if (memcmp(wp_payin->F_opcd,"30",2)==0)
	{
		apitoa(g_pub_tx.trace_no,6,wd_lv_pkghtr->check_trace_no);
	}
	else
	{
		apitoa(g_pub_tx.trace_no,6,wd_lv_pkghtr->send_trace_no);
	}
	wd_lv_pkghtr->in_date = g_pub_tx.tx_date;

	/** end by zyl 20110407 **/
	/****NEWTCTD:����������ʶ****/
	memcpy(wd_lv_pkghtr->lw_ind,wp_payin->T_lw_ind,sizeof(wd_lv_pkghtr->lw_ind)-1);
	return 0;
}
/***************************************************************************/
/* ��������: int iLvFetchUnanswerToPayin(PAY_IN_AREA *wp_payin)            */
/* ��������: �ӽ�ǻ���ǽ��ձ����ҵ���Ҫ�İ�����Сһ��δ��ִ�ļ�¼        */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:     �������� wtday,brno,pkgday,pkgid,orderno,cOrbrno              */
/***************************************************************************/
int iLvFetchUnanswerToPayin(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	int iPackday=0;
	int iWtday=0;
	char cWtday[9];
	char cPackid [9];
	char cOrderno[9];
	char cOrbrno [13];
	char cBrno [BRNO_LEN+1];
	char cFindValue[17];
	char cOldPkgno[4];
	char cOldOpcd[3];

	memset(cOldPkgno, 0, sizeof(cOldPkgno));
	memset(cOldOpcd , 0 , sizeof(cOldOpcd));
	memset(cFindValue , 0 , sizeof(cFindValue));
	memset(cPackid , 0 , sizeof(cPackid ));
	memset(cOrderno, 0 , sizeof(cOrderno));
	memset(cOrbrno , 0 , sizeof(cOrbrno ));

	
	memcpy(cWtday ,wp_payin->F_owtday ,sizeof(cWtday )-1);
	zip_space(cWtday );
	iWtday = apatoi(wp_payin->F_wtday,8);
	memcpy(cOrderno,wp_payin->F_orderno,sizeof(cOrderno)-1);zip_space(cOrderno);
	memcpy(cOrbrno ,wp_payin->F_orbrno ,sizeof(cOrbrno )-1);zip_space(cOrbrno );
	
	memset(cBrno , 0, sizeof(cBrno));
	memcpy(cBrno   ,lvca.brno, sizeof(cBrno)-1);
	zip_space(cBrno);
	vtcp_log("[%s][%d]payin_txnum==[%.5s]\n",__FILE__,__LINE__,wp_payin->F_txnum);
	vtcp_log("[%s][%d]wtday==[%8s]\n",__FILE__,__LINE__,wp_payin->F_owtday);
	vtcp_log("[%s][%d]orderno==[%.8s]\n",__FILE__,__LINE__,wp_payin->F_oorderno);
	vtcp_log("[%s][%d]F_orbrno==[%.12s]\n",__FILE__,__LINE__,wp_payin->F_acbrno);	
	
	
	vtcp_log("[%s][%d]cBrno==[%.5s]\n",__FILE__,__LINE__,cBrno);
	vtcp_log("[%s][%d]wtday==[%8s]\n",__FILE__,__LINE__,cWtday);
	vtcp_log("[%s][%d]cOrderno==[%.8s]\n",__FILE__,__LINE__,cOrderno);
	vtcp_log("[%s][%d]cOrbrno=[%.12s]\n",__FILE__,__LINE__,cOrbrno);
	memcpy(cOldOpcd, wp_payin->F_opcd,sizeof(cOldOpcd)-1);
	memcpy(cOldPkgno,wp_payin->F_pkgno,sizeof(cOldPkgno)-1);
	memcpy(wp_payin->F_opcd,"60",sizeof(wp_payin->F_opcd));
	if (  !memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		/* ʱʵ���� */
		vtcp_log("[%s][%d]������ʵʱ����ҵ��wp_payin->F_pkgno=[%s]",__FILE__,__LINE__,wp_payin->F_pkgno);
		g_reply_int = sql_unique_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue,16, "pkgno = '%s' and lw_ind = '%c' and br_no = '%s' and  lv_sts = '%c' and or_br_no = '%s'", PKGNO_SSDJ, "2", cBrno, 'I', cOrbrno);
		vtcp_log("[%s][%d]����ʵʱ���ǽ��ձ�����Сδ������� \n",
			__FILE__,__LINE__);
		memcpy(cOldPkgno,PKGNO_SSJJHZ,sizeof(cOldPkgno)-1);
		memcpy(wp_payin->F_pkgno,PKGNO_SSJJ,sizeof(wp_payin->F_pkgno));
	}
	else if (!memcmp(wp_payin->F_pkgno,PKGNO_SSDJHZ,sizeof(wp_payin->F_pkgno)))
	{
		/* ʵʱ���  -- ֻ��һ������*/
		vtcp_log("[%s][%d]������ʵʱ���ҵ��wp_payin->F_pkgno=[%s]",__FILE__,__LINE__,wp_payin->F_pkgno);
		g_reply_int = sql_unique_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue, 16,"pkgno = '%s' and lw_ind = '%c' and br_no = '%s' and lv_sts = '%c' and or_br_no = '%s'", PKGNO_SSJJ, "2", cBrno, 'I', cOrbrno);
		vtcp_log("[%s][%d]this is ����ʵʱ��ǽ��ձ�����Сδ������� \n",
			__FILE__,__LINE__);
		memcpy(cOldPkgno,PKGNO_SSDJHZ,sizeof(cOldPkgno)-1);
		memcpy(wp_payin->F_pkgno,PKGNO_SSDJ,sizeof(wp_payin->F_pkgno));
	}
	else if (!memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		/* ��ͨ��� */
		vtcp_log("[%s][%d]��������ͨ���ҵ��wp_payin->F_pkgno=[%s]",__FILE__,__LINE__,wp_payin->F_pkgno);
		memcpy(cOldPkgno,PKGNO_PTJJHZ,sizeof(cOldPkgno)-1);
		memcpy(wp_payin->F_pkgno,PKGNO_PTJJ,sizeof(wp_payin->F_pkgno));
		if (strlen(cWtday)==0 || strlen(cOrderno)==0)
		{
			/* ֱ���ҵ���ǰ��С��δ��˵�orderno */

			g_reply_int = sql_min_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue,16, "pkgno = '%s' and lw_ind = '%c' and  br_no = '%s' and lv_sts = 'H' and or_br_no = '%s'", PKGNO_PTJJ, "2", cBrno, cOrbrno);
			vtcp_log("[%s][%d]this is ������ͨ��ǽ��ձ�����Сδ������� \n",__FILE__,__LINE__);
		}
		else
		{
			g_reply_int = sql_min_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue,16, "pkgno = '%s' and lw_ind = '2' and wt_date >= '%ld' and br_no = '%s' and lv_sts = 'H' and or_br_no = '%s' and orderno >= '%s'", PKGNO_PTJJ, iWtday, cBrno, cOrbrno, cOrderno);
			vtcp_log("[%s][%d]this is ������ͨ��ǽ��ձ�����Сδ������� \n", __FILE__,__LINE__);
			vtcp_log("[%s][%d]brno=%s,wtday=%ld,orderno=[%s]orbrno=[%s]\n",__FL__,cBrno,iWtday,cOrderno,cOrbrno);
			vtcp_log("[%s][%d] find value==[%s] sqlcode=[%d]\n",__FILE__,__LINE__,cFindValue,g_reply_int);
		}
		vtcp_log("[%s][%d]������ͨ��ǽ��ձ�����Сδ������� \n",__FILE__,__LINE__);
		vtcp_log("[%s][%d]payin_txnum==[%.5s]\n",__FILE__,__LINE__,wp_payin->F_txnum);
	}
	else if (!memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		/* ���ڽ�� */
		vtcp_log("[%s][%d]pkgno==[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
		vtcp_log("[%s][%d]�����Ƕ��ڽ��ҵ��wp_payin->F_pkgno=[%s]",__FILE__,__LINE__,wp_payin->F_pkgno);
		memcpy(cOldPkgno,PKGNO_DQJJHZ,sizeof(cOldPkgno)-1);
		memcpy(wp_payin->F_pkgno,PKGNO_DQJJ,sizeof(wp_payin->F_pkgno));
		if (strlen(cWtday)==0 || strlen(cOrderno)==0)
		{
			g_reply_int = sql_min_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue,16, "pkgno = '%s' and lw_ind = '2' and br_no = '%s'  and lv_sts = 'H' and or_br_no = '%s' ", PKGNO_DQJJ, cBrno, cOrbrno );
			vtcp_log("[%s][%d]pkgno = '%s' and lw_ind = '%c' and br_no = '%s'  and lv_sts = 'H' and or_br_no = '%s' ", __FILE__,__LINE__,PKGNO_DQJJ, "2", cBrno, cOrbrno );
			vtcp_log("[%s][%d]this is �����ڽ�ǽ��ձ�����Сδ������� \n",
				__FILE__,__LINE__);
		}
		else
		{
			g_reply_int = sql_min_string("lv_pkgreg", "to_char(wt_date)||orderno ", cFindValue,16, "pkgno = '%s' and lw_ind = '%c' and wt_date >= '%ld' and br_no = '%s' and  lv_sts = 'H' and or_br_no = '%s' and orderno >= '%s'", PKGNO_DQJJ, '2', iWtday, cBrno,  cOrbrno, cOrderno);
			vtcp_log("[%s][%d]pkgno = '%s' and lw_ind = '%c' and wt_date >= '%ld' and br_no = '%s' and  lv_sts = 'H' and or_br_no = '%s' and orderno >= '%s'", __FILE__,__LINE__,PKGNO_DQJJ, '2', iWtday, cBrno,  cOrbrno, cOrderno);
			vtcp_log("[%s][%d]this is �����ڽ�ǽ��ձ�����Сδ������� \n",
				__FILE__,__LINE__);
		}
		vtcp_log("[%s][%d]�����ڽ�ǽ��ձ�����Сδ�������\n",__FILE__,__LINE__);
	}
	vtcp_log("[%s][%d] find value==[%s] sqlcode=[%d]\n",__FILE__,__LINE__,cFindValue,g_reply_int);
	vtcp_log("[%s][%d]pkgno==[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
	if (g_reply_int!=0)
	{
		vtcp_log("[%s][%d]brno=%s,wtday=%ld,orderno=[%s] \
			orbrno=[%s]\n",
			__FILE__,__LINE__,cBrno,iWtday,cOrderno,cOrbrno);
		if (g_reply_int==DB_NOTFOUND)
		{ 
			vtcp_log("[%s][%d],sqlcode=DB_NOTFOUND,û�з������� \n",__FILE__,__LINE__);
			sprintf(acErrMsg,"�ð���û��С�ڵ��ڵ�ǰ������ŵ�δ������");

		}
		else if (g_reply_int)
		{
			vtcp_log("[%s][%d],sqlcode=[%d],���ݿ���� \n",__FILE__,__LINE__,g_reply_int);
			sprintf(acErrMsg,"���ݿ����");

		}
		return g_reply_int;
	}
	vtcp_log("[%s][%d] find value==[%s] \n",__FILE__,__LINE__,cFindValue);
	zip_space(cFindValue);
	if (strlen(cFindValue)==0) return DB_NOTFOUND;
	memcpy(wp_payin->F_orderno,cFindValue+8,sizeof(wp_payin->F_orderno));
	memcpy(wp_payin->F_wtday,cFindValue,sizeof(wp_payin->F_wtday));
	vtcp_log("[%s][%d]pkgno==[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
	vtcp_log("[%s][%d]opcd==[%s]\n",__FILE__,__LINE__,wp_payin->F_opcd);
	vtcp_log("[%s][%d]orderno==[%s]\n",__FILE__,__LINE__,wp_payin->F_orderno);
	vtcp_log("[%s][%d]wtday==[%s]\n",__FILE__,__LINE__,wp_payin->F_wtday);
	vtcp_log("[%s][%d]orbrno==[%s]\n",__FILE__,__LINE__,wp_payin->F_orbrno);
	vtcp_log("[%s][%d] crdb=[%c] \n",__FILE__,__LINE__,wp_payin->F_crdb[0]);
	iRc=iLvFetchRecToPayin(wp_payin);
	if (iRc)
	{
		vtcp_log("[%s][%d] �õ���С���ʽ�����ݴ��� \n",__FILE__,__LINE__);

		sprintf(acErrMsg,"���ݿ�����ʧ�ܴ���");
		return iRc;
	}
	vtcp_log("[%s][%d] crdb=[%c] \n",__FILE__,__LINE__,wp_payin->F_crdb[0]);
	iLvResetPayin(wp_payin);
	vtcp_log("[%s][%d] crdb=[%c] \n",__FILE__,__LINE__,wp_payin->F_crdb[0]);
	vtcp_log("[%s][%d] pkgno=[%s] \n",__FILE__,__LINE__,wp_payin->F_pkgno);

	memcpy(wp_payin->F_pkgno,cOldPkgno,sizeof(wp_payin->F_pkgno));
	vtcp_log("[%s][%d] pkgno=[%s] \n",__FILE__,__LINE__,wp_payin->F_pkgno);
	memcpy(wp_payin->F_opcd,cOldOpcd,sizeof(wp_payin->F_opcd));
	return 0;
}
/***************************************************************************/
/* ��������: int iUpdRecvTableStat(PAY_IN_AREA *wp_payin)                  */
/* ��������: ����PAYIN�ṹ����±��ֵ                                     */
/* ��������: 2005.07.20                                                    */
/* ����:     SSH                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
int iUpdRecvTableStat(PAY_IN_AREA *wp_payin)
{
	int iRc=0;
	vtcp_log("[%s][%d]=wp_payin->F_pkgno=[%s]\n",__FILE__,__LINE__,wp_payin->F_pkgno);
	if (!memcmp(wp_payin->F_pkgno,PKGNO_PTJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		struct lv_pkgreg_c wd_lv_ptjjr;
		memset(&wd_lv_ptjjr , 0 ,sizeof(wd_lv_ptjjr));
		wd_lv_ptjjr.wt_date = apatoi(wp_payin->F_owtday,8);
		memcpy(wd_lv_ptjjr.or_br_no,wp_payin->F_acbrno,sizeof(wd_lv_ptjjr.or_br_no)-1);
		memcpy(wd_lv_ptjjr.orderno,wp_payin->F_oorderno,sizeof(wd_lv_ptjjr.orderno)-1);
		vtcp_log("[%s][%d]wd_lv_ptjjr.orderno=[%s]",__FILE__,__LINE__,wd_lv_ptjjr.orderno);
		vtcp_log("[%s][%d]wd_lv_ptjjr.or_br_no=[%s]",__FILE__,__LINE__,wd_lv_ptjjr.or_br_no);
		vtcp_log("[%s][%d]wd_lv_ptjjr.wt_date=[%d]",__FILE__,__LINE__,wd_lv_ptjjr.wt_date);
		if (g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "or_br_no = '%s' and wt_date = '%ld' and orderno = '%s' and pkgno = '%s' and lw_ind = '%s' ", wd_lv_ptjjr.or_br_no, wd_lv_ptjjr.wt_date, wd_lv_ptjjr.orderno, PKGNO_PTJJ, "2"))
		{
			sprintf(acErrMsg,"�����α���� - %d", g_reply_int);
			return(-1);
		}
		if ((g_reply_int = Lv_pkgreg_Fet_Upd(&wd_lv_ptjjr, g_pub_tx.reply)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,g_reply_int);
			Lv_pkgreg_Debug(&wd_lv_ptjjr);
			sprintf(acErrMsg,"δ�ҵ���ͨ���ҵ���¼");

			return(-1);
		}
		/**add by wangyongwei 20070828**/
		if (!memcmp(wp_payin->F_opcd,"10",sizeof(wp_payin->F_opcd))
			||!memcmp(wp_payin->F_opcd,"30",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_ptjjr.lv_sts[0]=STAT_NO_NOTSEND;
			else
				wd_lv_ptjjr.lv_sts[0]=STAT_YES_NOTSEND;
		}
		else if (!memcmp(wp_payin->F_opcd,"40",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_ptjjr.lv_sts[0]=STAT_NO_SEND;
			else
				wd_lv_ptjjr.lv_sts[0]=STAT_YES_SEND;		
		}
		memcpy(wd_lv_ptjjr.o_or_br_no,wp_payin->F_orbrno  ,sizeof(wp_payin->F_orbrno));
		memcpy(wd_lv_ptjjr.o_orderno ,wp_payin->F_orderno ,sizeof(wp_payin->F_orderno));
		wd_lv_ptjjr.o_wt_date = apatoi(wp_payin->F_wtday,8);
		if (DB_OK == (iRc=Lv_pkgreg_Upd_Upd(wd_lv_ptjjr, g_pub_tx.reply)))
			iRc = Lv_pkgreg_Clo_Upd();
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
			Lv_pkgreg_Debug(&wd_lv_ptjjr);
			sprintf(acErrMsg,"δ�ҵ���ͨ���ҵ���¼");
			return(-1);
		}
	}
	else if (!memcmp(wp_payin->F_pkgno,PKGNO_DQJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		struct lv_pkgreg_c wd_lv_dqjjr;
		memset(&wd_lv_dqjjr , 0 ,sizeof(wd_lv_dqjjr));
		wd_lv_dqjjr.wt_date = apatoi(wp_payin->F_owtday,8);
		memcpy(wd_lv_dqjjr.or_br_no,wp_payin->F_acbrno,sizeof(wd_lv_dqjjr.or_br_no)-1);
		memcpy(wd_lv_dqjjr.orderno,wp_payin->F_oorderno,sizeof(wd_lv_dqjjr.orderno)-1);

		if (g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "or_br_no = '%s' and wt_date = '%ld' and orderno = '%s' and pkgno = '%s' and lw_ind = '%s' ", wd_lv_dqjjr.or_br_no, wd_lv_dqjjr.wt_date, wd_lv_dqjjr.orderno, PKGNO_DQJJ, "2"))
		{
			sprintf(acErrMsg,"�����α���� - %d", g_reply_int);
			return(-1);
		}
		if ((g_reply_int = Lv_pkgreg_Fet_Upd(&wd_lv_dqjjr, g_pub_tx.reply)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,g_reply_int);
			Lv_pkgreg_Debug(&wd_lv_dqjjr);
			sprintf(acErrMsg,"δ�ҵ����ڽ��ҵ���¼");
			return(-1);
		}
		if (!memcmp(wp_payin->F_opcd,"10",sizeof(wp_payin->F_opcd))
			||!memcmp(wp_payin->F_opcd,"30",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_dqjjr.lv_sts[0]=STAT_NO_NOTSEND;
			else
				wd_lv_dqjjr.lv_sts[0]=STAT_YES_NOTSEND;
		}
		else if (!memcmp(wp_payin->F_opcd,"40",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_dqjjr.lv_sts[0]=STAT_NO_SEND;
			else
				wd_lv_dqjjr.lv_sts[0]=STAT_YES_SEND;
		}
		memcpy(wd_lv_dqjjr.o_or_br_no,wp_payin->F_orbrno  ,sizeof(wp_payin->F_orbrno));
		memcpy(wd_lv_dqjjr.o_orderno ,wp_payin->F_orderno ,sizeof(wp_payin->F_orderno));
		wd_lv_dqjjr.o_wt_date = apatoi(wp_payin->F_wtday,8);
		if (DB_OK == (iRc=Lv_pkgreg_Upd_Upd(wd_lv_dqjjr, g_pub_tx.reply)))
			iRc = Lv_pkgreg_Clo_Upd();
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
			Lv_pkgreg_Debug(&wd_lv_dqjjr);
			sprintf(acErrMsg,"δ�ҵ����ڽ��ҵ���¼");
			return(-1);
		}
	}
	else if (!memcmp(wp_payin->F_pkgno,PKGNO_SSJJHZ,sizeof(wp_payin->F_pkgno)))
	{
		struct lv_pkgreg_c wd_lv_ssjjr;
		memset(&wd_lv_ssjjr , 0 ,sizeof(wd_lv_ssjjr));
		wd_lv_ssjjr.wt_date = apatoi(wp_payin->F_wtday,8);
		memcpy(wd_lv_ssjjr.or_br_no,wp_payin->F_orbrno,sizeof(wd_lv_ssjjr.or_br_no)-1);
		memcpy(wd_lv_ssjjr.orderno,wp_payin->F_orderno,sizeof(wd_lv_ssjjr.orderno)-1);

		if (g_reply_int = Lv_pkgreg_Dec_Upd(g_pub_tx.reply, "or_br_no = '%s' and wt_date = '%ld' and orderno = '%s' and pkgno = '%s' and lw_ind = '%s' ", wd_lv_ssjjr.ac_br_no, wd_lv_ssjjr.o_wt_date, wd_lv_ssjjr.o_orderno, PKGNO_SSJJ, "2"))
		{
			sprintf(acErrMsg,"�����α���� - %d", g_reply_int);
			return(-1);
		}
		if ((g_reply_int = Lv_pkgreg_Fet_Upd(&wd_lv_ssjjr, g_pub_tx.reply)) != DB_OK)
		{
			vtcp_log("[%s][%d]-----lv_wbctl error [%d]\n",__FILE__,__LINE__,g_reply_int);
			Lv_pkgreg_Debug(&wd_lv_ssjjr);

			sprintf(acErrMsg,"δ�ҵ�ʵʱ���ҵ���¼");
			return(-1);
		}
		if (!memcmp(wp_payin->F_opcd,"10",sizeof(wp_payin->F_opcd))
			||!memcmp(wp_payin->F_opcd,"30",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_ssjjr.lv_sts[0]=STAT_NO_NOTSEND;
			else
				wd_lv_ssjjr.lv_sts[0]=STAT_YES_NOTSEND;
		}
		else if (!memcmp(wp_payin->F_opcd,"40",sizeof(wp_payin->F_opcd)))
		{
			if (memcmp(wp_payin->F_rcpstat,"00",sizeof(wp_payin->F_rcpstat)))
				wd_lv_ssjjr.lv_sts[0]=STAT_NO_SEND;
			else
				wd_lv_ssjjr.lv_sts[0]=STAT_YES_SEND;
		}
		if (DB_OK == (iRc=Lv_pkgreg_Upd_Upd(wd_lv_ssjjr, g_pub_tx.reply)))
			iRc = Lv_pkgreg_Clo_Upd();
		if (iRc!=DB_OK)
		{
			vtcp_log("%s,%d,Find error,iRc=%d\n",__FILE__,__LINE__,iRc);
			Lv_pkgreg_Debug(&wd_lv_ssjjr);
			sprintf(acErrMsg,"δ�ҵ�ʵʱ���ҵ���¼");
			return(-1);
		}
	}
	return 0;
} 
/******************************************************************************/
/* ��������: int	iLvPayinCheck10(PAY_IN_AREA *payin_lz,PAY_IN_AREA *payin_th)*/
/* ��������: �˻�ҵ��ר�ý���                                                 */
/* ��������: 2006-9-20 20:28                                                  */
/* ����:     xyy                                                              */
/* ˵��:                                                                      */
/******************************************************************************/
int	iLvPayinCheck10(PAY_IN_AREA *payin_lz,PAY_IN_AREA *payin_th)
{
	/****��ԭ���ձ����ҵ���������ҵ��****/
	/****ֻ�ȽϽ��ʺź��к��Ƿ����,�Ƿ񱾻�������,״̬�Ƿ����****/
	char cKinbr[BRNO_LEN+1];
	memset(cKinbr,0,sizeof(cKinbr));
	vtcp_log("[%s][%d]T_pkgno=[%s],txnum=[%s]\n",__FL__,payin_lz->T_pkgno,payin_lz->T_txnum);	
	if(payin_lz->T_stat[0]!=payin_lz->T_begstat[0])
	{
		vtcp_log("%s,%d,ԭ����״̬[%c][%c]���������˽���n",__FL__,\
			payin_lz->T_stat[0],payin_lz->T_begstat[0]);
		sprintf( acErrMsg,"[%s][%d]ԭ������������������,�������˻�!",__FL__);
		set_zd_data(DC_GET_MSG,"ԭ������������������,�������˻�");
		WRITEMSG
		return(-1);
	}
	get_zd_data(DC_TX_BR_NO  ,cKinbr);
	if(memcmp(payin_lz->T_brno,cKinbr,BRNO_LEN)!=0)
	{
		vtcp_log("[%s][%d],payin_lz->T_brno=[%s] cKinbr=[%s]ԭ���˷Ǳ���������",\
				__FL__,payin_lz->T_brno,cKinbr);
		sprintf( acErrMsg,"[%s][%d]ԭ���˷Ǳ���������!",__FL__);
		set_zd_data(DC_GET_MSG,"ԭ���˷Ǳ���������");
		WRITEMSG
		return(-1);
	}
	if(payin_lz->T_stat[0]==STAT_LZRECV2)
	{
		vtcp_log("[%s],[%d],payin_lz->T_stat[0]=[%c] �������ʲ������˻㽻��",__FL__,\
			payin_lz->T_stat[0]);
		sprintf( acErrMsg,"[%s][%d]�������ʲ������˻㽻��!",__FL__);
		set_zd_data(DC_GET_MSG,"�������ʲ������˻㽻��");
		WRITEMSG
		return(-1);
	}
	if(iAmtCmp(payin_lz->F_txamt,payin_th->F_txamt,	sizeof(payin_lz->F_txamt))!=0)
	{
		vtcp_log("%s,%d,���׽���[%s][%s]",__FL__,payin_lz->F_txamt,payin_th->F_txamt);
		sprintf( acErrMsg,"[%s][%d]���׽���!",__FL__);
		set_zd_data(DC_GET_MSG,"���׽���");
		WRITEMSG
		return(-1);
	}
	if(memcmp(payin_lz->F_orbrno,payin_th->F_acbrno,sizeof(payin_th->F_acbrno))!=0)
	{
		vtcp_log("%s,%d,ԭ���������˻�����в�ͬ[%s][%s]",__FL__,\
			payin_lz->F_orbrno,payin_th->F_acbrno);
		set_zd_data(DC_GET_MSG,"ԭ�����кŴ���");
		return(-1);
	}
	if(memcmp(payin_lz->F_payactno,payin_th->F_cashactno,sizeof(payin_lz->F_payactno))!=0)
	{
		vtcp_log("%s,%d,�������ʺŲ���[%s][%s]",__FL__,\
			payin_lz->F_payactno,payin_th->F_cashactno);
		sprintf( acErrMsg,"[%s][%d]�������ʺŲ���!",__FL__);
		set_zd_data(DC_GET_MSG,"�������ʺŲ���");
		WRITEMSG
		return(-1);
	}
	if(memcmp(payin_lz->F_cashactno,payin_th->F_payactno,sizeof(payin_lz->F_cashactno))!=0)
	{
		vtcp_log("%s,%d,�տ����ʺŲ���[%s][%s]",__FL__,\
			payin_lz->F_cashactno,payin_th->F_payactno);
		sprintf( acErrMsg,"[%s][%d]�տ����ʺŲ���!",__FL__);
		set_zd_data(DC_GET_MSG,"�տ����ʺŲ���");
		WRITEMSG
		return(-1);
	}
	return 0;
}	
/***************************************************************************/
/* ��������: iDateCmp                                                      */
/* ��������: �Ƚ�������������                                              */
/* ����:                                                                   */
/*            pcNum1   -��1����������                                      */
/*            pcNum2   -��2����������                                      */
/*            iSize    -�Ƚϳ���                                           */
/* ����ֵ:   0:���  1:pcNum1>pcNum2 -1:pcNum1<pcNum2                      */
/* ��������: 2006-9-22 19:40                                               */
/* ����:     xyy                                                           */
/* ˵��:                                                                   */
/***************************************************************************/
int iDateCmp(char *pcNum1,char *pcNum2,int iSize)
{
    int i;
    char c1,c2;
    if (pcNum1[0]=='\0')
    	memset(pcNum1,'\0',iSize);
    if (pcNum2[0]=='\0')
    	memset(pcNum2,'\0',iSize);

    for (i=0;i<iSize;i++)
    {
    	c1=pcNum1[i];
    	c2=pcNum2[i];
    	if (c1==' ' || c1=='\0')
    		c1='0';
    	if (c2==' ' || c2=='\0')
    		c2='0';
    	if (c1>c2)
    	{
    		return(1);
    	}else if(c1<c2)
    	{
    		return(-1);
    	}
    }
    return(0);
}
int iLvPrepareTh_ac_no(PAY_IN_AREA *payin)
{
    char cAc_no[33];
    memset(cAc_no, 0 , sizeof(cAc_no));
    vtcp_log("[%s],[%d] ��ʼ�����˻��Ŀpkgno=[%.3s] ",__FILE__,__LINE__,payin->F_pkgno);
    if(memcmp(payin->F_pkgno,PKGNO_DJTH,sizeof(payin->F_pkgno)))
        return -1;
    strncpy(cAc_no,"26202",5);
    set_zd_data("0300",cAc_no);
    set_zd_data("1021",cAc_no);
    vtcp_log("%s,%d cAc_no===[%s] ",__FILE__,__LINE__,cAc_no);
    return 0;
}
int iLv_Reb_Bp(char *txday,char *addid) /**YHBP������б�Ʊ��Ϣ**/
{
    int iRc=0;
    double dCpamt=0;
    struct lv_addit_c wd_lv_addit;
    struct cashier_c wd_cashier;
    char cBuff[2001];

    memset(cBuff      , 0 , sizeof(cBuff));
    memset(&wd_cashier, 0 , sizeof(wd_cashier));
    memset(&wd_lv_addit,'\0',sizeof(wd_lv_addit));

    printf("[%s][%d]\n",__FILE__,__LINE__);

    wd_lv_addit.tx_date = apatoi(txday,8);
    printf("[%s][%d]wd_lv_addit.tx_date=[%d]\n",__FILE__,__LINE__,wd_lv_addit.tx_date);
    memcpy(wd_lv_addit.addid,addid,sizeof(wd_lv_addit.addid)-1);
    printf("[%s][%d]wd_lv_addit.addid=[%s]\n",__FILE__,__LINE__,wd_lv_addit.addid);
    memcpy(wd_lv_addit.tagname,"001",sizeof(wd_lv_addit.tagname)-1);
    printf("[%s][%d]wd_lv_addit.tagname=[%s]\n",__FILE__,__LINE__,wd_lv_addit.tagname);

    g_reply_int = Lv_addit_Sel(g_pub_tx.reply,&wd_lv_addit,"tagname='001' and tx_date='%ld' and addid='%s'",wd_lv_addit.tx_date,wd_lv_addit.addid);
    if(g_reply_int)
    {
        vtcp_log("[%s][%d],��ѯLv_addit��ʧ��[%d]",__FILE__,__LINE__, g_reply_int);
        sprintf(acErrMsg,"��ѯLv_addit��ʧ��[%s][%d]",__FILE__,__LINE__);
        set_zd_data(DC_GET_MSG,acErrMsg);
        return(g_reply_int);
    }
    else
    {
        printf("%s,%d ��������Ϣ���������\n",__FILE__,__LINE__);
        wd_cashier.signday = apatoi(wd_lv_addit.tagdata,sizeof(wd_cashier.signday)-1);
        memcpy(wd_cashier.pono ,wd_lv_addit.tagdata+8,sizeof(wd_cashier.pono)-1);
        printf("%s,%d pono=[%s]signday=[%ld]\n",__FILE__,__LINE__,wd_cashier.pono,wd_cashier.signday);
    }
    
    iRc = Cashier_Dec_Upd(g_pub_tx.reply,"pono='%s' and signday='%ld' ",\
                                     wd_cashier.pono,wd_cashier.signday);
    if (iRc)
    {
        vtcp_log("%s,%d,���ұ�Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"�����ִ�Ǽǲ�ʧ��");
        WRITEMSG
        return(iRc);
    }
    vtcp_log("[%s][%d]׼��FETCH ������[%d]\n",__FILE__,__LINE__,iRc);
    iRc = Cashier_Fet_Upd(&wd_cashier,g_pub_tx.reply);
    if (iRc!=0 && iRc!=100)
    {
        vtcp_log("%s,%d,FET��Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"FET��Ʊ������Ϣ�����");
        WRITEMSG
        return(iRc);
     }
     else if(iRc == 100){
        vtcp_log("%s,%d,û�������Ʊ��¼[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"û�������Ʊ��¼");
        WRITEMSG
        return(iRc);
    }

    printf("[%s][%d]Ʊ��״̬Ϊ[%s]\n",__FILE__,__LINE__,wd_cashier.stat);

    wd_cashier.stat[0] = '1';   /* ��Ʊ��ǰ״̬��־ 0-�Ǽ� 1-ǩ�� */

    vtcp_log("[%s][%d]׼��UPDATE������\n",__FILE__,__LINE__);
    iRc = Cashier_Upd_Upd(wd_cashier,g_pub_tx.reply);
    if (iRc)
    {
        vtcp_log("%s,%d,���±�Ʊ������Ϣ�����[%d]\n",__FILE__,__LINE__,iRc);
        sprintf( acErrMsg,"���±�Ʊ������Ϣ�����");
        WRITEMSG
        return(iRc);
     }
     Cashier_Clo_Upd();
  
     return (0);
}
/****���㱾Ʊ��Ѻ add by YH_BP***/
/****����Ҫ��:
     ǩ���к�:12λ��������
     �����˺�:32λ����/��ĸ,����32λ�Ҳ��ո�
     ������:�Է�Ϊ��λ
     ��Ʊ����:20λ����,����20λ��0 
     ǩ������:8λ����
****/
int nCountBpMac(char *pcPayNo,char *pcPayActNo,double dTxamt,char *pcVocNo,char *pcSignDay,char *pcMac){
	char sKey[33];	
	char sBinKey[17];	
	char sMacBlock[128];
	char sMac[128];
	char sTmp[64];
	int  i,j;
	memset(sKey,'\0',sizeof(sKey));
	memset(sBinKey,'\0',sizeof(sBinKey));
	memset(sMacBlock,'\0',sizeof(sMacBlock));
	memset(sMac,'\0',sizeof(sMac));
	memset(sTmp,'\0',sizeof(sTmp));
	sprintf(sKey,"01234567890ABCDEF%8s98765432",pcSignDay);	
	iAscToBcd(sKey,sBinKey,32);	
	des2key(sBinKey,0);
	sprintf(sMacBlock,"%012s%32s%012.0f%020s0000",pcPayNo,pcPayActNo,dTxamt,pcVocNo);
	for(i=0;i<40;i++){
		sTmp[i]=(sMacBlock[i*2] & 0x0f)<<4 | (sMacBlock[i*2+1] & 0x0f);
	}
	memcpy(sMacBlock,sTmp,40);
	Ddes(sMacBlock,sTmp);	
	memcpy(sMac,sTmp,8);
	for(i=1;i<5;i++){
		Ddes(sMacBlock+i*8,sTmp);	
		for(j=0;j<8;j++){
			sMac[j]=sMac[j]^sTmp[j];
		}	
	}
	sMac[8]=sMac[0]^sMac[2]^sMac[4]^sMac[6];
	sMac[9]=sMac[1]^sMac[3]^sMac[5]^sMac[7];
	iBcdToAsc(sMac,sTmp,10);
	for(i=0;i<20;i++){
		if(sTmp[i]>='A' && sTmp[i]<='F'){
			pcMac[i]=sTmp[i]-'A'+'0';
		}else{
			pcMac[i]=sTmp[i];
		}
	}
	return(0);
}
/**** for test nCountBpMac
int main(){
	char sTmp[21];
	memset(sTmp,'\0',sizeof(sTmp));
	nCountBpMac("401148053964","10101",123.0,"8888","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053961","10101",123.0,"8888","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053981","10101",123.0,"8888","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053964","10101",123.0,"8888","20080201",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053961","10101",123.0,"8889","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053961","10101",124.0,"8889","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053961","10102",124.0,"8889","20080101",sTmp);
	printf("[%s]\n",sTmp);
	nCountBpMac("401148053964","10101",123.0,"8888","20080101",sTmp);
	printf("[%s]\n",sTmp);
	return(0);
}
**end by YH_BP**/



/*******************************************************************/
/**	��������:iLv_UpdateSSJJLZStat(PAY_IN_AREA *wp_payin_ssjjhz)   **/
/** ����:֧�����׽ṹ��                                           **/
/** ����:����ʵʱ���ҵ���ִ�ҵ�ԭʼʵʱ�������ҵ�񲢸���Ϊ�ܾ� **/
/**      ����״̬Ϊ��������ͬ��Ӧ����δ���ͻ�����ͬ��Ӧ�����ѷ��� **/
/** ����:0�ɹ� ����Ϊʧ��                                         **/
/** ��д:	xyy 2007-12-31 23:11    NEWBP1                          **/
/*******************************************************************/
int iLv_UpdateSSJJLZStat(PAY_IN_AREA * wp_payin_ssjjhz)
{
	PAY_IN_AREA wd_payin_or;
	int iRc=0;
	char cOr_br_no[13];
	char cOrderno[9];
	char cWt_date[9];
	
	iRc=0;
	memset(&wd_payin_or, 0 , sizeof(wd_payin_or));
	memset(cOr_br_no, 0 , sizeof(cOr_br_no));
	memset(cOrderno, 0 , sizeof(cOrderno));
	memset(cWt_date, 0 , sizeof(cWt_date));
			
	vtcp_log("[%s][%d] wp_payin_ssjjhz->T_stat ===[%.1s] ",__FILE__,__LINE__, wp_payin_ssjjhz->T_stat);
	vtcp_log("[%s][%d] wp_payin_ssjjhz->F_owtday ===[%.8s] ",__FILE__,__LINE__, wp_payin_ssjjhz->F_owtday);
	vtcp_log("[%s][%d] wp_payin_ssjjhz->F_oorderno ===[%.8s] ",__FILE__,__LINE__, wp_payin_ssjjhz->F_oorderno);
	vtcp_log("[%s][%d] wp_payin_ssjjhz->F_acbrno ===[%.12s] ",__FILE__,__LINE__, wp_payin_ssjjhz->F_acbrno);
	vtcp_log("[%s][%d] wp_payin_ssjjhz->F_o_or_br_no ===[%.12s] ",__FILE__,__LINE__, wp_payin_ssjjhz->F_o_or_br_no);
	vtcp_log("[%s][%d] wp_payin_ssjjhz->F_otxnum ===[%.3s] ",__FILE__,__LINE__, wp_payin_ssjjhz->F_otxnum);

	memcpy(wd_payin_or.F_wtday,wp_payin_ssjjhz->F_owtday,sizeof(wd_payin_or.F_owtday));
	memcpy(wd_payin_or.F_orderno,wp_payin_ssjjhz->F_oorderno,sizeof(wd_payin_or.F_orderno));
	memcpy(wd_payin_or.F_orbrno,wp_payin_ssjjhz->F_o_or_br_no,sizeof(wd_payin_or.F_o_or_br_no));
  memcpy(wd_payin_or.F_pkgno,PKGNO_SSJJ,sizeof(wd_payin_or.F_pkgno));
  

	vtcp_log("[%s][%d] wd_payin_or.F_wtday ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_wtday);
	vtcp_log("[%s][%d] wd_payin_or.F_orderno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orderno);
	vtcp_log("[%s][%d] wd_payin_or.F_orbrno ===[%.8s] ",__FILE__,__LINE__, wd_payin_or.F_orbrno);
	vtcp_log("[%s][%d] wd_payin_or.F_otxnum ===[%.3s] ",__FILE__,__LINE__, wd_payin_or.F_otxnum);

	iRc=iLvFetchRecToPayin(&wd_payin_or);
	if (iRc)
	{
		vtcp_log("[%s][%d]����ԭ����ʵʱ������ʱ����sqlcode=%d\n",__FILE__,__LINE__,iRc);
		return iRc;
	}
	vtcp_log("[%s][%d] === After iLvFetchRecToPayin() !! === ",__FILE__,__LINE__);

	iLvResetPayin(&wd_payin_or);

	vtcp_log("[%s][%d] === After iLvResetPayin() !! === ",__FILE__,__LINE__);

	vtcp_log("[%s][%d] wd_payin_or.T_stat ===[%.1s] ",__FILE__,__LINE__, wd_payin_or.T_stat);

	if (wd_payin_or.T_stat[0] == STAT_YES_NOTSEND || wd_payin_or.T_stat[0] == STAT_YES_SEND)
	{
		wd_payin_or.T_stat[0] = STAT_LZCZ;

		memcpy(cOr_br_no, wd_payin_or.T_orbrno, sizeof(cOr_br_no)-1);
		memcpy(cOrderno, wd_payin_or.T_orderno, sizeof(cOrderno)-1);
		memcpy(cWt_date, wd_payin_or.T_wtday, sizeof(cWt_date)-1);

		/*  ����ԭ����ʵʱ������ʼ�¼״̬  */
		g_reply_int = sql_execute(" update lv_pkgreg set lv_sts='%c' where lw_ind = '2' and wt_date=%d and or_br_no='%s' and orderno='%s'", STAT_NO_SEND, apatoi(cWt_date,8), cOr_br_no, cOrderno);
		if ( g_reply_int != DB_OK )
		{
			sprintf( acErrMsg,"�������ݿ�ʧ�� ");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			return(-1);
		}
		vtcp_log("[%s][%d] ����ԭ�������ʼ�¼״̬�ɹ� !! \n",__FILE__,__LINE__);
		return 0;
	}
	else
    return 0;
}