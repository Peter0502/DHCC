/*************************************************
* 文 件 名:  sp8915.c
* 功能描述： 债券投资台帐查询
*
* 作    者:  
* 完成日期： 
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_invest_c.h"

int sp8915()
{
	struct mo_invest_c vmo_invest;
	struct mo_invest_c f_mo_invest;
	char ac_no3[25];
	char ac_no9[25];
	char ac_no11[25];
	char f_sts21[41];
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

	get_zd_data("0630",f_mo_invest.pact_no );
	get_zd_data("0620",f_mo_invest.ticket_type );
	get_zd_long("0290",&f_mo_invest.isuue_date );
	get_zd_long("0440",&f_mo_invest.buy_date );
	get_zd_long("0450",&f_mo_invest.mtr_date );
	get_zd_data("0670",f_mo_invest.sts );

	/**组成查询条件**/
	pub_base_strpack( f_mo_invest.pact_no );
	if( strlen(f_mo_invest.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_invest.pact_no );
		strcat( wherelist,tmpstr );
	}
/**	增强like功能 20061009 martin **/
  pub_base_strpack( f_mo_invest.ticket_type );
	if( strlen(f_mo_invest.ticket_type) )
	{
		sprintf( tmpstr,"ticket_type like '%%%%%s%%%%' and",f_mo_invest.ticket_type );
		strcat( wherelist,tmpstr );
	} 

	if( f_mo_invest.isuue_date )
	{
		sprintf( tmpstr," isuue_date=%ld and",f_mo_invest.isuue_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_invest.buy_date )
	{
		sprintf( tmpstr," buy_date=%ld and",f_mo_invest.buy_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_invest.mtr_date )
	{
		sprintf( tmpstr," mtr_date=%ld and",f_mo_invest.mtr_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_invest.sts );
	if( strlen(f_mo_invest.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_invest.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_invest" );

	/**组成查询**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,bkrl_code,br_no,ac_id,ticket_type,isuue_date,term,par_bal,buy_amt,abate_ac_id,abate_amt,overflow_ac_id,overflow_amt,pre_intst,buy_date,rate,mtr_date,sol_amt,sol_date,wrk_date,trace_no,sts,tel");

	ret=Mo_invest_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_invest_Fet_Sel( &vmo_invest, g_pub_tx.reply );
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
			fprintf( fp,"~@投资合同号|@投资产品编号|@机构编码|@账号|@投资债券品种|@发行日期|@期限|$面值|$买入价格|@折价账号|$折价|@溢价账号|$溢价|$应记利息|@买入日期|@利率|@到期日期|$卖出价|@卖出日期|@流水日期|@主机流水号|@标志|@操作员|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_invest.ac_id,0,ac_no3 );
		if(vmo_invest.isuue_date<-2147000000) vmo_invest.isuue_date=0;
		if(vmo_invest.par_bal<-2147000000) vmo_invest.par_bal=0.0;
		if(vmo_invest.buy_amt<-2147000000) vmo_invest.buy_amt=0.0;
		pub_base_acid_acno( vmo_invest.abate_ac_id,0,ac_no9 );
		if(vmo_invest.abate_amt<-2147000000) vmo_invest.abate_amt=0.0;
		pub_base_acid_acno( vmo_invest.overflow_ac_id,0,ac_no11 );
		if(vmo_invest.overflow_amt<-2147000000) vmo_invest.overflow_amt=0.0;
		if(vmo_invest.pre_intst<-2147000000) vmo_invest.pre_intst=0.0;
		if(vmo_invest.buy_date<-2147000000) vmo_invest.buy_date=0;
		if(vmo_invest.rate<-2147000000) vmo_invest.rate=0.0;
		if(vmo_invest.mtr_date<-2147000000) vmo_invest.mtr_date=0;
		if(vmo_invest.sol_amt<-2147000000) vmo_invest.sol_amt=0.0;
		if(vmo_invest.sol_date<-2147000000) vmo_invest.sol_date=0;
		if(vmo_invest.wrk_date<-2147000000) vmo_invest.wrk_date=0;
		if(vmo_invest.trace_no<-2147000000) vmo_invest.trace_no=0;
		pub_base_dic_show_str(f_sts21,"all_sts",vmo_invest.sts);

		fprintf( fp, "%s|%s|%s|%s|%s|%ld|%d|%.2lf|%.2lf|%s|%.2lf|%s|%.2lf|%.2lf|%ld|%.5lf|%ld|%.2lf|%ld|%ld|%ld|%s|%s|\n",vmo_invest.pact_no,vmo_invest.bkrl_code,vmo_invest.br_no,ac_no3,vmo_invest.ticket_type,vmo_invest.isuue_date,vmo_invest.term,vmo_invest.par_bal,vmo_invest.buy_amt,ac_no9,vmo_invest.abate_amt,ac_no11,vmo_invest.overflow_amt,vmo_invest.pre_intst,vmo_invest.buy_date,vmo_invest.rate,vmo_invest.mtr_date,vmo_invest.sol_amt,vmo_invest.sol_date,vmo_invest.wrk_date,vmo_invest.trace_no,f_sts21,vmo_invest.tel);
		ttlnum++;
	}

	ret=Mo_invest_Clo_Sel( );
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
