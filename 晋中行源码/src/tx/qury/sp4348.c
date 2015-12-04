/*************************************************
* 文 件 名:  sp4348.c
* 功能描述： 银行承兑汇票查询
*
* 作    者:  jane
* 完成日期： 2003年9月25日
*
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
#include "mo_bank_po_query_c.h"

int sp4348()
{
	struct 	mo_bank_po_query_c 		vmo_bank_po_query;
	struct 	mo_bank_po_query_c 		f_mo_bank_po_query;
	char 	f_po_sts7[41];
	char 	f_po_ind8[41];
	char	f_acc_po_sts5[41];
	char	f_acc_po_ind5[41];
	char	f_at_term_ind5[41];
	char	cPoNo2[9];							/* 新承兑汇票号码有效位 */
	char	cPoNoHead2[9];						/* 新承兑汇票号码冠字头 */
	char	cPoNo22[17];						/* 新承兑汇票号码冠字头+有效位 */
		char filename[100];
		char wherelist1[1024];  /**查询条件**/
		char tmpstr1[512];
		int ttlnum=0;			/**读取的总条数**/
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist1,0,sizeof(wherelist1) );
	memset( tmpstr1,0,sizeof(tmpstr1) );

	pub_base_sysinit();
	get_zd_data("0590",f_mo_bank_po_query.po_no );
	get_zd_long("0440",&f_mo_bank_po_query.query_date );
	get_zd_data("0580",f_mo_bank_po_query.query_br_no );

	/**组成查询条件**/
	pub_base_strpack( f_mo_bank_po_query.po_no );
	if( strlen(f_mo_bank_po_query.po_no) )
	{
		/* 取承兑汇票冠字头 */
		strncpy( cPoNoHead2, f_mo_bank_po_query.po_no, 8 );
		cPoNoHead2[8]='\0';
		pub_base_strpack( cPoNoHead2);

		/* 取承兑汇票有效位 */
		strncpy( cPoNo2, f_mo_bank_po_query.po_no+4, 8 );
		cPoNo2[8] = '\0';
sprintf( acErrMsg, "xxxx    cPoNo2[%s]", cPoNo2 );
WRITEMSG

		/* 承兑汇票号码冠字头+有效位 */
		sprintf( cPoNo22, " %s%s ", cPoNoHead2, cPoNo2 );
sprintf( acErrMsg, "xxxx    cPoNo22[%s]", cPoNo22 );
WRITEMSG
		pub_base_strpack_all( cPoNo22 );

		sprintf( tmpstr1," po_no like '%%%%%s%%%%' and",cPoNo22 );
		strcat( wherelist1,tmpstr1 );
	}
	if( f_mo_bank_po_query.query_date )
	{
		sprintf( tmpstr1," query_date=%ld and",f_mo_bank_po_query.query_date );
		strcat( wherelist1,tmpstr1 );
	}
	pub_base_strpack( f_mo_bank_po_query.query_br_no );
	if( strlen(f_mo_bank_po_query.query_br_no) )
	{
		sprintf( tmpstr1," query_br_no='%s' and",f_mo_bank_po_query.query_br_no );
		strcat( wherelist1,tmpstr1 );
	}

	/**组成查询**/
	strcat(wherelist1," 1=1") ;
	vtcp_log( "__xxxx111__WHERE [%s]",wherelist1 );

	ret=Mo_bank_po_query_Dec_Sel(g_pub_tx.reply,wherelist1 );
	ERR_DEAL
	ttlnum=0;

	while(1)
	{
		ret=Mo_bank_po_query_Fet_Sel( &vmo_bank_po_query, g_pub_tx.reply );
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

			fprintf( fp,"~登记日期|@登记机构|查询书日期|查询机构|@汇票号码|@查询内容|\n" );
		}

		if(vmo_bank_po_query.query_date<-2147000000) vmo_bank_po_query.query_date=0;
		if(vmo_bank_po_query.tx_date<-2147000000) vmo_bank_po_query.tx_date=0;
		fprintf( fp, "%ld|%s|%ld|%s|%s|%s|\n",vmo_bank_po_query.tx_date,vmo_bank_po_query.tx_br_no, \
			vmo_bank_po_query.query_date,vmo_bank_po_query.query_br_no, \
			vmo_bank_po_query.po_no,vmo_bank_po_query.query_brf );
		ttlnum++;
	}
	ret=Mo_bank_po_query_Clo_Sel( );
	ERR_DEAL

	/* fclose(fp); */

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
