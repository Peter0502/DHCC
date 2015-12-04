/*************************************************
* �� �� ��:  sp9869.c
* ���������� ��ת����Э���Ż������ֵǼǱ���Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��3��11��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int sp9869()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	
	get_zd_data( "0630", sMoDiscnt.tpact_no );	/* ת����Э���� */

	/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�ת����Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B133" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */
    	set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
	set_zd_double( "0840", sMoDiscnt.tpn_rate );	/* ת������ */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* ��Ʊ������ */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* ��Ʊ���ʺ� */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* ��Ʊ��ȫ�� */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* ������ȫ�� */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* �տ����ʺ� */
	set_zd_data( "0270", sMoDiscnt.payee );		/* �տ���ȫ�� */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* �տ��˿�����ȫ�� */
	set_zd_data( "0900", sMoDiscnt.note_type );	/* Ʊ������ */
	set_zd_data( "0600", sMoDiscnt.pnote_no );	/* Ʊ�ݺ� */
	set_zd_data( "0370", sMoDiscnt.tin_ac_no );	/* ת�����˻� */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_long( "0480", sMoDiscnt.way_days );	/* ��;���� */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* ת������Ϣ */
	set_zd_data( "0670", sMoDiscnt.tpay_ac_type );	/* �����˻����� */
	set_zd_long( "0440", sMoDiscnt.tpn_date );	/* ת�������� */
	set_zd_long( "0460", sMoDiscnt.return_date );	/* �ع����� */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* Ʊ�ݺ� */
	pub_base_strpack( sMoDiscnt.tpn_name );
	if( strlen( sMoDiscnt.tpn_name ) == 0)
		set_zd_data( "0250", "��ɳ����ҵ����" );

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




