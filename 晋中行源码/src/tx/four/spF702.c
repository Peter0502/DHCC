/***********************************************************
* �� �� ��: spF702.c
* �����������ж�Ҫ�������ļ�¼�Ƿ����
* ��    ��: jack
* ������ڣ�2003��2��21��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF702()
{
	int ret = 0;
	int trace_no = 0;
	char cTrace_no[9];
	int iTrace_no = 0;
	char cCity[5];
	
	struct four_tradelog_c sFour_Tradelog;

	double tram = 0.00;		/*���׽��*/

	g_reply_int=0;

	memset(cCity,0x0,sizeof(cCity));
	memset(&sFour_Tradelog,0x0,sizeof(sFour_Tradelog));
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	get_zd_data("0360",cCity);
	get_zd_double("0400",&tram);
	
	memset( cTrace_no, 0x0, sizeof(cTrace_no) );
	get_zd_data( "0610", cTrace_no );
	iTrace_no = atol( cTrace_no );

	sprintf( acErrMsg,"---->Ҫ�������ĺ�̨��ˮ��Ϊ[%ld]\n", iTrace_no );
	WRITEMSG

	ret = Four_tradelog_Sel(g_pub_tx.reply, &sFour_Tradelog, \
		"tx_date = %ld and city_no = '%s' and trace_no = %ld \
		and tx_amt = %f and tx_stat = '0'", \
		g_pub_tx.tx_date, cCity, iTrace_no, tram);
	if (ret != 0)
	{
		sprintf(acErrMsg,"��ѯ���������׼�¼ʧ��![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0370", sFour_Tradelog.ac_no );

	ret = Four_tradelog_Dec_Upd( g_pub_tx.reply , "tx_date = %ld \
		and city_no = '%s' and trace_no = %ld and tx_amt = %f \
		and tx_stat = '0'",g_pub_tx .tx_date,cCity, \
		sFour_Tradelog.trace_no, tram);
	if (ret != 0)
	{
		sprintf(acErrMsg,"�޸ı��������׼�¼ʧ��111![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = Four_tradelog_Fet_Upd( &sFour_Tradelog , &g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"�޸ı��������׼�¼ʧ��222![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}
	
	strcpy(sFour_Tradelog.tx_stat,"1");

	ret = Four_tradelog_Upd_Upd( sFour_Tradelog , &g_pub_tx.reply  );
	if (ret != 0)
	{
		sprintf(acErrMsg,"�޸ı��������׼�¼ʧ��333![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	ret = Four_tradelog_Clo_Upd( );

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"��س�����¼����ɹ�![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	Four_tradelog_Clo_Upd( );
	sprintf(acErrMsg,"��س�����¼����ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
