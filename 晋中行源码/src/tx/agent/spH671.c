/*************************************************
* 文 件 名: spH671.c
* 功能描述：代缴费缴费方式处理
*
* 作    者: mike
* 完成日期：2004年06月11日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"
#include "ag_paytlcs_c.h"
#include "ag_itemlist_c.h"

int spH671()
{
	int ret = 0;
	int i = 0;

	char cFlag[2];
	char cUnit_no[5];
	
	char cCt_ind[5];
	
	char tmpstr[1024];
	char fldstr[38][64];

	char filename[128];
	FILE *fp;

	struct ag_paytlcs_c sAg_paytlcs;
	struct ag_itemlist_c sAg_itemlist;
	struct ag_unitinfo_c sAg_unitinfo;
	
	struct dd_mst_c sDd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	
	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	memset( cFlag, 0x0, sizeof(cFlag) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	get_zd_data( "0670", cFlag );	/* 缴费方式0-现金 */
	get_zd_data( "0920", cUnit_no );
	
	/*-------------为了取欠费年月提前读文件--------begin-----------*/
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	memset( filename, 0x0, sizeof(filename) );

	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "r" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fgets( tmpstr, 1024, fp );

	fclose( fp );

	memset( fldstr, 0x00, sizeof(fldstr) );
	for( i=0; i<34; i++ )
	{
		ret=pub_base_cut_str( tmpstr,fldstr[i],",",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
		pub_base_strpack( fldstr[i] );
	}
	/*------------------------------------------end--------------*/

	memset( &sAg_unitinfo, 0x00, sizeof(sAg_unitinfo) );
	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf(acErrMsg,"查找代理单位[%s]资料错[%d]!!", cUnit_no, ret);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_data( "0310", sAg_unitinfo.bank_acct_no );
	
	memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );	
	get_zd_data( "0920", sAg_paytlcs.unit_no );
	sprintf( sAg_paytlcs.bank_tlcs, "%ld", g_pub_tx.trace_no );
	strcpy( sAg_paytlcs.batch_no, "对帐批次" );
	strcpy( sAg_paytlcs.unit_tlcs, "代理流水" );
	strcpy( sAg_paytlcs.pre_no, "电子账号" );

	if ( cFlag[0] == '0' )
	{
		strcpy( sAg_paytlcs.bank_acct_no, "现金" ); /*个人账号 */
	}
	else
	{
		get_zd_data( "0370", sAg_paytlcs.bank_acct_no );
		pub_base_old_acno( sAg_paytlcs.bank_acct_no );
		set_zd_data( "0370", sAg_paytlcs.bank_acct_no);
	}
	
	sprintf( sAg_paytlcs.trn_date, "%ld", g_pub_tx.tx_date );
	sprintf( sAg_paytlcs.trn_time, "%ld", g_pub_tx.tx_time );
	strcpy( sAg_paytlcs.br_no, g_pub_tx.tx_br_no );
	strcpy( sAg_paytlcs.tel_no, g_pub_tx.tel );
	
	get_zd_double( "0400", &sAg_paytlcs.trn_amt );
	get_zd_data( "0160", sAg_paytlcs.trcode );
	strcpy( sAg_paytlcs.ckmk_flag, "0" );	
	
	get_zd_data( "0630", sAg_paytlcs.item_no );	/*-- 电话号码 --*/
	strcpy( sAg_paytlcs.pay_month, fldstr[3] );
	strcpy( sAg_paytlcs.pay_way, "0" );		/*-- 联机 --*/
	strcpy( sAg_paytlcs.bill_flag, "0" );
	get_zd_data( "0610", sAg_paytlcs.bill_no );
	strcpy( sAg_paytlcs.stat, "0" );

	ret = Ag_paytlcs_Ins( sAg_paytlcs, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf( acErrMsg, "------>写电信代缴费[%s]记录表出错[%d]", sAg_paytlcs.item_no, ret );
		WRITEMSG
		goto ErrExit;
	}
	
	memset( &sAg_itemlist, 0x00, sizeof(sAg_itemlist) );	
	get_zd_data( "0630", sAg_itemlist.item_no );	/*--- 电话号码 ---*/
	get_zd_data( "0920", sAg_itemlist.unit_no );
	sprintf( sAg_itemlist.bank_tlcs, "%ld", g_pub_tx.trace_no );
	sprintf( sAg_itemlist.trn_date, "%ld", g_pub_tx.tx_date );
	
/*--------------------------------------------------------------------------------
	memset( tmpstr, 0x0, sizeof(tmpstr) );
	memset( filename, 0x0, sizeof(filename) );

	Gl_file_first = 0;
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "r" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		goto ErrExit;
	}
	fgets( tmpstr, 1024, fp );

	fclose( fp );

	memset( fldstr, 0x00, sizeof(fldstr) );
	for( i=0; i<34; i++ )
	{
		ret=pub_base_cut_str( tmpstr,fldstr[i],",",i+1 );
			vtcp_log(" [%d] [%s] ",i+1,fldstr[i]);
		pub_base_strpack( fldstr[i] );
	}
---------------------------------------------------------------------------------*/
	
	strcpy( sAg_itemlist.owe_month, fldstr[3] );
	set_zd_data( "0440", fldstr[3] );	/*	为spH625中取数据	*/

	sAg_itemlist.am00 = atof( fldstr[6] );
	sAg_itemlist.am01 = atof( fldstr[7] );
	sAg_itemlist.am02 = atof( fldstr[8] );
	sAg_itemlist.am03 = atof( fldstr[9] );
	sAg_itemlist.am04 = atof( fldstr[10] );
	sAg_itemlist.am05 = atof( fldstr[11] );
	sAg_itemlist.am06 = atof( fldstr[12] );
	sAg_itemlist.am07 = atof( fldstr[13] );
	sAg_itemlist.am08 = atof( fldstr[14] );
	sAg_itemlist.am09 = atof( fldstr[15] );
	sAg_itemlist.am10 = atof( fldstr[16] );
	sAg_itemlist.am11 = atof( fldstr[17] );
	sAg_itemlist.am12 = atof( fldstr[18] );
	sAg_itemlist.am13 = atof( fldstr[19] );
	sAg_itemlist.am14 = atof( fldstr[20] );
	sAg_itemlist.am15 = atof( fldstr[21] );
	sAg_itemlist.am16 = atof( fldstr[22] );
	sAg_itemlist.am17 = atof( fldstr[23] );
	sAg_itemlist.am18 = atof( fldstr[24] );
	sAg_itemlist.am19 = atof( fldstr[25] );
	sAg_itemlist.am20 = atof( fldstr[26] );
	sAg_itemlist.am21 = atof( fldstr[27] );
	sAg_itemlist.am22 = 0.00;
	sAg_itemlist.am23 = atof( fldstr[29] );
	sAg_itemlist.am24 = atof( fldstr[30] );
	sAg_itemlist.am25 = atof( fldstr[31] );
	sAg_itemlist.am26 = atof( fldstr[32] );
	sAg_itemlist.am27 = atof( fldstr[33] );
	sAg_itemlist.am28 = atof( fldstr[34] );
	sAg_itemlist.am29 = 0.00;
	sAg_itemlist.am30 = sAg_paytlcs.trn_amt;	/*	本次缴费金额	*/
	strcpy( sAg_itemlist.sc00, fldstr[1] );		/*	用户姓名		*/
	strcpy( sAg_itemlist.stcd, "1" );
	
	ret = Ag_itemlist_Ins( sAg_itemlist, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf( acErrMsg, "------>写电信代缴费[%s]明细记录表出错[%d]", sAg_paytlcs.item_no, ret );
		WRITEMSG
		goto ErrExit;
	}

	if ( cFlag[0] == '2' )
	{
		set_zd_data( "102O", "1" );	/*	打印未登折项	*/
	}
	
	Gl_file_first = 1;		/*为了后面文件能够清零后再写*/
	
	memset( cCt_ind, 0x0, sizeof(cCt_ind) );
	memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
	memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
	if ( cFlag[0] == '0' )
	{
		strcpy( cCt_ind, "现金" );
	}
	else
	{
		strcpy( cCt_ind, "转帐" );
		ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
			"ac_no = '%s'", sAg_paytlcs.bank_acct_no );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Mdm_ac_rel_Sel出错!!!" );
			WRITEMSG
			goto ErrExit;
		}

		ret = Dd_mst_Sel( g_pub_tx.reply, &sDd_mst, \
			"ac_id = %ld", sMdm_ac_rel.ac_id );
		if ( ret )
		{
			sprintf(acErrMsg,"执行Dd_mst_Sel出错!!!" );
			WRITEMSG
			goto ErrExit;
		}
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

	pub_base_strpack( sDd_mst.name );
	fprintf( fp, "DLPT代缴话费(%s):%s|%s|%lf|%ld|%s|%s|%s|%ld|%s|%s|\n", \
		cUnit_no, sAg_paytlcs.item_no, fldstr[1], sAg_paytlcs.trn_amt, \
		g_pub_tx.tx_date, g_pub_tx.tel, sAg_paytlcs.bank_acct_no, \
		sDd_mst.name, g_pub_tx.trace_no, cCt_ind, g_pub_tx.tx_br_no );

	fclose( fp );

	Gl_file_first = 0;	/*---为了该交易中以后的写文件不冲掉该内容-----*/

	if ( cFlag[0] == '0' )
	{
		set_zd_data( "101B", "1" );
		goto OkExit1;
	}
	else
	{
		set_zd_data( "101B", "2" );
		set_zd_data( "102K", "2" );
		set_zd_data( "102Y", "缴话费" );
		goto OkExit2;
	}

OkExit1:
	strcpy( g_pub_tx.reply, "0001" );
	sprintf( acErrMsg,"---------->该业务为现金缴费" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
OkExit2:
	strcpy( g_pub_tx.reply, "0002" );
	sprintf(acErrMsg,"---------->该业务为卡折缴费" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"代理单位处理失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
