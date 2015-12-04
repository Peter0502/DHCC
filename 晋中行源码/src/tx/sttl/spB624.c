/****************************************************
*�������ƣ�spB624.c
*�����ܣ�ʵ�ֶԳжҵ���弶����״̬�ĵ���
*����    ��lwb
*ʱ��    ��20150518
*insert into tx_def (TX_CODE, TX_NAME, BIT_MAP, LOG_IND, TX_TYPE, WRK_STS) values ('5624', '�жҵ���弶״̬����', '10000000000000000000000000010000000000000000001000000000000001100000000000000000000000001000000000000000000000000000000000000000', '1', '4', '2');
*insert into tx_flow_def (TX_CODE, FLOW_CODE, SUB_TX_CODE, SUB_TX_RSLT) values ('5624', 0, 'B624', '#$');
*insert into tx_sub_def (SUB_TX_CODE, SUB_TX_NAME, TX_FUNC, ACCT_IND, CHNL_IND) values ('B624', '�жҵ���弶״̬����', 'spB624', '0', '0');
*****************************************************/
#include <stdio.h>
#include <string.h>
#include "public.h"
#define EXTERN
#include "mo_bank_acc_po_c.h"
#include "ln_mst_c.h"

int spB624()
{
		char cPact_no[20];
		char cFive_sts[2];
		char cAc_sts[2];
		int  iRet=0;
			
		struct mo_bank_acc_po_c sMo_bank_acc_po;
		struct mdm_ac_rel_c sMdm_ac_rel;
		struct ln_mst_c sLn_mst;
		
		memset(cPact_no,0x00,sizeof(cPact_no));
		memset(cFive_sts,0x00,sizeof(cFive_sts));
		memset(cAc_sts,0x00,sizeof(cAc_sts));
		memset(&sMo_bank_acc_po,0x00,sizeof(sMo_bank_acc_po));
		memset(&sMdm_ac_rel,0x00,sizeof(sMdm_ac_rel));
		memset(&sLn_mst,0x00,sizeof(sLn_mst));
		
		pub_base_sysinit();
		
		get_zd_data("0630",cPact_no);
		get_zd_data("0680",cFive_sts);
		get_zd_data("0690",cAc_sts);
		
		iRet = Mo_bank_acc_po_Sel(g_pub_tx.reply,&sMo_bank_acc_po,"pact_no = '%s' ",cPact_no);
		if(iRet&&iRet!=100)
		{
			vtcp_log("��ѯMo_bank_acc_po�� ");	
			strcpy(g_pub_tx.reply,"B334");
			goto ErrExit;
		}
		else if(iRet == 100 )
		{
			vtcp_log("�޴˳ж�Э���� [%s]",cPact_no);	
			strcpy(g_pub_tx.reply,"B199");
			goto ErrExit;
		}
		
		if(sMo_bank_acc_po.acc_po_ind[0]=='0')
		{
			sprintf(acErrMsg,"�˳ж�Э��δ���ڴ���");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto ErrExit;	
		}
		if(strlen(sMo_bank_acc_po.over_ac_no)==0)
		{
			sprintf(acErrMsg,"�˳ж�Э��δ���ɵ��");
			strcpy(g_pub_tx.reply,"H034");
			set_zd_data("0130",acErrMsg);
			WRITEMSG
			goto ErrExit;	
		}
		
		iRet = Mdm_ac_rel_Sel(g_pub_tx.reply,&sMdm_ac_rel,"ac_no = '%s'",sMo_bank_acc_po.over_ac_no);
		if(iRet&&iRet!=100)
		{
			vtcp_log("��ѯMdm_ac_rel�� ");	
			strcpy(g_pub_tx.reply,"C117");
			goto ErrExit;
		}
		else if(iRet==100)
		{
			vtcp_log("��ѯMdm_ac_rel�޴˼�¼[%s] ",sMo_bank_acc_po.over_ac_no);	
			strcpy(g_pub_tx.reply,"C117");
			goto ErrExit;	
		}
	
		iRet = Ln_mst_Dec_Upd(g_pub_tx.reply,"ac_id = %ld ",sMdm_ac_rel.ac_id);
		if(iRet)
		{
			vtcp_log("��ѯln_mst����[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L021");
			goto ErrExit;			
		}
		
		iRet = Ln_mst_Fet_Upd(&sLn_mst,g_pub_tx.reply);
		if(iRet == 100)
		{
			vtcp_log("��ѯln_mst�޴˼�¼[%ld] ",sLn_mst.ac_id);	
			strcpy(g_pub_tx.reply,"L034");
			goto ErrExit;			
		}
		else if(iRet)
		{
			vtcp_log("��ѯln_mst����[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L021");
			goto ErrExit;	
		}
		vtcp_log("״̬%s",sLn_mst.five_sts);
		sLn_mst.five_sts[0]=cFive_sts[0];
		vtcp_log("״̬%s",sLn_mst.five_sts);
		iRet = Ln_mst_Upd_Upd(sLn_mst,g_pub_tx.reply);
		if(iRet)
		{
			vtcp_log("�޸�ln_mst����[%s] ",iRet);	
			strcpy(g_pub_tx.reply,"L073");
			goto ErrExit;		
		}
		
		iRet = Ln_mst_Clo_Upd();
		
        /* Pub_tx�ṹ��ֵ */
	strcpy(g_pub_tx.ac_no,sMo_bank_acc_po.over_ac_no);
	g_pub_tx.ac_seqn=0;
       strcpy ( g_pub_tx.add_ind , "1" );              /* ����:0��1��2���� */
        strcpy( g_pub_tx.ac_wrk_ind , "0100000" );
        strcpy ( g_pub_tx.ct_ind , "2" );               /* ��ת:1��2ת */
       /* �Ƿ��ӡ����(�������۱�־):1��0�� */
        strcpy ( g_pub_tx.prt_ind , "0" );
       /* ����ϸ�ʱ�־:1�ռ���2���յ�����3���ջ����� */
        strcpy ( g_pub_tx.hst_ind , "1" );
       g_pub_tx.svc_ind = 3060;                        /* �����弶�϶� */
       strcpy ( g_pub_tx.brf,"�жҵ���弶״̬����" );
        sprintf(acErrMsg,"Pub_tx�ṹ��ֵ PASS!");
        WRITEMSG

        sprintf(acErrMsg,"�����˻����[%d]",g_pub_tx.ac_seqn);
        WRITEMSG

       /*** ���׼��ʴ��� ***/
        if( pub_acct_trance() )
        {
                sprintf(acErrMsg,"���׼��ʴ������!");
                WRITEMSG	
		goto ErrExit;
	}


OkExit:
		strcpy(g_pub_tx.reply,"0000");
		sprintf(acErrMsg, "Before OK return: reply [%s]", g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
		return 0;
ErrExit:
		sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
		WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
		return 1;
	
}
