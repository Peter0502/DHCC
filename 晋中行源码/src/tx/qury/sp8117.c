/*************************************************
* �� �� ��:  sp8117.c
* ���������� ���ʺ�̨�ʲ�ѯ
*
* ��    ��:  mike
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
#include "sub_dd_mst_c.h"

int sp8117()
{
	struct sub_dd_mst_c vsub_dd_mst;
	struct sub_dd_mst_c f_sub_dd_mst;
		char filename[100];
		char wherelist[1024];  /**��ѯ����**/
		char tmpstr[512];
		char tmpmo[512];
		char fieldlist[1024];  /**Ҫ��ѯ���ֶ��б�**/
		char tablelist[128];   /**Ҫ��ѯ�ı���**/
		char titstr[1024];
		char tm_ac_no[21];
		int ttlnum=0;			/**��ȡ��������**/
		int i=0;
		int ret=0;
		FILE *fp;
		char old_ac_no[21];
	double T_sum=0.00;
	double T_ys_sum=0.00;

	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( tm_ac_no,0,sizeof(tm_ac_no) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );

	pub_base_sysinit();

	get_zd_data("0910",f_sub_dd_mst.br_no );
	get_zd_data("0380",tm_ac_no );
 vtcp_log( "1ac_no [%s]",tm_ac_no );
	get_zd_data("0370",f_sub_dd_mst.sub_ac_no );
 vtcp_log( "2ac_no [%s]",tm_ac_no );
	get_zd_long("0290",&f_sub_dd_mst.opn_date );
	get_zd_data("0260",f_sub_dd_mst.name );
	get_zd_data("0670",f_sub_dd_mst.ac_sts );

	/**��ɲ�ѯ����**/
	pub_base_strpack( f_sub_dd_mst.br_no );
	if( strlen(f_sub_dd_mst.br_no) )
	{
		sprintf( tmpstr," br_no='%s' and",f_sub_dd_mst.br_no );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( tm_ac_no );
	if( strlen(tm_ac_no) )
	{
		pub_base_old_acno( tm_ac_no );
		TRACE
		sprintf( tmpstr," ac_no='%s' and",tm_ac_no);
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst.sub_ac_no );
	if( strlen(f_sub_dd_mst.sub_ac_no) )
	{
		sprintf( tmpstr," sub_ac_no='%s' and",f_sub_dd_mst.sub_ac_no );
		strcat( wherelist,tmpstr );
	}
	if( f_sub_dd_mst.opn_date )
	{
		sprintf( tmpstr," opn_date=%ld and",f_sub_dd_mst.opn_date );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst.name );
	if( strlen(f_sub_dd_mst.name) )
	{
		sprintf( tmpstr," name like '%%%%%s%%%%' and",f_sub_dd_mst.name );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_sub_dd_mst.ac_sts );
	if( strlen(f_sub_dd_mst.ac_sts) )
	{
		sprintf( tmpstr," ac_sts='%s' and",f_sub_dd_mst.ac_sts );
		strcat( wherelist,tmpstr );
	}

	strcpy( tablelist,"sub_dd_mst" );

	/**��ɲ�ѯ**/
	strcat(wherelist," ac_sts='1'  order by ac_no,sub_ac_no") ;

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"br_no,ac_no,sub_ac_no,bal,ys_bal,intst_acm,opn_date,ic_date,lst_date,hst_cnt,hst_pg,name,ac_sts");

	ret=Sub_dd_mst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL

		ttlnum=0;

	while(1)
	{
		ret=Sub_dd_mst_Fet_Sel( &vsub_dd_mst, g_pub_tx.reply );
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
			fprintf( fp,"~@��������|@ĸ�˺�|@���ʺ�|$���|$�������|$��Ϣ����|��������|��Ϣ����|�ϱʷ�������|��ϸ����|��ҳҳ��|@����|@���ʺ�״̬|\n" );
		}

		if(vsub_dd_mst.bal<-2147000000) vsub_dd_mst.bal=0.0;
		if(vsub_dd_mst.ys_bal<-2147000000) vsub_dd_mst.ys_bal=0.0;
		if(vsub_dd_mst.intst_acm<-2147000000) vsub_dd_mst.intst_acm=0.0;
		if(vsub_dd_mst.opn_date<-2147000000) vsub_dd_mst.opn_date=0;
		if(vsub_dd_mst.ic_date<-2147000000) vsub_dd_mst.ic_date=0;
		if(vsub_dd_mst.lst_date<-2147000000) vsub_dd_mst.lst_date=0;
		if(vsub_dd_mst.hst_cnt<-2147000000) vsub_dd_mst.hst_cnt=0;
		if(vsub_dd_mst.hst_pg<-2147000000) vsub_dd_mst.hst_pg=0;
		if( strcmp(vsub_dd_mst.ac_no,old_ac_no)!=0 )
		{
			strcpy(old_ac_no,vsub_dd_mst.ac_no);
			ret=sql_sum_double("sub_dd_mst","bal",&T_sum," ac_no='%s' and ac_sts='1' ",vsub_dd_mst.ac_no);
			ERR_DEAL
			ret=sql_sum_double("sub_dd_mst","ys_bal",&T_ys_sum," ac_no='%s' and ac_sts='1'",vsub_dd_mst.ac_no);
			ERR_DEAL
			ret=Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s' ",vsub_dd_mst.ac_no);
			ERR_DEAL
			ret=Dd_mst_Sel(g_pub_tx.reply,&g_dd_mst," ac_id=%ld and ac_seqn=1 ",g_mdm_ac_rel.ac_id);
			ERR_DEAL
			fprintf( fp, "%s|%s||%.2lf|%.2lf||%ld|%ld|%ld|%ld|%ld|%s|0|\n",vsub_dd_mst.br_no,vsub_dd_mst.ac_no,T_sum,T_ys_sum,g_dd_mst.opn_date,g_dd_mst.ic_date,g_dd_mst.lst_date,g_dd_mst.hst_cnt,g_dd_mst.hst_pg,g_dd_mst.name);
		}

		fprintf( fp, "%s|%s|%s|%.2lf|%.2lf|%.2lf|%ld|%ld|%ld|%ld|%ld|%s|%s|\n",vsub_dd_mst.br_no,vsub_dd_mst.ac_no,vsub_dd_mst.sub_ac_no,vsub_dd_mst.bal,vsub_dd_mst.ys_bal,vsub_dd_mst.intst_acm,vsub_dd_mst.opn_date,vsub_dd_mst.ic_date,vsub_dd_mst.lst_date,vsub_dd_mst.hst_cnt,vsub_dd_mst.hst_pg,vsub_dd_mst.name,vsub_dd_mst.ac_sts);
		ttlnum++;
	}

	ret=Sub_dd_mst_Clo_Sel( );
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
