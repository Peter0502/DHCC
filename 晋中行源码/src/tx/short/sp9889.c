/*************************************************
* 文 件 名:    sp9889.c
* 功能描述：   根据记帐标志为"记帐"且管理号码标志为"管理"
*	       且出售给客户标志为"可出售给客户"返回凭证种类
*
* 作    者:    jane
* 完成日期：   2003年04月27日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "note_parm_c.h"

sp9889()
{ 		
	/** 数据初始化 **/
	struct	note_parm_c      sNoteParm;		/* 凭证参数表 */
	int	ret;
	int 	vcnt=0;
	int 	i=0;
	char 	filename[100];
	FILE 	*fp;

	memset ( &sNoteParm, 0x00, sizeof( struct note_parm_c ) );

	/* 生成下传全路经文件名(批量) */
	pub_base_AllDwnFilName( filename );

	fp=fopen(filename,"w");
	if( fp==NULL )
	{
		sprintf( acErrMsg,"open file error [%s]",filename );
		WRITEMSG
		strcpy( g_pub_tx.reply,"S047");
		goto ErrExit;
	}

	/* 根据记帐标志为"记帐"且管理号码标志为"管理"
		且出售给客户标志为"可出售给客户"返回凭证种类 */
	ret = Note_parm_Dec_Sel( g_pub_tx.reply, "dc_ind='%s' \
				and ctl_no_ind='%s' and sell_ind='%s'\
				order by note_type", "Y", "Y", "Y");
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Note_parm_Fet_Sel( &sNoteParm, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret ) goto ErrExit;
		else
		{
			/* 检查凭证是否处于使用状态 */
			if( sNoteParm.use_ind[0] == 'N' )
			{
				sprintf( acErrMsg,"凭证处于停用状态!" );
				WRITEMSG
				strcpy( g_pub_tx.reply, "N025" );
				goto ErrExit;
			}

			/* 将第一条符合条件的记录作为初值赋给前台 */
			if( i==0 )
			{
				sprintf(acErrMsg,"[%s]",sNoteParm.note_type);
				WRITEMSG
				set_zd_data( "0890", sNoteParm.note_type );
			}
			fprintf( fp, "0890|%20s|%s|\n", sNoteParm.name, sNoteParm.note_type );
			i++;
			vcnt++;
		}
	}

	Note_parm_Clo_Sel();

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
