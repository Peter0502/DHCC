/*************************************************
* �� �� ��: sp9621.c
* �������������볬���ļ����
*
* ��    ��: jack
* ������ڣ�20031116
*
* �޸ļ�¼�� 
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int sp9621( )
{
	char cAcno[20];	
	char filename[128];
	char command[128];
	int ret=0;

    pub_base_sysinit();

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( filename, 0x0, sizeof(filename) );

	get_zd_data( "0310", cAcno );

	sprintf( filename, "/backup/pass_error/%s.%ld", cAcno, g_pub_tx.tx_date );

	memset( command, 0x0, sizeof(command) );
	sprintf(acErrMsg, "---filename=[%s]", filename);
	WRITEMSG
	sprintf( command, "rm -f %s", filename );

	ret = system( command );
	if ( ret )
	{
		sprintf(acErrMsg, "ɾ���ļ����� !! [%d]", ret);
		WRITEMSG
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"���볬���ļ�����ɹ� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���볬���ļ����ʧ�� reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
