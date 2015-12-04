/***************************************************************
* 文 件 名:     spH618.c
* 功能描述：	单位信息维护
*
* 作    者:     jack
* 完成日期：    2004年1月30日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"
#include "ag_peracctinfo_c.h"

spH618()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char cUnit_no[5];
	char cUnit_name[51];
	char cAcct_type[2];
	char cAcct_no[26];

	struct ag_unitinfo_c sUnitInfo;	  /*单位信息控制表*/

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sUnitInfo,0x0,sizeof(sUnitInfo));

	memset(cUnit_no,0x0,sizeof(cUnit_no));
	memset(cUnit_name,0x0,sizeof(cUnit_name));
	memset(cAcct_type,0x0,sizeof(cAcct_type));
	memset(cAcct_no,0x0,sizeof(cAcct_type));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0250",cUnit_name);
	get_zd_data("0670",cAcct_type);

	get_zd_data("0300",cAcct_no);
	pub_base_old_acno( cAcct_no );
	set_zd_data("0300",cAcct_no);

	if (proc_flag[0] == '2')	/*--- 删除 ---*/
	{
		get_zd_data("0920",cUnit_no);
		sprintf(tbwhere,"unit_no = '%s'",cUnit_no);
		ret = deldb("ag_unitinfo",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"删除单位信息记录错[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}
	}
	else if (proc_flag[0] == '1')	/*--- 修改 ---*/
	{

		get_zd_data("0920",cUnit_no);
			sprintf(acErrMsg,"修改单位信息的单位编号未[%s]",cUnit_no);
			WRITEMSG
		ret = Ag_unitinfo_Dec_Upd( g_pub_tx.reply , "unit_no = '%s'",cUnit_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位信息错1[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_unitinfo_Fet_Upd( &sUnitInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位信息错2[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		strcpy(sUnitInfo.unit_no,cUnit_no);
		strcpy(sUnitInfo.unit_name,cUnit_name);
		strcpy(sUnitInfo.acct_type,cAcct_type);
		strcpy(sUnitInfo.bank_acct_no,cAcct_no);
		strcpy(sUnitInfo.unit_stat,"0");

		ret = Ag_unitinfo_Upd_Upd( sUnitInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改单位信息错3[%s]",cUnit_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H008");
			goto ErrExit;
		}

		ret = Ag_unitinfo_Clo_Upd( );
	}
	else if (proc_flag[0] == '0')
	{
		ret = pub_agent_unitno(cUnit_no);
		if (ret != 0)
		{
			sprintf(acErrMsg,"生成单位编号错!!!");
			WRITEMSG
			goto ErrExit;
		}
		strcpy(sUnitInfo.unit_no,cUnit_no);
		strcpy(sUnitInfo.unit_name,cUnit_name);
		strcpy(sUnitInfo.acct_type,cAcct_type);
		strcpy(sUnitInfo.bank_acct_no,cAcct_no);
		strcpy(sUnitInfo.unit_stat,"0");

		ret = Ag_unitinfo_Ins(sUnitInfo,g_pub_tx.reply);
		if (ret != 0)
		{
			sprintf(acErrMsg,"插入单位信息表错[%s]",cUnit_no);
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
	set_zd_data("0920",cUnit_no);
OkExit:
	ret = Ag_unitinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"维护单位信息信息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_unitinfo_Clo_Upd( );
	sprintf(acErrMsg,"维护单位信息失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
