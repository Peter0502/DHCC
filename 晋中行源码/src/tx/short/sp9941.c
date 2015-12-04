/*************************************************
* 文 件 名:    sp9941.c
* 功能描述：   
*              查询活期和定期产品
*
* 作    者:    rob
* 完成日期：   2003年06月30日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#include "string.h"
#define EXTERN
#include "public.h"

sp9941()
{ 		
	/** 数据初始化 **/
	struct	dd_parm_c sDd_parm;
	struct  td_parm_c sTd_parm;

	int	ret;
	int vcnt;
	char 	filename[100];
	FILE 	*fp;

	memset(&sDd_parm, 0x0, sizeof(struct dd_parm_c));
	memset(&sTd_parm, 0x0, sizeof(struct td_parm_c));

    vcnt = 0;
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

	ret = Dd_parm_Dec_Sel( g_pub_tx.reply, "1=1");
	if( ret ) 
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Dd_parm_Fet_Sel( &sDd_parm, g_pub_tx.reply );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		/* 将第一条符合条件的记录作为初值赋给前台 */
		/***
		if( vcnt==0 )
		{
			sprintf(acErrMsg,"[%s]",sComParm.val);
			WRITEMSG
			set_zd_data("0720", sComParm.val);
		}
		****/
		fprintf( fp, "0230|%-30s|%-3s|\n", sDd_parm.title, sDd_parm.prdt_no );
		vcnt++;
	}

	Dd_parm_Clo_Sel();

	ret = Td_parm_Dec_Sel( g_pub_tx.reply, "1=1");
	if( ret ) 
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !![%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	while(1)
	{
		ret = Td_parm_Fet_Sel( &sTd_parm, g_pub_tx.reply );
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		/* 将第一条符合条件的记录作为初值赋给前台 */
		/***
		if( vcnt==0 )
		{
			sprintf(acErrMsg,"[%s]",sComParm.val);
			WRITEMSG
			set_zd_data("0720", sComParm.val);
		}
		****/
		fprintf( fp, "0230|%-30s|%-3s|\n", sTd_parm.title, sTd_parm.prdt_no );
		vcnt++;
	}

	Td_parm_Clo_Sel();

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
