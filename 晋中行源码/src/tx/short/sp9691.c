/*************************************************
* �� �� ��:  sp9691.c
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

int sp9691()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0630", sMoDiscnt.tpact_no );	/* ����Э���� */
vtcp_log( "zy   tpact_no[%s]",sMoDiscnt.tpact_no );
	/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "tpact_no='%s'", sMoDiscnt.tpact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�����Э����!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B113" );
		goto ErrExit;
   	}else if( ret )
   	{
   		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
   	}
vtcp_log( "zy    ac_id[%ld]  ac_seqn[%d]",sMoDiscnt.ac_id, sMoDiscnt.ac_seqn );

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_double( "0850", sMoDiscnt.tpn_rate );	/* ת������ */
    set_zd_double( "1002", sMoDiscnt.par_amt );	/* ��Ʊ��� */
	set_zd_long( "0440", sMoDiscnt.tpn_date );	/* ת�������� */
	set_zd_long( "0460", sMoDiscnt.return_date );	/* �ع����� */
	set_zd_data( "0250", sMoDiscnt.tpn_name );	/* ���������˻��� */
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
