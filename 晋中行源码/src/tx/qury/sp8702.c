/*************************************************
* 文 件 名:  sp8702.c
* 功能描述： 费率表查询
*
* 作    者:  
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
#include "com_chrg_rate_c.h"

int sp8702()
{
	struct com_chrg_rate_c vcom_chrg_rate;
	struct com_chrg_rate_c f_com_chrg_rate;
	char f_type6[41],f_type5[10];
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

	get_zd_data("0210",f_com_chrg_rate.cur_no );
	get_zd_data("0240",f_com_chrg_rate.chrg_rate_code );
	get_zd_long("0290",&f_com_chrg_rate.beg_date );
	get_zd_long("0440",&f_com_chrg_rate.end_date );
	get_zd_data("0750",f_com_chrg_rate.name );
	get_zd_data("0700",f_com_chrg_rate.type );
	get_zd_double("XXXX",&f_com_chrg_rate.val );
	get_zd_double("1001",&f_com_chrg_rate.min_amt );
	get_zd_double("1002",&f_com_chrg_rate.max_amt );

	/**组成查询条件**/
	pub_base_strpack( f_com_chrg_rate.cur_no );
	if( strlen(f_com_chrg_rate.cur_no) )
	{
		sprintf( tmpstr," cur_no='%s' and",f_com_chrg_rate.cur_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_chrg_rate.chrg_rate_code );
	if( strlen(f_com_chrg_rate.chrg_rate_code) )
	{
		sprintf( tmpstr," chrg_rate_code='%s' and",f_com_chrg_rate.chrg_rate_code );
		strcat( wherelist,tmpstr );
	}
	if( f_com_chrg_rate.beg_date )
	{
		sprintf( tmpstr," beg_date=%ld and",f_com_chrg_rate.beg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_com_chrg_rate.end_date )
	{
		sprintf( tmpstr," end_date=%ld and",f_com_chrg_rate.end_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_chrg_rate.name );
	if( strlen(f_com_chrg_rate.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_com_chrg_rate.name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_com_chrg_rate.type );
	if( strlen(f_com_chrg_rate.type) )
	{
		sprintf( tmpstr," type='%s' and",f_com_chrg_rate.type );
		strcat( wherelist,tmpstr );
	}
	if( f_com_chrg_rate.val )
	{
		sprintf( tmpstr," val=%lf and",f_com_chrg_rate.val );
		strcat( wherelist,tmpstr );
	}
	if( f_com_chrg_rate.min_amt )
	{
		sprintf( tmpstr," min_amt=%lf and",f_com_chrg_rate.min_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_com_chrg_rate.max_amt )
	{
		sprintf( tmpstr," max_amt=%lf and",f_com_chrg_rate.max_amt );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"com_chrg_rate" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cur_no,chrg_rate_code,beg_date,end_date,acc_hrt,name,type,val,min_amt,max_amt");

	ret=Com_chrg_rate_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Com_chrg_rate_Fet_Sel( &vcom_chrg_rate, g_pub_tx.reply );
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
			fprintf( fp,"~@币种|@费率编号|起始日期|终止日期|@名称|$费率值|$金额下限|$金额上限|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcom_chrg_rate.beg_date<-2147000000) vcom_chrg_rate.beg_date=0;
		if(vcom_chrg_rate.end_date<-2147000000) vcom_chrg_rate.end_date=0;
		pub_base_dic_show_str(f_type6,"chrg_type",vcom_chrg_rate.type);
		pub_base_dic_show_str(f_type5,"cur_name",vcom_chrg_rate.cur_no);
		if(vcom_chrg_rate.val<-2147000000) vcom_chrg_rate.val=0.0;
		if(vcom_chrg_rate.min_amt<-2147000000) vcom_chrg_rate.min_amt=0.0;
		if(vcom_chrg_rate.max_amt<-2147000000) vcom_chrg_rate.max_amt=0.0;

		fprintf( fp, "%.6s|%s|%ld|%ld|%s|%.4lf|%.2lf|%.2lf|\n",f_type5,vcom_chrg_rate.chrg_rate_code,vcom_chrg_rate.beg_date,vcom_chrg_rate.end_date,vcom_chrg_rate.name,vcom_chrg_rate.val,vcom_chrg_rate.min_amt,vcom_chrg_rate.max_amt);
		ttlnum++;
	}

	ret=Com_chrg_rate_Clo_Sel( );
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
