/*************************************************
* �� �� ��:  spB330.c
* ���������� ������ҵ�����еĳжһ�Ʊ����Ѻ,�����еĳжһ�Ʊ�ϵ����ݽ��еǼ� ��Ӧǰ̨5621
*
* ��    ��:  liuxuan
* ������ڣ� 2006��10��7��
*
* �޸ļ�¼�� 
*   ��   ��: 
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_bank_acc_po_c.h"
#include "mo_bank_po_c.h"
#include "mo_sg_cash_po_c.h"
#include "impa_dep_reg_c.h"
#include "mo_dep_reg_c.h"
#include "mo_bank_split_po_c.h"
#include "mo_po_co_c.h"
#include "mo_bail_rate_c.h"

int spB330()
{
	struct	mo_bank_acc_po_c 	sMoBankAcc,sMoBankAcc1;		/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_bank_po_c 	sMoBankPo,sMoBankPo_c;	/* �жһ�Ʊ�Ǽǲ� */
	struct	mo_sg_cash_po_c 	sMoSgCashPo;	/* ���ҲֵǼǱ� */
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* �жҲ��ԭʼƱ�ݵǼǲ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 			sDdMst;			/* �������ļ� */
	struct	impa_dep_reg_c 		sImpaDepReg;	/* ��Ѻ�浥�Ǽǲ� */
	struct	mo_dep_reg_c 		sMoDepReg;	/* ��Ʊ��Ѻ�浥�Ǽǲ� */
	struct  mo_po_co_c  sMoPoCo; 	/* �ж�Э�������ͬ�Ŷ��ձ� */
	struct  mo_bail_rate_c		mo_bail_rate; 	/* ��֤�����ʵǼǱ� */
	char    cLoadName[50];       		   		/* �ϴ��ļ��� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	double	dTmpAmt;							/* ��֤���� */
	double	dTallAmt;							/* �жһ�Ʊ����޶� */
	double	dSumMortAmt;						/* ��Ѻ�浥�Ǽǲ�����Ѻ��Ŷ�Ӧ��� */
	double	dSumMortAmt1;						/* �жһ�����Ϣ�Ǽǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	double	dSumMortAmt2;						/* �жҵǼǲ�����Ѻ��Ŷ�Ӧ���֮�� */
	int		ret, ret1, flag, num;
	char	Tmp_flag[2];

	memset( &sMoBankAcc, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankAcc1, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoBankPo_c, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( &sMoSgCashPo, 0x00, sizeof( struct mo_sg_cash_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( &sImpaDepReg, 0x00, sizeof( struct impa_dep_reg_c ) );
	memset( &sMoDepReg, 0x00, sizeof( struct mo_dep_reg_c ) );
	memset( cLoadName, 0x00, sizeof( cLoadName ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0330", sMoBankAcc.pact_no );	/*  �жһ�Ʊ��Ҳ��ΪЭ���� */
	vtcp_log("Э�����[%s]\n",sMoBankAcc.pact_no );

	pub_base_strpack( sMoBankAcc.pact_no );

	/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
	ret1 = Mo_bank_acc_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s'", \
						sMoBankAcc.pact_no );
	if ( ret1 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Dec_Upd��[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}

	ret1 = Mo_bank_acc_po_Fet_Upd( &sMoBankAcc, g_pub_tx.reply );
	vtcp_log("fet�Ľ��ret1�� [%d]\n",ret1);
	if( ret1 && ret1 != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_acc_po_Sel��![%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}
		/* ��ǰ̨��Ļȡֵ */
		strcpy( sMoBankAcc.tx_br_no, g_pub_tx.tx_br_no );	/* ���׻��� */
		get_zd_long( "0450", &sMoBankAcc.req_date);			/* �������� ��������Ϊ��Ϣ�� */
		get_zd_data( "0260", sMoBankAcc.payer_name );		/* ������ȫ�� */
		get_zd_data( "0250", sMoBankAcc.payer_br_name );	/* �����˿��������� */
		get_zd_data( "0270", sMoBankAcc.payee_name );		/* �տ���ȫ�� */
		get_zd_data( "0830", sMoBankAcc.payee_ac_no );		/* �տ����˺� */
		get_zd_data( "0810", sMoBankAcc.payee_br_name );	/* �տ��˿��������� */
		get_zd_double( "1001", &sMoBankAcc.sign_amt );		/* ǩ��Ʊ���� */
		vtcp_log("Ʊ����[%.0f]\n",sMoBankAcc.sign_amt);
		get_zd_long( "0440", &sMoBankAcc.matr_date );		/* �������� */
		get_zd_double( "1002", &sMoBankAcc.tot_mort_amt );	/* ��Ѻ��� */
		get_zd_double( "0400", &sMoBankAcc.bail_amt_ratio );/* ��֤����� */
		strcpy( sMoBankAcc.acc_po_ind, "0" );				/* ��Ʊ״̬: 0δ���ڴ���*/
		get_zd_data( "0380", sMoBankAcc.payer_ac_no );		/* �������ʺ� */
		get_zd_double( "1003", &dTmpAmt );					/* ��֤���� */
		get_zd_data( "0670", sMoBankAcc.acc_type );			/* �ж����� */
		get_zd_data( "0320", cBailAcNo );					/* �ڱ���������Ϊ������ҵ�ʺ� */
		strcpy( sMoBankAcc.acc_po_sts, "9" );				/* ������ʾ���й�������ѺƱ */

		if( ret1 == 100 )
		{
			vtcp_log("ִ�е���insert\n");
			ret = Mo_bank_acc_po_Ins( sMoBankAcc, g_pub_tx.reply );
			if( ret ){
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}
		else{
			vtcp_log("ִ�е���update\n");
			/* �޸ĳжһ�Ʊ������Ϣ�Ǽǲ� */
			ret = Mo_bank_acc_po_Upd_Upd( sMoBankAcc, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ������Ϣ�ǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
		}

	Mo_bank_acc_po_Clo_Upd();

	strcpy( sMoBankPo.sign_br_no, sMoBankAcc.payer_br_no );  /* ǩ�����к� */
	strcpy( sMoBankPo.pact_no, sMoBankAcc.pact_no);        /* Э���� */
	memcpy( sMoBankPo.po_no, sMoBankAcc.pact_no,sizeof(sMoBankPo.po_no)-1);        /* �жһ�Ʊ��*/
	sMoBankPo.sign_date = sMoBankAcc.req_date;             /* ǩ������ */
	sMoBankPo.receive_date = sMoBankAcc.req_date;             /* ǩ������ */
	sMoBankPo.po_amt = sMoBankAcc.sign_amt;             /* Ʊ���� */
	strcpy( sMoBankPo.at_term_ind, "N" );               /* Ʊ�ݵ��ڱ�־:Y ���ڣ�N δ���� */
	strcpy( sMoBankPo.po_sts, "9" );                    /* �жһ�Ʊ״̬:9 ���е�Ʊ */
	strcpy( sMoBankPo.po_ind, "1" );                    /* �жһ�Ʊ��־:1 ���� */

	/* ���ݳжһ�Ʊ����������гжһ�Ʊ�ǼǱ� */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo_c, "po_no='%s'", sMoBankPo.po_no);
	if(ret && ret!=100){
		sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��[%d]", ret1 );
		WRITEMSG
		goto ErrExit;
	}
	if( ret == 0 )
	{
		vtcp_log( "[%s][%d]�жһ�Ʊ�����Ѵ���,�����´���");
		/* ���ݳж�Э���Ų������гжһ�Ʊ������Ϣ�ǼǱ� */
		ret1 = Mo_bank_po_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and po_no='%s'", \
								sMoBankPo.pact_no ,sMoBankPo.pact_no);
			if ( ret1 )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_po_Dec_Upd��[%d]", ret1 );
				WRITEMSG
				goto ErrExit;
			}
			ret1 = Mo_bank_po_Fet_Upd( &sMoBankPo_c, g_pub_tx.reply );
			vtcp_log("fet�Ľ��ret1�� [%d]\n",ret1);
			if( ret1)
			{
				sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret1 );
				WRITEMSG
				goto ErrExit;
			}
			strcpy( sMoBankPo.rowid, sMoBankPo_c.rowid );	/* �����渳ֵ���ĸ��� */
			ret = Mo_bank_po_Upd_Upd( sMoBankPo, g_pub_tx.reply );
			if ( ret )
			{
				sprintf( acErrMsg, "�޸ĳжһ�Ʊ�ǼǱ���[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
			Mo_bank_po_Clo_Upd();
		}
	else{
			vtcp_log("�޴˳жһ�Ʊ[%s],���в������\n",sMoBankPo.pact_no);
			ret = Mo_bank_po_Ins( sMoBankPo, g_pub_tx.reply );
			if( ret )
			{
				sprintf( acErrMsg, "ִ��Mo_bank_po_Ins��[%d]", ret );
				WRITEMSG
				goto ErrExit;
			}
	}

	/* Ϊ����ˮ��ֵ */
	strcpy( g_pub_tx.brf, "��Ѻ�жһ�Ʊ��Ϣ¼��" );	/* ��ע */

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankAcc.pact_no);
	g_pub_tx.tx_amt1=0.00;
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}

	/*�Ǽ�Э����-��ͬ�Ŷ��յǼǲ� ΪMIS׼������*/
	memset( &sMoPoCo, 0x00, sizeof( struct mo_po_co_c ) );
	strcpy( sMoPoCo.pact_no, sMoBankAcc.pact_no );
	strcpy( sMoPoCo.contract_no, sMoBankAcc.pact_no );
	g_reply_int = Mo_po_co_Ins( sMoPoCo,g_pub_tx.reply );
	if( g_reply_int )
	{
		sprintf( acErrMsg, "�Ǽǳж�Э����-��ͬ�Ŷ��ձ����" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
