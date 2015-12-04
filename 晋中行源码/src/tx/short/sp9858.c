/*************************************************
* �� �� ��:  sp9858.c
* ���������� ������Э���Ż��������ʻ������������˻����
*
* ��    ��:  jane
* ������ڣ� 2003��2��24��
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

int sp9858()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	struct	ln_mst_c	sLnMst;		/* �������ļ� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */

	/* ������ֵǼǱ����Ƿ��Ѿ����ڴ�����Э���� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
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

	/* ���ݴ����˺�id���˺���ż��������ļ� */
	ret = Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d", sMoDiscnt.ac_id,\
			 sMoDiscnt.ac_seqn );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "�������ļ��в����ڴ��˺�!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "W018" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Ln_mst_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}
sprintf( acErrMsg, "name=[%s],bal=[%f]", sLnMst.name, sLnMst.bal);
                WRITEMSG	
	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0260", sLnMst.name );             /* �����ʻ����� */
    	set_zd_double( "1004", sLnMst.bal );		/* �����˻���� */
    	set_zd_double( "1001", sMoDiscnt.par_amt );	/* ��Ʊ��� */

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




