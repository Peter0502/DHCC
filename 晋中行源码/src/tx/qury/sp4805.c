/*************************************************
* 文 件 名:  sp4805.c
* 功能描述： 再贷款信息查询
*
* 作    者:  lance
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
#include "mo_again_ln_c.h"

int sp4805()
{
	struct mo_again_ln_c vmo_again_ln;
	struct mo_again_ln_c f_mo_again_ln;
	char ac_no1[25];
	char f_sts14[41];
	char f_sts15[41];
	char f_acno1[25];
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

	get_zd_data("0600",f_mo_again_ln.pact_no );
	get_zd_data("0380",f_acno1);
	get_zd_data("0910",f_mo_again_ln.br_no );
	get_zd_double("0400",&f_mo_again_ln.amt );
	get_zd_double("0840",&f_mo_again_ln.rate );
	get_zd_long("0290",&f_mo_again_ln.to_date );
	get_zd_long("0440",&f_mo_again_ln.reg_date );
	get_zd_data("0650",f_mo_again_ln.tel );
	get_zd_data("0670",f_mo_again_ln.again_ln_type );
	get_zd_data("0700",f_mo_again_ln.sts );

	/**组成查询条件**/
	pub_base_strpack( f_mo_again_ln.pact_no );
	if( strlen(f_mo_again_ln.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_again_ln.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_acno1 );
	if( strlen(f_acno1) )
	{
		if ( pub_base_acno_acid(&f_mo_again_ln.ac_id,f_acno1) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_mo_again_ln.ac_id );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_again_ln.br_no );
	if( strlen(f_mo_again_ln.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_mo_again_ln.br_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_again_ln.amt )
	{
		sprintf( tmpstr," amt=%lf and",f_mo_again_ln.amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_again_ln.rate )
	{
		sprintf( tmpstr," rate=%lf and",f_mo_again_ln.rate );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_again_ln.to_date )
	{
		sprintf( tmpstr," to_date=%ld and",f_mo_again_ln.to_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_again_ln.reg_date )
	{
		sprintf( tmpstr," reg_date=%ld and",f_mo_again_ln.reg_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_again_ln.tel );
	if( strlen(f_mo_again_ln.tel) )
	{
		sprintf( tmpstr," tel='%s' and",f_mo_again_ln.tel );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_again_ln.again_ln_type );
	if( strlen(f_mo_again_ln.again_ln_type) )
	{
		sprintf( tmpstr," again_ln_type='%s' and",f_mo_again_ln.again_ln_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_again_ln.sts );
	if( strlen(f_mo_again_ln.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_again_ln.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_again_ln" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,ac_id,br_no,prdt_no,amt,sum_amt,rate,to_date,reg_date,pay_date,tel,chk,auth,again_ln_type,sts");

	ret=Mo_again_ln_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_again_ln_Fet_Sel( &vmo_again_ln, g_pub_tx.reply );
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
			fprintf( fp,"~@协议编号|@账号|@再贷款录入机构编码|@产品编号|$贷款金额|$已还款总金额|利率|到期日期|登记日期|还款日期|@操作员|@复合员|@授权人|@再贷款类型|@标志|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_again_ln.ac_id,0,ac_no1 );
		if(vmo_again_ln.amt<-2147000000) vmo_again_ln.amt=0.0;
		if(vmo_again_ln.sum_amt<-2147000000) vmo_again_ln.sum_amt=0.0;
		if(vmo_again_ln.rate<-2147000000) vmo_again_ln.rate=0.0;
		if(vmo_again_ln.to_date<-2147000000) vmo_again_ln.to_date=0;
		if(vmo_again_ln.reg_date<-2147000000) vmo_again_ln.reg_date=0;
		if(vmo_again_ln.pay_date<-2147000000) vmo_again_ln.pay_date=0;
		pub_base_dic_show_str(f_sts14,"again_ln_sts",vmo_again_ln.sts);
		pub_base_dic_show_str(f_sts15,"again_ln_type",vmo_again_ln.again_ln_type);

		fprintf( fp, "%s|%s|%s|%s|%.2lf|%.2lf|%.6lf|%ld|%ld|%ld|%s|%s|%s|%s|%s|\n",vmo_again_ln.pact_no,ac_no1,vmo_again_ln.br_no,vmo_again_ln.prdt_no,vmo_again_ln.amt,vmo_again_ln.sum_amt,vmo_again_ln.rate,vmo_again_ln.to_date,vmo_again_ln.reg_date,vmo_again_ln.pay_date,vmo_again_ln.tel,vmo_again_ln.chk,vmo_again_ln.auth,f_sts15,f_sts14);
		ttlnum++;
	}

	ret=Mo_again_ln_Clo_Sel( );
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
