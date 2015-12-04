/***************************************************************
* 文 件 名:     spH601.c
* 功能描述：	个人编号建立
*
* 作    者:     jack
* 完成日期：    2004年1月27日
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

spH601()
{
	int ret = 0;
	char per_name[51];
	char ac_type[2];
	char acct_no[26];
	char ac_pwd[7];
	char per_pwd[7];
	char per_no[13];

	struct ag_peracctinfo_c sPerAcctInfo; /*个人信息表*/

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
	get_zd_data("0250",per_name);
	get_zd_data("0670",ac_type);
	get_zd_data("0300",acct_no);
	get_zd_data("0790",ac_pwd);
	get_zd_data("0800",per_pwd);

	/*---  到mdm_ac_rel中判断账户密码的正确性  ---*
	ret = pub_base_check_drawtype(acct_no,"","",ac_pwd);
	{
		sprintf(acErrMsg,"账户支取密码错!!![%s]",g_pub_tx.reply);
		WRITEMSG
	}
	---------------------------------------------*/

	/*---  检查该账号和户名对应的个人编号是否存在  ---*/
	ret = sql_count("ag_peracctinfo", "per_name = '%s' and bank_acct_no = '%s' and per_stat ='0'", per_name, acct_no);
	if (ret > 0)
	{
		sprintf(acErrMsg,"该账号户名对应的个人资料已存在!!![%s]",g_pub_tx.reply);
		WRITEMSG
		strcpy(g_pub_tx.reply,"H001");
		goto ErrExit;
	}

	/*---  生成个人编号  ---*/
	ret = pub_agent_perno(per_no);
	if (ret != 0)
	{
		sprintf(acErrMsg,"生成个人编号出错![%s]",g_pub_tx.reply);
		WRITEMSG
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

	ret = Ag_peracctinfo_Ins(sPerAcctInfo,g_pub_tx.reply);
	if (ret != 0)
	{
		sprintf(acErrMsg,"插入个人编信息表出错![%s]",g_pub_tx.reply);
		WRITEMSG
		goto ErrExit;
	}

	/*** 输出变量 ***/
	set_zd_data("0590",per_no);
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"建立个人编号成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"建立个人编号失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
