/*************************************************
* 文 件 名:  sp1603.c
* 功能描述： 客户建议/投诉信息查询
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
#include "cif_req_log_c.h"
#include "cif_basic_inf_c.h"

int sp1603()
{
	struct cif_req_log_c vcif_req_log;
	struct cif_req_log_c f_cif_req_log;
		char f_advice_type[41];
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

	get_zd_long("0280",&f_cif_req_log.cif_no);
	get_zd_data("0670",f_cif_req_log.advice_type);
	get_zd_data("0810",f_cif_req_log.request);
	get_zd_data("0920",f_cif_req_log.req_clerk_no);

	/**组成查询条件**/
	if( f_cif_req_log.cif_no )
	{
		sprintf(tmpstr,"cif_no=%ld and ",f_cif_req_log.cif_no);
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_req_log.advice_type) )
	{
		sprintf(tmpstr,"advice_type='%s' and ",f_cif_req_log.advice_type );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_req_log.request) )
	{
		sprintf(tmpstr,"request like '%%%%%%s%%%%' and ",f_cif_req_log.request );
		strcat( wherelist,tmpstr );
	}

	if( strlen(f_cif_req_log.req_clerk_no) )
	{
		sprintf(tmpstr,"req_clerk_no like '%%%%%s%%%%' and ",f_cif_req_log.req_clerk_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_req_log" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	ret=Cif_req_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Cif_req_log_Fet_Sel( &vcif_req_log, g_pub_tx.reply );
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
			fprintf( fp,"~客户代码|@序号|@建议类型|@投诉对象|@具体信息|@录入日期|@职员编号|\n");
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_req_log.cif_no<-2147000000) vcif_req_log.cif_no=0;
		pub_base_dic_show_str(f_advice_type,"advice_type",vcif_req_log.advice_type);
		fprintf( fp, "%ld|%d|%s|%s|%s|%ld|%s|\n",vcif_req_log.cif_no,vcif_req_log.seq_no,f_advice_type,vcif_req_log.req_clerk_no,vcif_req_log.request,vcif_req_log.date,vcif_req_log.clerk_no);
		ttlnum++;
	}

	ret=Cif_req_log_Clo_Sel( );
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
