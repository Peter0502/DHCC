/***************************************************************
* 文 件 名:     spH608.c
* 功能描述：	个人编号密码重置
*
* 作    者:     jack
* 完成日期：    2004年1月30日
*
* 重置记录：
* 日    期:
* 修 改 人:
* 重置内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"
#include "ag_peracctinfo_c.h"

spH608()
{
	int ret = 0;

	char old_pwd[7];
	char new_pwd[7];
	char per_no[13];
	char acct_no[26];
	char ac_pwd[7];

	struct ag_peracctinfo_c sPerAcctInfo; /*个人信息表*/

	memset(&sPerAcctInfo,0x0,sizeof(sPerAcctInfo));
	memset(old_pwd,0x0,sizeof(old_pwd));
	memset(new_pwd,0x0,sizeof(new_pwd));
	memset(per_no,0x0,sizeof(per_no));
	memset(acct_no,0x0,sizeof(acct_no));
	memset(ac_pwd,0x0,sizeof(ac_pwd));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*-------- 取输入变量  ---------*/
	get_zd_data("0590",per_no);
	get_zd_data("0800",new_pwd);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0300",acct_no);

	/*---  到mdm_ac_rel中判断账户密码的正确性  ---*
	ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
	{
		sprintf(acErrMsg,"账户支取密码错!!![%s]",g_pub_tx.reply);
		WRITEMSG
	}
	---------------------------------------------*/

	ret = Ag_peracctinfo_Dec_Upd( g_pub_tx.reply , "per_no = '%s'",per_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"重置个人编号密码错1[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	ret = Ag_peracctinfo_Fet_Upd( &sPerAcctInfo, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"重置个人编号错密码2[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	strcpy(sPerAcctInfo.per_pwd,new_pwd);

	ret = Ag_peracctinfo_Upd_Upd( sPerAcctInfo, g_pub_tx.reply );
	if (ret != 0)
	{
		sprintf(acErrMsg,"重置个人编号错密码3[%s]",per_no);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H005");
		goto ErrExit;
	}

	ret = Ag_peracctinfo_Clo_Upd( );

	/*** 输出变量 ***/
	set_zd_data("0590",per_no);
OkExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"重置个人编号密码成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	ret = Ag_peracctinfo_Clo_Upd( );
	sprintf(acErrMsg,"重置个人编号密码失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
