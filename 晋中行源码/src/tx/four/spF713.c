/*************************************************************
* 文 件 名: spF713.c
* 功能描述：判断是代理行冲账还是补账
* 作    者: jack
* 完成日期：2003年2月22日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "four_tradelog_c.h"

spF713()
{
	int ret = 0;
	
	char cCd_flag[2];

	double dTram = 0.00;		/*交易金额*/
	double dTram_can = 0.00;	/*冲正交易金额*/
	long lTrace_no = 0;
	char cTrcode[5];
	char cCdno[21];
	long lDate = 0;

	struct four_tradelog_c sFour_tradelog;

	memset(cCd_flag,0x0,sizeof(cCd_flag));
	memset( cTrcode, 0x0, sizeof(cTrcode) );
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	set_zd_data("0310",TESTCARD);	/*直接记异地通兑清算114账号*/
	set_zd_int("0350",1);				/*活期账号序号*/
	
	get_zd_long( "0780", &lTrace_no );
	set_zd_long( "0600", lTrace_no );
	
	get_zd_data( "0160", cTrcode );
	get_zd_double( "0400", &dTram );
	get_zd_data( "0370", cCdno );
	get_zd_long( "0550", &lDate );
	
	get_zd_data("0670",cCd_flag);
	
	if ( strcmp( cTrcode, "7711" ) == 0 )
	{
		sprintf( acErrMsg, "------->要冲帐的记录查询条件为[%ld][%ld][%lf][%s]", \
			lDate, lTrace_no, dTram, cCdno  );
		WRITEMSG
		
		memset( &sFour_tradelog, 0x0, sizeof(sFour_tradelog) );
		ret = Four_tradelog_Sel( g_pub_tx.reply, &sFour_tradelog, \
			"tx_date = %ld and chang_no = %ld and tx_amt = %lf \
			and ac_no = '%s' and tx_stat = '0'", lDate, lTrace_no, \
			dTram, cCdno );
		if ( ret )
		{
			sprintf( acErrMsg, "------->要冲帐的记录不存在[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}
		
		pub_base_strpack( sFour_tradelog.tx_code );
		
		if ( cCd_flag[0] == '0' && strcmp( sFour_tradelog.tx_code, "7801" ) )
		{
			strcpy( g_pub_tx.reply, "H061" );
			sprintf( acErrMsg, "------->存取标志与交易类型不匹配!!!" );
			WRITEMSG
			goto ErrExit;
		}
		if ( cCd_flag[0] == '1' && strcmp( sFour_tradelog.tx_code, "7802" ) )
		{
			strcpy( g_pub_tx.reply, "H061" );
			sprintf( acErrMsg, "------->存取标志与交易类型不匹配!!!" );
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
			sprintf( acErrMsg, "------->修改冲帐的记录出错!!!" );
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
	sprintf(acErrMsg,"判断冲补标志失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
OKExit1:
	strcpy(g_pub_tx.reply,"0001");
	sprintf(acErrMsg,"该交易为冲账交易![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
OKExit2:
	strcpy(g_pub_tx.reply,"0002");
	sprintf(acErrMsg,"该交易为补账交易![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
}
