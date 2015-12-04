/*************************************************
* 文 件 名:  spB372.c
* 功能描述： 保证金利率登记薄查询
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
#include "mo_bail_rate_c.h"

int spB372()
{
	struct mo_bail_rate_c vmo_bail_rate;
	struct mo_bail_rate_c f_mo_bail_rate;
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
		char tmpstr1[21];
		char tmpstr2[21];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	get_zd_data("0670",f_mo_bail_rate.bail_type );
	get_zd_data("0330",f_mo_bail_rate.pact_no );
	get_zd_data("0320",f_mo_bail_rate.bail_ac_no );
	get_zd_data("0310",f_mo_bail_rate.ac_no );
	get_zd_long("0440",&f_mo_bail_rate.beg_date );

	/**组成查询条件**/
	pub_base_strpack( f_mo_bail_rate.pact_no );
	if( strlen(f_mo_bail_rate.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and ",f_mo_bail_rate.pact_no );
		strcat( wherelist,tmpstr );
	}
	
	pub_base_strpack( f_mo_bail_rate.bail_type );
	if( strlen(f_mo_bail_rate.bail_type) )
	{
		sprintf( tmpstr," bail_type='%s'  and ",f_mo_bail_rate.bail_type );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_mo_bail_rate.bail_ac_no );
	if( strlen(f_mo_bail_rate.bail_ac_no) )
	{
		sprintf( tmpstr," bail_ac_no='%s' and",f_mo_bail_rate.bail_ac_no );
		strcat( wherelist,tmpstr );
	}

	pub_base_strpack( f_mo_bail_rate.ac_no );
	if( strlen(f_mo_bail_rate.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and",f_mo_bail_rate.ac_no );
		strcat( wherelist,tmpstr );
	}

	if( f_mo_bail_rate.beg_date!=0 )
	{
		sprintf( tmpstr," beg_date='%ld' and",f_mo_bail_rate.beg_date );
		strcat( wherelist,tmpstr );
	}
	strcpy( tablelist,"mo_bail_rate" );

	/**组成查询**/
	sprintf(tmpstr,"  br_no='%s' order by pact_no ",g_pub_tx.tx_br_no);
	strcat(wherelist,tmpstr) ;
	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_bail_rate_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;
	while(1)
	{
		ret=Mo_bail_rate_Fet_Sel( &vmo_bail_rate, g_pub_tx.reply );
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
			fprintf(fp,"~@机构编码|@保证金类型|@协议编号|@保证金帐号|@收息帐号|$计息金额|$计息利率|@起息日期|@到期日期|@计息日期|$利息金额|@是否已经计息|\n" );
		}

		if( vmo_bail_rate.bail_type[0]=='1' )
			strcpy(tmpstr1,"承兑保证金");
		else if( vmo_bail_rate.bail_type[0]=='2' )
			strcpy(tmpstr1,"保函保证金");
		else
			strcpy(tmpstr1,"其他保证金");
		
		if( vmo_bail_rate.sts[0]=='1' )
			strcpy(tmpstr2,"未计息");
		else if( vmo_bail_rate.sts[0]=='*' )
			strcpy(tmpstr2,"已经计息");
		else
			strcpy(tmpstr2,"");

		fprintf( fp, "%s|%s|%s|%s|%s|%.2f|%.5f|%ld|%ld|%ld|%.2f|%s|\n",vmo_bail_rate.br_no,tmpstr1,vmo_bail_rate.pact_no,vmo_bail_rate.bail_ac_no,vmo_bail_rate.ac_no,vmo_bail_rate.amt,vmo_bail_rate.ic_rate,vmo_bail_rate.beg_date,vmo_bail_rate.end_date,vmo_bail_rate.intst_date,vmo_bail_rate.intst_amt,tmpstr2);
		ttlnum++;
	}

	ret=Mo_bail_rate_Clo_Sel( );
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
