/*************************************************
* 文 件 名:    sp9752.c
* 功能描述：   
*              活期产品代码返回产品信息_对公
*
* 作    者:    lance
* 完成日期：   2003年02月01日
* 修改记录：   
*     1. 日    期:
*        修 改 人:	Azz
*        修改内容:	
*************************************************/
#define  ERR_DEAL   { WRITEMSG  goto ErrExit;} 
#include "string.h" 
#define EXTERN
#include "public.h"
	
sp9752()  
{ 		
	int ret=0;
	char prod_no[4];
	double rate_val , over_rate_val;
	char filename[100];
	FILE *fp;
	
	
	/** 数据初始化 **/
	pub_base_sysinit();
	
	/** 取值、赋值 **/	
	get_zd_data( "1031" , prod_no );	/* 产品代码 */
	
	/* 根据产品代码 返回产品名称 */
	ret = pub_base_get_prdtparm( prod_no );
	if( ret )
	{
		sprintf(acErrMsg,"调用根据产品代码取产品参数函数错误");
		WRITEMSG
		goto ErrExit;
	}		  

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

	/* 输出变量 */
	switch( prod_no[0] )
	{
		case '1':
			if(g_dd_parm.ac_type[0]=='5')
			{
				if(g_dd_parm.sttl_type[0]=='D')
					strcpy(g_dd_parm.ac_type,"5");
				else if(g_dd_parm.sttl_type[0]=='S')
					strcpy(g_dd_parm.ac_type,"6");
			}
			set_zd_data( "1032", g_dd_parm.title );	/* 产品名称 */
			set_zd_data( "1033", g_dd_parm.cur_no );
			set_zd_data( "1034", g_dd_parm.ac_type );
			if( g_dd_parm.cif_type[0]=='0' || g_dd_parm.cif_type[0]=='1' 
				|| g_dd_parm.cif_type[0]=='5' )
			{
				fprintf(fp,"1034|个人结算户|5|\n");
				fprintf(fp,"1034|个人储蓄户|6|\n");
			}
			if( g_dd_parm.cif_type[0]=='0' || g_dd_parm.cif_type[0]=='2' )
			{
				fprintf(fp,"1034|基本户|1|\n");
				fprintf(fp,"1034|一般户|2|\n");
				fprintf(fp,"1034|专用户|3|\n");
				fprintf(fp,"1034|临时户|4|\n");
			}
			set_zd_data( DC_FILE_SND,"2" );
			set_zd_data( "1036", g_dd_parm.intst_type );
			break;
		case '2':
			set_zd_data( "1032", g_td_parm.title );	/* 产品名称 */
			set_zd_data( "1033", g_td_parm.cur_no );
			/* set_zd_data( "1034", g_td_parm.ac_type ); */
			set_zd_data( "1036", g_td_parm.intst_type );
			set_zd_data( "103C", g_td_parm.td_type );
			set_zd_long( "103A", g_td_parm.term );
			if(g_td_parm.term_type[0]=='Y')
			{
				set_zd_data( "103B","年" );
			}
			else if(g_td_parm.term_type[0]=='M')
			{
				set_zd_data( "103B", "月" );
			}
			
	sprintf(acErrMsg,"名称[%s][%d][%s] [%s]",
		g_td_parm.title,g_td_parm.term,g_td_parm.term_type,g_td_parm.intst_type);
	WRITEMSG
			break;
	}

	fclose(fp);
		
	set_zd_data( "103E", "1" );

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
