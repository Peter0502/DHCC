/*************************************************
* �� �� ��:  sp9868.c
* ���������� ��Ʊ�����ͺ�Ʊ�ݺŻ���������Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��3��7��
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

int sp9868()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );

	get_zd_data( "0900", sMoDiscnt.note_type );	/* Ʊ������ */
	get_zd_data( "0600", sMoDiscnt.pnote_no );	/* Ʊ�ݺ� */

	/* ������ֵǼǱ����Ƿ���ڴ�Ʊ�� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' \
			and pnote_no='%s' and (pn_type='1' or pn_type='2')",\
			sMoDiscnt.note_type, sMoDiscnt.pnote_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�Ʊ����Ϣ!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B130" );
                goto ErrExit;
   	}
	else if( ret )
    {
    	sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
        WRITEMSG
        goto ErrExit;
    }

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */
    set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
	set_zd_long( "0450", sMoDiscnt.not_matr_date );/* ��Ʊ������ */
	set_zd_data( "0310", sMoDiscnt.rem_ac_no );	/* ��Ʊ���ʺ� */
	set_zd_data( "0260", sMoDiscnt.remitter );	/* ��Ʊ��ȫ�� */
	set_zd_data( "0810", sMoDiscnt.payer_brno_name );/* ������ȫ�� */
	set_zd_data( "0320", sMoDiscnt.payee_ac_no );	/* �տ����ʺ� */
	set_zd_data( "0270", sMoDiscnt.payee );		/* �տ���ȫ�� */
	set_zd_data( "0820", sMoDiscnt.payee_brno_name );/* �տ��˿�����ȫ�� */
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_double( "0850", sMoDiscnt.tpn_rate );	/* ת�������� */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* ת������Ϣ */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* ���������� */
	/*20101217 yanqq ���Գ�Ʊ������*/
	set_zd_data( "0910", sMoDiscnt.br_no );	/* ��Ʊ������� */

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
