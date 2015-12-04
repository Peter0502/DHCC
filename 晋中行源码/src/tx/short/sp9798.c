/*************************************************
* 文 件 名: sp9798.c
* 功能描述：根据客户生活方式类别显示明细_1108交易的快捷查询
* 作    者: rob
* 完成日期：2003年04月13日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9798()  
{ 		
	/** 数据初始化 **/
	struct dic_data_c        vdic_data; 
	int ret=0;
	char wherelist[40];
	char filename[100];
	char type[2],tmp[5];
	FILE *fp;
	int i=0;

	memset (&vdic_data, 0x00, sizeof(struct dic_data_c));
	
	/** 取值、赋值 **/	
	get_zd_data( "0670" , type );			/* 生活方式类型 */
	
	if(type[0]=='1')
		strcpy(wherelist,"life_de1");
	else if(type[0]=='2')
		strcpy(wherelist,"life_de2");
	else if(type[0]=='3')
		strcpy(wherelist,"life_de3");
	else if(type[0]=='4')
		strcpy(wherelist,"life_de4");
	else if(type[0]=='5')
		strcpy(wherelist,"life_de5");
	else if(type[0]=='6')
		strcpy(wherelist,"life_de6");
	else if(type[0]=='7')
		strcpy(wherelist,"life_de7");
	else if(type[0]=='8')
		strcpy(wherelist,"life_de8");

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

	/***选项列表***/
	ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s'",wherelist);
	if( ret ) goto ErrExit;

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if( ret ) goto ErrExit;
	
		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )	set_zd_data("0220",vdic_data.pr_seq);
		fprintf( fp,"0220|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq );
		i++;
	}

vtcp_log("SUCCESSSUCCESSSUCCESS");
	Dic_data_Clo_Sel();

	if( i )
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
