/*************************************************
* �� �� ��: spF606.c
* �������������մ�������������ͳ��
*
* ��    ��: jack
* ������ڣ�2004��07��14��
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

int spF606()
{
	int ret = 0;
	int i = 0;
	char tmpstr[32];

	char cUnit_no[5];
	
	double dTram = 0.00;
	long lCount = 0;

	struct ag_unitinfo_c sAg_unitinfo;
	struct pay_agent_c sPay_agent;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"��ʼ�����ýṹ����!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );

	get_zd_data( "0920", cUnit_no );

	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
		"unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ִ��Ag_unitinfo_Sel����[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}

	sprintf( acErrMsg, "ȡ��������������Ϊ[%ld][%ld]", sAg_unitinfo.beg_date, sAg_unitinfo.end_date );
	WRITEMSG

	lCount = sql_count( "pay_agent", "unit_no = '%s' and result = '1'", cUnit_no );
	if ( lCount < 0 )
	{
		sprintf( acErrMsg, "ͳ�ƴ������¼��������[%d]!!!", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}
	
	ret = sql_sum_double( "pay_agent", "tram", &dTram, \
		"unit_no = '%s' and result = '1'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "ͳ�ƴ������¼�ܽ�����[%d]!!!", ret );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D103" );
		goto ErrExit;
	}
	
	set_zd_long( "0520", lCount );
	set_zd_double( "0400", dTram );
	set_zd_long( "0440", sAg_unitinfo.beg_date );
	set_zd_long( "0470", sAg_unitinfo.end_date );

OkExit:
	sprintf( acErrMsg,"���մ�����ͳ�����ݴ���ɹ�!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"���մ�����ͳ�����ݴ���ʧ��[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
