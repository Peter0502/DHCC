/*************************************************
* �� �� ��:  sp8803.c
* ���������� ��Ա���ս��ײ�ѯ
*
* ��    ��:  lance
* ������ڣ� 20030623
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

int sp8803()
{

	char tel_no[5];
	char acc_hrt[8];

	int ttlnum=0;			/**��ȡ��������**/
	int i=0;
	int ret=0;
	
	double cash_amt1;
	double cash_amt2;
	double d_amt1;
	double d_amt2;
	long   cash_num1;
	long   cash_num2;
	long   d_num1;
	long   d_num2; 

	pub_base_sysinit();

	get_zd_data("0920",tel_no );
	get_zd_data("0640",acc_hrt );	

	/**��ɲ�ѯ����**/
	pub_base_strpack( acc_hrt );
	if( !strcmp(acc_hrt,"70400") )
	{
		sprintf( acErrMsg,"70400��Ŀ������ͳ��!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L160" );
		goto ErrExit;	
	}	
		
	/* ͳ���ֽ�跽��������� */
	if( strlen(acc_hrt) )
	{	
		ret = sql_sum_double( "dc_log" , "amt" , &cash_amt1 , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='1' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
		if (ret)	goto ErrExit;
		cash_num1 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='1' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
	}
	else
	{
		ret = sql_sum_double ( "dc_log" , "amt" , &cash_amt1 , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='1' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );
		if (ret)	goto ErrExit;
		cash_num1 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='1' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );	
	}
	/* ͳ���ֽ������������� */
	if( strlen(acc_hrt) )
	{	
		ret = sql_sum_double( "dc_log" , "amt" , &cash_amt2 , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='2' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
		if (ret)	goto ErrExit;
		cash_num2 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='2' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
	}
	else
	{
		ret = sql_sum_double ( "dc_log" , "amt" , &cash_amt2 , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='2' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );
		if (ret)	goto ErrExit;		
		cash_num2 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='1' and dc_ind='2' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );	
	}	
			
	/* ͳ��ת�˽跽��������� */
	if( strlen(acc_hrt) )
	{	
		ret = sql_sum_double( "dc_log" , "amt" , &d_amt1 , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='1' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
		if (ret)	goto ErrExit;
		d_num1 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='1' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
	}
	else
	{
		ret = sql_sum_double ( "dc_log" , "amt" , &d_amt1 , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='1' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );
		if (ret)	goto ErrExit;
		d_num1 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='1' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );	
	}
	/* ͳ��ת�˴������������ */
	if( strlen(acc_hrt) )
	{	
		ret = sql_sum_double( "dc_log" , "amt" , &d_amt2 , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='2' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
		if (ret)	goto ErrExit;
		d_num2 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='2' and sts='0' and acc_hrt='%s' " , tel_no,g_pub_tx.tx_date,acc_hrt );
	}
	else
	{
		ret = sql_sum_double ( "dc_log" , "amt" , &d_amt2 , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='2' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );
		if (ret)	goto ErrExit;		
		d_num2 = sql_count( "dc_log" , "tel='%s' and tx_date=%ld and ct_ind='2' and dc_ind='2' and sts='0' and acc_hrt[1]<'6'" , tel_no,g_pub_tx.tx_date );	
	}		

	set_zd_double( "0400" , cash_amt1 );
	set_zd_double( "0410" , cash_amt2 );	
	set_zd_double( "0420" , d_amt1 );
	set_zd_double( "0430" , d_amt2 );
	
	set_zd_long( "0440" , cash_num1 );	
	set_zd_long( "0450" , cash_num2 );	
	set_zd_long( "0460" , d_num1 );	
	set_zd_long( "0470" , d_num2 );	
				
GoodExit:
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
