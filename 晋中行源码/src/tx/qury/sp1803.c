/*************************************************
* 文 件 名:  sp1803.c
* 功能描述： 客户经理关系查询
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
#include "cif_mger_rel_c.h"

int sp1803()
{
	struct cif_mger_rel_c vcif_mger_rel;
	struct cif_mger_rel_c f_cif_mger_rel;
	struct cif_mger_rel_c f_cif_mger_rela;
	char f_mang_type2[41];
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

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_mger_rel.cif_no );
	get_zd_data("0910",f_cif_mger_rel.mang );
	get_zd_data("0700",f_cif_mger_rel.mang_type );
	get_zd_long("0290",&f_cif_mger_rel.crt_date );
	get_zd_long("0440",&f_cif_mger_rela.crt_date );

	/**组成查询条件**/
	if( f_cif_mger_rel.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_mger_rel.cif_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_mger_rel.mang );
	if( strlen(f_cif_mger_rel.mang) )
	{
		sprintf( tmpstr," mang='%s' and",f_cif_mger_rel.mang );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_mger_rel.mang_type );
	if( strlen(f_cif_mger_rel.mang_type) )
	{
		sprintf( tmpstr," mang_type='%s' and",f_cif_mger_rel.mang_type );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_mger_rel.crt_date )
	{
		sprintf( tmpstr," crt_date>=%ld and",f_cif_mger_rel.crt_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_mger_rela.crt_date )
	{
		sprintf( tmpstr," crt_date<=%ld and",f_cif_mger_rela.crt_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_mger_rel" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,mang,mang_type,crt_date,canl_date");

	ret=Cif_mger_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_mger_rel_Fet_Sel( &vcif_mger_rel, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@经理代码|@客户经理类型|建立日期|解除日期|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_mger_rel.cif_no<-2147000000) vcif_mger_rel.cif_no=0;
		pub_base_dic_show_str(f_mang_type2,"mang_type",vcif_mger_rel.mang_type);
		if(vcif_mger_rel.crt_date<-2147000000) vcif_mger_rel.crt_date=0;
		if(vcif_mger_rel.canl_date<-2147000000) vcif_mger_rel.canl_date=0;

		fprintf( fp, "%ld|%s|%s|%ld|%ld|\n",vcif_mger_rel.cif_no,vcif_mger_rel.mang,f_mang_type2,vcif_mger_rel.crt_date,vcif_mger_rel.canl_date);
		ttlnum++;
	}

	ret=Cif_mger_rel_Clo_Sel( );
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
