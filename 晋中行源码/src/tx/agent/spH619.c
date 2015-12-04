/***************************************************************
* 文 件 名:     spH619.c
* 功能描述：	单位临时账号维护
*
* 作    者:     jack
* 完成日期：    2004年1月31日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_unitacctmap_c.h"
#include "ag_peracctinfo_c.h"

spH619()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char cUnit_no[5];
	char cBr_no[6];
	char cAcct_type[2];
	char cAcct_no[26];

	struct ag_unitacctmap_c sUnitAcctInfo; /*单位临时账号对应表*/

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sUnitAcctInfo,0x0,sizeof(sUnitAcctInfo));

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cBr_no,0x0,sizeof(cBr_no));
	memset(cAcct_type,0x0,sizeof(cAcct_type));
	memset(cAcct_no,0x0,sizeof(cAcct_type));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0910",cBr_no);
	get_zd_data("0670",cAcct_type);
	get_zd_data("0300",cAcct_no);
	pub_base_old_acno( cAcct_no );
	set_zd_data("0300",cAcct_no);

	get_zd_data("0920",cUnit_no);

	if (proc_flag[0] == '2')	/*--- 删除 ---*/
	{
		sprintf(tbwhere,"unit_no = '%s' and br_no = '%s' and stat = '0'",cUnit_no,cBr_no);
		ret = deldb("ag_unitacctmap",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"删除单位临时账号信息记录错[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
	}
	else if (proc_flag[0] == '1')	/*--- 修改 ---*/
	{

		ret = Ag_unitacctmap_Dec_Upd( g_pub_tx.reply , "unit_no = '%s' and br_no = '%s'",cUnit_no, cBr_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位临时账号信息错1[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_unitacctmap_Fet_Upd( &sUnitAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位临时账号信息错2[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		strcpy(sUnitAcctInfo.unit_no,cUnit_no);
		strcpy(sUnitAcctInfo.br_no,cBr_no);
		strcpy(sUnitAcctInfo.acct_type,cAcct_type);
		strcpy(sUnitAcctInfo.bank_acct_no,cAcct_no);
		strcpy(sUnitAcctInfo.stat,"0");

		ret = Ag_unitacctmap_Upd_Upd( sUnitAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位临时账号信息错3[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_unitacctmap_Clo_Upd( );
	}
	else if (proc_flag[0] == '0')
	{
		strcpy(sUnitAcctInfo.unit_no,cUnit_no);
		strcpy(sUnitAcctInfo.br_no,cBr_no);
		strcpy(sUnitAcctInfo.acct_type,cAcct_type);
		strcpy(sUnitAcctInfo.bank_acct_no,cAcct_no);
		strcpy(sUnitAcctInfo.stat,"0");

		ret = Ag_unitacctmap_Ins(sUnitAcctInfo,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"插入单位临时账号信息表错[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
	}
	else
	{
		sprintf(acErrMsg,"处理类型错[%s]",proc_flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H006");
		goto ErrExit;
	}

	/*** 输出变量 ***/
	set_zd_data("0590",cUnit_no);
OkExit:
	ret = Ag_unitacctmap_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"维护单位临时账号信息信息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_unitacctmap_Clo_Upd( );
	sprintf(acErrMsg,"维护单位临时账号信息失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
