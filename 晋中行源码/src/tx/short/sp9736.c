/*************************************************
* 文 件 名: sp9736.c
* 功能描述：贷款产品转换中,根据条件显示贷款产品列表
* 作    者: lance
* 完成日期：2003年06月06日
* 修改记录：   
*     1. 日    期:
*        修 改 人:
*        修改内容:
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"

sp9736()  
{ 		
	/** 数据初始化 **/
	int ret=0, time_type=0;
	int sel_flag=0;
	int vcnt=0;
	char trust_flg[2];
	char cif_type[2];
	char ln_type[2];
	char time_typ[2];
	char wherelist[200];
	char tmpstr[100];
	char filename[100];
	FILE *fp;
	int i=0;
	
	/** 取值、赋值 **/	
	get_zd_data( "0680", cif_type );    /* 客户类型 */
	get_zd_int( "0510" , &time_type ); /* 时限 */
	get_zd_data( "0700", ln_type );    /* 贷款方式 */
	
	if( time_type<=12 )
	{
		strcpy( time_typ , "1" );
	}else if( time_type<=36 ){
		strcpy( time_typ , "2" );
	}else	if( time_type<=60 ){
		strcpy( time_typ , "3" );
	}else{
		strcpy( time_typ , "4" );
	}
	
	TRACE

	/*1：个人； 2：公司 ；3：机构（指政府机关等组织）； 
	4：金融机构； 5：内部员工 */
		if( cif_type[0]!='0' )
		{
			sprintf( wherelist," (cif_type='%s' or cif_type='0') ",cif_type );
		}else{
			strcpy( wherelist," cif_type in ('0','1','2','3','4','5','9') " );
		}
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG			
			
	/* 贷款期限 */

		sprintf( tmpstr," and time_type='%s' ",time_typ );
		strcat( wherelist,tmpstr );
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG	
	/* 贷款方式 */
		sprintf( tmpstr," and ln_type='%s' ",ln_type );
		strcat( wherelist,tmpstr );		
		
		sprintf(acErrMsg,"%s",wherelist);
		WRITEMSG	
		
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

	/***产品列表***/
	ret=Ln_parm_Dec_Sel( g_pub_tx.reply,"%s order by prdt_no",wherelist );
	if( ret ) goto ErrExit;

	i=0;
	while(1)
	{
		ret = Ln_parm_Fet_Sel( &g_ln_parm , g_pub_tx.reply );
		if( ret==100 ) break;		
		if( ret ) goto ErrExit;
		
		sel_flag++;
		    
		if( i==0 )
		{
			set_zd_data("0240",g_ln_parm.prdt_no);
			set_zd_data("0270",g_ln_parm.title);
		}

		pub_base_strpack(g_ln_parm.title);
		fprintf( fp,"0240|%-30s|%s|\n",g_ln_parm.title,g_ln_parm.prdt_no );

		i++;
		vcnt++;
	}
	Ln_parm_Clo_Sel();

		if ( sel_flag==0 )
		{    
		    sprintf(acErrMsg,"没有满足上述条件的记录![%s]",g_pub_tx.reply);
		    WRITEMSG
		    strcpy(g_pub_tx.reply,"L123");
		    goto ErrExit;	
		}
		
	if( vcnt )
		set_zd_data( DC_FILE_SND,"2" );
	fclose(fp);
	
	/* 输出变量 */

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
