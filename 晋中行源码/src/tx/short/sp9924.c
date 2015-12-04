/*************************************************
* 文 件 名:    sp9924.c
* 功能描述：   
*              返回内部开户的产品
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

sp9924()  
{ 		
	int ret=0;
	char mdm_code[5];
	int first=0;
	char filename[100];
	FILE *fp;

	struct in_parm_c in_parm_c;

	memset(&in_parm_c,0x00,sizeof(struct in_parm_c));

	ret = In_parm_Dec_Sel( g_pub_tx.reply , 
		" substr(spe_ind,1,1)='1' order by acc_hrt "  );
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
		ret = In_parm_Fet_Sel( &in_parm_c , g_pub_tx.reply );
		if( ret==100 ) break;
		if( ret ) goto ErrExit;

			pub_base_strpack( in_parm_c.prdt_mo );
		if( first==0 )
		{
			set_zd_data("1121", in_parm_c.prdt_no );
			set_zd_data("1126", in_parm_c.prdt_mo );
			set_zd_data( DC_FILE_SND,"2" );
		}
			fprintf( fp,"1121|%-30s|%s|\n",in_parm_c.prdt_mo,in_parm_c.prdt_no );

		first++;
	}
	In_parm_Clo_Sel();

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
