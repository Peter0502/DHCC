/*************************************************
* �� �� ��:  sp9912.c
* ���������� �ɳж�Э���Ż��Գжһ�Ʊ��Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��5��27��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_po_c.h"

int sp9912()
{
	struct	mo_bank_po_c		sMoBankPo;	/* �жһ�Ʊ�Ǽǲ� */
	int	ret;

	memset( &sMoBankPo, 0x00, sizeof( struct mo_bank_po_c ) );

	get_zd_data( "0330", sMoBankPo.pact_no );	/* �ж�Э���� */
	get_zd_long( "0500", &sMoBankPo.pact_no_num );	/* ԭ��� */

	/* ���ݳж�Э���Ų������гжһ�Ʊ�ǼǱ� */
	ret = Mo_bank_po_Sel( g_pub_tx.reply, &sMoBankPo, "pact_no='%s'\
				and pact_no_num=%d", sMoBankPo.pact_no, sMoBankPo.pact_no_num );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�޴˳ж�Э����!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B187" );
		goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_bank_po_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_long( "0450", sMoBankPo.sign_date );		/* ǩ������ */

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
