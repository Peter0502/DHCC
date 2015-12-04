/*************************************************
* 文 件 名:  sp1503.c
* 功能描述： 黑名单信息查询
*
* 作    者:  peter
* 完成日期： 
*
* 修改记录：
* 日    期:
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
#include "cif_blklst_rel_c.h"
#include "cif_basic_inf_c.h"

int sp1503()
{
	struct cif_blklst_rel_c vcif_blklst_rel;
	struct cif_blklst_rel_c f_cif_blklst_rel;
		char f_blalist_origin[41];
		char f_relation_sts[41];
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

	get_zd_data("0670",f_cif_blklst_rel.blalist_origin);
	get_zd_data("0810",f_cif_blklst_rel.rmk);

	/**组成查询条件**/
	if( f_cif_blklst_rel.cif_no )
	{
		sprintf(tmpstr,"blalist_origin='%s' and ",f_cif_blklst_rel.blalist_origin);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_blklst_rel.rmk) )
	{
		sprintf(tmpstr,"rmk like '%s' and ",f_cif_blklst_rel.rmk );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_blklst_rel" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	ret=Cif_blklst_rel_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Cif_blklst_rel_Fet_Sel( &vcif_blklst_rel, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@信息来源|@关系状态|@上单原因|@建立日期|@解除日期|\n");
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_blklst_rel.cif_no<-2147000000) vcif_blklst_rel.cif_no=0;
		pub_base_dic_show_str(f_blalist_origin,"blalist_origin",vcif_blklst_rel.blalist_origin);
		pub_base_dic_show_str(f_relation_sts,"rel_sts",vcif_blklst_rel.relation_sts);
		fprintf( fp, "%ld|%s|%s|%s|%ld|%ld|\n",vcif_blklst_rel.cif_no,f_blalist_origin,f_relation_sts,vcif_blklst_rel.rmk,vcif_blklst_rel.crt_date,vcif_blklst_rel.cls_date);
		ttlnum++;
	}

	ret=Cif_blklst_rel_Clo_Sel( );
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
