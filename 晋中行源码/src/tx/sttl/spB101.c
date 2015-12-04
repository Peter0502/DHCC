/*************************************************
* �� �� ��:  spB101.c
* ���������� ������л�Ʊ��Ϣ¼��Ĺ��ܡ�
*	     �����˺��տ���Ϊ��λ�ģ�����ǩ���ֽ����л�Ʊ
*	    ��ϵͳ�Զ��жϣ��������Ϊ�Ǹ��˿ͻ���������ת
*	     �ʻ�Ʊ���ֽ��Ʊ����������������кźʹ���
*	     ������������ת�˲����룻�ֽ��Ʊ���Բ������տ�
*	     ���ʺš�
*
* ��    ��:  jane
* ������ڣ� 2003��1��17��
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
#include "mo_bank_po_c.h"

int spB101()
{
	struct	mo_bank_po_c	sMoBankPo;		/* ���л�Ʊ�Ǽǲ� */
	char	cRequAcno[25];				/* �������ʺ� */
	char	cif_type[2];				/* �ͻ����� */
	char	cOpenBrno[9];				/* �������ʺſ������� */
	char	cOperCode[6];				/* ҵ����� */
	double	dAcBal;					/* �ʻ���� */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );
	memset( cRequAcno, 0x00, sizeof( cRequAcno ) );
	memset( cif_type, 0x00, sizeof( cif_type ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoBankPo.po_requ_no );		/* �������� */
	get_zd_data( "0590", sMoBankPo.po_no );			/* ��Ʊ��� */
	get_zd_data( "0670", sMoBankPo.ct_ind );		/* ��ת��־ */
	get_zd_data( "0250", sMoBankPo.requ_name );		/* ���������� */
	get_zd_data( "0300", cRequAcno );	
	get_zd_long( "0340", &sMoBankPo.requ_ac_seqn );		/* �������ʺ���� */
	get_zd_data( "0810", sMoBankPo.payee_name );		/* �տ������� */
	get_zd_data( "0310", sMoBankPo.payee_ac_no );		/* �տ����˺� */
	get_zd_data( "0870", sMoBankPo.payer_br_no );		/* ���������к� */
	get_zd_data( "0270", sMoBankPo.payer_br_name );		/* ������������ */
	get_zd_double( "0390", &sMoBankPo.sign_amt );		/* ǩ����� */
	get_zd_data( "0820", sMoBankPo.purpose );		/* ��; */
	get_zd_data( "0830", sMoBankPo.rmk );			/* ��ע */
	get_zd_data( "0680", sMoBankPo.attorn_ind );		/* ת�ñ�־ */

	strcpy( cOperCode, "10001" );

	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻���Ȩ�޴�" );
		WRITEMSG
		goto ErrExit;
	}

	/* �����������ʺź��ʺ����ȡ�������˺�ID */
	ret = pub_base_ChkAc( cRequAcno, sMoBankPo.requ_ac_seqn, cOpenBrno, &sMoBankPo.requ_ac_id );
	if( ret )
	{
		sprintf( acErrMsg, "�����������ʺź��ʺ����ȡ�������˺�ID��" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* �����������ʺ�ȡ�ͻ����� */
	ret = pub_cif_AcToCifType ( cRequAcno, cif_type );
	if( ret )
	{
		sprintf( acErrMsg, "�����������ʺ�ȡ�ͻ����ʹ�" );
		WRITEMSG
		goto ErrExit;
	}

	/* ����ǷǸ����ʻ�������ת�� */
	if( cif_type[0]!='3' && cif_type[0]!='4' )
	{
		if( sMoBankPo.ct_ind[0] == '1' )
		{
			sprintf( acErrMsg, "�Ǹ��������ˣ��������ֽ��Ʊ" );
			WRITEMSG
			sprintf( g_pub_tx.reply, "B013" );
			goto ErrExit;
		}
	}
	
	/* ����Ʊ����Ƿ�С���ʻ���� */
	ret = pub_base_ChkAcBal( cRequAcno, sMoBankPo.requ_ac_seqn, &dAcBal );
	if( ret )	
	{
		sprintf( acErrMsg, "����ʻ�����" );
		WRITEMSG
		goto ErrExit;
	}
	
	if( pub_base_CompDblVal( sMoBankPo.sign_amt , dAcBal ) == 1 )
	{
		sprintf( acErrMsg, "��Ʊ�������ʻ����,���ܽ��л�Ʊ����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B001" );
		goto ErrExit;
	}

	/* Ϊ���л�Ʊ�ṹ��ֵ */
	strcpy( sMoBankPo.po_sts, "0" );			/* ״̬Ϊ¼�� */
	strcpy( sMoBankPo.po_ind, "1" );			/* ��־Ϊ���� */
	sMoBankPo.requ_date = g_pub_tx.tx_date;			/* ��Ʊ�������� */
	strcpy( sMoBankPo.tx_br_no, g_pub_tx.tx_br_no );	/* ��Ʊ���к� */
	
	/* �Ǽǻ�Ʊ�ǼǱ� */
	ret = pub_sttl_RegMoBankPo( sMoBankPo );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoBankPo����!ret=[%d]", ret );
        	WRITEMSG
        	goto ErrExit;
    	}
		
	/* ����ˮ��־ */
	ret = pub_ins_trace_log();
	if ( ret )
	{
		sprintf( acErrMsg, "����ˮ��־��" );
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
