/*************************************************
* �� �� ��:  sp8922.c
* ���������� �¾��ʺŲ�ѯ
*
* ��    ��:  
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
#include "mo_merge_ac_c.h"

int sp8922()
{
	struct mo_merge_ac_c vmo_merge_ac;
	struct mo_merge_ac_c f_mo_merge_ac;
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

	get_zd_data("0380",f_mo_merge_ac.old_ac_no );
	get_zd_data("0630",f_mo_merge_ac.ac_no );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_mo_merge_ac.old_ac_no );
	if( strlen(f_mo_merge_ac.old_ac_no) )
	{
		sprintf( tmpstr," old_ac_no like '%%%%%s%%%%' and ",f_mo_merge_ac.old_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_mo_merge_ac.ac_no );
	if( strlen(f_mo_merge_ac.ac_no) )
	{
		sprintf( tmpstr," ac_no='%s' and ",f_mo_merge_ac.ac_no );
		strcat( wherelist,tmpstr );
	}
	strcat(wherelist,"1=1");
	strcpy( tablelist,"mo_merge_ac" );

	/**��ɲ�ѯ**/
	/*	���ӻ����˷���ͨ�ҵ�ʱ�򲻺ò�
	sprintf(tmpstr,"  br_no='%s'",g_pub_tx.tx_br_no);
	strcat(wherelist,tmpstr) ;
	*/

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"old_br_no,old_ac_no,br_no,ac_no");

	ret=Mo_merge_ac_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Mo_merge_ac_Fet_Sel( &vmo_merge_ac, g_pub_tx.reply );
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
			fprintf( fp,"~@�ɻ�����|@����/����|@�»�����|@���˿���|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */


		fprintf( fp, "%s|%s|%s|%s|\n",vmo_merge_ac.old_br_no,vmo_merge_ac.old_ac_no,vmo_merge_ac.br_no,vmo_merge_ac.ac_no);
		ttlnum++;
	}

	ret=Mo_merge_ac_Clo_Sel( );
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
