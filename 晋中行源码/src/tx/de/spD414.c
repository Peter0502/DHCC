/***************************************************************
* 文 件 名: spD414.c
* 功能描述：存折写磁
* 作    者: mike
* 完成日期：2003年06月05日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <stdio.h>
#define EXTERN
#include "public.h"

spD414()
{
	int ret = 0;
	char tmp_ac_no[20];

	memset(&g_mdm_ac_rel,0x00,sizeof(struct mdm_ac_rel_c));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	/*** 取输入变量  ***/
	get_zd_data("0310",g_pub_tx.ac_no);							/*帐号*/

	get_zd_data("0670",g_pub_tx.draw_pwd_yn);
	get_zd_data("0790",g_pub_tx.draw_pwd);
 	get_zd_data("0680",g_pub_tx.draw_id_yn);
	get_zd_data("0690",g_pub_tx.id_type);
	get_zd_data("0620",g_pub_tx.id_no);

	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'", \
						 g_pub_tx.ac_no);
	if( ret==100 )
	{
	  	sprintf(acErrMsg,"介质账户对照表中不存在该记录!");
	   	WRITEMSG
		strcpy(g_pub_tx.reply,"P237");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"查找记录出错[%d]",ret);
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	/*检查支取方式是否合法*/
	ret=pub_base_check_draw("001");
	if( ret )
	{
	 	sprintf(acErrMsg,"调用检查支取方式是否合法函数出错!!");
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"存折写磁成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"存折写磁失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
