/*************************************************
* �� �� ��: spH614.c
* ��������������ҵ��������������ά��
*
* ��    ��: jack
* ������ڣ�2004��06��28��
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
#include "ag_unitinfo_c.h"
#include "pay_agent_c.h"

int spH614()
{
	int ret = 0;

	char cUnit_no[5];
	long lDate = 0;
	long lDate1 = 0;

	struct ag_unitinfo_c sAg_unitinfo;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	get_zd_data( "0920", cUnit_no );
	get_zd_long( "0440", &lDate );
	get_zd_long( "0470", &lDate1 );

	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg,"Ҫ�޸ĵĵ�λ��Ϣ������!!!" );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"����Ҫ�޸ĵĵ�λ��Ϣ����!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_unitinfo set beg_date ='%ld',end_date ='%ld' where unit_no = '%s' and unit_stat = '0' ",lDate, lDate1, cUnit_no );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"�޸ĵ�λ��Ϣ�����������ڴ�!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update pay_agent set tx_date ='%ld' where unit_no = '%s' and result = '1' ",lDate, cUnit_no );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"�޸Ĵ����¼�������������ڴ�!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg,"�޸Ĵ�����Ϣ���������ڳɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"�޸Ĵ�����Ϣ����������ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
