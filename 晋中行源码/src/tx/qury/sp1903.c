/*************************************************
* 文 件 名:  sp1903.c
* 功能描述： 客户资料修改日志查询
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
#include "cif_inf_chg_log_c.h"
#include "cif_id_code_rel_c.h"

int sp1903()
{
	struct cif_inf_chg_log_c vcif_inf_chg_log;
	struct cif_inf_chg_log_c f_cif_inf_chg_log;
	struct cif_id_code_rel_c f_cif_id_code_rel;
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
	memset( &vcif_inf_chg_log,0,sizeof(struct cif_inf_chg_log_c) );
	memset( &f_cif_inf_chg_log,0,sizeof(struct cif_inf_chg_log_c) );
	memset( &f_cif_id_code_rel,0,sizeof(struct cif_id_code_rel_c) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_inf_chg_log.cif_no );
	get_zd_data("0680",f_cif_id_code_rel.id_type );
	get_zd_data("0620",f_cif_id_code_rel.id_no );

	/**组成查询条件**/
	if( f_cif_inf_chg_log.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_inf_chg_log.cif_no );
		strcat( wherelist,tmpstr );
	}
	else if(strlen(f_cif_id_code_rel.id_type)&&strlen(f_cif_id_code_rel.id_no))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel, \
								"id_type='%s' and id_no='%s'", \
								f_cif_id_code_rel.id_type, \
								f_cif_id_code_rel.id_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"无匹配的纪录!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		f_cif_inf_chg_log.cif_no=f_cif_id_code_rel.cif_no;
		sprintf( tmpstr," cif_no=%ld and",f_cif_inf_chg_log.cif_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_inf_chg_log" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,chg_tablename,chg_columnname,chg_date,chg_time,bef_content,aft_content,clerk_no,memo");

	ret=Cif_inf_chg_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_inf_chg_log_Fet_Sel( &vcif_inf_chg_log, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@被修改客户信息表名|@被修改客户信息列名|修改日期|修改时间|@修改前内容|@修改后内容|@职员编号|@说明|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_inf_chg_log.cif_no<-2147000000) vcif_inf_chg_log.cif_no=0;
		if(vcif_inf_chg_log.chg_date<-2147000000) vcif_inf_chg_log.chg_date=0;
		if(vcif_inf_chg_log.chg_time<-2147000000) vcif_inf_chg_log.chg_time=0;
		vtcp_log("[%s][%d] bef_content==[%s]",__FILE__,__LINE__,vcif_inf_chg_log.bef_content);
		vtcp_log("[%s][%d] aft_content==[%s]",__FILE__,__LINE__,vcif_inf_chg_log.aft_content);

		fprintf( fp, "%ld|%s|%s|%ld|%ld|%s|%s|%s|%s|\n",vcif_inf_chg_log.cif_no,vcif_inf_chg_log.chg_tablename,vcif_inf_chg_log.chg_columnname,vcif_inf_chg_log.chg_date,vcif_inf_chg_log.chg_time,vcif_inf_chg_log.bef_content,vcif_inf_chg_log.aft_content,vcif_inf_chg_log.clerk_no,vcif_inf_chg_log.memo);
		ttlnum++;
	}

	ret=Cif_inf_chg_log_Clo_Sel( );
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
