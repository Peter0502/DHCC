/*************************************************
* 文 件 名:  spB317.c
* 功能描述： 本票挂失登记簿查询
*
* 作    者:  jane
* 完成日期： 2003年11月10日
*
* 修改记录：
*   日    期:
*   修 改 人:
*   修改内容:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_cashier_los_rpt_c.h"

int spB317()
{
	struct mo_cashier_los_rpt_c vmo_cashier_los_rpt;
	struct mo_cashier_los_rpt_c f_mo_cashier_los_rpt;
	struct mo_cashier_los_rpt_c f_mo_cashier_los_rpta;
	char filename[100];
	char wherelist[1024];  /**查询条件**/
	char tmpstr[512];
	char tmpmo[512];
	char tablelist[128];   /**要查询的表名**/
	char titstr[1024];
	int ttlnum=0;			/**读取的总条数**/
	int i=0;
	int ret=0;
	FILE *fp;
	char sts[20],id_type[20];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	if (get_sys_par_val("01",mbfetran.isdficode)) goto ErrExit;

	get_zd_data("0580",f_mo_cashier_los_rpt.cashier_no );
	get_zd_long("0440",&f_mo_cashier_los_rpt.los_rpt_date );
	get_zd_long("0450",&f_mo_cashier_los_rpta.los_rpt_date );

	/**组成查询条件**/
	pub_base_strpack( f_mo_cashier_los_rpt.cashier_no );
	if( strlen(f_mo_cashier_los_rpt.cashier_no) )
	{
		sprintf( tmpstr," cashier_no='%s' and",f_mo_cashier_los_rpt.cashier_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier_los_rpt.los_rpt_date )
	{
		sprintf( tmpstr," los_rpt_date>=%ld and",f_mo_cashier_los_rpt.los_rpt_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier_los_rpta.los_rpt_date )
	{
		sprintf( tmpstr," los_rpt_date<=%ld and",f_mo_cashier_los_rpta.los_rpt_date );
		strcat( wherelist,tmpstr );
	}

	sprintf( tmpstr," los_rpt_br_no='%s' and", g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	strcpy( tablelist,"mo_cashier_los_rpt" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by los_rpt_date ");

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_cashier_los_rpt_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Mo_cashier_los_rpt_Fet_Sel( &vmo_cashier_los_rpt, g_pub_tx.reply );
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
			fprintf( fp,"~@状态|@本票号码|挂失日期|@申请人证件类型|@证件号码|@名称|解挂日期|\n" );
		}

		if(vmo_cashier_los_rpt.los_rpt_date<-2147000000) vmo_cashier_los_rpt.los_rpt_date=0;
		if(vmo_cashier_los_rpt.clos_rpt_date<-2147000000) vmo_cashier_los_rpt.clos_rpt_date=0;

		pub_base_dic_show_str(sts,"los_rpt_sts",vmo_cashier_los_rpt.sts);
		pub_base_dic_show_str(id_type,"id_type",vmo_cashier_los_rpt.los_rpt_id_type);

		fprintf( fp, "%s|%s|%ld|%s|%s|%s|%ld|\n",
		pub_base_strpack(sts),
		pub_base_strpack(vmo_cashier_los_rpt.cashier_no),
		vmo_cashier_los_rpt.los_rpt_date,
		pub_base_strpack(id_type),
		pub_base_strpack(vmo_cashier_los_rpt.los_rpt_id_no),
		pub_base_strpack(vmo_cashier_los_rpt.los_rpt_name),
		vmo_cashier_los_rpt.clos_rpt_date);

		ttlnum++;
	}

	ret=Mo_cashier_los_rpt_Clo_Sel( );
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
