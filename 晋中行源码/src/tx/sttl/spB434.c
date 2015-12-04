/*************************************************
* �� �� ��:  spB434.c
* ���������� ������ճи��Ǽǹ��ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��2��3��
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
#include "mo_trust_pay_c.h"

int spB434()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* ί���տ�ǼǱ� */
	char	cOpenBrno[9];			/* �տ��˺ſ������� */
	char	cPayeeAcNo[20];			/* �տ��˺� */
	char	cOperCode[6];			/* ҵ����� */
	int	ret;
	
	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( cOpenBrno, 0x00, sizeof( cOpenBrno ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
	memset( cOperCode, 0x00, sizeof( cOperCode ) );
	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	strcpy( sMoTrustPay.br_no, g_pub_tx.tx_br_no );		/* �������� */
	get_zd_data( "0580", sMoTrustPay.turst_pay_no );	/* ί���տ�Э���� */
	strcpy( sMoTrustPay.turst_fag, "2" );			/* ί�б�־��1ί���տ� 2���ճи� */
	get_zd_data( "0250", sMoTrustPay.requ_name );		/* ������ȫ�� */
	get_zd_data( "0670", sMoTrustPay.requ_id_type );	/* ί����֤������ */
	get_zd_data( "0620", sMoTrustPay.requ_id_no );		/* ������֤������ */
	get_zd_data( "0300", cPayeeAcNo );			/* �տ����˺� */
	get_zd_long( "0340", &sMoTrustPay.payee_ac_seqn );	/* �տ����˺�ID��� */
	strcpy( sMoTrustPay.sts, "1" );				/* ״̬��1�Ǽ� 2ע��*/
	strcpy( cOperCode, "10006" );
	
	/* ��齻�׻����Ƿ����Ȩ�� */
	ret = pub_sttl_ChkInputBrno( cOperCode, g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "��齻�׻���Ȩ�޴�" );
		WRITEMSG
		goto ErrExit;
	}
	
	/* ����տ��ʺ���Ч�ԣ��������ʺ�id */
	ret = pub_base_ChkAc( cPayeeAcNo, sMoTrustPay.payee_ac_seqn, \
				cOpenBrno, &sMoTrustPay.payee_ac_id );
	if ( ret )
	{
		sprintf( acErrMsg, "����տ��ʺ���Ч�Դ�" );
		WRITEMSG
		goto ErrExit;	
	}
	
	/* �Ǽ�ί���տ�Э��ǼǱ���״̬Ϊ¼�� */
	ret = pub_sttl_RegMoTrustPay( sMoTrustPay );
	if ( ret )
    	{
        	sprintf( acErrMsg, "ִ��pub_sttl_RegMoTrustPay����!ret=[%d]", ret );
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
