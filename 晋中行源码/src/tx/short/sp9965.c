/*************************************************
* �� �� ��: sp9965.c
* �������������ֻ������ѯ�ͻ���Ӧ����(mob_custinfo)
*
* ��    ��: jack
* ������ڣ�2003��7��18��
*
* �޸ļ�¼�� 
*   ��   ��:
*   �� �� ��:
*   �޸�����:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mob_custinfo_c.h"

int sp9965()
{
	int ret=0;
	char cMobile[15];

	struct mob_custinfo_c sMob_custinfo;

	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( &sMob_custinfo, 0x0, sizeof(sMob_custinfo) );

	get_zd_data( "0610", cMobile );

	ret = Mob_custinfo_Sel( g_pub_tx.reply, &sMob_custinfo, \
		"mobile = '%s'", cMobile );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Mob_custinfo_Sel����[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sMob_custinfo.cust_name );
	set_zd_data( "0260", sMob_custinfo.cust_name );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "���ֻ������ѯ�ͻ������ɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "���ֻ������ѯ�ͻ�����ʧ��[%d]!!!", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
