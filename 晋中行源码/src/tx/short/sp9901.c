/*************************************************
* �� �� ��:  sp9901.c
* ���������� ���ֽ��ո���־�����ֽ�����ź��ֽ���;
*
* ��    ��:  jane
* ������ڣ� 2003��4��27��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9901()
{
	char	cOperCode[2];				/* ������ */	

	memset( cOperCode, 0x00, sizeof( cOperCode ) );

	get_zd_data( "0670", cOperCode );		/* ������ */

	if( strcmp( cOperCode, "0" ) == 0 )		/* ���� */
	{
		set_zd_data( "1185", "220" );		/* �ֽ������ */
		set_zd_data( "1186", "23" );		/* ��; */
	}
	else						/* ���� */
	{
		set_zd_data( "1185", "120" );		/* �ֽ������ */
		set_zd_data( "1186", "22" );		/* ��; */
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
