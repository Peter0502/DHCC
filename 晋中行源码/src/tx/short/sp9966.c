/*************************************************
* 文 件 名:    sp9966.c
* 功能描述：   查询单边记账摘要选项
*
* 作    者:    jane
* 完成日期：   2003年07月18日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"
#include "note_parm_c.h"

sp9966()
{ 		
	/** 数据初始化 **/
	struct	dic_data_c      sDicData;		/* 数据字典项目内容 */
	struct	note_parm_c      sNoteParm;		/* 凭证参数表 */
	int	ret;
	int 	vcnt=0;
	int 	i=0;
	char 	filename[100];
	FILE 	*fp;

	memset ( &sDicData, 0x00, sizeof( struct dic_data_c ) );
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

	ret = Dic_data_Dec_Sel( g_pub_tx.reply, "pr_code='tran_memo'" );
	if( ret ) 
		goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel( &sDicData, g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		pub_base_strpack(sDicData.pr_seq);
		pub_base_strpack(sDicData.content);
		if(i==0)
		{
			set_zd_data("0230",sDicData.pr_seq);
		}

		fprintf(fp,"0230|%-20s|%s|\n",sDicData.content,sDicData.pr_seq);
		i++;
		vcnt++;
	}

	Dic_data_Clo_Sel();

	ret = Note_parm_Dec_Sel( g_pub_tx.reply, "name NOT LIKE '%%存单' and name NOT LIKE '%%存折' and name NOT LIKE '%%卡%%'order by note_type" );
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
			fprintf( fp, "0890|%-20s|%s|\n", sNoteParm.name, sNoteParm.note_type );
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
