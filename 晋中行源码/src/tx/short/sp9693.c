/*************************************************
* �� �� ��:  sp9693.c
* ���������� ������Э���Ż������ֵǼǱ���Ϣ
*
* ��    ��:  rob 
* ������ڣ� 2003��9��27��
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

int sp9693()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0900", sMoDiscnt.note_type );	/* �жһ�Ʊ���� */
	get_zd_data( "0600", sMoDiscnt.pnote_no );	/* �жһ�Ʊ���� */

	/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "note_type='%s' and pnote_no='%s' and pn_type='4' ", sMoDiscnt.note_type,sMoDiscnt.pnote_no );
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
	set_zd_data( "0680", sMoDiscnt.local_ind );	/* ���ͬ�Ǳ�־ */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* ���������˻��� */
	set_zd_double( "0840", sMoDiscnt.tpn_rate );	/* ת������ */
	set_zd_double( "1001", sMoDiscnt.tpn_int );	/* ������Ϣ */

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
