/*************************************************
* �� �� ��:  sp9984.c
* ���������� �ɱ�֤���˺Ż��ԳжҲ����Ϣ
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

int sp9984()
{
	struct	mdm_ac_rel_c		sMdmAcRel;			/* �������˻���ϵ�� */
	struct	mo_bank_split_po_c	sMoBankSplitPo;		/* ���ֵǼǱ� */
	struct	dd_mst_c			sDdMst;				/* ���ڴ�����ļ� */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sMoBankSplitPo, 0x00, sizeof( struct mo_bank_split_po_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0330", g_pub_tx.ac_no );	/* ��֤���˺� */

	/* ��ѯ�ʺ��Ƿ�Ϊ�����ʺ� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˿ͻ��ʺŲ�����![%s]", g_pub_tx.ac_no );
   		WRITEMSG
		set_zd_data( "0961", "���䱣֤���˺�����,������ж�Э���Ż�����Ӧ��Ϣ!" );	/* ��ʾ��Ϣ */
TRACE
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* ���ݱ�֤���˺�ID���ʻ���Ų�ѯ�жҲ��ԭʼƱ�ݵǼǱ� */
	ret = Mo_bank_split_po_Sel( g_pub_tx.reply, &sMoBankSplitPo, "bail_ac_no='%s'", g_pub_tx.ac_no );
	if( ret == 100 )
	{
		set_zd_data( "0961", "���䱣֤���˺�����,������ж�Э���Ż�����Ӧ��Ϣ!" );	/* ��ʾ��Ϣ */
		goto OkExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_bank_split_po_Sel��![%d]", ret );
       	WRITEMSG
       	goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0620", sMoBankSplitPo.pact_no );	/* �ж�Э���� */
	set_zd_data( "0900", "111" );					/* Ʊ������ */
	set_zd_data( "0600", sMoBankSplitPo.pnote_no );	/* Ʊ�ݺ� */
	set_zd_double( "1002", sMoBankSplitPo.par_amt );/* ��Ʊ��� */

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
