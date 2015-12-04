/*************************************************
* 文 件 名: sp9799.c
* 功能描述：根据客户类型调证件类型_1101交易的客户类型快捷查询
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
	
sp9799()  
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
	get_zd_data( "0670" , type );	/* 客户类型 */
	
	if(type[0]=='1')
		strcpy(wherelist,"per_id");
	else if(type[0]=='2'||type[0]=='4'||type[0]=='3')
		strcpy(wherelist,"com_id");
	else if(type[0]=='5')
		strcpy(wherelist,"cop_id");

/*1：个人；2：公司 ；3：机构（指政府机关等组织）；4：同业 */
/* 5: 对公  add by lwb 20141015*/
	
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
	if(type[0] == '5')
	{
		ret=Dic_data_Dec_Sel(g_pub_tx.reply,"pr_code='%s' ",wherelist);
		if( ret ) goto ErrExit;

	}
	else if(type[0]=='3')
	{
		ret=Dic_data_Dec_Sel(g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','B')",wherelist);
		if( ret ) goto ErrExit;
	}
	else if(type[0]=='2'){
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','9','A','C','D') order by pr_seq",wherelist);
		if( ret ) goto ErrExit;
	}
	else if(type[0]!='1'){
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' and pr_seq in('8','9','A')",wherelist);
		if( ret ) goto ErrExit;
	}else{
		ret=Dic_data_Dec_Sel( g_pub_tx.reply,"pr_code='%s' ",wherelist);
		if( ret ) goto ErrExit;
	}	

	while(1)
	{
		ret = Dic_data_Fet_Sel(&vdic_data,g_pub_tx.reply);
		if( ret==100 ) break;
		if(type[0] == '1')
		{
		}		
		if( ret ) goto ErrExit;
	
		pub_base_strpack(vdic_data.pr_seq);
		pub_base_strpack(vdic_data.content);

		if( i==0 )	set_zd_data("0680",vdic_data.pr_seq);
		fprintf( fp,"0680|%-20s|%s|\n", vdic_data.content, vdic_data.pr_seq );
		i++;
	}

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
