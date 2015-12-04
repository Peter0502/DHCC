/*************************************************
* �� �� ��:  sp9985.c
* ���������� �ɳж�Э���Ż��ԳжҲ��ԭʼƱ�ݵǼǱ���Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��8��25��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_bank_split_po_c.h"

int sp9985()
{
	struct  mo_bank_split_po_c  sMoBankSplitPo; /* �жҲ��ԭʼƱ�ݵǼǲ� */
	struct	mdm_ac_rel_c		sMdmAcRel;		/* �������˻���ϵ�� */
	int	ret;

	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	
	get_zd_data( "0630", sMoBankSplitPo.pact_no );	/* ��Ѻ��� */

	/* ���жҲ��ԭʼƱ�ݵǼǱ����Ƿ���ڴ˳ж�Э���� */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "pact_no='%s'", sMoBankSplitPo.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�жҲ��ԭʼƱ�ݵǼǱ��в����ڴ���Ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B308" );
		goto ErrExit;
  	}else if( ret )
  	{
		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Sel��![%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_double( "1002", sMoBankSplitPo.par_amt );	/* Ʊ���� */
	set_zd_data( "0900", "111" );						/* Ʊ������ */
	set_zd_data( "0600", sMoBankSplitPo.pnote_no );		/* Ʊ�ݺ� */
	set_zd_data( "0330", sMoBankSplitPo.bail_ac_no );	/* ��֤���˺� */

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
