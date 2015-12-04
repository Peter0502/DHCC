/*************************************************
* 文 件 名:  sp8917.c
* 功能描述： 证券回购台帐查询
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
#include "mo_stok_rebuy_prot_c.h"

int sp8917()
{
	struct mo_stok_rebuy_prot_c vmo_stok_rebuy_prot;
	struct mo_stok_rebuy_prot_c f_mo_stok_rebuy_prot;
	char ac_no2[25];
	char f_buy_type3[41];
	char f_tcht_type4[41];
	char f_sts16[41];
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

	get_zd_data("0630",f_mo_stok_rebuy_prot.pact_no ); /*证券回购协议编号*/
	get_zd_data("0670",f_mo_stok_rebuy_prot.buy_type ); /*证券回购类型*/
	get_zd_long("0290",&f_mo_stok_rebuy_prot.sign_date ); /*签约日期*/
	get_zd_long("0440",&f_mo_stok_rebuy_prot.matr_date ); /*到期日期*/
	get_zd_data("0260",f_mo_stok_rebuy_prot.brf );  /*备注*/
	get_zd_data("0700",f_mo_stok_rebuy_prot.sts );  /*状态*/

	/**组成查询条件**/
	pub_base_strpack( f_mo_stok_rebuy_prot.pact_no );
	if( strlen(f_mo_stok_rebuy_prot.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_stok_rebuy_prot.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_stok_rebuy_prot.buy_type );
	if( strlen(f_mo_stok_rebuy_prot.buy_type) )
	{
		sprintf( tmpstr," buy_type='%s' and",f_mo_stok_rebuy_prot.buy_type );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_stok_rebuy_prot.sign_date )
	{
		sprintf( tmpstr," sign_date=%ld and",f_mo_stok_rebuy_prot.sign_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_stok_rebuy_prot.matr_date )
	{
		sprintf( tmpstr," matr_date=%ld and",f_mo_stok_rebuy_prot.matr_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_stok_rebuy_prot.brf );
	if( strlen(f_mo_stok_rebuy_prot.brf) )
	{
		sprintf( tmpstr," brf='%s' and",f_mo_stok_rebuy_prot.brf );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_stok_rebuy_prot.sts );
	if( strlen(f_mo_stok_rebuy_prot.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_stok_rebuy_prot.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_stok_rebuy_prot" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,br_no,ac_id,buy_type,tcht_type,par_value,bal,sign_date,matr_date,term,pledge_rate,rate,opt_acno,opt_name,reg_date,brf,sts,tel");

	ret=Mo_stok_rebuy_prot_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_stok_rebuy_prot_Fet_Sel( &vmo_stok_rebuy_prot, g_pub_tx.reply );
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
			fprintf( fp,"~@证券回购协议编号|@机构编码|账号|@证券回购类型|@证券类型|$证券面值|$金额|签约日期|到期日期|抵押率|年收益率|@对方名称|登记日期|@备注|@状态|@操作员|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_stok_rebuy_prot.ac_id,vmo_stok_rebuy_prot.ac_seqn,ac_no2 );
		pub_base_dic_show_str(f_buy_type3,"buy_type",vmo_stok_rebuy_prot.buy_type);
		pub_base_dic_show_str(f_tcht_type4,"tcht_type",vmo_stok_rebuy_prot.tcht_type);
		if(vmo_stok_rebuy_prot.par_value<-2147000000) vmo_stok_rebuy_prot.par_value=0.0;
		if(vmo_stok_rebuy_prot.bal<-2147000000) vmo_stok_rebuy_prot.bal=0.0;
		if(vmo_stok_rebuy_prot.sign_date<-2147000000) vmo_stok_rebuy_prot.sign_date=0;
		if(vmo_stok_rebuy_prot.matr_date<-2147000000) vmo_stok_rebuy_prot.matr_date=0;
		if(vmo_stok_rebuy_prot.pledge_rate<-2147000000) vmo_stok_rebuy_prot.pledge_rate=0.0;
		if(vmo_stok_rebuy_prot.rate<-2147000000) vmo_stok_rebuy_prot.rate=0.0;
		if(vmo_stok_rebuy_prot.reg_date<-2147000000) vmo_stok_rebuy_prot.reg_date=0;
		pub_base_dic_show_str(f_sts16,"all_sts",vmo_stok_rebuy_prot.sts);

		fprintf( fp, "%s|%s|%s|%s|%s|%.2lf|%.2lf|%ld|%ld|%.5lf|%.5lf|%s|%ld|%s|%s|%s|\n",vmo_stok_rebuy_prot.pact_no,vmo_stok_rebuy_prot.br_no,ac_no2,f_buy_type3,f_tcht_type4,vmo_stok_rebuy_prot.par_value,vmo_stok_rebuy_prot.bal,vmo_stok_rebuy_prot.sign_date,vmo_stok_rebuy_prot.matr_date,vmo_stok_rebuy_prot.pledge_rate,vmo_stok_rebuy_prot.rate,vmo_stok_rebuy_prot.opt_name,vmo_stok_rebuy_prot.reg_date,vmo_stok_rebuy_prot.brf,f_sts16,vmo_stok_rebuy_prot.tel);
		ttlnum++;
	}

	ret=Mo_stok_rebuy_prot_Clo_Sel( );
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
