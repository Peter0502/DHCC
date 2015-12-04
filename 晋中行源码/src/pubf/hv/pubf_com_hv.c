#include <stdlib.h>
#include <stdio.h> 
#include <math.h>
#define EXTERN
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "hvpack.h"
#include "hv_addit_c.h"
#include "hv_orno_brno_c.h"
#include "hv_brno_orno_c.h"
#include "hv_zfhtr_c.h"
#include "hv_fzf_c.h"
#include "hv_fzfhtr_c.h"
#include "hv_sendlist_c.h"
#include "seqctl_c.h"
#include "hv_define.h"
#include "find_debug.h"

int iHvStr2Dbl(char* cAmt,double* dAmt);
int  del_dot( char *source,char *target,int size);
void vGetRecvTagTxamt(char *tagamt,char *newtxamt);
#define MAX_BUF 1000
int dot_str2dbl(char * str,int size,double *d_value);
int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length);
int iGetHvOrderno(char * orderno);
int iGet_full_hv_no(char *or_br_no, char *sendco,char * qs_br_no);
int iHVSETTAG(THVPACKAGE * pack,char * tag,char * content,int size);
char* hv_getcmtdesc(char *cmtno);
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur);

/* ��������:��ǰ̨2λ�ı���ת��Ϊ��̨mbfe�ı��� *
 * ��������:vCur_dhcc_mbfe                      *
 * ��    ��:char *fd_cur,char *mbfe_cur         *
 * ��������:8583�ֵı��֣�mbfe�ı���            *
 * ��    ��:LiuHuafeng 2006-8-21 16:21          *
 * ��    ��:
*************************************************/
void vCur_dhcc_mbfe(char *fd_cur,char *mbfe_cur)
{
	if(!memcmp(fd_cur,"01",2))
	{
		MEMCPY_DEBUG(mbfe_cur,"RMB",3);
	}
	else
	{
		MEMCPY_DEBUG(mbfe_cur,"RMB",3);
	}
}
/************************************************
 * ��������:��8583�ֵ�����ת��Ϊhv_payin��      *
 * ��������:vCur_dhcc_mbfe                      *
 * ��    ��:HV_PAYIN_AREA * phv_payin           *
 * ��������:hv_payin�ṹ                        *
 * ��    ��:LiuHuafeng 2006-8-21 16:21          *
 * ��    ��:                                    *
*************************************************/
int iHvTistoPayin(HV_PAYIN_AREA * phv_payin)
{
	HV_FD123_AREA fd123;
	char cBuff4[60];
	char cOrderno[9];
	char cInd[2];
	char cCur_no[3];
	char cTistemp[9];
	char cTistemp2[9];
	int iRet=0;
	int i=0;
	MEMSET_DEBUG(cCur_no , 0 , sizeof(cCur_no));
	MEMSET_DEBUG(cInd    , 0 , sizeof(cInd));
	MEMSET_DEBUG(cOrderno, 0 , sizeof(cOrderno));
	MEMSET_DEBUG(cTistemp, 0 , sizeof(cTistemp));
	MEMSET_DEBUG(cTistemp2, 0 , sizeof(cTistemp2));
	MEMSET_DEBUG(&fd123  , 0 , sizeof(fd123));
	MEMSET_DEBUG(cBuff4,'\0',sizeof(cBuff4));
	
	get_fd_data("1230",(char*)&fd123);
	vtcp_log("123��=[%s]",(char*)&fd123.cmtno);TRACE 
	vtcp_log("[%s][%d]sizeof(fd123)=[%d]\n",__FILE__,__LINE__,sizeof(fd123));

	/* ���׻��� */ 
	get_fd_data("0030",phv_payin->F_tx_br_no);
	vtcp_log("ǰ̨������brno=[%.5s]",phv_payin->F_tx_br_no);TRACE 

	/**ί������       **/
	get_fd_data("0440",phv_payin->F_wt_date);
	vtcp_log("ǰ̨������wt_date=[%.8s]",phv_payin->F_wt_date);TRACE 
	get_fd_data("0050",phv_payin->F_tx_date);
	/* ���� */
	get_fd_data("0210",cCur_no);
	vCur_dhcc_mbfe(cCur_no,phv_payin->F_cur_no);
	/* ���׽�� */
	get_fd_data("0390",phv_payin->F_tx_amt);
	get_zd_double("0390",&g_pub_tx.tx_amt1);
	vtcp_log("ǰ̨������txamt=[%.16s]",phv_payin->F_tx_amt);TRACE 
	/**ƾ֤���� **/
	get_fd_data("0890",phv_payin->F_note_type);
	vtcp_log("ǰ̨������notetype=[%.3s]",phv_payin->F_note_type);TRACE 
	/**ƾ֤���� **/
	get_fd_data("0580",phv_payin->F_note_no);
	vtcp_log("ǰ̨������note_no=[%.16s]",phv_payin->F_note_no);TRACE 
	/**����Ա��**/
	vtcp_log("ǰ̨������wt_date=[%.8s]",phv_payin->F_wt_date);TRACE 
	vtcp_log("ǰ̨������txamt=[%.16s]",phv_payin->F_tx_amt);TRACE 
	get_fd_data(DC_TEL,phv_payin->F_tel);
	vtcp_log("ǰ̨������tel=[%.8s]",phv_payin->F_tel);TRACE 
	/**����Ա��**/
	get_fd_data("0080",phv_payin->F_chk);
	vtcp_log("ǰ̨������chk=[%.8s]",phv_payin->F_chk);TRACE 
	/**��Ȩ����Ա��**/
	get_fd_data("0091",phv_payin->F_auth_tel);
	vtcp_log("ǰ̨������auth_tel=[%.8s]",phv_payin->F_auth_tel);TRACE 
	/**��Ȩ����**/
	get_fd_data("0092",phv_payin->F_auth_pwd);
	/*��������***/
	MEMCPY_DEBUG(phv_payin->F_opcd,fd123.opcd,sizeof(phv_payin->F_opcd));
	vtcp_log("ǰ̨������opcd=[%.2s]",phv_payin->F_opcd);TRACE 
	/* �������е������״���
	* MEMCPY_DEBUG(phv_payin->F_chrg_amt       ,,sizeof(phv_payin->F_chrg_amt      ));**�����ѽ�� **
	*/
	/* �������Ҫ����Ϊ��ר�ŵ�ԭ�ӽ��׼������
	* MEMCPY_DEBUG(phv_payin->F_userpswd   ,fd79,sizeof(phv_payin->F_userpswd  ));**��������       **
	* MEMCPY_DEBUG(phv_payin->F_auth_pwd   ,,sizeof(phv_payin->F_auth_pwd      ));**��Ȩ����Ա���� **
	*/	
	vtcp_log("[%s][%d]fd123.yw_type =[%.8s]",__FILE__,__LINE__,fd123.yw_type );
	MEMCPY_DEBUG(phv_payin->F_yw_type,fd123.yw_type,sizeof(phv_payin->F_yw_type));
	vtcp_log("[%s][%d]phv_payin->F_yw_type =[%.8s]",__FILE__,__LINE__,phv_payin->F_yw_type );
	vtcp_log("[%s,%d]ywtype===[%s]\n",__FILE__,__LINE__,phv_payin->F_yw_type);
	/*֧���������   **/
	MEMCPY_DEBUG(cOrderno ,fd123.orderno ,sizeof(cOrderno)-1);
	vtcp_log("ǰ̨������wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	zip_space(cOrderno);
	vtcp_log("ǰ̨������orderno=[%.8s]",cOrderno);TRACE  
	if(strlen(cOrderno)!=8 || memcmp(phv_payin->F_opcd,"10",sizeof(phv_payin->F_opcd))==0)
	{
		/**���������ķ��ʹ��ҵ���� 20130607 hzh**/
		char cTx_code[5];
		char cSub_code[5];
		memset(cTx_code, 0 , sizeof(cTx_code));
		memset(cSub_code , 0 , sizeof(cSub_code));
		memcpy(cTx_code, g_pub_tx.tx_code, sizeof(cTx_code)-1);
		memcpy(cSub_code,g_pub_tx.sub_tx_code , sizeof(cSub_code)-1);
		vtcp_log("[%s][%d]cTx_code=[%s][%s]\n",__FILE__,__LINE__,cTx_code,cSub_code);
		if(memcmp(cTx_code,"6389",sizeof(cTx_code)-1)==0 &&
		memcmp(cSub_code,"J009",sizeof(cSub_code)-1)==0)
		{
			memcpy(phv_payin->F_opcd,"40",sizeof(phv_payin->F_opcd));
			memcpy(fd123.opcd,"40",sizeof(fd123.opcd));
			g_pub_tx.trace_cnt++;
		}
		else
		{
		iRet=iGetHvOrderno(cOrderno);
		if(iRet)
		{
		sprintf( acErrMsg,"�õ�֧��������Ŵ���![%s][%d][%d]",__FILE__,__LINE__,iRet);
		set_zd_data(DC_GET_MSG,"�õ�֧��������Ŵ���!");
	    	strcpy(g_pub_tx.reply,"HV01");
		return iRet;
		}
		/**���������ķ��ʹ��ҵ���� 20130607 hzh**/
		MEMCPY_DEBUG(fd123.orderno,cOrderno,sizeof(fd123.orderno));
		set_zd_data("1230",(char*)&fd123);
		}
	}
	vtcp_log("ǰ̨������wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	/*������ʾ***/
	phv_payin->F_lw_ind[0]=fd123.lw_ind[0];
	vtcp_log("ǰ̨������������־=[%c]",phv_payin->F_lw_ind[0]);
	/* �����ѱ�־ */
	get_fd_data("0093",phv_payin->F_tx_chrg_ind);
	vtcp_log("ǰ̨������tx_chrg_ind=[%.1s]",phv_payin->F_tx_chrg_ind);TRACE 
	MEMCPY_DEBUG(phv_payin->F_orderno ,cOrderno ,sizeof(phv_payin->F_orderno ));
	/*cmt���**/
	MEMCPY_DEBUG(phv_payin->F_cmtno   ,fd123.cmtno   ,sizeof(phv_payin->F_cmtno   ));
	vtcp_log("cmtnoǰ̨�����ĸ�����=[%s][%s]",phv_payin->F_cmtno,fd123.cmtno);TRACE 

	/*�������к�     **/
	vtcp_log("[%s,%d]�����к� br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_tx_br_no);TRACE 
	if(phv_payin->F_lw_ind[0]=='1')
	{
		char cOpnbr[BRNO_LEN+1];
		MEMSET_DEBUG(cOpnbr, 0 , sizeof(cOpnbr));
		get_fd_data("0020",cOpnbr);
		g_reply_int=GetOr_br_noByBr_no(phv_payin->F_tx_br_no,phv_payin->F_or_br_no);
		if(g_reply_int)
		{
			strncpy(g_pub_tx.reply,"P007",4);
			vtcp_log("%s,%d ����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
			return g_reply_int;
		}
		
		g_reply_int=GetOr_br_noByBr_no(cOpnbr,phv_payin->F_pay_opn_br_no);
		if(g_reply_int)
		{
			strncpy(g_pub_tx.reply,"P007",4);
			vtcp_log("%s,%d ����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
			return g_reply_int;
		}  
	}
	else
	{
		MEMCPY_DEBUG(phv_payin->F_or_br_no,fd123.or_br_no,sizeof(phv_payin->F_or_br_no));
		/*�����˿�����**/
		MEMCPY_DEBUG(phv_payin->F_pay_opn_br_no ,fd123.pay_opn_br_no ,
			sizeof(phv_payin->F_pay_opn_br_no ));
	}
	vtcp_log("[%s,%d]֧��ϵͳ�����к� or_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_or_br_no);TRACE 
	vtcp_log("[%s,%d]�����˿����к� opn_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->F_pay_opn_br_no);TRACE 
	/*�������к�     **/
	MEMCPY_DEBUG(phv_payin->F_ac_br_no,fd123.ac_br_no,sizeof(phv_payin->F_ac_br_no));
	vtcp_log("ǰ̨������ac_br_no=[%.12s]",phv_payin->F_ac_br_no);TRACE 
	/*�������ʺ�*/
	MEMCPY_DEBUG(phv_payin->F_pay_ac_no ,fd123.pay_ac_no,
		sizeof(phv_payin->F_pay_ac_no));
	vtcp_log("ǰ̨������pay_ac_no=[%.12s]",phv_payin->F_pay_ac_no);TRACE 
	/*����������*/
	MEMCPY_DEBUG(phv_payin->F_pay_name  ,fd123.pay_name,
		sizeof(phv_payin->F_pay_name  ));
	/* ��黧������Чʹ�÷�Χ */
	vtcp_log("ǰ̨������pay_name=[%.60s]",phv_payin->F_pay_name);TRACE 
	/*�����˵�ַ*/
	MEMCPY_DEBUG(phv_payin->F_pay_addr  ,fd123.pay_addr,
		sizeof(phv_payin->F_pay_addr  ));
	/*�տ����ʺ�*/
	MEMCPY_DEBUG(phv_payin->F_cash_ac_no,fd123.cash_ac_no,
		sizeof(phv_payin->F_cash_ac_no));
	/*�տ�������*/
	MEMCPY_DEBUG(phv_payin->F_cash_name ,fd123.cash_name ,
		sizeof(phv_payin->F_cash_name ));
	/*�տ��˵�ַ*/
	MEMCPY_DEBUG(phv_payin->F_cash_addr,fd123.cash_addr,
		sizeof(phv_payin->F_cash_addr));
	vtcp_log("ǰ̨������cash_addr=[%.60s]",phv_payin->F_cash_addr);TRACE 
	/*����*/
	MEMCPY_DEBUG(phv_payin->F_brf,fd123.brf,sizeof(phv_payin->F_brf));
	/*���ȼ�*/
	MEMCPY_DEBUG(phv_payin->F_operlevel ,fd123.operlevel ,
		sizeof(phv_payin->F_operlevel ));
	vtcp_log("ǰ̨������wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 

	/*�տ��˿������к�**/
	MEMCPY_DEBUG(phv_payin->F_cash_opn_br_no,fd123.cash_opn_br_no	,
		sizeof(phv_payin->F_cash_opn_br_no));

	vtcp_log("ǰ̨������wtdate=[%.8s]",phv_payin->F_wt_date);TRACE 
	/* ԭ�������� */
	get_fd_data("0450",phv_payin->F_o_tx_date);
	vtcp_log("ǰ̨������o_tx_date=[%.8s]",phv_payin->F_o_tx_date);TRACE 
	/* ԭί������ */
	get_fd_data("0450",phv_payin->F_o_wt_date);
	vtcp_log("ǰ̨������o_wt_date=[%.8s]",phv_payin->F_o_wt_date);TRACE 
	/**ԭCMT���**/
	MEMCPY_DEBUG(phv_payin->F_o_cmtno,fd123.o_cmtno,
		sizeof(phv_payin->F_o_cmtno));
	/**����***/
	MEMCPY_DEBUG(phv_payin->F_hv_brf,fd123.brf,sizeof(phv_payin->F_hv_brf));
	vtcp_log("ǰ̨������hv_brf=[%.8s]",phv_payin->F_hv_brf);TRACE 
	/**ԭ֧���������**/
	MEMCPY_DEBUG(phv_payin->F_o_orderno,fd123.o_orderno,
		sizeof(phv_payin->F_o_orderno));
	vtcp_log("ǰ̨������o_orderno=[%.8s]",phv_payin->F_o_orderno);TRACE 
	/**�������**/
	MEMCPY_DEBUG(phv_payin->F_resp1,fd123.procode,sizeof(phv_payin->F_resp1));
	vtcp_log("cmtnoǰ̨�����ĸ�����=[%.3s][%.3s]",phv_payin->F_cmtno,fd123.cmtno);TRACE 
	iHvZip_hvpayinF(phv_payin);
	vtcp_log("%s%d",__FILE__,__LINE__);
	return 0;
}

/************************************************
 * ��������:��hv_payin��F��ͷ������д�뵽T��ͷ  *
 * ��������:iHvFrontToTable                     *
 * ��    ��:HV_PAYIN_AREA * phv_payin           *
 * ��������:hv_payin�ṹ                        *
 * ��    ��:LiuHuafeng 2006-8-21 16:21          *
 * ��    ��:                                    *
*************************************************/
int iHvFrontToTable(HV_PAYIN_AREA * phv_payin)
{
	char cFilename[1000];
	FILE * fpr;
	char reply[5];
	int iRet=0;
	char cBuff[2000];
	MEMSET_DEBUG(reply, 0 , sizeof(reply));
	MEMSET_DEBUG(cBuff, 0 , sizeof(cBuff));
	MEMSET_DEBUG(cFilename, 0  , sizeof(cFilename));
	Gl_file_first=0;
	MEMCPY_DEBUG(phv_payin->T_tx_br_no,phv_payin->F_tx_br_no,
		sizeof(phv_payin->T_tx_br_no));/* �������к�*/
	MEMCPY_DEBUG(phv_payin->T_tx_date ,phv_payin->F_tx_date ,
		sizeof(phv_payin->T_tx_date ));/* ��������*/
	MEMCPY_DEBUG(phv_payin->T_wt_date ,phv_payin->F_wt_date ,
		sizeof(phv_payin->T_wt_date ));/* ί������*/
	/* ����ʱ��*/
	get_fd_data(DC_TIME,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]�õ���ʱ��===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]�õ���ʱ��===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]�õ���ʱ��===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]�õ���ʱ��===%s",__FILE__,__LINE__,phv_payin->T_tx_time);
	vtcp_log("[%s][%d]F_tx_br_no===%s",__FILE__,__LINE__,phv_payin->F_tx_br_no);
	vtcp_log("[%s][%d]T_tx_br_no===%s",__FILE__,__LINE__,phv_payin->T_tx_br_no);
	MEMCPY_DEBUG(phv_payin->T_orderno  ,phv_payin->F_orderno  ,
		sizeof(phv_payin->T_orderno  ));/* ���еĽ������*/
	MEMCPY_DEBUG(phv_payin->T_cur_no   ,phv_payin->F_cur_no   ,
		sizeof(phv_payin->T_cur_no   ));/* ���ҷ���*/
	MEMCPY_DEBUG(phv_payin->T_note_type,phv_payin->F_note_type,
		sizeof(phv_payin->T_note_type));/* ƾ֤���� */
	MEMCPY_DEBUG(phv_payin->T_note_no  ,phv_payin->F_note_no  ,
		sizeof(phv_payin->T_note_no  ));/* ƾ֤�� */
	vtcp_log("ǰ̨������note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	MEMCPY_DEBUG(phv_payin->T_tx_amt   ,phv_payin->F_tx_amt   ,
		sizeof(phv_payin->T_tx_amt   ));/* ���׽�� */
	vtcp_log("ǰ̨������note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	MEMCPY_DEBUG(phv_payin->T_tx_chrg_ind,phv_payin->F_tx_chrg_ind,
		sizeof(phv_payin->T_tx_chrg_ind ));/*�����ѱ�־ 0.�ֽ� 1.�ֽ�Ӽ� 2.ת��3.ת�ʼӼ� 4.���� */
	/*note ��������ô����û�������꣬�ȷŷ�2006-8-7 14:28
	*MEMCPY_DEBUG(phv_payin->T_chrg_amt,phv_payin->F_chrg_amt,
	sizeof(phv_payin->T_chrg_amt         ));  ������ 
	*/

	vtcp_log("ǰ̨������note_no=[%.16s]",phv_payin->T_note_no);TRACE 
	g_reply_int=GetOr_br_noByBr_no(phv_payin->T_tx_br_no,phv_payin->T_or_br_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d ����֧��ϵͳ�кŴ���",__FILE__,__LINE__);
		return g_reply_int;
	}

	g_reply_int=iGet_full_hv_no(phv_payin->T_or_br_no,phv_payin->T_sendco,phv_payin->T_pay_qs_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d ����֧��ϵͳ�����кŴ���",__FILE__,__LINE__);
		return g_reply_int;
	}
	if(!memcmp(phv_payin->F_cmtno,"121",3))
	{
		MEMCPY_DEBUG(phv_payin->F_ac_br_no,HV_HPCENTER,sizeof(phv_payin->F_ac_br_no));
	}
	vtcp_log("���������к�=[%.12s]",phv_payin->T_pay_qs_no);TRACE 
	
	MEMCPY_DEBUG(phv_payin->T_ac_br_no ,phv_payin->F_ac_br_no ,
		sizeof(phv_payin->T_ac_br_no));/* �������к�--�˻㽻�׵�ԭ������ */
	g_reply_int=iGet_full_hv_no(phv_payin->T_ac_br_no,phv_payin->T_receco,phv_payin->T_cash_qs_no);
	if(g_reply_int)
	{
		strncpy(g_pub_tx.reply,"P007",4);
		vtcp_log("%s,%d ac_br_no=[%.12s]",__FILE__,__LINE__,phv_payin->T_ac_br_no);
		vtcp_log("%s,%d ����֧��ϵͳ�����кŴ���",__FILE__,__LINE__);
		return g_reply_int;
	}
	
	/*iFindQsbrno(phv_payin->T_ac_br_no,phv_payin->T_cash_qs_no); �����������к� 	
	 *iFindQsbrno(phv_payin->T_or_br_no,phv_payin->T_pay_qs_no);  �����������к� 
	 *iGetSendco(phv_payin->T_or_br_no,phv_payin->T_sendco);�������Ĵ��� 
	 *iGetSendco(phv_payin->T_ac_br_no,phv_payin->T_receco);�ձ����Ĵ��� 
	 */
  vtcp_log("[%s][%d]F_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->F_pay_opn_br_no);
	MEMCPY_DEBUG(phv_payin->T_pay_opn_br_no,phv_payin->F_pay_opn_br_no,
		sizeof(phv_payin->T_pay_opn_br_no));/* �����˿������к� */
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);	
	MEMCPY_DEBUG(phv_payin->T_pay_ac_no    ,phv_payin->F_pay_ac_no    ,
		sizeof(phv_payin->T_pay_ac_no    ));/* �������ʺ� */
	MEMCPY_DEBUG(phv_payin->T_pay_name , phv_payin->F_pay_name ,
		sizeof(phv_payin->T_pay_name));/* ����������-�˻㽻�׵�ԭ�տ��˻���*/
	MEMCPY_DEBUG(phv_payin->T_pay_addr ,phv_payin->F_pay_addr ,
		sizeof(phv_payin->T_pay_addr     ));/* �����˵�ַ */


	MEMCPY_DEBUG(phv_payin->T_cash_opn_br_no,phv_payin->F_cash_opn_br_no,
		sizeof(phv_payin->T_cash_opn_br_no));/* �տ��˿������к� */
	MEMCPY_DEBUG(phv_payin->T_cash_ac_no,phv_payin->F_cash_ac_no,
		sizeof(phv_payin->T_cash_ac_no    ));/* �տ����ʺ�       */
	MEMCPY_DEBUG(phv_payin->T_cash_name ,phv_payin->F_cash_name ,
		sizeof(phv_payin->T_cash_name     ));/* �տ�������-�˻㽻�׵�ԭ�����˻���*/
	MEMCPY_DEBUG(phv_payin->T_cash_addr ,phv_payin->F_cash_addr ,
		sizeof(phv_payin->T_cash_addr     ));/* �տ��˵�ַ   */
	vtcp_log("[%s][%d]phv_payin->F_yw_type =[%.8s]",__FILE__,__LINE__,phv_payin->F_yw_type );	
	MEMCPY_DEBUG(phv_payin->T_yw_type   ,phv_payin->F_yw_type   ,
		sizeof(phv_payin->T_yw_type       ));/* ҵ������     */
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,phv_payin->T_yw_type);	
	vtcp_log("[%s,%d]ywtype===[%s]\n",__FILE__,__LINE__,phv_payin->T_yw_type);

	MEMCPY_DEBUG(phv_payin->T_cmtno    ,phv_payin->F_cmtno    ,
		sizeof(phv_payin->T_cmtno    ));/* CMT���      */
	vtcp_log("[%s][%d]phv_payin->F_hv_brf======[%s]",__FILE__,__LINE__,phv_payin->F_hv_brf);	
	MEMCPY_DEBUG(phv_payin->T_hv_brf      ,phv_payin->F_hv_brf      ,
		sizeof(phv_payin->T_hv_brf      ));/* ����         */
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,phv_payin->T_hv_brf);
	MEMCPY_DEBUG(phv_payin->T_operlevel,phv_payin->F_operlevel,
		sizeof(phv_payin->T_operlevel));/* ���ȼ���0-һ��1-����2-�ؼ�[����ս�����֧��]*/

	MEMCPY_DEBUG(phv_payin->T_auth_tel ,phv_payin->F_auth_tel ,
		sizeof(phv_payin->T_auth_tel ));/* ��Ȩ����Ա*/
	/* note ��Ҫ���뵽hv_define.h�� */
	MEMCPY_DEBUG(phv_payin->T_o_orderno,phv_payin->F_o_orderno,
		sizeof(phv_payin->T_o_orderno));/* ԭ֧���������--�˻㽻���õ� */
	MEMCPY_DEBUG(phv_payin->T_o_tx_date,phv_payin->F_o_tx_date,
		sizeof(phv_payin->T_o_tx_date));/* ԭ֧����������--�˻㽻���õ� */
	MEMCPY_DEBUG(phv_payin->T_o_cmtno  ,phv_payin->F_o_cmtno  ,
		sizeof(phv_payin->T_o_cmtno  ));/* ԭcmtno--�˻㽻���õ� */
	MEMCPY_DEBUG(phv_payin->T_resp1    ,phv_payin->F_resp1    ,
		sizeof(phv_payin->T_resp1    ));/* Ӧ����*/
	MEMCPY_DEBUG(phv_payin->T_o_wt_date,phv_payin->F_o_wt_date,
		sizeof(phv_payin->T_o_wt_date));/*ԭί������*/			
	vtcp_log("%s,%d ԭί������==[%.8s]",__FILE__,__LINE__,phv_payin->T_o_wt_date);
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	MEMCPY_DEBUG(phv_payin->T_hv_brf   ,phv_payin->F_hv_brf   ,
		sizeof(phv_payin->T_hv_brf   ));/* ��ע */
	/*���� �Ը�������д��� */
	/* add by LiuHuafeng ����Ƿ��;Ͳ������ø����� */
	/***change by liuyue 20070122 ��Ʊ�ƴ� ��ʱ��᲻д���������Ѻ�ֶΣ���˽���������ŵ�spJ008�н��д���**/

	if(memcmp(phv_payin->F_opcd,"40",sizeof(phv_payin->F_opcd)))
	if (memcmp(phv_payin->T_cmtno , HV_HD , sizeof(phv_payin->T_cmtno))!=0 && 
		memcmp(phv_payin->T_cmtno , HV_GKZJHH , sizeof(phv_payin->T_cmtno))!=0  &&
		memcmp(phv_payin->T_cmtno , HV_HPYC , sizeof(phv_payin->T_cmtno))!=0 ) 
	{
		vtcp_log("�и�����cmtno=[%s]",phv_payin->T_cmtno);TRACE 
		/* ���ǻ�Һ͹����ʽ����� */
		Gl_file_first=0;/* add by LiuHuafeng 2006-8-26 14:32 */
		pub_base_AllDwnFilName( cFilename);
		vtcp_log("%s,%d filename=[%s]\n",__FILE__,__LINE__,cFilename);
		fpr=fopen(cFilename,"rb");
		if (fpr==NULL)
		{
			sprintf( acErrMsg,"�õ��������ļ�����![%s][%d]",__FILE__,__LINE__);
			set_zd_data(DC_GET_MSG,"�õ��������ļ�����");
			WRITEMSG
			return -1;
		}
		fgets(cBuff,sizeof(cBuff),fpr);
		vtcp_log("%s,%d file=[%s]\n",__FILE__,__LINE__,cBuff);
		fclose(fpr);
		iRet=iHvSetAddit(phv_payin->F_tx_date,phv_payin->F_wt_date,phv_payin->T_addid,"000",cBuff,strlen(cBuff));
		if (iRet)
		{
			sprintf( acErrMsg,"д�븽�������![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"д�븽�������");
			WRITEMSG
			return -1;
		}
		vtcp_log("%s,%d wt_date=%s,addid=[%s]",__FILE__,__LINE__,phv_payin->F_wt_date,phv_payin->T_addid);
	}
	MEMCPY_DEBUG(phv_payin->T_refno , phv_payin->F_refno ,
		sizeof(phv_payin->T_refno ));/* ���Ĳο��� */
	/* ��ʱû�а취����Ĵ���
	* MEMCPY_DEBUG(phv_payin->T_auth_trace_no , phv_payin->F_auth_trace_no ,
	sizeof(phv_payin->T_auth_trace_no    )); *��Ȩ����Ա��ˮ��*
	*/
	
	vtcp_log("%s,%d phv_payin->F_refno************=[%s]\n",__FILE__,__LINE__,phv_payin->F_refno);
	vtcp_log("%s,%d phv_payin->T_refno************=[%s]\n",__FILE__,__LINE__,phv_payin->T_refno);
	vtcp_log("%s,%d phv_payin->F_opcd*[%s]\n",__FILE__,__LINE__,phv_payin->F_opcd);
	phv_payin->T_lw_ind[0]=phv_payin->F_lw_ind[0];
	if (memcmp(phv_payin->F_opcd,"10",2) == 0)
	{/* 01¼�� */
		phv_payin->T_lw_ind[0]='1'; /* ������ʶ 1-�� 2-�� */	
		phv_payin->T_hv_sts[0]=SEND_REGIST;
		if(!memcmp(phv_payin->T_cmtno,"121",3)){
			phv_payin->T_hv_sts[0]=SEND_ACCT_CHECK;
		}
		iRet=iGetHvRefno(phv_payin->T_or_br_no,phv_payin->T_refno);/*modify by xyy 2009-7-17 14:42:02*/
		if (iRet)
		{
				vtcp_log("[%s,%d]�õ�֧��������Ŵ���=%d",__FILE__,__LINE__,iRet);
				sprintf( acErrMsg,"�õ�֧��������Ŵ���![%s][%d][%d]",__FILE__,__LINE__,iRet);
				set_zd_data(DC_GET_MSG,"�õ�֧��������Ŵ���");
				WRITEMSG
				return -1;
		}

		MEMCPY_DEBUG(phv_payin->F_refno,phv_payin->T_refno,sizeof(phv_payin->F_refno));
		vtcp_log("%s,%d phv_payin->F_refno=[%s]****hvsts=[%c]\n",__FILE__,__LINE__,phv_payin->F_refno,phv_payin->T_hv_sts[0]);
		vtcp_log("%s,%d phv_payin->T_refno=[%s]****hvsts=[%c]\n",__FILE__,__LINE__,phv_payin->T_refno,phv_payin->T_hv_sts[0]);
		/* ��ˮ��*/
		get_fd_data(DC_TRACE_NO,phv_payin->T_trace_no);
		get_fd_data(DC_TEL,phv_payin->T_tel);
		MEMCPY_DEBUG(g_pub_tx.ac_no,phv_payin->F_pay_ac_no,sizeof(g_pub_tx.ac_no)-1);
		g_pub_tx.ac_no[strlen(g_pub_tx.ac_no)]='\0';
		pub_base_old_acno(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.ac_no);
		iMbfeGetAcid(g_pub_tx.ac_no);
	}
	if (memcmp(phv_payin->F_opcd,"30",2) == 0)
	{/* 02���� */
		iRet=iHvCheckRecordStat(phv_payin->F_cmtno,phv_payin->T_hv_sts[0],SEND_ACCT_CHECK,phv_payin->T_lw_ind[0],acErrMsg);
		if(iRet)
		{
			sprintf( acErrMsg,"��¼״̬����![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"��¼״̬����");
			WRITEMSG
			return -1;
		}
		get_fd_data(DC_TRACE_NO,phv_payin->T_chk_trace_no);/* ���˲���Ա��ˮ��*/
		get_fd_data(DC_TEL,phv_payin->T_chk);
		phv_payin->T_hv_sts[0]=SEND_ACCT_CHECK;
	}
	if(memcmp(phv_payin->F_opcd,"40",2) == 0)
	{/* 03���� */
		iRet=iHvCheckRecordStat(phv_payin->F_cmtno,phv_payin->T_hv_sts[0],SEND_ACCT_WAIT,phv_payin->T_lw_ind[0],acErrMsg);
		if(iRet)
		{
			sprintf( acErrMsg,"��¼״̬����![%s][%d][%d]",__FILE__,__LINE__,iRet);
			set_zd_data(DC_GET_MSG,"��¼״̬����");
			WRITEMSG
			return -1;
		}
		/* ���Ͳ���Ա��ˮ��*/
		get_fd_data(DC_TRACE_NO,phv_payin->T_send_tel_trace_no);
		get_fd_data(DC_TEL,phv_payin->T_send_tel);
		/***add 20061009 liuyue** 20061116��һ��  liuyue   ֮ǰû�м�return  try*****/
		 if ( phv_payin->T_hv_sts[0] == SEND_ACCT_WAIT || phv_payin->T_hv_sts[0]!=SEND_ACCT_CHECK)
    		{
        		sprintf(acErrMsg,"��¼״̬����!\n");
        		strcpy( g_pub_tx.reply, "P016");
        		vtcp_log("[%s][%d]T_hv_sts=[%c][%c]\n",__FILE__,__LINE__,phv_payin->T_hv_sts[0] , SEND_ACCT_CHECK);
        		set_zd_data(DC_GET_MSG,"��¼״̬����");
        		WRITEMSG
				return (-1);
        		
    		}

		phv_payin->T_hv_sts[0]=SEND_ACCT_WAIT;
	}
	if(memcmp(phv_payin->F_opcd,"60",2)==0)
	{/* 04���� */
		phv_payin->T_beg_sts[0]=phv_payin->T_hv_sts[0];
		phv_payin->T_lw_ind[0]='2'; /* ������ʶ 1-�� 2-�� */
		get_fd_data("0120",reply);
		if (!memcmp(reply,"0000",4))
		{
			phv_payin->T_hv_sts[0]=RECEIVE_ACCT_QS;
		}
		else
		{
			/* ����ʧ���ˣ�Ҫ���жϽ��ջ����Ƿ���ڣ����ڹ�֧�У������ڹ����� */
			phv_payin->T_hv_sts[0]=RECEIVE_CUSTNAME_ERR;
		}
	}
	if (memcmp(phv_payin->F_opcd,"50",2)==0)
	{/* 05Ӧ�� */
		phv_payin->T_lw_ind[0]='2'; /* ������ʶ 1-�� 2-�� */	
		phv_payin->T_hv_sts[0]=SEND_ACCT_QS;
	}
	if(memcmp(phv_payin->F_opcd,"11",2)==0)
	{/* 06��ѯ */
	}
	if(memcmp(phv_payin->F_opcd,"71",2)==0)
	{/* 07�����޸� */
		MEMCPY_DEBUG(phv_payin->T_dealerr_date , phv_payin->F_tx_date ,
			sizeof(phv_payin->T_dealerr_date));/* ���ʴ�������*/
		MEMCPY_DEBUG(phv_payin->T_dealerr_tel , phv_payin->F_tel ,
			sizeof(phv_payin->T_dealerr_tel ));/* ���ʴ����Ա*/
		phv_payin->T_lw_ind[0]='2'; /* ������ʶ 1-�� 2-�� */	
		MEMCPY_DEBUG(g_pub_tx.ac_no,phv_payin->F_pay_ac_no,sizeof(g_pub_tx.ac_no)-1);
		g_pub_tx.ac_no[strlen(g_pub_tx.ac_no)]='\0';
		pub_base_old_acno(g_pub_tx.ac_no);
		pub_base_strpack(g_pub_tx.ac_no);
		iMbfeGetAcid(g_pub_tx.ac_no);
	}
	if (memcmp(phv_payin->F_opcd,"80",2)==0)
	{/* 10 �ܾ��˻�֧�� */
		MEMCPY_DEBUG(phv_payin->T_resend_date , phv_payin->T_tx_date ,
			sizeof(phv_payin->T_resend_date)); /* �ܾ����·������� */
		phv_payin->T_lw_ind[0]='1'; /* ������ʶ 1-�� 2-�� */	
	}
	vtcp_log("[%s][%d]T_pay_opn_br_no===[%s]",__FILE__,__LINE__,phv_payin->T_pay_opn_br_no);
	return 0;
}
/************************
*       PART 1       ***
*   ���ʱ��Ĵ���ӿ� ***
************************/

int pub_com_CMT100_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack100)
{    
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack100->opeType='0';/*���*/
	pack100->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack100->CMTCode,"100",3);					/*CMT���*/
	MEMCPY_DEBUG(pack100->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_payin->T_refno======[%s]",__FILE__,__LINE__,hv_payin->T_refno);
	
	vtcp_log("[%s][%d]pack100->reNo======[%s]",__FILE__,__LINE__,pack100->reNo);
	MEMCPY_DEBUG(pack100->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVSETTAG(pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack100,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack100,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack100,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack100,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack100,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack100,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack100,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVSETTAG(pack100,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack100,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack100,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));/*�տ��˿����к�*/
	iHVSETTAG(pack100,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack100,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack100,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*�տ��˵�ַ*/
	iHVSETTAG(pack100,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVSETTAG(pack100,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack100,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack100,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,hv_payin->T_hv_brf);
	/**
	iHVSETTAG(pack100,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			����*/
	vtcp_log("[%s][%d]hv_payin->T_brf======[%s]",__FILE__,__LINE__,hv_payin->T_hv_brf);
	iHVSETTAG(pack100,"72A",hv_payin->T_hv_brf,60);			/*����*/
	
	/******���ҵ��û�и�����***************/	
	return 0; 
}
int pub_com_CMT101_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack101)
{  
	/***************ί���տ���أ�֧������***************************/  
	char cTmpBuf[19];	
	char cAddid[9];
	HV_WTSK_ADD  hv_wtsk;/***������****/

	MEMSET_DEBUG(&hv_wtsk,0,sizeof(hv_wtsk));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));

	pack101->opeType='0';/*���*/					
	pack101->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack101->CMTCode,"101",3);					/*CMT���*/
	MEMCPY_DEBUG(pack101->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack101->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack101,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/	
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack101,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack101,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack101,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack101,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack101,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack101,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack101,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack101,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack101,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));/*�տ��˿����к�*/
	iHVSETTAG(pack101,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack101,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack101,"CEJ",hv_payin->T_note_type,sizeof(hv_payin->T_note_type));	/*ƾ֤����*/
	/** rem by LiuHuafeng 20070130
	iHVSETTAG(pack101,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		*ҵ������*
	***************************************************************************/
	iHVSETTAG(pack101,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack101,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack101,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack101,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_wtsk,sizeof(hv_wtsk));
		iHVSETTAG(pack101,"30B",hv_wtsk.pj_date,sizeof(hv_wtsk.pj_date));	/*Ʊ������*/
		iHVSETTAG(pack101,"21A",hv_wtsk.pj_num,sizeof(hv_wtsk.pj_num));		/*Ʊ�ݺ���*/
		iHVSETTAG(pack101,"CEJ",hv_wtsk.note_type,sizeof(hv_wtsk.note_type));   /**ƾ֤���� add by zyl 20110715 **/
	}
	return 0; 
}
int pub_com_CMT102_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack102)
{   
	/*****************���ճи������أ�֧������*************************/ 
	char cTmpBuf[19];	
	char cAddid[9];

	HV_TSCF_ADD hv_tscf;  /****��������***********/
	MEMSET_DEBUG(&hv_tscf,0,sizeof(hv_tscf));

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));				

	pack102->opeType='0';/*���*/
	pack102->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack102->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack102->CMTCode,"102",3);					/*CMT���*/
	MEMCPY_DEBUG(pack102->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack102,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);		 		/*��������*/ 
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack102,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack102,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack102,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack102,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack102,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack102,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack102,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack102,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVSETTAG(pack102,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack102,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack102,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack102,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack102,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack102,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*����*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_tscf,sizeof(hv_tscf));
		iHVSETTAG(pack102,"30B",hv_tscf.pj_date,sizeof(hv_tscf.pj_date));		/*Ʊ������*/
		iHVSETTAG(pack102,"21A",hv_tscf.pj_num,sizeof(hv_tscf.pj_num));			/*Ʊ�ݺ���*/
		del_dot(hv_tscf.ptxamt,hv_tscf.ptxamt,15);
		iHVSETTAG(pack102,"33A",hv_tscf.ptxamt,sizeof(hv_tscf.ptxamt));		/*�⳥���*/
		del_dot(hv_tscf.jtxamt,hv_tscf.jtxamt,15);
		iHVSETTAG(pack102,"33B",hv_tscf.jtxamt,sizeof(hv_tscf.jtxamt));		/*�ܸ����*/
	}
	vtcp_log("body==$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$[%s]",pack102->body);
	return 0; 
}
int pub_com_CMT103_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack103)
{   
	/****************�����ʽ�㻮�����ǣ�֧������**********************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack103->opeType='0';/*���*/
	pack103->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack103->CMTCode,"103",3);					/*CMT���*/
	MEMCPY_DEBUG(pack103->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack103->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack103,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack103,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack103,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack103,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack103,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack103,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack103,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack103,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack103,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack103,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVSETTAG(pack103,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack103,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin->T_yw_type);
	iHVSETTAG(pack103,"0BF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	vtcp_log("[%s][%d]hv_payin->T_yw_type=[%.8s]",__FILE__,__LINE__,hv_payin->T_yw_type);
	iHVSETTAG(pack103,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack103,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack103,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack103,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/
	vtcp_log("body==$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$[%s]",pack103->body);
	/*****û�и�����********/	
	return 0; 
}
int pub_com_CMT104_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack104)
{   
	/***************���ڴ���֧������***********************************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	pack104->opeType='0';/*���*/
	pack104->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack104->CMTCode,"104",3);					/*CMT���*/
	MEMCPY_DEBUG(pack104->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack104->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack104,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack104,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack104,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack104,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack104,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack104,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack104,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack104,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack104,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack104,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVSETTAG(pack104,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack104,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack104,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*�տ��˵�ַ*/
	iHVSETTAG(pack104,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVSETTAG(pack104,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack104,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack104,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack104,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/**********û�и�����******************************/	
	return 0; 
}
int pub_com_CMT105_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack105)
{    
	/************���м�ͬҵ���֧������********************************/
	char cTmpBuf[19];	
	char cAddid[9];
	HV_TYCJ_ADD hv_tycj;

	MEMSET_DEBUG(&hv_tycj,0,sizeof(hv_tycj));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));

	pack105->opeType='0';/*���*/
	pack105->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack105->CMTCode,"105",3);					/*CMT���*/
	MEMCPY_DEBUG(pack105->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack105->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack105,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack105,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack105,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack105,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack105,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack105,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack105,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVSETTAG(pack105,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack105,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack105,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack105,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	MEMCPY_DEBUG(cAddid,hv_payin->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,cAddid,"000",&hv_tycj,sizeof(hv_tycj));
		iHVSETTAG(pack105,"CF3",hv_tycj.rate,sizeof(hv_tycj.rate));				/*�������*/
		iHVSETTAG(pack105,"CF4",hv_tycj.ratelimit,sizeof(hv_tycj.ratelimit));		/*�������*/
	}
	/*******************************************************************/
	return 0; 
}
int pub_com_CMT108_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack108)
{   
	/**************�˻�֧������***************************************/ 
	char cTmpBuf[19];	

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack108->opeType='0';/*���*/
	pack108->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/	
	MEMCPY_DEBUG(pack108->CMTCode,"108",3);					/*CMT���*/		
	MEMCPY_DEBUG(pack108->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack108->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack108,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack108,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack108,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack108,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack108,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack108,"32A",cTmpBuf,sizeof(cTmpBuf)-1);					/*����+���*/
	iHVSETTAG(pack108,"051",hv_payin->T_o_wt_date,sizeof(hv_payin->T_o_wt_date));	/*ԭί������*/
	iHVSETTAG(pack108,"005",hv_payin->T_o_orderno,sizeof(hv_payin->T_o_orderno));	/*ԭ�������*/
	iHVSETTAG(pack108,"02B",hv_payin->T_o_cmtno,sizeof(hv_payin->T_o_cmtno));		/*ԭcmt���*/
	iHVSETTAG(pack108,"CQ1",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*ԭ�������ʺ�*/
	iHVSETTAG(pack108,"CR1",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*ԭ����������*/
	iHVSETTAG(pack108,"CQ2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*ԭ�տ����ʺ�*/
	iHVSETTAG(pack108,"CR2",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*ԭ�տ�������*/
	iHVSETTAG(pack108,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack108,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack108,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack108,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/********û�и�����*********/	
	return 0; 
}
int pub_com_CMT109_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack109)
{   
	/**********��������ר�û�ұ���************************************/ 
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack109->opeType='0';/*���*/
	pack109->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack109->CMTCode,"109",3);					/*CMT���*/
	MEMCPY_DEBUG(pack109->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack109->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack109,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack109,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	iHVSETTAG(pack109,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack109,"CG1",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*�����к�*/
	iHVSETTAG(pack109,"CG2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack109,"CG3",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack109,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack109,"CG4",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack109,"CG5",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack109,"CG6",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack109,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack109,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack109,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack109,"CG7",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	return 0; 
}
int pub_com_CMT119_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack119)
{    
	/***************ͨ�ý��֧������**********************************/
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	pack119->opeType='0';/*���*/
	pack119->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack119->CMTCode,"119",3);					/*CMT���*/
	MEMCPY_DEBUG(pack119->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack119->workDate,hv_payin->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack119,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack119,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	iHVSETTAG(pack119,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack119,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack119,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVSETTAG(pack119,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVSETTAG(pack119,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack119,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*�տ��˵�ַ*/
	iHVSETTAG(pack119,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack119,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack119,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack119,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack119,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack119,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVSETTAG(pack119,"20A",hv_payin->T_note_no,sizeof(hv_payin->T_note_no));		/*ƾ֤����*/
	iHVSETTAG(pack119,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVSETTAG(pack119,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack119,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack119,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack119,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/********û�и�����***************/	
	return 0; 
}
int pub_com_CMT121_T2TAG(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack121)
{   
	/***************���л�Ʊ�ʽ��ƴ汨��******************************/ 
	char cTmpBuf[19];	
	HV_121_ADD hv_121;/****������*****/

	MEMSET_DEBUG(&hv_121,0,sizeof(hv_121));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));
	pack121->opeType='0';/*���*/
	pack121->PRI=hv_payin->T_operlevel[0];				/*���ȼ���*/
	MEMCPY_DEBUG(pack121->CMTCode,"121",3);					/*CMT���*/
	MEMCPY_DEBUG(pack121->workDate,hv_payin->T_tx_date,8);	/*��������*/

	vtcp_log("[%s][%d]��ʼ��ȡ������\n",__FILE__,__LINE__);
	vtcp_log("��������:tx_date=[%s],wt_date=[%s],addid=[%s]\n",
		hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid);
	iHvGetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000" ,&hv_121,sizeof(hv_121));
	vtcp_log("%s,%d ��Ʊ������==[%s] ",__FILE__,__LINE__,(char*)&hv_121);

	MEMCPY_DEBUG(pack121->reNo,hv_payin->T_refno,sizeof(hv_payin->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack121,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	MEMCPY_DEBUG(cTmpBuf,hv_payin->T_cur_no,3);				/*��������*/
	del_dot(hv_payin->T_tx_amt,cTmpBuf+3,15);/*���׽��*/
	iHVSETTAG(pack121,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	iHVSETTAG(pack121,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack121,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVSETTAG(pack121,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack121,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack121,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVSETTAG(pack121,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVSETTAG(pack121,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVSETTAG(pack121,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVSETTAG(pack121,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVSETTAG(pack121,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	iHVSETTAG(pack121,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack121,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVSETTAG(pack121,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*����*/

	/****��������******/	
	{	
	char cSpec_br_no[13];
	MEMSET_DEBUG(cSpec_br_no , 0 , sizeof(cSpec_br_no));
	MEMCPY_DEBUG(cSpec_br_no,hv_121.spec_br_no,sizeof(cSpec_br_no));
	zip_space(cSpec_br_no);
	iHVSETTAG(pack121,"C33",cSpec_br_no,sizeof(cSpec_br_no)-1);	/*�ֽ��Ʊ�Ҹ���*/
	}
	iHVSETTAG(pack121,"30B",hv_121.pj_date,sizeof(hv_121.pj_date));	/*Ʊ������*/
	vtcp_log("%s,%d ��Ʊ������==[%s] ",__FILE__,__LINE__,(char*)&hv_121);
	iHVSETTAG(pack121,"21A",hv_121.pj_num,sizeof(hv_121.pj_num));		/*Ʊ�ݺ���*/
	iHVSETTAG(pack121,"D02",hv_121.hp_type,sizeof(hv_121.hp_type));	/*Ʊ������*/
	iHVSETTAG(pack121,"C10",hv_121.hp_miya,sizeof(hv_121.hp_miya));		/*��Ʊ��Ѻ*/


	return 0; 
}
int pub_com_CMT301_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack301)
{    
	/****************��ѯ����*********************************/
	char cTmpBuf[19];

	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	pack301->opeType='2';/*����*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);	
	vtcp_log("[%s][%d]cTmpBuf==[%s]\n",__FILE__,__LINE__,cTmpBuf);				/*��������*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***���׽��**/
	vtcp_log("[%s][%d]cTmpBuf==[%s]\n",__FILE__,__LINE__,cTmpBuf);
	MEMCPY_DEBUG(pack301->CMTCode,"301",3);
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);						/*CMT���*/
	MEMCPY_DEBUG(pack301->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	MEMCPY_DEBUG(pack301->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG( pack301,"CJ9",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*��ѯ����*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"01C",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*��ѯ���к�*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*����������*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"056",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*��ѯ���*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�鸴���к�*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*ԭ�������к�*/
	iHVSETTAG(pack301,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*ԭ�����к�*/
	iHVSETTAG(pack301,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVSETTAG(pack301,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	iHVSETTAG(pack301,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVSETTAG(pack301,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVSETTAG(pack301,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*��ѯ����*/

	vtcp_log("[%s][%d]\n",__FILE__,__LINE__);
	/***********û�и�����***************/	
	return 0; 
}
int pub_com_CMT302_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack302)
{    
	/****************�鸴����*********************************/
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0x00,sizeof(cTmpBuf));

	pack302->opeType='2';/*����*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);					/*��������*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/***���׽��**/
	MEMCPY_DEBUG(pack302->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack302->CMTCode,"302",3);						/*CMT���*/
	MEMCPY_DEBUG(pack302->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	iHVSETTAG( pack302,"CJ1",hv_notpay->T_res_date,sizeof(hv_notpay->T_res_date));		/*�鸴����*/
	iHVSETTAG(pack302,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�鸴���к�*/
	iHVSETTAG(pack302,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*�����������к�*/
	iHVSETTAG(pack302,"043",hv_notpay->T_res_orderno,sizeof(hv_notpay->T_res_orderno));			/*�鸴���*/
	iHVSETTAG(pack302,"CCB",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*ԭ��ѯ���к�*/
	iHVSETTAG(pack302,"CCB",hv_notpay->T_req_br_no,sizeof(hv_notpay->T_req_br_no));		/*ԭ��ѯ���к�*/
	iHVSETTAG(pack302,"CCC",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*ԭ��ѯ�����к�*/
	iHVSETTAG(pack302,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*ԭ��ѯ���*/
	iHVSETTAG(pack302,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*ԭ��ѯ����*/
	iHVSETTAG(pack302,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVSETTAG(pack302,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*ԭ�������к�*/
	iHVSETTAG(pack302,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*ԭ�����к�*/
	iHVSETTAG(pack302,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVSETTAG(pack302,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	iHVSETTAG(pack302,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	iHVSETTAG(pack302,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�������Ĵ���*/
	iHVSETTAG(pack302,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�ձ����Ĵ���*/
	iHVSETTAG(pack302,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*�鸴����*/

    vtcp_log("[%s][%d] hv_notpay->T_req_br_no=[%.12s] ",__FILE__,__LINE__,hv_notpay->T_req_br_no);  
    vtcp_log("[%s][%d] hv_notpay->T_req_date=[%.8s] ",__FILE__,__LINE__,hv_notpay->T_req_date);  
    vtcp_log("[%s][%d] hv_notpay->T_req_orderno=[%.8s] ",__FILE__,__LINE__,hv_notpay->T_req_orderno);  
 
 
 
	/***********û�и�����***************/	
	return 0; 
}
int pub_com_CMT303_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack303)
{
	/*************���ɸ�ʽ����********************/    

	pack303->opeType='2';/*����*/
	MEMCPY_DEBUG(pack303->CMTCode,"303",3);					/*CMT���*/
	MEMCPY_DEBUG(pack303->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack303->workDate,hv_notpay->T_tx_date,8);	/*��������*/
	MEMCPY_DEBUG(pack303->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack303,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*ί������*/
	iHVSETTAG(pack303,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));	/*���������*/
	iHVSETTAG(pack303,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));	/*����������*/
	iHVSETTAG(pack303,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));	/*�����к�*/
	iHVSETTAG(pack303,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));	/*���������к�*/
	iHVSETTAG(pack303,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));	/*����*/
	iHVSETTAG(pack303,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));		/*�������Ĵ���*/
	iHVSETTAG(pack303,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));		/*�ձ����Ĵ���*/

	/****û�и�����***********/	
	return 0; 
}
int pub_com_CMT311_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack311)
{   
	/******���볷������***********************************************/ 
	char	cSub_content[61];
	char	cSub_orderno[4];

	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/***������������ ���Դ� reqȡ**/
	vtcp_log("[%s][%d]T_orderno=[%s]",__FILE__,__LINE__,hv_notpay->T_orderno);
	vtcp_log("[%s][%d]T_orderno=[%.3s]",__FILE__,__LINE__,hv_notpay->T_orderno);
	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_orderno+5,sizeof(cSub_orderno)-1);
	zip_space(cSub_orderno);
	vtcp_log("[%s][%d]cSub_orderno=[%s]",__FILE__,__LINE__,cSub_orderno);
	pack311->opeType='2';/*���*/
	MEMCPY_DEBUG(pack311->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack311->CMTCode,"311",3);					/*CMT���*/
	MEMCPY_DEBUG(pack311->workDate,hv_notpay->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack311,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*ί������*/
	iHVSETTAG(pack311,"C47",cSub_orderno,sizeof(cSub_orderno)-1);				/*�����*/
	iHVSETTAG(pack311,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));	/*ԭί������*/
	iHVSETTAG(pack311,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));	/*ԭ֧�����׷�����*/
	iHVSETTAG(pack311,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));	/*ԭ��������*/
	iHVSETTAG(pack311,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));/*ԭ֧���������*/
	iHVSETTAG(pack311,"72A",cSub_content,sizeof(cSub_content));					/*����*/
	/***û�и�����*****/	
	return 0; 
}
int pub_com_CMT312_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack312)
{    
	/****************����Ӧ����*************/
	char	cSub_content[61];
	char	cSub_orderno[4];
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_res_content,60);/**Ӧ���ཻ�״�res��ȡ***/
	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_req_orderno,3); /*��ȡ3λORDERNO��Ϊ���,����Ӧ����Ľ���,��Ҫ�ҵ�ԭ���յ��������ཻ��, �ҵ���,��ȡ�ü�¼�еķ�֧��������� ���ṹ����ֶ�T_req_orderno��***/
	pack312->opeType='2';/*���*/

	MEMCPY_DEBUG(pack312->CMTCode,"312",3);						/*CMT���*/
	MEMCPY_DEBUG(pack312->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(pack312->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	iHVSETTAG( pack312,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVSETTAG(pack312,"CCD",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*���볷���к�*/
	iHVSETTAG(pack312,"CP2",cSub_orderno,sizeof(cSub_orderno));					/*ԭ���������*/
	iHVSETTAG(pack312,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*����֧���������*/
	iHVSETTAG(pack312,"04C",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*����Ӧ��*/
	iHVSETTAG(pack312,"72A",cSub_content,sizeof(cSub_content));					/*����*/
	/****û�и�����*****/	
	return 0; 
}
int pub_com_CMT313_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack313)
{    
	/****************�����˻ر���*********************************/
	char cTmpBuf[19];
	char cSub_content[61];/***��ȡ60λ�ֶ�,60λ��255λ���ñ�����***/
	char cSub_orderno[4];/***��ȡ���������Ϊ�ʺ�*****/
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_orderno+5,3);/**��ȡorderno�ĺ�3λ***/
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/***��ȡ60λ��Ϊ����**/

	pack313->opeType='2';/*���*/
	MEMCPY_DEBUG(pack313->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);				/*��������*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/**���׽��**/
	MEMCPY_DEBUG(pack313->CMTCode,"313",3);					/* CMT���*/
	MEMCPY_DEBUG(pack313->workDate,hv_notpay->T_tx_date,8);	/*��������*/
	iHVSETTAG( pack313,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVSETTAG(pack313,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*�������к�*/
	iHVSETTAG(pack313,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*����������*/
	iHVSETTAG(pack313,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVSETTAG(pack313,"C47",cSub_orderno,sizeof(cSub_orderno));					/*�˻������*/
	iHVSETTAG(pack313,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�������к�*/
	iHVSETTAG(pack313,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	iHVSETTAG(pack313,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVSETTAG(pack313,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVSETTAG(pack313,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVSETTAG(pack313,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	iHVSETTAG(pack313,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	iHVSETTAG(pack313,"72A",cSub_content,sizeof(cSub_content));					/*����*/

	/***********û�и�����***************/	
	return 0; 
}
int pub_com_CMT314_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack314)
{    
	/****************�˻�Ӧ����*********************************/
	char cTmpBuf[19];
	char cSub_content[61];/***��ȡ60λ�ֶ�,60λ��255λ���ñ�����***/
	char cSub_orderno[4];/***��ȡ���������Ϊ�ʺ�*****/

	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));
	MEMSET_DEBUG(cSub_orderno,0,sizeof(cSub_orderno));

	MEMCPY_DEBUG(cSub_orderno,hv_notpay->T_req_orderno+5,3);/****��ȡorderno��3λ,Ӧ���ཻ����Ҫ�����Ӧ����������н�ȡ�ֶ�,˵�����ձ���312**/
	MEMCPY_DEBUG(cSub_content,hv_notpay->T_res_content,60);/***��ȡ60λ��Ϊ����**/

	pack314->opeType='2';/*���*/
	MEMCPY_DEBUG(cTmpBuf,hv_notpay->T_o_cur_no,3);					/*��������*/
	del_dot(hv_notpay->T_o_tx_amt,cTmpBuf+3,15);/**���׽��**/
	MEMCPY_DEBUG(pack314->CMTCode,"314",3);						/*CMT���*/
	MEMCPY_DEBUG(pack314->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	MEMCPY_DEBUG(pack314->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	iHVSETTAG( pack314,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVSETTAG(pack314,"CCE",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_or_br_no));		/*�˻����к�*/
	iHVSETTAG(pack314,"011",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*���������� ���е�д����cash��*/
	iHVSETTAG(pack314,"010",hv_notpay->T_receco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVSETTAG(pack314,"58A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�������к�*/
	iHVSETTAG(pack314,"012",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	iHVSETTAG(pack314,"0B9",hv_notpay->T_sendco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVSETTAG(pack314,"CJB",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*ԭ�˻���������*/
	iHVSETTAG(pack314,"CP2",cSub_orderno,sizeof(cSub_orderno));					/*ԭ�˻��������*/
	iHVSETTAG(pack314,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVSETTAG(pack314,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVSETTAG(pack314,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	iHVSETTAG(pack314,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	iHVSETTAG(pack314,"018",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*�˻�Ӧ��*/
	iHVSETTAG(pack314,"72A",cSub_content,sizeof(cSub_content));					/*����*/

	/***********û�и�����***************/	
	return 0; 
}
int pub_com_CMT721_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack721)
{   
	/********����Ҹ�������ҵ�������л�Ʊ����ҵ����ר��**************/ 
	char cAddid[9];
	char cSub_content[61];
	HV_721_ADD hv_721;

	MEMSET_DEBUG(&hv_721,0,sizeof(hv_721));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/**��255�������н�ȡ60�� �ֶ�,��Ϊ¼���60λ�ĳ��ȷŵ���T_req_content�ֶ���**/

	pack721->opeType='2';/*���*/
	MEMCPY_DEBUG(pack721->CMTCode,"721",3);						/*CMT���*/
	MEMCPY_DEBUG(pack721->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	MEMCPY_DEBUG(pack721->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	iHVSETTAG( pack721,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	iHVSETTAG(pack721,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*���������*/
	iHVSETTAG(pack721,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack721,"72A",cSub_content,sizeof(cSub_content));					/*����*/
	vtcp_log("body**********==[%s]",pack721->body);
	MEMCPY_DEBUG(cAddid,hv_notpay->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		iHvGetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,cAddid,"000",&hv_721,sizeof(hv_721));
		iHVSETTAG(pack721,"30B",hv_721.pj_date,sizeof(hv_721.pj_date));		/*Ʊ������*/
		iHVSETTAG(pack721,"21A",hv_721.pj_num,sizeof(hv_721.pj_num));		/*Ʊ�ݺ���*/
		iHVSETTAG(pack721,"D02",hv_721.hp_type,sizeof(hv_721.hp_type));		/*��Ʊ���*/
		iHVSETTAG(pack721,"C10",hv_721.hp_miya,sizeof(hv_721.hp_miya));		/*��Ʊ��Ѻ*/
		del_dot(hv_721.cp_amt,hv_721.cp_amt,15);
		iHVSETTAG(pack721,"33C",hv_721.cp_amt,sizeof(hv_721.cp_amt));		/*��Ʊ���*/
		vtcp_log("%s,%d real_js_amt=[%.15s],change_real_js_amt=[%.15s]",__FILE__,
			__LINE__,hv_721.real_js_amt,hv_721.real_js_amt);
		del_dot(hv_721.real_js_amt,hv_721.real_js_amt,15);
		vtcp_log("%s,%d real_js_amt=[%.15s],change_real_js_amt=[%.15s]",__FILE__,
			__LINE__,hv_721.real_js_amt,hv_721.real_js_amt);
		iHVSETTAG(pack721,"CNV",hv_721.real_js_amt,sizeof(hv_721.real_js_amt));	/*ʵ�ʽ�����*/
		del_dot(hv_721.dy_amt,hv_721.dy_amt,15);
		iHVSETTAG(pack721,"33D",hv_721.dy_amt,sizeof(hv_721.dy_amt));		/*������*/
		iHVSETTAG(pack721,"59E",hv_721.last_ac_no,sizeof(hv_721.last_ac_no));	/*����Ʊ���˺�*/
		iHVSETTAG(pack721,"59D",hv_721.last_name,sizeof(hv_721.last_name));		/*����Ʊ������*/
		iHVSETTAG(pack721,"30C",hv_721.return_date,sizeof(hv_721.return_date));	/*��ʾ��������*/
		iHVSETTAG(pack721,"CC4",hv_721.pay_opn_br_no,sizeof(hv_721.pay_opn_br_no));	/*�����˿����к�*/
		iHVSETTAG(pack721,"CC5",hv_721.cash_opn_br_no,sizeof(hv_721.cash_opn_br_no));/*�տ��˿����к�*/

	}
	return 0; 
}
int pub_com_CMT724_T2TAG(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack724)
{   
	/********����Ҹ�������ҵ�������л�Ʊ����ҵ����ר��**************/ 
	char cAddid[9];
	char cSub_content[61];
	HV_724_ADD hv_724;

	MEMSET_DEBUG(&hv_724,0,sizeof(hv_724));
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMSET_DEBUG(cSub_content,0,sizeof(cSub_content));

	MEMCPY_DEBUG(cSub_content,hv_notpay->T_req_content,60);/**��255�������н�ȡ60�� �ֶ�,��Ϊ¼���60λ�ĳ��ȷŵ���T_req_content�ֶ���**/

	pack724->opeType='2';/*���*/
	MEMCPY_DEBUG(pack724->CMTCode,"724",3);						/*CMT���*/
	MEMCPY_DEBUG(pack724->reNo,hv_notpay->T_refno,sizeof(hv_notpay->T_refno));/**add by xyy 2006-8-29 9:57**/
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,hv_notpay->T_wt_date);
	MEMCPY_DEBUG(pack724->workDate,hv_notpay->T_tx_date,8);		/*��������*/
	iHVSETTAG( pack724,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVSETTAG(pack724,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*���������*/
	iHVSETTAG(pack724,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�����к�*/
	iHVSETTAG(pack724,"72A",cSub_content,sizeof(cSub_content));					/*����*/

	MEMCPY_DEBUG(cAddid,hv_notpay->T_addid , sizeof(cAddid)-1);
	zip_space(cAddid);
	if (strlen(cAddid)==8)
	{
		/* begin add by LiuHuafeng 20061002 */
		char cRec_br_no[13];
		MEMSET_DEBUG(cRec_br_no, 0 , sizeof(cRec_br_no));
		/* end by LiuHuafeng 20061002 */
		iHvGetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,cAddid,"000",&hv_724,sizeof(hv_724));
		iHVSETTAG(pack724,"30B",hv_724.pj_date,sizeof(hv_724.pj_date));		/*Ʊ������*/
		iHVSETTAG(pack724,"21A",hv_724.pj_num,sizeof(hv_724.pj_num));		/*Ʊ�ݺ���*/
		iHVSETTAG(pack724,"D02",hv_724.hp_type,sizeof(hv_724.hp_type));		/*��Ʊ���*/
		MEMCPY_DEBUG(cRec_br_no, hv_724.df_br_no,sizeof(cRec_br_no)-1);
		zip_space(cRec_br_no);
		iHVSETTAG(pack724,"D01",cRec_br_no,sizeof(cRec_br_no)-1);		/*ԭָ���Ҹ��к�*/
		del_dot(hv_724.cp_amt,hv_724.cp_amt,15);
		iHVSETTAG(pack724,"33C",hv_724.cp_amt,sizeof(hv_724.cp_amt));		/*��Ʊ���*/
		iHVSETTAG(pack724,"C10",hv_724.hp_miya,sizeof(hv_724.hp_miya));		/*��Ʊ��Ѻ*/
		iHVSETTAG(pack724,"CC4",hv_724.pay_opn_br_no,sizeof(hv_724.pay_opn_br_no));	/*�����˿����к�*/
	}
	return 0; 
}

/************************
*       PART 2       ***
*   ���ʱ��Ĵ���ӿ� ***
************************/

int pub_com_CMT100_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack100)
{    
	char	cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0x00,sizeof(cTmpBuf));


	MEMCPY_DEBUG(hv_payin->T_cmtno,pack100->CMTCode,3);		/*CMT���*/
	hv_payin->T_operlevel[0] = pack100->PRI;			/**���ȼ�**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack100->workDate,8);	/**��������**/
	iHVGETTAG(pack100,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack100,"32A",cTmpBuf,sizeof(cTmpBuf));	/*���׽��*/
	vtcp_log("%s,%d tmpbuf===[%s]",__FILE__,__LINE__,cTmpBuf);
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack100,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack100,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack100,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack100,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack100,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack100,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVGETTAG(pack100,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack100,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack100,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack100,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack100,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack100,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*�տ��˵�ַ*/
	iHVGETTAG(pack100,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVGETTAG(pack100,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack100,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack100,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack100,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/
	/******���ҵ��û�и�����***************/	
	return 0; 
}
int pub_com_CMT101_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack101)
{  
	/***************ί���տ���أ�֧������***************************/  
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;

	HV_WTSK_ADD  hv_wtsk;/***������****/

	MEMSET_DEBUG(&hv_wtsk,0,sizeof(hv_wtsk));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack101->PRI;			/**���ȼ���**/
	MEMCPY_DEBUG(hv_payin->T_cmtno,pack101->CMTCode,3);		/**CMT���**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack101->workDate,8);	/**��������**/
	iHVGETTAG(pack101,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/**ί������**/
	iHVGETTAG(pack101,"32A",cTmpBuf,sizeof(cTmpBuf));					/*���׽��*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/**��������**/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack101,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack101,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack101,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack101,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack101,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack101,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack101,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack101,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack101,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack101,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack101,"CEJ",hv_payin->T_note_type,sizeof(hv_payin->T_note_type));	/*ƾ֤����*/
	iHVGETTAG(pack101,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVGETTAG(pack101,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack101,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack101,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack101,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/***��Ӹ�����*****/	
	iHVGETTAG(pack101,"30B",hv_wtsk.pj_date,sizeof(hv_wtsk.pj_date));		/*Ʊ������*/
	iHVGETTAG(pack101,"21A",hv_wtsk.pj_num,sizeof(hv_wtsk.pj_num));			/*Ʊ�ݺ���*/
	/*********iHvSetAddit��������hv_payin->T_addid�ֶθ�ֵ***/
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_wtsk,sizeof(hv_wtsk));

	return 0; 
}

int pub_com_CMT102_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack102)
{   
	/*****************���ճи������أ�֧������*************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_TSCF_ADD hv_tscf;  /****��������***********/

	MEMSET_DEBUG(&hv_tscf,0,sizeof(hv_tscf));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack102->PRI;			/**���ȼ�**/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"102",3);					/**CMT���**/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack102->workDate,8);	/**��������**/
	iHVGETTAG(pack102,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf));					/**����+���*/	
	iHVGETTAG(pack102,"32A",cTmpBuf,sizeof(cTmpBuf));					/*���׽��*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/**��������**/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/

	iHVGETTAG(pack102,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack102,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack102,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack102,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack102,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack102,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack102,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack102,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack102,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack102,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack102,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack102,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack102,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack102,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*����*/

	/***��������**/	
	iHVGETTAG(pack102,"30B",hv_tscf.pj_date,sizeof(hv_tscf.pj_date));		/*Ʊ������*/
	iHVGETTAG(pack102,"21A",hv_tscf.pj_num,sizeof(hv_tscf.pj_num));		/*Ʊ�ݺ���*/
	iHVGETTAG(pack102,"33A",hv_tscf.ptxamt,sizeof(hv_tscf.ptxamt));		/*�⳥���*/
	iHVGETTAG(pack102,"33B",hv_tscf.jtxamt,sizeof(hv_tscf.jtxamt));		/*�ܸ����*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_tscf,sizeof(hv_tscf));

	return 0; 
}
int pub_com_CMT103_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack103)
{   
	/****************�����ʽ�㻮�����ǣ�֧������**********************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack103->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"103",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack103->workDate,8);	/*��������*/
	iHVGETTAG(pack103,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack103,"32A",cTmpBuf,sizeof(cTmpBuf));					/*��� ����*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack103,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack103,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack103,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack103,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack103,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack103,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack103,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack103,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack103,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack103,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack103,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVGETTAG(pack103,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack103,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack103,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack103,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*����*/

	/*****û�и�����********/	
	return 0; 
}
int pub_com_CMT104_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack104)
{   
	double  dTxamt     = 0.0;
	/***************���ڴ���֧������***********************************/ 
	char cTmpBuf[19];	
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0]=pack104->PRI;				/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"104",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack104->workDate,8);	/*��������*/
	iHVGETTAG(pack104,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack104,"32A",cTmpBuf,sizeof(cTmpBuf));					/*���� ���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/	
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack104,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack104,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack104,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack104,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack104,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack104,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack104,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack104,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack104,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack104,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack104,"59B",hv_payin->T_cash_addr,sizeof(hv_payin->T_cash_addr));	/*�տ��˵�ַ*/
	iHVGETTAG(pack104,"OBF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVGETTAG(pack104,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack104,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack104,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack104,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/**********û�и�����******************************/	
	return 0; 
}
int pub_com_CMT105_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack105)
{    
	/************���м�ͬҵ���֧������********************************/
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_TYCJ_ADD hv_tycj;

	MEMSET_DEBUG(&hv_tycj,0,sizeof(hv_tycj));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack105->PRI;			/*���޼���*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"105",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack105->workDate,8);	/*��������*/
	iHVGETTAG( pack105,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack105,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack105,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack105,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack105,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack105,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack105,"CEF",hv_payin->T_yw_type,sizeof(hv_payin->T_yw_type));		/*ҵ������*/
	iHVGETTAG(pack105,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack105,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack105,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack105,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/******** ��������**/	
	{
	char cRate[8];
	char cLimit[6];
	MEMSET_DEBUG(cRate, 0 , sizeof(cRate));
	MEMSET_DEBUG(cLimit, 0 , sizeof(cLimit));
	iHVGETTAG(pack105,"CF3",cRate,sizeof(hv_tycj.rate));				/*�������*/
	iHVGETTAG(pack105,"CF4",cLimit,sizeof(hv_tycj.ratelimit));		/*�������*/
	sprintf(cRate,"%07d",atoi(cRate));
	sprintf(cLimit,"%05d",atoi(cLimit));
	MEMCPY_DEBUG(hv_tycj.rate,cRate,sizeof(hv_tycj.rate));
	MEMCPY_DEBUG(hv_tycj.ratelimit,cLimit,sizeof(hv_tycj.ratelimit));
	}
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_tycj,sizeof(hv_tycj));
	return 0; 
}
int pub_com_CMT108_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack108)
{   
	/**************�˻�֧������***************************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack108->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"108",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack108->workDate,8);	/*��������*/
	iHVGETTAG( pack108,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack108,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack108,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack108,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack108,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack108,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack108,"051",hv_payin->T_o_wt_date,sizeof(hv_payin->T_o_wt_date));	/*ԭί������*/
	iHVGETTAG(pack108,"02B",hv_payin->T_o_cmtno,sizeof(hv_payin->T_o_cmtno));		/*ԭcmt���*/
	iHVGETTAG(pack108,"CQ1",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*ԭ�������ʺ�*/
	iHVGETTAG(pack108,"CR1",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*ԭ����������*/
	iHVGETTAG(pack108,"CQ2",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*ԭ�տ����ʺ�*/
	iHVGETTAG(pack108,"CR2",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*ԭ�տ�������*/
	iHVGETTAG(pack108,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack108,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack108,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack108,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/
  iHVGETTAG(pack108,"005",hv_payin->T_o_orderno,sizeof(hv_payin->T_o_orderno));		/*ԭ֧���������*/

	/********û�и�����*********/	
	return 0; 
}
int pub_com_CMT122_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack122)
{   
	/***************�������л�Ʊ�ʽ���******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_HP_ADD hv_hp;/****������*****/

	MEMSET_DEBUG(&hv_hp,0,sizeof(hv_hp));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack122->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"122",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack122->workDate,8);	/*��������*/	
	iHVGETTAG( pack122,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack122,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack122,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack122,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack122,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack122,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack122,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack122,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack122,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack122,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack122,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVGETTAG(pack122,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack122,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/

  /***add   2006-11-7 17:17  *****/
	iHVGETTAG(pack122,"59E",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_name));		/*����Ʊ���ʺ�*/	
	
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack122,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack122,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack122,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));		/*����*/

	/****��������******/	
	iHVGETTAG(pack122,"59E",hv_hp.last_ac_no,sizeof(hv_hp.last_ac_no));		/*����Ʊ���ʺ�*/
	iHVGETTAG(pack122,"59D",hv_hp.last_name,sizeof(hv_hp.last_name));		/*����Ʊ������*/
	iHVGETTAG(pack122,"30B",hv_hp.pj_date,sizeof(hv_hp.pj_date));		/*Ʊ������*/
	iHVGETTAG(pack122,"21A",hv_hp.pj_num,sizeof(hv_hp.pj_num));			/*Ʊ�ݺ���*/
	iHVGETTAG(pack122,"C10",hv_hp.hpmy,sizeof(hv_hp.hpmy));			/*��Ʊ��Ѻ*/
	iHVGETTAG(pack122,"33C",hv_hp.cp_amt,sizeof(hv_hp.cp_amt));			/*��Ʊ���*/
	iHVGETTAG(pack122,"33D",hv_hp.dy_amt,sizeof(hv_hp.dy_amt));			/*������*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_hp,sizeof(hv_hp));
	return 0; 
}
int pub_com_CMT123_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack123)
{   
	/***************�������л�Ʊ�ʽ���******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;

	HV_123_ADD hv_123;/****������*****/

	MEMSET_DEBUG(&hv_123,0,sizeof(hv_123));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack123->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"123",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack123->workDate,8);	/*��������*/	
	iHVGETTAG( pack123,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack123,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack123,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack123,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack123,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack123,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack123,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack123,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack123,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack123,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack123,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVGETTAG(pack123,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack123,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack123,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack123,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/****��������******/	
	iHVGETTAG(pack123,"30B",hv_123.pj_date,sizeof(hv_123.pj_date));		/*Ʊ������*/
	iHVGETTAG(pack123,"21A",hv_123.pj_num,sizeof(hv_123.pj_num));			/*Ʊ�ݺ���*/
	iHVGETTAG(pack123,"C10",hv_123.hp_miya,sizeof(hv_123.hp_miya));			/*��Ʊ��Ѻ*/

  MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
  dTxamt = 0.0;
	iHVGETTAG(pack123,"33C",cTmpBuf,sizeof(cTmpBuf));						/*��Ʊ���*/
	iHvStr2Dbl(cTmpBuf,&dTxamt);
	dTxamt = dTxamt/100;
	vtcp_log("%s,%d  dTxamt====[%lf] ",__FILE__,__LINE__,dTxamt);	
	sprintf(hv_123.cp_amt,"%016.2f",dTxamt);	                  /*��Ʊ���*/
	vtcp_log("%s,%d  hv_123.cp_amt==[%s]",__FILE__,__LINE__,hv_123.cp_amt);

  MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));
  dTxamt = 0.0;			
	iHVGETTAG(pack123,"CNV",cTmpBuf,sizeof(cTmpBuf));						/*ʵ�ʽ�����*/
	iHvStr2Dbl(cTmpBuf,&dTxamt);
	dTxamt = dTxamt/100;
	vtcp_log("%s,%d  dTxamt====[%lf] ",__FILE__,__LINE__,dTxamt);	
	sprintf(hv_123.real_js_amt,"%016.2f",dTxamt);	              /*ʵ�ʽ�����*/
	vtcp_log("%s,%d  hv_123.real_js_amt==[%s]",__FILE__,__LINE__,hv_123.real_js_amt);

	dTxamt   =  0.0;
	iHVGETTAG(pack123,"CC5",hv_123.cash_opn_br_no,sizeof(hv_123.cash_opn_br_no));	/*����Ʊ�˿�����*/
	iHVGETTAG(pack123,"59E",hv_123.last_ac_no,sizeof(hv_123.last_ac_no));		/*����Ʊ���ʺ�*/
	iHVGETTAG(pack123,"59D",hv_123.last_name,sizeof(hv_123.last_name));		/*����Ʊ������*/
	iHVGETTAG(pack123,"30C",hv_123.return_date,sizeof(hv_123.return_date));	/*��ʾ��������*/
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_123,sizeof(hv_123));
	return 0; 
}
int pub_com_CMT124_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack124)
{   
	/***************���л�Ʊδ���˻��ʽ���************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_124_ADD hv_124;/****������*****/

	MEMSET_DEBUG(&hv_124,0,sizeof(hv_124));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack124->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"124",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack124->workDate,8);	/*��������*/	
	iHVGETTAG( pack124,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack124,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack124,"011",hv_payin->T_pay_qs_no,sizeof(hv_payin->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack124,"52A",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack124,"58A",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack124,"012",hv_payin->T_cash_qs_no,sizeof(hv_payin->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack124,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack124,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack124,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack124,"50B",hv_payin->T_pay_addr,sizeof(hv_payin->T_pay_addr));		/*�����˵�ַ*/
	iHVGETTAG(pack124,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack124,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_payin->T_orderno,hv_payin->T_orderno,sizeof(hv_payin->T_orderno));
	iHVGETTAG(pack124,"010",hv_payin->T_sendco,sizeof(hv_payin->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack124,"0B9",hv_payin->T_receco,sizeof(hv_payin->T_receco));		/*�ձ����Ĵ���*/
	iHVGETTAG(pack124,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/

	/****��������******/	
	iHVGETTAG(pack124,"C33",hv_124.spec_br_no,sizeof(hv_124.spec_br_no));		/*�ֽ�֧Ʊ�����*/
	iHVGETTAG(pack124,"30B",hv_124.pj_date,sizeof(hv_124.pj_date));			/*Ʊ������*/
	iHVGETTAG(pack124,"21A",hv_124.pj_num,sizeof(hv_124.pj_num));			/*Ʊ�ݺ���*/
	iHVGETTAG(pack124,"C10",hv_124.hp_miya,sizeof(hv_124.hp_miya));				/*��Ʊ��Ѻ*/

	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_124,sizeof(hv_124));
	return 0; 
}
int pub_com_CMT232_TAG2T(HV_PAYIN_AREA * hv_payin,THVPACKAGE * pack232)
{   
	/***************�������л�Ʊ�ʽ���******************************/ 
	char cTmpBuf[19];	
	double  dTxamt     = 0.0;
	HV_232_ADD hv_232;/****������*****/

	MEMSET_DEBUG(&hv_232,0,sizeof(hv_232));
	MEMSET_DEBUG(cTmpBuf, 0 , sizeof(cTmpBuf));

	hv_payin->T_operlevel[0] = pack232->PRI;			/*���ȼ�*/
	MEMCPY_DEBUG(hv_payin->T_cmtno,"232",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_payin->T_tx_date,pack232->workDate,8);	/*��������*/	
	iHVGETTAG( pack232,"30A",hv_payin->T_wt_date,sizeof(hv_payin->T_wt_date));		/*ί������*/
	iHVGETTAG(pack232,"OB6",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*���������*/
	iHVGETTAG(pack232,"32A",cTmpBuf,sizeof(cTmpBuf));					/*����+���*/
	MEMCPY_DEBUG(hv_payin->T_cur_no,cTmpBuf,3);				/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_payin->T_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"0BC",hv_payin->T_orderno,sizeof(hv_payin->T_orderno));		/*���еĽ������*/
	iHVGETTAG(pack232,"CC7",hv_payin->T_or_br_no,sizeof(hv_payin->T_or_br_no));		/*������к�*/
	iHVGETTAG(pack232,"CC4",hv_payin->T_pay_opn_br_no,sizeof(hv_payin->T_pay_opn_br_no));/*�����˿����к�*/
	iHVGETTAG(pack232,"50C",hv_payin->T_pay_ac_no,sizeof(hv_payin->T_pay_ac_no));	/*�������˺�*/
	iHVGETTAG(pack232,"50A",hv_payin->T_pay_name,sizeof(hv_payin->T_pay_name));		/*����������*/
	iHVGETTAG(pack232,"CC8",hv_payin->T_ac_br_no,sizeof(hv_payin->T_ac_br_no));		/*���������к�*/
	iHVGETTAG(pack232,"CC5",hv_payin->T_cash_opn_br_no,sizeof(hv_payin->T_cash_opn_br_no));		/*�տ��˿����к�*/
	iHVGETTAG(pack232,"59C",hv_payin->T_cash_ac_no,sizeof(hv_payin->T_cash_ac_no));	/*�տ����˺�*/
	iHVGETTAG(pack232,"59A",hv_payin->T_cash_name,sizeof(hv_payin->T_cash_name));	/*�տ�������*/
	iHVGETTAG(pack232,"72A",hv_payin->T_hv_brf,sizeof(hv_payin->T_hv_brf));			/*����*/
	strncpy(g_pub_tx.brf,hv_payin->T_hv_brf,sizeof(g_pub_tx.brf)-1);

	/****��������******/	
	iHVGETTAG(pack232,"90A",hv_232.tag_90A,sizeof(hv_232.tag_90A));		/*�����������/��������*/
	iHVGETTAG(pack232,"CEB",hv_232.tag_CEB,sizeof(hv_232.tag_CEB));		/*��ʱת��ҵ������*/
	iHVGETTAG(pack232,"CF2",hv_232.tag_CF2,sizeof(hv_232.tag_CF2));		/*ծȯ������ƥ��ָ��� */
	iHVGETTAG(pack232,"CF1",hv_232.tag_CF1,sizeof(hv_232.tag_CF1));		/*ծȯ���� */
	iHVGETTAG(pack232,"CNN",hv_232.tag_CNN,sizeof(hv_232.tag_CNN));		/*���۽�� */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNN,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNN,"%015.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"CNP",hv_232.tag_CNP,sizeof(hv_232.tag_CNP));		/*ծȯ��Ϣ*/
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNP,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNP,"%015.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"CNM",hv_232.tag_CNM,sizeof(hv_232.tag_CNM));		/*ծȯ���*/	
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNM,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNM,"%015.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"CNQ",hv_232.tag_CNQ,sizeof(hv_232.tag_CNQ));		/*�ع����ڽ���� */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNQ,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNQ,"%015.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"CJ8",hv_232.tag_CJ8,sizeof(hv_232.tag_CJ8));		/*�ع�������*/
	iHVGETTAG(pack232,"CNR",hv_232.tag_CNR,sizeof(hv_232.tag_CNR));		/*�ع���Ϣ */
	dTxamt     = 0.0;
	str2dbl(hv_232.tag_CNR,15,0,&dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_232.tag_CNR,"%015.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG(pack232,"90C",hv_232.tag_90C,sizeof(hv_232.tag_90C));		/*�����ʶ*/
	iHVGETTAG(pack232,"CCF",hv_232.tag_CCF,sizeof(hv_232.tag_CCF));		/*�����������CCPC����*/
	iHVGETTAG(pack232,"CCG",hv_232.tag_CCG,sizeof(hv_232.tag_CCG));		/*������������CCPC����*/
	/* begin add by LiuHuafeng 2009-5-17 19:52:16 �������־����operlevl��� */
	hv_payin->T_operlevel[0]=hv_232.tag_90C[0];
	iHvSetAddit(hv_payin->T_tx_date,hv_payin->T_wt_date,hv_payin->T_addid,"000",(char*)&hv_232,sizeof(hv_232));
	return 0; 
}
int pub_com_CMT253_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack253)
{   
	/**************������������ر���*********************************/ 
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"253",3);					/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack253->workDate,8);	/*��������*/
	iHVGETTAG( pack253,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVGETTAG(pack253,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*���������*/
	iHVGETTAG(pack253,"0BC",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));		/*֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack253,"C42",hv_notpay->T_resp1,sizeof(hv_notpay->T_resp1));		/*������*/
vtcp_log("%s,%d,hv_notpay->T_orderno   [%s]",__FILE__,__LINE__,hv_notpay->T_orderno );
vtcp_log("%s,%d,hv_notpay->T_or_br_no  [%s]",__FILE__,__LINE__,hv_notpay->T_or_br_no);
vtcp_log("%s,%d,hv_notpay->T_wt_date   [%s]",__FILE__,__LINE__,hv_notpay->T_wt_date );
vtcp_log("%s,%d,hv_notpay->T_resp1     [%s]",__FILE__,__LINE__,hv_notpay->T_resp1   );


	return 0; 
}
int pub_com_CMT910_TAG2T(HV_NOTPAY_AREA * hv_notpayin,THVPACKAGE * pack)
{   
	/***************���л�Ʊ�ʽ��ƴ汨��******************************/ 
	iHVGETTAG(pack,"30A",hv_notpayin->T_wt_date,sizeof(hv_notpayin->T_wt_date));/*ί������*/
	iHVGETTAG(pack,"C42",hv_notpayin->T_resp1  ,sizeof(hv_notpayin->T_resp1));	   /*������*/
	iHVGETTAG(pack,"72A",hv_notpayin->T_hv_brf ,sizeof(hv_notpayin->T_hv_brf));  /*����*/
	MEMCPY_DEBUG(hv_notpayin->T_refno,pack->reNo,sizeof(hv_notpayin->T_refno)); /* �ο��� */
	return 0; 
}

int pub_com_CMT301_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack301)
{    
	/****************��ѯ����*********************************/
	char cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"301",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack301->workDate,8);		/*��������*/
	iHVGETTAG(pack301,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	vtcp_log("cTmpBuf==[%s]\n",cTmpBuf);
	vtcp_log("���ʵĽ��dTxmat=[%f]\n",dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	vtcp_log("ת�����o_Tx_amt=[%s]\n",hv_notpay->T_o_tx_amt);
	iHVGETTAG(pack301,"CJ9",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));			/*��ѯ����*/
	iHVGETTAG(pack301,"01C",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*��ѯ���к�*/
	iHVGETTAG(pack301,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*����������*/
	iHVGETTAG(pack301,"056",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*��ѯ���*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack301,"01D",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�鸴���к�*/
	iHVGETTAG(pack301,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	iHVGETTAG(pack301,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVGETTAG(pack301,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*ԭ�������к�*/
	iHVGETTAG(pack301,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*ԭ�����к�*/
	iHVGETTAG(pack301,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVGETTAG(pack301,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack301,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVGETTAG(pack301,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVGETTAG(pack301,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*��ѯ����*/

	/***********û�и�����***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("��ѯ�ĳ�ʼ״̬Ϊ:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	hv_notpay->T_otype[0]='1';/**���**/
	return 0; 
}
int pub_com_CMT302_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack302)
{    
	/****************�鸴����*********************************/
	char cTmpBuf[19];
	double  dTxamt     = 0.0;
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"302",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack302->workDate,8);		/*��������*/
	iHVGETTAG(pack302,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG( pack302,"CJ1",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*�鸴����*/
	iHVGETTAG(pack302,"01D",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*�鸴���к�*/
	iHVGETTAG(pack302,"012",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*�鸴����������*/
	iHVGETTAG(pack302,"043",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*�鸴���*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack302,"CCB",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*ԭ��ѯ���к�*/
	iHVGETTAG(pack302,"CCC",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*ԭ��ѯ�����к�*/
	iHVGETTAG(pack302,"CP1",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*ԭ��ѯ���*/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack302,"CJA",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*ԭ��ѯ����*/
	iHVGETTAG(pack302,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVGETTAG(pack302,"CC1",hv_notpay->T_o_or_br_no,sizeof(hv_notpay->T_o_or_br_no));		/*ԭ�������к�*/
	iHVGETTAG(pack302,"CC2",hv_notpay->T_o_ac_br_no,sizeof(hv_notpay->T_o_ac_br_no));		/*ԭ�����к�*/
	iHVGETTAG(pack302,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVGETTAG(pack302,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack302,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVGETTAG(pack302,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVGETTAG(pack302,"053",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*��ѯ����*/

	/***********û�и�����***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("��ѯ�ĳ�ʼ״̬Ϊ:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_req_br_no));/***��ȡԭ��ѯ��****/
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT303_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack303)
{
	/*************���ɸ�ʽ����********************/

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"303",3);					/*CMT���*/ 
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack303->workDate,8);	/*��������*/
	iHVGETTAG( pack303,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));	/*ί������*/
	iHVGETTAG(pack303,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));	/*���������*/
	iHVGETTAG(pack303,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));	/*����������*/
	iHVGETTAG(pack303,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));	/*�����к�*/
	iHVGETTAG(pack303,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));	/*���������к�*/
	iHVGETTAG(pack303,"053",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));	/*����*/
	iHVGETTAG(pack303,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));		/*�������Ĵ���*/
	iHVGETTAG(pack303,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));		/*�ձ����Ĵ���*/
		
  vtcp_log(" hv_notpay->T_cmtno[%s] ",hv_notpay->T_cmtno);
  vtcp_log(" hv_notpay->T_tx_date[%s] ",hv_notpay->T_tx_date);
  vtcp_log(" hv_notpay->T_wt_date[%s] ",hv_notpay->T_wt_date);
  vtcp_log(" hv_notpay->T_or_br_no[%s] ",hv_notpay->T_or_br_no);
  vtcp_log(" hv_notpay->T_pay_qs_no[%s] ",hv_notpay->T_pay_qs_no);
  vtcp_log(" hv_notpay->T_ac_br_no[%s] ",hv_notpay->T_ac_br_no);
  vtcp_log(" hv_notpay->T_cash_qs_no[%s] ",hv_notpay->T_cash_qs_no);
  vtcp_log(" hv_notpay->T_req_content[%s] ",hv_notpay->T_req_content);
  vtcp_log(" hv_notpay->T_sendco[%s] ",hv_notpay->T_sendco);
  vtcp_log(" hv_notpay->T_receco[%s] ",hv_notpay->T_receco);   
  
	/****û�и�����***********/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	if(iGetHvFzfOrderno(hv_notpay->T_orderno))
	{
   	    sprintf(acErrMsg,"[%s][%d]���ɽ�����Ŵ���!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	return 0; 
}
int pub_com_CMT312_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack312)
{    
	/****************����Ӧ����*************/
	/***Ӧ�����еĵ�ԭ�����ֶθ�ֵ��T_req_orderno��ȥ**/
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"312",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack312->workDate,8);		/*��������*/
	iHVGETTAG(pack312,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));			/*ί������*/
	iHVGETTAG(pack312,"CCD",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*���볷���к�*/
	iHVGETTAG(pack312,"CP2",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*ԭ���������*/
    vtcp_log("[%s][%d]ԭ�����������req_orderno=[%s]\n",__FILE__,__LINE__,hv_notpay->T_req_orderno);
	/***add by liuyue*****/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack312,"046",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*����֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack312,"04C",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*����Ӧ��*/
	iHVGETTAG(pack312,"72A",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*����*/
	/****û�и�����*****/	
	/***add by liuyue 20060908***/
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***����һ���������*****/
	{
   	    sprintf(acErrMsg,"[%s][%d]���ɽ�����Ŵ���!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("��ѯ�ĳ�ʼ״̬Ϊ:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT313_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack313)
{    
	/****************�����˻ر���*********************************/
	double  dTxamt     = 0.0;
	/***�˻������ ��ֵ��orderno �ֶ�****/
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"313",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack313->workDate,8);		/*��������*/
	iHVGETTAG(pack313,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*��������*/
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	vtcp_log("!!!!!![%d]cTmpbuf==[%s]\n",__LINE__,cTmpBuf);
	dTxamt = dTxamt/100;
	vtcp_log("!!!!![%f]\n",dTxamt);
	vtcp_log("!!!!![%f]\n",dTxamt);
	sprintf(hv_notpay->T_o_tx_amt,"%16.2f",dTxamt);	/*���ҽ��*/
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	vtcp_log("t_o_tx_amt==[%s][%d]\n",hv_notpay->T_o_tx_amt,sizeof(hv_notpay->T_o_tx_amt));
	iHVGETTAG( pack313,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVGETTAG(pack313,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*�������к�*/
	iHVGETTAG(pack313,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*����������*/
	iHVGETTAG(pack313,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVGETTAG(pack313,"C47",hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));			/*�˻������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_orderno));
	iHVGETTAG(pack313,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�������к�*/
	iHVGETTAG(pack313,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	iHVGETTAG(pack313,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVGETTAG(pack313,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVGETTAG(pack313,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVGETTAG(pack313,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack313,"72A",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));		/*����*/

	/***********û�и�����***************/	
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("��ѯ�ĳ�ʼ״̬Ϊ:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_req_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_req_orderno));
	MEMCPY_DEBUG(hv_notpay->T_req_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_req_br_no));
	MEMCPY_DEBUG(hv_notpay->T_req_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_req_date));
	hv_notpay->T_otype[0]='1';/**���**/
	/*****����Ҫ����ԭ��֧��������֧�����ף�������Ҫ��ԭ֧�����׵Ľ����и�ֵ**/
	MEMCPY_DEBUG(hv_notpay->T_o_or_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));
	return 0; 
}
int pub_com_CMT314_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack314)
{    
	/****************�˻�Ӧ����*********************************/
	/***Modified by SSH,2006.9.7,��Щ�ֶθ�ֵ����****/
	double  dTxamt     = 0.0;
	char cTmpBuf[19];
	MEMSET_DEBUG(cTmpBuf,0,sizeof(cTmpBuf));

	MEMCPY_DEBUG(hv_notpay->T_cmtno,"314",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack314->workDate,8);		/*��������*/
	iHVGETTAG(pack314,"CND",cTmpBuf,sizeof(cTmpBuf));						/*ԭ���ҷ���,���*/
	MEMCPY_DEBUG(hv_notpay->T_o_cur_no,cTmpBuf,3);					/*��������*/	
	/*  Rem by ChenMing  2006-11-7 11:15
	vGetRecvTagTxamt(cTmpBuf,cTmpBuf);
	str2dbl(cTmpBuf,strlen(cTmpBuf),0,&dTxamt);*/
	iHvStr2Dbl(cTmpBuf+3, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/*���ҽ��*/
	iHVGETTAG( pack314,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVGETTAG(pack314,"CCE",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*�˻����к�*/
	iHVGETTAG(pack314,"011",hv_notpay->T_pay_qs_no,sizeof(hv_notpay->T_pay_qs_no));		/*����������*/
	iHVGETTAG(pack314,"010",hv_notpay->T_sendco,sizeof(hv_notpay->T_sendco));			/*�������Ĵ���*/
	iHVGETTAG(pack314,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�������к�*/
	iHVGETTAG(pack314,"012",hv_notpay->T_cash_qs_no,sizeof(hv_notpay->T_cash_qs_no));		/*���������к�*/
	iHVGETTAG(pack314,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVGETTAG(pack314,"CJB",hv_notpay->T_req_date,sizeof(hv_notpay->T_req_date));		/*ԭ�˻���������*/
	iHVGETTAG(pack314,"CP2",hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));		/*ԭ�˻��������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_req_orderno,hv_notpay->T_req_orderno,sizeof(hv_notpay->T_req_orderno));
	iHVGETTAG(pack314,"051",hv_notpay->T_o_wt_date,sizeof(hv_notpay->T_o_wt_date));		/*ԭί������*/
	iHVGETTAG(pack314,"CE2",hv_notpay->T_o_tx_type,sizeof(hv_notpay->T_o_tx_type));		/*ԭ��������*/
	iHVGETTAG(pack314,"005",hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));	/*ԭ֧���������*/
	/***add by liuyue*****/
	del_dot(hv_notpay->T_o_payin_orderno,hv_notpay->T_o_payin_orderno,sizeof(hv_notpay->T_o_payin_orderno));
	iHVGETTAG(pack314,"018",hv_notpay->T_respcode,sizeof(hv_notpay->T_respcode));		/*�˻�Ӧ��*/
	iHVGETTAG(pack314,"72A",hv_notpay->T_res_content,sizeof(hv_notpay->T_res_content));		/*����*/

	/***********û�и�����***************/	
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***����һ���������*****/
	{
   	    sprintf(acErrMsg,"[%s][%d]���ɽ�����Ŵ���!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
	hv_notpay->T_hv_fzf_sts[0] = '9';
	vtcp_log("��ѯ�ĳ�ʼ״̬Ϊ:[%c]\n",hv_notpay->T_hv_fzf_sts[0]);
	MEMCPY_DEBUG(hv_notpay->T_res_orderno,hv_notpay->T_orderno,sizeof(hv_notpay->T_res_orderno));
	MEMCPY_DEBUG(hv_notpay->T_res_br_no,hv_notpay->T_or_br_no,sizeof(hv_notpay->T_res_br_no));
	MEMCPY_DEBUG(hv_notpay->T_res_date,hv_notpay->T_wt_date,sizeof(hv_notpay->T_res_date));
	return 0; 
}
int pub_com_CMT725_TAG2T(HV_NOTPAY_AREA * hv_notpay,THVPACKAGE * pack725)
{   
	/**********���л�Ʊȫ��Ҹ�֪ͨ����**********/
	double  dTxamt     = 0.0;
	HV_725_ADD hv_725;
	MEMSET_DEBUG(&hv_725,' ',sizeof(hv_725));
	char cTx_amt[17];

	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(hv_notpay->T_cmtno,"725",3);						/*CMT���*/
	MEMCPY_DEBUG(hv_notpay->T_tx_date,pack725->workDate,8);		/*��������*/
	iHVGETTAG( pack725,"30A",hv_notpay->T_wt_date,sizeof(hv_notpay->T_wt_date));		/*ί������*/
	iHVGETTAG(pack725,"52A",hv_notpay->T_or_br_no,sizeof(hv_notpay->T_or_br_no));		/*���������*/
	iHVGETTAG(pack725,"0B9",hv_notpay->T_receco,sizeof(hv_notpay->T_receco));			/*�ձ����Ĵ���*/
	iHVGETTAG(pack725,"58A",hv_notpay->T_ac_br_no,sizeof(hv_notpay->T_ac_br_no));		/*�����к�*/
	iHVGETTAG(pack725,"72A",hv_notpay->T_req_content,sizeof(hv_notpay->T_req_content));					/*����*/
  hv_notpay->T_hv_fzf_sts[0] = '9';

  iHVGETTAG(pack725,"21A",hv_notpay->T_hv_brf,sizeof(hv_notpay->T_hv_brf));	/*Ʊ�ݺ���*/
  
	if(iGetHvFzfOrderno(hv_notpay->T_orderno)) /***����һ���������*****/
	{
   	  sprintf(acErrMsg,"[%s][%d]���ɽ�����Ŵ���!\n",__FILE__,__LINE__);
	    WRITEMSG
	    strcpy(g_pub_tx.reply,"HV01");
	    return (-1);
		
	}
  vtcp_log("[%s][%d] hv_notpay->T_orderno==[%.8s]  ",__FILE__,__LINE__,hv_notpay->T_orderno);

	/*****���￪ʼ��������****/	
	iHVGETTAG(pack725,"50C",hv_725.pay_ac_no,sizeof(hv_725.pay_ac_no));	/*�������ʺ�,��Ʊ�������ʺ�*/
	iHVGETTAG(pack725,"50A",hv_725.pay_name,sizeof(hv_725.pay_name));	/*����������*/
	iHVGETTAG(pack725,"50B",hv_725.pay_addr,sizeof(hv_725.pay_addr));	/*�����˵�ַ*/
	iHVGETTAG(pack725,"30B",hv_725.pj_date,sizeof(hv_725.pj_date));	/*Ʊ������*/
	iHVGETTAG(pack725,"21A",hv_725.pj_num,sizeof(hv_725.pj_num));	/*Ʊ�ݺ���*/
	iHVGETTAG(pack725,"C10",hv_725.hp_miya,sizeof(hv_725.hp_miya));		/*��Ʊ��Ѻ*/
	
	iHVGETTAG(pack725,"33C",hv_725.cp_amt,sizeof(hv_725.cp_amt));		/*��Ʊ���*/
	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(cTx_amt, hv_725.cp_amt, sizeof(hv_725.cp_amt));
	iHvStr2Dbl(cTx_amt, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_725.cp_amt,"%016.2f",dTxamt);	

	sprintf(hv_notpay->T_o_tx_amt,"%016.2f",dTxamt);	/* Added by Chenming  2006-11-6 23:15 */
		
	iHVGETTAG(pack725,"CNV",hv_725.real_js_amt,sizeof(hv_725.real_js_amt));	/*ʵ�ʽ�����*/
	dTxamt     = 0.0;
	MEMSET_DEBUG(cTx_amt, 0, sizeof(cTx_amt));
	MEMCPY_DEBUG(cTx_amt, hv_725.real_js_amt, sizeof(hv_725.real_js_amt));
	iHvStr2Dbl(cTx_amt, &dTxamt);
	dTxamt = dTxamt/100;
	sprintf(hv_725.real_js_amt,"%016.2f",dTxamt);	
	
	iHVGETTAG(pack725,"CC5",hv_725.cash_opn_br_no,sizeof(hv_725.cash_opn_br_no));/*�տ��˿����к�*/
	iHVGETTAG(pack725,"59E",hv_725.last_ac_no,sizeof(hv_725.last_ac_no));	/*����Ʊ���˺�*/
	iHVGETTAG(pack725,"59D",hv_725.last_name,sizeof(hv_725.last_name));		/*����Ʊ������*/
	iHVGETTAG(pack725,"30C",hv_725.return_date,sizeof(hv_725.return_date));	/*��ʾ��������*/

	iHvSetAddit(hv_notpay->T_tx_date,hv_notpay->T_wt_date,hv_notpay->T_addid,"000",(char*)&hv_725,sizeof(hv_725));

	return 0; 
}

/************************
*       PART 3       ***
*   ��������ӿ�   ***
************************/

/***************************************************************************/
/* ��������: iHvSetAddit                                                   */
/* ��������: ���Ӹ���������                                                */
/* ����:                                                                   */
/*         !!!!   �ڵ���ʱ�����Ƚ��в�������֤��addid����ΪNULL    !!!!    */
/*            hv_payin   -֧������ͨ������ṹ��                           */
/*                hv_payin->T_inday   hv_payin->T_addid                    */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���           >= 0                  */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ, Ϊ000��ʾ�˸�����Ҫ  */
/*           ֱ��ƴ����������������,����Ϊ�����ڲ�����                     */
/***************************************************************************/

int iHvSetAddit(char *inday,char *txday,char *addid,char *name,char *buf,int length)
{
	int iRc;
	int index = 0;
	struct hv_addit_c hv_addit;
	/*begin add by LiuHuafeng 20060323 */
	char cTxday[9];
	char cAddid[9];
	MEMSET_DEBUG(cTxday, '\0', sizeof(cTxday));
	MEMSET_DEBUG(cAddid, '\0', sizeof(cAddid));
	MEMSET_DEBUG(&hv_addit,'\0',sizeof(hv_addit));	
	MEMCPY_DEBUG(cTxday,txday,sizeof(cTxday)-1);
	zip_space(cTxday);	  	
	if ( strlen(cTxday) == 0 )
	{
		get_fd_data("0050",cTxday);
		/*MEMCPY_DEBUG(cTxday,pcHvGetDay(),sizeof(cTxday)-1);by lhf 20060325 */
	}
	sprintf(acErrMsg,"%s,%d, cTxday=[%s],addid=[%s]len_addid=[%d],len_txday=[%d]",
		__FILE__,__LINE__,cTxday,cAddid,strlen(cAddid),strlen(cTxday));
	WRITEMSG
	hv_addit.tx_date=apatoi(cTxday,8);
	MEMSET_DEBUG(cAddid, 0 , sizeof(cAddid));
	MEMCPY_DEBUG(cAddid,addid,sizeof(cAddid)-1);
	zip_space(cAddid);
	/****add liuyue 20061114 ����ַ����г��ֿ��ֽڵ�����***/
	for( index=0;index<length;index++)
		if(buf[index]==0x00)
			buf[index]=' ';
	/**end***********/
	/* MEMCPY_DEBUG(hv_addit.tx_date,cTxday,sizeof(hv_addit.tx_date)-1); */
	
	if ( strlen(cAddid) ==0 )
	{
		iGetHvAddid(hv_addit.addid);
		MEMCPY_DEBUG(hv_addit.tagname,name, sizeof(hv_addit.tagname)-1);
		MEMCPY_DEBUG(hv_addit.tagdata,buf,length);

		iRc = Hv_addit_Ins(hv_addit, g_pub_tx.reply);
		if (iRc != 0)
		{
			sprintf( acErrMsg,"���븽�����ݱ�ʧ��![%s][%d][%d]",__FILE__,__LINE__,iRc);
			set_zd_data(DC_GET_MSG,"���븽�����ݱ�ʧ��");
			WRITEMSG
			return(1);
		}				
		MEMCPY_DEBUG(addid,hv_addit.addid,sizeof(hv_addit.addid)-1);
	}else
	{
	sprintf(acErrMsg,"[%s][%d] addid=[%s]name=[%s]",__FILE__,__LINE__,addid,name);
	WRITEMSG
	sprintf(acErrMsg,"[%s][%d] buf==[%s]",__FILE__,__LINE__,buf);
	WRITEMSG

		iRc = Hv_addit_Sel(g_pub_tx.reply,&hv_addit,"addid='%s' and tagname='%s' ",cAddid,name );
		if (iRc != 0)
		{  /* ����������� */
			iRc = Hv_addit_Ins(&hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"���븽�����ݱ�ʧ��![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"���븽�����ݱ�ʧ��");
				WRITEMSG
				return(1);
			}		    	
		}
		else
		{    /* ��������� */
			iRc = Hv_addit_Dec_Upd(g_pub_tx.reply,"addid='%s' and tagname='%s' ",cAddid,name);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"DECLARE FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"DECLARE FOR ERROR");
				WRITEMSG
				return(1);
			}        
			iRc = Hv_addit_Fet_Upd(&hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"fetch FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"fetch FOR ERROR");
				WRITEMSG
					return(1);
			}	
			MEMCPY_DEBUG(hv_addit.tagdata,buf,length);

			iRc = Hv_addit_Upd_Upd(hv_addit, g_pub_tx.reply);
			if (iRc != 0)
			{
				sprintf( acErrMsg,"update FOR ERROR![%s][%d][%d]",__FILE__,__LINE__,iRc);
				set_zd_data(DC_GET_MSG,"update FOR ERROR");
				WRITEMSG
					return(1);
			}
			Hv_addit_Clo_Upd();  
		}	  
		/* if(addid != 0 ) ����ʱ��������֤������addid����ΪNULL��ֻ���ڴ����� */
		MEMCPY_DEBUG(addid,hv_addit.addid,sizeof(hv_addit.addid)-1);
	}
  vtcp_log("iHvSetAddit() ������������ !  ");	
	return(0);
}

/*******1.3.7.	�õ���������**************/
/***************************************************************************/
/* ��������: iHvGetAddit                                                   */
/* ��������: �õ�����������                                                */
/* ����:                                                                   */
/*          !!!!   �ڵ���ʱ�����Ƚ��в�������֤��          !!!!            */
/*            hv_payin   -֧������ͨ������ṹ��                           */
/*                hv_payin->T_inday  hv_payin->F_wt_date hv_payin->T_addid   */
/*            name       -���������������                                 */
/*            buf        -���������������                                 */
/*            length     -����������ĳ���           >= 0                  */
/* ����ֵ:   0/-1                                                          */
/* ��������: 2005.07.20                                                    */
/* ����:     ʯ����                                                        */
/* ˵��:     �������ݵ�����Ϊ3���ֽڳ������ֻ�����ĸ                       */
/***************************************************************************/
int iHvGetAddit(char *inday,char *wt_date,char *addid,char *name,char *buf,int length)
{
	int iRc;
	struct hv_addit_c hv_addit;
	char cTxday[9];	
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMSET_DEBUG(cTxday,0 , sizeof(cTxday));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMSET_DEBUG(&hv_addit,'\0',sizeof(hv_addit));
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	MEMCPY_DEBUG(cTxday,inday,sizeof(cTxday)-1);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	zip_space(cTxday);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);
	if (strlen(cTxday)!=8)
		MEMCPY_DEBUG(cTxday,wt_date,sizeof(cTxday)-1);
	vtcp_log("[%s][%d]",__FILE__,__LINE__);

	vtcp_log("[%s][%d]txday=[%.8s]",__FILE__,__LINE__,cTxday);
	vtcp_log("[%s][%d]addid=[%.8s]",__FILE__,__LINE__,addid);
	vtcp_log("[%s][%d]tagname=[%.3s]",__FILE__,__LINE__,name);

	iRc = Hv_addit_Sel(g_pub_tx.reply,&hv_addit,"addid='%.8s' and tagname='%.3s' and tx_date=%d ",addid,name,apatoi(cTxday,8));
	if ( iRc==100 )
	{
		sprintf( acErrMsg,"�����ڸø�������![%s][%d][%d]",__FILE__,__LINE__,iRc);
		set_zd_data(DC_GET_MSG,"�����ڸø�������");
		WRITEMSG
			strcpy(g_pub_tx.reply,"M003");
		return(1);
	}
	else if ( iRc )
	{
		sprintf( acErrMsg,"���Ҹ������ݱ����![%s][%d][%d]",__FILE__,__LINE__,iRc);
		set_zd_data(DC_GET_MSG,"���Ҹ������ݱ����");
		WRITEMSG
			strcpy(g_pub_tx.reply,"D103");
		return(1);
	}	
	if (length<=0)
	{
		strcpy(buf,hv_addit.tagdata);
	}else
	{
		MEMCPY_DEBUG(buf,hv_addit.tagdata,length);
	}
	vtcp_log("%s,%d buf==[%s]",__FILE__,__LINE__,buf);
	return(0);
}     

/****************************************************************************/
/*  OBJECT :  double truncation*/
/*  INPUT  :  ival->  123.4567*/
/*itnclen   ->  2*/
/*  OUTPUT :  ival->  123.45*/
/*  RETURN :  0->  sucessed*/
/*-1->  failed*/
/****************************************************************************/
int dbltrunc( ival, itnclen)
double  *ival;
int     itnclen;
{
	char lstrbuf[ MAX_BUF ];
	char lfmt[ MAX_BUF ];
	int     lofset,len;
	double  temp;
	sprintf(lfmt,"%%.%df",itnclen+1);
	sprintf(lstrbuf,lfmt,*ival);
	len=strlen(lstrbuf);
	for(lofset=0; ( lofset < len) && ( lstrbuf[lofset] != '.') ; lofset++);
	if ( itnclen >= 0 )
		MEMSET_DEBUG(&lstrbuf[lofset+itnclen+1],0x00,MAX_BUF-(lofset+itnclen+2 ));
	else
	{
		MEMSET_DEBUG(&lstrbuf[lofset+itnclen],'0', itnclen * -1 );
		MEMSET_DEBUG(&lstrbuf[lofset],0x00, MAX_BUF -lofset+1 );
	}
	temp=atof(lstrbuf);
	*ival=temp;
	return(0);
}

/****************************************************************************/
/*  OBJECT :  decimal to  string                                            */
/*  INPUT  :  idec      ->  456.123                                         */
/*            idot      ->  3                                               */
/*            ilen      ->  10           :output length                     */
/*            isign     ->  0 : none                                        */
/*                      ->  1 : add sign                                    */
/*  OUTPUT :  obuf      ->  +00123.456                                      */
/*  RETURN :  0         ->  sucessed                                        */
/*            -1        ->  failed                                          */
/****************************************************************************/
dbl2str( idbl,idot,ilen,isign,obuf )
	double	*idbl       ;
short	idot        ;
short	ilen        ;
short	isign       ;
char	*obuf       ;
{
	char	lstrbuf[ MAX_BUF ];
	char	lstrtmp[ MAX_BUF ];
	char	Lstr[ MAX_BUF ];
	char	fmt[10];
	char	*p1;
	double	ldbl;
	/*********** check input data ****************/
	if ( idot > ilen )
		return -1;
	/*********** truncate ************************/
	ldbl=*idbl;
	dbltrunc( &ldbl,idot );
	sprintf(fmt,"%%+#0%d.%df",ilen+2,idot);
	sprintf(lstrbuf,fmt,ldbl);
	strcpy(lstrtmp,&lstrbuf[1]);
	if ( isign != 0 )
	{
		strcpy(Lstr,lstrtmp);
		p1=strtok(Lstr,".");
		if ( strlen(p1) > ( ilen - idot - 1 ) )
			MEMCPY_DEBUG(&lstrbuf[1],&lstrtmp[strlen(p1)-ilen+idot+1],
			ilen-idot);
		else
			strcpy(&lstrbuf[1],p1);

		p1=strtok(NULL,".");
		MEMCPY_DEBUG(&lstrbuf[ilen-idot],p1,idot);
		if ( *idbl > 0 )
			lstrbuf[0]='+';
		else
			lstrbuf[0]='-';
	}
	else
	{
		strcpy(Lstr,lstrtmp);
		p1=strtok(Lstr,".");
		if ( strlen(p1) > ( ilen - idot ) )
			MEMCPY_DEBUG(lstrbuf,&lstrtmp[strlen(p1)-ilen+idot],
			ilen-idot);
		else
			strcpy(lstrbuf,p1);

		p1=strtok(NULL,".");
		MEMCPY_DEBUG(&lstrbuf[ilen-idot],p1,idot);
	}
	MEMCPY_DEBUG(obuf,lstrbuf,ilen);
	return(0);
}
/*************************************************
* �� �� ��:  iGetHvOrderno
* ���������� �õ����֧��ϵͳ��֧���������
*            ����֧������
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iGetHvOrderno(char * orderno)
{
	char cSeqno[9];
	char cKinbr[BRNO_LEN+1];
	int ret=0;
	struct seqctl_c hv_seqctl_c;
	MEMSET_DEBUG(cSeqno, 0 , sizeof(cSeqno));
	MEMSET_DEBUG(cKinbr, 0 , sizeof(cKinbr));
	MEMSET_DEBUG(&hv_seqctl_c, 0x00 , sizeof(hv_seqctl_c));
	get_fd_data("0030",cKinbr);
	vtcp_log("[%s][%d] cKinbr=[%s]",__FILE__,__LINE__,cKinbr);
	GetOr_br_noByBr_no(cKinbr,hv_seqctl_c.br_no);
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	vtcp_log("[%s][%d] hv_seqctl_c.ac_ind=[%s]",__FILE__,__LINE__,hv_seqctl_c.ac_ind);
	MEMCPY_DEBUG(hv_seqctl_c.ac_ind,HV_ORDERNO_IND,sizeof(hv_seqctl_c.ac_ind)-1);
	vtcp_log("[%s][%d] hv_seqctl_c.ac_ind=[%s]",__FILE__,__LINE__,hv_seqctl_c.ac_ind);
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	sprintf(acErrMsg,"%s,%d ac_ind=[%s],br_no=[%s]",
		__FILE__,__LINE__,hv_seqctl_c.ac_ind,hv_seqctl_c.br_no);
	WRITEMSG
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='%s'",
		hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	if (ret)
	{
		sprintf( acErrMsg,"ִ��Seqctl_Dec_Upd��![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Seqctl_Dec_Upd��");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"ִ��Seqctl_Fet_Upd��ѯ���֧��������ű����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Seqctl_Fet_Upd��ѯ���֧��������ű����");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"ִ��seqctl_Fet_Upd����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��seqctl_Fet_Upd����");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);	
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	vtcp_log("%s,%d ��ֵǰseqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d ��ֵ��seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);

	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"�޸Ļ����������![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"�޸Ļ����������");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(orderno,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"ȡ��֧���������=[%s]�ɹ�![%s]",cSeqno,g_pub_tx.reply);
	Seqctl_Sel(g_pub_tx.reply,&hv_seqctl_c," br_no='%s' and ac_ind='%s'",hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	Seqctl_Debug(&hv_seqctl_c);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"ȡ��֧���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}

/*************************************************
* �� �� ��:  iGetHvFzfOrderno
* ���������� �õ����֧��ϵͳ�ķ�֧���������
*            ����ȫ������
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iGetHvFzfOrderno(char * orderno)
{
	char cSeqno[9];
	char cKinbr[BRNO_LEN+1];
	int ret=0;
	struct seqctl_c hv_seqctl_c;
	MEMSET_DEBUG(cSeqno, 0 , sizeof(cSeqno));
	MEMSET_DEBUG(cKinbr, 0 , sizeof(cKinbr));
	MEMSET_DEBUG(&hv_seqctl_c, 0x00 , sizeof(hv_seqctl_c));
	vtcp_log("[%s][%d] hv_seqctl_c.br_no=[%s]",__FILE__,__LINE__,hv_seqctl_c.br_no);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply," br_no='%s' and ac_ind='%s'",
		MBFE_FZF_BR_NO,MBFE_FZF_IND);
	if (ret)
	{
		sprintf( acErrMsg,"ִ��Seqctl_Dec_Upd��![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Seqctl_Dec_Upd��");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"ִ��Seqctl_Fet_Upd��ѯ���֧��������ű����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Seqctl_Fet_Upd��ѯ���֧��������ű����");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"ִ��seqctl_Fet_Upd����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��seqctl_Fet_Upd����");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);	
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	vtcp_log("%s,%d ��ֵǰseqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d ��ֵ��seqno ++++++++ [%f]",__FILE__,__LINE__,hv_seqctl_c.seqno);

	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"�޸Ļ����������![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"�޸Ļ����������");
		WRITEMSG
		goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(orderno,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"ȡ��֧���������=[%s]�ɹ�![%s]",cSeqno,g_pub_tx.reply);
	Seqctl_Sel(g_pub_tx.reply,&hv_seqctl_c," br_no='%s' and ac_ind='%s'",hv_seqctl_c.br_no,hv_seqctl_c.ac_ind);
	Seqctl_Debug(&hv_seqctl_c);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"ȡ��֧���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}

/*************************************************
* �� �� ��:  iGetHvRefno
* ���������� �õ����֧��ϵͳ�Ĳο��ţ�ȫ��ͳһ����
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iGetHvRefno(char * pay_or_br_no,char * refno)
{
	struct seqctl_c hv_seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	int ret=0;
	MEMSET_DEBUG(cSeqno, 0 ,sizeof(cSeqno));
	MEMSET_DEBUG(cFullRefno, 0 , sizeof(cFullRefno));
	MEMSET_DEBUG(&hv_seqctl_c, 0 , sizeof(hv_seqctl_c));
	MEMCPY_DEBUG(cFullRefno,pay_or_br_no,12);
	vtcp_log("%s,%d Seqctl_Dec_Upd",__FILE__,__LINE__);
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",HV_BR_NO,HV_REFNO_IND);
	if (ret)
	{
		sprintf( acErrMsg,"ִ��Point_point_mst_Dec_Upd��![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Point_point_mst_Dec_Upd��");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Fet_Upd",__FILE__,__LINE__);
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"��ѯ���֧��������ű����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"��ѯ���֧��������ű����");
		WRITEMSG
		goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"ִ��seqctl_Fet_Upd����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��seqctl_Fet_Upd����");
		WRITEMSG
		goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);
	strncat(cFullRefno,cSeqno,sizeof(cSeqno)-1);
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	hv_seqctl_c.seqno++;
	vtcp_log("%s,%d Seqctl_Upd_Upd",__FILE__,__LINE__);
	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"�޸Ļ����������![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"�޸Ļ����������");
		WRITEMSG
		goto ErrExit;
	}
	vtcp_log("%s,%d Seqctl_Clo_Upd",__FILE__,__LINE__);
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(refno,cFullRefno,sizeof(cFullRefno)-1);
	sprintf(acErrMsg,"ȡ��֧��������ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"ȡ��֧���������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}
/*************************************************
* �� �� ��:  iGetHvFlgno
* ���������� �õ����֧��ϵͳ�ı��ı�ʶ�ţ�ȫ��ͳһ����
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iGetHvFlgno(char * refno,char * flgno)
{
	char cRefno[21];
	char cFlgno[9];
	MEMSET_DEBUG(cFlgno, 0 , sizeof(cFlgno));
	MEMSET_DEBUG(cRefno, 0 , sizeof(cRefno));
	MEMCPY_DEBUG(cRefno,refno,sizeof(cRefno)-1);
	vtcp_log("%s,%d refno==[%s]",__FILE__,__LINE__,cRefno);
	if (strlen(cRefno)!=20)
		return -1;
	MEMCPY_DEBUG(cFlgno,cRefno+13,7);
	cFlgno[7]=rand()%10+'0';
	vtcp_log("%s,%d refno==[%s]flag7=[%c]",__FILE__,__LINE__,cRefno,cFlgno[7]);
	MEMCPY_DEBUG(flgno,cFlgno,sizeof(cFlgno)-1);
	return 0;
}

/*************************************************
* �� �� ��:  iGetHvAddid
* ���������� �õ����֧��ϵͳ�ĸ�����ţ�ȫ��ͳһ����
*
* ��    ��:  LiuHuafeng
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iGetHvAddid(char * addid)
{
	struct seqctl_c hv_seqctl_c;
	char cFullRefno[21];
	char cSeqno[9];
	int ret=0;
	MEMSET_DEBUG(cSeqno, 0 ,sizeof(cSeqno));
	MEMSET_DEBUG(&hv_seqctl_c, 0 , sizeof(hv_seqctl_c));
	ret=Seqctl_Dec_Upd(g_pub_tx.reply,"br_no='%s' and ac_ind='%s'",
		HV_BR_NO,HV_ADDID_IND);
	if (ret)
	{
		sprintf( acErrMsg,"ִ��Point_point_mst_Dec_Upd��![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��Point_point_mst_Dec_Upd��");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Fet_Upd(&hv_seqctl_c,g_pub_tx.reply);
	if (ret==100)
	{
		sprintf( acErrMsg,"��ѯ���֧����������ű����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"��ѯ���֧����������ű����");
		WRITEMSG
			goto ErrExit;
	}
	else if (ret)
	{
		sprintf( acErrMsg,"ִ��seqctl_Fet_Upd����![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"ִ��seqctl_Fet_Upd����");
		WRITEMSG
			goto ErrExit;
	}
	sprintf(cSeqno,"%08.0f",hv_seqctl_c.seqno);
	if (hv_seqctl_c.seqno==99999999) hv_seqctl_c.seqno=0;
	hv_seqctl_c.seqno++;
	ret=Seqctl_Upd_Upd(hv_seqctl_c,g_pub_tx.reply);
	if (ret!=0)
	{
		sprintf( acErrMsg,"�޸Ļ����������![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"�޸Ļ����������");
		WRITEMSG
			goto ErrExit;
	}
	ret=Seqctl_Clo_Upd();
	OkExit:
		MEMCPY_DEBUG(addid,cSeqno,sizeof(cSeqno)-1);
	sprintf(acErrMsg,"ȡ��֧�����׸�������ųɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
		return 0;
	ErrExit:
		sprintf(acErrMsg,"ȡ��֧�����׸��������ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
		return 1;
}


/*************************************************
* �� �� ��:  iHvTable2Notpayin
* ���������� ����NOTPAY�ṹ���е������ֶμ��������ݱ�,
*			 �����ؽ����notpay��T�ֶ���?
* ��    ��:	 dadary 
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

int iHvTable2Notpayin(HV_NOTPAY_AREA *phv_notpay)
{
	int ret = 0;
	struct hv_fzf_c hv_fzf;/**��֧���ṹ��**/
	MEMSET_DEBUG(&hv_fzf,0x00,sizeof(hv_fzf));

	/*****����F�ֶζ�hv_fzf�ж�Ӧ���ֶθ�ֵ,��Ϊ����****/
	MEMCPY_DEBUG(hv_fzf.or_br_no,phv_notpay->F_or_br_no,sizeof(phv_notpay->F_or_br_no));
	hv_fzf.wt_date = apatoi(phv_notpay->F_wt_date,8);
	MEMCPY_DEBUG(hv_fzf.orderno,phv_notpay->F_orderno,sizeof(phv_notpay->F_orderno));
	
	MEMCPY_DEBUG(hv_fzf.cmtno,phv_notpay->F_cmtno,sizeof(phv_notpay->F_cmtno));

	sprintf(acErrMsg,"orbrno=[%.12s],wt_date=[%d],orderno=[%s],cmtno=[%s]\n",\
		hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno,
		hv_fzf.cmtno);WRITEMSG
	/****SELECT���ݱ�********/
	ret = Hv_fzf_Sel(g_pub_tx.reply,&hv_fzf,"or_br_no='%s' and wt_date='%d' and orderno='%s' and cmtno='%s'", \
		 hv_fzf.or_br_no,hv_fzf.wt_date,hv_fzf.orderno,hv_fzf.cmtno);
	if (ret!=0)
	{
		sprintf( acErrMsg,"Select hv_zf table error!![%s][%d][%d]",__FILE__,__LINE__,ret);
		set_zd_data(DC_GET_MSG,"Select hv_zf table error!");
		WRITEMSG
		return 1;
	}
	phv_notpay->T_otype[0]=	hv_fzf.otype[0];
	MEMCPY_DEBUG(phv_notpay->T_or_br_no,	hv_fzf.or_br_no,	sizeof(hv_fzf.or_br_no));
	MEMCPY_DEBUG(phv_notpay->T_orderno,	hv_fzf.orderno,		sizeof(hv_fzf.orderno));
	apitoa(hv_fzf.tx_date,	sizeof(phv_notpay->T_tx_date),phv_notpay->T_tx_date);
	apitoa(hv_fzf.wt_date,	sizeof(phv_notpay->T_wt_date),phv_notpay->T_wt_date);
	vtcp_log("[%s][%d]hv_notpay.T_wt_date=[%.8s]",__FILE__,__LINE__,phv_notpay->T_wt_date);
	MEMCPY_DEBUG(phv_notpay->T_br_no,	hv_fzf.br_no,		sizeof(hv_fzf.br_no));
	phv_notpay->T_hv_fzf_sts[0]  =  hv_fzf.hv_fzf_sts[0];
	MEMCPY_DEBUG(phv_notpay->T_cmtno,	hv_fzf.cmtno,		sizeof(hv_fzf.cmtno));
	/**
	sprintf(phv_notpay->T_tx_time,	"%08s",			hv_fzf.tx_time);
	apitoa(hv_fzf.o_trace_no,sizeof(phv_notpay->T_o_trace_no),phv_notpay->T_o_trace_no);
	**/
	sprintf(phv_notpay->T_tx_time,	"%6d",			hv_fzf.tx_time);
	sprintf(phv_notpay->T_o_trace_no,"%6d",			hv_fzf.trace_no);
	MEMCPY_DEBUG(phv_notpay->T_pay_qs_no,	hv_fzf.pay_qs_no,	sizeof(hv_fzf.pay_qs_no));
	MEMCPY_DEBUG(phv_notpay->T_ac_br_no,	hv_fzf.ac_br_no,	sizeof(hv_fzf.ac_br_no));
	MEMCPY_DEBUG(phv_notpay->T_cash_qs_no,hv_fzf.cash_qs_no,	sizeof(hv_fzf.cash_qs_no));
	/**
	apitoa(hv_fzf.o_tx_date,sizeof(phv_notpay->T_o_wt_date),phv_notpay->T_o_wt_date);
	**/
	sprintf(phv_notpay->T_o_wt_date,"%8d",hv_fzf.o_wt_date);
	MEMCPY_DEBUG(phv_notpay->T_o_or_br_no,hv_fzf.o_or_br_no,	sizeof(hv_fzf.o_or_br_no));
	MEMCPY_DEBUG(phv_notpay->T_o_ac_br_no,hv_fzf.o_ac_br_no,	sizeof(hv_fzf.o_ac_br_no));
	MEMCPY_DEBUG(phv_notpay->T_o_payin_orderno,hv_fzf.o_orderno,	sizeof(hv_fzf.o_orderno));
	MEMCPY_DEBUG(phv_notpay->T_o_cur_no,	hv_fzf.o_cur_ind,	sizeof(hv_fzf.o_cur_ind));
	/**
	dbl2str(hv_fzf.o_tx_amt,2,sizeof(phv_notpay->T_o_tx_amt),0,phv_notpay->T_o_tx_amt);
	dot_dbl2str(&hv_fzf.o_tx_amt,0,sizeof(phv_notpay->T_o_tx_amt),0,phv_notpay->T_o_tx_amt);
	**/
	/****Modified by SSH,2006.9.7,ȡ����С����Ľ��****/
	dot_dbl2str(hv_fzf.o_tx_amt,sizeof(phv_notpay->T_o_tx_amt),2,phv_notpay->T_o_tx_amt);
	
	vtcp_log("[%s][%d]o_tx_amt=[%f]T_o_tx_amt=[%s]\n",__FILE__,__LINE__,hv_fzf.o_tx_amt,phv_notpay->T_o_tx_amt);
	MEMCPY_DEBUG(phv_notpay->T_o_cmtno,	hv_fzf.o_cmtno,		sizeof(hv_fzf.o_cmtno));
	MEMCPY_DEBUG(phv_notpay->T_o_tx_type,	hv_fzf.o_tx_type,	sizeof(hv_fzf.o_tx_type));
	MEMCPY_DEBUG(phv_notpay->T_sendco,	hv_fzf.sendco,		sizeof(hv_fzf.sendco));
	MEMCPY_DEBUG(phv_notpay->T_receco,	hv_fzf.receco,		sizeof(hv_fzf.receco));
	apitoa(hv_fzf.req_date,	sizeof(phv_notpay->T_req_date),phv_notpay->T_req_date);
	MEMCPY_DEBUG(phv_notpay->T_req_br_no,	hv_fzf.req_br_no,	sizeof(hv_fzf.req_br_no));
	MEMCPY_DEBUG(phv_notpay->T_req_content,hv_fzf.req_content,	sizeof(hv_fzf.req_content));
	MEMCPY_DEBUG(phv_notpay->T_req_orderno,hv_fzf.req_orderno,	sizeof(hv_fzf.req_orderno));
	apitoa(hv_fzf.res_date,	sizeof(phv_notpay->T_res_date),phv_notpay->T_res_date);
	MEMCPY_DEBUG(phv_notpay->T_res_br_no,	hv_fzf.res_br_no,	sizeof(hv_fzf.res_br_no));
	MEMCPY_DEBUG(phv_notpay->T_res_content,hv_fzf.res_content,	sizeof(hv_fzf.res_content));
	MEMCPY_DEBUG(phv_notpay->T_res_orderno,hv_fzf.res_orderno,	sizeof(hv_fzf.res_orderno));
	phv_notpay->T_respcode[0] =  hv_fzf.res_sts[0];
	MEMCPY_DEBUG(phv_notpay->T_addid,	hv_fzf.addid,		sizeof(hv_fzf.addid));
	MEMCPY_DEBUG(phv_notpay->T_refno,	hv_fzf.refno,		sizeof(hv_fzf.refno));
	MEMCPY_DEBUG(phv_notpay->T_hv_prt_ind,hv_fzf.hv_prt_ind,sizeof(hv_fzf.hv_prt_ind));
	MEMCPY_DEBUG(phv_notpay->T_hv_brf,	hv_fzf.hv_brf,sizeof(hv_fzf.hv_brf));
	vtcp_log("[%s,%d]����iHvTable2Notpayin�Ѿ�����",__FILE__,__LINE__);
	return 0;
}
/*************************************************
* �� �� ��:  iResetNotpay
* ���������� ��NOTPAY�ṹ���е�T�ֶθ�ֵ��F�ֶ��� 
*
* ��    ��:  dadary 
* ������ڣ� 2006-8-9 14:57
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/
int iResetNotpay(HV_NOTPAY_AREA *phv_notpay)
{
	sprintf(acErrMsg,"Begin to reset struct Notpay,T_*->F_*!");
	WRITEMSG
		MEMCPY_DEBUG(phv_notpay->F_or_br_no,	phv_notpay->T_or_br_no,	sizeof(phv_notpay->F_or_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_orderno,	phv_notpay->T_orderno,	sizeof(phv_notpay->F_orderno));	
	MEMCPY_DEBUG(phv_notpay->F_tx_date,	phv_notpay->T_tx_date,	sizeof(phv_notpay->F_tx_date));	
	MEMCPY_DEBUG(phv_notpay->F_wt_date,	phv_notpay->T_wt_date,	sizeof(phv_notpay->F_wt_date));	
	MEMCPY_DEBUG(phv_notpay->F_tx_br_no,	phv_notpay->T_br_no,	sizeof(phv_notpay->F_tx_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_cmtno,		phv_notpay->T_cmtno,	sizeof(phv_notpay->F_cmtno));	
	MEMCPY_DEBUG(phv_notpay->F_ac_br_no,	phv_notpay->T_ac_br_no,	sizeof(phv_notpay->F_ac_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_wt_date,	phv_notpay->T_o_wt_date,sizeof(phv_notpay->F_o_wt_date));	
	MEMCPY_DEBUG(phv_notpay->F_o_or_br_no,phv_notpay->T_o_or_br_no,sizeof(phv_notpay->F_o_or_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_ac_br_no,phv_notpay->T_o_ac_br_no,sizeof(phv_notpay->F_o_ac_br_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_payin_orderno,	phv_notpay->T_o_payin_orderno,	sizeof(phv_notpay->F_o_payin_orderno));	
	MEMCPY_DEBUG(phv_notpay->F_o_cur_no,	phv_notpay->T_o_cur_no,sizeof(phv_notpay->F_o_cur_no));	
	MEMCPY_DEBUG(phv_notpay->F_o_txamt,	phv_notpay->T_o_tx_amt,sizeof(phv_notpay->F_o_txamt));	
	MEMCPY_DEBUG(phv_notpay->F_o_cmtno,	phv_notpay->T_o_cmtno,	sizeof(phv_notpay->F_o_cmtno));	
	MEMCPY_DEBUG(phv_notpay->F_o_tx_type,	phv_notpay->T_o_tx_type,sizeof(phv_notpay->F_o_tx_type));	

	if (!memcmp(phv_notpay->T_cmtno,"302",3)||!memcmp(phv_notpay->T_cmtno,"312",3)||!memcmp(phv_notpay->T_cmtno,"314",3) )
	{
		/******NOTE:����Ӧ����Ľ���,�丽�Բ��ֶ�д����res_content�У������������ཻ�׸��Բ���д����req_content��*******/
		/******����������ཻ�׵Ļ���T_req_ordernoӦ���ǿյģ�ֻ�ǵ�λӦ�����ʱ��req_orderno�в�������,���Ҷ����ֶ�T_res_ordernoһֱû���õ�*****/
		/***ֻ�н�������Ϊ302 312 314��ʱ�� req�ֶβ�������*********/

		MEMCPY_DEBUG(phv_notpay->F_content,	phv_notpay->T_res_content,sizeof(phv_notpay->F_content));	
		MEMCPY_DEBUG(phv_notpay->F_o_fzf_date,phv_notpay->T_req_date,sizeof(phv_notpay->F_o_fzf_date));
			MEMCPY_DEBUG(phv_notpay->F_o_notpay_orderno,	phv_notpay->T_req_orderno,sizeof(phv_notpay->F_o_notpay_orderno));
	}
	else
	{
		MEMCPY_DEBUG(phv_notpay->F_content,	phv_notpay->T_req_content,sizeof(phv_notpay->F_content));	
	}
	MEMCPY_DEBUG(phv_notpay->F_respcode,	phv_notpay->T_respcode,	sizeof(phv_notpay->F_respcode));	
	MEMCPY_DEBUG(phv_notpay->F_refno,		phv_notpay->T_refno,	sizeof(phv_notpay->F_refno));	
	MEMCPY_DEBUG(phv_notpay->F_hv_brf,		phv_notpay->T_hv_brf,	sizeof(phv_notpay->F_hv_brf));	
}

/****************************************************************************
 * ��������: dot_dbl2str(double, int size,int dot,char * str)               *
 * ��������: ��dblת��Ϊ�ַ������ͺ�С����                                  *
 * ��������: Ҫת����double�������ܳ��ȣ�С����λ�ã�ת���������ַ���       *
 * ��������: 2006-8-19 15:58                                                *
 * ����:     LiuHuafeng                                                     *
*****************************************************************************/
int dot_dbl2str(double dvalue, int size,int dot,char * str)
{
	char cTmpbuf[100];
	char cFormat[30];
	MEMSET_DEBUG(cTmpbuf, 0 , sizeof(cTmpbuf));
	MEMSET_DEBUG(cFormat, 0 , sizeof(cFormat));
	sprintf(cFormat,"%%%d.%df",size,dot);
	sprintf(cTmpbuf,cFormat,dvalue);
	zip_space(cTmpbuf);
	MEMCPY_DEBUG(str,cTmpbuf,size);
	if(size<dot) 
		return -1;
	else 
		return 0;
}
int dot_str2dbl(char * str,int size,double *d_value)
{
	char * endstr;
	char cStr[101];
	MEMSET_DEBUG(cStr, 0 , sizeof(cStr));
	if(size>sizeof(cStr)-1)
		return -1;
	else
		MEMCPY_DEBUG(cStr, str,size);
	vtcp_log("%s,%d cStr=%s",__FILE__,__LINE__,cStr);			
	*d_value=strtod(cStr,&endstr);
	vtcp_log("%s,%d txamt=%f",__FILE__,__LINE__,*d_value);	
	return 0;
}
int iHVSETTAG(THVPACKAGE * pack,char * tag,char * content,int size)
{
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	char *cContent = (char *)malloc(size+1);
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	MEMSET_DEBUG(cContent, 0 , size+1);
	MEMCPY_DEBUG(cContent,content,size);
	cContent[size]='\0';
	vtcp_log("[%s][%d]content==[%s],tag=[%.3s],pack=[%ld],size=[%d,%d]\n",__FILE__,__LINE__,cContent,tag,pack,size,sizeof(cContent));
	hv_set_tag( pack,tag,cContent);
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	free(cContent);
	/*	
	vtcp_log("[%s][%d]content==[%s]\n",__FILE__,__LINE__,content);
	*20061218**/
	return 0;
}
int iHVGETTAG(THVPACKAGE * pack,char * tag,char * content,int size)
{
	hv_get_tag( pack,tag,content);
	vtcp_log("%s,%d size='%d'",__FILE__,__LINE__,size);
	vtcp_log("%s,%d content='%s'",__FILE__,__LINE__,content);
	return 0;
}
/****************************************************
 *ȥ�����(�ַ�����ʽ)�еĿո�,����ǰ�油0��15λ    *
 *��Ҫ���ڴ���ƴ����ʱ���32A�ֶ�,�÷�:             *
 *del_dot(hv_notpayin->tx_amt,cTmp_buf+3)           *
 *���ؽ��ֱ�Ӹ�����target�ֶ���*
 ****************************************************/
int  del_dot( char *source,char *target,int size)
{
	int index = 0;
	int iLen=0;
	int j=0;
	char cTmpbuf[40];
	char * endprt;
	double dValue=0.0;
	MEMSET_DEBUG(cTmpbuf, 0 , sizeof(cTmpbuf));
	MEMCPY_DEBUG(cTmpbuf,source,size);
	zip_space(cTmpbuf);
	iLen=strlen(cTmpbuf);
	for(index=0;index<iLen;index++) 
	{
		if(cTmpbuf[index]=='.')
		{
			for(j=index;j<iLen-1;j++)
				cTmpbuf[j]=cTmpbuf[j+1];
			iLen--;
		}
	}
	cTmpbuf[index]='\0';
	vtcp_log("%s,%d amt=[%s]",__FILE__,__LINE__,cTmpbuf);
	dValue=strtod(cTmpbuf,&endprt);
	dbl2str(&dValue,0,size,0,cTmpbuf);
	MEMCPY_DEBUG(target,cTmpbuf,size);
	vtcp_log("%s,%d target=[%s]",__FILE__,__LINE__,target);
	return 0;
}


/************************************************
* �������б����еĽ���ȫ����RMB+��0�Ľ�� *
* ����Ҫ��û������Ľ���Զ��ҳ��������ػ�ȥ  *
* ע��һ����ԭʼ�ı���tag���ݣ�����ǰ����RMB��  *
************************************************/
void vGetRecvTagTxamt(char *tagamt,char *newtxamt)
{
	char cTagamt[16];
	int i=0;
	MEMSET_DEBUG(cTagamt, 0 , sizeof(cTagamt));
	MEMCPY_DEBUG(cTagamt,tagamt+3,sizeof(cTagamt)-1);
	for(i=0;i<19;i++)
	{
		if(cTagamt[i]>='0' && cTagamt[i]<='9')
		{
		}
		else
		{
			cTagamt[i]='\0';
		}
	}
	MEMCPY_DEBUG(newtxamt,cTagamt,sizeof(cTagamt)-1);
	return;
}

/* 16λ�ַ���ת���� Double���� */
int iHvStr2Dbl(char* cAmt, double* dAmt)
{
	char cTmp_amt[17];
	int i;
	MEMSET_DEBUG(cTmp_amt, 0, sizeof(cTmp_amt));
	MEMCPY_DEBUG(cTmp_amt, cAmt, sizeof(cTmp_amt)-1);
	for(i=0;i<16;i++)
	{
		if(cTmp_amt[i]>='0' && cTmp_amt[i]<='9')
		{
		}
		else
		{
			cTmp_amt[i]='\0';
		}
	}
	*dAmt = atof(cTmp_amt);
	return 0;
}

/******************************************
* ��֧������������Ϣд�뵽�ļ��У�
* ��ֹ���ͺ����ݿ�rollback�ٴη��ͻ�ȡ��
*
*
*
******************************************/
int iInsertSendRecord(char* cmtno,char * txday,char *orderno,char *brno,char *wssrno,char *resp1)
{
	int  iFd=0,iSize=0;
	extern int errno;
	char cFilename[513];
	char cBasename[15];
	char cTxday[9];
	struct STR_BaseSend str_writeRecord;
	MEMCPY_DEBUG(cTxday, 0 , sizeof(cTxday));
	MEMCPY_DEBUG(cTxday,txday,sizeof(cTxday)-1);
	MEMSET_DEBUG(&str_writeRecord, 0 , sizeof(str_writeRecord));
	MEMSET_DEBUG(cFilename, 0 ,sizeof(cFilename));
	MEMSET_DEBUG(cBasename, 0 ,sizeof(cBasename));
	MEMCPY_DEBUG(cBasename,"sd",2);
	MEMCPY_DEBUG(cBasename+2,cTxday,8);
	MEMCPY_DEBUG(cBasename+10,".txt",4);
	sprintf(cFilename,"%s/%s",(char*)getenv("LOGDIR"),cBasename);
	vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
	iFd=open(cFilename,O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO);
	if(iFd==-1)
	{
		vtcp_log("[%s][%d]�ļ���������==[%d] \n",__FILE__,__LINE__,iFd);
		vtcp_log("[%s][%d] file=[%s] \n",__FILE__,__LINE__,cFilename);
		vtcp_log("[%s][%d] ��׼��д����ļ�[%s]���� \n",__FILE__,__LINE__,cFilename);
		return -1;
	}
	MEMCPY_DEBUG(str_writeRecord.cmtno  ,cmtno  ,sizeof(str_writeRecord.cmtno  ));
	str_writeRecord.separator1='|';
	MEMCPY_DEBUG(str_writeRecord.txday  ,txday  ,sizeof(str_writeRecord.txday  ));
	str_writeRecord.separator2='|';
	MEMCPY_DEBUG(str_writeRecord.brno   ,brno  ,sizeof(str_writeRecord.brno   ));
	str_writeRecord.separator3='|';
	MEMCPY_DEBUG(str_writeRecord.wssrno,wssrno,sizeof(str_writeRecord.wssrno));
	str_writeRecord.separator4='|';
	MEMCPY_DEBUG(str_writeRecord.orderno,orderno,sizeof(str_writeRecord.orderno));
	str_writeRecord.separator5='|';/* add by lhf 050803 */
	MEMCPY_DEBUG(str_writeRecord.resp1,resp1,sizeof(str_writeRecord.resp1));
	str_writeRecord.separator6='|';/* add by ayy 2006-11-20 22:00 */
	str_writeRecord.nextline='\n';

	iSize=write(iFd,(char *)&str_writeRecord,sizeof(str_writeRecord));
	if(iSize<=0)
	{
		vtcp_log("û�����ݱ�д�� errno=[%d] ,size=[%d] \n",errno,iSize);
		return -1;
	}
	if(close(iFd))
	{
		vtcp_log("[%s][%d] close id error \n",__FILE__,__LINE__);
		return -1;
	}
	return 0;
}
/*ѹ��ȫ�ǿո�*/
char *DBC2SBC(char *DBCstr,int cplen)
{
	int Length,i;
	char *dp;
	char *sp;
	static   char SBCstr[1024];
	dp = DBCstr;
	sp = SBCstr;
	MEMSET_DEBUG(SBCstr,'\0',sizeof(SBCstr));
	Length = 0;
	while(*dp!=0)
	{
		if(Length>=cplen)
		{
			break;
		}
		if((unsigned char)(*dp)>128)
		{
			*sp = *dp;
			*(sp+1) = *(dp+1);
			sp +=2;
			dp +=2;
			Length+=2;
		}else{
			if(*dp == ' ')
			{
				*sp = (char)(161);
				*(sp+1) = (char)(161); 
			}else{
				*sp = (char)(163);
				*(sp+1) = (char)( (unsigned char)(*dp)+128 );
			}
			sp +=2;
			dp ++;
			Length+=2;
		}
	}
	/*****20061110
	while(cplen>Length){
		*sp=(char)(161);
		*(sp+1)=(char)(161);
		sp+=2;
		Length+=2;
	}**************/	
	sp[cplen]='\0';	
	vtcp_log("[%s][%d]SBCstr===[%s]\n",__FILE__,__LINE__,SBCstr);
	return(SBCstr);
}
char *SBC2DBC(char *SBCstr)
{
	char *dp;
	char *sp;
	static  char DBCstr[1024];
	sp = SBCstr;
	dp = DBCstr;
	MEMSET_DEBUG(DBCstr,'\0',sizeof(DBCstr));
	if(strlen(sp) % 2!=0){
		MEMSET_DEBUG(dp,' ',strlen(sp));
		return(DBCstr);
	}
	while(*sp!=0)
	{
		if( *sp == (char)(161) ){
			*dp = ' ';
			sp +=2;
			dp +=1;
		}else if( *sp == (char)(163)){
			*dp = (unsigned char)(*(sp+1)) - 128;
			sp +=2;
			dp +=1;
		}else{
			*dp = *sp;
			*(dp+1) = *(sp+1);
			dp +=2;
			sp +=2;
		}
	}
	return(DBCstr);
}
char * dbc_zipspace(char *str)
{
	char str1[512];
	char str2[1024];
	char str3[512];
	char str4[1024];
	MEMSET_DEBUG(str1, 0, sizeof(str1));
	MEMSET_DEBUG(str2, 0, sizeof(str2));
	MEMSET_DEBUG(str3, 0, sizeof(str3));
	MEMSET_DEBUG(str4, 0, sizeof(str4));
	MEMCPY_DEBUG(str1, str, sizeof(str1));
	vtcp_log("[%s][%d]dbc_zipspace.str1==[%s]\n",__FILE__,__LINE__,str1);
	strcpy(str2,DBC2SBC(str1,strlen(str1)*2));
	vtcp_log("[%s][%d]dbc_zipspace.str2==[%s]\n",__FILE__,__LINE__,str2);
	strcpy(str3,SBC2DBC(str2));
	zip_space(str3);
	strcpy((char *)str,str3);
	vtcp_log("[%s][%d]dbc_zipspace.str==[%s]\n",__FILE__,__LINE__,str);
	return(str);
	/****************************/	
	vtcp_log("[%s][%d]dbc_zipspace.str3==[%s]\n",__FILE__,__LINE__,str3);
	strcpy(str4,DBC2SBC(str3,strlen(str3)*2));
	zip_space(str4);
	vtcp_log("[%s][%d]dbc_zipspace.str4==[%s]\n",__FILE__,__LINE__,str4);
	strcpy((char *)str,str4);
	vtcp_log("[%s][%d]dbc_zipspace.str==[%s]\n",__FILE__,__LINE__,str);
	return(str);
}
int iMbfeGetAcid(char * ac_no)
{
	struct mdm_ac_rel_c sMdm_ac_rel;
	MEMSET_DEBUG(&sMdm_ac_rel , 0 , sizeof(sMdm_ac_rel));
	MEMCPY_DEBUG(sMdm_ac_rel.ac_no,ac_no,sizeof(sMdm_ac_rel.ac_no)-1);
	pub_base_strpack(sMdm_ac_rel.ac_no);
	g_reply_int=Mdm_ac_rel_Sel(	g_pub_tx.reply,&sMdm_ac_rel,"ac_no='%s'",sMdm_ac_rel.ac_no);
	if(g_reply_int)
	{
		return g_reply_int;
	}
	else
		g_pub_tx.ac_id=sMdm_ac_rel.ac_id;
	return 0;
}
