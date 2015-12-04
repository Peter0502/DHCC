/*************************************************
* 文 件 名:  spF617.c
* 功能描述： 批量查询个人资料
*
* 作    者:  jack
* 完成日期： 2004年08月29日
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
#include "ag_peritemmap_c.h"

int spF617()
{
	char cUnit_no[5];
	char cName[51];

	char cFileName[50];			/* 临时文件名 */
	int	 ret = 0;
	FILE *fp;

	int num = 0;

	struct ag_peritemmap_c sAg_peritemmap;

	memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );

	memset( cFileName, 0x00, sizeof( cFileName ) );
	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	memset( cName, 0x0, sizeof(cName) );
	
	/* 数据初始化 */
	pub_base_sysinit();

	get_zd_data( "0920", cUnit_no );
	get_zd_data( "0260", cName );

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


	if ( strlen(cName) == 0 )
	{
		ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , \
			"unit_no = '%s'", cUnit_no );
	}
	else
	{
		ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , \
			"unit_no = '%s' and name = '%s'", cUnit_no, cName );
	}
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap_Dec_Sel错[%d]", ret );
		WRITEMSG
		goto ErrExit;
	}

	fprintf( fp, "~%s|%s|%s|\n", "个人编号","账号","户名" );
	
	while(1)
	{
		memset( &sAg_peritemmap, 0x00, sizeof(sAg_peritemmap) );
		
		ret = Ag_peritemmap_Fet_Sel( &sAg_peritemmap, g_pub_tx.reply );
		if ( ret == 100)
		{
			break;
		}
		else if( ret )
		{
			sprintf( acErrMsg, "执行Ag_peritemmap_Fet_Sel错[%d]", ret );
			WRITEMSG
			goto ErrExit;
		}

		fprintf(fp,"%s|%s|%s|\n", sAg_peritemmap.item_no, sAg_peritemmap.ac_no, sAg_peritemmap.name );

		++num;

	}
	fclose(fp);

	Ag_peritemmap_Clo_Sel( );

	set_zd_data( DC_FILE_SND, "1" );		/* 批量传送标志 */

	if (num == 0)
	{
		sprintf( acErrMsg,"本单位无个人代理资料!!!" );
		WRITEMSG
		strcpy( g_pub_tx.reply, "D104" );
	}
	else
	{
		strcpy( g_pub_tx.reply, "0000" );
	}

OkExit:
	sprintf(acErrMsg,"生成批量个人资料文件文件成功!!!");
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	fclose(fp);
	Ag_peritemmap_Clo_Sel( );
	sprintf(acErrMsg,"生成批量个人资料文件文件失败reply[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
