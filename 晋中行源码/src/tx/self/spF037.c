/*************************************************
* �� �� ��:  spF037.c
* ���������� POS Ԥ��Ȩ����
*
* ��    ��:  rob
* ������ڣ� 2003��2��21��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#define EXTERN
#include "public.h"
#include "chnl_marchinfo_c.h"
#include "chnl_self_trade_c.h"
#include "chnl_premise_c.h"

int spF037()
{

	 int ret;
	 char flag;
	 long old_date;
	 long old_trace_no;
	 char dc_ind[2],ct_ind[2],fee_ind[2];
	 char tmpstr[51];
	 char msr_str[38];
	 struct chnl_marchinfo_c chnl_marchinfo;
	 struct chnl_self_trade_c chnl_self_trade;
	 struct chnl_premise_c    sChnl_premise;
	 struct chnl_premise_c    mChnl_premise;

	 memset(&chnl_marchinfo,0x0,sizeof(struct chnl_marchinfo_c));
	 memset(&chnl_self_trade,0x0,sizeof(struct chnl_self_trade_c));
	 memset(&sChnl_premise,0x0,sizeof(struct chnl_premise_c));
	 memset(&mChnl_premise,0x0,sizeof(struct chnl_premise_c));
	 memset(dc_ind,0,sizeof(dc_ind));
	 memset(ct_ind,0,sizeof(ct_ind));
	 memset(fee_ind,0,sizeof(fee_ind));
	 memset(tmpstr,0,sizeof(tmpstr));
	 memset(msr_str,0,sizeof(msr_str));
	 if (pub_base_sysinit())
	 {
		  sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		  WRITEMSG
		  goto ErrExit;
	 }

	 get_zd_data("0300",g_pub_tx.crd_no); /* ���� */
	 get_zd_double("0400",&g_pub_tx.tx_amt1);  /* ���ѽ�� */

	 /*get_zd_data("0790",g_pub_tx.draw_pwd); ���������,֧ȡ���� */

	 get_zd_data("0660",dc_ind); /*���*/
	 get_zd_data("0680",ct_ind); /*��ת*/
	 get_zd_data("0690",fee_ind); /*���ʱ�־*/
	 get_zd_long("0780",&old_trace_no);  /* ԭƽ̨��ˮ�� */
	 get_zd_long("0450",&old_date);      /* ԭƽ̨���� */
	 get_zd_long("0520",&sChnl_premise.pt_trace_no); /* ƽ̨��ˮ�� */
	 get_zd_long("0440",&sChnl_premise.pt_date); /*ƽ̨����*/
	 get_zd_int("0860",&sChnl_premise.l_premise_no); /*ԭƽ̨����*/
	 vtcp_log("%s,%d,ԭƽ̨��ˮ��==g_pub_tx.old_trace_no==[%ld]",__FILE__,__LINE__,sChnl_premise.l_premise_no);

	 /* ��鿨�ĺϷ��� */
	 if (pub_base_CheckCrdNo())
	 {
		  sprintf(acErrMsg,"��鿨�ĺϷ��Դ���!!");
		  WRITEMSG
		  goto ErrExit;
	 }
	 get_zd_data("0750",msr_str);/*2�ŵ���Ϣ*/
	 if(strlen(msr_str) != 0)	
	 { 
	 	g_reply_int=pubf_card_chk_msr(3,g_pub_tx.crd_no); /*cvn���*/
	 	if(g_reply_int)
	 	{
		  	sprintf(acErrMsg,"��鿨�ŵ���cvnʧ��!");
		  	WRITEMSG
		  	strcpy(g_pub_tx.reply, "CU14");
		  	goto ErrExit;
	 	}
	 } 
	 /* ��鿨��֧ȡ��ʽ *
	 if (g_mdm_ac_rel.draw_pwd_yn[0] != 'Y')
	 {
		  sprintf(acErrMsg,"�ÿ���֧ȡ��ʽ����,������ƾ����֧ȡ!!");
		  WRITEMSG
		  strcpy(g_pub_tx.reply,"CU55");
		  goto ErrExit;
	 }

	 * ���֧ȡ��ʽ�ĺϷ���
	 ret = pub_base_DesChk(g_pub_tx.tx_date, g_pub_tx.crd_no,
		                g_pub_tx.draw_pwd,g_mdm_ac_rel.draw_pwd);
	 if (ret != 0)
	 {
		  sprintf(acErrMsg,"����֧ȡ�������!!!");
		  WRITEMSG
		  strcpy(g_pub_tx.reply,"CU55");
		  goto ErrExit;
	 } 
	 ��������� �е�20080310*/


	 /* �Ǽ�Ԥ��Ȩ״̬��¼ */
	 ret = Chnl_premise_Dec_Upd(g_pub_tx.reply,"pt_date=%ld and pt_trace_no=%ld",old_date,old_trace_no);
	 if(ret)
	 {
		sprintf(acErrMsg,"�����α����!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU25");
		goto ErrExit;
	 } 
	 ret = Chnl_premise_Fet_Upd(&mChnl_premise,g_pub_tx.reply);
	 if(ret)
	 {
		sprintf(acErrMsg,"δ�ҵ���¼!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU25");
		Chnl_premise_Clo_Upd();
		goto ErrExit;
	 }
	 get_zd_data("0810",tmpstr);
	 memcpy(mChnl_premise.filler,tmpstr,30);
	 vtcp_log("%s,%d,mChnl_premise.premise_amt==[%f]",__FILE__,__LINE__,sChnl_premise.premise_amt);
	 vtcp_log("%s,%d,g_pub_tx.tx_amt1==[%f]",__FILE__,__LINE__,g_pub_tx.tx_amt1);
	 if(pub_base_CompDblVal(mChnl_premise.premise_amt,g_pub_tx.tx_amt1)==0)
	 {
		mChnl_premise.premise_amt = 0.0;
		mChnl_premise.sts[0]='3';	
	 }else if(pub_base_CompDblVal(mChnl_premise.premise_amt,g_pub_tx.tx_amt1)>0){
		mChnl_premise.premise_amt = mChnl_premise.premise_amt - g_pub_tx.tx_amt1;
		mChnl_premise.premise_no = mChnl_premise.l_premise_no;
	 }else{
		vtcp_log("%s,%d,������������Ȩ���",__FILE__,__LINE__);
		strcpy(g_pub_tx.reply,"CU64");
	 	Chnl_premise_Clo_Upd();
	 	goto ErrExit;
	 }
	 ret = Chnl_premise_Upd_Upd(mChnl_premise,g_pub_tx.reply);
	 if(ret)
	 {
		sprintf(acErrMsg,"����ԭ��¼����!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"CU25");
	 	Chnl_premise_Clo_Upd();
		goto ErrExit;
	 }
	 Chnl_premise_Clo_Upd();

	 /* �޸��ʻ��Ŀ��ƽ����ʻ��ļ�� */
	 g_pub_tx.tx_amt1 = 0 - g_pub_tx.tx_amt1;
	 if(dc_ind[0]=='1')
	 {
	 	g_pub_tx.add_ind[0] = '0'; /* ���ٿ��ƽ�� */
	 }else{
	 	g_pub_tx.add_ind[0] = '1'; /* ���ӿ��ƽ�� */
	 }
	 g_pub_tx.svc_ind = 1006;   /*  Ԥ��Ȩ  */
	 g_pub_tx.hst_ind[0] = '0'; /* ������ϸ */
	 memcpy(g_pub_tx.brf,tmpstr,20);

	 if (pub_acct_trance())
	 {
		  sprintf(acErrMsg,"pos Ԥ��Ȩ--����!!!");
		  WRITEMSG
		  goto ErrExit;
	 }
/*

	 chnl_self_trade.tx_date = g_pub_tx.tx_date;
	 chnl_self_trade.tx_time = g_pub_tx.tx_time;
	 chnl_self_trade.trace_no = g_pub_tx.trace_no;

	 strcpy(chnl_self_trade.tx_code,g_pub_tx.tx_code);
	 strcpy(chnl_self_trade.tx_br_no,g_pub_tx.tx_br_no);
	 strcpy(chnl_self_trade.opn_br_no,g_pub_tx.opn_br_no);
	 strcpy(chnl_self_trade.tel,g_pub_tx.tel);
	 strcpy(chnl_self_trade.ac_no,g_pub_tx.crd_no);
	 strcpy(chnl_self_trade.ttyp,"2");
	 strcpy(chnl_self_trade.sctp,"PS");
	 strcpy(chnl_self_trade.brf_no,"PSXF");
	 get_zd_long("0440",&chnl_self_trade.dz_date);
	 get_zd_long("0520",&chnl_self_trade.stra );
	 get_zd_long("0780",&chnl_self_trade.strb );
	 strcpy(chnl_self_trade.stat,"0");
	 ret = pub_base_Insertselftrade(chnl_self_trade);
	 if (ret != 0)
	 {
		  sprintf(acErrMsg,"�Ǽ������豸�����ռǴ���!!");
		  WRITEMSG
		  goto ErrExit;
	 }
*/

OkExit:
	 strcpy( g_pub_tx.reply, "0000" );
	 sprintf(acErrMsg,"success!!");
	 WRITEMSG
	 set_zd_data(DC_REPLY,g_pub_tx.reply);
	 return 0;
ErrExit:
	if(strlen(g_pub_tx.reply) == 0 || memcmp(g_pub_tx.reply,"0000",4)==0)
	{
		strcpy(g_pub_tx.reply,"CU06");
	}

	sprintf(acErrMsg,"failed !! reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
