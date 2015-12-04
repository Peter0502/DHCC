/***************************************************************
* 文 件 名:     spA011.c
* 功能描述：	内部帐开介质
*
* 作    者:     jack
* 完成日期：    2004年1月17日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define loc_t sys_loc_t
#define EXTERN
#include "public.h"

spA011()
{
	int ret = 0;

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	data_init_a011();

	strcpy(g_pub_tx.prt_ind,"0");
	strcpy(g_pub_tx.brf,"内部帐开户");
	strcpy(g_pub_tx.ac_wrk_ind,"000");
	strcpy(g_pub_tx.add_ind,"1");
	strcpy(g_pub_tx.ct_ind,"2");
	strcpy(g_pub_tx.prt_ind,"0");

	/***开介质***/
	ret=pub_acct_opn_in_mdm();
	if( ret )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	strcpy(g_pub_tx.ac_wrk_ind,"011");
	/***开户***/
	ret=pub_acct_opn_ac();
	if( ret )
	{
		sprintf(acErrMsg,"开介质错误![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	/*** 输出变量 ***/
	sprintf(acErrMsg,"ac_no[%s]",g_pub_tx.ac_no);
	WRITEMSG
	set_zd_data("112A",g_pub_tx.ac_no);
        set_zd_data("0370",g_pub_tx.ac_no);
       
OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
data_init_a011()
{
	get_zd_data("1121",g_pub_tx.prdt_code);	/**产品编号**/
	get_zd_long("1122",&g_pub_tx.mach_date); /**到期日期**/
	get_zd_data("1125",g_pub_tx.tmp_ind); /**计息类型**/
	get_zd_double("1123",&g_pub_tx.rate); /**利率**/
	get_zd_double("1124",&g_pub_tx.over_rate); /**逾期利率**/
	get_zd_data("1129",g_pub_tx.name); /**户名**/
}
