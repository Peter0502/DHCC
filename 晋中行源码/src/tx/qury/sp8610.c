/*************************************************
* �� �� ��:  sp8610.c
* ���������� ÿ��ʵʱ�������ѯ
*
* ��    ��:  rob
* ������ڣ� 20031220
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"

int sp8610()
{
	int ret = 0;

	double dAcbl1 = 0.00;	/*�������*/	
	double dAcbl2 = 0.00;	/*�������*/	
	double dAcbl = 0.00;	/*�����*/	

	pub_base_sysinit();

	ret = sql_sum_double( "dd_mst", "bal", &dAcbl1, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "ͳ�ƻ������ʧ��!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}

	ret = sql_sum_double( "td_mst", "bal", &dAcbl2, "1=1" );
	if ( ret )
	{
		sprintf( acErrMsg, "ͳ�ƶ������ʧ��!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D101" );
		goto ErrExit;
	}

	dAcbl = dAcbl1 + dAcbl2;

	set_zd_double( "0400", dAcbl );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
