/*************************************************
* 文 件 名:  spJLZ0.c
* 功能描述： MBFE来账初始化数据,本交易不执行任何业务逻辑，
*			       仅仅将有些数据域初始化一下，防止下边程序取
*            域中内容出现问题
*             1,fd52 =0 ,2,fd3==qsbrno,3fd7 5001
* 作    者:  LiuHuafeng
* 完成日期： 2006-10-7 17:46
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/
#define  EXTERN
#include <string.h>
#include <stdio.h>
#include "public.h"
#include "lv_pub.h"
#include "lv_define.h"

int spJLZ0()
{
	long iSubpackIndex=0;
	pub_base_sysinit();
	set_zd_long("0520",iSubpackIndex); /* 当前笔数 */
	set_zd_long("0530",iSubpackIndex); /* spJJJ1中要处理的记录序号 初始值=0 */	
	set_zd_data(DC_TX_BR_NO, QS_BR_NO );
	set_zd_data(DC_TEL,"5001");
	memcpy(g_pub_tx.reply,"0000",4);
OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}

