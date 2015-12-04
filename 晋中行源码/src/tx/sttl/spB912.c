/*************************************************
* �� �� ��:  spB912.c
* ���������� ���������˹��ܡ�
*
* ��    ��:  jane
* ������ڣ� 2003��4��8��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "mo_discnt_c.h"

int spB912()
{
	struct	mo_discnt_c	sMoDiscnt;	/* ���ֵǼǱ� */
	int	ret;

	memset( &sMoDiscnt, 0x00, sizeof( struct mo_discnt_c ) );
	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	/* ��ǰ̨��Ļȡֵ */
	get_zd_data( "0620", sMoDiscnt.pact_no );	/* ����Э���� */

	/* ��������Э���Ų�ѯ���ֵǼǲ� */
	ret = Mo_discnt_Sel( g_pub_tx.reply, &sMoDiscnt, "pact_no='%s'", sMoDiscnt.pact_no );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "������Э���Ų�����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Sel��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	if( sMoDiscnt.pn_type[0] != '1' )
	{
		sprintf( acErrMsg, "��Э����Ϊת���ֻ�������Э����!����Ҫ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B151" );
		goto ErrExit;
	}

	if( sMoDiscnt.sts[0] == '2' )
	{
		sprintf( acErrMsg, "������Э�������!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B152" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '3' )
	{
		sprintf( acErrMsg, "�˱������Ѽ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B148" );
		goto ErrExit;
	}else if( sMoDiscnt.sts[0] == '*' )
	{
		sprintf( acErrMsg, "�˱������ѻ���!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "B149" );
		goto ErrExit;
	}

	/* ��������Э���ź����������޸����ֵǼǲ� */
	ret = Mo_discnt_Dec_Upd( g_pub_tx.reply, "pact_no='%s' and pn_type='%s'", \
				sMoDiscnt.pact_no, "1" );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mo_discnt_Dec_Upd��[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = Mo_discnt_Fet_Upd( &sMoDiscnt, g_pub_tx.reply );
	if( ret == 100 )
	{
		sprintf( acErrMsg, "���ֵǼǱ��в����ڴ�����Э����!" );
                WRITEMSG
                strcpy( g_pub_tx.reply, "B113" );
                goto ErrExit;
       	}else if( ret )
       	{
       		sprintf( acErrMsg, "ִ��Mo_discnt_Fet_Upd��![%d]", ret );
                WRITEMSG
                goto ErrExit;
       	}

	/* �޸����ֵǼǱ���״̬Ϊ�����*/
	strcpy( sMoDiscnt.sts, "2" );			/* ��־��2����� */

	ret = Mo_discnt_Upd_Upd( sMoDiscnt, g_pub_tx.reply );
	if ( ret )
	{
		sprintf( acErrMsg, "�޸����ֵǼǱ���[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	Mo_discnt_Clo_Upd();

	/**----- xxx ---- @20030819@ ---- ��������Ҫ��Э���� ----**/
	strcpy(g_pub_tx.ac_no,sMoDiscnt.pact_no);
	g_pub_tx.tx_amt1=0.00;
	strcpy( g_pub_tx.brf, "�������" );
	if (pub_ins_trace_log())
	{
		sprintf(acErrMsg,"�Ǽǽ�����ˮ����!!!");
		WRITEMSG
		goto ErrExit;
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
