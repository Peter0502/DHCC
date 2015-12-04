/*************************************************
* �� �� ��:  sp8206.c
* ���������� �ſ�ƻ����ѯ
*
* ��    ��:  lance
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
#include "ln_pay_pln_c.h"

int sp8206()
{
	struct ln_pay_pln_c vln_pay_pln;
	struct ln_pay_pln_c f_ln_pay_pln;
	struct ln_pay_pln_c f_ln_pay_plna;
	char ac_no0[25];
	char ac_no1[25];
	char f_pay_ind5[41];
	char f_acno1[25];
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		char pay_ind[2];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0670",f_ln_pay_pln.pay_ind );
	get_zd_data("0380",f_acno1);
	get_zd_double("1001",&f_ln_pay_pln.ttl_amt );
	get_zd_double("1002",&f_ln_pay_plna.ttl_amt );
	get_zd_long("0290",&f_ln_pay_pln.beg_date );
	get_zd_long("0440",&f_ln_pay_plna.beg_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_ln_pay_pln.pay_ind );
	if( strlen(f_ln_pay_pln.pay_ind) )
	{
		if( f_ln_pay_pln.pay_ind[0]=='0' )	/* δ�� */
		{
			strcpy( pay_ind , "2" );
			sprintf( tmpstr," pay_ind='%s' and",pay_ind );
		}
		if( f_ln_pay_pln.pay_ind[0]=='1' )	/* �ѷ� */
		{
			strcpy( pay_ind , "3" );
			sprintf( tmpstr," pay_ind='%s' and",pay_ind );
		}		
		strcat( wherelist,tmpstr );
	}else{
		strcpy( tmpstr," pay_ind in( '2','3') and ");
		strcat( wherelist,tmpstr );
	}				
	
	pub_base_strpack( f_acno1 );
	if( strlen(f_acno1) )
	{
		if ( pub_base_acno_acid(&f_ln_pay_pln.ac_id,f_acno1) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_ln_pay_pln.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_pay_pln.ttl_amt )
	{
		sprintf( tmpstr," ttl_amt>=%lf and",f_ln_pay_pln.ttl_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_pay_plna.ttl_amt )
	{
		sprintf( tmpstr," ttl_amt<=%lf and",f_ln_pay_plna.ttl_amt );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_pay_pln.beg_date )
	{
		sprintf( tmpstr," beg_date>=%ld and",f_ln_pay_pln.beg_date );
		strcat( wherelist,tmpstr );
	}
	if( f_ln_pay_plna.beg_date )
	{
		sprintf( tmpstr," beg_date<=%ld and",f_ln_pay_plna.beg_date );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"ln_pay_pln" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,ttl_amt,wrk_date,beg_date,pay_ind");

	ret=Ln_pay_pln_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Ln_pay_pln_Fet_Sel( &vln_pay_pln, g_pub_tx.reply );
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
			fprintf( fp,"~��    ��|���|@����|$�ſ���|�ſ��˺�|���|�Ǽ�����|��������|@�ſ�״̬|\n" );
		}

		/* ȡ������� */
		ret = Ln_mst_Sel( g_pub_tx.reply , &g_ln_mst , " ac_id='%ld' and ac_seqn='%d' " , vln_pay_pln.ac_id,vln_pay_pln.ac_seqn );		
		if( ret )
		{
			sprintf(acErrMsg,"��ȡ�������ļ���Ϣerror");
			WRITEMSG
			strcpy( g_pub_tx.reply,"S049" );
			goto ErrExit;
		}
		
		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vln_pay_pln.ac_id,vln_pay_pln.ac_seqn,ac_no0 );
		pub_base_acid_acno( vln_pay_pln.de_ac_id,vln_pay_pln.de_ac_seqn,ac_no1 );
		if(vln_pay_pln.ttl_amt<-2147000000) vln_pay_pln.ttl_amt=0.0;
		if(vln_pay_pln.wrk_date<-2147000000) vln_pay_pln.wrk_date=0;
		if(vln_pay_pln.beg_date<-2147000000) vln_pay_pln.beg_date=0;
		pub_base_dic_show_str(f_pay_ind5,"pay_sts",vln_pay_pln.pay_ind);

		fprintf( fp, "%s|%d|%s|%.2lf|%s|%d|%ld|%ld|%s|\n",ac_no0,vln_pay_pln.ac_seqn,g_ln_mst.name,vln_pay_pln.ttl_amt,ac_no1,vln_pay_pln.de_ac_seqn,vln_pay_pln.wrk_date,vln_pay_pln.beg_date,f_pay_ind5);
		ttlnum++;
	}

	ret=Ln_pay_pln_Clo_Sel( );
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
