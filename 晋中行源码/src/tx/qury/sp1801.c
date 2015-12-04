/*************************************************
* 文 件 名:  sp1801.c
* 功能描述： 客户与客户间关系查询
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
#include "cif_cif_rel_c.h"

int sp1801()
{
	struct cif_cif_rel_c vcif_cif_rel;
	struct cif_cif_rel_c f_cif_cif_rel;
	struct cif_cif_rel_c f_cif_cif_rela;
	char f_rel_sts3[41];
	char f_rel_sts4[41];	
	
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

	get_zd_long("0540",&f_cif_cif_rel.cif_no );
	get_zd_long("0290",&f_cif_cif_rel.rel_cid_no );
	get_zd_data("0240",f_cif_cif_rel.rel_code );
	get_zd_long("0440",&f_cif_cif_rel.crt_date );
	get_zd_long("0450",&f_cif_cif_rela.crt_date );

	/**组成查询条件**/
	if( f_cif_cif_rel.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_cif_rel.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cif_rel.rel_cid_no )
	{
		sprintf( tmpstr," rel_cid_no=%ld and",f_cif_cif_rel.rel_cid_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_cif_rel.rel_code );
	if( strlen(f_cif_cif_rel.rel_code) )
	{
		sprintf( tmpstr," rel_code='%s' and",f_cif_cif_rel.rel_code );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cif_rel.crt_date )
	{
		sprintf( tmpstr," crt_date>=%ld and",f_cif_cif_rel.crt_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_cif_rela.crt_date )
	{
		sprintf( tmpstr," crt_date<=%ld and",f_cif_cif_rela.crt_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_cif_rel" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,rel_cid_no,rel_code,rel_sts,crt_date,cls_date");

	ret=Cif_cif_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_cif_rel_Fet_Sel( &vcif_cif_rel, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|关联客户代码|@客户关系代码|@关系状态|建立日期|解除日期|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_cif_rel.cif_no<-2147000000) vcif_cif_rel.cif_no=0;
		if(vcif_cif_rel.rel_cid_no<-2147000000) vcif_cif_rel.rel_cid_no=0;
		pub_base_dic_show_str(f_rel_sts3,"rel_sts",vcif_cif_rel.rel_sts);
		pub_base_dic_show_str(f_rel_sts4,"rel_code_cif",vcif_cif_rel.rel_code);
				
		if(vcif_cif_rel.crt_date<-2147000000) vcif_cif_rel.crt_date=0;
		if(vcif_cif_rel.cls_date<-2147000000) vcif_cif_rel.cls_date=0;

		fprintf( fp, "%ld|%ld|%s|%s|%ld|%ld|\n",vcif_cif_rel.cif_no,vcif_cif_rel.rel_cid_no,f_rel_sts4,f_rel_sts3,vcif_cif_rel.crt_date,vcif_cif_rel.cls_date);
		ttlnum++;
	}

	ret=Cif_cif_rel_Clo_Sel( );
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
