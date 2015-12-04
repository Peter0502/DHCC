/*************************************************
* �� �� ��:  sp8136.c
* ���������� �����˻���ϸ��ѯ
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
#include "dd_mst_hst_c.h"

int sp8136()
{
	struct dd_mst_hst_c vdd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hst;
	struct dd_mst_hst_c f_dd_mst_hsta;
	
	char add_ind[3];
	char ct_ind[5];
	char ac_no3[25];
	char f_note_type9[41];
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
	double j_tx_amt=0.00;
	double d_tx_amt=0.00;
	FILE *fp;
	char tmp_date[9];
	char tmp_time[7];

	memset(tmp_date,0x00,sizeof(tmp_date));
	memset(tmp_time,0x00,sizeof(tmp_time));
	memset( &g_pub_tx,0,sizeof(g_pub_tx) );
	memset( wherelist,0,sizeof(wherelist) );
	memset( fieldlist,0,sizeof(fieldlist) );
	memset( tablelist,0,sizeof(tablelist) );
	memset( &vdd_mst_hst,0,sizeof(vdd_mst_hst));
	memset( &f_dd_mst_hst,0,sizeof(f_dd_mst_hst));
	memset( &f_dd_mst_hsta,0,sizeof(f_dd_mst_hsta));
	
	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	pub_base_sysinit();
	get_zd_data("0700",f_dd_mst_hst.add_ind );
	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_data("0380",f_acno0);
	pub_base_old_acno(f_acno0);
	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	get_zd_long("0510",&f_dd_mst_hst.ac_seqn );
	get_zd_data("0910",f_dd_mst_hst.opn_br_no );
	get_zd_data("0700",f_dd_mst_hst.add_ind );
	get_zd_data("0660",f_dd_mst_hst.ct_ind );
	get_zd_long("0290",&f_dd_mst_hst.tx_date );
	get_zd_long("0440",&f_dd_mst_hsta.tx_date );

	vtcp_log( "0700===f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
	
	/**У�鿨״̬**/
	pub_base_strpack( f_acno0 );
	ret = Mdm_ac_rel_Sel(g_pub_tx.reply,&g_mdm_ac_rel,"ac_no='%s'",f_acno0);
	if( ret==100 )
	{
		sprintf(acErrMsg,"�����ʺŶ��ձ����ڸü�¼!![%s]",f_acno0);
		WRITEMSG
		strcpy(g_pub_tx.reply,"M003");
		goto ErrExit;
	}
	else if( ret )
	{
		sprintf(acErrMsg,"�����ʺų���!!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D103");
		goto ErrExit;
	}

	if( g_mdm_ac_rel.note_sts[0]=='1' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P170");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='2' )
	{
		sprintf(acErrMsg,"�ý����ѹ�ʧ��֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P171");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='3' )
	{
		sprintf(acErrMsg,"�ý����ѱ���֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"M022");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='4' )
	{
		sprintf(acErrMsg,"�ý����Ѳ��ỻ֤!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"D215");
		goto ErrExit;
	}
	else if( g_mdm_ac_rel.note_sts[0]=='*' )
	{
		sprintf(acErrMsg,"�ý���������!");
		WRITEMSG
		strcpy(g_pub_tx.reply,"P172");
		goto ErrExit;
	}
	
	/**��ɲ�ѯ����**/
	if( strlen(f_acno0) )
	{
		if ( pub_base_acno_acid(&f_dd_mst_hst.ac_id,f_acno0) )
		{
			goto ErrExit;
		}
		sprintf( tmpstr," ac_id=%ld and",f_dd_mst_hst.ac_id );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hst.ac_seqn )
	{
		sprintf( tmpstr," ac_seqn=%d and",f_dd_mst_hst.ac_seqn );
		strcat( wherelist,tmpstr );
	}
	pub_base_strpack( f_dd_mst_hst.opn_br_no );
	pub_base_strpack( f_dd_mst_hst.add_ind );
	if( strlen(f_dd_mst_hst.add_ind) )
	{
		if( f_dd_mst_hst.add_ind[0]=='0' || f_dd_mst_hst.add_ind[0]=='1' )
		{
			vtcp_log( "f_dd_mst_hst.add_ind [%s]",f_dd_mst_hst.add_ind );
			sprintf( tmpstr," add_ind='%s' and",f_dd_mst_hst.add_ind );
			strcat( wherelist,tmpstr );
		}
	}
	pub_base_strpack( f_dd_mst_hst.ct_ind );
	if( strlen(f_dd_mst_hst.ct_ind) )
	{
		sprintf( tmpstr," ct_ind='%s' and",f_dd_mst_hst.ct_ind );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hst.tx_date )
	{
		sprintf( tmpstr," tx_date>=%ld and",f_dd_mst_hst.tx_date );
		strcat( wherelist,tmpstr );
	}
	if( f_dd_mst_hsta.tx_date )
	{
		sprintf( tmpstr," tx_date<=%ld and",f_dd_mst_hsta.tx_date );
		strcat( wherelist,tmpstr );
	}
	
	strcpy( tablelist,"dd_mst_hst" );
	
	strcat(wherelist," 1=1 ORDER BY tx_date desc,trace_no desc,trace_cnt desc");

	vtcp_log( "WHERE [%s]",wherelist );
	strcpy( fieldlist,"trace_no,trace_cnt,opn_br_no,ac_id,ac_seqn,add_ind,ct_ind,tx_amt,bal,note_type,note_no,tx_date,tx_time,brf,hst_cnt,tel,chk,auth");

	ret=Dd_mst_hst_Dec_Sel( g_pub_tx.reply,wherelist );
	ERR_DEAL
	
	ttlnum=0;
	
	while(1)
	{
		ret=Dd_mst_hst_Fet_Sel( &vdd_mst_hst, g_pub_tx.reply );
		if( ret==100 ) break;
		ERR_DEAL

		if( !ttlnum )
		{
			pub_base_AllDwnFilName( filename );
			vtcp_log( "%s,%d,FILENAME [%s]",__FILE__,__LINE__,filename );
			fp = fopen( filename,"w" );
			if( fp==NULL )
			{
				sprintf(acErrMsg," open file [%s] error ",filename );
				WRITEMSG
				strcpy( g_pub_tx.reply,"S047" );
				goto ErrExit;
			}
			
				/**��ʾ�б���**/
			fprintf( fp,"~@��������|@���|$���׽��|@��ת|$�������|@ժҪ|@ƾ֤����|@ƾ֤����|@����Ա|����ʱ��|���|������ˮ��|\n" );
		}
		
		if(vdd_mst_hst.trace_no<-2147000000) vdd_mst_hst.trace_no=0;
		if(vdd_mst_hst.trace_cnt<-2147000000) vdd_mst_hst.trace_cnt=0;
		pub_base_acid_acno( vdd_mst_hst.ac_id,vdd_mst_hst.ac_seqn,ac_no3 );
		if(vdd_mst_hst.tx_amt<-2147000000) vdd_mst_hst.tx_amt=0.0;
		if(vdd_mst_hst.bal<-2147000000) vdd_mst_hst.bal=0.0;
		pub_base_dic_show_str(f_note_type9,"note_type",vdd_mst_hst.note_type);
		if(vdd_mst_hst.tx_date<-2147000000) vdd_mst_hst.tx_date=0;
		if(vdd_mst_hst.tx_time<-2147000000) vdd_mst_hst.tx_time=0;
		if(vdd_mst_hst.hst_cnt<-2147000000) vdd_mst_hst.hst_cnt=0;

		if(vdd_mst_hst.add_ind[0]=='0')		 
		{
			strcpy(add_ind,"��");
			j_tx_amt=j_tx_amt+vdd_mst_hst.tx_amt;
	}
		
		else if(vdd_mst_hst.add_ind[0]=='1') 
		{
			strcpy(add_ind,"��");
			d_tx_amt=d_tx_amt+vdd_mst_hst.tx_amt;
		}
		pub_base_dic_show_str(ct_ind,"ct_ind",vdd_mst_hst.ct_ind);
		sprintf(tmp_date,"%ld",vdd_mst_hst.tx_date);
		sprintf(tmp_time,"%d",vdd_mst_hst.tx_time);
		
		fprintf(fp, "%s|%s|%.2lf|%s|%.2lf|%s|%s|%s|%s|%d|%d|%ld|\n",tmp_date,add_ind,vdd_mst_hst.tx_amt,ct_ind,vdd_mst_hst.bal,vdd_mst_hst.brf,f_note_type9,vdd_mst_hst.note_no,vdd_mst_hst.tel,vdd_mst_hst.tx_time,vdd_mst_hst.ac_seqn,vdd_mst_hst.trace_no);
		
		ttlnum++;
	}
	
	ret=Dd_mst_hst_Clo_Sel( );
	ERR_DEAL

	if( !ttlnum )
	{
		strcpy( g_pub_tx.reply,"S049" );
		goto ErrExit;
	}
	else
	{
		fprintf( fp,"�跽�ϼ�:|");
		fprintf( fp,"%.2lf||",j_tx_amt);
		fprintf( fp,"�����ϼ�:|");
		fprintf( fp,"%.2lf|\n",d_tx_amt);
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
