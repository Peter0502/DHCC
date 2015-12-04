/*************************************************
* 文 件 名:  spB316.c
* 功能描述： 本票登记簿查询
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
#include "mo_cashier_c.h"

int spB316()
{
	struct mo_cashier_c vmo_cashier;
	struct mo_cashier_c f_mo_cashier;
	struct mo_cashier_c f_mo_cashiera;
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
	char cashier_sts[20],use_ind[20],los_rpt_ind[20],ct_ind[20],ration_ind[20];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	if (get_sys_par_val("01",mbfetran.isdficode)) goto ErrExit;

	get_zd_data("0580",f_mo_cashier.cashier_no );
	get_zd_long("0440",&f_mo_cashier.sign_date );
	get_zd_long("0450",&f_mo_cashiera.sign_date );
	get_zd_double("1001",&f_mo_cashier.sign_amt );
	get_zd_double("1002",&f_mo_cashiera.sign_amt );
	get_zd_data("0680",f_mo_cashier.cashier_sts );

	/**组成查询条件**/
	pub_base_strpack( f_mo_cashier.cashier_no );
	if( strlen(f_mo_cashier.cashier_no) )
	{
		sprintf( tmpstr," cashier_no='%s' and",f_mo_cashier.cashier_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier.sign_date )
	{
		sprintf( tmpstr," sign_date>=%ld and",f_mo_cashier.sign_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashiera.sign_date )
	{
		sprintf( tmpstr," sign_date<=%ld and",f_mo_cashiera.sign_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier.sign_amt )
	{
		sprintf( tmpstr," sign_amt>=%f and",f_mo_cashier.sign_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashiera.sign_amt )
	{
		sprintf( tmpstr," sign_amt<=%f and",f_mo_cashiera.sign_amt );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_cashier.cashier_sts );
	if( strlen(f_mo_cashier.cashier_sts) )
	{
		sprintf( tmpstr," cashier_sts='%s' and",f_mo_cashier.cashier_sts );
		strcat( wherelist,tmpstr );
	}

	sprintf( tmpstr," (tx_br_no='%s' or sign_br_no='%s') and",g_pub_tx.tx_br_no, g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	strcpy( tablelist,"mo_cashier" );

	/**组成查询**/
	strcat(wherelist," 1=1 order by sign_date, cashier_no ");

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_cashier_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Mo_cashier_Fet_Sel( &vmo_cashier, g_pub_tx.reply );
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
			fprintf( fp,"~@业务编号|@本票号码|@本票状态|@核销方式|@挂失标志|@录入机构|录入日期|$出票金额|@现转类型|@定额类型|@申请人帐号|@申请人全称|@申请人地址|@收款人帐号|@收款人全称|@备注|录入流水号|@录入操作员|签发日期|签发流水号|$兑付金额|$退回金额|@付款人帐号|@兑付帐号|@退回帐号|核销日期|核销流水号|\n" );
		}

		if(vmo_cashier.tx_date<-2147000000) vmo_cashier.tx_date=0;
		if(vmo_cashier.sign_amt<-2147000000) vmo_cashier.sign_amt=0.0;
		if(vmo_cashier.rec_trace_no<-2147000000) vmo_cashier.rec_trace_no=0;
		if(vmo_cashier.sign_date<-2147000000) vmo_cashier.sign_date=0;
		if(vmo_cashier.sign_trace_no<-2147000000) vmo_cashier.sign_trace_no=0;
		if(vmo_cashier.cash_amt<-2147000000) vmo_cashier.cash_amt=0.0;
		if(vmo_cashier.rem_amt<-2147000000) vmo_cashier.rem_amt=0.0;
		if(vmo_cashier.can_date<-2147000000) vmo_cashier.can_date=0;
		if(vmo_cashier.can_trace_no<-2147000000) vmo_cashier.can_trace_no=0;

		pub_base_dic_show_str(cashier_sts,"cashier_sts",vmo_cashier.cashier_sts);
		pub_base_dic_show_str(use_ind,"cashier_use_ind",vmo_cashier.use_ind);
		pub_base_dic_show_str(los_rpt_ind,"clos_rpt_ind",vmo_cashier.los_rpt_ind);
		pub_base_dic_show_str(ct_ind,"cashier_ct_ind",vmo_cashier.ct_ind);
		pub_base_dic_show_str(ration_ind,"cration_ind",vmo_cashier.ration_ind);

		fprintf( fp, "%s|%s|%s|%s|%s|%s|%ld|%lf|%s|%s|%s|%s|%s|%s|%s|%s|%ld|%s|%ld|%ld|%lf|%lf|%s|%s|%s|%ld|%ld|\n",
		pub_base_strpack(vmo_cashier.pact_no),
		pub_base_strpack(vmo_cashier.cashier_no),
		pub_base_strpack(cashier_sts),
		pub_base_strpack(use_ind),
		pub_base_strpack(los_rpt_ind),
		pub_base_strpack(vmo_cashier.tx_br_no),
		vmo_cashier.tx_date,
		vmo_cashier.sign_amt,
		pub_base_strpack(ct_ind),
		pub_base_strpack(ration_ind),
		pub_base_strpack(vmo_cashier.requ_ac_no),
		pub_base_strpack(vmo_cashier.requ_name),
		pub_base_strpack(vmo_cashier.requ_addr),
		pub_base_strpack(vmo_cashier.payee_ac_no),
		pub_base_strpack(vmo_cashier.payee_name),
		pub_base_strpack(vmo_cashier.rmk),
		vmo_cashier.rec_trace_no,
		vmo_cashier.rec_tel,
		vmo_cashier.sign_date,
		vmo_cashier.sign_trace_no,
		vmo_cashier.cash_amt,
		vmo_cashier.rem_amt,
		pub_base_strpack(vmo_cashier.pay_ac_no),
		pub_base_strpack(vmo_cashier.rec_ac_no),
		pub_base_strpack(vmo_cashier.rem_ac_no),
		vmo_cashier.can_date,
		vmo_cashier.can_trace_no);

		ttlnum++;
	}

	ret=Mo_cashier_Clo_Sel( );
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
