/*****************************************************
* �� �� ��:  sp9958.c
* ���������� ���˺�ת���˺�
*
* ��    ��:  jane
* ������ڣ� 2003��7��23�� 
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*****************************************************/

#define EXTERN
#include <string.h>
#include "public.h"
#include "mbfe_pub.h"

int sp9958()
{
	char ac_no[21];

	memset( ac_no, 0x00, sizeof( ac_no ) );

	get_zd_data( "0380", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0380", ac_no );
		goto OkExit;
	}

	get_zd_data( "0370", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0370", ac_no );
		goto OkExit;
	}

	get_zd_data( "0300", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0300", ac_no );
		goto OkExit;
	}

	get_zd_data( "0310", ac_no );
	if( strlen( ac_no ) )
	{
		pub_base_old_acno( ac_no );
		set_zd_data( "0310", ac_no );
		goto OkExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;

ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
