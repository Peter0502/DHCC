/*************************************************
* �� �� ��:  sp1903.c
* ���������� �ͻ������޸���־��ѯ
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
#include "cif_inf_chg_log_c.h"
#include "cif_id_code_rel_c.h"

int sp1903()
{
	struct cif_inf_chg_log_c vcif_inf_chg_log;
	struct cif_inf_chg_log_c f_cif_inf_chg_log;
	struct cif_id_code_rel_c f_cif_id_code_rel;
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
	memset( &vcif_inf_chg_log,0,sizeof(struct cif_inf_chg_log_c) );
	memset( &f_cif_inf_chg_log,0,sizeof(struct cif_inf_chg_log_c) );
	memset( &f_cif_id_code_rel,0,sizeof(struct cif_id_code_rel_c) );

	pub_base_sysinit();

	get_zd_long("0540",&f_cif_inf_chg_log.cif_no );
	get_zd_data("0680",f_cif_id_code_rel.id_type );
	get_zd_data("0620",f_cif_id_code_rel.id_no );

	/**��ɲ�ѯ����**/
	if( f_cif_inf_chg_log.cif_no )
	{
		sprintf( tmpstr," cif_no=%ld and",f_cif_inf_chg_log.cif_no );
		strcat( wherelist,tmpstr );
	}
	else if(strlen(f_cif_id_code_rel.id_type)&&strlen(f_cif_id_code_rel.id_no))
	{
		ret=Cif_id_code_rel_Sel(g_pub_tx.reply,&f_cif_id_code_rel, \
								"id_type='%s' and id_no='%s'", \
								f_cif_id_code_rel.id_type, \
								f_cif_id_code_rel.id_no);
		if(ret==100)
		{
			sprintf(acErrMsg,"��ƥ��ļ�¼!! ");
			WRITEMSG
			strcpy(g_pub_tx.reply,"O043");
			goto ErrExit;
		}

		f_cif_inf_chg_log.cif_no=f_cif_id_code_rel.cif_no;
		sprintf( tmpstr," cif_no=%ld and",f_cif_inf_chg_log.cif_no );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"cif_inf_chg_log" );

	/**��ɲ�ѯ**/
	strcat(wherelist," 1=1") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"cif_no,chg_tablename,chg_columnname,chg_date,chg_time,bef_content,aft_content,clerk_no,memo");

	ret=Cif_inf_chg_log_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Cif_inf_chg_log_Fet_Sel( &vcif_inf_chg_log, g_pub_tx.reply );
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
			fprintf( fp,"~�ͻ�����|@���޸Ŀͻ���Ϣ����|@���޸Ŀͻ���Ϣ����|�޸�����|�޸�ʱ��|@�޸�ǰ����|@�޸ĺ�����|@ְԱ���|@˵��|\n" );
		}

		/*pub_pubdb_prt_fld( fp,tablelist,fieldlist ); */

		if(vcif_inf_chg_log.cif_no<-2147000000) vcif_inf_chg_log.cif_no=0;
		if(vcif_inf_chg_log.chg_date<-2147000000) vcif_inf_chg_log.chg_date=0;
		if(vcif_inf_chg_log.chg_time<-2147000000) vcif_inf_chg_log.chg_time=0;
		vtcp_log("[%s][%d] bef_content==[%s]",__FILE__,__LINE__,vcif_inf_chg_log.bef_content);
		vtcp_log("[%s][%d] aft_content==[%s]",__FILE__,__LINE__,vcif_inf_chg_log.aft_content);

		fprintf( fp, "%ld|%s|%s|%ld|%ld|%s|%s|%s|%s|\n",vcif_inf_chg_log.cif_no,vcif_inf_chg_log.chg_tablename,vcif_inf_chg_log.chg_columnname,vcif_inf_chg_log.chg_date,vcif_inf_chg_log.chg_time,vcif_inf_chg_log.bef_content,vcif_inf_chg_log.aft_content,vcif_inf_chg_log.clerk_no,vcif_inf_chg_log.memo);
		ttlnum++;
	}

	ret=Cif_inf_chg_log_Clo_Sel( );
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
