/*************************************************
* 文 件 名:  sp4926.c
* 功能描述： 转贴现转入信息查询
*
* 作    者:  mike
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
#include "mo_discnt_c.h"

int sp4926()
{
	struct mo_discnt_c vmo_discnt;
	struct mo_discnt_c f_mo_discnt;
	struct mo_discnt_c f_mo_discnta;
	char ac_no1[25];
	char f_pn_type5[41];
	char f_sts28[41];
	char f_note_type23[41];
	char f_local_ind9[5];
	char f_pay_ac_type12[5];
	char f_get_ac_type13[5];
	char f_tpay_ac_type14[5];
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

	get_zd_data("0380",f_mo_discnt.pact_no );
	get_zd_double("0840",&f_mo_discnt.pn_rate );
	get_zd_long("0290",&f_mo_discnt.pn_date );
	get_zd_data("0670",f_mo_discnt.local_ind );
	get_zd_data("0260",f_mo_discnt.pn_name );
	get_zd_data("0700",f_mo_discnt.get_ac_type );
	get_zd_double("0400",&f_mo_discnt.par_amt );
	get_zd_double("0410",&f_mo_discnta.par_amt );
	get_zd_long("0440",&f_mo_discnt.not_matr_date );
	get_zd_data("0240",f_mo_discnt.note_type );
	get_zd_data("0600",f_mo_discnt.pnote_no );
	get_zd_long("0510",&f_mo_discnt.way_days );
	get_zd_data("0680",f_mo_discnt.sts );

	/**组成查询条件**/
	pub_base_strpack( f_mo_discnt.pact_no );
	if( strlen(f_mo_discnt.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_discnt.pact_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnt.pn_rate )
	{
		sprintf( tmpstr," pn_rate=%lf and",f_mo_discnt.pn_rate );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnt.pn_date )
	{
		sprintf( tmpstr," pn_date=%ld and",f_mo_discnt.pn_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.local_ind );
	if( strlen(f_mo_discnt.local_ind) )
	{
		sprintf( tmpstr," local_ind='%s' and",f_mo_discnt.local_ind );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.pn_name );
	if( strlen(f_mo_discnt.pn_name) )
	{
		sprintf( tmpstr," pn_name like '%%%%%s%%%%' and",f_mo_discnt.pn_name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.get_ac_type );
	if( strlen(f_mo_discnt.get_ac_type) )
	{
		sprintf( tmpstr," get_ac_type='%s' and",f_mo_discnt.get_ac_type );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnt.par_amt )
	{
		sprintf( tmpstr," par_amt>=%lf and",f_mo_discnt.par_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnta.par_amt )
	{
		sprintf( tmpstr," par_amt<=%lf and",f_mo_discnta.par_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnt.not_matr_date )
	{
		sprintf( tmpstr," not_matr_date=%ld and",f_mo_discnt.not_matr_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.note_type );
	if( strlen(f_mo_discnt.note_type) )
	{
		sprintf( tmpstr," note_type='%s' and",f_mo_discnt.note_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.pnote_no );
	if( strlen(f_mo_discnt.pnote_no) )
	{
		sprintf( tmpstr," pnote_no='%s' and",f_mo_discnt.pnote_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_discnt.way_days )
	{
		sprintf( tmpstr," way_days=%d and",f_mo_discnt.way_days );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_discnt.sts );
	if( strlen(f_mo_discnt.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_discnt.sts );
		strcat( wherelist,tmpstr );
	}

	strcat( wherelist, " pn_type in ('2','3') and " );

	strcpy( tablelist,"mo_discnt" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,ac_id,ac_seqn,br_no,bkrl_code,pn_type,tranx_bal,pn_rate,pn_date,local_ind,pn_int,pn_name,pay_ac_type,get_ac_type,tpay_ac_type,remitter,rem_ac_no,payer_brno_name,payee,payee_ac_no,payee_brno_name,par_amt,not_matr_date,note_type,pnote_no,way_days,wrk_date,trace_no,sts");

	ret=Mo_discnt_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_discnt_Fet_Sel( &vmo_discnt, g_pub_tx.reply );
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
			fprintf( fp,"~@转贴现协议编号|转贴现账号|@机构编码|@产品编号|@贴现类型|$贴现账户余额|贴现率|贴现日|@异地同城标志|$贴现利息|@贴现申请人户名|@贴现还款记账类型|@收款帐户类型|@付款帐户类型|@出票人全称|@出票人帐号|@付款行全称|@收款人全称|@收款人帐号|@收款人开户行全称|$汇票金额|汇票到期日|@票据类型|@票据号|在途天数|流水日期|主机流水号|@状态|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_discnt.ac_id,vmo_discnt.ac_seqn,ac_no1 );
		vtcp_log("pn_type[%s]",vmo_discnt.pn_type);
		pub_base_dic_show_str(f_pn_type5,"pn_type",vmo_discnt.pn_type);
		pub_base_dic_show_str(f_local_ind9,"local_ind",vmo_discnt.local_ind);
		pub_base_dic_show_str(f_pay_ac_type12,"pay_ac_type1",vmo_discnt.pay_ac_type);
		pub_base_dic_show_str(f_get_ac_type13,"pay_ac_type1",vmo_discnt.get_ac_type);
		pub_base_dic_show_str(f_tpay_ac_type14,"pay_ac_type1",vmo_discnt.tpay_ac_type);
		pub_base_dic_show_str(f_note_type23,"note_type",vmo_discnt.note_type);
		if(vmo_discnt.tranx_bal<-2147000000) vmo_discnt.tranx_bal=0.0;
		if(vmo_discnt.pn_rate<-2147000000) vmo_discnt.pn_rate=0.0;
		if(vmo_discnt.pn_date<-2147000000) vmo_discnt.pn_date=0;
		if(vmo_discnt.pn_int<-2147000000) vmo_discnt.pn_int=0.0;
		if(vmo_discnt.par_amt<-2147000000) vmo_discnt.par_amt=0.0;
		if(vmo_discnt.not_matr_date<-2147000000) vmo_discnt.not_matr_date=0;
		if(vmo_discnt.wrk_date<-2147000000) vmo_discnt.wrk_date=0;
		if(vmo_discnt.trace_no<-2147000000) vmo_discnt.trace_no=0;
		pub_base_dic_show_str(f_sts28,"discnt_sts",vmo_discnt.sts);

		fprintf( fp, "%s|%s|%s|%s|%s|%.2lf|%.6lf|%ld|%s|%.2lf|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%.2lf|%ld|%s|%s|%d|%ld|%ld|%s|\n",vmo_discnt.pact_no,ac_no1,vmo_discnt.br_no,vmo_discnt.bkrl_code,f_pn_type5,vmo_discnt.tranx_bal,vmo_discnt.pn_rate,vmo_discnt.pn_date,f_local_ind9,vmo_discnt.pn_int,vmo_discnt.pn_name,f_pay_ac_type12,f_get_ac_type13,f_tpay_ac_type14,vmo_discnt.remitter,vmo_discnt.rem_ac_no,vmo_discnt.payer_brno_name,vmo_discnt.payee,vmo_discnt.payee_ac_no,vmo_discnt.payee_brno_name,vmo_discnt.par_amt,vmo_discnt.not_matr_date,f_note_type23,vmo_discnt.pnote_no,vmo_discnt.way_days,vmo_discnt.wrk_date,vmo_discnt.trace_no,f_sts28);
		ttlnum++;
	}

	ret=Mo_discnt_Clo_Sel( );
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
