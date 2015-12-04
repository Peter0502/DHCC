/*************************************************
* �� �� ��:  sp8108.c
* ���������� �����Ǽǲ���ѯ
*
* ��    ��:  mike
* ������ڣ� 2003-08-10
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
#include "mo_unchange_c.h"

int sp8108()
{
	struct mo_unchange_c vmo_unchange;
	struct mo_unchange_c f_mo_unchange;
	char ac_no0[25];
	char name[51];
	char f_ind4[41];
	char f_acno0[25];
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

	get_zd_data("0380",f_acno0);
	get_zd_long("0510",&f_mo_unchange.ac_seqn );
	get_zd_double("1001",&f_mo_unchange.bal );
	get_zd_data("0910",f_mo_unchange.prdt_no );
	get_zd_long("0290",&f_mo_unchange.tx_date );
	get_zd_data("0750",f_mo_unchange.brf );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_acno0 );
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_mo_unchange.ac_id,f_acno0) )
		{
		goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_mo_unchange.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_unchange.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_mo_unchange.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_unchange.bal )
	{
		sprintf( tmpstr," bal=%lf and",f_mo_unchange.bal );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_unchange.prdt_no );
	if( strlen(f_mo_unchange.prdt_no) )
	{
		sprintf( tmpstr," prdt_no='%s' and",f_mo_unchange.prdt_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_unchange.tx_date )
	{
		sprintf( tmpstr," tx_date=%ld and",f_mo_unchange.tx_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_unchange.brf );
	if( strlen(f_mo_unchange.brf) )
	{
		sprintf( tmpstr," brf='%s' and",f_mo_unchange.brf );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_unchange" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 ORDER BY ind,ac_id,ac_seqn,bal,intst ") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"ac_id,ac_seqn,bal,intst,ind,prdt_no,trace_no,tx_br_no,tx_date,tx_time,brf,tel");

	ret=Mo_unchange_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_unchange_Fet_Sel( &vmo_unchange, g_pub_tx.reply );
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
			fprintf( fp,"~��      ��|���|@����|$��      ��|$��      Ϣ|@֧ȡ|@��Ʒ|��ˮ��|@���׻�����|��������|����ʱ��|@ժҪ|@����Ա|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		pub_base_acid_acno( vmo_unchange.ac_id,vmo_unchange.ac_seqn,ac_no0 );
		pub_base_acgetname_mst( vmo_unchange.ac_id,vmo_unchange.ac_seqn,name );
		if(vmo_unchange.bal<-2147000000) vmo_unchange.bal=0.0;
		if(vmo_unchange.intst<-2147000000) vmo_unchange.intst=0.0;
		pub_base_dic_show_str(f_ind4,"YN_ind",vmo_unchange.ind);
		if(vmo_unchange.trace_no<-2147000000) vmo_unchange.trace_no=0;
		if(vmo_unchange.tx_date<-2147000000) vmo_unchange.tx_date=0;
		if(vmo_unchange.tx_time<-2147000000) vmo_unchange.tx_time=0;

		fprintf( fp, "%s|%d|%s|%.2lf|%.2lf|%s|%s|%ld|%s|%ld|%ld|%s|%s|\n",ac_no0,vmo_unchange.ac_seqn,name,vmo_unchange.bal,vmo_unchange.intst,f_ind4,vmo_unchange.prdt_no,vmo_unchange.trace_no,vmo_unchange.tx_br_no,vmo_unchange.tx_date,vmo_unchange.tx_time,vmo_unchange.brf,vmo_unchange.tel);
		ttlnum++;
	}

	ret=Mo_unchange_Clo_Sel( );
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
