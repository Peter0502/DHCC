/*************************************************
* �� �� ��:  spB492.c
* ���������� �жһ�ƱЭ���ź�ͬ�Ŷ��ձ��ѯ
*
* ��    ��:  jane
* ������ڣ� 2003��11��1��
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
#include "mo_po_co_c.h"

int spB492()
{
	struct mo_po_co_c vmo_po_co;
	struct mo_po_co_c f_mo_po_co;
	struct mo_po_co_c f_mo_po_coa;
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
	char cmtname[20],procstate[10];

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0330",f_mo_po_co.pact_no );
	get_zd_data("0320",f_mo_po_co.contract_no );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_po_co.pact_no );
	if( strlen(f_mo_po_co.pact_no) )
	{
		sprintf( tmpstr," pact_no='%s' and",f_mo_po_co.pact_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_po_co.contract_no );
	if( strlen(f_mo_po_co.contract_no) )
	{
		sprintf( tmpstr," contract_no='%s' and",f_mo_po_co.contract_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"mo_po_co" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1 order by pact_no");

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"pact_no,contract_no");

	ret=Mo_po_co_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

	ttlnum=0;

	while(1)
	{
		ret=Mo_po_co_Fet_Sel( &vmo_po_co, g_pub_tx.reply );
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
			fprintf( fp,"~@�ж�Э����|��ͬ��|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		fprintf( fp, "%s|%s|\n",vmo_po_co.pact_no,vmo_po_co.contract_no);
		ttlnum++;
	}

	ret=Mo_po_co_Clo_Sel( );
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
