/***************************************************************
* �� �� ��: spB310.c
* ������������Ʊ�����ӽ���(�ж��Ƿ��ж�����)
*
* ��    ��: jane
* ������ڣ�2003��11��09��
*
* �޸ļ�¼��
*    ��    ��: 
*    �� �� ��: 
*    �޸�����: 
*
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "mbfe_pub.h"
#include "mo_cashier_c.h"

spB310()
{
	struct mo_cashier_c sMoCashier;

	memset( &sMoCashier, 0x00, sizeof( struct mo_cashier_c ) );

	get_zd_data( "0580", sMoCashier.cashier_no );
	get_zd_double( "0410", &sMoCashier.rem_amt );

	g_reply_int = pub_base_CompDblVal( sMoCashier.rem_amt, 0.00 );
	if( g_reply_int == 0 )
	{
		strcpy( g_pub_tx.reply, "0000" );
		goto OkExit;
	}

	vtcp_log( "��Ʊ���������[%lf]", sMoCashier.rem_amt );

	get_zd_data( "0320", sMoCashier.rem_ac_no );
	pub_base_strpack( sMoCashier.rem_ac_no );
	if( strlen( sMoCashier.rem_ac_no ) == 0 )
	{
		g_reply_int = Mo_cashier_Sel( g_pub_tx.reply, &sMoCashier, "cashier_no='%s'", sMoCashier.cashier_no );
		if( g_reply_int != 0 )
		{
			sprintf( acErrMsg, "��ѯ��Ʊ�Ǽǲ�����![%d]", g_reply_int );
			MBFEWRITEMSG
			goto ErrExit;
		}
		strcpy( sMoCashier.rem_ac_no, sMoCashier.requ_ac_no );
	}

	set_zd_data( "0191", "1" );
	set_zd_data( "0192", "0001" );
	set_zd_data( "0193", "1001" );
	set_zd_data( "1011", sMoCashier.rem_ac_no );
	set_zd_double( "1013", sMoCashier.rem_amt );
	set_zd_data( "1016", "��Ʊ�����˻�" );
	set_zd_data( "101A", "01" );
	set_zd_data( "101B", "2" );

	strcpy( g_pub_tx.reply, "0001" );

OkExit:
	sprintf(acErrMsg,"���±�Ʊǩ���Ǽǲ��ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���±�Ʊǩ���Ǽǲ�ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
