/*************************************************
* �� �� ��:  sp9801.c
* ���������� ���˺Ż��Ի������
*
* ��    ��:  jack
* ������ڣ� 
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9801()
{
	get_zd_data( "0300", g_pub_tx.ac_no );			/* �ʺ� */

	set_zd_data( DC_FILE_SND,"1" );
	set_zd_double( "0400",800.00 );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
