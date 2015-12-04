/*************************************************
* �� �� ��:  sp8119.c
* ���������� ����ĸ�ʺŲ�ѯ
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
#include "cif_basic_inf_c.h"
#include "mo_prot_ac_rel_c.h"

int sp8119()
{
	struct cif_basic_inf_c sCif_basic_inf;
	struct mo_prot_ac_rel_c sMo_prot_ac_rel;
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
		long old_main_ac_id=0;
	char f_sub_ac_no[20],f_name[51];
	long	f_ac_no=0;
	double	dd_bal=0.00;
	double	td_bal=0.00;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &sCif_basic_inf,0x00,sizeof(struct cif_basic_inf_c));

	pub_base_sysinit();

	get_zd_long("0280",&f_ac_no );
	get_zd_data("0370",f_sub_ac_no );
	get_zd_data("0260",f_name );

	/**��ɲ�ѯ����**/
	sprintf( tmpstr," tx_br_no='%s' and",g_pub_tx.tx_br_no);
	strcat( wherelist,tmpstr );

	if( f_ac_no>0 )
	{
		sprintf( tmpstr," main_ac_id=%ld and",f_ac_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_ac_no );
	if( strlen(f_sub_ac_no) )
	{
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",f_sub_ac_no);
		if( ret )
		{
			strcpy( g_pub_tx.reply, "P447" );
			sprintf(acErrMsg,"Before OK return: reply [%s]",f_sub_ac_no);
			WRITEMSG
			goto ErrExit;
		}
		sprintf( tmpstr," app_ac_id=%d and",g_mdm_ac_rel.ac_id );
		strcat( wherelist,tmpstr );
	}
	/***
	pub_base_strpack( f_name );
	if( strlen(f_name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_name );
		strcat( wherelist,tmpstr );
	}
	***/

	/**��ɲ�ѯ**/
	strcat(wherelist,"  prot_type='Z01' and sts='1' ") ;
	sprintf(tmpstr," order by main_ac_id" );
	strcat( wherelist,tmpstr );

	/**
	ret=distinct_long_dec_sel("mo_prot_ac_rel","main_ac_id",wherelist  );
	**/
	ret=Mo_prot_ac_rel_Dec_Sel(g_pub_tx.reply,wherelist  );
	ERR_DEAL
	ttlnum=0;
	while(1)
	{
		/**
		ret=distinct_long_fet_sel(&f_ac_no);
		**/
		ret=Mo_prot_ac_rel_Fet_Sel(&sMo_prot_ac_rel,g_pub_tx.reply);
		if( ret==100 ) break;
		ERR_DEAL
vtcp_log("11111111111111111 main_ac_id=[%ld]",sMo_prot_ac_rel.main_ac_id);

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
			fprintf( fp,"~@��������|@ĸ�˺�|@���ʺ�|$���|@����|\n" );
		}

		/*** ȡĸ�ʺ���� ***/
		if( sMo_prot_ac_rel.main_ac_id!=old_main_ac_id )
		{
			ret=sql_sum_double("dd_mst","bal",&dd_bal,"ac_id in (select app_ac_id from mo_prot_ac_rel where main_ac_id=%ld and sts='1' )",sMo_prot_ac_rel.main_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ȡĸ�ʺ�������[%d][%d]",f_ac_no,ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			ret=sql_sum_double("td_mst","bal",&td_bal,"ac_id in (select app_ac_id from mo_prot_ac_rel where main_ac_id=%ld and sts='1' )",sMo_prot_ac_rel.main_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ȡĸ�ʺ�������[%d][%d]",f_ac_no,ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			/**** ȡĸ�ʺŻ��� *****/
			ret=Cif_basic_inf_Sel(g_pub_tx.reply,&sCif_basic_inf," cif_no=%ld",sMo_prot_ac_rel.main_ac_id);
			if( ret )
			{
				sprintf(acErrMsg,"ȡĸ�ʺŻ�������[%d][%d]",f_ac_no,ret);
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}

			fprintf( fp, "%s|%ld||%.2lf|%s|\n",g_pub_tx.tx_br_no,sMo_prot_ac_rel.main_ac_id,dd_bal+td_bal,sCif_basic_inf.name);
			ttlnum++;
			old_main_ac_id=sMo_prot_ac_rel.main_ac_id;
		}

		memset(&g_dd_mst,0x00,sizeof(struct dd_mst_c));
		memset(&g_td_mst,0x00,sizeof(struct td_mst_c));
		ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_id=%ld ",sMo_prot_ac_rel.app_ac_id);
		ERR_DEAL
		ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst," ac_id=%ld ",sMo_prot_ac_rel.app_ac_id);
		if( ret ==100 )
		{
			ret=Td_mst_Sel(g_pub_tx.reply,&g_td_mst," ac_id=%ld ",sMo_prot_ac_rel.app_ac_id);
			ERR_DEAL
			fprintf( fp, "%s|%ld|%s|%.2lf|%s|\n",g_pub_tx.tx_br_no,sMo_prot_ac_rel.main_ac_id,g_mdm_ac_rel.ac_no,g_td_mst.bal,g_td_mst.name);
			continue;
		}
		ERR_DEAL
		fprintf( fp, "%s|%ld|%s|%.2lf|%s|\n",g_pub_tx.tx_br_no,sMo_prot_ac_rel.main_ac_id,g_mdm_ac_rel.ac_no,g_dd_mst.bal,g_dd_mst.name);
		ttlnum++;
	}
	/**
	ret=distinct_long_clo_sel( );
	**/
	ret=Mo_prot_ac_rel_Clo_Sel();
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
