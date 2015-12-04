/*************************************************
* �� �� ��:  spB343.c
* ���������� ���������ڹ���жҲ�����͵�ԭʼƱ�� 
*	    	 ��Ϣ���Ǽǳж�Э��ͱ�֤���˻��� 
*
* ��    ��:  jane
* ������ڣ� 2003��8��24��
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
#include "mo_bank_split_po_c.h"

int spB343()
{
	struct	mo_bank_acc_po_c 	sMoBankAccPo;	/* �жһ�Ʊ������Ϣ�Ǽǲ� */
	struct	mo_bank_split_po_c 	sMoBankSplitPo;	/* �жҲ��ԭʼƱ�ݵǼǲ� */
	struct	mdm_ac_rel_c 		sMdmAcRel;		/* �����˺Ŷ��ձ� */
	struct	dd_mst_c 			sDdMst;			/* �������ļ� */
	char	cBailAcNo[25];						/* ��֤���ʺ� */
	int		ret;

	memset( &sMoBankAccPo, 0x00, sizeof( struct mo_bank_acc_po_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );
	memset( cBailAcNo, 0x00, sizeof( cBailAcNo ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	strcpy( sMoBankSplitPo.tx_br_no, g_pub_tx.tx_br_no );	/* ���׻��� */
	sMoBankSplitPo.req_date = g_pub_tx.tx_date;				/* �������� */
	get_zd_data( "0620", sMoBankSplitPo.pact_no );			/* ��Ѻ��� */
	get_zd_data( "0830", cBailAcNo );						/* ��֤���ʺ� */
	get_zd_double( "1002", &sMoBankSplitPo.par_amt );		/* ��Ʊ��� */
	get_zd_long( "0450", &sMoBankSplitPo.matr_date );		/* ��Ʊ������ */
	get_zd_data( "0310", sMoBankSplitPo.payer_ac_no );		/* ��Ʊ���ʺ� */
	get_zd_data( "0260", sMoBankSplitPo.payer_name );		/* ��Ʊ��ȫ�� */
	get_zd_data( "0810", sMoBankSplitPo.payer_br_name );	/* ������ȫ�� */
	get_zd_data( "0320", sMoBankSplitPo.payee_ac_no );		/* �տ����ʺ� */
	get_zd_data( "0270", sMoBankSplitPo.payee_name );		/* �տ���ȫ�� */
	get_zd_data( "0820", sMoBankSplitPo.payee_br_name );	/* �տ��˿�����ȫ�� */
	get_zd_data( "0600", sMoBankSplitPo.pnote_no );			/* Ʊ�ݺ� */

	/* ���жҲ��ԭʼƱ�ݵǼǲ������޴�Э���� */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pact_no='%s'", \
						sMoBankSplitPo.pact_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "����Ѻ����Ѵ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B307" );
		goto ErrExit;
	}else if( ret !=0 && ret != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ���жҲ��ԭʼƱ�ݵǼǲ������޴˳жһ�Ʊ��¼ */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pnote_no='%s'", \
						sMoBankSplitPo.pnote_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "�˳жһ�Ʊ������ֵǼ�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B294" );
		goto ErrExit;
	}else if( ret !=0 && ret != 100 )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ��鱣֤���ʺŵ���Ч��:�����˺š��˻�״̬���� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
						cBailAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "��֤���˺Ų�����![%s]", cBailAcNo );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B195" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d",\
				sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "����ı�֤���˺ŷǻ����˺�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B196" );
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "ִ��Dd_mst_Sel����![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( strcmp( sDdMst.prdt_no, "131" ) !=0 )
	{
		sprintf( acErrMsg, "������˺ŷǳжұ�֤���˺�!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B202" );
		goto ErrExit;
	}

	if( sDdMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "�˱�֤���˺�״̬������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B197" );
		goto ErrExit;
	}

	/* �ǼǳжҲ��ԭʼƱ�ݵǼǲ� */
	strcpy( sMoBankSplitPo.bail_ac_no, cBailAcNo );		/* ��֤���ʺ� */
	strcpy( sMoBankSplitPo.sts, "1" );       		/* ״̬��1 ¼�� */
	ret = Mo_bank_split_po_Ins( sMoBankSplitPo, g_pub_tx.reply );
	if ( ret )
   	{
       	sprintf( acErrMsg, "ִ��Mo_bank_split_po_Ins����!ret=[%d]", ret );
       	WRITEMSG
       	goto ErrExit;
   	}

	/* �Ǽǻ�Ʊ�ǼǱ� */
	strcpy( sMoBankAccPo.bail_ac_no, cBailAcNo );			/* ��֤���ʺ� */

	ret = pub_sttl_RegMoBankAccPo( sMoBankAccPo );
	if ( ret )
   	{
   		sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankAccPo����!ret=[%d]", ret );
   		WRITEMSG
   		goto ErrExit;
   	}

	/* Ϊ����ˮ��׼�� */
	strcpy( g_pub_tx.brf, "�жһ�Ʊ���Ʊ�ݹ���" );	/* ��ע */

	/* ����ˮ��־ */
	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoBankSplitPo.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy(g_pub_tx.add_ind,"");
	strcpy(g_pub_tx.note_type,"111");	/*** �жһ�Ʊ���� ***/
	strcpy( g_pub_tx.beg_note_no, cBailAcNo );	/*** ��֤���ʺų����� ***/
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
		WRITEMSG
		goto ErrExit;
	}
	set_zd_data( "0820", sDdMst.name );    /* Ϊ��ӡ������ */

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
