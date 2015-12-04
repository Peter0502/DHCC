/*************************************************
* �� �� ��:  sp9909.c
* ���������� �ɳж�Э���Ż��Գжһ�Ʊ������Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��5��14��
*
* �޸ļ�¼��
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "com_parm_c.h"
#include "com_branch_c.h"
#include "hv_uniontab_c.h"
#include "mo_bank_po_fee_c.h"

int sp9909()
{
	struct com_parm_c wd_com_parm;	/* ���гжһ�Ʊ������Ϣ�ǼǱ� */
	struct mo_bank_acc_po_c sMoBankAccPo;	/* ���гжһ�Ʊ������Ϣ�ǼǱ� */
	struct mo_bank_po_c sMoBankPo;	/* �жһ�Ʊ�Ǽǲ� */
	struct mo_sg_cash_po_c sMoSgCashPo;	/* ���ҲֵǼǲ� */
	struct com_branch_c sComBranch;
	struct hv_uniontab_c sHv_uniontab;	
	struct mo_bank_po_fee_c sMo_bank_po_fee;	
	/*add by yl �ƺ� 20090930*/
	struct dd_mst_c sDdmst;		/* �������� */
	struct mdm_ac_rel_c sMdmAcRel;	/* �˻����ʶ��ձ� */
	double s_amt;
	/*add end*/
	double          dPoAmt = 0.0;	/* ��ǩ��Ʊ���ܽ�� */
	double          dPickAmt = 0.0;	/* �ɿ�������ܽ�� */
	int             ret;
	int             parm_ind = 0;
	char            sOrno_brno[13];	
	char            cTx_code[8];/** add by chenchao 2011-8-7 15:19:19 **/
	memset(&sMoBankAccPo, 0x00, sizeof(struct mo_bank_acc_po_c));
	memset(&sMoBankPo, 0x00, sizeof(struct mo_bank_po_c));
	memset(&sMoSgCashPo, 0x00, sizeof(struct mo_sg_cash_po_c));
	memset(&wd_com_parm, 0x00, sizeof(struct com_parm_c));
	memset(&sDdmst, 0x00, sizeof(struct dd_mst_c));
	memset(&sMdmAcRel, 0x00, sizeof(struct mdm_ac_rel_c));
	memset(&sComBranch, 0x00, sizeof(struct com_branch_c));
	memset(&sHv_uniontab,0,sizeof(struct hv_uniontab_c));
	memset(cTx_code,0x00,sizeof(cTx_code));
	memset(&sMo_bank_po_fee,0,sizeof(struct mo_bank_po_fee_c));
	get_zd_data("0330", sMoBankAccPo.pact_no);	/* �ж�Э���� */
	get_zd_data("0650",cTx_code);			/** ���ֽ��� **/
	pub_base_sysinit();
	sprintf(acErrMsg, "pact_no=[%s]", sMoBankAccPo.pact_no);
	WRITEMSG

	/*Add By Yl 20091209*
	get_zd_data("0030", sComBranch.br_no);	
	vtcp_log("[%s][%d]sComBranch.br_no==[%s]",__FILE__,__LINE__,sComBranch.br_no);
	ret = Com_branch_Sel(g_pub_tx.reply, &sComBranch, "br_no='%s'", sComBranch.br_no);
	if (ret == 100) {
                sprintf(acErrMsg, "�޴˻�����!");
                WRITEMSG
                        sprintf(g_pub_tx.reply, "O116");
                goto ErrExit;
        } else if (ret) {
                sprintf(acErrMsg, "ִ��Com_branch_Sel��![%d]", ret);
                WRITEMSG
                goto ErrExit;
        }
	set_zd_data("0961", sComBranch.br_name);
	vtcp_log("[%s][%d]sComBranch.br_name==[%s]",__FILE__,__LINE__,sComBranch.br_name);
	*End By Yl 20091209*/

	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret = Mo_bank_acc_po_Sel(g_pub_tx.reply, &sMoBankAccPo, "pact_no='%s' ", sMoBankAccPo.pact_no);
	if (ret == 100) {
		sprintf(acErrMsg, "�޴˳ж�Э����!");
		WRITEMSG
			sprintf(g_pub_tx.reply, "D104");
		goto ErrExit;
	} else if (ret) {
		sprintf(acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret);
		WRITEMSG
			goto ErrExit;
	}
	/***
		if( sMoBankAccPo.acc_po_sts[0] == '2'&& sMoBankAccPo.acc_po_sts[0] == '3' )
		{
			sprintf( acErrMsg, "�˳ж�Э������ǩ��!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B188" );
			goto ErrExit;
		}else if( sMoBankAccPo.acc_po_sts[0] == 'A' )
		{
			sprintf( acErrMsg, "�˳ж�δ��¼��!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B274" );
			goto ErrExit;
		}else if( sMoBankAccPo.acc_po_sts[0] == '0' )
		{
			sprintf( acErrMsg, "�˳ж�δ�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B275" );
			goto ErrExit;
		}
	***/
	/* ���ݳж�Э���Ų�ѯ��ǩ��Ʊ�� */
	ret = Mo_bank_po_Dec_Sel(g_pub_tx.reply, "pact_no='%s' and po_sts!='3'", sMoBankAccPo.pact_no);
	if (ret) {
		sprintf(acErrMsg, "ִ��Mo_bank_po_Dec_Sel��![%d]", ret);
		WRITEMSG
		goto ErrExit;
	}
	dPoAmt = 0.00;
	while (1) {
		ret = Mo_bank_po_Fet_Sel(&sMoBankPo, g_pub_tx.reply);
		if (ret == 100) {
			break;
		} else if (ret) {
			sprintf(acErrMsg, "ִ��Mo_bank_po_Fet_Sel��![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		dPoAmt += sMoBankPo.po_amt;
	vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,sMoBankPo.sign_br_no);
	}

	Mo_bank_po_Clo_Sel();

	/*add by yl 20091003 ��֤�����
	if( (sMoBankAccPo.acc_type[0]=='1' || sMoBankAccPo.acc_type[0]=='3') &&  )
	{*��֤��ж�*
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'", sMoBankAccPo.bail_ac_no );
		if (ret)
		{
			sprintf(acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}

		ret = Dd_mst_Sel( g_pub_tx.reply, &sDdmst, "ac_id = %d ", sMdmAcRel.ac_id );
		if (ret)
		{
			sprintf(acErrMsg, "ִ��Dd_mst_Sel��![%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
		vtcp_log("[%s][%d]sDdmst.bal==[%f]",__FILE__,__LINE__,sDdmst.bal);
		s_amt = 0.00;
		ret = sql_sum_double("mo_bank_po", "po_amt", &s_amt, "pact_no='%s' and po_sts!='3'", sMoBankAccPo.pact_no);
		s_amt = sDdmst.bal - s_amt;
		vtcp_log("[%s][%d]s_amt==[%f]",__FILE__,__LINE__,s_amt);
		set_zd_double( "0430", s_amt );
	}
	add end */

	/*
	 * ����ж�����Ϊ���Ҳ� if( sMoBankAccPo.acc_type[0] == '4' ) {
	 * ���ݳж�Э���Ų��ұ��ҲֵǼǱ� * ret = Mo_sg_cash_po_Sel(
	 * g_pub_tx.reply, &sMoSgCashPo, "pact_no='%s'", sMoBankAccPo.pact_no
	 * ); if( ret == 100 ) {
	 *
	 * sprintf( acErrMsg, "�޴˳ж�Э����!" ); WRITEMSG sprintf(
	 * g_pub_tx.reply, "B187" ); goto ErrExit;
	 *
	**memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );***
	 * } else if( ret ) { sprintf( acErrMsg,
	 * "ִ��Mo_sg_cash_po_Sel��![%d]", ret ); WRITEMSG goto ErrExit; }
	 *
	 * dPickAmt = sMoBankAccPo.bail_amt - sMoSgCashPo.pickup_sum; vtcp_log(
	 * "dPickAmt[%lf]", dPickAmt ); }	*
	 */


	if (sMoBankAccPo.acc_po_sts[0] == '9')	/** ���л�Ʊ **/
		parm_ind = 6;
	else {
		switch (sMoBankAccPo.acc_type[0]) {
		case '1':	/** ���ж� **/
			parm_ind = 1;
			break;
		case '2':	/** ��ѹ�浥 **/
			parm_ind = 2;
			break;
		case '3':	/** 100%��֤�� **/
			parm_ind = 3;
			break;
		case '4':	/** �����ж� **/
			parm_ind = 4;
			break;
		case '5':	/** ��Ʊ��СƱ **/
			parm_ind = 5;
			break;
		default:
			sprintf(acErrMsg, "[%s][%d]��Ʊ���ʹ���![%s]", __FILE__, __LINE__, sMoBankAccPo.acc_type);
			WRITEMSG
			goto ErrExit;

		}
	}
	/** �Ӳ�����com_parmȡ�ò�ͬ���ͳжһ�Ʊ�Ŀ�Ŀ **/
	ret = Com_parm_Sel(g_pub_tx.reply, &wd_com_parm, "parm_code='CDHPR' and parm_seqn=%d", parm_ind);
	if (ret) {
		sprintf(acErrMsg, "[%s][%d]��ѯ���������![%d]", __FILE__, __LINE__, ret);
		WRITEMSG
		goto ErrExit;
	}
	memset(sOrno_brno,0,sizeof(sOrno_brno));
	vtcp_log("%s,%d g_pub_tx.tx_br_no=[%s]\n",__FILE__,__LINE__,g_pub_tx.tx_br_no);
	vtcp_log("%s,%d sOrno_brno=[%s]\n",__FILE__,__LINE__,sOrno_brno);
	GetOr_br_noByBr_no(g_pub_tx.tx_br_no,sOrno_brno);
	ret = Hv_uniontab_Sel(g_pub_tx.reply,&sHv_uniontab,"or_br_no='%s'",sOrno_brno);
	if(ret)
	{
		vtcp_log("%s,%d,��ѯ�����кŴ���!",__FILE__,__LINE__);
		goto ErrExit;
	}
	/** add by chenchao 20110728 ���ӱ�ŵ�ȡֵ���Ժ������ѽ�� begin **/
	if(memcmp(cTx_code,"4333",4) == 0)
	{
		ret = Mo_bank_po_fee_Dec_Sel(g_pub_tx.reply,"pact_no='%s' and sts='X' order by num ",sMoBankAccPo.pact_no);
		if(ret)
		{
			sprintf( acErrMsg, "�����α����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D101" );
			goto ErrExit;
		}
		ret = Mo_bank_po_fee_Fet_Sel(&sMo_bank_po_fee,g_pub_tx.reply);
		if(ret == 100)
		{
			sprintf( acErrMsg, "��Э���Ŵ�δ�����!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "B275" );
			goto ErrExit;
		}else if(ret)
		{
			sprintf( acErrMsg, "��ѯ�ж��շѳ���!" );
			WRITEMSG
			strcpy( g_pub_tx.reply, "D102" );
			goto ErrExit;
		}
		Mo_bank_po_fee_Clo_Sel();
		set_zd_double("1004",sMo_bank_po_fee.fee_amt); /** �շѽ�� */
		set_zd_long("0500",sMo_bank_po_fee.num); /** ������ **/
		set_zd_double("1131",sMo_bank_po_fee.prm_amt); /** ��ŵ��� **/
		set_zd_double("0430",sMo_bank_po_fee.po_amt); /** Ʊ���� **/
	}
	/** end add by chenchao  20110728 �շ� ***/
	vtcp_log("%s,%d sOrno_brno=[%s]\n",__FILE__,__LINE__,sOrno_brno);
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data("0640", wd_com_parm.val);	/* �жһ�Ʊ���ʿ�Ŀ */
	set_zd_data("0260", sMoBankAccPo.payer_name);	/* ������ȫ�� */
	set_zd_data("0380", sMoBankAccPo.payer_ac_no);	/* �������ʺ� */
	set_zd_data("0300", sMoBankAccPo.payer_br_no);	/* �����˿������к� */
	set_zd_data("0250", sMoBankAccPo.payer_br_name);	/* �����˿��������� */
	set_zd_data("0270", sMoBankAccPo.payee_name);	/* �տ���ȫ�� */
	set_zd_data("0670", sMoBankAccPo.acc_type);	/* �ж����� */
	set_zd_data("0830", sMoBankAccPo.payee_ac_no);	/* �տ����˺� */
	set_zd_double("0840", sMoBankAccPo.over_rate);   /*��Ϣ����*/
	set_zd_data("0810", sMoBankAccPo.payee_br_name);	/* �տ��˿��������� */
	set_zd_double("1001", sMoBankAccPo.sign_amt);	/* ǩ��Ʊ���� */
	set_zd_long("0440", sMoBankAccPo.matr_date);	/* �������� */
	set_zd_long("0460", sMoBankAccPo.req_date);	/* �������� */
	set_zd_double("1003", sMoBankAccPo.bail_amt);	/* ��֤���� */
	set_zd_double("1002", sMoBankAccPo.tot_mort_amt);	/* ��Ѻ�ܽ�� */
	set_zd_data("0320", sMoBankAccPo.bail_ac_no);	/* ��֤���ʺ� */
	set_zd_data("0700", sMoBankAccPo.acc_po_sts);	/* Э��״̬ */
	set_zd_data("0710", sMoBankAccPo.acc_po_ind);	/* ���ڴ����־ */
	set_zd_double("0400", sMoBankAccPo.bail_amt_ratio);	/* ��֤����� */
	set_zd_double("0410", dPoAmt);	/* ��ǩ��Ʊ���ܽ�� */
	set_zd_double("0390", sMoSgCashPo.pickup_sum);	/* �ѿ�������ܽ�� */
	set_zd_double("0420", dPickAmt);	/* �ɿ�������ܽ�� */
	/*Mod By Yl 20091207 �ƺ���������������,��ǩ��ȴ��Ӫҵ��,���Բ�����
	����������ΪǩƱ����*/
	/**set_zd_data("0910", sMoBankAccPo.tx_br_no);	 ��Ʊ�к� */
	set_zd_data("0910", sMoBankPo.sign_br_no);	 /**��Ʊ�к� */
	vtcp_log("[%s][%d][%s]",__FILE__,__LINE__,sMoBankPo.sign_br_no);
	set_zd_data("0961",sHv_uniontab.addr);
	set_zd_data("0962",sOrno_brno);

OkExit:
	strcpy(g_pub_tx.reply, "0000");
	sprintf(acErrMsg, "Before OK return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg, "Before return: reply is[%s]", g_pub_tx.reply);
	WRITEMSG
		set_zd_data(DC_REPLY, g_pub_tx.reply);
	return 1;
}
