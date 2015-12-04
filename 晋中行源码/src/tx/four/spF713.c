/*************************************************************
* �� �� ��: spF713.c
* �����������ж��Ǵ����г��˻��ǲ���
* ��    ��: jack
* ������ڣ�2003��2��22��
*
* �޸ļ�¼��
* ��    ��:
* �� �� ��:
* �޸�����:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF713()
{
	int ret = 0;
	
	char cCd_flag[2];

	double dTram = 0.00;		/*���׽��*/
	double dTram_can = 0.00;	/*�������׽��*/
	long lTrace_no = 0;
	char cTrcode[5];
	char cCdno[21];
	long lDate = 0;

	struct four_tradelog_c sFour_tradelog;

	memset(cCd_flag,0x0,sizeof(cCd_flag));
	memset( cTrcode, 0x0, sizeof(cTrcode) );
	
	/*** ��ʼ��ȫ�ֱ��� ***/
	pub_base_sysinit();

	set_zd_data("0310",TESTCARD);	/*ֱ�Ӽ����ͨ������114�˺�*/
	set_zd_int("0350",1);				/*�����˺����*/
	
	get_zd_long( "0780", &lTrace_no );
	set_zd_long( "0600", lTrace_no );
	
	get_zd_data( "0160", cTrcode );
	get_zd_double( "0400", &dTram );
	get_zd_data( "0370", cCdno );
	get_zd_long( "0550", &lDate );
	
	get_zd_data("0670",cCd_flag);
	
	if ( strcmp( cTrcode, "7711" ) == 0 )
	{
		sprintf( acErrMsg, "------->Ҫ���ʵļ�¼��ѯ����Ϊ[%ld][%ld][%lf][%s]", \
			lDate, lTrace_no, dTram, cCdno  );
		WRITEMSG
		
		memset( &sFour_tradelog, 0x0, sizeof(sFour_tradelog) );
		ret = Four_tradelog_Sel( g_pub_tx.reply, &sFour_tradelog, \
			"tx_date = %ld and chang_no = %ld and tx_amt = %lf \
			and ac_no = '%s' and tx_stat = '0'", lDate, lTrace_no, \
			dTram, cCdno );
		if ( ret )
		{
			sprintf( acErrMsg, "------->Ҫ���ʵļ�¼������[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sFour_tradelog.tx_code );
		
		if ( cCd_flag[0] == '0' && strcmp( sFour_tradelog.tx_code, "7801" ) )
		{
			strcpy( g_pub_tx.reply, "H061" );
			sprintf( acErrMsg, "------->��ȡ��־�뽻�����Ͳ�ƥ��!!!" );
			WRITEMSG
			goto ErrExit;
		}
		if ( cCd_flag[0] == '1' && strcmp( sFour_tradelog.tx_code, "7802" ) )
		{
			strcpy( g_pub_tx.reply, "H061" );
			sprintf( acErrMsg, "------->��ȡ��־�뽻�����Ͳ�ƥ��!!!" );
			WRITEMSG
			goto ErrExit;
		}
		
		ret = sql_execute( "update four_tradelog set tx_stat = '2' where \
			tx_date = %ld and chang_no = %ld and tx_amt = %lf \
			and ac_no = '%s' and tx_stat = '0'", lDate, lTrace_no, \
			dTram, cCdno );
		if ( ret )
		{
			strcpy( g_pub_tx.reply, "H008" );
			sprintf( acErrMsg, "------->�޸ĳ��ʵļ�¼����!!!" );
			WRITEMSG
			goto ErrExit;
		}
		
		dTram_can = 0 - dTram;
		set_zd_double( "0410", dTram_can );
	}
	
	if (cCd_flag[0] == '0')
	{
		goto OKExit1;
	}
	else
	{
		goto OKExit2;
	}

ErrExit:
	sprintf(acErrMsg,"�жϳ岹��־ʧ��![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
OKExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"�ý���Ϊ���˽���![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OKExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"�ý���Ϊ���˽���![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
