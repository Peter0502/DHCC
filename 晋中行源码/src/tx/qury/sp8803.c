/*************************************************
* 文 件 名:  sp8803.c
* 功能描述： 柜员当日交易查询
*
* 作    者:  lance
* 完成日期： 20030623
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
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

	int ttlnum=0;			/**读取的总条数**/
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

	/**组成查询条件**/
	pub_base_strpack( acc_hrt );
	if( !strcmp(acc_hrt,"70400") )
	{
		sprintf( acErrMsg,"70400科目不参与统计!");
		WRITEMSG
		strcpy( g_pub_tx.reply ,"L160" );
		goto ErrExit;	
	}	
		
	/* 统计现金借方笔数、金额 */
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
	/* 统计现金贷方笔数、金额 */
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
			
	/* 统计转账借方笔数、金额 */
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
	/* 统计转账贷方笔数、金额 */
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
