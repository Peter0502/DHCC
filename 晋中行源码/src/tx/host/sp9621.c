/*************************************************
* 文 件 名: sp9621.c
* 功能描述：密码超限文件清除
*
* 作    者: jack
* 完成日期：20031116
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <stdio.h>
#include <math.h>
#define EXTERN
#include "public.h"

int sp9621( )
{
	char cAcno[20];	
	char filename[128];
	char command[128];
	int ret=0;

    pub_base_sysinit();

	memset( cAcno, 0x0, sizeof(cAcno) );
	memset( filename, 0x0, sizeof(filename) );

	get_zd_data( "0310", cAcno );

	sprintf( filename, "/backup/pass_error/%s.%ld", cAcno, g_pub_tx.tx_date );

	memset( command, 0x0, sizeof(command) );
	sprintf(acErrMsg, "---filename=[%s]", filename);
	WRITEMSG
	sprintf( command, "rm -f %s", filename );

	ret = system( command );
	if ( ret )
	{
		sprintf(acErrMsg, "删除文件错误 !! [%d]", ret);
		WRITEMSG
	}

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"密码超限文件清除成功 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"密码超限文件清除失败 reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
