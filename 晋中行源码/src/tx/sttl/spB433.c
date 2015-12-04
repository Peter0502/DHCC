/*************************************************
* �� �� ��:  spB433.c
* ���������� ��ɵǼ�ί���տ��嵥�������ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��2��2��
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
#include "mo_trust_list_c.h"

int spB433()
{
	struct	mo_trust_pay_c	sMoTrustPay;	/* ί���տ�ǼǱ� */
	struct	mo_trust_list_c	sMoTrustList;	/* ί���嵥 */
	double	dAmt;
	int	ret;

	memset( &sMoTrustPay, 0x00, sizeof( struct mo_trust_pay_c ) );
	memset( &sMoTrustList, 0x00, sizeof( struct mo_trust_list_c ) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0580", sMoTrustList.turst_pay_no );	/* ί���տ�Э���� */
	get_zd_data( "0300", sMoTrustList.payer_ac_no );	/* �������˺� */
	get_zd_double( "0390", &dAmt );				/* ��� */

	/* ����ί���տ�Э���Ų�ί���տ�ǼǱ�����鱾���׻����Ƿ�Ϊ¼����� */
	ret = Mo_trust_pay_Sel( g_pub_tx.reply, &sMoTrustPay, "turst_pay_no='%s' and br_no='%s'", \
				sMoTrustList.turst_pay_no, g_pub_tx.tx_br_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "ί���տ�ǼǱ����޴�Э������Ϣ!turst_pay_no=[%s]",\
			sMoTrustPay.turst_pay_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B100" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_trust_pay_Sel��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ���ί���տ�Э���Ƿ����,���׽���Ƿ�������ݿ��еĽ�� */
	ret = Mo_trust_list_Dec_Upd( g_pub_tx.reply, "turst_pay_no='%s' and payer_ac_no='%s'", \
					sMoTrustList.turst_pay_no, sMoTrustList.payer_ac_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_trust_list_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_trust_list_Fet_Upd( &sMoTrustList, g_pub_tx.reply );
	if ( ret == 100 )
	{
		sprintf( acErrMsg, "��ѯί���嵥���޼�¼!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B104" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_trust_list_Fet_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	if( pub_base_CompDblVal( sMoTrustList.amt, dAmt ) != 0 )
	{
		sprintf( acErrMsg, "���׽����ί���嵥�н���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B102" );
		goto ErrExit;
	}
	
	/* �޸ĸñ��嵥״̬Ϊ���� */
	strcpy( sMoTrustList.sts, "2" );			/* ״̬��1�Ǽ� 2ע��*/

	ret = Mo_trust_list_Upd_Upd( sMoTrustList, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸�ί���嵥��[%d]", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B101" );
		goto ErrExit;	
	}

	Mo_trust_list_Clo_Upd();

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
