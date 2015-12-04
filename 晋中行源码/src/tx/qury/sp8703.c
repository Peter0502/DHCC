/*************************************************
* 文 件 名:  sp8703.c
* 功能描述： 汇率表查询
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
#include "com_exch_rate_c.h"

int sp8703()
{
	struct com_exch_rate_c vcom_exch_rate;
	struct com_exch_rate_c f_com_exch_rate;
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

	get_zd_data("0210",f_com_exch_rate.cur_no1 );
	get_zd_data("0220",f_com_exch_rate.cur_no2 );
	get_zd_long("0290",&f_com_exch_rate.date );

	/**组成查询条件**/
	pub_base_strpack( f_com_exch_rate.cur_no1 );
	if( strlen(f_com_exch_rate.cur_no1) )
	{
		sprintf( tmpstr," cur_no1='%s' and",f_com_exch_rate.cur_no1 );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_exch_rate.cur_no2 );
	if( strlen(f_com_exch_rate.cur_no2) )
	{
		sprintf( tmpstr," cur_no2='%s' and",f_com_exch_rate.cur_no2 );
		strcat( wherelist,tmpstr );
	}
	if( f_com_exch_rate.date )
	{
		sprintf( tmpstr," \"date\"=%ld and",f_com_exch_rate.date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"com_exch_rate" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cur_no1,cur_no2,unit,od_buy,od_sale,dt_buy,dt_sale,mdl,date");

	ret=Com_exch_rate_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Com_exch_rate_Fet_Sel( &vcom_exch_rate, g_pub_tx.reply );
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
			fprintf( fp,"~@币种|@币种2|单位多少元|钞买价|钞卖价|汇买价|汇卖价|外管局中间价|日期|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcom_exch_rate.unit<-2147000000) vcom_exch_rate.unit=0;
		if(vcom_exch_rate.od_buy<-2147000000) vcom_exch_rate.od_buy=0.0;
		if(vcom_exch_rate.od_sale<-2147000000) vcom_exch_rate.od_sale=0.0;
		if(vcom_exch_rate.dt_buy<-2147000000) vcom_exch_rate.dt_buy=0.0;
		if(vcom_exch_rate.dt_sale<-2147000000) vcom_exch_rate.dt_sale=0.0;
		if(vcom_exch_rate.mdl<-2147000000) vcom_exch_rate.mdl=0.0;
		if(vcom_exch_rate.date<-2147000000) vcom_exch_rate.date=0;

		fprintf( fp, "%s|%s|%ld|%.4lf|%.4lf|%.4lf|%.4lf|%.4lf|%ld|\n",vcom_exch_rate.cur_no1,vcom_exch_rate.cur_no2,vcom_exch_rate.unit,vcom_exch_rate.od_buy,vcom_exch_rate.od_sale,vcom_exch_rate.dt_buy,vcom_exch_rate.dt_sale,vcom_exch_rate.mdl,vcom_exch_rate.date);
		ttlnum++;
	}

	ret=Com_exch_rate_Clo_Sel( );
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
