/*************************************************
* �� �� ��:  spB431.c
* ���������� ��ɵǼ�ί���տ�Э�鱡���ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��6��15��
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

int spB431()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* ί���տ�ǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	struct	ln_mst_c	sLnMst;		/* ���ڴ�����ļ� */
	char	cPayeeAcNo[20];			/* �տ��˺� */
	int	ret;
	
TRACE	
	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	memset( cPayeeAcNo, 0x00, sizeof( cPayeeAcNo ) );
TRACE	
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();
	
	/* ��ǰ̨��Ļȡֵ */
	strcpy( sMoTrustPay.br_no, g_pub_tx.tx_br_no );		/* �������� */
	get_zd_data( "0580", sMoTrustPay.turst_pay_no );	/* ί���տ�Э���� */
	strcpy( sMoTrustPay.turst_fag, "1" );			/* ί�б�־��1ί���տ� 2���ճи� */
	get_zd_data( "0260", sMoTrustPay.requ_name );		/* ������ȫ�� */
	get_zd_data( "0670", sMoTrustPay.requ_id_type );	/* ί����֤������ */
	get_zd_data( "0620", sMoTrustPay.requ_id_no );		/* ������֤������ */
	get_zd_data( "0370", cPayeeAcNo );			/* �տ����˺� */
	get_zd_long( "0340", &sMoTrustPay.payee_ac_seqn );	/* �տ����˺�ID��� */
	strcpy( sMoTrustPay.sts, "1" );				/* ״̬��1�Ǽ� 2ע��*/

	/* ���ί���տ�ǼǱ����Ƿ��Ѿ����ڴ�Э���� */
	ret = Mo_trust_pay_Sel( g_pub_tx.reply, &sMoTrustPay, "turst_pay_no='%s'",\
			 	sMoTrustPay.turst_pay_no );
	if( ret == 0 )
	{
		sprintf( acErrMsg, "ί���տ�ǼǱ����Ѿ����ڴ�Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B246" );
                goto ErrExit;
       	}else if( ret != 100 )
       	{
       		sprintf( acErrMsg, "ִ��Mo_trust_pay_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ����տ��ʺ���Ч�ԣ������˺š��˻�״̬�������� *
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				cPayeeAcNo );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�����տ��ʺŲ�����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B243" );
                goto ErrExit;
	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}
TRACE
	* ���ݿͻ��ʺ�ID���ʻ���Ų�ѯ�������ļ� *
	ret =Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d",\
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���տ��ʺŷǴ����ʺ�![%s]", cPayeeAcNo );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "B244" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Ln_mst_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}
TRACE

	if( sLnMst.ac_sts[0] != '1' )
	{
		sprintf( acErrMsg, "���տ��ʺ�״̬������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B245" );
		goto ErrExit;
	}
TRACE
*/
	/* �Ǽ�ί���տ�Э��ǼǱ� */
	sMoTrustPay.payee_ac_id= sMdmAcRel.ac_id ;	/* �տ����˺�ID */

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
