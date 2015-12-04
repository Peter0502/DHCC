/*************************************************
* �� �� ��:  sp8916.c
* ���������� Ȩ��Ͷ��̨�ʲ�ѯ
*
* ��    ��:  peter
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "mo_righ_invest_c.h"

int sp8916()
{
	struct mo_righ_invest_c vmo_righ_invest;
	struct mo_righ_invest_c f_mo_righ_invest;
	char ac_no0[25];
	char f_sts12[41];
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0630",f_mo_righ_invest.pact_no );
	get_zd_data("0670",f_mo_righ_invest.invst_type );
	get_zd_data("0260",f_mo_righ_invest.name );
	get_zd_long("0290",&f_mo_righ_invest.wrk_date );
	get_zd_data("0700",f_mo_righ_invest.sts );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_righ_invest.pact_no );
	if( strlen(f_mo_righ_invest.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_righ_invest.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_righ_invest.invst_type );
	if( strlen(f_mo_righ_invest.invst_type) )
	{
		sprintf( tmpstr," invst_type='%s' and",f_mo_righ_invest.invst_type );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_righ_invest.name );
	if( strlen(f_mo_righ_invest.name) )
	{
		sprintf( tmpstr," name='%s' and",f_mo_righ_invest.name );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_righ_invest.wrk_date )
	{
		sprintf( tmpstr," wrk_date=%ld and",f_mo_righ_invest.wrk_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_righ_invest.sts );
	if( strlen(f_mo_righ_invest.sts) )
	{
		sprintf( tmpstr," sts='%s' and",f_mo_righ_invest.sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_righ_invest" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,br_no,bkrl_code,pact_no,invst_type,num,amt,incm_bal,cif_code,name,wrk_date,trace_no,sts");

	ret=Mo_righ_invest_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_righ_invest_Fet_Sel( &vmo_righ_invest, g_pub_tx.reply );
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
			fprintf( fp,"~�˺�|@��������|@�ʽ��Ʒ���|@Ȩ����Ͷ�ʺ�ͬ���|@Ͷ�����|������|$���|$������ʧ���|@Ͷ�ʶ������֤��|@Ȩ����Ͷ�ʻ�������|�Ǽ�����|������ˮ��|@��־|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_righ_invest.ac_id,0,ac_no0 );
		if(vmo_righ_invest.num<-2147000000) vmo_righ_invest.num=0;
		if(vmo_righ_invest.amt<-2147000000) vmo_righ_invest.amt=0.0;
		if(vmo_righ_invest.incm_bal<-2147000000) vmo_righ_invest.incm_bal=0.0;
		if(vmo_righ_invest.wrk_date<-2147000000) vmo_righ_invest.wrk_date=0;
		if(vmo_righ_invest.trace_no<-2147000000) vmo_righ_invest.trace_no=0;
		pub_base_dic_show_str(f_sts12,"all_sts",vmo_righ_invest.sts);

		fprintf( fp, "%s|%s|%s|%s|%s|%ld|%.2lf|%.2lf|%s|%s|%ld|%ld|%s|\n",ac_no0,vmo_righ_invest.br_no,vmo_righ_invest.bkrl_code,vmo_righ_invest.pact_no,vmo_righ_invest.invst_type,vmo_righ_invest.num,vmo_righ_invest.amt,vmo_righ_invest.incm_bal,vmo_righ_invest.cif_code,vmo_righ_invest.name,vmo_righ_invest.wrk_date,vmo_righ_invest.trace_no,f_sts12);
		ttlnum++;
	}

	ret=Mo_righ_invest_Clo_Sel( );
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
