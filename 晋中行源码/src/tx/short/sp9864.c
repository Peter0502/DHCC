/*************************************************
* �� �� ��:  sp9864.c
* ���������� ������Э���Ż������ֻ���ǼǱ���Ϣ
*
* ��    ��:  jane
* ������ڣ� 2003��3��9��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mo_pay_discnt_c.h"

int sp9864()
{
	struct	mo_pay_discnt_c	sMoPayDiscnt;	/* ���ֻ���ǼǱ� */
	struct	ln_mst_c	sLnMst;		/* �������ļ� */
	int	ret;

	memset( &sMoPayDiscnt, 0x00, sizeof( struct mo_pay_discnt_c ) );
	memset( &sLnMst, 0x00, sizeof( struct ln_mst_c ) );
	
	get_zd_data( "0620", sMoPayDiscnt.pact_no );/* ����Э���� */

	/* ������ֵǼǱ����Ƿ���ڴ�����Э���� */
	ret = Mo_pay_discnt_Sel( g_pub_tx.reply, &sMoPayDiscnt, "pact_no='%s'", sMoPayDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֻ���ǼǱ��в����ڴ�����Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_pay_discnt_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* ���ݴ����˺�id���˺���ż��������ļ� */
	ret = Ln_mst_Sel( g_pub_tx.reply, &sLnMst, "ac_id=%ld and ac_seqn=%d", sMoPayDiscnt.ac_id,\
			 sMoPayDiscnt.ac_seqn );
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

	/* ��ǰ̨��Ļ��ֵ */
	set_zd_data( "0260", sLnMst.name );             /* �����ʻ����� */
    	set_zd_double( "1004", sLnMst.bal );		/* �����˻���� */
    	set_zd_double( "1003", sMoPayDiscnt.pay_amt );	/* ������ */
	set_zd_data( "0700", sMoPayDiscnt.ct_ind );	/* ��ת��־ */
	if( sMoPayDiscnt.ct_ind[0] == '2' )		/* ת��ʱ */
	{
		set_zd_data( "0300", sMoPayDiscnt.turn_ac_no );	/* �����˺� */
		set_zd_data( "0250", sMoPayDiscnt.turn_name );	/* ���� */
		goto OkExit;
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




