/*************************************************
* �� �� ��:  sp2718.c
* ���������� �Թ�/���˻����˻���ѯ
*
* ��    ��:  �Լ���
* ������ڣ� 
*
* �޸ļ�¼��
* ��   ��:
* �� �� ��:
* �޸�����:
*insert into tx_def  values ('2718', '���˻����˻���ѯ', '10000000000000010000011111111111111110001100000000010100000000000000000000000000000000000000000000000000000000000000000000000000', '1', '3', '1', '0', null);
*insert into tx_flow_def  values ('2718', 0, '2718', '#$');
*insert into tx_sub_def  values ('2718', '�Թ�/���˻����˻���ѯ', 'sp2718', '0', '0');
*************************************************/
#define ERR_DEAL if( ret ) {\
		sprintf( acErrMsg, "sql error" ); \
		WRITEMSG \
		goto ErrExit; \
		}
#define EXTERN
#include "public.h"
#include "dd_mst_c.h"

int sp2718()
{
	struct dd_mst_c vcif_alias_inf;
	/*struct dd_mst_c f_cif_alias_inf;*/
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
		int ac_type=0;
		long ac_id=0;
		double min=0.00;
		double max=0.00;
		FILE *fp;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_int("0520",&ac_type);
	get_zd_long("0540",&ac_id );
	get_zd_double("0410",&min );
	get_zd_double("0420",&max );
	
	/**��ɲ�ѯ����**/
	if(ac_type==1 || ac_type==5)
		{
			sprintf( tmpstr," ac_type=%d and",ac_type );
			strcat( wherelist,tmpstr );
		}		
	if( ac_id )
		{
			sprintf( tmpstr," ac_id=%ld and",ac_id );
			strcat( wherelist,tmpstr );
		}
	if( min>0.00 && min<max )
		{
			sprintf( tmpstr," bal>%lf and",min );
			strcat( wherelist,tmpstr );
		}
	if(max>min && max!=0)
		{
			sprintf( tmpstr," bal<%lf and",max );
			strcat( wherelist,tmpstr );
		}
		
	/*strcpy( tablelist,"cif_alias_inf" );*/

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	/*strcpy( fieldlist,"cif_no,alias_seqn,alias");*/

	ret=Dd_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Dd_mst_Fet_Sel( & vcif_alias_inf, g_pub_tx.reply );
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
			fprintf( fp,"~�˺�|����|��������|�˻�����|���\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if( vcif_alias_inf.ac_id<-2147000000)  vcif_alias_inf.ac_id=0;

		fprintf( fp, "%ld|%s|%ld|%d|%lf|\n", vcif_alias_inf.ac_id, vcif_alias_inf.name, vcif_alias_inf.opn_date,  vcif_alias_inf.ac_type, vcif_alias_inf.bal);
		ttlnum++;
	}

	ret=Dd_mst_Clo_Sel( );
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
