/*************************************************
* �� �� ��:  spH652.c
* ���������� �ͻ�����ȡ�����ŷ���
*
* ��    ��:  jack
* ������ڣ� 2004��04��24��
*
* �޸ļ�¼�� 
* ��    ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int spH652()
{
	int ret = 0;
	int i = 0;

	char cMobile[14];
	char cPasswd[7];

	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( cPasswd, 0x0, sizeof(cPasswd) );

	/* ���ݳ�ʼ�� */
	pub_base_sysinit();

	get_zd_data( "0610", cMobile );

	ret = sql_execute( "delete from mob_acct_type where \
		mobile = '%s'", cMobile );
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"ȡ���ͻ��˺ŷ���Ʒ�ֳ���!!![%s]", cMobile );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from mob_custinfo where \
		mobile = '%s'", cMobile );
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"ȡ���������пͻ���Ϣ����!!![%s]", cMobile );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg,"ȡ�����ŷ���ɹ�!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"ȡ�����ŷ���ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
