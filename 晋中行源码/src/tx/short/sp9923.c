/*************************************************
* 文 件 名:    sp9923.c
* 功能描述：   
*              返回制卡的介质种类
*
* 作    者:    
* 完成日期：   2003年01月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
#include "mdm_attr_c.h"

sp9923()  
{ 		
	int ret=0;
	char mdm_code[5];
	int first=0;
	char filename[100];
	FILE *fp;

	struct mdm_attr_c mdm_attr;

	memset(&mdm_attr,0x00,sizeof(struct mdm_attr_c));

	ret = Mdm_attr_Dec_Sel( g_pub_tx.reply , 
		" mdm_type in('3','4','5') "  );
	if (ret)
	{
		WRITEMSG
		goto ErrExit;
	}

		first=0;
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

	first=0;

	while(1 )
	{
		ret = Mdm_attr_Fet_Sel( &mdm_attr , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

		if( first==0 )
		{
			set_zd_data("1161", mdm_attr.mdm_code );			/*介质种类*/
			set_zd_data( DC_FILE_SND,"2" );
		}
			pub_base_strpack( mdm_attr.mdm_name );
			fprintf( fp,"1161|%-20s|%s|\n",mdm_attr.mdm_name,mdm_attr.mdm_code );

		first++;
	}
	Mdm_attr_Clo_Sel();

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
