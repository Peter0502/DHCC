/*************************************************
* �� �� ��:  spB316.c
* ���������� ��Ʊ�Ǽǲ���ѯ
*
* ��    ��:  jane
* ������ڣ� 2003��11��10��
*
* �޸ļ�¼��
*   ��    ��:
*   �� �� ��:
*   �޸�����:
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
	char wherelist[1024];  /**��ѯ����**/
	char tmpstr[512];
	char tmpmo[512];
	char tablelist[128];   /**Ҫ��ѯ�ı���**/
	char titstr[1024];
	int ttlnum=0;			/**��ȡ��������**/
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

	/**��ɲ�ѯ����**/
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

	/**��ɲ�ѯ**/
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

			/**��ʾ�б���**/
			fprintf( fp,"~@ҵ����|@��Ʊ����|@��Ʊ״̬|@������ʽ|@��ʧ��־|@¼�����|¼������|$��Ʊ���|@��ת����|@��������|@�������ʺ�|@������ȫ��|@�����˵�ַ|@�տ����ʺ�|@�տ���ȫ��|@��ע|¼����ˮ��|@¼�����Ա|ǩ������|ǩ����ˮ��|$�Ҹ����|$�˻ؽ��|@�������ʺ�|@�Ҹ��ʺ�|@�˻��ʺ�|��������|������ˮ��|\n" );
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
