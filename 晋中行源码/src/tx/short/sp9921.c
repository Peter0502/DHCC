/*************************************************
* �� �� ��:  sp9921.c
* ���������� �ɿͻ��ʺź��ʻ����ͻ��Ի���
*
* ��    ��:  jane
* ������ڣ� 2003��6��15��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9921()
{
	struct	mdm_ac_rel_c	sMdmAcRel;	/* �������˻���ϵ�� */
	struct	dd_mst_c	sDdMst;		/* ���ڴ�����ļ� */
	int	ret;

	memset( &sMdmAcRel, 0x00, sizeof( struct mdm_ac_rel_c ) );
	memset( &sDdMst, 0x00, sizeof( struct dd_mst_c ) );

	get_zd_data( "0830", g_pub_tx.ac_no );		/* �ͻ��ʺ� */

	/* ��ѯ�ͻ��ʺ��Ƿ�Ϊ�����ʺ� */
	ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdmAcRel, "ac_no='%s'",\
				g_pub_tx.ac_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˿ͻ��ʺŲ�����![%s]", g_pub_tx.ac_no );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "L113" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}

	/* ���ݿͻ��ʺ�ID���ʻ���Ų�ѯ���ڴ�����ļ� */
	ret =Dd_mst_Sel( g_pub_tx.reply, &sDdMst, "ac_id=%ld and ac_seqn=%d", \
			sMdmAcRel.ac_id, sMdmAcRel.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�˿ͻ��ʺŷǻ���![%s]", g_pub_tx.ac_no );
    		WRITEMSG
    		strcpy( g_pub_tx.reply, "B114" );
		goto ErrExit;
	}else if( ret )
	{
		sprintf( acErrMsg, "ִ��Mdm_ac_rel_Sel��![%d]", ret );
        	WRITEMSG
        	goto ErrExit;
	}

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0250", sDdMst.name );	/* ���� */

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




