/*************************************************
* 文 件 名:  sp1804.c
* 功能描述： 柜员揽存信息查询
*
* 作    者:  peter
* 完成日期： 
*
* 修改记录：
* 日   期:
* 修 改 人:
* 修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "tel_save_inf_c.h"
#include "com_tel_c.h"

int sp1804()
{
	struct tel_save_inf_c vtel_save_inf;
	struct tel_save_inf_c f_tel_save_inf;
	struct com_tel_c	g_com_tel;

		
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset(&g_com_tel,0x00,sizeof(struct com_tel_c));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0370",f_tel_save_inf.ac_no );
	get_zd_long("0340",&f_tel_save_inf.ac_seqn );
	get_zd_data("0920",f_tel_save_inf.tel );

	/**组成查询条件**/
	pub_base_strpack( f_tel_save_inf.ac_no );
	if( strlen(f_tel_save_inf.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_tel_save_inf.ac_no );
		strcat( wherelist,tmpstr );
	}
	if( f_tel_save_inf.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_tel_save_inf.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_tel_save_inf.tel );
	if( strlen(f_tel_save_inf.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_tel_save_inf.tel );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"tel_save_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_no,ac_seqn,tel");

	ret=Tel_save_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Tel_save_inf_Fet_Sel( &vtel_save_inf, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**显示列标题**/
			fprintf( fp,"~@揽存账号|序号|@账号名称|@柜员号|@柜员名称|\n" );
		}
		
		/* 查询客户名称*/
		ret = pub_base_disac_file(vtel_save_inf.ac_no,vtel_save_inf.ac_seqn,
								g_pub_tx.ac_id_type);
		if( ret )
		{
			sprintf(acErrMsg,"查询账号出错ac_no[%s]ac_seqn[%d]!!",vtel_save_inf.ac_no,vtel_save_inf.ac_seqn );
			WRITEMSG
			strcpy( vtel_save_inf.ac_no,"----" );
			/***
			strcpy( g_pub_tx.reply, "W010" );
			goto	ErrExit;
			**/
		}

		switch( g_pub_tx.ac_id_type[0] )
		{
			case '1':
				strcpy(g_pub_tx.name,g_dd_mst.name);
				break;
			case '2':
				strcpy(g_pub_tx.name,g_td_mst.name);
				break;
			case '3':
				strcpy(g_pub_tx.name,g_ln_mst.name);
				break;		
			case '9':
				strcpy(g_pub_tx.name,g_in_mst.name);
				break;							
			default	:
				sprintf(acErrMsg,"账号对应产品类型错误,请与中心机房联系!");
				WRITEMSG
				strcpy( g_pub_tx.reply, "C120" );
				goto	ErrExit;
		}	
			
		/* 查询柜员名称 */
		ret=Com_tel_Sel(g_pub_tx.reply,&g_com_tel,"tel='%s'",vtel_save_inf.tel);
		if(ret==100)
		{
			sprintf(acErrMsg,"该柜员不存在");
			WRITEMSG
			/*
			strcpy(g_pub_tx.reply,"O120");
			goto ErrExit;
			*/
			strcpy( g_com_tel.name,"??" );
		}else if(ret){
			sprintf(acErrMsg,"查询柜员信息表错误[%s]",ret);
			WRITEMSG
			strcpy(g_pub_tx.reply,"O065");
			goto ErrExit;
		}		

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */


		fprintf( fp, "%s|%d|%s|%s|%s|\n",vtel_save_inf.ac_no,vtel_save_inf.ac_seqn,g_pub_tx.name,vtel_save_inf.tel,g_com_tel.name);
		ttlnum++;
	}

	ret=Tel_save_inf_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fclose(fp);

		set_zd_data( DC_FILE_SND,"1" );
	}

GoodExit:
	strcpy( g_pub_tx.reply, "0000" );
	sprintf(acErrMsg,"Before OK return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 0;
ErrExit:
	sprintf(acErrMsg,"Before bad return: reply [%s]",g_pub_tx.reply);
	WRITEMSG
	set_zd_data(DC_REPLY,g_pub_tx.reply);
	return 1;
}
