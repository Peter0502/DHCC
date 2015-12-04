/*************************************************
* 文 件 名: sp9965.c
* 功能描述：由手机号码查询客户对应姓名(mob_custinfo)
*
* 作    者: jack
* 完成日期：2003年7月18日
*
* 修改记录： 
*   日   期:
*   修 改 人:
*   修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"
#include "mob_custinfo_c.h"

int sp9965()
{
	int ret=0;
	char cMobile[15];

	struct mob_custinfo_c sMob_custinfo;

	memset( cMobile, 0x0, sizeof(cMobile) );
	memset( &sMob_custinfo, 0x0, sizeof(sMob_custinfo) );

	get_zd_data( "0610", cMobile );

	ret = Mob_custinfo_Sel( g_pub_tx.reply, &sMob_custinfo, \
		"mobile = '%s'", cMobile );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_custinfo_Sel出错[%d]!!!", ret );
		WRITEMSG
		goto ErrExit;
	}

	pub_base_strpack( sMob_custinfo.cust_name );
	set_zd_data( "0260", sMob_custinfo.cust_name );

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg, "由手机号码查询客户姓名成功!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf( acErrMsg, "由手机号码查询客户姓名失败[%d]!!!", g_pub_tx.reply );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
