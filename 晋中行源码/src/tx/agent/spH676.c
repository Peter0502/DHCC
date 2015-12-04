/*************************************************
* 文 件 名: spH676.c
* 功能描述：代理业务传递资料(到渠道)处理 
*
* 作    者: mike
* 完成日期：2004年09月12日
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
#include "ag_peritemmap_c.h"

int spH676()
{
	int ret = 0;
	char tmpstr[32];

	char cUnit_no[5];
	
	char filename[128];
	FILE *fp;

	struct ag_peritemmap_c sAg_peritemmap;

	if (pub_base_sysinit())
	{
		sprintf(acErrMsg,"初始化公用结构错误!!");
		WRITEMSG
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}

	memset( cUnit_no, 0x0, sizeof(cUnit_no) );
	get_zd_data( "0920", cUnit_no );
	
	memset( filename, 0x0, sizeof(filename) );
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if ( fp == NULL )
	{
		sprintf(acErrMsg," open file [%s] error ",filename );
		strcpy( g_pub_tx.reply,"S047" );
		set_zd_data(DC_REPLY,g_pub_tx.reply);
		return 1;
	}
	
	ret = Ag_peritemmap_Dec_Sel( g_pub_tx.reply , "unit_no = '%s'" , cUnit_no );
	if ( ret )
	{
		sprintf( acErrMsg, "执行Ag_peritemmap_Dec_Sel出错!!!" );
		WRITEMSG
		goto ErrExit;
	}
	
	while(1)
	{
		memset( &sAg_peritemmap, 0x0, sizeof(sAg_peritemmap) );
		ret = Ag_peritemmap_Fet_Sel( &sAg_peritemmap, g_pub_tx.reply );
		if ( ret == 100 )
		{
			break;
		}
		else if ( ret )
		{
			sprintf( acErrMsg, "执行Ag_peritemmap_Fet_Sel出错!!!" );
			WRITEMSG
			Ag_peritemmap_Clo_Sel();
			goto ErrExit;
		}
		
		pub_base_strpack( sAg_peritemmap.item_no );
		pub_base_strpack( sAg_peritemmap.ac_no );
		fprintf( fp, "%s|%s|%s|\n", cUnit_no, sAg_peritemmap.item_no, sAg_peritemmap.ac_no );
	}
	
	fclose( fp );
	
	Ag_peritemmap_Clo_Sel();	

	set_zd_data(DC_FILE_SND,"1");

OkExit:
	sprintf( acErrMsg,"代理资料传递处理成功!!!" );
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"代理资料传递处理失败[%s]", g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
