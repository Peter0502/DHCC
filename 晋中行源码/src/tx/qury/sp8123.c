/***************************************************************
* 文 件 名: sp8123.c
* 功能描述：客户存款账户异动情况查询
* 作    者: jack
* 日	期: 20031115
***************************************************************/

#define EXTERN
#include <stdio.h>
#include "public.h"
#include "mo_jl_bal_c.h"

sp8123()
{   
    int ret=0;
	double dTram = 0.00;
	long lBeg_date = 0;
	long lEnd_date = 0;

	double dAcbl = 0.00;	/*余额变化*/
	double dBacbl = 0.00;	/*其实余额*/
	double dEacbl = 0.00;	/*终止余额*/
	
	char filename[128];
	FILE *fp = NULL;

	struct dd_mst_c sDd_mst;
	struct td_mst_c sTd_mst;
	struct mdm_ac_rel_c sMdm_ac_rel;
	struct mo_jl_bal_c sMo_jl_bal;
	
	pub_base_sysinit();

	get_zd_double( "0400", &dTram );
	get_zd_long( "0440", &lBeg_date );
	get_zd_long( "0450", &lEnd_date );

    sprintf( acErrMsg,"－－－>[%.2lf][%ld][%ld][%s]", dTram, lBeg_date, lEnd_date, g_pub_tx.tx_br_no );
    WRITEMSG
	
	pub_base_AllDwnFilName( filename );
	fp = fopen( filename,"w" );
     	if( fp==NULL )
     	{
        	sprintf(acErrMsg," 打开写账户异动信息文件 [%s] error ",filename );
        	WRITEMSG
        	strcpy( g_pub_tx.reply,"S047" );
        	goto ErrExit;
     	}
     	
     	fprintf( fp, "~@账号|$异动金额|$起始日余额|$终止日余额|@户名|\n" );

	ret = Dd_mst_Dec_Sel( g_pub_tx.reply , "opn_br_no = '%s'", \
			g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Dd_mst_Dec_Sel错[%s]", g_pub_tx.reply );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sDd_mst, 0x0, sizeof(sDd_mst) );
		ret = Dd_mst_Fet_Sel( &sDd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Dd_mst_Fet_Sel错[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		/*---------先取起始日期活期余额----------*/
		memset( &sMo_jl_bal, 0x0, sizeof(sMo_jl_bal) );
		ret = Mo_jl_bal_Sel( g_pub_tx.reply , &sMo_jl_bal, \
			"ac_id = %ld and beg_date <= %ld and end_date >= %ld", \
			sDd_mst.ac_id, lBeg_date, lBeg_date );
		if ( ret != 0 && ret != 100 )
		{
			sprintf( acErrMsg, "查询活期余额变化[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		dBacbl = sMo_jl_bal.bal;

		/*---------再取终止日期活期余额----------*/
		memset( &sMo_jl_bal, 0x0, sizeof(sMo_jl_bal) );
		ret = Mo_jl_bal_Sel( g_pub_tx.reply , &sMo_jl_bal, \
			"ac_id = %ld and beg_date <= %ld and end_date >= %ld", \
			sDd_mst.ac_id, lEnd_date, lEnd_date );
		if ( ret != 0 && ret != 100 )
		{
			sprintf( acErrMsg, "查询活期余额变化[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		dEacbl = sMo_jl_bal.bal;

		ret = pub_base_CompDblVal( dBacbl, dEacbl );
		if ( ret >=0 )
		{
			dAcbl = dBacbl - dEacbl;
		}
		else
		{
			dAcbl = dEacbl - dBacbl;
		}

		ret = pub_base_CompDblVal( dAcbl, dTram );
		if ( ret >= 0 )
		{
			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_id = %ld ", sDd_mst.ac_id );
			if ( ret )
			{
				sprintf( acErrMsg, "查询介质账号对照表错[%s][%s]", \
					sMo_jl_bal.ac_id, g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			
			/*--------打印输出记录--------*/
			pub_base_strpack( sDd_mst.name );
			fprintf( fp, "%s|%.2lf|%.2lf|%.2lf|%s|\n", sMdm_ac_rel.ac_no, \
					dAcbl, dBacbl, dEacbl, sDd_mst.name );
		}
	}

	Dd_mst_Clo_Sel();	
	
	/*------------------定期处理----------------------------*/
	ret = Td_mst_Dec_Sel( g_pub_tx.reply , "opn_br_no = '%s'", \
			g_pub_tx.tx_br_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Td_mst_Dec_Sel错[%s]", g_pub_tx.reply );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sTd_mst, 0x0, sizeof(sTd_mst) );
		ret = Td_mst_Fet_Sel( &sTd_mst, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Td_mst_Fet_Sel错[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		/*---------先取起始日期定期余额----------*/
		memset( &sMo_jl_bal, 0x0, sizeof(sMo_jl_bal) );
		ret = Mo_jl_bal_Sel( g_pub_tx.reply , &sMo_jl_bal, \
			"ac_id = %ld and beg_date <= %ld and end_date >= %ld", \
			sTd_mst.ac_id, lBeg_date, lBeg_date );
		if ( ret != 0 && ret != 100 )
		{
			sprintf( acErrMsg, "查询定期余额变化[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		dBacbl = sMo_jl_bal.bal;

		/*---------再取终止日期定期余额----------*/
		memset( &sMo_jl_bal, 0x0, sizeof(sMo_jl_bal) );
		ret = Mo_jl_bal_Sel( g_pub_tx.reply , &sMo_jl_bal, \
			"ac_id = %ld and beg_date <= %ld and end_date >= %ld", \
			sTd_mst.ac_id, lEnd_date, lEnd_date );
		if ( ret != 0 && ret != 100 )
		{
			sprintf( acErrMsg, "查询定期余额变化[%s]", g_pub_tx.reply );
			WRITEMSG
			goto ErrExit;
		}

		dEacbl = sMo_jl_bal.bal;

		ret = pub_base_CompDblVal( dBacbl, dEacbl );
		if ( ret >=0 )
		{
			dAcbl = dBacbl - dEacbl;
		}
		else
		{
			dAcbl = dEacbl - dBacbl;
		}

		ret = pub_base_CompDblVal( dAcbl, dTram );
		if ( ret >= 0 )
		{
			memset( &sMdm_ac_rel, 0x0, sizeof(sMdm_ac_rel) );
			ret = Mdm_ac_rel_Sel( g_pub_tx.reply, &sMdm_ac_rel, \
				"ac_id = %ld", sTd_mst.ac_id );
			if ( ret )
			{
				sprintf( acErrMsg, "查询介质账号对照表错[%s][%s]", \
					sMo_jl_bal.ac_id, g_pub_tx.reply );
				WRITEMSG
				goto ErrExit;
			}
			/*--------打印输出记录--------*/
			pub_base_strpack( sTd_mst.name );
			fprintf( fp, "%s|%.2lf|%.2lf|%.2lf|%s|\n", sMdm_ac_rel.ac_no, \
					dAcbl, dBacbl, dEacbl, sTd_mst.name );
		}

	}

	Td_mst_Clo_Sel();	
	
	fclose( fp );
	
	set_zd_data( DC_FILE_SND, "1" );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose( fp );
	sprintf(acErrMsg, "查询账户异动情况出错[%s]", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

