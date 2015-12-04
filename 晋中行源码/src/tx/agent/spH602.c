/***************************************************************
* 文 件 名:     spH602.c
* 功能描述：	个人编号删除/修改
*
* 作    者:     jack
* 完成日期：    2004年1月28日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"

spH602()
{
	int ret = 0;
	char proc_flag[2];
	char tbwhere[128];

	char per_name[51];
	char ac_type[2];
	char acct_no[26];
	char ac_pwd[7];
	char per_pwd[7];
	char per_no[13];

	struct ag_peracctinfo_c sPerAcctInfo; /*个人信息表*/

	memset(proc_flag,0,sizeof(proc_flag));
	memset(tbwhere,0,sizeof(tbwhere));

	memset(&sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset(per_name,0x0,sizeof(per_name));
	memset(per_pwd,0x0,sizeof(per_pwd));
	memset(per_no,0x0,sizeof(per_no));
	memset(ac_type,0x0,sizeof(ac_type));
	memset(acct_no,0x0,sizeof(acct_no));
	memset(ac_pwd,0x0,sizeof(ac_pwd));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("0700",proc_flag);

	get_zd_data("0250",per_name);
	get_zd_data("0590",per_no);
	get_zd_data("0670",ac_type);
	get_zd_data("0300",acct_no);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0800",per_pwd);

	if (proc_flag[0] == '2')	/*--- 删除 ---*/
	{
		sprintf(tbwhere,"per_no = '%s' ",per_no);
		ret = deldb("ag_peracctinfo",tbwhere);
		if (ret != 0)
		{
			sprintf(acErrMsg,"删除个人编号错[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H004");
			goto ErrExit;
		}
	}
	else if (proc_flag[0] == '1')	/*--- 修改 ---*/
	{
		/*---  到mdm_ac_rel中判断账户密码的正确性  ---*
		ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
		{
			sprintf(acErrMsg,"账户支取密码错!!![%s]",g_pub_tx.reply);
			WRITEMSG
		}
		---------------------------------------------*/

		ret = Ag_peracctinfo_Dec_Upd( g_pub_tx.reply , "per_no = '%s' and per_pwd = '%s'",per_no,per_pwd);
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改个人编号错1[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		ret = Ag_peracctinfo_Fet_Upd( &sPerAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改个人编号错2[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		strcpy(sPerAcctInfo.per_no,per_no);
		strcpy(sPerAcctInfo.per_name,per_name);
		strcpy(sPerAcctInfo.per_stat,"0");
		strcpy(sPerAcctInfo.acct_type,ac_type);
		strcpy(sPerAcctInfo.bank_acct_no,acct_no);
		strcpy(sPerAcctInfo.per_pwd,per_pwd);
		strcpy(sPerAcctInfo.opn_br_no,g_pub_tx.tx_br_no);
		sprintf(sPerAcctInfo.opn_date,"%ld",g_pub_tx.tx_date);
		strcpy(sPerAcctInfo.opn_tel,g_pub_tx.tel);
		strcpy(sPerAcctInfo.cls_date,"000000");
		strcpy(sPerAcctInfo.cls_tel,"0000");

		ret = Ag_peracctinfo_Upd_Upd( sPerAcctInfo, g_pub_tx.reply );
		if (ret != 0)
		{
			sprintf(acErrMsg,"修改个人编号错3[%s]",per_no);
			WRITEMSG
			strcpy(g_pub_tx.reply,"H005");
			goto ErrExit;
		}

		ret = Ag_peracctinfo_Clo_Upd( );
	}
	else
	{
		sprintf(acErrMsg,"处理类型错[%s]",proc_flag);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H006");
		goto ErrExit;
	}

	/*** 输出变量 ***/
	set_zd_data("0590",per_no);
OkExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"维护个人编号信息成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	sprintf(acErrMsg,"维护个人编号信息失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
