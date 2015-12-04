/*************************************************
* �� �� ��:  spB317.c
* ���������� ��Ʊ��ʧ�Ǽǲ���ѯ
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
#include "mo_cashier_los_rpt_c.h"

int spB317()
{
	struct mo_cashier_los_rpt_c vmo_cashier_los_rpt;
	struct mo_cashier_los_rpt_c f_mo_cashier_los_rpt;
	struct mo_cashier_los_rpt_c f_mo_cashier_los_rpta;
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
	char sts[20],id_type[20];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();
	if (get_sys_par_val("01",mbfetran.isdficode)) goto ErrExit;

	get_zd_data("0580",f_mo_cashier_los_rpt.cashier_no );
	get_zd_long("0440",&f_mo_cashier_los_rpt.los_rpt_date );
	get_zd_long("0450",&f_mo_cashier_los_rpta.los_rpt_date );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_cashier_los_rpt.cashier_no );
	if( strlen(f_mo_cashier_los_rpt.cashier_no) )
	{
		sprintf( tmpstr," cashier_no='%s' and",f_mo_cashier_los_rpt.cashier_no );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier_los_rpt.los_rpt_date )
	{
		sprintf( tmpstr," los_rpt_date>=%ld and",f_mo_cashier_los_rpt.los_rpt_date );
		strcat( wherelist,tmpstr );
	}
	if( f_mo_cashier_los_rpta.los_rpt_date )
	{
		sprintf( tmpstr," los_rpt_date<=%ld and",f_mo_cashier_los_rpta.los_rpt_date );
		strcat( wherelist,tmpstr );
	}

	sprintf( tmpstr," los_rpt_br_no='%s' and", g_pub_tx.tx_br_no );
	strcat( wherelist,tmpstr );

	strcpy( tablelist,"mo_cashier_los_rpt" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 order by los_rpt_date ");

	vtcp_log( "WHERE [%s]",wherelist );

	ret=Mo_cashier_los_rpt_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Mo_cashier_los_rpt_Fet_Sel( &vmo_cashier_los_rpt, g_pub_tx.reply );
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
			fprintf( fp,"~@״̬|@��Ʊ����|��ʧ����|@������֤������|@֤������|@����|�������|\n" );
		}

		if(vmo_cashier_los_rpt.los_rpt_date<-2147000000) vmo_cashier_los_rpt.los_rpt_date=0;
		if(vmo_cashier_los_rpt.clos_rpt_date<-2147000000) vmo_cashier_los_rpt.clos_rpt_date=0;

		pub_base_dic_show_str(sts,"los_rpt_sts",vmo_cashier_los_rpt.sts);
		pub_base_dic_show_str(id_type,"id_type",vmo_cashier_los_rpt.los_rpt_id_type);

		fprintf( fp, "%s|%s|%ld|%s|%s|%s|%ld|\n",
		pub_base_strpack(sts),
		pub_base_strpack(vmo_cashier_los_rpt.cashier_no),
		vmo_cashier_los_rpt.los_rpt_date,
		pub_base_strpack(id_type),
		pub_base_strpack(vmo_cashier_los_rpt.los_rpt_id_no),
		pub_base_strpack(vmo_cashier_los_rpt.los_rpt_name),
		vmo_cashier_los_rpt.clos_rpt_date);

		ttlnum++;
	}

	ret=Mo_cashier_los_rpt_Clo_Sel( );
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
