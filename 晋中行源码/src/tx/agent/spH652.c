/*************************************************
* 文 件 名:  spH652.c
* 功能描述： 客户申请取消短信服务
*
* 作    者:  jack
* 完成日期： 2004年04月24日
*
* 修改记录： 
* 日    期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int spH652()
{
	int ret = 0;
	int i = 0;

	char cMobile[14];
	char cPasswd[7];

	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( cPasswd, 0x0, sizeof(cPasswd) );

	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0610", cMobile );

	ret = sql_execute( "delete from mob_acct_type where \
		mobile = '%s'", cMobile );
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"取消客户账号服务品种出错!!![%s]", cMobile );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "delete from mob_custinfo where \
		mobile = '%s'", cMobile );
	if ( ret != 0 )
	{
		sprintf(acErrMsg,"取消短信银行客户信息出错!!![%s]", cMobile );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	sprintf(acErrMsg,"取消短信服务成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"取消短信服务失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
