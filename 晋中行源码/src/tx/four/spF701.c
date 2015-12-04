/*************************************************************
* 文 件 名: spF701.c
* 功能描述：判断是否本行账号业务(长沙城市号为:4302)
* 作    者: jack
* 完成日期：2003年2月20日
*
* 修改记录：
* 日    期: 2003年5月23日
* 修 改 人: jack
* 修改内容: 将本代他和他代本分开
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spF701()
{
	char cCity[5];
	int ret = 0;

	char cTrcode[5];
	char cRemark[20];
	char cName[50];
	char cAc_no[20];
	
	char filename[64];
	FILE *fp;

	double tram = 0.00;		/*交易金额*/

	memset(cCity,0x0,sizeof(cCity));
	memset( cTrcode, 0x0, sizeof(cTrcode) );
	memset( cName, 0x0, sizeof(cName) );
	memset( cRemark, 0x0, sizeof(cRemark) );
	memset( cAc_no, 0x0, sizeof(cAc_no) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0160", cTrcode );
	get_zd_data( "0270", cName );
	get_zd_data( "0370", cAc_no );

	get_zd_data("0360",cCity);
	if( strncmp(cCity, "4302", 4) == 0 )	/*	目标行不能是长沙 */
	{
		sprintf( acErrMsg, "目标行错!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}

	if ( strncmp( cTrcode, "7701", 4 ) == 0 )
	{
		strcpy( cRemark, "异地账户续存" );
	}
	else if ( strncmp( cTrcode, "7702", 4 ) == 0 )
	{
		strcpy( cRemark, "异地账户取款" );
	}
	get_zd_double("0400",&tram);

	fprintf( fp, "DLPT%s|%s|%lf|%ld|%s|%s|%s|%ld|%s|%s|\n", \
		cRemark, "", tram, \
		g_pub_tx.tx_date, g_pub_tx.tel, cAc_no, cName, \
		g_pub_tx.trace_no, "现金", g_pub_tx.tx_br_no );

	fclose( fp );

	set_zd_data("0310",TESTCARD );		/* 直接记清算科目*/
	set_zd_int("0350",1);				/*活期账号序号*/
	
	set_zd_data( "0110", "1" );

OKExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"该账户为他行账户![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply,"F006" );
	sprintf( acErrMsg,"交易目标行错![%s]",cCity );
	WRITEMSG
	set_zd_data( "0120",g_pub_tx.reply );
	return 1;
}
