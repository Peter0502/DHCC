/*************************************************
* 文 件 名: spH614.c
* 功能描述：代理业务批量处理日期维护
*
* 作    者: jack
* 完成日期：2004年06月28日
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
#include "ag_unitinfo_c.h"
#include "pay_agent_c.h"

int spH614()
{
	int ret = 0;

	char cUnit_no[5];
	long lDate = 0;
	long lDate1 = 0;

	struct ag_unitinfo_c sAg_unitinfo;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		goto ErrExit;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	get_zd_data( "0920", cUnit_no );
	get_zd_long( "0440", &lDate );
	get_zd_long( "0470", &lDate1 );

	ret = Ag_unitinfo_Sel( g_pub_tx.reply, &sAg_unitinfo, \
			"unit_no = '%s' and unit_stat = '0'", cUnit_no );
	if ( ret == 100 )
	{
		sprintf( acErrMsg,"要修改的单位信息不存在!!!" );
		WRITEMSG
		goto ErrExit;
	}
	else if ( ret )
	{
		sprintf( acErrMsg,"查找要修改的单位信息出错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update ag_unitinfo set beg_date ='%ld',end_date ='%ld' where unit_no = '%s' and unit_stat = '0' ",lDate, lDate1, cUnit_no );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"修改单位信息中批处理日期错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	ret = sql_execute( "update pay_agent set tx_date ='%ld' where unit_no = '%s' and result = '1' ",lDate, cUnit_no );
	if ( ret )
	{
		strcpy( g_pub_tx.reply, "H020" );
		sprintf( acErrMsg,"修改代理记录表中批处理日期错!!!" );
		WRITEMSG
		goto ErrExit;
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf( acErrMsg,"修改代理信息批处理日期成功!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"修改代理信息批处理日期失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
