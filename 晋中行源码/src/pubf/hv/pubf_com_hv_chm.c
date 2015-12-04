#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#include "find_debug.h"
#define EXTERN
/**
#include "mbfe_pub.h"
#include "public.h"
#include "hv_pub.h"
#include "hvnet.h"
#include "errlog.h"
***/
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "lv_define.h"
#include "prdt_ac_id_c.h"
#include "hv_poinfo_c.h"

#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
int iGetHvOrderno(char * orderno);
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);

/****************************************************************************/
/* ��������: iGetSendco(char * or_br_no,char * sendco)                      */
/* ��������: ����֧��ϵͳ�кŵõ�(����/�ձ�)���Ĵ���                        */
/* ��������: 2006-8-12 19:22                                                */
/* ����:     ChenMing                                                       */
/****************************************************************************/ 
int iGetSendco( char *sendco, char *or_br_no)
{
	int ret=0;
	char cOr_br_no[13];
	struct hv_uniontab_c hv_uniontab; 
	memset(cOr_br_no , 0, sizeof(cOr_br_no));
	memset( &hv_uniontab, 0x00, sizeof(hv_uniontab) );   
	
	memcpy(cOr_br_no, or_br_no, sizeof(cOr_br_no)-1);

	ret = Hv_uniontab_Sel( g_pub_tx.reply, &hv_uniontab, "or_br_no = '%s' ", cOr_br_no );
	if(ret)
	{
		sprintf(acErrMsg,"find hvuniontab  ret= [%d]",ret);
		WRITEMSG
		return(ret);
	}
	memcpy(sendco,hv_uniontab.sendco,sizeof(hv_uniontab.sendco)-1);	
	return 0;
}

/****************************************************************************/
/* ��������: iGet_full_hv_no(char * or_br_no,char * sendco,char * qs_br_no) */
/* ��������: ����֧��ϵͳ�кŵõ�(����/�ձ�)���Ĵ���                        */
/* ��������: 2006-8-12 19:22                                                */
/* ����:     ChenMing                                                       */
/****************************************************************************/ 
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no)
{
	int ret=0;
	struct hv_uniontab_c hv_uniontab;    
	char cOrbrno[13];
	memset(cOrbrno, 0 , sizeof(cOrbrno));
	memcpy(cOrbrno,or_br_no,sizeof(cOrbrno)-1);
	zip_space(cOrbrno);
	if  (strlen(cOrbrno) == 0 )
		return 1;
	memset( &hv_uniontab, 0x00, sizeof(hv_uniontab) );    
	ret = Hv_uniontab_Sel( g_pub_tx.reply, &hv_uniontab, "or_br_no = '%s' ", cOrbrno );
	if(ret)
	{
		sprintf(acErrMsg,"find hvuniontab error ret=[%d] or_brno=[%s]",ret,or_br_no);
		WRITEMSG
		return(ret);
	}
	memcpy(sendco,hv_uniontab.sendco,sizeof(hv_uniontab.sendco)-1);
	memcpy(qs_br_no,hv_uniontab.qs_no,sizeof(hv_uniontab.qs_no)-1);		
	sprintf(acErrMsg,"sendco=[%s] or_brno=[%s]",hv_uniontab.sendco,hv_uniontab.qs_no);
	WRITEMSG
	sprintf(acErrMsg,"sendco=[%s] or_brno=[%s]",sendco,qs_br_no);
	WRITEMSG
	return ret;
}

/****************************************************************************/
/* ��������: int GetOr_br_noByBr_no(char *br_no,char *or_br_no)             */
/* ��������: ���� �������� ���� ֧��ϵͳ�к�                                */
/* ��������: 2006.08.08                                                     */
/* ����:     ChenMing                                                       */
/* ˵��:     ���� �������� ���� ֧��ϵͳ�к�                                */
/****************************************************************************/ 
int GetOr_br_noByBr_no(char *br_no, char *or_br_no )
{
  int ret=0;
	char cBrno[BRNO_LEN+1];
	struct hv_brno_orno_c hv_brno_orno;
	memset(cBrno, 0 , sizeof(cBrno));
	memset( &hv_brno_orno, 0x00, sizeof(hv_brno_orno) );  
	memcpy(cBrno,br_no,BRNO_LEN);
	zip_space(cBrno);
  ret = Hv_brno_orno_Sel( g_pub_tx.reply, &hv_brno_orno, "br_no = '%s' ", cBrno );
  if ( ret!=0  && ret!=100)
	{
		sprintf(acErrMsg,"����hv_brno_orno����[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		return(ret);
	}	
	memcpy(or_br_no,hv_brno_orno.or_br_no,sizeof(hv_brno_orno.or_br_no)-1);
  return 0;
}
/****************************************************************************/
/* ��������: int GetBr_noByOr_br_no(char *br_no, char *or_br_no )           */
/* ��������: ���� ֧��ϵͳ�к� ���� ��������                                */
/* ��������: 2006.08.08                                                     */
/* ����:     ChenMing                                                       */
/* ˵��:     ���� ֧��ϵͳ�к� ���� ��������                                */
/****************************************************************************/ 
int GetBr_noByOr_br_no(char *or_br_no, char *br_no )
{
    int ret=0;
    char cOr_br_no[13];
    struct hv_orno_brno_c hv_orno_brno;   
    memset( cOr_br_no,0,sizeof(cOr_br_no));
    memset( &hv_orno_brno, 0x00, sizeof(hv_orno_brno) );        
    
    memcpy(cOr_br_no,or_br_no, sizeof(cOr_br_no)-1);
    zip_space(cOr_br_no);
    /* add by LiuHuafeng 20070501 */
    if(strlen(cOr_br_no)==0)
    {
        memcpy(cOr_br_no,HV_QS_BR_NO,sizeof(cOr_br_no)-1);
        vtcp_log("%s,%d �����֧��ϵͳ����Ϊ������Ϊ�������к�",__FILE__,__LINE__);
    }

    ret = Hv_orno_brno_Sel( g_pub_tx.reply, &hv_orno_brno, "or_br_no = '%s' ", cOr_br_no );
    if ( ret!=0  && ret!=100)
    {
        sprintf(acErrMsg,"����hv_brno_orno����[%d]",ret);
        WRITEMSG
        strcpy(g_pub_tx.reply,"D103");
        return(ret);
    }	    
    memcpy(br_no,hv_orno_brno.br_no,BRNO_LEN);
    return 0;
}
/****************************************************************************/
/* ��������: int FetchRecToPayin(PAY_IN_AREA *hv_payin)                     */
/* ��������: ����payin����������ȡһ�����֧����¼                          */
/* ��������: 2006.08.08                                                     */
/* ����:     ChenMing                                                       */
/* ˵��:                                                                    */
/**     ��Ϊ���ô˺���ʱ�����벿�� F_xx �Ѿ���ֵ�������¼����û������      */
/**     ���������������Ĺؼ����������õ���������                          */
/**     �˺���ʹ��FIND,��Ȼ�ڸ���ʱ���ܶ������һ�α�������������Ŀ���   */
/**     ȱ���Ƕ������ͬʱ��ͼ�޸�ͬһ����¼ʱ�ᷢ����һ�µ����⣬�����Ա���*/
/****************************************************************************/
int FetchRecToPayin(HV_PAYIN_AREA *hv_payin)
{
	int iRc;
	char cTlrno[5];
	struct hv_zf_c hv_zf;
	vtcp_log("%s,%d hv_payin->F_or_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_or_br_no);
	memset(&hv_zf,'\0',sizeof(hv_zf));
	memset(cTlrno,0,sizeof(cTlrno));
	get_fd_data("0070",cTlrno);
	memcpy(hv_zf.cmtno,hv_payin->F_cmtno,sizeof(hv_zf.cmtno)-1);		
	memcpy(hv_zf.or_br_no,hv_payin->F_or_br_no,sizeof(hv_zf.or_br_no)-1);	
	if (strlen(hv_payin->F_wt_date) > 0)	   hv_zf.wt_date = apatoi(hv_payin->F_wt_date,8);
	memcpy(hv_zf.orderno,hv_payin->F_orderno,sizeof(hv_zf.orderno)-1);	
	vtcp_log("%s,%d hv_payin->F_orderno************=[%s]\n",__FILE__,__LINE__,hv_payin->F_orderno);	
	iRc = Hv_zf_Sel(g_pub_tx.reply,&hv_zf, "cmtno='%s' and orderno='%s' and or_br_no='%s' and wt_date=%d ", \ 
		hv_zf.cmtno,hv_zf.orderno,hv_zf.or_br_no,hv_zf.wt_date);
	if ( iRc==100 )
	{
		sprintf(acErrMsg,"δ�ҵ����֧������ҵ���¼");
		strncpy( g_pub_tx.reply,"H031",4);
		WRITEMSG
		/* strcpy( g_pub_tx.reply, "MBFE" );*/
			return iRc; 
	}
	else if ( iRc )
	{
		sprintf(acErrMsg,"��ѯ���֧�����׼�¼����[%d]",iRc);
		strncpy( g_pub_tx.reply,"H031",4);
		WRITEMSG
			return iRc;
	}
	vtcp_log("[%s][%d]cTlrno=[%s]",__FILE__,__LINE__,cTlrno);
	vtcp_log("[%s][%d]hv_zf.tel=[%s]",__FILE__,__LINE__,hv_zf.tel);
	vtcp_log("[%s][%d]hv_payin->F_opcd=[%s]",__FILE__,__LINE__,hv_payin->F_opcd);
	if(!memcmp(hv_payin->F_opcd,"30",sizeof(hv_payin->F_opcd)) && !memcmp(cTlrno,hv_zf.tel,sizeof(cTlrno)-1) && strcmp(getenv("FHTEST"),"LVTEST")!=0)
	{
		vtcp_log("%s,%d,¼��͸��˲���ͬһ������Ա",__FILE__,__LINE__);
		sprintf(acErrMsg,"¼��͸��˲���ͬһ������Ա!");
		set_zd_data("0130",acErrMsg);
		strncpy( g_pub_tx.reply,"P080",4);
		return(-1);
	}
	memcpy(hv_payin->T_tx_br_no,hv_zf.br_no,sizeof(hv_payin->T_tx_br_no));

	sprintf(hv_payin->T_tx_date, "%d", hv_zf.tx_date);
	sprintf(hv_payin->T_wt_date, "%d", hv_zf.wt_date);	
	sprintf(hv_payin->T_tx_time, "%d", hv_zf.tx_time);
	
	memcpy(hv_payin->T_hv_sts,hv_zf.hv_sts,sizeof(hv_payin->T_hv_sts));
	vtcp_log("[%s][%d] hv_sts====[%c]",__FILE__,__LINE__,hv_zf.hv_sts[0]);
	memcpy(hv_payin->T_orderno,hv_zf.orderno,sizeof(hv_payin->T_orderno));	
	memcpy(hv_payin->T_cur_no,hv_zf.cur_no,sizeof(hv_payin->T_cur_no));	
	memcpy(hv_payin->T_note_type,hv_zf.note_type,sizeof(hv_payin->T_note_type));	
	memcpy(hv_payin->T_note_no,hv_zf.note_no,sizeof(hv_payin->T_note_no));
	dot_dbl2str(hv_zf.tx_amt,sizeof(hv_payin->T_tx_amt) ,2,hv_payin->T_tx_amt);	

	memcpy(hv_payin-> T_tx_chrg_ind,hv_zf.tx_chrg_ind,sizeof(hv_payin->T_tx_chrg_ind));	
	dbl2str(&hv_zf.chrg_amt,2,sizeof(hv_payin->T_chrg_amt),0,	hv_payin->T_chrg_amt); 
	memcpy(hv_payin->T_pay_qs_no,hv_zf.pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	
	memcpy(hv_payin->T_pay_opn_br_no,hv_zf.pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));
	memcpy(hv_payin->T_or_br_no,hv_zf.or_br_no,sizeof(hv_payin->T_or_br_no));	
	vtcp_log("%s,%d hv_payin->T_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->T_pay_opn_br_no);	
	vtcp_log("%s,%d hv_payin->F_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_pay_opn_br_no);
	
	memcpy(hv_payin->T_pay_ac_no,hv_zf.pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	
	memcpy(hv_payin->T_pay_name,hv_zf.pay_name,sizeof(hv_payin->T_pay_name));	
	memcpy(hv_payin->T_pay_addr,hv_zf.pay_addr,sizeof(hv_payin->T_pay_addr));	
	memcpy(hv_payin->T_cash_qs_no,hv_zf.cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	

	memcpy(hv_payin->T_ac_br_no,hv_zf.ac_br_no,sizeof(hv_payin->T_ac_br_no));	
	memcpy(hv_payin->T_cash_opn_br_no,hv_zf.cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));	
	memcpy(hv_payin->T_cash_ac_no,hv_zf.cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	
	memcpy(hv_payin->T_cash_name,hv_zf.cash_name,sizeof(hv_payin->T_cash_name));	
	memcpy(hv_payin->T_cash_addr,hv_zf.cash_addr,sizeof(hv_payin->T_cash_addr));	
	memcpy(hv_payin->T_yw_type,hv_zf.yw_type,sizeof(hv_payin->T_yw_type));	

	memcpy(hv_payin->T_sendco,hv_zf.sendco,sizeof(hv_payin->T_sendco));	
	memcpy(hv_payin->T_receco,hv_zf.receco,sizeof(hv_payin->T_receco));	
	memcpy(hv_payin->T_cmtno,hv_zf.cmtno,sizeof(hv_payin->T_cmtno));	

	memcpy(hv_payin->T_beg_sts,hv_zf.beg_sts,sizeof(hv_payin->T_beg_sts));	
	vtcp_log("[%s][%d] beg_sts====[%c]",__FILE__,__LINE__,hv_zf.beg_sts[0]);
	memcpy(hv_payin->T_operlevel,hv_zf.operlevel,sizeof(hv_payin->T_operlevel));	
	memcpy(hv_payin->T_tel,hv_zf.tel,sizeof(hv_payin->T_tel));		
	memcpy(hv_payin->T_resend_ind,hv_zf.beg_sts,sizeof(hv_payin->T_resend_ind));	
	memcpy(hv_payin->T_operlevel,hv_zf.operlevel,sizeof(hv_payin->T_operlevel));	

	sprintf(hv_payin->T_trace_no, "%d", hv_zf.trace_no);

	memcpy(hv_payin->T_resend_ind,hv_zf.resend_ind,sizeof(hv_payin->T_resend_ind));	
	memcpy(hv_payin->T_hv_osts,hv_zf.hv_osts,sizeof(hv_payin->T_hv_osts));	
	memcpy(hv_payin->T_checkflag,hv_zf.checkflag,sizeof(hv_payin->T_checkflag));	
	memcpy(hv_payin->T_hv_prt_ind,hv_zf.hv_prt_ind,sizeof(hv_payin->T_hv_prt_ind));

	sprintf(hv_payin->T_resend_date, "%d", hv_zf.resend_date);

	memcpy(hv_payin->T_o_orderno,hv_zf.o_orderno,sizeof(hv_payin->T_o_orderno));	

	sprintf(hv_payin->T_o_tx_date, "%d", hv_zf.o_tx_date);

	memcpy(hv_payin->T_o_cmtno,hv_zf.o_cmtno,sizeof(hv_payin->T_o_cmtno));	

	memcpy(hv_payin->T_lw_ind,hv_zf.lw_ind,sizeof(hv_payin->T_lw_ind));	
	memcpy(hv_payin->T_addid,hv_zf.addid,sizeof(hv_payin->T_addid));
	vtcp_log("%s,%d hv_zf.refno************=[%s]\n",__FILE__,__LINE__,hv_zf.refno);	
	memcpy(hv_payin->T_refno,hv_zf.refno,sizeof(hv_payin->T_refno));	
	vtcp_log("%s,%d hv_payin->T_refno******=[%s]\n",__FILE__,__LINE__,hv_payin->T_refno);
	memcpy(hv_payin->T_auth_tel,hv_zf.auth_tel,sizeof(hv_payin->T_auth_tel));		

	sprintf(hv_payin->T_auth_trace_no, "%d", hv_zf.auth_trace_no);

	memcpy(hv_payin->T_chk,hv_zf.chk,sizeof(hv_payin->T_chk));					

	sprintf(hv_payin->T_chk_trace_no, "%d", hv_zf.chk_trace_no);
	sprintf(hv_payin->T_dealerr_date, "%d", hv_zf.dealerr_date);

	memcpy(hv_payin->T_dealerr_tel,hv_zf.dealerr_tel,sizeof(hv_payin->T_dealerr_tel));	
	memcpy(hv_payin->T_resp1,hv_zf.resp1,sizeof(hv_payin->T_resp1));	
	memcpy(hv_payin->T_hv_brf,hv_zf.hv_brf,sizeof(hv_payin->T_hv_brf));	
  vtcp_log("%s,%d hv_payin->T_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->T_pay_opn_br_no);	
	vtcp_log("%s,%d hv_payin->F_pay_opn_br_no************=[%s]\n",__FILE__,__LINE__,hv_payin->F_pay_opn_br_no);
	sprintf(hv_payin->T_o_wt_date, "%d", hv_zf.o_wt_date);

	return(0);	                             
}

/*************************************************
* �� �� ��:  iHvTisToNotPayin
* ���������� ��ǰ̨�������� 8583 ���� NOTPAYIN �ṹ����Ӧ��
* ��    ��:  ChenMing
* ������ڣ� 2006-8-9 
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iHvTisToNotPayin(HV_NOTPAY_AREA * phv_notpayin)
{
	HV_FD123_AREA fd123;
	memset(&fd123, 0 , sizeof(fd123));
	get_fd_data("1230",(char*)&fd123);
	/*��������***/
	memcpy(phv_notpayin->F_opcd,fd123.opcd,sizeof(phv_notpayin->F_opcd));
	/*������ʾ***/
	get_fd_data("0200",phv_notpayin->F_lw_ind);	
	/* ���׻��� */
	get_fd_data("0030",phv_notpayin->F_tx_br_no);	
	/**ί������       **/
	get_fd_data("0440",phv_notpayin->F_wt_date);	
	strncpy(phv_notpayin->F_tx_date,phv_notpayin->F_wt_date,sizeof(phv_notpayin->F_wt_date));
	/*cmt���**/
	memcpy(phv_notpayin->F_cmtno,fd123.cmtno,sizeof(phv_notpayin->F_cmtno));
	/*ԭcmt���**/
	memcpy(phv_notpayin->F_o_cmtno,fd123.o_cmtno,sizeof(phv_notpayin->F_o_cmtno));	
	/****Add by SSH,2006.9.7,��otype��ֵ****/
	memcpy(phv_notpayin->F_otype,fd123.otype,sizeof(phv_notpayin->F_otype));
	GetOr_br_noByBr_no(phv_notpayin->F_tx_br_no,phv_notpayin->F_or_br_no);
	/*�������к�     **/
	memcpy(phv_notpayin->F_ac_br_no,fd123.ac_br_no,sizeof(phv_notpayin->F_ac_br_no));
	/*actno ��֧���������漰���ʺ�   ???  **/
	memcpy(phv_notpayin->F_actno,fd123.pay_ac_no,sizeof(phv_notpayin->F_actno));
	/*F_name �ʺŻ���**/
	memcpy(phv_notpayin->F_name,fd123.pay_name,sizeof(phv_notpayin->F_name));
	/* ����   ��ѯ���鸴�����ɸ�ʽʹ��*/
	memcpy(phv_notpayin->F_content,fd123.notpay_content,sizeof(phv_notpayin->F_content));		
	/* ���� */
	memcpy(phv_notpayin->F_hv_brf,fd123.brf,sizeof(phv_notpayin->F_hv_brf));	
	/**����Ա��**/
	get_fd_data("0070",phv_notpayin->F_tel);
	get_fd_data("0070",phv_notpayin->F_send_tel);	
	/**�˻�/����Ӧ��**/
	memcpy(phv_notpayin->F_respcode,fd123.resp,sizeof(phv_notpayin->F_respcode));	
	/**ԭ֧���������**/
	memcpy(phv_notpayin->F_o_payin_orderno,fd123.o_orderno,sizeof(phv_notpayin->F_o_payin_orderno));	
	/**ԭ֧���������  �鸴֮ԭ��ѯ��ŵ�Ӧ�����Ӧ�����뽻�׵���� **/
	memcpy(phv_notpayin->F_o_notpay_orderno,fd123.qr_tx_num,sizeof(phv_notpayin->F_o_notpay_orderno));	

	/* ԭ���ҷ���,�鸴�Ƚ���ʹ�� */
	get_fd_data("0210",phv_notpayin->F_o_cur_no);
	/* ֧�����׽��,�鸴��ʹ�� */
	get_fd_data("0390",phv_notpayin->F_o_txamt);	

	/**ԭ��������**/
	memcpy(phv_notpayin->F_o_tx_type,fd123.tx_type,sizeof(phv_notpayin->F_o_tx_type));	

	/**֧�����׶�Ӧ������**/
	memcpy(phv_notpayin->F_o_or_br_no,fd123.or_br_no,sizeof(phv_notpayin->F_o_or_br_no));	

	/* ԭί������  �ṩ����Ӧ��֧������ */
	get_fd_data("0450",phv_notpayin->F_o_wt_date);	
	/* ԭ��֧����������,�鸴���õ��Ĳ�ѯ���� */	
	get_fd_data("0460",phv_notpayin->F_o_fzf_date);	

	/**ԭCMT���**/
	memcpy(phv_notpayin->F_o_cmtno,fd123.o_cmtno,sizeof(phv_notpayin->F_o_cmtno));
	/**ҵ���ִ״̬   ����״̬*/
	memcpy(phv_notpayin->F_rcpstat,fd123.proc_sts,sizeof(phv_notpayin->F_rcpstat));
	/** ���Ĳο��� 
	if (iGetHvRefno(phv_notpayin.F_or_br_no,phv_notpayin->F_refno) != 0)
	{
		return 1;  	
	}**/
	/**ԭ֧�����׽����к� == �������к�**/
	memcpy(phv_notpayin->F_o_ac_br_no,phv_notpayin->F_or_br_no,sizeof(phv_notpayin->F_o_ac_br_no)); /**ԭ֧�����׷����к�**/
	memcpy(phv_notpayin->F_o_or_br_no,phv_notpayin->F_ac_br_no,sizeof(phv_notpayin->F_o_or_br_no));
	/**ccpcӦ����  ����״̬**/
	memcpy(phv_notpayin->F_resp1,fd123.procode,sizeof(phv_notpayin->F_resp1));

	return 0;
}
/*************************************************
* �� �� ��:  iHvNotPayinFToT
* ���������� ��֧�� ǰ̨ F ---> T
* ��    ��:  ChenMing
* ������ڣ� 2006-8-9 
* �޸ļ�¼��
*************************************************/
int iHvNotPayinFToT(HV_NOTPAY_AREA *phv_notpayin)
{
	char cFilename[1000];
	FILE * fpr;
	char reply[5];
	int iRet=0;
	char cBuff[2000];
	memset(reply, 0 , sizeof(reply));
	memset(cBuff, 0 , sizeof(cBuff));
	memset(cFilename, 0  , sizeof(cFilename));

	memcpy(phv_notpayin->T_or_br_no,phv_notpayin->F_or_br_no,sizeof(phv_notpayin->T_or_br_no));/* �������к�*/
	memcpy(phv_notpayin->T_orderno,phv_notpayin->F_orderno,sizeof(phv_notpayin->T_orderno));/* �������,��ѯ��ţ��鸴���*/
	memcpy(phv_notpayin->T_tx_date ,phv_notpayin->F_tx_date,sizeof(phv_notpayin->T_tx_date ));/* ��������*/
	memcpy(phv_notpayin->T_wt_date ,phv_notpayin->F_wt_date ,sizeof(phv_notpayin->T_wt_date ));/* ί������,��ѯ�鸴����*/
	memcpy(phv_notpayin->T_br_no,phv_notpayin->F_tx_br_no,sizeof(phv_notpayin->T_br_no));/* �������к�*/

	memcpy(phv_notpayin->T_cmtno,phv_notpayin->F_cmtno,sizeof(phv_notpayin->T_cmtno    ));/* CMT���      */
	get_fd_data("0060",phv_notpayin->T_tx_time);	/* ����ʱ��*/
	get_fd_data("0040",phv_notpayin->T_trace_no);	/* ��ˮ��*/

	/*iFindQsbrno(phv_payin->T_or_br_no,phv_payin->T_pay_qs_no); �����������к� 
	 *iFindQsbrno(phv_payin->T_ac_br_no,phv_payin->T_cash_qs_no); �����������к� 
	 *iGetSendco(phv_payin->T_or_br_no,phv_payin->T_sendco); �������Ĵ���
	 *iGetSendco(phv_payin->T_ac_br_no,phv_payin->T_receco); �ձ����Ĵ���
	 */	
	iGet_full_hv_no(phv_notpayin->T_or_br_no,phv_notpayin->T_sendco,phv_notpayin->T_pay_qs_no);
	iGet_full_hv_no(phv_notpayin->T_ac_br_no,phv_notpayin->T_receco,phv_notpayin->T_cash_qs_no);
	memcpy(phv_notpayin->T_ac_br_no ,phv_notpayin->F_ac_br_no,sizeof(phv_notpayin->T_ac_br_no));/* �������к�--�˻㽻�׵�ԭ������ */
	memcpy(phv_notpayin->T_o_wt_date,phv_notpayin->F_o_wt_date,sizeof(phv_notpayin->T_o_wt_date));/*ԭί������*/			
	memcpy(phv_notpayin->T_o_or_br_no ,phv_notpayin->F_o_or_br_no,sizeof(phv_notpayin->T_o_or_br_no ));/* ԭ֧�����׷�����*/
	memcpy(phv_notpayin->T_o_ac_br_no ,phv_notpayin->F_o_ac_br_no,sizeof(phv_notpayin->T_o_ac_br_no )); /* ԭ֧�����׽����� */
	memcpy(phv_notpayin->T_o_payin_orderno ,phv_notpayin->F_o_payin_orderno ,sizeof(phv_notpayin->T_o_payin_orderno )); /* ԭ֧�����׽������*/
	memcpy(phv_notpayin->T_o_cur_no,phv_notpayin->F_o_cur_no,sizeof(phv_notpayin->T_o_cur_no));/* ��Ӧ֧�����׵Ļ�������*/
	memcpy(phv_notpayin->T_o_tx_amt,phv_notpayin->F_o_txamt,sizeof(phv_notpayin->T_o_tx_amt));/* ��Ӧ֧�����׵Ľ��׽��*/
	memcpy(phv_notpayin->T_o_tx_type,phv_notpayin->F_o_tx_type,sizeof(phv_notpayin->T_o_tx_type));/* ԭ�������� ���1 ����2 ���� */
	memcpy(phv_notpayin->T_o_cmtno  ,phv_notpayin->F_o_cmtno,sizeof(phv_notpayin->T_o_cmtno  ));/* ԭCMT��� */

	/* memcpy(phv_notpayin->T_req_date,phv_notpayin->     ,sizeof(phv_notpayin->T_req_date));�������� */
	/*memcpy(phv_notpayin->T_req_br_no,phv_notpayin->    ,sizeof(phv_notpayin->T_req_br_no)); �����к� */

	memcpy(phv_notpayin->T_req_content,phv_notpayin->F_content,sizeof(phv_notpayin->T_req_content));/**�������к�**/
/******
	memcpy(phv_notpayin->T_req_orderno,phv_notpayin->   ,sizeof(phv_notpayin->T_req_orderno)); �����к� 
	memcpy(phv_notpayin->T_res_date,phv_notpayin->      ,sizeof(phv_notpayin->T_res_date)); Ӧ������ 
	memcpy(phv_notpayin->T_res_br_no,phv_notpayin->     ,sizeof(phv_notpayin->T_res_br_no)); Ӧ���к� 

	memcpy(phv_notpayin->T_respcode,phv_notpayin->    ,sizeof(phv_notpayin->T_respcode)); �˻�/����Ӧ�� 
	memcpy(phv_notpayin->T_addid,phv_notpayin->       ,sizeof(phv_notpayin->T_addid)); ������id*****/ 

	memcpy(phv_notpayin->T_send_tel,phv_notpayin->F_send_tel,sizeof(phv_notpayin->T_send_tel));/*  */

	memcpy(phv_notpayin->T_refno,phv_notpayin->F_refno,sizeof(phv_notpayin->T_refno));/* Ӧ���к� */

	/*memcpy(phv_notpayin->T_hv_prt_ind,phv_notpayin->  ,sizeof(phv_notpayin->T_hv_prt_ind)); ��ӡ��� */
	memcpy(phv_notpayin->T_req_orderno,phv_notpayin->F_o_notpay_orderno,sizeof(phv_notpayin->T_req_orderno));/* ԭ��֧��������� */

/****
	memcpy(phv_notpayin->T_lw_ind,phv_notpayin->F_lw_ind,sizeof(phv_notpayin->T_lw_ind)); ������־ */

	/*  д�븽���� ???  */

	if (!memcmp(phv_notpayin->F_opcd,"10",sizeof(phv_notpayin->F_opcd)))
	{/* 01¼�� */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_REGIST;
	}
	if (!memcmp(phv_notpayin->F_opcd,"30",sizeof(phv_notpayin->F_opcd)))
	{/* 02���� */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_CHECK;
	}
	if (!memcmp(phv_notpayin->F_opcd,"40",sizeof(phv_notpayin->F_opcd)))
	{/* 03���� */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_WAIT;
	}
	if (!memcmp(phv_notpayin->F_opcd,"60",sizeof(phv_notpayin->F_opcd)))
	{/* 04���� */
		if (!memcmp(reply,"0000",4))
		{
			phv_notpayin->T_hv_fzf_sts[0]=RECEIVE_ACCT_QS;
		}
		else
		{
			/* ����ʧ���ˣ�Ҫ���жϽ��ջ����Ƿ���ڣ����ڹ�֧�У������ڹ����� */
			phv_notpayin->T_hv_fzf_sts[0]=RECEIVE_CUSTNAME_ERR;
		}
	}
	if (!memcmp(phv_notpayin->F_opcd,"50",sizeof(phv_notpayin->F_opcd)))
	{/* 05Ӧ�� */
		phv_notpayin->T_hv_fzf_sts[0]=SEND_ACCT_QS;
	}
	return 0;
}

/************************************************************************* 
* �� �� ��: iHvUpdFzfRecFromPayin                  
* ���������� ���� ��֧���ṹ T ���� ���� hv_fzf    
* ��    ��:  ChenMing                             
* ������ڣ� 2006-8-9                             
* �޸ļ�¼��                                      
*   ��   ��:                                      
*   �� �� ��:                                     
*   �޸�����:                                      
*************************************************************************/ 
int iHvUpdFzfRecFromPayin(HV_NOTPAY_AREA *phv_notpayin,int insflg)
{          	/* insflg 0.����   1.����  */

	int ret=0;
	struct hv_fzf_c hv_fzf;
	memset(&hv_fzf, 0x00, sizeof(hv_fzf));

	vtcp_log("%s,%d iHvUpdFzfRecFromPayin() ��ʼ \n",__FILE__,__LINE__);
	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
	
	memcpy(hv_fzf.cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf.cmtno)-1);
	memcpy(hv_fzf.or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf.or_br_no)-1);
	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
	if ( strlen(phv_notpayin->T_wt_date) > 0 )  
		hv_fzf.wt_date = apatoi(phv_notpayin->T_wt_date,8);
	memcpy(hv_fzf.orderno,phv_notpayin->T_orderno,sizeof(hv_fzf.orderno)-1);


	vtcp_log("%s,%d cmtno=[%s] or_br_no=[%s]\n wt_date=[%d] orderno=[%s]\n", \
		__FILE__,__LINE__,hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);

	if (insflg == 0) 
	{ /* 0.����    */
		ret = Hv_fzf_Dec_Upd(g_pub_tx.reply,"cmtno='%s' and or_br_no='%s' and wt_date='%d' and orderno='%s'", \
			hv_fzf.cmtno,hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
				return ret;
		}
		ret = Hv_fzf_Fet_Upd(&hv_fzf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"��ѯ��֧�����ױ����!![%d]",ret);
			WRITEMSG
				return ret;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"��֧�����ױ��в����ڸü�¼!!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"O127");
			return ret;
		}		
		iHvNotPayin_Copy(phv_notpayin,&hv_fzf);

		ret = Hv_fzf_Upd_Upd(hv_fzf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
			WRITEMSG
				return ret;
		}    
		Hv_fzf_Clo_Upd();	

	}
	else
	{  /* 1.����  */

    vtcp_log("%s,%d,  phv_notpayin->T_br_no=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_br_no );
    vtcp_log("%s,%d,  phv_notpayin->T_cmtno=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_cmtno );
    vtcp_log("%s,%d, T_orderno= [%.8s]",__FILE__,__LINE__,phv_notpayin->T_orderno );
    vtcp_log("%s,%d, T_or_br_no= [%.12s]",__FILE__,__LINE__,phv_notpayin->T_or_br_no);
        
		iHvNotPayin_Copy(phv_notpayin,&hv_fzf);

    vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf.br_no );
    vtcp_log("%s,%d,  hv_fzf.cmtno=[%.7s]",__FILE__,__LINE__,hv_fzf.cmtno );  
    
    vtcp_log("%s,%d,  hv_fzf.or_br_no=[%.12s]",__FILE__,__LINE__,hv_fzf.or_br_no );
    vtcp_log("%s,%d,  hv_fzf.orderno=[%.8s]",__FILE__,__LINE__,hv_fzf.orderno );  
      

    Hv_fzf_Print(&hv_fzf);
    
		ret = Hv_fzf_Ins(hv_fzf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"���֧�����ױ������Ӽ�¼����!![%d]",ret);
			WRITEMSG
				return ret;
		}			
	}	
}
/***********************************************************************     
* �� �� ��: iHvNotPayin_Copy                    
* ��������: ��HV_NOTPAY_AREA����Ӧ�ֶ�ֵ����hv_fzf_c����Ӧ�ֶ� 
* ��    ��:  ChenMing                            
* ������ڣ� 2006-8-9                            
* �޸ļ�¼��                                     
*   ��   ��:                                      
*   �� �� ��:                                    
*   �޸�����:                                    
*************************************************************************/  
int iHvNotPayin_Copy(HV_NOTPAY_AREA *phv_notpayin, struct hv_fzf_c *hv_fzf)
{
	/*  	 */
	
	memcpy(hv_fzf->cmtno,phv_notpayin->T_cmtno,sizeof(hv_fzf->cmtno)-1);
	memcpy(hv_fzf->or_br_no,phv_notpayin->T_or_br_no,sizeof(hv_fzf->or_br_no)-1);
	if ( strlen(phv_notpayin->T_wt_date) > 0 )  
		hv_fzf->wt_date = apatoi(phv_notpayin->T_wt_date,8);
	memcpy(hv_fzf->orderno,phv_notpayin->T_orderno,sizeof(hv_fzf->orderno)-1);
	
  vtcp_log(" phv_notpayin->T_otype[%s] ",phv_notpayin->T_otype);		
  hv_fzf->otype[0] = phv_notpayin->T_otype[0];
	
	if( strlen(phv_notpayin->T_tx_date) > 0 ) 	   
	{
			hv_fzf->tx_date = apatoi(phv_notpayin->T_tx_date,8);
	}

  vtcp_log("%s,%d,  phv_notpayin->T_br_no=[%.7s]",__FILE__,__LINE__,phv_notpayin->T_br_no );
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );

	
	memcpy(hv_fzf->br_no,phv_notpayin->T_br_no,BRNO_LEN);	  
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );
  
	vtcp_log("[%s] [%d] T_hv_fzf_sts = [%c]",__FILE__,__LINE__,phv_notpayin->T_hv_fzf_sts[0]); 
	hv_fzf->hv_fzf_sts[0] = phv_notpayin->T_hv_fzf_sts[0];    
	vtcp_log("[%s] [%d]hv_fzf->hv_fzf_sts = [%c]",__FILE__,__LINE__,hv_fzf->hv_fzf_sts[0]); 
  
	if ( strlen(phv_notpayin->T_tx_time) > 0 )   	  hv_fzf->tx_time = apatoi(phv_notpayin->T_tx_time,6);
  if ( strlen(phv_notpayin->T_trace_no) > 0 )   	hv_fzf->trace_no = apatoi(phv_notpayin->T_trace_no,6);

	memcpy(hv_fzf->pay_qs_no,phv_notpayin->T_pay_qs_no,sizeof(hv_fzf->pay_qs_no)-1);
	memcpy(hv_fzf->ac_br_no,phv_notpayin->T_ac_br_no,sizeof(hv_fzf->ac_br_no)-1);      	
	memcpy(hv_fzf->cash_qs_no,phv_notpayin->T_cash_qs_no,sizeof(hv_fzf->cash_qs_no)-1);

	vtcp_log("phv_notpayin->T_o_wt_date=[%s]   phv_notpayin->T_o_wt_date=[%s]",phv_notpayin->T_o_wt_date,phv_notpayin->T_o_wt_date);	
	hv_fzf->o_tx_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* ���ڴ���ʱ��֤�κ�ʱ��o_wt_date��o_tx_dateһ�� */
	hv_fzf->o_wt_date = apatoi(phv_notpayin->T_o_wt_date,8);   /* ���ڴ���ʱ��֤�κ�ʱ��o_wt_date��o_tx_dateһ�� */

	memcpy(hv_fzf->o_or_br_no,phv_notpayin->T_o_or_br_no,sizeof(hv_fzf->o_or_br_no)-1);	
	memcpy(hv_fzf->o_ac_br_no,phv_notpayin->T_o_ac_br_no,sizeof(hv_fzf->o_ac_br_no)-1);
	vtcp_log("[%s] [%d]hv_fzf->o_tx_date = [%d]",__FILE__,__LINE__,hv_fzf->o_tx_date);

	memcpy(hv_fzf->tel,phv_notpayin->T_tel,sizeof(hv_fzf->tel)-1);
	memcpy(hv_fzf->send_tel,phv_notpayin->T_send_tel,sizeof(hv_fzf->send_tel)-1);
		
	memcpy(hv_fzf->o_orderno,phv_notpayin->T_o_payin_orderno,sizeof(hv_fzf->o_orderno)-1);
	memcpy(hv_fzf->o_cur_ind,phv_notpayin->T_o_cur_no,sizeof(hv_fzf->o_cur_ind)-1);
	vtcp_log("%s,%d,Gethere,T_AMT=%s",__FILE__,__LINE__,phv_notpayin->T_o_tx_amt);

 	if(strlen(phv_notpayin->T_o_tx_amt) >0 )  	  
		dot_str2dbl(phv_notpayin->T_o_tx_amt,sizeof(phv_notpayin->T_o_tx_amt), &hv_fzf->o_tx_amt);
	vtcp_log("%s,%d,Gethere,D_AMT=%f",__FILE__,__LINE__,hv_fzf->o_tx_amt);
  			
	memcpy(hv_fzf->o_cmtno,phv_notpayin->T_o_cmtno,sizeof(hv_fzf->o_cmtno)-1);
	/**
	memcpy(hv_fzf->o_tx_type,phv_notpayin->T_o_tx_type,sizeof(hv_fzf->o_tx_type)-1);
	**/
	hv_fzf->o_tx_type[0] = phv_notpayin->T_o_tx_type[0];
	memcpy(hv_fzf->sendco,phv_notpayin->T_sendco,sizeof(hv_fzf->sendco)-1);
	memcpy(hv_fzf->receco,phv_notpayin->T_receco,sizeof(hv_fzf->receco)-1);
	vtcp_log("[%s] [%d]hv_fzf->o_tx_type[0] = [%c]",__FILE__,__LINE__,hv_fzf->o_tx_type[0]);
	hv_fzf->req_date = apatoi(phv_notpayin->T_req_date,8);
	memcpy(hv_fzf->req_br_no,phv_notpayin->T_req_br_no,sizeof(hv_fzf->req_br_no)-1);
	memcpy(hv_fzf->req_content,phv_notpayin->T_req_content,sizeof(hv_fzf->req_content)-1);
	memcpy(hv_fzf->req_orderno,phv_notpayin->T_req_orderno,sizeof(hv_fzf->req_orderno)-1);	
	hv_fzf->res_date = apatoi(phv_notpayin->T_res_date,8);
	vtcp_log("[%s] [%d]hv_fzf->res_date = [%d]",__FILE__,__LINE__,hv_fzf->res_date);
	memcpy(hv_fzf->res_br_no,phv_notpayin->T_res_br_no,sizeof(hv_fzf->res_br_no)-1);
	memcpy(hv_fzf->res_content,phv_notpayin->T_res_content,sizeof(hv_fzf->res_content)-1);
	memcpy(hv_fzf->res_orderno,phv_notpayin->T_res_orderno,sizeof(hv_fzf->res_orderno)-1);
	/**
	memcpy(hv_fzf->res_sts,phv_notpayin->T_respcode,sizeof(hv_fzf->res_sts)-1);
	**/
	hv_fzf->res_sts[0] = phv_notpayin->T_respcode[0];
	vtcp_log("[%s] [%d]hv_fzf->res_sts = [%c]",__FILE__,__LINE__,hv_fzf->res_sts[0]);
	memcpy(hv_fzf->addid,phv_notpayin->T_addid,sizeof(hv_fzf->addid)-1);
	memcpy(hv_fzf->refno,phv_notpayin->T_refno,sizeof(hv_fzf->refno)-1);
 	
	memcpy(hv_fzf->hv_prt_ind,phv_notpayin->T_hv_prt_ind,sizeof(hv_fzf->hv_prt_ind)-1);
	memcpy(hv_fzf->hv_brf,phv_notpayin->T_hv_brf,sizeof(hv_fzf->hv_brf)-1);
	hv_fzf->send_trace_no = apatoi(phv_notpayin->T_o_trace_no,6);
	vtcp_log("[%s] [%d]hv_fzf->o_trace_no= [%d]",__FILE__,__LINE__,hv_fzf->send_trace_no);
	vtcp_log("[%s] [%d]hv_fzf->refno= [%s]",__FILE__,__LINE__,hv_fzf->refno);
	hv_fzf->lw_ind[0] = phv_notpayin->T_lw_ind[0];/* ������ʶ 1-�� 2-��  */
  vtcp_log("%s,%d,  hv_fzf.br_no=[%.7s]",__FILE__,__LINE__,hv_fzf->br_no );
  		
  	/* */
  return 0;	
}

/************************************************************************* 
* �� �� ��: iHvUpdZfRecFromPayin                  
* ���������� ���� ֧���ṹ T ���� ���� hv_zf    
* ��    ��:  ChenMing                             
* ������ڣ� 2006-8-18                             
* �޸ļ�¼��                                      
*   ��   ��:                                      
*   �� �� ��:                                     
*   �޸�����:                                      
*************************************************************************/ 
int iHvUpdZfRecFromPayin(HV_PAYIN_AREA *phv_payin,int insflg)
{          	/* insflg 0.����   1.����  */

	int ret=0;
	char sqltmp[200];
	struct hv_zf_c hv_zf;
	char    cTempstr    [1024];
	
	
	memset(&sqltmp, 0x00, sizeof(sqltmp));	
	memset(&hv_zf, 0x00, sizeof(hv_zf));
	vtcp_log("iHvUpdRecFromPayin() ��ʼ ");

	
	memcpy(hv_zf.cmtno,phv_payin->T_cmtno,sizeof(phv_payin->T_cmtno));
	memcpy(hv_zf.or_br_no,phv_payin->T_or_br_no,sizeof(phv_payin->T_or_br_no));
	if ( strlen(phv_payin->T_wt_date) > 0 )  
		hv_zf.wt_date = apatoi(phv_payin->T_wt_date,8);
	memcpy(hv_zf.orderno,phv_payin->T_orderno,sizeof(phv_payin->T_orderno));
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 

	if (insflg == 0) 
	{ /* 0.����    */
		ret = Hv_zf_Dec_Upd(g_pub_tx.reply," cmtno='%s' and or_br_no='%s' and wt_date=%d and orderno='%s' ", \
			hv_zf.cmtno,hv_zf.or_br_no,hv_zf.wt_date,hv_zf.orderno);
		if (ret != 0)
		{
			sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
			WRITEMSG
				return ret;
		}
		ret = Hv_zf_Fet_Upd(&hv_zf, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"��ѯ֧�����ױ����!![%d]",ret);
			WRITEMSG
				return ret;
		}
		else if (ret == 100)
		{
			sprintf(acErrMsg,"֧�����ױ��в����ڸü�¼!!");
			WRITEMSG
				strcpy(g_pub_tx.reply,"O127");
			return ret;
		}

		iHvPayin_Copy(phv_payin,&hv_zf);

		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.pay_name, sizeof(hv_zf.pay_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]����\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
			memcpy(hv_zf.pay_name, cTempstr, sizeof(hv_zf.pay_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		
		
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.cash_name, sizeof(hv_zf.cash_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]����\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
			memcpy(hv_zf.cash_name, cTempstr, sizeof(hv_zf.cash_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name); 

		vtcp_log("[%s][%d] ����֧�����ױ��¼�ɹ�",__FILE__,__LINE__);	

		ret = Hv_zf_Upd_Upd(hv_zf, g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"UPDATE COM_BRANCH ERROR ! [%d]",ret);
			WRITEMSG
			return ret;
		}    
		Hv_zf_Clo_Upd();	
		vtcp_log("[%s][%d] ����֧�����ױ��¼�ɹ�",__FILE__,__LINE__);	
	}
	else
	{  /* 1.����  */
		iHvPayin_Copy(phv_payin,&hv_zf);
			
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.pay_name, sizeof(hv_zf.pay_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]����\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
			memcpy(hv_zf.pay_name, cTempstr, sizeof(hv_zf.pay_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.pay_name==[%.60s] ",__FILE__,__LINE__,hv_zf.pay_name); 
		
		
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name);
		memset(cTempstr,0,sizeof(cTempstr));
		memcpy(cTempstr, hv_zf.cash_name, sizeof(hv_zf.cash_name)-1);
		if(phv_payin->F_lw_ind[0]=='2')
		{
			vtcp_log("[%s][%d]����\n",__FILE__,__LINE__);
			dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
			memcpy(hv_zf.cash_name, cTempstr, sizeof(hv_zf.cash_name)-1);
		}
		vtcp_log("[%s][%d] hv_zf.cash_name==[%.60s] ",__FILE__,__LINE__,hv_zf.cash_name); 
			
		ret = Hv_zf_Ins(hv_zf, g_pub_tx.reply);
		vtcp_log("[%s][%d] reply is[%s]",__FILE__,__LINE__,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"��֧�����ױ������Ӽ�¼����!![%d][%s]",ret,g_pub_tx.reply);
			WRITEMSG
			Hv_zf_Debug(&hv_zf);
				return ret;
		}		
		vtcp_log("[%s][%d] ��֧�����ױ������Ӽ�¼�ɹ�",__FILE__,__LINE__);	
	}
	ret = dg_hv_zfhtr(phv_payin,phv_payin->T_tx_amt);
	if (ret != 0)
	{
	   sprintf(acErrMsg,"��֧��������ϸ�����Ӽ�¼����!![%d]",ret);
	   strcpy(g_pub_tx.reply,"P010");
	   WRITEMSG
	   Hv_zf_Debug(&hv_zf);
		return ret;
	}
	vtcp_log("[%s][%d] ���� hv_zfhtr ��ɹ���",__FILE__,__LINE__); 
}
/***********************************************************************     
* �� �� ��: iHvPayin_Copy                    
* ��������: ��HV_NOTPAY_AREA����Ӧ�ֶ�ֵ����hv_zf_c����Ӧ�ֶ� 
* ��    ��:  ChenMing                            
* ������ڣ� 2006-8-9                            
* �޸ļ�¼��                                     
*   ��   ��:                                      
*   �� �� ��:                                    
*   �޸�����:                                    
*************************************************************************/  
int iHvPayin_Copy(HV_PAYIN_AREA *phv_payin, struct hv_zf_c *hv_zf_c)
{
	
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	memcpy(hv_zf_c->cmtno,phv_payin->T_cmtno,sizeof(phv_payin->T_cmtno));
	memcpy(hv_zf_c->or_br_no,phv_payin->T_or_br_no,sizeof(phv_payin->T_or_br_no));
	if ( strlen(phv_payin->T_wt_date) > 0 )  
		hv_zf_c->wt_date = apatoi(phv_payin->T_wt_date,8);
	memcpy(hv_zf_c->orderno,phv_payin->T_orderno,sizeof(phv_payin->T_orderno));

	
	strncpy(hv_zf_c->br_no,phv_payin->T_tx_br_no,BRNO_LEN);
	hv_zf_c->tx_date=apatoi(phv_payin->T_tx_date,8);/*�������� */
	
  vtcp_log("phv_payin->T_tx_time==[%.6s]",phv_payin->T_tx_time);		
	hv_zf_c->tx_time = apatoi(phv_payin->T_tx_time,6);/* ����ʱ�� */
  vtcp_log("hv_zf_c->tx_time==[%d]",hv_zf_c->tx_time);	
  	
	strncpy(hv_zf_c->orderno,phv_payin->T_orderno,sizeof(hv_zf_c->orderno  )-1);/* ���еĽ������ */
	strncpy(hv_zf_c->cur_no,phv_payin->T_cur_no,sizeof(hv_zf_c->cur_no   )-1);/* ���ҷ��� */
	strncpy(hv_zf_c->note_type,phv_payin->T_note_type,sizeof(hv_zf_c->note_type)-1);/* ƾ֤����  */	  
	strncpy(hv_zf_c->note_no,phv_payin->T_note_no,sizeof(hv_zf_c->note_no  )-1);/* ƾ֤��  */

  vtcp_log("phv_payin->T_tx_amt==[%.16s]",phv_payin->T_tx_amt);			
	dot_str2dbl(phv_payin->T_tx_amt,sizeof(phv_payin->T_tx_amt),&hv_zf_c->tx_amt);/* ���׽��  */	   	
  vtcp_log("hv_zf_c->tx_amt==[%lf]",hv_zf_c->tx_amt);			
		   	
	hv_zf_c->tx_chrg_ind[0]=phv_payin->T_tx_chrg_ind[0]; /* �����ѱ�־*/
	/*��ʱ������
	*hv_zf_c->chrg_amt  ������  
	*/	   	
	/* �����������к�(�ʺ�) accountno  */	
	
		   	
  strncpy(hv_zf_c->pay_qs_no,phv_payin->T_pay_qs_no,sizeof(hv_zf_c->pay_qs_no)-1); 
	vtcp_log("ת���ķ��������к�=[%.12s]",phv_payin->T_pay_qs_no,hv_zf_c->pay_qs_no);TRACE 
	strncpy(hv_zf_c->or_br_no,phv_payin->T_or_br_no,sizeof(hv_zf_c->or_br_no)-1);/* �������к�--�˻㽻�׵�ԭ������  */
	strncpy(hv_zf_c->pay_opn_br_no ,phv_payin->T_pay_opn_br_no,sizeof(hv_zf_c->pay_opn_br_no )-1);/* �����˿������к�  */
	vtcp_log("[%s][%d]phv_payin->T_pay_opn_br_no=[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	strncpy(hv_zf_c->pay_ac_no,phv_payin->T_pay_ac_no,sizeof(hv_zf_c->pay_ac_no)-1);/* �������ʺ�  */
	strncpy(hv_zf_c->pay_name,phv_payin->T_pay_name,sizeof(hv_zf_c->pay_name)-1);/* ����������--�˻㽻�׵�ԭ�տ��˻���  */
	strncpy(hv_zf_c->pay_addr,phv_payin->T_pay_addr,sizeof(hv_zf_c->pay_addr)-1);/* �����˵�ַ  */
	strncpy(hv_zf_c->cash_qs_no,phv_payin->T_cash_qs_no,sizeof(hv_zf_c->cash_qs_no)-1);/* �����������к�  */
	strncpy(hv_zf_c->ac_br_no,phv_payin->T_ac_br_no,sizeof(hv_zf_c->ac_br_no)-1);/* �������к�--�˻㽻�׵�ԭ������  */
	strncpy(hv_zf_c->cash_opn_br_no,phv_payin->T_cash_opn_br_no,sizeof(hv_zf_c->cash_opn_br_no)-1);/* �տ��˿������к�  */
	strncpy(hv_zf_c->cash_ac_no,phv_payin->T_cash_ac_no,	sizeof(hv_zf_c->cash_ac_no)-1);/* �տ����ʺ�  */
 
	strncpy(hv_zf_c->cash_name,phv_payin->T_cash_name,sizeof(hv_zf_c->cash_name)-1);/* �տ�������--�˻㽻�׵�ԭ�����˻���  */
	strncpy(hv_zf_c->cash_addr,phv_payin->T_cash_addr,sizeof(hv_zf_c->cash_addr)-1);/* �տ��˵�ַ  */
	strncpy(hv_zf_c->yw_type,phv_payin->T_yw_type,sizeof(hv_zf_c->yw_type)-1);/* ҵ������  */
	vtcp_log("[%s,%d]yw_type===[%s]",__FILE__,__LINE__,phv_payin->T_yw_type);
	strncpy(hv_zf_c->sendco,phv_payin->T_sendco,sizeof(hv_zf_c->sendco)-1);/* �������Ĵ���  */
	strncpy(hv_zf_c->receco,phv_payin->T_receco,sizeof(hv_zf_c->receco)-1);/* �ձ����Ĵ���  */
	strncpy(hv_zf_c->cmtno,phv_payin->T_cmtno,	sizeof(hv_zf_c->cmtno)-1);/* CMT���  */
	hv_zf_c->beg_sts  [0]=phv_payin->T_beg_sts  [0];/* ���statԭʼ״̬����ʱ��ල��  */
	hv_zf_c->operlevel[0]=phv_payin->T_operlevel[0];/*���ȼ���0-һ��1-����2-�ؼ�(����ս�����֧��) */
	hv_zf_c->hv_sts[0]=phv_payin->T_hv_sts[0];/* ״̬ �����hv_define.h����  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	strncpy(hv_zf_c->tel,phv_payin->T_tel,sizeof(hv_zf_c->tel)-1); /* ����ԱԱ  */
	hv_zf_c->trace_no=apatoi(phv_payin->T_trace_no,sizeof(phv_payin->T_trace_no));/* ��ˮ�� */
	hv_zf_c->hv_osts[0]=phv_payin->T_hv_osts[0];/* �˻㽻���õ���ԭ֧�����ʽ���״̬ */
	hv_zf_c->resend_ind[0]=phv_payin->T_resend_ind[0]; /* �ܾ��ط���־*/
	hv_zf_c->resend_date=apatoi(phv_payin->T_resend_date,8);/* �ܾ����·�������  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	vtcp_log("phv_payin->T_o_tx_date=[%.8s]   phv_payin->T_o_wt_date=[%.8s]",phv_payin->T_o_tx_date,phv_payin->T_o_wt_date); 
	hv_zf_c->o_tx_date=apatoi(phv_payin->T_o_tx_date,8);/* ԭ֧����������--�˻㽻���õ�  */
	hv_zf_c->o_wt_date=apatoi(phv_payin->T_o_tx_date,8);/* ԭί������  ���ڴ���ʱ��֤�κ�ʱ��o_wt_date��o_tx_dateһ��??  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	hv_zf_c->lw_ind[0]=phv_payin->T_lw_ind[0];/* ������ʶ 1-�� 2-��  */
	hv_zf_c->auth_trace_no=apatoi(phv_payin->T_auth_trace_no,sizeof(phv_payin->T_auth_trace_no));/* ��Ȩ����Ա��ˮ�� */
	hv_zf_c->chk_trace_no=apatoi(phv_payin->T_chk_trace_no,sizeof(phv_payin->T_chk_trace_no));/* ���˲���Ա��ˮ�� */
	hv_zf_c->send_tel_trace_no=apatoi(phv_payin->T_send_tel_trace_no,sizeof(phv_payin->T_send_tel_trace_no));/* ���Ͳ���Ա��ˮ�� */
	hv_zf_c->dealerr_date=apatoi(phv_payin->T_dealerr_date,sizeof(phv_payin->T_dealerr_date));/* ���ʴ������� */
	memcpy(hv_zf_c->o_orderno,phv_payin->T_o_orderno,sizeof(hv_zf_c->o_orderno)-1);/* ԭ֧���������--�˻㽻���õ�  */
	memcpy(hv_zf_c->o_cmtno,phv_payin->T_o_cmtno,sizeof(hv_zf_c->o_cmtno)-1);/* ԭcmtno--�˻㽻���õ�  */
	memcpy(hv_zf_c->addid,phv_payin->T_addid,sizeof(hv_zf_c->addid)-1);/* ������ id --addit�� */
	memcpy(hv_zf_c->refno,phv_payin->T_refno,sizeof(hv_zf_c->refno)-1);/* ���Ĳο���  */
	memcpy(hv_zf_c->auth_tel,phv_payin->T_auth_tel,sizeof(hv_zf_c->auth_tel)-1);/* ��Ȩ����Ա */
	memcpy(hv_zf_c->chk,phv_payin->T_chk,sizeof(hv_zf_c->chk)-1);/* ���˲���Ա */
	memcpy(hv_zf_c->send_tel,phv_payin->T_send_tel,sizeof(hv_zf_c->send_tel   )-1);/* ���Ͳ���Ա */
	memcpy(hv_zf_c->dealerr_tel,phv_payin->T_dealerr_tel,sizeof(hv_zf_c->dealerr_tel)-1);/* ���ʴ����Ա */
	memcpy(hv_zf_c->resp1,phv_payin->T_resp1,sizeof(hv_zf_c->resp1)-1);/* Ӧ���� */
	vtcp_log("[%s][%d]hv_zf_c->T_hv_brf======[%s]",__FILE__,__LINE__,hv_zf_c->hv_brf);
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	memcpy(hv_zf_c->hv_brf,phv_payin->T_hv_brf,sizeof(hv_zf_c->hv_brf)-1);/* ��ע  */
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
	vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf);
	hv_zf_c->hv_sts[0] = phv_payin->T_hv_sts[0]; 
	vtcp_log(">>>phv_payin->T_hv_sts=[%c]",phv_payin->T_hv_sts[0]);TRACE 
 
	memcpy(hv_zf_c->hv_prt_ind,phv_payin->T_hv_prt_ind,sizeof(phv_payin->T_hv_prt_ind));  /*��ӡ��־ */ 
	/*	vtcp_log("[%s][%d]hv_payin->T_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf); */	
	return 0;
}



int Hv_fzf_Print(struct hv_fzf_c *ps){
	vtcp_log("hv_fzf_c.rowid=[%s]",ps->rowid);
	vtcp_log("hv_fzf_c.or_br_no=[%s]",ps->or_br_no);
	vtcp_log("hv_fzf_c.orderno=[%s]",ps->orderno);
	vtcp_log("hv_fzf_c.tx_date=[%ld]",ps->tx_date);
	vtcp_log("hv_fzf_c.wt_date=[%ld]",ps->wt_date);
	vtcp_log("hv_fzf_c.br_no=[%s]",ps->br_no);
	vtcp_log("hv_fzf_c.hv_fzf_sts=[%s]",ps->hv_fzf_sts);
	vtcp_log("hv_fzf_c.cmtno=[%s]",ps->cmtno);
	vtcp_log("hv_fzf_c.tx_time=[%ld]",ps->tx_time);
	vtcp_log("hv_fzf_c.trace_no=[%ld]",ps->trace_no);
	vtcp_log("hv_fzf_c.pay_qs_no=[%s]",ps->pay_qs_no);
	vtcp_log("hv_fzf_c.ac_br_no=[%s]",ps->ac_br_no);
	vtcp_log("hv_fzf_c.cash_qs_no=[%s]",ps->cash_qs_no);
	vtcp_log("hv_fzf_c.o_tx_date=[%ld]",ps->o_tx_date);
	vtcp_log("hv_fzf_c.o_or_br_no=[%s]",ps->o_or_br_no);
	vtcp_log("hv_fzf_c.o_ac_br_no=[%s]",ps->o_ac_br_no);
	vtcp_log("hv_fzf_c.o_orderno=[%s]",ps->o_orderno);
	vtcp_log("hv_fzf_c.o_cur_ind=[%s]",ps->o_cur_ind);
	vtcp_log("hv_fzf_c.o_tx_amt=[%f]",ps->o_tx_amt);
	vtcp_log("hv_fzf_c.o_cmtno=[%s]",ps->o_cmtno);
	vtcp_log("hv_fzf_c.o_tx_type=[%s]",ps->o_tx_type);
	vtcp_log("hv_fzf_c.sendco=[%s]",ps->sendco);
	vtcp_log("hv_fzf_c.receco=[%s]",ps->receco);
	vtcp_log("hv_fzf_c.req_date=[%ld]",ps->req_date);
	vtcp_log("hv_fzf_c.req_br_no=[%s]",ps->req_br_no);
	vtcp_log("hv_fzf_c.req_content=[%s]",ps->req_content);
	vtcp_log("hv_fzf_c.req_orderno=[%s]",ps->req_orderno);
	vtcp_log("hv_fzf_c.res_date=[%ld]",ps->res_date);
	vtcp_log("hv_fzf_c.res_br_no=[%s]",ps->res_br_no);
	vtcp_log("hv_fzf_c.res_content=[%s]",ps->res_content);
	vtcp_log("hv_fzf_c.res_orderno=[%s]",ps->res_orderno);
	vtcp_log("hv_fzf_c.tx_sts=[%s]",ps->tx_sts);
	vtcp_log("hv_fzf_c.res_sts=[%s]",ps->res_sts);
	vtcp_log("hv_fzf_c.addid=[%s]",ps->addid);
	vtcp_log("hv_fzf_c.refno=[%s]",ps->refno);
	vtcp_log("hv_fzf_c.hv_prt_ind=[%s]",ps->hv_prt_ind);
	vtcp_log("hv_fzf_c.hv_brf=[%s]",ps->hv_brf);
	vtcp_log("hv_fzf_c.tel=[%s]",ps->tel);
	vtcp_log("hv_fzf_c.lw_ind=[%s]",ps->lw_ind);
	return(0);
}


int pub_com_CMT802_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack802)
{    
	/************֧��ҵ���շ��嵥********************************/
	char cTmpBuf[16];	
	double  dTxamt = 0.0;	
	HV_802_MAIN_ADD hv_802_main;

	memset(&hv_802_main,0,sizeof(hv_802_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*֧���������  һ��ֻ��һ�ʣ���ʱд����*/
	
	hv_payin->T_operlevel[0] = pack802->PRI;			/*���޼���*/
	memcpy(hv_payin->T_cmtno,"802",3);					  /*CMT���*/
	memcpy(hv_payin->T_tx_date,pack802->workDate,8);	/*��������*/
	iHVGETTAG( pack802,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*ί������*/
	iHVGETTAG( pack802,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*�ձ����Ĵ���*/	
	iHVGETTAG( pack802,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/	
	iHVGETTAG( pack802,"33H",cTmpBuf,sizeof(cTmpBuf));					/*  ���   û�л��� */

  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);
	
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
  vtcp_log("[%s][%d] cTmpBuf=[%lf]:",__FILE__,__LINE__,dTxamt);	

	/******** ��������**/	
	iHVGETTAG(pack802,"072",hv_802_main.beg_date,sizeof(hv_802_main.beg_date));		/*�Ʒѿ�ʼ����*/
	iHVGETTAG(pack802,"073",hv_802_main.end_date,sizeof(hv_802_main.end_date));		/*�Ʒ���ֹ���� */
	iHVGETTAG(pack802,"FN1",hv_802_main.zjcy_rate,sizeof(hv_802_main.zjcy_rate));	/*ֱ�Ӳ������շѱ���*/
	iHVGETTAG(pack802,"FN2",hv_802_main.hb_rate,sizeof(hv_802_main.hb_rate));		  /*�б��շѱ��� */
	iHVGETTAG(pack802,"FN3",hv_802_main.ywl_rate,sizeof(hv_802_main.ywl_rate));		/*���ۼ�ҵ�����շѱ���*/

	iHVGETTAG(pack802,"EN1",hv_802_main.zf_fee,sizeof(hv_802_main.zf_fee));		/*֧����ҵ���շѽ�� */
	iHVGETTAG(pack802,"ED1",hv_802_main.zf_cnt,sizeof(hv_802_main.zf_cnt));		/*֧����ҵ���շѼ�¼���� */
	iHVGETTAG(pack802,"EN2",hv_802_main.db_fee,sizeof(hv_802_main.db_fee));		/*����ҵ���շѽ��*/
	iHVGETTAG(pack802,"ED2",hv_802_main.db_cnt,sizeof(hv_802_main.db_cnt));		/*����ҵ���շѼ�¼���� */
	iHVGETTAG(pack802,"EN3",hv_802_main.xx_fee,sizeof(hv_802_main.xx_fee));		/*��Ϣ��ҵ���շѽ��*/
	iHVGETTAG(pack802,"ED3",hv_802_main.xx_cnt,sizeof(hv_802_main.xx_cnt));		/*��Ϣ��ҵ���շѼ�¼���� */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_802_main,sizeof(HV_802_MAIN_ADD));
	return 0; 
}

int pub_com_CMT81x_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack81x)
{    
	/************֧��ҵ���շ��嵥********************************/
	char cTmpBuf[16];	
	double  dTxamt = 0.0;	
	HV_81X_MAIN_ADD hv_81x_main;

	memset(&hv_81x_main,0,sizeof(hv_81x_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*֧���������  һ��ֻ��һ�ʣ���ʱд����*/
	
	hv_payin->T_operlevel[0] = pack81x->PRI;			/*���޼���*/
	memcpy(hv_payin->T_cmtno,pack81x->CMTCode,3);					  /*CMT���*/
	memcpy(hv_payin->T_tx_date,pack81x->workDate,8);	/*��������*/
	iHVGETTAG( pack81x,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*ί������*/
	iHVGETTAG( pack81x,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*�ձ����Ĵ���*/	
	iHVGETTAG( pack81x,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/	
	iHVGETTAG( pack81x,"33H",cTmpBuf,sizeof(cTmpBuf));					/*  ���   û�л��� */

  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);
	
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*�Ʒ��ܽ��*/
  vtcp_log("[%s][%d] cTmpBuf=[%lf]:",__FILE__,__LINE__,dTxamt);	

	/******** ��������**/	
	iHVGETTAG(pack81x,"072",hv_81x_main.beg_date,sizeof(hv_81x_main.beg_date));		/*�Ʒѿ�ʼ����*/
	iHVGETTAG(pack81x,"073",hv_81x_main.end_date,sizeof(hv_81x_main.end_date));		/*�Ʒ���ֹ���� */
	iHVGETTAG(pack81x,"33J",hv_81x_main.hv_amt,sizeof(hv_81x_main.hv_amt));		/** ���Ʒѽ�� **/
	iHVGETTAG(pack81x,"33K",hv_81x_main.lv_amt,sizeof(hv_81x_main.lv_amt));			/** С��Ʒѽ�� **/

	iHVGETTAG(pack81x,"ED1",hv_81x_main.hv_cnt,sizeof(hv_81x_main.hv_cnt));		/*�����ߴ��Ʒ���Ŀ */
	iHVGETTAG(pack81x,"ED2",hv_81x_main.lv_cnt,sizeof(hv_81x_main.lv_cnt));		/*������С��Ʒ���Ŀ */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_81x_main,sizeof(HV_81X_MAIN_ADD));
	return 0; 
}



/******�շ�803�嵥����**END ADD BY ChenMing*****/
int pub_com_CMT803_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack803)
{    
	/************�ռ�͸֧��Ϣ����********************************/
	char cRecCnt[5];	
	char cTmpBuf[23];	
	char cTmpBuf_CLD[MAX_803_REC*56+10];	
	double  dTxamt = 0.0;	
	
	
	HV_803_MAIN_ADD hv_803_main;
  vtcp_log("pub_com_CMT803_TAG2T() ��ʼ  !  "); 
	memset(&hv_803_main,0,sizeof(hv_803_main));
	memset(cTmpBuf, 0 , sizeof(cTmpBuf));
	memset(cTmpBuf_CLD, 0 , sizeof(cTmpBuf_CLD));	
	memset(cRecCnt, 0 , sizeof(cRecCnt));	
	
	memcpy(hv_payin->T_orderno,"00000001",8);			/*֧���������  һ��ֻ��һ�ʣ���ʱд����*/
	
	hv_payin->T_operlevel[0] = pack803->PRI;			/*���޼���*/
	memcpy(hv_payin->T_cmtno,"803",3);					  /*CMT���*/
	memcpy(hv_payin->T_tx_date,pack803->workDate,8);	/*��������*/
	iHVGETTAG( pack803,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		  /*ί������*/
	
	iHVGETTAG( pack803,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		    /*�ձ����Ĵ���*/	
	iHVGETTAG( pack803,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));	  /*�������к�*/	
	iHVGETTAG( pack803,"CC3",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	  /*�������к�*/	
	/*  ͸֧��Ϣ�ϼ�  22 λ */
	iHVGETTAG( pack803,"0B4",cTmpBuf,sizeof(cTmpBuf));	
  vtcp_log("[%s][%d] cTmpBuf=[%s]:",__FILE__,__LINE__,cTmpBuf);	
	dot_str2dbl(cTmpBuf,sizeof(cTmpBuf),&dTxamt);	
	dTxamt = dTxamt/100;
  vtcp_log("[%s][%d] dTxamt=[%lf]:",__FILE__,__LINE__,dTxamt);		
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	 
  vtcp_log("hv_payin->T_tx_amt=[%.15s]:",hv_payin->T_tx_amt); 
  	
	/******** ��������**/	
	iHVGETTAG(pack803,"CD4",hv_803_main.tag_CD4,sizeof(hv_803_main.tag_CD4));	/* ��Ϣʱ���� */
  vtcp_log("hv_803_main.tag_CD4=[%.4s] ",hv_803_main.tag_CD4 ); 
  memcpy(cRecCnt,hv_803_main.tag_CD4,4 );
  if(atoi(cRecCnt) > 30)
  {
     vtcp_log(" CD4  ���¼������ MAX_803_REC���Ų����ˣ�����  ");   	
	   return -1; 
  }
  
	iHVGETTAG(pack803,"CLD",hv_803_main.tag_CLD,sizeof(MAX_803_REC*56));	  /* ��Ϣ��Ϣ   */

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_803_main,sizeof(HV_803_MAIN_ADD));
  vtcp_log("pub_com_CMT803_TAG2T() �ɹ�����!  "); 
	return 0; 
}



/*******************************************************************************
* ��������: iHvHang_Check_Brno                                                 *
* ��    ��: ���ʼ�飬��Ҫ�ǻ�����״̬ 
            ���������ڣ���״̬������������Ϊ��������
* ��    ��:                                                                    *
* ��    ��:                                                                    *
* �� �� ֵ:int                                                                 *
*******************************************************************************/
int iHvHang_Check_Brno( char* cOpn_br_no, char* cKinbr)
{
	int  ret;
	char stat;
  struct com_branch_c sCom_branch;
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	
	if(cOpn_br_no)  /* �п����У������ʺŵĿ�����Ϊ׼ */	
	{
    	memcpy(sCom_branch.br_no,cOpn_br_no,BRNO_LEN);
    	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
    	if(ret == 100)  /* �����в����� */
    	{
        	vtcp_log("%s,%d �����в�����   ret=[%d]",__FILE__,__LINE__,ret);
    		  if(strlen(cKinbr) >0 ) /* �н��ջ����� */
    		  {
            	memcpy(sCom_branch.br_no,cKinbr,BRNO_LEN);
            	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
            	if(ret == 100)  /* ���ջ���������*/
            	{
            		 vtcp_log("%s,%d �����в����ڣ����ջ���Ҳ������,���ý��ջ���Ϊ��������ret=[%d]",__FILE__,__LINE__,ret);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);      		 
                 stat = RECEIVE_DATA_ERR;    /* �������Ĺ���  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0;    		 		
            	}
            	else if(ret)
            	{
                  sprintf(acErrMsg,"���ݿ����ʧ��[%d]",g_reply_int);
                  WRITEMSG
                  return -1;
            	}		
            	
            	if(sCom_branch.wrk_sts[0]=='*')
            	{
            		 vtcp_log("%s,%d ���ջ���[%s]״̬���ر�",__FILE__,__LINE__,sCom_branch.br_no);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);     		 
                 stat = RECEIVE_DATA_ERR;  /* �������Ĺ���  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0; 				
            	}

            	vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
        		  set_zd_data(DC_TX_BR_NO,cKinbr);	
        		  set_zd_data("0020",cKinbr);
        		  return 0;		  	
    		  }
    		  else  /* û�н��ջ����� */
    		  {
            		 vtcp_log("%s,%d �����в����ڣ����ջ���Ҳ������,���ý��ջ���Ϊ��������ret=[%d]",__FILE__,__LINE__,ret);
            		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
        		     set_zd_data(DC_TX_BR_NO,cKinbr);	
        		     set_zd_data("0020",cKinbr);      		 
                 stat = RECEIVE_DATA_ERR;    /* �������Ĺ���  */
            		 set_zd_data("0980",&stat); 
        		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
        		     return 0; 	
    		  }
    	}
    	else if(ret)
    	{
          sprintf(acErrMsg,"���ݿ����ʧ��[%d]",g_reply_int);
          WRITEMSG
          return -1;
    	}
    	if(sCom_branch.wrk_sts[0]=='*')
    	{
    		 vtcp_log("%s,%d ���ջ���[%s]״̬���ر�",__FILE__,__LINE__,sCom_branch.br_no);
    		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
		     set_zd_data(DC_TX_BR_NO,cKinbr);	
		     set_zd_data("0020",cKinbr); 
         stat = RECEIVE_DATA_ERR;    /* �������Ĺ���  */
    		 set_zd_data("0980",&stat); 
		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
		     return 0; 
    	}
    	
   	 vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cOpn_br_no);
   	 /*memcpy(cKinbr, cOpn_br_no, BRNO_LEN);*/
		 set_zd_data(DC_TX_BR_NO,cOpn_br_no);	
		 set_zd_data("0020",cOpn_br_no);
		 return 0;
	}
	else   /* û�п����У����Խ��ջ�����Ϊ׼ */	
	{
		  if(strlen(cKinbr) >0 )  /* ���ջ����Ŵ��� */
		  {
        	memcpy(sCom_branch.br_no,cKinbr,BRNO_LEN);
        	ret=Com_branch_Sel(g_pub_tx.reply,&sCom_branch,"br_no='%s'",sCom_branch.br_no);
        	if(ret == 100)
        	{
        		 vtcp_log("%s,%d ���ջ���������,���ý��ջ���Ϊ��������ret=[%d]",__FILE__,__LINE__,ret);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);      		 
             stat = RECEIVE_DATA_ERR;    /* �������Ĺ���  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0;    		 		
        	}
        	else if(ret)
        	{
              sprintf(acErrMsg,"���ݿ����ʧ��[%d]",g_reply_int);
              WRITEMSG
              return -1;
        	}		
        	
        	if(sCom_branch.wrk_sts[0]=='*')
        	{
        		 vtcp_log("%s,%d ���ջ���[%s]״̬���ر�",__FILE__,__LINE__,sCom_branch.br_no);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);     		 
             stat = RECEIVE_DATA_ERR;   /* �������Ĺ���  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0; 				
        	}
        	
        	vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
    		  set_zd_data(DC_TX_BR_NO,cKinbr);	
    		  set_zd_data("0020",cKinbr);
    		  return 0;		  	
		  }
		  else  /* ���ջ����Ų����� */
		  {
        		 vtcp_log("%s,%d �������������������ʻ���Ҳ�����ڣ�������������Ϊ���׻��� ",__FILE__,__LINE__);
        		 memcpy(cKinbr,QS_BR_NO,BRNO_LEN);
    		     set_zd_data(DC_TX_BR_NO,cKinbr);	
    		     set_zd_data("0020",cKinbr);     		 
             stat = RECEIVE_DATA_ERR;       /* ���ջ�������ʱ���������  */
        		 set_zd_data("0980",&stat); 
    		     vtcp_log("%s,%d �õ��µĽ��׻���===[%s]\n",__FILE__,__LINE__,cKinbr);
    		     return 0; 		
		  }
	}

  return 0;
}



/*******************************************************************************
* ��������: iHvHang_Check_Acno                                                 *
* ��    ��: ���ʼ�飬����ʺż�״̬���Լ����� 
            ���ú�һЩ8583��Ϊ����׼��
* ��    ��:                                                                    *
* ��    ��:                                                                    *
* �� �� ֵ:int                                                                 *
*******************************************************************************/
int iHvHang_Check_Acno(char* ac_no, char* ac_name, char* cKinbr)
{
  char stat;
  char cGZflag[2];
  int ret;
  struct com_branch_c sCom_branch;
	char cQs_br_no[BRNO_LEN+1];
  char  acc_v[33];
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;  
  int   ac_seqn_v; 
	char    cTempstr    [1024];
	
	char cOldStat[2];
  memset(cOldStat, 0, sizeof(cOldStat));        
	get_zd_data("0980",cOldStat);     /* ��¼ԭ״̬ */
  vtcp_log("[%s][%d] cOldStat===[%s] ",__FILE__,__LINE__,cOldStat);
  	
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));  
  memset(acc_v,0x00,sizeof(acc_v));  

  vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name); 
  memset(cTempstr,0,sizeof(cTempstr));
  memcpy(cTempstr, ac_name, 60);
  dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
  memcpy(ac_name, cTempstr, 60);
  vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
        
  /* ========= �տ����˺ż�鿪ʼ  ==========*/
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
  memcpy(acc_v, ac_no ,sizeof(acc_v)-1);
  
  /*���˺�ת���˺�*/
  if(pub_base_old_acno(acc_v))
  {
	   sprintf(acErrMsg,"pub_base_old_acno() ����!! [%d]");
		 WRITEMSG
		return -1;
	}
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);

    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)   /* 1. �ʺŲ���ʱ */
    {
	      vtcp_log("%s,%d  �˺�[%s]������,֧�й���  ",__FILE__,__LINE__,acc_v);    	
              
			  set_zd_data("0680","6"); /* �ʻ����� */        
        /* �ʺŲ���ʱ�����ù��ʻ��� */
        ret = iHvHang_Check_Brno(0, cKinbr);
        if(ret)
        {
           sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
        }
        get_zd_data(DC_TX_BR_NO,cKinbr);             
        vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);         
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        } 

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    vtcp_log("[%s][%d] cKinbr===[%s]  QS_BR_NO=[%s]",__FILE__,__LINE__,cKinbr,QS_BR_NO); 
    vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);    


    /* 2. �ʺŴ�ʱ�������ʺŵĿ�������  ���������ʺŵĿ�������Ϊ׼    */
    ret = iHvHang_Check_Brno(mdm_ac_rel_v.opn_br_no, cKinbr);
    if(ret)
    {
           sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
    }
    get_zd_data(DC_TX_BR_NO,cKinbr);             
    vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);

   /*  ��������������ű����ã���ʾû���ҵ������ţ������������   
    if( memcmp(cKinbr,mdm_ac_rel_v.opn_br_no, BRNO_LEN) )  
    {
        stat = RECEIVE_CUSTNAME_ERR;
	      vtcp_log("%s,%d  �����ű�����: opn_br_no=[%s]     cKinbr=[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no,cKinbr);        
 			  set_zd_data("0680","6");        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }   */  
    
    /* ============= ���»�������ȷ�� ============= */

    /*  �жϽ���״̬ */
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
    {
        /* ����״̬���������ʴ��� */

	      vtcp_log("%s,%d  �˺�[%s]����״̬����  ",__FILE__,__LINE__,acc_v);        
 			  set_zd_data("0680","6");/* �ʻ����� */        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }

    /*  �Ƿ���ڴ� */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)  /* �ǻ��ڴ� */
    {
			  set_zd_data("0680","6");/* �ʻ����� */           
        vtcp_log("��������տ����˺ŷǻ��ڴ�[%s],����!",acc_v);
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 

        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        sprintf(acErrMsg,"������˼���տ����˺�[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    /* �ڲ��ʻ�����黧�� */
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
        ac_seqn_v  =  1; 
        set_zd_data("0680","1");
                   
        g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
        if (g_reply_int )
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"��ѯ��������տ����˺Ż������ļ���sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
            WRITEMSG
            return -1;
        }
        if (dd_mst_v.ac_sts[0] != '1' )
        {
            sprintf(acErrMsg,"��������տ����˺�״̬������ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  

            set_zd_data("0680","6");/* �ʻ����� */
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
        /*  �ж��Ƿ񱻶���  */
        if(dd_mst_v.hold_sts[0]=='1' )
        {
            /* �ʺ�id����ȫ��������Ҫ���� */
            sprintf(acErrMsg,"��������տ����˺�״̬������ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* �ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
                
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        

        /*  �ж��Ƿ��ǽ��㻧 */
        if(dd_mst_v.sttl_type[0]=='S')
        {
            sprintf(acErrMsg,"��������տ����˺ŷǽ��㻧sttl_type[%s]",dd_mst_v.sttl_type);
            WRITEMSG
            set_zd_data("0680","6");/* �ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }

        pub_base_strpack(dd_mst_v.name);
        pub_base_strpack(ac_name);
        
        vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, dd_mst_v.name, sizeof(dd_mst_v.name)-1);
        dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
        memcpy(dd_mst_v.name, cTempstr, sizeof(dd_mst_v.name)-1);
        vtcp_log("[%s][%d] dd_mst_v.name==[%.60s] ",__FILE__,__LINE__,dd_mst_v.name);

        vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name); 
        memset(cTempstr,0,sizeof(cTempstr));
        memcpy(cTempstr, ac_name, 60);
        dbc_zipspace(cTempstr);  /*ѹ��ȫ�ǿո� 2006-11-7 23:45  */
        memcpy(ac_name, cTempstr, 60);
        vtcp_log("[%s][%d] ac_name==[%.60s] ",__FILE__,__LINE__,ac_name);
          
        
        if (strcmp(dd_mst_v.name,ac_name))
        {
            sprintf(acErrMsg,"��������տ������Ʋ���ȷ,�˺�ac_id[%ld]����[%s]������[%s]", dd_mst_v.ac_id,dd_mst_v.name,ac_name);
            WRITEMSG
            set_zd_data("0680","6");/* �ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  

            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }
    else /* ����ڲ��ʻ� */
    {
        struct in_mst_c sIn_mst;
        memset(&sIn_mst,0,sizeof(sIn_mst));
        set_zd_data("0680","6");/* �����ʻ����� */
                
        g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
        if (g_reply_int)
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"��ѯ��������տ����˺��ڲ������ļ���sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
            WRITEMSG
            return -1;
        }
        
        if (sIn_mst.sts[0] != '1' )
        {
            sprintf(acErrMsg,"��������տ����˺�״̬������ ����! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* �����ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }

    memset(cOldStat, 0, sizeof(cOldStat));        
	  get_zd_data("0980",cOldStat);     /* ��¼ԭ״̬ */
    vtcp_log("[%s][%d] û�й��ʣ�״̬Ӧ�ò���=='9' ?  cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	

	  vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
    set_zd_data("0300", acc_v );  /* �տ����˺� */ 
	  return 0;	
}


/*******************************************************************************
* ��������: iHvHang_Check_AcnoNoName                                                 *
* ��    ��: ���ʼ�飬ֻ����ʺż�״̬ 
            ���ú�һЩ8583��Ϊ����׼��
* ��    ��:                                                                    *
* ��    ��:                                                                    *
* �� �� ֵ:int                                                                 *
*******************************************************************************/
int iHvHang_Check_AcnoNoName(char* ac_no, char* cKinbr)
{
  char stat;
  char cGZflag[2];
  int ret;
  struct com_branch_c sCom_branch;
	char cQs_br_no[BRNO_LEN+1];
  char  acc_v[33];
  struct mdm_ac_rel_c mdm_ac_rel_v;
  struct prdt_ac_id_c prdt_ac_id_v;
  struct dd_mst_c dd_mst_v;  
  int   ac_seqn_v; 

	char cOldStat[2];
  memset(cOldStat, 0, sizeof(cOldStat));        
	get_zd_data("0980",cOldStat);     /* ��¼ԭ״̬ */
  vtcp_log("[%s][%d] cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	
  	
	memset(&sCom_branch, 0 , sizeof(sCom_branch));
	memset(cQs_br_no, 0 ,sizeof(cQs_br_no));  
  memset(acc_v,0x00,sizeof(acc_v));  

  /* ========= �տ����˺ż�鿪ʼ  ==========*/
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);
  memcpy(acc_v, ac_no ,sizeof(acc_v)-1);
  
  /*���˺�ת���˺�*/
  if(pub_base_old_acno(acc_v))
  {
	   sprintf(acErrMsg,"pub_base_old_acno() ����!! [%d]");
		 WRITEMSG
		return -1;
	}
	vtcp_log("%s,%d  ac_no == [%s] === ",__FILE__,__LINE__,ac_no);

    memset( &mdm_ac_rel_v, 0x00, sizeof( struct mdm_ac_rel_c ) );
    g_reply_int = Mdm_ac_rel_Sel(g_pub_tx.reply,&mdm_ac_rel_v,"ac_no='%s'",acc_v);
    if (g_reply_int==100)   /* 1. �ʺŲ���ʱ */
    {
	      vtcp_log("%s,%d  �˺�[%s]������,֧�й���  ",__FILE__,__LINE__,acc_v);    	
              
			  set_zd_data("0680","6"); /* �ʻ����� */        
        /* �ʺŲ���ʱ�����ù��ʻ��� */
        ret = iHvHang_Check_Brno(0, cKinbr);
        if(ret)
        {
           sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
        }
        get_zd_data(DC_TX_BR_NO,cKinbr);             
        vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);
        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);         
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        } 

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        stat = RECEIVE_CUSTNAME_ERR;
        sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    vtcp_log("[%s][%d] cKinbr===[%s]  QS_BR_NO=[%s]",__FILE__,__LINE__,cKinbr,QS_BR_NO); 
    vtcp_log("[%s][%d] mdm_ac_rel_v.opn_br_no===[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no);    


    /* 2. �ʺŴ�ʱ�������ʺŵĿ�������  ���������ʺŵĿ�������Ϊ׼    */
    ret = iHvHang_Check_Brno(mdm_ac_rel_v.opn_br_no, cKinbr);
    if(ret)
    {
           sprintf(acErrMsg,"[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
           WRITEMSG
           return -1;
    }
    get_zd_data(DC_TX_BR_NO,cKinbr);             
    vtcp_log("%s,%d �õ��Ľ��׻�����[%s]",__FILE__,__LINE__,cKinbr);

   /*  ��������������ű����ã���ʾû���ҵ������ţ������������   
    if( memcmp(cKinbr,mdm_ac_rel_v.opn_br_no, BRNO_LEN) )  
    {
        stat = RECEIVE_CUSTNAME_ERR;
	      vtcp_log("%s,%d  �����ű�����: opn_br_no=[%s]     cKinbr=[%s] ",__FILE__,__LINE__,mdm_ac_rel_v.opn_br_no,cKinbr);        
 			  set_zd_data("0680","6");        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }   */  
    
    /* ============= ���»�������ȷ�� ============= */

    /*  �жϽ���״̬ */
    if(mdm_ac_rel_v.note_sts[0]!='0' && mdm_ac_rel_v.note_sts[0]!='1' && mdm_ac_rel_v.note_sts[0]!='3' )
    {
        /* ����״̬���������ʴ��� */

	      vtcp_log("%s,%d  �˺�[%s]����״̬����  ",__FILE__,__LINE__,acc_v);        
 			  set_zd_data("0680","6");/* �ʻ����� */        
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }

    /*  �Ƿ���ڴ� */
    memset(&prdt_ac_id_v,0x00,sizeof(struct prdt_ac_id_c));
    g_reply_int = Prdt_ac_id_Sel( g_pub_tx.reply, &prdt_ac_id_v,"ac_id='%ld' and (ac_id_type='1' or ac_id_type='9')", \
        mdm_ac_rel_v.ac_id );
    if (g_reply_int==100)  /* �ǻ��ڴ� */
    {
			  set_zd_data("0680","6");/* �ʻ����� */           
        vtcp_log("��������տ����˺ŷǻ��ڴ�[%s],����!",acc_v);
        vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 

        memset(cGZflag,0x00,sizeof(cGZflag));        
        get_zd_data("0980",cGZflag);
        vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
        if( cGZflag[0] != RECEIVE_DATA_ERR)
        {
            stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
    		    set_zd_data("0980",&stat);             
        }  

        memset(acc_v,0x00,sizeof(acc_v));                                      
        ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
        if(ret)
        {
          		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
          		return -1;
        } 
        vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
        set_zd_data("0300",acc_v);        
        return 0;
    }
    else if (g_reply_int)
    {
        sprintf(acErrMsg,"������˼���տ����˺�[%s]���ݿ����ʧ��[%d]",acc_v,g_reply_int);
        WRITEMSG
        return -1;
    }

    /* �ڲ��ʻ�����黧�� */
    if(prdt_ac_id_v.ac_id_type[0]=='1')
    {
        ac_seqn_v  =  1; 
        set_zd_data("0680","1");
                   
        g_reply_int = Dd_mst_Sel(g_pub_tx.reply,&dd_mst_v,"ac_id='%ld' and ac_seqn='%d'",mdm_ac_rel_v.ac_id,ac_seqn_v);
        if (g_reply_int )
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"��ѯ��������տ����˺Ż������ļ���sqlcode=[%d] ac_id[%ld] ac_seqn[%d]",g_reply_int,mdm_ac_rel_v.ac_id,ac_seqn_v);
            WRITEMSG
            return -1;
        }
        
        if (dd_mst_v.ac_sts[0] != '1' )
        {
            sprintf(acErrMsg,"��������տ����˺�״̬������ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr);
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  

            set_zd_data("0680","6");/* �ʻ����� */
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        
        
        /*  �ж��Ƿ񱻶���  */
        if(dd_mst_v.hold_sts[0]=='1' )
        {
            /* �ʺ�id����ȫ��������Ҫ���� */
            sprintf(acErrMsg,"��������տ����˺�״̬������ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* �ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
                
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }        

        /*  �ж��Ƿ��ǽ��㻧 */
        if(dd_mst_v.sttl_type[0]=='S')
        {
            sprintf(acErrMsg,"��������տ����˺ŷǽ��㻧sttl_type[%s]",dd_mst_v.sttl_type);
            WRITEMSG
            set_zd_data("0680","6");/* �ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }

    }
    else /* ����ڲ��ʻ� */
    {
        struct in_mst_c sIn_mst;
        memset(&sIn_mst,0,sizeof(sIn_mst));
        set_zd_data("0680","6");/* �����ʻ����� */
                
        g_reply_int = In_mst_Sel(g_pub_tx.reply,&sIn_mst,"ac_id='%ld' and ac_seqn='0'",mdm_ac_rel_v.ac_id);
        if (g_reply_int)
        {
            stat = RECEIVE_CUSTNAME_ERR;
            sprintf(acErrMsg,"��ѯ��������տ����˺��ڲ������ļ���sqlcode=[%d] ac_id[%ld] ac_seqn[0]",g_reply_int,mdm_ac_rel_v.ac_id);
            WRITEMSG
            return -1;
        }
        
        if (sIn_mst.sts[0] != '1' )
        {
            sprintf(acErrMsg,"��������տ����˺�״̬������ ����! ac_sts[%s]hold_sts[%s]",dd_mst_v.ac_sts,dd_mst_v.hold_sts);
            WRITEMSG
            set_zd_data("0680","6");/* �����ʻ����� */            
            vtcp_log("[%s][%d] cKinbr===[%s] ",__FILE__,__LINE__,cKinbr); 
            memset(cGZflag,0x00,sizeof(cGZflag));        
            get_zd_data("0980",cGZflag);
            vtcp_log("[%s][%d] cGZflag===[%s] ",__FILE__,__LINE__,cGZflag);    
            if( cGZflag[0] != RECEIVE_DATA_ERR)
            {
                stat = RECEIVE_CUSTNAME_ERR;    /* ֧�й���  */
        		    set_zd_data("0980",&stat);             
            }  
            
            memset(acc_v,0x00,sizeof(acc_v));                                      
            ret = pub_base_GetParm_Str(cKinbr,PARM_HVGZ,acc_v);
            if(ret)
            {
              		vtcp_log("%s,%d ϵͳ���ô��󣬵õ�����%s�Ĵ������ʺŴ���[%d]",__FILE__,__LINE__,ret);
              		return -1;
            } 
            vtcp_log("[%s][%d]   acc_v===[%s]  ",__FILE__,__LINE__,acc_v);  
            set_zd_data("0300",acc_v);        
            return 0;
        }
    }

    memset(cOldStat, 0, sizeof(cOldStat));        
	  get_zd_data("0980",cOldStat);     /* ��¼ԭ״̬ */
    vtcp_log("[%s][%d] û�й��ʣ�״̬Ӧ�ò���=='9' ?  cOldStat===[%s] ",__FILE__,__LINE__,cOldStat); 	

	  vtcp_log("%s,%d  acc_v == [%s] === ",__FILE__,__LINE__,acc_v);
    set_zd_data("0300", acc_v );  /* �տ����˺� */ 
	  return 0;	
}


int iHv_HPCMT_proc( THVPACKAGE* phv_pack)
{
    int ret;
    struct hv_poinfo_c hv_poinfo;   
    char cPay_br_no[13];
    char cPo_no[9];
    char cLast_sts;
    
     memset(&hv_poinfo, 0 , sizeof(hv_poinfo));
     memset(cPay_br_no, 0 , sizeof(cPay_br_no));    
     memset(cPo_no, 0 , sizeof(cPo_no));    
     hv_get_tag(phv_pack,"CC4",cPay_br_no);
     hv_get_tag(phv_pack,"21A",cPo_no);  
     vtcp_log("%s,%d cPay_br_no===[%s]",__FILE__,__LINE__,cPay_br_no);    
     vtcp_log("%s,%d cPo_no    ===[%s]",__FILE__,__LINE__,cPo_no);      
      
    ret = Hv_poinfo_Dec_Upd(g_pub_tx.reply," po_no='%s' and pay_br_no='%s' ", cPo_no, cPay_br_no);
    if (ret)
    {
        sprintf(acErrMsg,"DECLARE FOR UPDATE ERROR !! [%d]",ret);
        WRITEMSG
        return ret;
    }
    
    ret = Hv_poinfo_Fet_Upd(&hv_poinfo, g_pub_tx.reply);
    if (ret == 100)
    {
        sprintf(acErrMsg,"��Ʊ�ǼǱ����޴˼�¼!![%d]",ret);
        WRITEMSG
        Hv_poinfo_Clo_Upd();     
        return 0;
    }else if(ret )
    {
        sprintf(acErrMsg,"��ѯ��Ʊ�ǼǱ����!![%d]",ret);
        WRITEMSG
        return ret;
    }

    cLast_sts = hv_poinfo.po_sts[0]; /* ��Ʊ����һ��״̬ */
    hv_poinfo.l_tx_date = g_pub_tx.tx_date;
    hv_poinfo.td_cnt ++;
    
    vtcp_log("[%s][%d] CMTCode== [%s] ",__FILE__,__LINE__,phv_pack->CMTCode);   
    if(memcmp(phv_pack->CMTCode,"122",sizeof(phv_pack->CMTCode) )==0)
    {
        if(hv_poinfo.po_sts[0] == HV_HP_JQ) /* 725���յ� ��ȫ����� */
        {
            vtcp_log(" [%s][%d] ��ȫ����� 725���յ� ���ø�����!  ",__FILE__,__LINE__); 
            Hv_poinfo_Clo_Upd();  
            return 0; 
        }
        else if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH )  /* ����Ҹ� */  
        {
            hv_poinfo.po_sts[0] = HV_HP_CASH;           /* ��Ʊ�Ҹ� */
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        }        
    }else if(memcmp(phv_pack->CMTCode,"123",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER ||
            hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_CASH   )  
        {
           hv_poinfo.po_sts[0] =  HV_HP_PARTBACK;   /* �����˻� */
           double dCash_amt=0.0;
           double dRem_amt=0.0;
           char   cCash_amt[16];
           /*char   cRem_amt[16];  del by  martin 2009/4/29*/
			     char   cRem_amt[19];
			     char   cTemp_amt[16];/**add by martin 2009/4/29**/

           memset(cCash_amt,0, sizeof(cCash_amt) );        
           memset(cRem_amt,0, sizeof(cRem_amt) ); 
           
           hv_get_tag(phv_pack,"32A",cRem_amt);   /* �˻ؽ��==ʣ����*/
           hv_get_tag(phv_pack,"CNV",cCash_amt); /* ʵ�ʽ�����*/
           memcpy(cTemp_amt,cRem_amt+3,sizeof(cTemp_amt)-1);
           
           vtcp_log("[%s][%d]   cCash_amt===[%s]  ",__FILE__,__LINE__, cCash_amt);             
           vtcp_log("[%s][%d]   cRem_amt===[%s]  ",__FILE__,__LINE__, cRem_amt);      
           vtcp_log("[%s][%d]   cTemp_amt===[%s]  ",__FILE__,__LINE__,cTemp_amt );
               
            /*str2dbl(cCash_amt,strlen(cCash_amt),0,&dCash_amt);  ʵ�ʽ�����   */
            str2dbl(cCash_amt,strlen(cTemp_amt),0,&dCash_amt); /* ʵ�ʽ�����  modified by martin */
            /*str2dbl(cRem_amt,strlen(cRem_amt),0,&dRem_amt);     �˻ؽ��==ʣ����*/  
            str2dbl(cTemp_amt,strlen(cTemp_amt),0,&dRem_amt); /**�˻ؽ�� modified by martin*/
            dCash_amt = dCash_amt/100;
            dRem_amt = dRem_amt/100;   
           vtcp_log("[%s][%d]   dCash_amt===[%lf]  ",__FILE__,__LINE__, dCash_amt);             
           vtcp_log("[%s][%d]   dRem_amt===[%lf]  ",__FILE__,__LINE__, dRem_amt);             
           hv_poinfo.cash_amt = dCash_amt ;   /* ʵ�ʽ����� */           
           hv_poinfo.rem_amt = dRem_amt;   /* �˻ؽ�� */ 
           hv_poinfo.pay_date =  g_pub_tx.tx_date;         
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        } 
    }else if(memcmp(phv_pack->CMTCode,"124",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_CASH ||
            hv_poinfo.po_sts[0] == HV_HP_ASKRETURN || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER )  
        {
            long iPay_date;
            vtcp_log("[%s][%d]   hv_poinfo.sign_date===[%d]  ",__FILE__,__LINE__, hv_poinfo.sign_date);      
            vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
            pub_base_deadlineM(hv_poinfo.sign_date,1,&iPay_date);
            vtcp_log("[%s][%d]   g_pub_tx.tx_date===[%d]  ",__FILE__,__LINE__, g_pub_tx.tx_date);      
            vtcp_log("[%s][%d]   iPay_date===[%d]  ",__FILE__,__LINE__, iPay_date);      
            if(iPay_date < g_pub_tx.tx_date)
            {
               hv_poinfo.po_sts[0] = HV_HP_OVERDATE_UNTREAD;  /* ����δ���˻� */
            }
            else
            {
               hv_poinfo.po_sts[0] = HV_HP_FULLBACK;  /* δ���˻� */
            }  
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        } 

    }else if(memcmp(phv_pack->CMTCode,"725",sizeof(phv_pack->CMTCode) )==0)
    {
         if( hv_poinfo.po_sts[0] == HV_HP_ASKCASH || hv_poinfo.po_sts[0] == HV_HP_MV_CENTER || hv_poinfo.po_sts[0] == HV_HP_CASH)  
        {
        	 hv_poinfo.pay_date =  g_pub_tx.tx_date; 
           hv_poinfo.po_sts[0] =  HV_HP_JQ;  /* ȫ����� */
        }
        else
        {
            Hv_poinfo_Clo_Upd();  
            sprintf(acErrMsg," ��Ʊ��¼��ǰ״̬���� ! hv_poinfo.po_sts=[%s] ",hv_poinfo.po_sts);
            WRITEMSG
            return 0;
        }
    }else
    {
        sprintf(acErrMsg,"CMTCode== [%s] �������ﴦ��! ",phv_pack->CMTCode);
        WRITEMSG
        return -1;
    }
    hv_poinfo.l_po_sts[0] = cLast_sts;
    ret = Hv_poinfo_Upd_Upd(hv_poinfo, g_pub_tx.reply);
    if (ret != 0)
    {
        sprintf(acErrMsg,"HV_POINFO_UPD_UPD ERROR ! [%d]",ret);
        WRITEMSG
        return ret;
    }    
    Hv_poinfo_Clo_Upd();  
    vtcp_log("[%s][%d] ����֧�����ױ��¼�ɹ�",__FILE__,__LINE__);      
    return 0;  
}

