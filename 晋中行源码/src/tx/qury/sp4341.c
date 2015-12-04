/*************************************************
* 文 件 名:  sp4341.c
* 功能描述： 保兑仓登记簿查询
*
* 作    者:  jane
* 完成日期： 2003年8月11日
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
#include "mo_sg_cash_po_c.h"

int sp4341()
{
	struct mo_sg_cash_po_c vmo_sg_cash_po;
	struct mo_sg_cash_po_c f_mo_sg_cash_po;
	struct mo_sg_cash_po_c f_mo_sg_cash_poa;
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

	get_zd_data("0590",f_mo_sg_cash_po.pact_no );
	get_zd_data("0910",f_mo_sg_cash_po.br_no );
	get_zd_double("0400",&f_mo_sg_cash_po.pickup_sum );
	get_zd_double("0410",&f_mo_sg_cash_poa.pickup_sum );
	get_zd_double("0420",&f_mo_sg_cash_po.safeg_sum );
	get_zd_double("0390",&f_mo_sg_cash_poa.safeg_sum );

	/**组成查询条件**/
	pub_base_strpack( f_mo_sg_cash_po.pact_no );
	if( strlen(f_mo_sg_cash_po.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_sg_cash_po.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_sg_cash_po.br_no );
	if( strlen(f_mo_sg_cash_po.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_mo_sg_cash_po.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_po.pickup_sum )
	{
		sprintf( tmpstr," pickup_sum>=%lf and",f_mo_sg_cash_po.pickup_sum );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_poa.pickup_sum )
	{
		sprintf( tmpstr," pickup_sum<=%lf and",f_mo_sg_cash_poa.pickup_sum );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_po.safeg_sum )
	{
		sprintf( tmpstr," safeg_sum>=%lf and",f_mo_sg_cash_po.safeg_sum );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_sg_cash_poa.safeg_sum )
	{
		sprintf( tmpstr," safeg_sum<=%lf and",f_mo_sg_cash_poa.safeg_sum );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_sg_cash_po" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_sg_cash_po_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_sg_cash_po_Fet_Sel( &vmo_sg_cash_po, g_pub_tx.reply );
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
			fprintf( fp,"~@承兑协议编号|@缴存保证金机构编码|$已开提货单总金额|$已存保证金总金额|\n" );
		}

		if(vmo_sg_cash_po.pickup_sum<-2147000000) vmo_sg_cash_po.pickup_sum=0.0;
		if(vmo_sg_cash_po.safeg_sum<-2147000000) vmo_sg_cash_po.safeg_sum=0.0;

		fprintf( fp, "%s|%s|%.2lf|%.2lf|\n",vmo_sg_cash_po.pact_no,vmo_sg_cash_po.br_no,vmo_sg_cash_po.pickup_sum,vmo_sg_cash_po.safeg_sum);
		ttlnum++;
	}

	ret=Mo_sg_cash_po_Clo_Sel( );
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
