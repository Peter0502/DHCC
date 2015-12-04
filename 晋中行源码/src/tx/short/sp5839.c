/*************************************************
* 文 件 名:  sp5839.c
* 功能描述： 大额来账公用子交易
*			       拆报文 登记表
*			 
* 作    者:  ChenMing
* 完成日期： 2006年8月17日
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
#include "hv_pub.h"
#include "hv_define.h"

int sp5839()
{
	pub_base_sysinit();

	if (pub_hv_recv_main()) goto ErrExit; /* 调用大额来帐处理主函数 */
		strcpy( g_pub_tx.reply,"0000" );

OkExit:
	sprintf(acErrMsg,"Before OK return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	strcpy( g_pub_tx.reply,"1111" );
	sprintf(acErrMsg,"Before return: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}


