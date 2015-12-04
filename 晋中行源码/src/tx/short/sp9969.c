/*--------------------------------------------------------
* 文 件 名: sp9969.c
* 功能描述：根据代理类型查询单位编号(除税务和财政代发工资) 
*
* 作    者: 
* 完成日期：2003年07月20日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
--------------------------------------------------------*/
#include "string.h"
#define EXTERN
#include "public.h"
#include "ag_unitinfo_c.h"

sp9969()
{ 		
	int	ret;
	int 	vcnt=0;
	int 	i=0;
	char 	filename[100];
	FILE 	*fp;

	struct ag_unitinfo_c sAg_unitinfo;

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp = fopen( filename, "w" );
	if( fp == NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	ret = Ag_unitinfo_Dec_Sel( g_pub_tx.reply, "pay_type not in \
		( '31', '46' ) order by unit_no" );
	if( ret ) 
	{
		sprintf( acErrMsg, "执行Ag_unitinfo__Dec_Sel错!!!" );
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		memset( &sAg_unitinfo, 0x0, sizeof(sAg_unitinfo) );
		ret = Ag_unitinfo_Fet_Sel( &sAg_unitinfo, g_pub_tx.reply );
		if( ret == 100 )
		{
			break;
		}
		else if ( ret ) 
		{
			sprintf( acErrMsg, "执行Ag_unitinfo_Fet_Sel错!!!" );
			WRITEMSG
			goto ErrExit;
		}

		pub_base_strpack( sAg_unitinfo.unit_name );

		/* 将第一条符合条件的记录作为初值赋给前台 */
		if( i==0 )
		{
			sprintf(acErrMsg,"[%s]", sAg_unitinfo.unit_no );
			WRITEMSG
			set_zd_data( "0360", sAg_unitinfo.unit_no );
		}
		fprintf( fp, "0360|%s|%s|\n", sAg_unitinfo.unit_name, sAg_unitinfo.unit_no );
		i++;
		vcnt++;
	}

	Ag_unitinfo_Clo_Sel();

	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
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
