/*************************************************
* 文 件 名:  spH654.c
* 功能描述： 查询本行短信服务品种
*
* 作    者:  mike
* 完成日期： 2004年04月24日
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
#include "mob_server_type_c.h"

int spH654()
{
	char cMobile[14];

	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct mob_server_type_c sMob_server_type;

	memset( &sMob_server_type, 0x00, sizeof(sMob_server_type) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cMobile, 0x0, sizeof(cMobile) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0610", cMobile );

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

	ret = Mob_server_type_Dec_Sel( g_pub_tx.reply , \
			"1=1 order by server_type" );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Mob_server_type_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|%s|%s|\n", "服务种类代码", \
			"服务种类名称", "有记录收费标准(单笔)", \
			"无记录收费标准(单笔)", "月租费标准" );
	
	while(1)
	{
		memset( &sMob_server_type, 0x00, sizeof(sMob_server_type) );
		
		ret = Mob_server_type_Fet_Sel( &sMob_server_type, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Mob_server_type_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%.2f|%.2f|%.2f|\n", sMob_server_type.server_type, sMob_server_type.servername, sMob_server_type.sfbz_yes, sMob_server_type.sfbz_no, sMob_server_type.sfbz_mon );

		++num;

	}
	fclose(fp);

	Mob_server_type_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

	if (num == 0)
	{
		sprintf(acErrMsg,"本行无短信服务品种记录!!!");
		WRITEMSG
		strcpy( g_pub_tx.reply, "0001" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"生成本行服务品种文件成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Mob_server_type_Clo_Sel( );
	sprintf(acErrMsg,"生成本行服务品种文件失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
