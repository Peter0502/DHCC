/*************************************************************
* 文 件 名: gD007.c
* 功能描述：财政记账余额核对
* 把财政记账文件中记录的余额和总帐的余额核对，结果写在报告里
**************************************************************/
#define EXTERN
#include <stdio.h>
#include "public.h"
#include "com_sys_parm_c.h"
#include "com_branch_c.h"
#include "fin_acct_c.h"
#include "gl_mst_c.h"
#include "com_item_c.h"

gD007()
{
	int i=0, j=0;
	struct com_sys_parm_c sComSysParm;
	struct com_branch_c sComBranch;
	struct fin_acct_c sFinAcct;
	struct gl_mst_c sGlMst;
	struct com_item_c sComItem;

	memset( &sComSysParm, 0x00, sizeof( struct com_sys_parm_c ) );
	memset( &sComBranch, 0x00, sizeof( struct com_branch_c ) );

    g_reply_int=sql_begin(); /*打开事务*/
	if( g_reply_int )
	{
		sprintf(acErrMsg, "数据库操作 打开事务失败!");
		WRITEMSG
		goto ErrExit;
	}

	g_reply_int = pub_base_GetSysparm( &sComSysParm );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "取公共系统参数表错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}
	g_reply_int = Com_branch_Sel( g_pub_tx.reply, &sComBranch, "br_type='1'" );
	if ( g_reply_int )
	{
		sprintf( acErrMsg, "取清算中心机构号错![%ld]", g_reply_int );
		WRITEMSG
		goto ErrExit;			
	}

	fin_report( "---------- %ld - 翻牌后核对余额开始 ----------", sComSysParm.lst_date );

	g_reply_int = Fin_acct_Dec_Sel( g_pub_tx.reply, "tx_date='%ld' and acct_flag='1'", sComSysParm.lst_date );
	if( g_reply_int )
	{
		sprintf(acErrMsg, "财政记帐核对余额定义游标错误![%ld]", g_reply_int );
		WRITEMSG
		return -1;
	}

	while(1)
	{
		memset( &sFinAcct, 0x00, sizeof( struct fin_acct_c ) );
		memset( &sGlMst, 0x00, sizeof( struct gl_mst_c ) );
		memset( &sComItem, 0x00, sizeof( struct com_item_c ) );
		g_reply_int = Fin_acct_Fet_Sel( &sFinAcct, g_pub_tx.reply );
		if( g_reply_int == 100 ) break;
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "财政记帐核对余额取游标错误![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		i++;
		g_reply_int = Com_item_Sel( g_pub_tx.reply, &sComItem, "acc_hrt='%s'", sFinAcct.ac_no );
		if( g_reply_int == 100 )
		{
			fin_report( " %3d 【科目号】%7s 不存在!", i, sFinAcct.ac_no );
			vtcp_log( " %3d 【科目号】%7s 不存在!", i, sFinAcct.ac_no );
			continue;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "财政记帐核对余额取科目控制字出错![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}
		g_reply_int = Gl_mst_Sel( g_pub_tx.reply, &sGlMst, 
			"br_no='%s' and cur_no='01' and acc_hrt='%s'", sComBranch.br_no, sComItem.acc_hrt );
		if( g_reply_int == 100 )
		{
			fin_report( " %3d 【科目控制字】%5s 不存在!", i, sComItem.acc_hrt );
			vtcp_log( " %3d 【科目控制字】%5s 不存在!", i, sComItem.acc_hrt );
			continue;
		}
		else if( g_reply_int )
		{
			sprintf(acErrMsg, "财政记帐核对余额取对应总账记录出错![%ld]", g_reply_int );
			WRITEMSG
			return -1;
		}

		pub_base_strpack( sFinAcct.ac_no );
		TRACE;
		/* 借方 贷方 双向 */
		if( ( sGlMst.dc_ind[0] == '1' && sGlMst.dr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '2' && sGlMst.cr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '0' && sFinAcct.bal >= 0 && sGlMst.dr_bal == sFinAcct.bal ) || 
			( sGlMst.dc_ind[0] == '0' && sFinAcct.bal < 0 && sGlMst.cr_bal == -sFinAcct.bal ) )
		{
			fin_report( "   %3d 【科目号】%7s 余额相符 借方余额: %.2lf", i, sFinAcct.ac_no, sGlMst.dr_bal );
			fin_report( "                                  贷方余额: %.2lf", sGlMst.cr_bal );
		}
		else
		{
			fin_report( "** %3d 【科目号】%7s 余额不符 借方余额: %.2lf", i, sFinAcct.ac_no, sGlMst.dr_bal );
			fin_report( "                                  贷方余额: %.2lf", sGlMst.cr_bal );
			fin_report( "                            记账文件中余额: %.2lf", sFinAcct.bal );
		}
	}
	Fin_acct_Clo_Sel();

OkExit:
	sql_commit();	/*--提交事务--*/
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"财政记账核对余额成功![%s]",g_pub_tx.reply);
	WRITEMSG
	fin_report( "---------- %ld - 财政记账核对余额成功结束 ----------", g_pub_tx.tx_date );
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sql_rollback();	/*--事务回滚--*/
	sprintf(acErrMsg,"财政记账核对余额失败![%s]",g_pub_tx.reply);
	WRITEMSG
	fin_report( "---------- %ld - 财政记账核对余额失败结束 ----------", g_pub_tx.tx_date );
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
