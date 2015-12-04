/*************************************************
* 文 件 名:    sp9905.c
* 功能描述：   
*              根据币种是否启用为"是"的返回币种
*
* 作    者:    jane
* 完成日期：   2003年04月30日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "com_cur_no_code_c.h"

sp9905()
{ 		
	/** 数据初始化 **/
	struct	com_cur_no_code_c      sComCurNoCode;		/* 公共币种表 */
	int	ret;
	int 	vcnt=0;
	int 	i=0;
	char 	filename[100];
	FILE 	*fp;

	memset ( &sComCurNoCode, 0x00, sizeof( struct com_cur_no_code_c ) );

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

	/* 根据币种是否启用为"是"的返回币种 */
	ret = Com_cur_no_code_Dec_Sel( g_pub_tx.reply, "use_ind='%s' order by cur_no", "Y" );
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Com_cur_no_code_Fet_Sel( &sComCurNoCode, g_pub_tx.reply );
		if( ret == 100 ) break;
		else if( ret ) goto ErrExit;
		else
		{
			/* 将第一条符合条件的记录作为初值赋给前台 */
			if( i==0 )
			{
				sprintf( acErrMsg,"[%s]", sComCurNoCode.cur_no );
				WRITEMSG
				set_zd_data( "0210", sComCurNoCode.cur_no );
			}
			fprintf( fp, "0210|%s|%s|\n", sComCurNoCode.cur_no_name, sComCurNoCode.cur_no );
			i++;
			vcnt++;
		}
	}

	Com_cur_no_code_Clo_Sel();

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
