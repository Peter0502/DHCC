/*************************************************
* 文 件 名:  sp8109.c
* 功能描述： 长短款登记簿查询
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
#include "mo_rmny_smny_c.h"

int sp8109()
{
	struct mo_rmny_smny_c vmo_rmny_smny;
	struct mo_rmny_smny_c f_mo_rmny_smny;
	struct mo_rmny_smny_c f_mo_rmny_smnya;
	char f_tx_opmd3[41];
		char filename[100];
		char wherelist[1024];  /**查询条件**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**要查询的字段列表**/
		char tablelist[128];   /**要查询的表名**/
		char titstr[1024];
		char f_cur_name[21];
		int ttlnum=0;			/**读取的总条数**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_mo_rmny_smny.br_no );
	get_zd_long("0290",&f_mo_rmny_smny.occ_date );
	get_zd_data("0700",f_mo_rmny_smny.tx_opmd );
	get_zd_double("1001",&f_mo_rmny_smny.amt );
	get_zd_double("1002",&f_mo_rmny_smnya.amt );
	get_zd_long("0440",&f_mo_rmny_smny.tx_date );
	get_zd_long("0450",&f_mo_rmny_smnya.tx_date );
	get_zd_data("0640",f_mo_rmny_smny.occ_tel );
	get_zd_data("0260",f_mo_rmny_smny.brf );

	/**组成查询条件**/
	pub_base_strpack( f_mo_rmny_smny.br_no );
	if( strlen(f_mo_rmny_smny.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_mo_rmny_smny.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rmny_smny.occ_date )
	{
		sprintf( tmpstr," occ_date=%ld and",f_mo_rmny_smny.occ_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rmny_smny.tx_opmd );
	if( strlen(f_mo_rmny_smny.tx_opmd) )
	{
		sprintf( tmpstr," tx_opmd='%s' and",f_mo_rmny_smny.tx_opmd );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rmny_smny.amt )
	{
		sprintf( tmpstr," amt>=%lf and",f_mo_rmny_smny.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rmny_smnya.amt )
	{
		sprintf( tmpstr," amt<=%lf and",f_mo_rmny_smnya.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rmny_smny.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_mo_rmny_smny.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_rmny_smnya.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_mo_rmny_smnya.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rmny_smny.occ_tel );
	if( strlen(f_mo_rmny_smny.occ_tel) )
	{
		sprintf( tmpstr," occ_tel='%s' and",f_mo_rmny_smny.occ_tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_rmny_smny.brf );
	if( strlen(f_mo_rmny_smny.brf) )
	{
		sprintf( tmpstr," brf='%s' and",f_mo_rmny_smny.brf );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_rmny_smny" );

	/**组成查询**/
	strcat(wherelist," 1=1 ORDER BY tx_opmd,occ_date,br_no,occ_tel,cur_no,amt ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"br_no,occ_date,seqn,tx_opmd,cur_no,amt,tx_date,trace_no,occ_tel,brf,tel,chk,auth,occ_br_no");

	ret=Mo_rmny_smny_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_rmny_smny_Fet_Sel( &vmo_rmny_smny, g_pub_tx.reply );
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
			fprintf( fp,"~序号|@性质|@币  种|$金    额|发生日期|@发生机构|@发生长短人|@摘要|交易日期|@交易机构|@操作员|@复核员|@授权人|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vmo_rmny_smny.occ_date<-2147000000) vmo_rmny_smny.occ_date=0;
		if(vmo_rmny_smny.seqn<-2147000000) vmo_rmny_smny.seqn=0;
		pub_base_dic_show_str(f_tx_opmd3,"tx_opmd",vmo_rmny_smny.tx_opmd);
		if(vmo_rmny_smny.amt<-2147000000) vmo_rmny_smny.amt=0.0;
		if(vmo_rmny_smny.tx_date<-2147000000) vmo_rmny_smny.tx_date=0;
		if(vmo_rmny_smny.trace_no<-2147000000) vmo_rmny_smny.trace_no=0;
		pub_base_dic_show_str(f_cur_name,"cur_name",vmo_rmny_smny.cur_no);
		fprintf( fp, "%ld|%s|%s|%.2lf|%ld|%s|%s|%s|%ld|%s|%s|%s|%s|\n",vmo_rmny_smny.seqn,f_tx_opmd3,f_cur_name,vmo_rmny_smny.amt,vmo_rmny_smny.occ_date,vmo_rmny_smny.occ_br_no,vmo_rmny_smny.occ_tel,vmo_rmny_smny.brf,vmo_rmny_smny.tx_date,vmo_rmny_smny.br_no,vmo_rmny_smny.tel,vmo_rmny_smny.chk,vmo_rmny_smny.auth);
		ttlnum++;
	}

	ret=Mo_rmny_smny_Clo_Sel( );
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
