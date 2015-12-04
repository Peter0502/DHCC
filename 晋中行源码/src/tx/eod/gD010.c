/*************************************************
* 文 件 名: gD010.c
* 功能描述：批处理后生成税务对帐报表
*
* 作    者: jack
* 完成日期: 2003年07月06日
* 修改日期:
* 修 改 人:
* 修改内容:
*
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;}
#define EXTERN
#include "public.h"
#include <errno.h>
#include "ag_unitinfo_c.h"
#include "ag_peritemmap_c.h"
#include "pay_agent_c.h"
#include "ag_paytlcs_c.h"

gD010()
{
	int ret=0;

	char cUnit_no[5];
	
	char filename[64];
	FILE *fp;

	int i=0;

	struct ag_unitinfo_c sAg_unitinfo;
	struct ag_peritemmap_c sAg_peritemmap;
	struct pay_agent_c sPay_agent;
	struct ag_paytlcs_c sAg_paytlcs;

	memset(cUnit_no,0x0,sizeof(cUnit_no));

	strcpy( cUnit_no, "1004" );		/* 暂时定为1004	*/

	ret = sql_begin();
	if ( ret )
	{
		sprintf( acErrMsg, "打开事务失败!!" );
		WRITEMSG
		goto ErrExit;
	}

	/** 初始化全局变量 **/
	pub_base_sysinit();
	
	memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
	ret=Ag_unitinfo_Sel( g_pub_tx.reply , &sAg_unitinfo ,\
			 "unit_no = '%s' and and pl_date = %ld and \
			 unit_stat = '0'", cUnit_no, g_pub_tx.tx_date );
	if( ret==100 )
	{
		sprintf( acErrMsg, "今日非税务单位[%s]批量代扣日", cUnit_no );
		WRITEMSG
		strcpy( g_pub_tx.reply, "0000" );
		return 0;
	}
	else if( ret )
	{
		sprintf( acErrMsg, "执行Ag_unitinfo错误!ret=[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	memset( filename, 0x0, sizeof(filename) );
	sprintf( filename, "%s/Respondsyyh%ld002.txt", \
		getenv("FILDIR"), g_pub_tx.tx_date );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		strcpy( g_pub_tx.reply, "S047" );
		sprintf( acErrMsg, "打开写代扣结果文件错!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = Ag_paytlcs_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
		and stat = '0' and ckmk_flag = '0' \
		order by trn_date,bank_tlcs", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_paytlcs_Dec_Sel错!" );
		WRITEMSG
		goto ErrExit;
	}

	while (1)
	{
		memset( &sAg_paytlcs, 0x00, sizeof(sAg_paytlcs) );
		ret = Ag_paytlcs_Fet_Sel( &sAg_paytlcs, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Ag_paytlcs_Fet_Sel错[%d]!", ret );
			WRITEMSG
			goto ErrExit;
		}

		memset( &sPay_agent, 0x0, sizeof(sPay_agent) );
		ret = Pay_agent_Sel( g_pub_tx.reply, &sPay_agent, \
			"unit_no = '%s' and item_no = '%s'", \
			cUnit_no, sAg_paytlcs.item_no );
		if ( ret )
		{
			sprintf( acErrMsg, "执行Pay_agent_Sel错[%d]!", ret );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sAg_paytlcs.item_no );
		pub_base_strpack( sPay_agent.name );
		pub_base_strpack( sAg_paytlcs.bank_acct_no );
		fprintf( fp, "%s\t%s\t\t%s\t00\t%s\t%lf\n", \
			sAg_paytlcs.item_no, sPay_agent.name, \
			sAg_paytlcs.bank_acct_no, sAg_paytlcs.trn_date, \
			sAg_paytlcs.trn_amt );

		ret = sql_execute( "update ag_paytlcs set ckmk_flag = '1' \
			where unit_no = '%s' and item_no = '%s' \
			and bank_tlcs = '%s'", cUnit_no, sAg_paytlcs.item_no, \
			sAg_paytlcs.bank_tlcs );
		if ( ret )
		{
			sprintf( acErrMsg, "修改银行流水记录中对帐标志 \
				出错[%s][%s][%d]", cUnit_no, \
				sAg_paytlcs.item_no, ret );
			WRITEMSG
			goto ErrExit;
		}
	}

	fclose( fp );

	ret = sql_execute( "update ag_unitinfo set pl_date = 99999999 \
		where unit_no = '%s'", cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "修改单位[%s]信息中批量日期出错[%d]", \
			cUnit_no, ret );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from pay_agent \
		where unit_no = '%s' and result = '0'", \
		cUnit_no );		/*----删除所有已缴费的记录----*/
	if ( ret )
	{
		sprintf( acErrMsg, "删除单位[%s]未缴记录失败!!!", cUnit_no );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sql_commit();
    strcpy(g_pub_tx.reply,"0000");
    sprintf(acErrMsg,"----->生成税务对帐报表成功！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 0;
    
ErrExit:
	fclose( fp );
	sql_rollback();
    sprintf(acErrMsg,"----->生成税务对帐报表失败！[%s]",g_pub_tx.reply);
    WRITEMSG
    set_zd_data(DC_REPLY,g_pub_tx.reply);
    return 1;
}
