/*************************************************
* 文 件 名:  sp8304.c
* 功能描述： 支票透支登记簿查询
*
* 作    者:  jane
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
#include "mo_cheq_od_c.h"

int sp8304()
{
	struct mo_cheq_od_c vmo_cheq_od;
	struct mo_cheq_od_c f_mo_cheq_od;
	char ac_no1[25];
	char f_note_type3[41];
	char f_acno0[25];
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

	get_zd_data("0380",f_acno0);
	get_zd_long("0510",&f_mo_cheq_od.ac_seqn );
	get_zd_data("0890",f_mo_cheq_od.note_type );
	get_zd_data("0590",f_mo_cheq_od.note_no );
	get_zd_data("0260",f_mo_cheq_od.brf );

	/**组成查询条件**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_mo_cheq_od.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_mo_cheq_od.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cheq_od.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_mo_cheq_od.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_cheq_od.note_type );
	if( strlen(f_mo_cheq_od.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_mo_cheq_od.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_cheq_od.note_no );
	if( strlen(f_mo_cheq_od.note_no) )
	{
		sprintf( tmpstr," note_no='%s' and",f_mo_cheq_od.note_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_cheq_od.brf );
	if( strlen(f_mo_cheq_od.brf) )
	{
		sprintf( tmpstr," brf like '%%%%%s%%%%' and",f_mo_cheq_od.brf );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_cheq_od" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY ac_id,ac_seqn,note_type ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,ac_id,ac_seqn,note_type,note_no,brf,tel,tx_date");

	ret=Mo_cheq_od_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_cheq_od_Fet_Sel( &vmo_cheq_od, g_pub_tx.reply );
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
			fprintf( fp,"~@凭证种类|@凭证号|   账    号|序号|@备注|@操作员|交易日期|流水号|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_cheq_od.trace_no<-2147000000) vmo_cheq_od.trace_no=0;
		pub_base_acid_acno( vmo_cheq_od.ac_id,vmo_cheq_od.ac_seqn,ac_no1 );
		pub_base_dic_show_str(f_note_type3,"note_type",vmo_cheq_od.note_type);
		if(vmo_cheq_od.tx_date<-2147000000) vmo_cheq_od.tx_date=0;

		fprintf( fp, "%s|%s|%s|%d|%s|%s|%ld|%ld|\n",f_note_type3,vmo_cheq_od.note_no,ac_no1,vmo_cheq_od.ac_seqn,vmo_cheq_od.brf,vmo_cheq_od.tel,vmo_cheq_od.tx_date,vmo_cheq_od.trace_no);
		ttlnum++;
	}

	ret=Mo_cheq_od_Clo_Sel( );
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
