/*************************************************
* �� �� ��:  sp1705.c
* ���������� �ͻ���ϵ��ʽ��Ϣά��
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
#include "cif_email_inf_c.h"

int sp1705()
{
	struct cif_email_inf_c vcif_email_inf;
	struct cif_email_inf_c f_cif_email_inf;
	char f_addr_type2[41];
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

	get_zd_long("0540",&f_cif_email_inf.cif_no );
	get_zd_long("0510",&f_cif_email_inf.addr_seqn );

	/**��ɲ�ѯ����**/
	if( f_cif_email_inf.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_email_inf.cif_no );
		strcat( wherelist,tmpstr );
	}
	if( f_cif_email_inf.addr_seqn )
	{
		sprintf( tmpstr," addr_seqn=%d and",f_cif_email_inf.addr_seqn );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_email_inf" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,addr_seqn,addr_type,email,link_man");

	ret=Cif_email_inf_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_email_inf_Fet_Sel( &vcif_email_inf, g_pub_tx.reply );
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
			fprintf( fp,"~�ͻ�����|���|@��ַ����|@���ӵ�ַ|@��ϵ��|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_email_inf.cif_no<-2147000000) vcif_email_inf.cif_no=0;
		pub_base_dic_show_str(f_addr_type2,"addr_type",vcif_email_inf.addr_type);

		fprintf( fp, "%ld|%d|%s|%s|%s|\n",vcif_email_inf.cif_no,vcif_email_inf.addr_seqn,f_addr_type2,vcif_email_inf.email,vcif_email_inf.link_man);
		ttlnum++;
	}

	ret=Cif_email_inf_Clo_Sel( );
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
