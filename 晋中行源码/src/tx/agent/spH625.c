/***************************************************************
* 文 件 名: spH625.c
* 功能描述：柜台补打电信发票
*
* 作    者: jack
* 完成日期：2004年03月29日
*
* 修改记录：
* 日    期: 2004年7月5日
* 修 改 人: jack
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_itemlist_c.h"
#include "ag_paytlcs_c.h"

spH625()
{
	int ret = 0;
	int i = 0;
	int iNum = 0;
	char cItem_no[21];
	char cUnit_no[5];
	char cPay_month[7];
	char cTrcode[5];
	char cRcno[17];
	char cRcno1[11];

	char cFileName[128];
	FILE *fp;
	
	char cFlag[2];

	char tmp_fldstr[32][64];
	char fldstr[32][64];
	char cTmpName[32];
	double dAmt = 0.00;

	struct ag_paytlcs_c sAg_paytlcs;
	struct ag_itemlist_c sAg_itemlist; /*缴费信息明细表*/

	memset(cItem_no,0x0,sizeof(cItem_no));
	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cPay_month,0x0,sizeof(cPay_month));

	memset( cFileName, 0x00, sizeof( cFileName ) );

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0630", cItem_no );
	get_zd_data( "0440", cPay_month );
	
	memset( cTrcode, 0x0, sizeof(cTrcode) );
	get_zd_data( "0160", cTrcode );

	sprintf( acErrMsg, "------>[%s][%s][%s]", cUnit_no, cItem_no, cPay_month );
	WRITEMSG

	if ( strcmp( cTrcode, "6704" ) == 0 )
	{
		memset(&sAg_paytlcs,0x0,sizeof(sAg_paytlcs));
		ret = Ag_paytlcs_Sel( g_pub_tx.reply , &sAg_paytlcs, \
			"unit_no = '%s' and item_no = '%s' and pay_month = '%s' \
			and bill_flag = '0'", cUnit_no, cItem_no, cPay_month );
		if( ret )
		{
			sprintf( acErrMsg, "需要打印的发票记录不存在!!!ret=[%d]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	memset(&sAg_itemlist,0x0,sizeof(sAg_itemlist));
	ret = Ag_itemlist_Sel( g_pub_tx.reply , &sAg_itemlist, \
		"item_no = '%s' and unit_no = '%s' and stcd = '1' \
		and owe_month = '%s'", cItem_no, cUnit_no, cPay_month );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_itemlist_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	memset( tmp_fldstr, 0x0, sizeof(tmp_fldstr) );
	sprintf( tmp_fldstr[0], "%lf", sAg_itemlist.am00 );
	sprintf( tmp_fldstr[1], "%lf", sAg_itemlist.am01 );
	sprintf( tmp_fldstr[2], "%lf", sAg_itemlist.am02 );
	sprintf( tmp_fldstr[3], "%lf", sAg_itemlist.am03 );
	sprintf( tmp_fldstr[4], "%lf", sAg_itemlist.am04 );
	sprintf( tmp_fldstr[5], "%lf", sAg_itemlist.am05 );
	sprintf( tmp_fldstr[6], "%lf", sAg_itemlist.am06 );
	sprintf( tmp_fldstr[7], "%lf", sAg_itemlist.am07 );
	sprintf( tmp_fldstr[8], "%lf", sAg_itemlist.am08 );
	sprintf( tmp_fldstr[9], "%lf", sAg_itemlist.am09 );
	sprintf( tmp_fldstr[10], "%lf", sAg_itemlist.am10 );
	sprintf( tmp_fldstr[11], "%lf", sAg_itemlist.am11 );
	sprintf( tmp_fldstr[12], "%lf", sAg_itemlist.am12 );
	sprintf( tmp_fldstr[13], "%lf", sAg_itemlist.am13 );
	sprintf( tmp_fldstr[14], "%lf", sAg_itemlist.am14 );
	sprintf( tmp_fldstr[15], "%lf", sAg_itemlist.am15 );
	sprintf( tmp_fldstr[16], "%lf", sAg_itemlist.am16 );
	sprintf( tmp_fldstr[17], "%lf", sAg_itemlist.am17 );
	sprintf( tmp_fldstr[18], "%lf", sAg_itemlist.am18 );
	sprintf( tmp_fldstr[19], "%lf", sAg_itemlist.am19 );
	sprintf( tmp_fldstr[20], "%lf", sAg_itemlist.am20 );
	sprintf( tmp_fldstr[21], "%lf", sAg_itemlist.am21 );

	memset( cFlag, 0x0, sizeof(cFlag) );
	get_zd_data( "0670", cFlag );
	if ( (cFlag[0] != '1') && (cFlag[0] != '2') )
	{
		Gl_file_first = 1;	/*暂定现金只打电信发票*/
	}
	Gl_file_first = 0;
	
	if ( strcmp( cTrcode, "6704") == 0 )
	{
		Gl_file_first = 1;
	}
	ret = pub_base_AllDwnFilName( cFileName );
	if( ret )
	{
		sprintf( acErrMsg, "生成下传全路经文件名(批量)错" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "a" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "临时文件错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}
	
	if ( cFlag[0] == '0' )	/*现金交易*/
	{
		/*打印凭条内容待定*/
	}

	pub_base_strpack( sAg_itemlist.sc00 );
	pub_base_strpack( sAg_itemlist.item_no );
	pub_base_strpack( sAg_itemlist.trn_date );
	fprintf( fp, "DXTT%s|%s|%s|\n", sAg_itemlist.sc00, \
				sAg_itemlist.item_no, sAg_itemlist.trn_date );

	memset( fldstr, 0x0, sizeof(fldstr) );
	for ( i=0; i<=21; i++ )
	{
		dAmt = atof( tmp_fldstr[i] );

		ret = pub_base_CompDblVal( 0.00, dAmt );
		if ( ret )
		{
			if ( i == 0 )
			{
				sprintf( fldstr[iNum], "月租费:%.2f", dAmt );
			}
			if ( i == 1 )
			{
				sprintf( fldstr[iNum], "新业务费:%.2f", dAmt );
			}
			if ( i == 2 )
			{
				sprintf( fldstr[iNum], "分期付款费:%.2f", dAmt );
			}
			if ( i == 3 )
			{
				sprintf( fldstr[iNum], "BP机分期付款:%.2f", dAmt );
			}
			if ( i == 4 )
			{
				sprintf( fldstr[iNum], "110联动:%.2f", dAmt );
			}
			if ( i == 5 )
			{
				sprintf( fldstr[iNum], "其他固定:%.2f", dAmt );
			}
			if ( i == 6 )
			{
				sprintf( fldstr[iNum], "市话费:%.2f", dAmt );
			}
			if ( i == 7 )
			{
				sprintf( fldstr[iNum], "长话费:%.2f", dAmt );
			}
			if ( i == 8 )
			{
				sprintf( fldstr[iNum], "本地网费:%.2f", dAmt );
			}
			if ( i == 9 )
			{
				sprintf( fldstr[iNum], "17909费:%.2f", dAmt );
			}
			if ( i == 10 )
			{
				sprintf( fldstr[iNum], "上网费:%.2f", dAmt );
			}
			if ( i == 11 )
			{
				sprintf( fldstr[iNum], "17989话费:%.2f", dAmt );
			}
			if ( i == 12 )
			{
				sprintf( fldstr[iNum], "主叫BP费:%.2f", dAmt );
			}
			if ( i == 13 )
			{
				sprintf( fldstr[iNum], "电报费:%.2f", dAmt );
			}
			if ( i == 14 )
			{
				sprintf( fldstr[iNum], "专线费:%.2f", dAmt );
			}
			if ( i == 15 )
			{
				sprintf( fldstr[iNum], "800费:%.2f", dAmt );
			}
			if ( i == 16 )
			{
				sprintf( fldstr[iNum], "声讯费:%.2f", dAmt );
			}
			if ( i == 17 )
			{
				sprintf( fldstr[iNum], "其他费:%.2f", dAmt );
			}
			if ( i == 18 )
			{
				sprintf( fldstr[iNum], "税金:%.2f", dAmt );
			}
			if ( i == 19 )
			{
				sprintf( fldstr[iNum], "利息:%.2f", dAmt );
			}
			if ( i == 20 )
			{
				sprintf( fldstr[iNum], "违约金:%.2f", dAmt );
			}
			if ( i == 21 )
			{
				sprintf( fldstr[iNum], "欠费:%.2f", dAmt );
			}

			iNum = iNum + 1;
		}
	}

	fprintf( fp, "DXXH" );
	for ( i=1; i<=iNum; i++ )
	{
		fprintf( fp, "%s|", fldstr[i-1] );
		if ( i % 3 == 0 )
		{
			if ( i != iNum )
			{
				fprintf( fp, "\n" );
				fprintf( fp, "DXXH" );
			}
		}
	}

	dAmt = 0.00;	/* 用来计算本次节余	*/
	dAmt = sAg_itemlist.am25 + sAg_itemlist.am23 - sAg_itemlist.am24;
	fprintf( fp, "\nDXTW%.2lf|%.2lf|%.2lf|%s|%.2lf|%.2lf|\n", sAg_itemlist.am24, \
		sAg_itemlist.am23, sAg_itemlist.am19, sAg_itemlist.owe_month, \
		sAg_itemlist.am25, dAmt );

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );
	

	if ( strcmp( cTrcode, "6704" ) == 0 )	/*该交易为补打发票交易*/
	{
		memset( cRcno, 0x0, sizeof(cRcno) );
		get_zd_data( "0610", cRcno );
		ret = pub_com_NoteUse( 0, 0, "142", cRcno, cRcno, g_pub_tx.tel);
		if ( ret )
		{
			sprintf(acErrMsg,"--------->销柜员凭证错误!");
			WRITEMSG
			goto ErrExit;
		}
		
		memset( cRcno1, 0x0, sizeof(cRcno1) );
		memcpy( cRcno1, cRcno+6, 10 );
		
		ret = sql_execute( "update ag_paytlcs set bill_no = '%s' where \
		unit_no = '%s' and item_no = '%s' and pay_month = '%s' \
		and bill_flag = '0'", cRcno1, cUnit_no, cItem_no, cPay_month);
		if( ret )
		{
			sprintf( acErrMsg, "修改打印发票标志错!!!ret=[%s]", ret);
			WRITEMSG
			goto ErrExit;
		}
	}

	ret = sql_execute( "update ag_paytlcs set bill_flag = '1' where \
		unit_no = '%s' and item_no = '%s' and pay_month = '%s' \
		and bill_flag = '0'", cUnit_no, cItem_no, cPay_month);
	if( ret )
	{
		sprintf( acErrMsg, "修改打印发票标志错!!!ret=[%s]", ret);
		WRITEMSG
		goto ErrExit;
	}

	set_zd_double( "0390", sAg_itemlist.am30 );

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"-------->发票补打成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"-------->发票补打失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
