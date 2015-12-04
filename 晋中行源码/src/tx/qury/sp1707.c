/*************************************************
* 文 件 名:  sp1707.c
* 功能描述： 客户工作信息查询
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
#include "cif_wrk_hst_inf_c.h"

int sp1707()
{
	struct cif_wrk_hst_inf_c vcif_wrk_hst_inf;
	struct cif_wrk_hst_inf_c f_cif_wrk_hst_inf;
	char tmp1[41],tmp2[41],tmp3[41];
	char f_wrk_ind6[41];
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

	get_zd_long("0540",&f_cif_wrk_hst_inf.cif_no );
	get_zd_long("0510",&f_cif_wrk_hst_inf.wrk_no );
	get_zd_data("0240",f_cif_wrk_hst_inf.industry_no );
	get_zd_long("0290",&f_cif_wrk_hst_inf.beg_date );
	get_zd_long("0440",&f_cif_wrk_hst_inf.end_date );

	/**组成查询条件**/
	if( f_cif_wrk_hst_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_wrk_hst_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_wrk_hst_inf.wrk_no )
	{
		sprintf( tmpstr," wrk_no=%d and",f_cif_wrk_hst_inf.wrk_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_cif_wrk_hst_inf.industry_no );
	if( strlen(f_cif_wrk_hst_inf.industry_no) )
	{
		sprintf( tmpstr," industry_no='%s' and",f_cif_wrk_hst_inf.industry_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_wrk_hst_inf.beg_date )
	{
		sprintf( tmpstr," beg_date>=%ld and",f_cif_wrk_hst_inf.beg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_wrk_hst_inf.end_date )
	{
		sprintf( tmpstr," end_date<=%ld and",f_cif_wrk_hst_inf.end_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_wrk_hst_inf" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,wrk_no,industry_no,vocation_no,position_no,beg_date,wrk_ind,end_date,unit_name,income");

	ret=Cif_wrk_hst_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_wrk_hst_inf_Fet_Sel( &vcif_wrk_hst_inf, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
	vtcp_log( "WHERE [%s]",wherelist );
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
			fprintf( fp,"~客户代码|序号|@行业代码|@职业代码|@职位代码|开始年月|@工作状态|结束年月|@单位名称|$年薪收入|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */
		pub_base_dic_show_str(tmp1,"industry_no",vcif_wrk_hst_inf.industry_no);
		pub_base_dic_show_str(tmp2,"vocation_no",vcif_wrk_hst_inf.vocation_no);
		pub_base_dic_show_str(tmp3,"position_no",vcif_wrk_hst_inf.position_no);

		if(vcif_wrk_hst_inf.cif_no<-2147000000) vcif_wrk_hst_inf.cif_no=0;
		if(vcif_wrk_hst_inf.beg_date<-2147000000) vcif_wrk_hst_inf.beg_date=0;
		pub_base_dic_show_str(f_wrk_ind6,"wrk_ind",vcif_wrk_hst_inf.wrk_ind);
		if(vcif_wrk_hst_inf.end_date<-2147000000) vcif_wrk_hst_inf.end_date=0;
		if(vcif_wrk_hst_inf.income<-2147000000) vcif_wrk_hst_inf.income=0.0;

		fprintf( fp, "%ld|%d|%s|%s|%s|%ld|%s|%ld|%s|%.2lf|\n",vcif_wrk_hst_inf.cif_no,vcif_wrk_hst_inf.wrk_no,tmp1,tmp2,tmp3,vcif_wrk_hst_inf.beg_date,f_wrk_ind6,vcif_wrk_hst_inf.end_date,vcif_wrk_hst_inf.unit_name,vcif_wrk_hst_inf.income);
		ttlnum++;
	}

	ret=Cif_wrk_hst_inf_Clo_Sel( );
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
