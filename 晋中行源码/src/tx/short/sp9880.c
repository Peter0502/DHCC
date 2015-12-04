/***************************************************************
* 文 件 名: sp9880.c
* 功能描述：代发记录查询
*
* 作    者: jack
* 完成日期：2003年03月31日
*
* 修改记录：
* 日    期:
* 修 改 人:
* 修改内容:
**************************************************************/
#include <string.h>
#define EXTERN
#include "public.h"
#include "pay_agent_c.h"

sp9880()
{
	int ret = 0;
	int num = 0;
	char cUnit_no[5];
	char cFlag[2];

	char cFileName[128];
	FILE *fp;

	struct pay_agent_c sPay_agent; /*  代发业务记录表  */

	memset(&sPay_agent,0x0,sizeof(sPay_agent));
	memset(cUnit_no,0x0,sizeof(cUnit_no));

	/*** 初始化全局变量 ***/
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0700", cFlag );

	if ( cFlag[0] == '1' )
	{
		goto OkExit;
	}

	memset( cFileName, 0x00, sizeof( cFileName ) );

	/* 根据柜员号、终端号生成临时文件 */
	ret = pub_base_AllDwnFilName( cFileName ); 
	if( ret )
	{
		sprintf( acErrMsg, "生成下传全路经文件名(批量)错" );
		WRITEMSG
		goto ErrExit;
	}

	fp = fopen( cFileName, "w" );
	if( fp==NULL )
	{
		sprintf( acErrMsg, "临时文件错误!" );
		WRITEMSG
		sprintf( g_pub_tx.reply, "B080" );
		goto ErrExit;
	}

	ret = Pay_agent_Dec_Sel( g_pub_tx.reply , "unit_no = '%s' \
					order by item_no", cUnit_no);
	if ( ret )
	{
		sprintf( acErrMsg, "执行Pay_agent_Dec_Upd错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sPay_agent, 0x00, sizeof(sPay_agent));
		
		ret = Pay_agent_Fet_Sel( &sPay_agent, g_pub_tx.reply );
		if ( ret == 100)
		{
			if (num == 0)
			{
				sprintf( acErrMsg, "代发记录不存在!!!!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "H013");
				goto ErrExit;
			}
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Pay_agent_Fet_Upd错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%f|\n", sPay_agent.unit_no, sPay_agent.item_no, sPay_agent.tram);

		num++;
	}
	Pay_agent_Clo_Upd( );

	fclose(fp);
	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

OkExit:
	strcpy(g_pub_tx.reply,"0000");
	sprintf(acErrMsg,"代发信息查询成功![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"代发信息查询失败![%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data("0120",g_pub_tx.reply);
	return 1;
}
