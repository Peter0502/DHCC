/*************************************************
* 文 件 名:  sp9753.c
* 功能描述： 查询转帐摘要
*
* 作    者:  rob
* 完成日期： 2003年5月24日 
*
* 修改记录： 
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/

#include <string.h>
#define EXTERN
#include "public.h"

int sp9753()
{
    int ret;
	FILE *fp;
	int vcnt;
	char cFileName[30];

	struct dic_data_c dic_data_c;

	memset(&dic_data_c, 0x0, sizeof(struct dic_data_c));

    ret = pub_base_AllDwnFilName( cFileName ); 
	if (ret != 0) 
	{ 
		sprintf( acErrMsg, "生成文件名错错误!!" ); 
		WRITEMSG 
		strcpy(g_pub_tx.reply,"D126"); 
		goto ErrExit; 
	} 
	
	sprintf(acErrMsg,"filename=[%s]",cFileName); 
	WRITEMSG 
	
	fp = fopen( cFileName, "w" ); 
	if( fp==NULL ) 
	{ 
		sprintf( acErrMsg, "临时文件错误!" ); 
		WRITEMSG 
		strcpy( g_pub_tx.reply, "B080" ); 
		goto ErrExit; 
	} 
	
	ret = Dic_data_Dec_Sel(g_pub_tx.reply, "pr_code='out_meno' order by \
			pr_seq");
	if (ret != 0)
	{
		sprintf(acErrMsg,"DECLARE FOR SELECT ERROR !! [%d]",ret);
		WRITEMSG
		goto ErrExit;
	}

	vcnt = 0;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&dic_data_c, g_pub_tx.reply);
		if (ret != 0 && ret != 100)
		{
			sprintf(acErrMsg,"FETCH FOR SELECT ERROR !! [%d]",ret);
			WRITEMSG
			goto ErrExit;
		}
		else if (ret == 100)
		{
			break;
		}

		pub_base_strpack(dic_data_c.content);
		pub_base_strpack(dic_data_c.pr_seq);

		fprintf(fp, "0890|%-10s|%-3s|\n", dic_data_c.content, dic_data_c.pr_seq);
		vcnt++;
	}

	Dic_data_Clo_Sel();
	if( vcnt )
		 set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);

OkExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"success !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"fail !!: reply is[%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
